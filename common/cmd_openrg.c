#define DEBUG_CMD_ORG

#include <config.h>
#include <common.h>
#include <command.h>
#include <image.h>
#include <malloc.h>
#include <asm/byteorder.h>
#if defined(CONFIG_CMD_NAND)
#include <nand.h>
#endif
#define _RG_DEF_H_ /* Hack to avoid including rg_def.h */
#include <vendor/c/flash_layout.c>
#define FLASH_ADDR(off) (CFG_FLASH_BASE + (off))
#ifdef CONFIG_RG_POLARSSL
#include <polarssl/sha4.h>
#include <polarssl/rsa.h>
#include <polarssl/aes.h>
#include <polarssl/x509.h>
#endif

#ifdef CONFIG_BOOTLDR_UBOOT_SIGNED_IMAGES
#include <deu_aes.h>
#include <rmt-update/rmt_upd_p.h>
#endif

flash_section_layout_t *flash_layout_get(void)
{
    return flash_layout;
}

int flash_layout_section_count(void)
{
    return flash_section_count;
}

void chksum_update(u32 *sum, u8 *buf, u32 size)
{
    register u32 chksum = 0;
    register u32 tmp;

    /* The buffer may begin at an unaligned address, so process the unaligned
     * part one byte at a time */
    while (((u32)buf & 3) && size)
    {
	chksum += *(buf++);
	size--;
    }

    /* By the time we're here, 'buf' is 32-bit aligned so we can start reading 
     * from memory 32-bits at a time, thus minimizing access to memory */
    while (size >= 4)
    {
	tmp = *((u32 *)buf);
	chksum += tmp & 0xff;
	tmp >>= 8;
	chksum += tmp & 0xff;
	tmp >>= 8;
	chksum += tmp & 0xff;
	tmp >>= 8;
	chksum += tmp;
	
	size -= 4;
	buf += 4;
    }

    /* Process unaligned leftovers one byte at a time */
    while (size--)
	chksum += *(buf++);

    *sum += chksum;
}

static void openrg_env_set(const u_char *openrg_start, int openrg_size)
{
    char buf[32];

    sprintf(buf, "%#x", openrg_start);
    setenv("openrg_start", buf);
    sprintf(buf, "%#x", openrg_size);
    setenv("openrg_size", buf);
}

#ifdef CONFIG_BOOTLDR_UBOOT_SIGNED_IMAGES
static x509_cert *cacerts;

#define CERT_PREFIX "cert"
static int init_certificates(void)
{
    char *env_cert;

    env_cert = getenv(CERT_PREFIX);
    if (!env_cert)
	return 0;

    if (!(cacerts = malloc(sizeof(x509_cert))))
    {
	printf("Failed\n Can't allocate memory for certificate!\n");
	return 0;
    }
    memset(cacerts, 0, sizeof(x509_cert));
    if (x509parse_crt(cacerts, (unsigned char *)env_cert, strlen(env_cert)))
    {
	printf("Failed\n Cannot parse certificate!\n");
	return 0;
    }

    return 1;
}

/* Same as pkg/util/strconv.c */
static int hex_2_bin(u8 *bin, int binlen, char *hex)
{
    char tmp[3];
    int len = 0;

    tmp[2] = 0;
    while (*hex && *(hex+1) && binlen--)
    {
	tmp[0] = *hex++;
	tmp[1] = *hex++;
	*bin++ = (u8)simple_strtoul(tmp, NULL, 16);
	len++;
    }
    return len;
}

static int image_decrypt(rmt_upd_header_t *header, u_char *buf)
{
    u8 iv[16] = {};
#include "symkey.c"

    if (header->enc_type == RMT_UPD_ENC_NONE)
	return 0;

    secure_decrypt(sym_key, iv, buf, buf, ntohl(header->image_size));

    return 0;
}

#ifdef CONFIG_RG_POLARSSL

/* Verify signature of section
 * return the address of the actual image, NULL on error */
static u_char *section_sign_verify(const u_char *image, int image_size)
{
    u_char hash[64];
    u_char head_sig[RMT_UPD_SIG_SIZE];
    u_char img_sig[RMT_UPD_SIG_SIZE];
    u_char *end;
    rmt_upd_header_t *rmt_header = (rmt_upd_header_t *)image;
    x509_cert *cert;
    int match;
    
    /* Save the signatures, and zero the signatures on the header */
    memcpy(head_sig, rmt_header->headers_sig, RMT_UPD_SIG_SIZE);
    memcpy(img_sig, rmt_header->headers_n_data_sig, RMT_UPD_SIG_SIZE);
    memset(rmt_header->headers_sig, 0, RMT_UPD_SIG_SIZE);
    memset(rmt_header->headers_n_data_sig, 0, RMT_UPD_SIG_SIZE);

    end = memchr(image + sizeof(*rmt_header), 0, sizeof(image) -
	sizeof(*rmt_header));
    end++;

    /* Verify header signature first */
    sha4((u8*)image, end - image, hash, 0);
    match = 0;
    for (cert = cacerts; cert; cert = cert->next)
    {
	if (!rsa_pkcs1_verify(&cert->rsa, RSA_PUBLIC, SIG_RSA_SHA512, 64,
	    hash, head_sig))
	{
	    match = 1;
	    break;
	}
    }
    if (!match) 
    {
        printf( "failed\n  Header signature does not match!\n");
	return NULL;
    }

    sha4((u8*)image, image_size, hash, 0);
    match = 0;
    for (cert = cacerts; cert; cert = cert->next)
    {
	if (!rsa_pkcs1_verify(&cert->rsa, RSA_PUBLIC, SIG_RSA_SHA512, 64,
	    hash, img_sig))
	{
	    match = 1;
	    break;
	}
    }
    if (!match)
    {
        printf( "failed\n  Image signature does not match!\n");
	return NULL;
    }

    memcpy(rmt_header->headers_sig, head_sig, RMT_UPD_SIG_SIZE);
    memcpy(rmt_header->headers_n_data_sig, img_sig, RMT_UPD_SIG_SIZE);

    /* Skip NULL padding before the actual image */
    while (!*end)
	end++;

    if (image_decrypt(rmt_header, end))
	end = NULL;

    return end;
}

static u_char *check_section(flash_section_layout_t *sec,
    flash_section_header_t *header, const u_char *image)
{
    return section_sign_verify(image, header->size);
}

static int do_signverify(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    u_char *image;
    int image_size;

    if (argc < 3)
    {
	printf("signverify: signverify <0x-address> <0x-size>\n");
	return -1;
    }

    if (!init_certificates())
    {
	printf("signverify: Failed to init certificates\n");
	return 0;
    }

    image = (u_char *)simple_strtoul(argv[1], NULL, 16);
    image_size = simple_strtoul(argv[2], NULL, 16);
    if (!(image = section_sign_verify(image, image_size)))
    {
	printf("signverify: Failed to verify image\n");
	return -1;
    }

    openrg_env_set(image, image_size);
    return 0;
}
#endif

#else /* No RSA */
/* section validity: check magic, calculate and compare checksum.
 * section header is given in 'header', image data is given in 'image'
 * returns the address to run from, NULL on error
 */
static u_char *check_section(flash_section_layout_t *sec,
    flash_section_header_t *header, const u_char *image)
{
    u32 section_chksum = 0, chksum = 0;

    if (!HAS_HEADER(sec))
	return image;

    if (header->magic != FLASH_SECTION_MAGIC)
	return NULL;

    /* Update check sum for the header itself */
    section_chksum = header->chksum;
    header->chksum = 0;
    chksum_update(&chksum, (u8 *)header, sizeof(*header));
    /* Update check sum for the data in section */
    chksum_update(&chksum, (u8 *)image, header->size);

    header->chksum = section_chksum;

    return (chksum == section_chksum) ? image : NULL;
}
#endif

/* XXX should be arch specific, depending on available SDRAM size, u-boot
 * RAM location, expected image size, and expected location and length of
 * uncompressed openrg kernel.
 */
#ifdef CONFIG_VR9
#define RAM_TEMP_BASE 0x81000000
#else 
#define RAM_TEMP_BASE 0x80C00000
#endif

/* read 'len' bytes at the 'offset' from beginning of flash section.
 * will correctly skip bad-blocks if it is nand section.
 * 
 * returns pointer to the read data, NULL on read failure.
 *
 * CAUTION:
 * the returned data pointer is:
 *   (NOR)  the memory mapped address of the requested data WITHIN the NOR
 *   (NAND) a temporary RAM storage were the data has been copied into
 * HENCE:
 *   the returned data should be treated as read-only;
 *   the returned data should be considered temporary (i.e. consecutive
 *   calls to 'section_read' might overwrite previously returned data)
 */
#if defined(CONFIG_CMD_UBI)
/* XXX B124976: This code doesn't allow to handle sections 
   with different flash types  */
static const u8 *section_read(flash_section_layout_t *sec, u32 offset, u32 len)
{
    int read;

    if (offset + len > sec->size)
	return NULL;

    read = ubi_volume_read_offset("OpenRG", (void *)RAM_TEMP_BASE,
	sec->offset + offset, len);
    if (read < 0)
    {
	printf("ubi_volume_read failed; read %d\n", read);
	return NULL;
    }

    return RAM_TEMP_BASE;
}
#else /* NOR and raw NAND support */
static const u8 *section_read(flash_section_layout_t *sec, u32 offset, u32 len)
{
#if defined(CONFIG_CMD_NAND)
    nand_read_options_t opts = {};
#endif
    u8 *ret = NULL;

    if (offset + len > sec->size)
	return NULL;

    if (sec->s.type != FLASH_SEC_STRUCT_NAND)
	return (u8 *)FLASH_ADDR(sec->offset + offset);

#if defined(CONFIG_CMD_NAND)
    opts.buffer = (uchar *)RAM_TEMP_BASE + offset;
    opts.length = len;
    opts.offset = sec->offset + offset;
#if defined(DEBUG_CMD_ORG)
    opts.quiet      = 0;
#else
    opts.quiet      = 1;
#endif
    if (nand_read_opts(&nand_info[nand_curr_device], &opts))
	return NULL;
    ret = opts.buffer;
#endif

    return ret;
}
#endif

#define MAX_SECTIONS 16

typedef struct sec_hdr_list_t {
    int sec_num;
    flash_section_header_t hdr;
    struct sec_hdr_list_t *next;
} sec_hdr_list_t;

static flash_section_header_t *flash_swap_header(flash_section_header_t *hdr)
{
    hdr->magic = flash_swap_32(hdr->magic);
    hdr->size = flash_swap_32(hdr->size);
    hdr->chksum = flash_swap_32(hdr->chksum);
    hdr->counter = flash_swap_32(hdr->counter);
    hdr->start_offset = flash_swap_32(hdr->start_offset);

    return hdr;
}

static sec_hdr_list_t *find_image_order(void)
{
    int i; 
    static sec_hdr_list_t img_list[MAX_SECTIONS]; /* preallocated list items */
    sec_hdr_list_t *img_list_head = NULL;
    sec_hdr_list_t **iter;
    u32 counter[MAX_SECTIONS] = {};

    /* read all image section counters */
    for (i = 0; i < flash_layout_section_count(); i++)
    {
	const flash_section_header_t *hdr;
	flash_section_layout_t *sec = &flash_layout_get()[i];
	printf("  %d. section: type:%d ", i, sec->type);

	if (sec->type != FLASH_SECT_IMAGE)
	{
	    printf("not an image\n");
	    continue;
	}
	
	printf("image. reading section header @%#x...\n",
	    sec->offset + HEADER_OFFSET(sec));
	hdr = (const flash_section_header_t *)section_read(sec,
	    HEADER_OFFSET(sec), HEADER_SIZE(sec));
	if (!hdr)
	{
	    printf("header read failed!\n");
	    continue;
	}
	memcpy(&img_list[i].hdr, hdr, sizeof(flash_section_header_t));
	hdr = flash_swap_header(&img_list[i].hdr);

	if (hdr->magic != FLASH_SECTION_MAGIC)
	{
	    printf("bad header magic!\n");
	    continue;
	}
	if (hdr->size > DATA_AVAILABLE_SIZE(sec))
	{
	    printf("bad header, image size too long!\n");
	    continue;
	}
	printf("     counter:%#x\n", hdr->counter);
	counter[i] = hdr->counter;

	/* valid image header. initialize the appropriate list item */
	img_list[i].sec_num = i;
	img_list[i].next = NULL;
    }

    iter = &img_list_head;
    /* calculate images order */
    while (1)
    {
	u32 run_max = 0;
	int run_max_idx = -1;

	/* find largest counter and its index */
	for (i = 0; i < flash_layout_section_count(); i++)
	{
	    if (!counter[i])
		continue;
	    if (counter[i] > run_max)
	    {
		run_max = counter[i];
		run_max_idx = i;
	    }
	}
	/* none found? we're done */
	if (run_max_idx == -1)
	    break;
	/* update sctions order list with current run's largest counter index */
	*iter = &img_list[run_max_idx];
	iter = &(*iter)->next;
	/* zero current run max counter, so it won't be caught next run */
	counter[run_max_idx] = 0;
    }
    return img_list_head;
}

static int find_section(void)
{
    int size = 0;
    const u_char *active_image = NULL;
    sec_hdr_list_t *img_order, *iter;

#ifdef CONFIG_BOOTLDR_UBOOT_SIGNED_IMAGES
    if (!init_certificates())
    {
	show_boot_progress(-1);
	return 0;
    }
#endif

    printf("\nLooking for active section/image:\n");

    img_order = find_image_order();

    printf("\nVerifying image(s):\n");
    
    for (iter = img_order; iter; iter = iter->next)
    {
	const u_char *image; /* signed image (sec headers not included) */
	const u_char *images_start; /* actual image data */
	flash_section_layout_t *sec = &flash_layout_get()[iter->sec_num];
	flash_section_header_t *header = &iter->hdr;

	printf("  %d. section: image. reading image content %#x@%#x...\n",
	    iter->sec_num, header->size, sec->offset + DATA_OFFSET(sec));

	if (!(image = section_read(sec, DATA_OFFSET(sec), header->size)))
	{
	    printf("read error\n");
	    continue;
	}
	
	printf("     checking ... ");

	if (!(images_start = check_section(sec, header, image)))
	{
	    printf("corrupted\n");
	    continue;
	}

	printf("ok: '%s' %#x@%#x count:%#x\n", header->name,
	    header->size, image, header->counter);
	
	size = header->size;
	active_image = images_start;
	break;
    }

    if (!active_image)
    {
    	printf("No active image found!!!\n");
	show_boot_progress(-1);
    	return -1;
    }

    printf("\nActive image: %#x.\n", active_image);
    openrg_env_set(active_image, size);
    return 0;
}

static int do_dualimage(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    return find_section();
}

U_BOOT_CMD(
    dualimage, 2, 0, do_dualimage,
    "dualimage - sets openrg_start and openrg_size according to "
    "the current active image.\n",
);

#if defined(CONFIG_RG_POLARSSL)
U_BOOT_CMD(
    signverify, 3, 0, do_signverify,
    "signverify - verify the active image signature and decryption if needed.\n",
);
#endif
