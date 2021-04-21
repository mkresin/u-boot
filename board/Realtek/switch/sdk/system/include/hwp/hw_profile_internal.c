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
 * $Revision: 96290 $
 * $Date: 2019-04-12 17:43:06 +0800 (Fri, 12 Apr 2019) $
 *
 * Purpose : Internal model list.
 *
 * Feature :
 *
 */

/*
 * Data Type Definition
 */

typedef enum hwp_id_internal_e
{
    HWP_RTL8391M_RTL9310_FPGA_L3               = 8390501,
    HWP_RTL8391M_RTL9310_FPGA,
    HWP_RTL8393M_TWO8382L_SIMU,
    HWP_RTL8393M_RTL9300_24FE_4XGE_FPGA,
    HWP_RTL8393M_RTL9300_8XGE_FPGA,
    HWP_RTL8393M_DEMO_SKIP_PHY1,
    HWP_RTL9300_24GE_4XGE_I2C_FPGA             = 9300501,
    HWP_RTL9300_CASCADE_24GE_4XGE_I2C_FPGA,
    HWP_RTL9300_24GE_4XGE_FPGA,
    HWP_RTL9300_8XGE_FPGA,
    HWP_RTL9303_8XGE_QA1,
    HWP_RTL9310_FPGA_L3                        = 9310501,
    HWP_RTL9310_FPGA,
    HWP_RTL9310_48GE_4XGE_FPGA,
    HWP_RTL9311_FT,
}hwp_id_internal_t;


/*
 * Include Files
 */

#include <common/rt_autoconf.h>


/*
 * Data Declaration
 */
#define RTK_INTERNAL_PROFILE\
    RTL9300_INTERNAL_PROFILE\



