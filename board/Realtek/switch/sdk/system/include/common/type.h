/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 91355 $
 * $Date: 2018-08-13 21:54:45 +0800 (Mon, 13 Aug 2018) $
 *
 * Purpose : Definition the basic types in the SDK.
 *
 * Feature : type definition
 *
 */

#ifndef __COMMON_TYPE_H__
#define __COMMON_TYPE_H__

/*
 * Symbol Definition
 */

#define USING_RTSTK_PKT_AS_RAIL


#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN 6
#endif

#ifndef IP6_ADDR_LEN
#define IP6_ADDR_LEN    16
#endif


/*
 * Data Type Declaration
 */
#ifndef uint64
typedef unsigned long long  uint64;
#endif

#ifndef int64
typedef signed long long    int64;
#endif

#ifndef uint32
typedef unsigned int        uint32;
#endif

#ifndef int32
typedef signed int          int32;
#endif

#ifndef uint16
typedef unsigned short      uint16;
#endif

#ifndef int16
typedef signed short        int16;
#endif

#ifndef uint8
typedef unsigned char       uint8;
#endif

#ifndef int8
typedef signed char         int8;
#endif

#ifndef ipaddr_t
typedef uint32  ipaddr_t;           /* ipv4 address type */
#endif

/* configuration mode type */
typedef enum rtk_enable_e
{
    DISABLED = 0,
    ENABLED,
    RTK_ENABLE_END
} rtk_enable_t;

/* initial state of module */
typedef enum init_state_e
{
    INIT_NOT_COMPLETED = 0,
    INIT_COMPLETED,
    INIT_STATE_END
} init_state_t;

/* ethernet address type */
typedef struct  rtk_mac_s
{
    uint8 octet[ETHER_ADDR_LEN];
} rtk_mac_t;

/*
 * Macro Definition
 */

#endif /* __COMMON_TYPE_H__ */

