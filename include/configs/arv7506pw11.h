/*
 * Copyright (C) 2016 Mathias Kresin <dev@kresin.me>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_MACH_TYPE	"ARV7506PW11"
#define CONFIG_IDENT_STRING	" "CONFIG_MACH_TYPE
#define CONFIG_BOARD_NAME	"Arcadyan ARV7506PW11"

/* Configure SoC */
#define CONFIG_LTQ_SUPPORT_UART		/* Enable ASC and UART */

#define CONFIG_LTQ_SUPPORT_ETHERNET	/* Enable ethernet */

#define CONFIG_LTQ_SUPPORT_NOR_FLASH	/* Have a parallel NOR flash */

#define CONFIG_LTQ_SUPPORT_SPL_NOR_FLASH	/* Build NOR flash SPL */
#define CONFIG_LTQ_SPL_CONSOLE
#define CONFIG_LTQ_SPL_COMP_LZMA
#define CONFIG_SPL_TPL_OFFS		0x800
#define CONFIG_SPL_TPL_SIZE		0x5000
#define CONFIG_SPL_MC_TUNE_OFFS		0x5800
#define CONFIG_SPL_U_BOOT_OFFS		0x6000
#define CONFIG_SPL_U_BOOT_SIZE		0x32000

/* support (uncompressed) kernels up to 32 MByte */
#define CONFIG_SYS_LOAD_ADDR		0x82000000
#define CONFIG_SYS_BOOTM_LEN		0x2000000

/* Switch devices */
#define CONFIG_SWITCH_MULTI
#define CONFIG_SWITCH_RTL8306

/* 2nd stage bootloader */
#if defined(CONFIG_SYS_BOOT_BRN)
#define CONFIG_SYS_TEXT_BASE		0x80002000
#define CONFIG_SKIP_LOWLEVEL_INIT
#endif

/* Environment */
#if defined(CONFIG_SYS_BOOT_NOR)
#define CONFIG_ENV_IS_IN_FLASH
#define CONFIG_ENV_OVERWRITE
#define CONFIG_ENV_OFFSET		(256 * 1024)
#define CONFIG_ENV_SECT_SIZE		(64 * 1024)
#elif defined(CONFIG_SYS_BOOT_NORSPL)
#define CONFIG_ENV_IS_IN_FLASH
#define CONFIG_ENV_OVERWRITE
#define CONFIG_ENV_OFFSET		(192 * 1024)
#define CONFIG_ENV_SECT_SIZE		(64 * 1024)
#else
#define CONFIG_ENV_IS_NOWHERE
#endif

#define CONFIG_ENV_SIZE			(8 * 1024)

/* Console */
#define CONFIG_LTQ_ADVANCED_CONSOLE
#define CONFIG_BAUDRATE			115200
#define CONFIG_CONSOLE_ASC		1

/* Pull in default board configs for Lantiq XWAY Danube */
#include <asm/lantiq/config.h>
#include <asm/arch/config.h>

/* Pull in default OpenWrt configs for Lantiq SoC */
#include "openwrt-lantiq-common.h"

#define CONFIG_ENV_UPDATE_UBOOT_NOR		\
	"update-uboot-nor=run load-uboot-norspl-lzma write-uboot-nor\0"

#define CONFIG_EXTRA_ENV_SETTINGS	\
	CONFIG_ENV_LANTIQ_DEFAULTS	\
	CONFIG_ENV_UPDATE_UBOOT_NOR	\
	"kernel_addr=0xB0050000\0"

#endif /* __CONFIG_H */
