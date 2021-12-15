/*
 * Copyright (C) 2011-2013 Daniel Schwierzeck, daniel.schwierzeck@gmail.com
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_IDENT_STRING	" ARV7519RW22"
#define CONFIG_BOARD_NAME	"Arcadyan ARV7519RW22"

/* Configure SoC */
#define CONFIG_LTQ_SUPPORT_UART		/* Enable ASC and UART */

#define CONFIG_LTQ_SUPPORT_ETHERNET	/* Enable ethernet */
#define CONFIG_FW_VRX200_PHY22F_A1X
#define CONFIG_FW_VRX200_PHY22F_A2X

#define CONFIG_LTQ_SUPPORT_NOR_FLASH	/* Have a parallel NOR flash */

#if defined(CONFIG_SYS_BOOT_NORSPL)
#define CONFIG_LTQ_SUPPORT_SPL_NOR_FLASH	/* Build NOR flash SPL */
#define CONFIG_LTQ_SPL_CONSOLE
#define CONFIG_LTQ_SPL_COMP_LZO
#define CONFIG_LTQ_SPL_MC_TUNE
#define CONFIG_MISC_INIT_R

#define CONFIG_SPL_TPL_OFFS		0x100
#define CONFIG_SPL_TPL_SIZE		0x4000
#define CONFIG_SPL_MC_TUNE_OFFS		0x4100
#define CONFIG_SPL_U_BOOT_OFFS		0x4300
#define CONFIG_SPL_U_BOOT_SIZE		0x5bd00
#endif

/* 2nd stage bootloader */
#if defined(CONFIG_SYS_BOOT_BRN)
#define CONFIG_SYS_TEXT_BASE		0x80002000
#define CONFIG_SKIP_LOWLEVEL_INIT
#endif

/* Environment */
#if defined(CONFIG_SYS_BOOT_NOR) || defined(CONFIG_SYS_BOOT_NORSPL)
#define CONFIG_ENV_IS_IN_FLASH
#define CONFIG_ENV_OVERWRITE
#define CONFIG_ENV_OFFSET		(384 * 1024)
#define CONFIG_ENV_SECT_SIZE		(128 * 1024)
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
	"bootm 0xb0080000"

/* Pull in default board configs for Lantiq XWAY VRX200 */
#include <asm/lantiq/config.h>
#include <asm/arch/config.h>

/* Pull in default OpenWrt configs for Lantiq SoC */
#include "openwrt-lantiq-common.h"

#define CONFIG_EXTRA_ENV_SETTINGS	\
	CONFIG_ENV_LANTIQ_DEFAULTS	\
	"kernel_addr=0xB0080000\0"

#endif /* __CONFIG_H */
