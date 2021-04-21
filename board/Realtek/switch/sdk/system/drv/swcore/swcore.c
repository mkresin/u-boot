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
 * $Revision: 83626 $
 * $Date: 2017-11-21 16:06:39 +0800 (Tue, 21 Nov 2017) $
 *
 * Purpose : chip symbol and data type definition in the SDK.
 *
 * Feature : chip symbol and data type definition
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/debug/rt_log.h>
#include <osal/print.h>
#include <ioal/ioal_init.h>
#include <ioal/mem32.h>

//#if ((defined(RTK_UNIVERSAL_BSP) && defined(CONFIG_SDK_KERNEL_LINUX)) || (!defined(RTK_UNIVERSAL_BSP) && defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)) || (defined(CONFIG_SDK_OS_KERNEL_OTHER)) || (defined(__BOOTLOADER__)))
/*
#if (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_RTL8390_SERIES))
#include <private/drv/swcore/swcore_rtl8390.h>
#endif
#if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_RTL8380_SERIES))
#include <private/drv/swcore/swcore_rtl8380.h>
#endif
*/
#if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_RTL9300_SERIES))
#include <private/drv/swcore/swcore_rtl9300.h>
#endif
/*
#if (defined(CONFIG_SDK_RTL9310) || defined(CONFIG_RTL9310_SERIES))
#include <private/drv/swcore/swcore_rtl9310.h>
#endif
*/
#if (defined(RTK_UNIVERSAL_BSP))
#include "chip_probe.h"
#else
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#endif
//#endif
#include <hwp/hw_profile.h>
//#include <private/drv/watchdog/watchdog_common.h>
//#include <private/drv/watchdog/watchdog_mapper.h>
//#include <private/drv/uart/uart_common.h>
//#include <private/drv/uart/uart_mapper.h>
//#include <private/drv/tc/tc_common.h>
//#include <private/drv/tc/tc_mapper.h>
#include <soc/socdef/rtl9300_soc_reg.h>

/*
 * Symbol Definition
 */

//#if !defined(RTK_UNIVERSAL_BSP)
#if defined(CONFIG_SDK_RTL8390)
static int32
_drv_swcore_jtag_intf_8390_set(uint32 unit, drv_swcore_jtag_intf_t intf_sel)
{
    ioal_mem32_write(unit, RTL8390_MAC_IF_CTRL_ADDR, ((intf_sel) & RTL8390_MAC_IF_CTRL_JTAG_SEL_MASK));
    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8380)
static int32
_drv_swcore_jtag_intf_8380_set(uint32 unit, drv_swcore_jtag_intf_t intf_sel)
{
    ioal_mem32_write(unit, RTL8380_GMII_INTF_SEL_ADDR, ((intf_sel) & RTL8380_GMII_INTF_SEL_GMII_IF_SEL_MASK));
    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL9300)
static int32
_drv_swcore_jtag_intf_9300_set(uint32 unit, drv_swcore_jtag_intf_t intf_sel)
{
    ioal_mem32_write(unit, RTL9300_JTAG_SEL_CTRL_ADDR, ((intf_sel) & RTL9300_JTAG_SEL_CTRL_JTAG_SEL_MASK));
    return RT_ERR_OK;
}
#endif

/* Function Name:
 *      drv_swcore_jtag_intf_set
 * Description:
 *      Set the usage of JTAG interface.
 * Input:
 *      unit           - unit id
 *      intf_sel       - interface type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK      - OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_swcore_jtag_intf_set(uint32 unit, drv_swcore_jtag_intf_t intf_sel)
{

    switch(HWP_CHIP_FAMILY_ID(unit))
    {
   #if defined(CONFIG_SDK_RTL8390)
    case RTL8390_FAMILY_ID:
    case RTL8350_FAMILY_ID:
        _drv_swcore_jtag_intf_8390_set(unit, intf_sel);
        break;
   #endif
   #if defined(CONFIG_SDK_RTL8380)
    case RTL8380_FAMILY_ID:
    case RTL8330_FAMILY_ID:
        _drv_swcore_jtag_intf_8380_set(unit, intf_sel);
        break;
   #endif
   #if defined(CONFIG_SDK_RTL9300)
    case RTL9300_FAMILY_ID:
        _drv_swcore_jtag_intf_9300_set(unit, intf_sel);
        break;
   #endif
    default:
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;

} /* end of drv_swcore_jtag_intf_set */

#if defined(CONFIG_SDK_RTL8390)
static int32
_drv_swcore_jtag_intf_8390_get(uint32 unit, drv_swcore_jtag_intf_t * pIntf_sel)
{
    uint32 reg_value;
    ioal_mem32_read(unit, RTL8390_MAC_IF_CTRL_ADDR, &reg_value);
    *pIntf_sel = ((reg_value) & RTL8390_MAC_IF_CTRL_JTAG_SEL_MASK);
    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8380)
static int32
_drv_swcore_jtag_intf_8380_get(uint32 unit, drv_swcore_jtag_intf_t * pIntf_sel)
{
    uint32 reg_value;
    ioal_mem32_read(unit, RTL8380_GMII_INTF_SEL_ADDR, &reg_value);
    *pIntf_sel = ((reg_value) & RTL8380_GMII_INTF_SEL_GMII_IF_SEL_MASK);
    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL9300)
static int32
_drv_swcore_jtag_intf_9300_get(uint32 unit, drv_swcore_jtag_intf_t * pIntf_sel)
{
    uint32 reg_value;
    ioal_mem32_read(unit, RTL9300_JTAG_SEL_CTRL_ADDR, &reg_value);
    *pIntf_sel = ((reg_value) & RTL9300_JTAG_SEL_CTRL_JTAG_SEL_MASK);
    return RT_ERR_OK;
}
#endif

/* Function Name:
 *      drv_swcore_jtag_intf_get
 * Description:
 *      Get the configuration of JTAG interface.
 * Input:
 *      unit           - unit id
 *      intf_sel       - interface type
 * Output:
 *      intf_sel       - interface type
 * Return:
 *      RT_ERR_OK      - OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_swcore_jtag_intf_get(uint32 unit, drv_swcore_jtag_intf_t * pIntf_sel)
{
    drv_swcore_jtag_intf_t intf_conf;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIntf_sel), RT_ERR_NULL_POINTER);

    switch(HWP_CHIP_FAMILY_ID(unit))
    {
   #if defined(CONFIG_SDK_RTL8390)
    case RTL8390_FAMILY_ID:
    case RTL8350_FAMILY_ID:
        _drv_swcore_jtag_intf_8390_get(unit, &intf_conf);
        break;
   #endif
   #if defined(CONFIG_SDK_RTL8380)
    case RTL8380_FAMILY_ID:
    case RTL8330_FAMILY_ID:
        _drv_swcore_jtag_intf_8380_get(unit, &intf_conf);
        break;
   #endif
   #if defined(CONFIG_SDK_RTL9300)
    case RTL9300_FAMILY_ID:
        _drv_swcore_jtag_intf_9300_get(unit, &intf_conf);
        break;
   #endif
    default:
        return RT_ERR_FAILED;
    }
    *pIntf_sel = intf_conf;
    return RT_ERR_OK;

} /* end of drv_swcore_jtag_intf_get */

/* Function Name:
 *      drv_swcore_family_cid_get
 * Description:
 *      Get Chip Family ID
 * Input:
 *      unit            - unit id
 * Output:
 *      pFamily_id      - Family_id
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - Failed
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_swcore_family_cid_get(uint32 unit, uint32 * pFamily_id)
{
#if defined(CONFIG_SDK_RTL9310)
    if(HWP_9310_FAMILY_ID(unit))
    {
        *pFamily_id = RTL9310_FAMILY_ID;
    }
    else
#endif
#if defined(CONFIG_SDK_RTL9300)
    if(HWP_9300_FAMILY_ID(unit))
    {
        *pFamily_id = RTL9300_FAMILY_ID;
    }
    else
#endif
#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_FAMILY_ID(unit))
    {
        *pFamily_id = RTL8390_FAMILY_ID;
    }
    else
    if(HWP_8350_FAMILY_ID(unit))
    {
        *pFamily_id = RTL8350_FAMILY_ID;
    }
    else
#endif
#if defined(CONFIG_SDK_RTL8380)
    if(HWP_8380_FAMILY_ID(unit))
    {
        *pFamily_id = RTL8380_FAMILY_ID;
    }
    else
    if(HWP_8330_FAMILY_ID(unit))
    {
        *pFamily_id = RTL8330_FAMILY_ID;
    }
    else
#endif
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

} /* end of drv_swcore_family_cid_get */


int32
drv_swcore_CPU_freq_get_8390(uint32 unit, uint32 * pCPU_freq)
{
    uint32 pll_value0 = 0, pll_value1 = 0;

    ioal_mem32_read(unit, RTL839x5x_CPU_PLL0, &pll_value0);
    ioal_mem32_read(unit, RTL839x5x_CPU_PLL1, &pll_value1);
    *pCPU_freq = 0;

    if((pll_value0 == RTL839x5x_CPU_750M_PLL0) && (pll_value1 == RTL839x5x_CPU_750M_PLL1))
        *pCPU_freq = 750;
    if((pll_value0 == RTL839x5x_CPU_700M_PLL0) && (pll_value1 == RTL839x5x_CPU_700M_PLL1))
        *pCPU_freq = 700;
    if((pll_value0 == RTL839x5x_CPU_650M_PLL0) && (pll_value1 == RTL839x5x_CPU_650M_PLL1))
        *pCPU_freq = 650;

    if(*pCPU_freq != 0)
        return RT_ERR_OK;

    return RT_ERR_FAILED;
}

int32
drv_swcore_CPU_freq_get_8380(uint32 unit, uint32 * pCPU_freq)
{
    uint32  pll_value0 = 0, pll_value1 = 0;
    uint32  enable_value = 0, pll_value2 = 0;

    enable_value = 0x3;
    ioal_mem32_write(unit, RTL838x3x_CPU_READ_CTL, enable_value);
    ioal_mem32_read(unit, RTL838x3x_CPU_CTL0, &pll_value0);
    ioal_mem32_read(unit, RTL838x3x_CPU_CTL1, &pll_value1);
    ioal_mem32_read(unit, RTL838x3x_CPU_MISC, &pll_value2);
    enable_value = 0x0;
    ioal_mem32_write(unit, RTL838x3x_CPU_READ_CTL, enable_value);
    *pCPU_freq = 0;
    if((pll_value0 == RTL838x3x_CPU_500M_CTL0) && (pll_value1 == RTL838x3x_CPU_500M_CTL1)  && (pll_value2 ==RTL838x3x_CPU_500M_MISC))
        *pCPU_freq = 500;
    if((pll_value0 == RTL838x3x_CPU_300M_CTL0) && (pll_value1 == RTL838x3x_CPU_300M_CTL1)  && (pll_value2 ==RTL838x3x_CPU_300M_MISC))
        *pCPU_freq = 300;

    if(*pCPU_freq == 0)
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}

int32
drv_swcore_CPU_freq_get_9310(uint32 unit, uint32 * pCPU_freq)
{
    return RT_ERR_OK;
}

int32
drv_swcore_CPU_freq_get_9300(uint32 unit, uint32 * pCPU_freq)
{
    return RT_ERR_OK;
}

/* Function Name:
 *      drv_swcore_CPU_freq_get
 * Description:
 *      Get Chip CPU Frequency
 * Input:
 *      unit           - unit id
 * Output:
 *      pCPU_freq       - CPU_freq
 * Return:
 *      RT_ERR_OK        - OK
 *      RT_ERR_FAILED   - Failed
 *      RT_ERR_CHIP_NOT_SUPPORTED - Not support by this API
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_swcore_CPU_freq_get(uint32 unit, uint32 * pCPU_freq)
{

#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
        return( drv_swcore_CPU_freq_get_8390(unit,pCPU_freq));
    }
#endif


#if defined(CONFIG_SDK_RTL8380)
    if(HWP_8380_30_FAMILY(unit))
    {
        return( drv_swcore_CPU_freq_get_8380(unit,pCPU_freq));
    }
#endif

#if defined(CONFIG_SDK_RTL9310)

#endif

#if defined(CONFIG_SDK_RTL9300)

#endif

    return RT_ERR_CHIP_NOT_SUPPORTED;

} /* end of drv_swcore_CPU_freq_get */


int32
_drv_swcore_gpioRegBase_get(uint32 unit, uint32 *pReg_base_offset, uint32 *pReg_num)
{
    if(HWP_9300_FAMILY_ID(unit))
    {
        *pReg_base_offset = RTL9300_GPIO_NO_USE_0004_ADDR;
        *pReg_num     = 7;
    }
    else
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

#define MAX_REG_IDX 64

//#endif

