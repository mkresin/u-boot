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
 * $Revision: 96561 $
 * $Date: 2019-04-26 16:12:10 +0800 (Fri, 26 Apr 2019) $
 *
 * Purpose : Definition of the default values in the SDK
 *
 * Feature : Define the default values
 *
 */

#ifndef __RTK_DEFAULT_H__
#define __RTK_DEFAULT_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <rtk/l2.h>

/*
#include <rtk/trunk.h>
#include <rtk/vlan.h>
#include <rtk/stp.h>
#include <rtk/qos.h>
#include <rtk/switch.h>
#include <rtk/pie.h>
#include <rtk/rate.h>
#include <rtk/l3.h>
#include <rtk/led.h>
*/

/*
 * Symbol Definition
 */

/* L2 module */
#define RTK_DEFAULT_L2_FLUSH_LINKDOWN_MAC           DISABLED
#define RTK_DEFAULT_L2_IP6_CACHE_TBL_SIZE           512
#define RTK_DEFAULT_L2_NON_UCAST_SA_ACTION          ACTION_DROP
#define RTK_DEFAULT_L2_NEW_SA_ACTION                ACTION_FORWARD
#define RTK_DEFAULT_L2_NEW_SA_LEARN_MODE            HARDWARE_LEARNING
#define RTK_DEFAULT_L2_CPU_NEW_SA_LEARN_MODE        NOT_LEARNING
#define RTK_DEFAULT_L2_IP4_MCAST_DIP_VALIDATION_STATE ENABLED
#define RTK_DEFAULT_L2_IP_MCAST_IVL_LOOKUP_STATE    ENABLED
#define RTK_DEFAULT_L2_IP6_MCAST_LOOKUP_SIP_CARE_BYTE 0x5210
#define RTK_DEFAULT_L2_IP6_MCAST_LOOKUP_DIP_CARE_BYTE 0x3210
#define RTK_DEFAULT_L2_SA_BLOCK_PORT_STATE          ENABLED
#define RTK_DEFAULT_L2_SA_BLOCK_CPU_PORT_STATE      DISABLED
#define RTK_DEFAULT_L2_DA_BLOCK_PORT_STATE          ENABLED
#define RTK_DEFAULT_L2_DA_BLOCK_CPU_PORT_STATE      DISABLED

#define RTK_DEFAULT_L2_STTC_PM_LRN_EN	DISABLED
#define RTK_DEFAULT_L2_NTFY_LD_FLUSH_EN	ENABLED
#define RTK_DEFAULT_L2_NTFY_DYN_EN		ENABLED

/* PORT module */
#define RTK_DEFAULT_PORT_AUTONEGO_ENABLE            ENABLED
#define RTK_DEFAULT_PORT_10HALF_CAPABLE             ENABLED
#define RTK_DEFAULT_PORT_10FULL_CAPABLE             ENABLED
#define RTK_DEFAULT_PORT_100HALF_CAPABLE            ENABLED
#define RTK_DEFAULT_PORT_100FULL_CAPABLE            ENABLED
#define RTK_DEFAULT_PORT_1000HALF_CAPABLE           ENABLED
#define RTK_DEFAULT_PORT_1000FULL_CAPABLE           ENABLED
#define RTK_DEFAULT_PORT_2_5G_CAPABLE               ENABLED
#define RTK_DEFAULT_PORT_5G_CAPABLE                 ENABLED
#define RTK_DEFAULT_PORT_10GHALF_CAPABLE            ENABLED
#define RTK_DEFAULT_PORT_10GFULL_CAPABLE            ENABLED
#define RTK_DEFAULT_PORT_PAUSE_CAPABILITY           ENABLED
#define RTK_DEFAULT_PORT_ASYPAUSE_CAPABILITY        DISABLED
#define RTK_DEFAULT_PORT_ISOLATION_PORTMASK         0x1FFFFFFF
#define RTK_DEFAULT_PORT_TX_ENABLE                  ENABLED
#define RTK_DEFAULT_PORT_RX_ENABLE                  ENABLED
#define RTK_DEFAULT_PORT_ADMIN_ENABLE               DISABLED
#define RTK_DEFAULT_PORT_VID                        RTK_DEFAULT_VLAN_ID

/* NIC module */
/* Configure CPU Tag in packet descriptor or in packet raw data
 * ENABLED: in packet descriptor
 * DISABLED: in packet raw data
 */
#define RTK_DEFAULT_NIC_CPU_TAG_IN_DESCRIPTOR               ENABLED

/* Configure CPU Tag parser ability in cpu port
 * ENABLED: have parser cpu tag ability.
 * DISABLED: no parser cpu tag ability.
 */
#define RTK_DEFAULT_CPU_TAG_PARSER_ABILITY_IN_CPU_PORT      ENABLED

/* Configure insert all packets leaving CPU port with CPU tag
 * ENABLED: insert to raw data.
 * DISABLED: no insert.
 */
#define RTK_DEFAULT_CPU_TAG_INSERT_IN_LEAVING_CPU_PORT_PKT  ENABLED

/* RTK Link Monitor Thread Configuration
 */
#define RTK_DEFAULT_LINK_MON_POLLING_STACK_SIZE 4096
#define RTK_DEFAULT_LINK_MON_POLLING_THREAD_PRI 3
#define RTK_DEFAULT_LINK_MON_ISR_STACK_SIZE 4096
#define RTK_DEFAULT_LINK_MON_ISR_THREAD_PRI 0

/* RTK LOS Monitor Thread Configuration
 */
#define RTK_DEFAULT_LOS_MON_STACK_SIZE 4096
#define RTK_DEFAULT_LOS_MON_THREAD_PRI 3

/* RTK Workaround Monitor Thread Configuration
 */
#define RTK_DEFAULT_WA_MON_STACK_SIZE 4096
#define RTK_DEFAULT_WA_MON_THREAD_PRI 3

/* RTK Mac Constraint Workaround Monitor Thread Configuration
 */
#define RTK_DEFAULT_MAC_MON_STACK_SIZE 4096
#define RTK_DEFAULT_MAC_MON_THREAD_PRI 2

/*
 * Data Declaration
 */



/*
 * Macro Definition
 */
#endif /* __RTK_DEFAULT_H__ */

