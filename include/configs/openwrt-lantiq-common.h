/*
 * Copyright (C) 2013 Luka Perkov <luka@openwrt.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __OPENWRT_LANTIQ_COMMON_H
#define __OPENWRT_LANTIQ_COMMON_H

/* Commands */
#if defined(CONFIG_LTQ_SUPPORT_ETHERNET)
#define CONFIG_CMD_PING
#define CONFIG_CMD_TFTPPUT
#endif

/* Compression */
#define CONFIG_LZMA

/* Auto boot */
#define CONFIG_BOOTDELAY	2

/* Maximum boot image size */
#define CONFIG_SYS_BOOTM_LEN	0x1000000 /* 16 MB */

/* Ethernet */
#if defined(CONFIG_LTQ_SUPPORT_ETHERNET)
#define CONFIG_ETHADDR		00:01:02:03:04:05
#define CONFIG_SERVERIP		192.168.1.2
#define CONFIG_IPADDR		192.168.1.1
#endif

/* Unnecessary */
#undef CONFIG_BOOTM_NETBSD
#undef CONFIG_BOOTM_PLAN9
#undef CONFIG_BOOTM_RTEMS
#undef CONFIG_GZIP_COMPRESSED
#if !defined(CONFIG_CMD_UBIFS)
#undef CONFIG_ZLIB
#undef CONFIG_GZIP
#endif

/* allow tiny binaries */
#if !defined(CONFIG_LTQ_ADVANCED_CONSOLE)
#undef CONFIG_SYS_HUSH_PARSER
#undef CONFIG_CMD_MII
#undef CONFIG_CMD_BDI
#undef CONFIG_CMD_EDITENV
#undef CONFIG_CMD_IMI
#undef CONFIG_CMD_LOADS
#undef CONFIG_CMD_LOADB
/* only cp (memory copy) for u-boot update */
#if defined(CONFIG_SYS_BOOT_NOR) || defined(CONFIG_SYS_BOOT_NORSPL)
#define CONFIG_CMD_MEMORY_MINIMAL
#else
#undef CONFIG_CMD_MEMORY
#endif
#define CONFIG_SYS_FLASH_EMPTY_INFO
#define CONFIG_SYS_IMAGE_EMPTY_INFO
#endif

#endif /* __OPENWRT_LANTIQ_COMMON_H */
