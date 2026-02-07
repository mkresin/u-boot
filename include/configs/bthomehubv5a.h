/*
 * Copyright (C) 2016 Mathias Kresin <openwrt@kresin.me>
 * Copyright (C) 2015 Martin Blumenstingl <martin.blumenstingl@googlemail.com>
 * Based on p2812hnufx.h: (C) 2013 Luka Perkov <luka@openwrt.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_MACH_TYPE	"BTHOMEHUBV5A"
#define CONFIG_IDENT_STRING	" "CONFIG_MACH_TYPE
#define CONFIG_BOARD_NAME	"BT Home Hub 5A"

/* Configure SoC */
#define CONFIG_LTQ_SUPPORT_UART			/* Enable ASC and UART */

#define CONFIG_LTQ_SUPPORT_ETHERNET		/* Enable ethernet */

#define CONFIG_LTQ_SUPPORT_NAND_FLASH		/* Have a ML01G100BHI00 NAND flash */
#define CONFIG_SYS_NAND_USE_FLASH_BBT

#define CONFIG_LTQ_SUPPORT_SPL_NAND_FLASH	/* Build NAND flash SPL */
#define CONFIG_SYS_NAND_PAGE_COUNT	64
#define CONFIG_SYS_NAND_PAGE_SIZE	2048
#define CONFIG_SYS_NAND_OOBSIZE		64
#define CONFIG_SYS_NAND_BLOCK_SIZE	(128 * 1024)
#define CONFIG_SYS_NAND_5_ADDR_CYCLE

#define CONFIG_LTQ_SPL_COMP_LZO			/* Compress SPL with LZO */
#define CONFIG_LTQ_SPL_CONSOLE			/* Enable SPL console */
#define CONFIG_LTQ_SPL_MC_TUNE

#define CONFIG_SYS_BOOTM_LEN		0x1000000	/* 16 MB */

/* MTD devices */
#define CONFIG_MTD_PARTITIONS
#define CONFIG_MTD_DEVICE
#define CONFIG_CMD_MTDPARTS
#define MTDIDS_DEFAULT			"nand0=nand-xway"
#define MTDPARTS_DEFAULT		"mtdparts=nand-xway:0x07e80000@0x100000(UBI)"

/* UBI */
#define CONFIG_RBTREE
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS

/* Environment */
#if defined(CONFIG_SYS_BOOT_NANDSPL)
#define CONFIG_SPL_TPL_OFFS		0x800
#define CONFIG_SPL_TPL_SIZE		0x5000
#define CONFIG_SPL_MC_TUNE_OFFS		0x5800
#define CONFIG_SPL_U_BOOT_OFFS		0x6000
#define CONFIG_SPL_U_BOOT_SIZE		0x3a000

#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OVERWRITE
#define CONFIG_ENV_OFFSET		(640 * 1024)
#define CONFIG_ENV_SECT_SIZE		(128 * 1024)
#else
#define CONFIG_ENV_IS_NOWHERE
#endif

#define CONFIG_ENV_SIZE			(128 * 1024)

#define CONFIG_LOADADDR			CONFIG_SYS_LOAD_ADDR

/* Console */
#define CONFIG_LTQ_ADVANCED_CONSOLE
#define CONFIG_BAUDRATE			115200
#define CONFIG_CONSOLE_ASC		1
#define CONFIG_CONSOLE_DEV		"ttyLTQ1"

/* Pull in default board configs for Lantiq XWAY VRX200 */
#include <asm/lantiq/config.h>
#include <asm/arch/config.h>

/* Pull in default OpenWrt configs for Lantiq SoC */
#include "openwrt-lantiq-common.h"

#undef CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND \
	"mtdparts default; ubi part UBI; ubi read ${loadaddr} kernel; bootm ${loadaddr}"

#define CONFIG_EXTRA_ENV_SETTINGS	\
	CONFIG_ENV_LANTIQ_DEFAULTS

#endif /* __CONFIG_H */
