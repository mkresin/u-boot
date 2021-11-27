/*
 * Copyright (C) 2013 Luka Perkov <luka@openwrt.org>
 * Copyright (C) 2012-2015 Daniel Schwierzeck, daniel.schwierzeck@gmail.com
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_IDENT_STRING	" P2812HNUF1"
#define CONFIG_BOARD_NAME	"Zyxel P-2812HNU-F1"

/* Configure SoC */
#define CONFIG_LTQ_SUPPORT_UART		/* Enable ASC and UART */

#define CONFIG_LTQ_SUPPORT_ETHERNET	/* Enable ethernet */
#define CONFIG_FW_VRX200_PHY11G_A1X
#define CONFIG_FW_VRX200_PHY11G_A2X

#define CONFIG_LTQ_SUPPORT_NAND_FLASH		/* Have a K9F1G08U0D NAND flash */
#define CONFIG_LTQ_NAND_CS1
#define CONFIG_SYS_NAND_PAGE_COUNT	64
#define CONFIG_SYS_NAND_PAGE_SIZE	2048
#define CONFIG_SYS_NAND_OOBSIZE		64
#define CONFIG_SYS_NAND_BLOCK_SIZE	(128 * 1024)
#define CONFIG_SYS_NAND_5_ADDR_CYCLE
/* The whole NAND flash is used in OpenWrt and no space left for the BBT */
/* #define CONFIG_SYS_NAND_USE_FLASH_BBT */

#define CONFIG_LTQ_SUPPORT_SPL_NAND_FLASH	/* Build NAND flash SPL */
#define CONFIG_LTQ_SPL_COMP_LZO
#define CONFIG_LTQ_SPL_CONSOLE
#define CONFIG_LTQ_SPL_MC_TUNE
#define CONFIG_MISC_INIT_R

#define CONFIG_SPL_TPL_OFFS		0x800
#define CONFIG_SPL_TPL_SIZE		0x5000
#define CONFIG_SPL_MC_TUNE_OFFS		0x5800
#define CONFIG_SPL_U_BOOT_OFFS		0x6000
#define CONFIG_SPL_U_BOOT_SIZE		0x3a000

/* Environment */
#if defined(CONFIG_SYS_BOOT_NANDSPL)
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OVERWRITE
#define CONFIG_ENV_OFFSET		(2 * CONFIG_SYS_NAND_BLOCK_SIZE)
#define CONFIG_ENV_SECT_SIZE		CONFIG_SYS_NAND_BLOCK_SIZE
#else
#define CONFIG_ENV_IS_NOWHERE
#endif

#define CONFIG_ENV_SIZE			(8 * 1024)

/* Console */
#define CONFIG_LTQ_ADVANCED_CONSOLE
#define CONFIG_BAUDRATE			115200
#define CONFIG_CONSOLE_ASC		1

/* Boot */
#define CONFIG_BOOTCOMMAND \
	"nand read ${loadaddr} 0x60000 0x300000; bootm ${loadaddr}"

/* Pull in default board configs for Lantiq XWAY VRX200 */
#include <asm/lantiq/config.h>
#include <asm/arch/config.h>

/* Pull in default OpenWrt configs for Lantiq SoC */
#include "openwrt-lantiq-common.h"

#define CONFIG_EXTRA_ENV_SETTINGS	\
	CONFIG_ENV_LANTIQ_DEFAULTS

#endif /* __CONFIG_H */
