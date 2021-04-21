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
 * $Revision: 97845 $
 * $Date: 2019-06-20 10:00:13 +0800 (Thu, 20 Jun 2019) $
 *
 * Purpose : Model list.
 *           This file shall be custom defined.
 * Feature :
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_error.h>
#include <osal/lib.h>
#include <osal/print.h>
#include <hwp/hw_profile.h>
#include <hal/chipdef/chipdef.h>
#include <drv/gpio/generalCtrl_gpio.h>
#include <common/debug/rt_log.h>
#include <common/util/rt_util.h>

/* Include hardware profile files of supported boards */

#if defined(CONFIG_SDK_RTL9300)
    #include <hwp/hw_profiles/rtl9302b_8218d_2x8226_2xge.c>
#endif

#include <hwp/hw_profile_internal.c>


/*
 * Data Declaration
 */

/* list of supported hardware profiles */
const hwp_hwProfile_t *hwp_hwProfile_list[] =
{
#if defined(CONFIG_SDK_RTL9300)
    &rtl9302b_8218d_2x8226_2xge,
#endif

    NULL,
}; /* end hwp_hwProfile_list */


