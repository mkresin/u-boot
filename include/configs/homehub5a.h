/*
 * Copyright (C) 2015 Martin Blumenstingl <martin.blumenstingl@googlemail.com>
 * Copyright (C) 2016 Mathias Kresin <dev@kresin.me>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_IDENT_STRING	" BTHOMEHUB5A"
#define CONFIG_BOARD_NAME	"BT Home Hub 5 Type A"

/* Configure SoC */
#define CONFIG_LTQ_SUPPORT_UART			/* Enable ASC and UART */

#define CONFIG_LTQ_SUPPORT_ETHERNET		/* Enable ethernet */
#define CONFIG_FW_VRX200_PHY11G_A2X

#define CONFIG_LTQ_SUPPORT_NAND_FLASH		/* Have a ML01G100BHI00 NAND flash */
#define CONFIG_LTQ_NAND_CS1
#define CONFIG_SYS_NAND_PAGE_COUNT	64
#define CONFIG_SYS_NAND_PAGE_SIZE	2048
#define CONFIG_SYS_NAND_OOBSIZE		64
#define CONFIG_SYS_NAND_BLOCK_SIZE	(128 * 1024)
#define CONFIG_SYS_NAND_5_ADDR_CYCLE
#define CONFIG_SYS_NAND_USE_FLASH_BBT

#define CONFIG_LTQ_SUPPORT_SPL_NAND_FLASH	/* Build NAND flash SPL */
#define CONFIG_LTQ_SPL_COMP_LZO			/* Compress SPL with LZO */
#define CONFIG_LTQ_SPL_CONSOLE			/* Enable SPL console */
#define CONFIG_LTQ_SPL_MC_TUNE

#define CONFIG_SPL_TPL_OFFS		0x800
#define CONFIG_SPL_TPL_SIZE		0x5000
#define CONFIG_SPL_MC_TUNE_OFFS		0x5800
#define CONFIG_SPL_U_BOOT_OFFS		0x6000
#define CONFIG_SPL_U_BOOT_SIZE		0x9a000

/* MTD devices */
#define CONFIG_MTD_PARTITIONS
#define CONFIG_MTD_DEVICE
#define CONFIG_CMD_MTDPARTS
#define MTDIDS_DEFAULT			"nand0=nand-xway"
#define MTDPARTS_DEFAULT		"mtdparts=nand-xway:0x07e80000@0x100000(ubi)"

/* UBI */
#define CONFIG_RBTREE
#define CONFIG_CMD_UBI

/* 2nd stage bootloader */
#if defined(CONFIG_SYS_BOOT_2NDSTAGE)
#define CONFIG_SYS_TEXT_BASE		0x80002000
#define CONFIG_SKIP_LOWLEVEL_INIT
#endif

/* Environment */
#if defined(CONFIG_SYS_BOOT_NANDSPL)
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OVERWRITE
#define CONFIG_ENV_OFFSET		(5 * CONFIG_SYS_NAND_BLOCK_SIZE)
#define CONFIG_ENV_SECT_SIZE		CONFIG_SYS_NAND_BLOCK_SIZE

#define CONFIG_MISC_INIT_R
#else
#define CONFIG_ENV_IS_NOWHERE
#endif

#define CONFIG_ENV_SIZE			(8 * 1024)

/* Console */
#define CONFIG_LTQ_ADVANCED_CONSOLE
#define CONFIG_BAUDRATE			115200
#define CONFIG_CONSOLE_ASC		1

/* Boot */
#define CONFIG_ENV_MTDPARTS			\
	"mtdids="MTDIDS_DEFAULT"\0"		\
	"mtdparts="MTDPARTS_DEFAULT"\0"

#define CONFIG_BOOTCOMMAND \
	"ubi part ubi; ubi read ${loadaddr} kernel; bootm"

/* Pull in default board configs for Lantiq XWAY VRX200 */
#include <asm/lantiq/config.h>
#include <asm/arch/config.h>

/* Pull in default OpenWrt configs for Lantiq SoC */
#include "openwrt-lantiq-common.h"

#define CONFIG_EXTRA_ENV_SETTINGS	\
	CONFIG_ENV_LANTIQ_DEFAULTS \
	CONFIG_ENV_MTDPARTS

#endif /* __CONFIG_H */
