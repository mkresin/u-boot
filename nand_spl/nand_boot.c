/*
 * (C) Copyright 2006-2008
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <nand.h>
#include <asm/io.h>
#include <configs/rg_config.h>

static int bbt_valid=0;

#define CONFIG_SYS_NAND_READ_DELAY \
	{ volatile int dummy; int i; for (i=0; i<10000; i++) dummy = i; }

static int nand_ecc_pos[] = CONFIG_SYS_NAND_ECCPOS;

#if (CONFIG_SYS_NAND_PAGE_SIZE <= 512)
/*
 * NAND command for small page NAND devices (512)
 */
static int nand_command(struct mtd_info *mtd, int block, int page, int offs, u8 cmd)
{
	struct nand_chip *this = mtd->priv;
	int page_addr = page + block * CONFIG_SYS_NAND_PAGE_COUNT;

	if (this->dev_ready)
		while (!this->dev_ready(mtd))
			;
	else
		CONFIG_SYS_NAND_READ_DELAY;

	/* Begin command latch cycle */
	this->cmd_ctrl(mtd, cmd, NAND_CTRL_CLE | NAND_CTRL_CHANGE);
	/* Set ALE and clear CLE to start address cycle */
	/* Column address */
	this->cmd_ctrl(mtd, offs, NAND_CTRL_ALE | NAND_CTRL_CHANGE);
	this->cmd_ctrl(mtd, page_addr & 0xff, NAND_CTRL_ALE); /* A[16:9] */
	this->cmd_ctrl(mtd, (page_addr >> 8) & 0xff,
		       NAND_CTRL_ALE); /* A[24:17] */
#ifdef CONFIG_SYS_NAND_4_ADDR_CYCLE
	/* One more address cycle for devices > 32MiB */
	this->cmd_ctrl(mtd, (page_addr >> 16) & 0x0f,
		       NAND_CTRL_ALE); /* A[28:25] */
#endif
	/* Latch in address */
	this->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);

	/*
	 * Wait a while for the data to be ready
	 */
	if (this->dev_ready)
		while (!this->dev_ready(mtd))
			;
	else
		CONFIG_SYS_NAND_READ_DELAY;

	return 0;
}
#else
/*
 * NAND command for large page NAND devices (2k)
 */
static int nand_command(struct mtd_info *mtd, int block, int page, int offs, u8 cmd)
{
	struct nand_chip *this = mtd->priv;
	int page_addr = page + block * CONFIG_SYS_NAND_PAGE_COUNT;

	if (this->dev_ready)
		while (!this->dev_ready(mtd))
			;
	else
		CONFIG_SYS_NAND_READ_DELAY;

	/* Emulate NAND_CMD_READOOB */
	if (cmd == NAND_CMD_READOOB) {
		offs += CONFIG_SYS_NAND_PAGE_SIZE;
		cmd = NAND_CMD_READ0;
	}

	/* Begin command latch cycle */
	this->cmd_ctrl(mtd, cmd, NAND_CTRL_CLE | NAND_CTRL_CHANGE);
	/* Set ALE and clear CLE to start address cycle */
	/* Column address */
	this->cmd_ctrl(mtd, offs & 0xff,
		       NAND_CTRL_ALE | NAND_CTRL_CHANGE); /* A[7:0] */
	this->cmd_ctrl(mtd, (offs >> 8) & 0xff, NAND_CTRL_ALE); /* A[11:9] */
	/* Row address */
	this->cmd_ctrl(mtd, (page_addr & 0xff), NAND_CTRL_ALE); /* A[19:12] */
	this->cmd_ctrl(mtd, ((page_addr >> 8) & 0xff),
		       NAND_CTRL_ALE); /* A[27:20] */
#ifdef CONFIG_SYS_NAND_5_ADDR_CYCLE
	/* One more address cycle for devices > 128MiB */
	this->cmd_ctrl(mtd, (page_addr >> 16) & 0x0f,
		       NAND_CTRL_ALE); /* A[31:28] */
#endif
	/* Latch in address */
	this->cmd_ctrl(mtd, NAND_CMD_READSTART,
		       NAND_CTRL_CLE | NAND_CTRL_CHANGE);
	this->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);

	/*
	 * Wait a while for the data to be ready
	 */
	if (this->dev_ready)
		while (!this->dev_ready(mtd))
			;
	else
		CONFIG_SYS_NAND_READ_DELAY;

	return 0;
}
#endif

static int nand_is_bad_block(struct mtd_info *mtd, int block)
{
	struct nand_chip *this = mtd->priv;
#ifdef CONFIG_NAND_SPL_BBT	
	if(bbt_valid){
      return nand_isbad_bbt(mtd, block<<this->bbt_erase_shift, 1);
	}else{
#endif	
	 nand_command(mtd, block, 0, CONFIG_SYS_NAND_BAD_BLOCK_POS, NAND_CMD_READOOB);

	 /*
	 * Read one byte
	 */
	if (readb(this->IO_ADDR_R) != 0xff)
		return 1;
#ifdef CONFIG_NAND_SPL_BBT    
	}
#endif	
	return 0;
}



int nand_read_page(struct mtd_info *mtd, int block, int page, uchar *dst)
{
	struct nand_chip *this = mtd->priv;
	u_char *ecc_calc;
	u_char *ecc_code;
	u_char *oob_data;
	int i;
	int eccsize = CONFIG_SYS_NAND_ECCSIZE;
	int eccbytes = CONFIG_SYS_NAND_ECCBYTES;
	int eccsteps = CONFIG_SYS_NAND_ECCSTEPS;
	uint8_t *p = dst;
	int stat;

	nand_command(mtd, block, page, 0, NAND_CMD_READ0);

	/* No malloc available for now, just use some temporary locations
	 * in SDRAM
	 */
	ecc_calc = (u_char *)(CONFIG_SYS_SDRAM_BASE + 0x10000);
	ecc_code = ecc_calc + 0x100;
	oob_data = ecc_calc + 0x200;

	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		this->ecc.hwctl(mtd, NAND_ECC_READ);
		this->read_buf(mtd, p, eccsize);
		this->ecc.calculate(mtd, p, &ecc_calc[i]);
	}
	this->read_buf(mtd, oob_data, CONFIG_SYS_NAND_OOBSIZE);

	/* Pick the ECC bytes out of the oob data */
	for (i = 0; i < CONFIG_SYS_NAND_ECCTOTAL; i++)
		ecc_code[i] = oob_data[nand_ecc_pos[i]];

	eccsteps = CONFIG_SYS_NAND_ECCSTEPS;
	p = dst;

	for (i = 0 ; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		/* No chance to do something with the possible error message
		 * from correct_data(). We just hope that all possible errors
		 * are corrected by this routine.
		 */
		stat = this->ecc.correct(mtd, p, &ecc_code[i], &ecc_calc[i]);
	}

	return 0;
}

#ifdef CONFIG_NAND_SPL_BBT
int scan_read_raw(struct mtd_info *mtd, uint8_t *buf, loff_t offs, size_t len)
{
	 struct nand_chip *this = mtd->priv;
	 int block,page;
	 block=offs>>this->bbt_erase_shift;
	 page=0;/*always the first page for bbt*/
	
	 nand_command(mtd, block, page, 0, NAND_CMD_READ0);
	 this->read_buf(mtd, buf, mtd->writesize+mtd->oobsize);
}			 
#endif

static int nand_load(struct mtd_info *mtd, unsigned int offs,
		     unsigned int uboot_size, uchar *dst)
{
	unsigned int block, lastblock;
	unsigned int page;

	/*
	 * offs has to be aligned to a page address!
	 */
	block = offs / CONFIG_SYS_NAND_BLOCK_SIZE;
	lastblock = (offs + uboot_size - 1) / CONFIG_SYS_NAND_BLOCK_SIZE;
	page = (offs % CONFIG_SYS_NAND_BLOCK_SIZE) / CONFIG_SYS_NAND_PAGE_SIZE;

	while (block <= lastblock) {
				if (!nand_is_bad_block(mtd,block)){
			/*
			 * Skip bad blocks
			 */
			while (page < CONFIG_SYS_NAND_PAGE_COUNT) {
				nand_read_page(mtd, block, page, dst);
				dst += CONFIG_SYS_NAND_PAGE_SIZE;
				page++;
			}

			page = 0;
		} else {
			lastblock++;
		}

		block++;
	}

	return 0;
}

/*
 * The main entry for NAND booting. It's necessary that SDRAM is already
 * configured and available since this code loads the main U-Boot image
 * from NAND into SDRAM and starts it from there.
 */
void nand_boot(void)
{
	struct nand_chip nand_chip;
	nand_info_t nand_info;
	int ret;
	__attribute__((noreturn)) void (*uboot)(void);

	/*
	 * Init board specific nand support
	 */
	nand_info.priv = &nand_chip;
	
	nand_chip.IO_ADDR_R = nand_chip.IO_ADDR_W = (void  __iomem *)CONFIG_SYS_NAND_BASE;
	nand_chip.dev_ready = NULL;	/* preset to NULL */
	board_nand_init(&nand_chip);

	if (nand_chip.select_chip)
		nand_chip.select_chip(&nand_info, 0);
#ifdef CONFIG_NAND_SPL_BBT
   /*search and setup bbt in the memory*/

    nand_info.size=CONFIG_NAND_FLASH_SIZE*(1<<20);
    nand_info.erasesize=CONFIG_NAND_BLOCK_SIZE;
    nand_info.writesize=CONFIG_NAND_PAGE_SIZE;
    nand_info.oobsize=64;
    nand_chip.page_shift=ffs(nand_info.writesize)-1;
    nand_chip.bbt_erase_shift=ffs(nand_info.erasesize)-1;
    nand_chip.numchips=1;
    nand_chip.bbt_td=NULL;
    nand_chip.bbt_md=NULL;
    nand_chip.chipsize=nand_info.size;
    nand_chip.badblock_pattern=NULL;

    if(nand_default_bbt(&nand_info)){
    		bbt_valid=1;
    }else{
    	  bbt_valid=0;
    }
#endif
    /*
	 * Load U-Boot image from NAND into RAM
	 */
	ret = nand_load(&nand_info, CONFIG_SYS_NAND_U_BOOT_OFFS, CONFIG_SYS_NAND_U_BOOT_SIZE,
			(uchar *)CONFIG_SYS_NAND_U_BOOT_DST);

#ifdef CONFIG_NAND_ENV_DST
	nand_load(&nand_info, CONFIG_ENV_OFFSET, CONFIG_ENV_SIZE,
		  (uchar *)CONFIG_NAND_ENV_DST);

#ifdef CONFIG_BOOTLDR_UBOOT_SECURED_ENV
	{
#include "../common/symkey.c"
		u8 iv[16] = {};

		secure_decrypt(sym_key, iv,(u8*)CONFIG_NAND_ENV_DST,
	        	(u8*)CONFIG_NAND_ENV_DST, CONFIG_ENV_SIZE);
	}
#endif
#ifdef CONFIG_ENV_OFFSET_REDUND
	nand_load(&nand_info, CONFIG_ENV_OFFSET_REDUND, CONFIG_ENV_SIZE,
		  (uchar *)CONFIG_NAND_ENV_DST + CONFIG_ENV_SIZE);
#endif
#endif

	if (nand_chip.select_chip)
		nand_chip.select_chip(&nand_info, -1);

       asm("sync");

#ifdef CONFIG_LTQ_SECURE_BOOT
#include "aes_key.h"
extern secure_decrypt(u8 *key, u8 *iv, u8 *src, u8 *dst, u32 nbytes);
extern void flush_dcache_range(ulong start_addr, ulong stop);
	   secure_decrypt(aes_key, iv,(u8*)CONFIG_STAGE2_LOADADDR,\
	                (u8*)CONFIG_STAGE2_LOADADDR, CONFIG_STAGE2_SIZE);
	   flush_dcache_range((ulong)CONFIG_STAGE2_LOADADDR,\
	                      (ulong)(CONFIG_STAGE2_LOADADDR+CONFIG_STAGE2_SIZE));			
#endif

	/*
	 * Jump to U-Boot image
	 */
	uboot = (void *)CONFIG_SYS_NAND_U_BOOT_START;
	(*uboot)();
}
