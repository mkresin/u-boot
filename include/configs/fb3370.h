/*
 * Copyright (C) 2011-2013 Daniel Schwierzeck, daniel.schwierzeck@googlemail.com
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_IDENT_STRING	" FB3370"
#define CONFIG_BOARD_NAME	"AVM FritzBox 3370"

/* Configure SoC */
#define CONFIG_LTQ_SUPPORT_UART			/* Enable ASC and UART */

#define CONFIG_LTQ_SUPPORT_ETHERNET		/* Enable ethernet */
#define CONFIG_FW_VRX200_PHY11G_A1X
#define CONFIG_FW_VRX200_PHY11G_A2X

#define CONFIG_LTQ_SUPPORT_SPI_FLASH
#define CONFIG_SPI_FLASH_MACRONIX		/* Have a MX29LV620 serial flash */

#define CONFIG_LTQ_SUPPORT_NAND_FLASH
#define CONFIG_LTQ_NAND_CS0
#define CONFIG_SYS_NAND_PAGE_COUNT     64
#define CONFIG_SYS_NAND_PAGE_SIZE      2048
#define CONFIG_SYS_NAND_OOBSIZE                64
#define CONFIG_SYS_NAND_BLOCK_SIZE     (128 * 1024)
#define CONFIG_SYS_NAND_5_ADDR_CYCLE
#define CONFIG_SYS_NAND_USE_FLASH_BBT

#define CONFIG_LTQ_SUPPORT_SPL_SPI_FLASH	/* Build SPI flash SPL */
#define CONFIG_LTQ_SPL_COMP_LZMA			/* Compress SPL with LZMA */
#if 0
#define CONFIG_LTQ_SPL_CONSOLE			/* Enable SPL console */
#define CONFIG_LTQ_SPL_MC_TUNE
#endif

#define CONFIG_SPL_SPI_BUS		0
#define CONFIG_SPL_SPI_CS		4
#define CONFIG_SPL_SPI_MAX_HZ		25000000
#define CONFIG_SPL_SPI_MODE		0
#if 0
#define CONFIG_SPL_MC_TUNE_OFFS		0x6000
#endif
#define CONFIG_SPL_U_BOOT_OFFS		0x4700
#define CONFIG_SPL_U_BOOT_SIZE		0x1b900


/* 2nd stage bootloader */
#if defined(CONFIG_SYS_BOOT_EVA)
#define CONFIG_SYS_TEXT_BASE		0x80002000
#define CONFIG_SKIP_LOWLEVEL_INIT
#endif

#if 0
/* MTD devices */
#define CONFIG_MTD_PARTITIONS
#define CONFIG_MTD_DEVICE
#define CONFIG_CMD_MTDPARTS
#define MTDIDS_DEFAULT                 "nand0=nand-xway"
#define MTDPARTS_DEFAULT               "mtdparts=nand-xway:0x7c00000@0x400000(ubi)"

/* UBI */
#define CONFIG_RBTREE
#define CONFIG_CMD_UBI
#endif

/* Environment */
#define CONFIG_ENV_SPI_BUS		CONFIG_SPL_SPI_BUS
#define CONFIG_ENV_SPI_CS		CONFIG_SPL_SPI_CS
#define CONFIG_ENV_SPI_MAX_HZ		CONFIG_SPL_SPI_MAX_HZ
#define CONFIG_ENV_SPI_MODE		CONFIG_SPL_SPI_MODE

#if 0
#if defined(CONFIG_SYS_BOOT_SFSPL)
#define CONFIG_ENV_IS_IN_UBI
#define CONFIG_ENV_OVERWRITE
#define CONFIG_ENV_UBI_PART		"ubi"
#define CONFIG_ENV_UBI_VOLUME		"uboot_env"
#define CONFIG_ENV_UBI_VID_HDR_OFF	"2048"
#else
#define CONFIG_ENV_IS_NOWHERE
#endif
#else
#define CONFIG_ENV_IS_NOWHERE
#endif

#define CONFIG_ENV_SIZE			(8 * 1024)

/* Console */
#if !defined(CONFIG_SYS_BOOT_SFSPL)
#define CONFIG_LTQ_ADVANCED_CONSOLE
#endif
#define CONFIG_BAUDRATE			115200
#define CONFIG_CONSOLE_ASC		1

/* Boot */
#define CONFIG_BOOTCOMMAND \
	"nand read ${loadaddr} 0x0 0x400000; bootm ${loadaddr}"

/* Pull in default board configs for Lantiq XWAY VRX200 */
#include <asm/lantiq/config.h>
#include <asm/arch/config.h>

/* Pull in default OpenWrt configs for Lantiq SoC */
#include "openwrt-lantiq-common.h"

#define CONFIG_EXTRA_ENV_SETTINGS	\
	CONFIG_ENV_LANTIQ_DEFAULTS

#endif /* __CONFIG_H */
