/*
 * Copyright (C) 2012-2013 Daniel Schwierzeck, daniel.schwierzeck@gmail.com
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <linux/mtd/nand.h>
#include <linux/compiler.h>
#include <asm/arch/soc.h>
#include <asm/arch/nand.h>
#include <asm/lantiq/io.h>

#define NAND_CON_ECC_ON		(1 << 31)
#define NAND_CON_LATCH_PRE	(1 << 23)
#define NAND_CON_LATCH_WP	(1 << 22)
#define NAND_CON_LATCH_SE	(1 << 21)
#define NAND_CON_LATCH_CS	(1 << 20)
#define NAND_CON_LATCH_CLE	(1 << 19)
#define NAND_CON_LATCH_ALE	(1 << 18)
#define NAND_CON_OUT_CS1	(1 << 10)
#define NAND_CON_IN_CS1		(1 << 8)
#define NAND_CON_PRE_P		(1 << 7)
#define NAND_CON_WP_P		(1 << 6)
#define NAND_CON_SE_P		(1 << 5)
#define NAND_CON_CS_P		(1 << 4)
#define NAND_CON_CLE_P		(1 << 3)
#define NAND_CON_ALE_P		(1 << 2)
#define NAND_CON_CSMUX		(1 << 1)
#define NAND_CON_NANDM		(1 << 0)

#define NAND_WAIT_WR_C		(1 << 3)
#define NAND_WAIT_RDBY		(1 << 0)

#define NAND_CMD_ALE		(1 << 2)
#define NAND_CMD_CLE		(1 << 3)
#define NAND_CMD_CS		(1 << 4)
#define NAND_CMD_SE		(1 << 5)
#define NAND_CMD_WP		(1 << 6)
#define NAND_CMD_PRE		(1 << 7)

struct ltq_nand_regs {
	__be32	con;		/* NAND controller control */
	__be32	wait;		/* NAND Flash Device RD/BY State */
	__be32	ecc0;		/* NAND Flash ECC Register 0 */
	__be32	ecc_ac;		/* NAND Flash ECC Register address counter */
	__be32	ecc_cr;		/* NAND Flash ECC Comparison */
};

static struct ltq_nand_regs *ltq_nand_regs =
	(struct ltq_nand_regs *) CKSEG1ADDR(LTQ_EBU_NAND_BASE);

static void ltq_nand_wait_ready(void)
{
	while ((ltq_readl(&ltq_nand_regs->wait) & NAND_WAIT_WR_C) == 0)
		;
}

static int ltq_nand_dev_ready(struct mtd_info *mtd)
{
	u32 data = ltq_readl(&ltq_nand_regs->wait);
	return data & NAND_WAIT_RDBY;
}

static void ltq_nand_select_chip(struct mtd_info *mtd, int chip)
{
	if (chip == 0) {
		ltq_setbits(&ltq_nand_regs->con, NAND_CON_NANDM);
		ltq_setbits(&ltq_nand_regs->con, NAND_CON_LATCH_CS);
	} else {
		ltq_clrbits(&ltq_nand_regs->con, NAND_CON_LATCH_CS);
		ltq_clrbits(&ltq_nand_regs->con, NAND_CON_NANDM);
	}
}

static void ltq_nand_cmd_ctrl(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	struct nand_chip *chip = mtd->priv;
	unsigned long addr = (unsigned long) chip->IO_ADDR_W;

	if (ctrl & NAND_CTRL_CHANGE) {
		if (ctrl & NAND_ALE)
			addr |= NAND_CMD_ALE;
		else
			addr &= ~NAND_CMD_ALE;

		if (ctrl & NAND_CLE)
			addr |= NAND_CMD_CLE;
		else
			addr &= ~NAND_CMD_CLE;

		chip->IO_ADDR_W = (void __iomem *) addr;
	}

	if (cmd != NAND_CMD_NONE) {
		writeb(cmd, chip->IO_ADDR_W);
		ltq_nand_wait_ready();
	}
}

int ltq_nand_init(struct nand_chip *nand)
{
	/* Enable NAND, set NAND CS to EBU CS1, enable EBU CS mux */
	ltq_writel(&ltq_nand_regs->con, NAND_CON_OUT_CS1 | NAND_CON_IN_CS1 |
		NAND_CON_PRE_P | NAND_CON_WP_P | NAND_CON_SE_P |
		NAND_CON_CS_P | NAND_CON_CSMUX);

	nand->dev_ready = ltq_nand_dev_ready;
	nand->select_chip = ltq_nand_select_chip;
	nand->cmd_ctrl = ltq_nand_cmd_ctrl;

	nand->chip_delay = 30;
	nand->options = 0;
	nand->ecc.mode = NAND_ECC_SOFT;

	/* Enable CS bit in address offset */
	nand->IO_ADDR_R = nand->IO_ADDR_R + NAND_CMD_CS;
	nand->IO_ADDR_W = nand->IO_ADDR_W + NAND_CMD_CS;

	return 0;
}

__weak int board_nand_init(struct nand_chip *chip)
{
	return ltq_nand_init(chip);
}

#if defined(CONFIG_SYS_BOOT_NANDSPL) && defined(CONFIG_SPL_BUILD)
static noinline void spl_nand_write(unsigned int cmd, unsigned int addr)
{
	unsigned long base_addr = CONFIG_SYS_NAND_BASE + NAND_CMD_CS;
	void __iomem *io_addr = (void __iomem *)(base_addr + addr);
	ltq_writeb(io_addr, cmd);
	ltq_nand_wait_ready();
}

static noinline void spl_nand_command(int block, int page, unsigned int cmd)
{
	int page_addr = page + block * CONFIG_SYS_NAND_PAGE_COUNT;

	/* command latch cycle */
	spl_nand_write(cmd, NAND_CMD_CLE);

	/* column address */
	spl_nand_write(0, NAND_CMD_ALE);
#if (CONFIG_SYS_NAND_PAGE_SIZE > 512)
	spl_nand_write(0, NAND_CMD_ALE);
#endif

	/* row address */
	spl_nand_write(page_addr & 0xff, NAND_CMD_ALE);
	spl_nand_write((page_addr >> 8) & 0xff, NAND_CMD_ALE);

#if (CONFIG_SYS_NAND_PAGE_SIZE <= 512) && defined(CONFIG_SYS_NAND_4_ADDR_CYCLE)
	/* One more address cycle for devices > 32MiB */
	spl_nand_write((page_addr >> 16) & 0x0f, NAND_CMD_ALE);
#endif
#if (CONFIG_SYS_NAND_PAGE_SIZE > 512) && defined(CONFIG_SYS_NAND_5_ADDR_CYCLE)
	/* One more address cycle for devices > 128MiB */
	spl_nand_write((page_addr >> 16) & 0x0f, NAND_CMD_ALE);
#endif

#if (CONFIG_SYS_NAND_PAGE_SIZE > 512)
	spl_nand_write(NAND_CMD_READSTART, NAND_CMD_CLE);
#endif

	while ((ltq_readl(&ltq_nand_regs->wait) & NAND_WAIT_RDBY) != NAND_WAIT_RDBY)
		;
}

static noinline void spl_nand_read_buf(u8 *buf, unsigned int len)
{
	void __iomem *io_addr = (void __iomem *)(CONFIG_SYS_NAND_BASE + NAND_CMD_CS);
	unsigned int i;

	for (i = 0; i < len; i++)
		buf[i] = ltq_readb(io_addr);
}

static noinline int spl_nand_read_page(int block, int page, void *dst)
{
	u8 *p = dst;

	spl_nand_command(block, page, NAND_CMD_READ0);
	spl_nand_read_buf(p, CONFIG_SYS_NAND_PAGE_SIZE);

	return 0;
}

int nand_spl_load_image(uint32_t offs, unsigned int size, void *dst)
{
	unsigned int block = offs / CONFIG_SYS_NAND_BLOCK_SIZE;
	unsigned int lastblock = (offs + size - 1) / CONFIG_SYS_NAND_BLOCK_SIZE;
	unsigned int page = (offs % CONFIG_SYS_NAND_BLOCK_SIZE) /
					CONFIG_SYS_NAND_PAGE_SIZE;
	unsigned int cnt = 0;

	while (block <= lastblock) {
		while (page < CONFIG_SYS_NAND_PAGE_COUNT) {
			spl_nand_read_page(block, page, dst);
			dst += CONFIG_SYS_NAND_PAGE_SIZE;
			page++;

			cnt += CONFIG_SYS_NAND_PAGE_SIZE;
			if (cnt >= size)
				return 0;
		}
		page = 0;
		block++;
	}

	return 0;
}

void nand_init(void)
{
	ltq_writel(&ltq_nand_regs->con, NAND_CON_OUT_CS1 | NAND_CON_IN_CS1 |
		NAND_CON_PRE_P | NAND_CON_WP_P | NAND_CON_SE_P |
		NAND_CON_CS_P | NAND_CON_CSMUX);
}
#endif
