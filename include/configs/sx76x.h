/*
 * Copyright (C) 2011-2013 Luka Perkov <luka@openwrt.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_IDENT_STRING	" GIGASX76X"
#define CONFIG_BOARD_NAME	"Gigaset sx76x"

/* Configure SoC */
#define CONFIG_LTQ_SUPPORT_UART		/* Enable ASC and UART */

#define CONFIG_LTQ_SUPPORT_ETHERNET	/* Enable ethernet */

#define CONFIG_LTQ_SUPPORT_NOR_FLASH	/* Have a parallel NOR flash */

#if defined(CONFIG_SYS_BOOT_NORSPL)
#define CONFIG_LTQ_SUPPORT_SPL_NOR_FLASH	/* Build NOR flash SPL */
#define CONFIG_LTQ_SPL_CONSOLE
#define CONFIG_LTQ_SPL_COMP_LZO
#define CONFIG_SPL_TPL_OFFS		0x100
#define CONFIG_SPL_TPL_SIZE		0x3100
#define CONFIG_SPL_U_BOOT_OFFS		0x3200
#define CONFIG_SPL_U_BOOT_SIZE		0x2ce00
#endif

/* Switch devices */
#define CONFIG_SWITCH_MULTI
#define CONFIG_SWITCH_ADM6996I

/* Environment */
#if defined(CONFIG_SYS_BOOT_NOR) || defined(CONFIG_SYS_BOOT_NORSPL)
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

/* Boot */
#define CONFIG_BOOTCOMMAND \
	"bootm 0xb0040000"

/* Pull in default board configs for Lantiq XWAY Danube */
#include <asm/lantiq/config.h>
#include <asm/arch/config.h>

/* Pull in default OpenWrt configs for Lantiq SoC */
#include "openwrt-lantiq-common.h"

#define CONFIG_EXTRA_ENV_SETTINGS	\
	CONFIG_ENV_LANTIQ_DEFAULTS

#endif /* __CONFIG_H */
