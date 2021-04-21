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
 * $Revision: 101663 $
 * $Date: 2019-11-11 11:20:12 +0800 (Mon, 11 Nov 2019) $
 *
 * Purpose : Definition those public Port APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Port
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/sem.h>
#include <osal/thread.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/time.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/longan/rtk_longan_table_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/mac/serdes.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <hal/common/miim.h>
#include <hal/common/halctrl.h>
#include <dal/dal_common.h>
#include <dal/dal_phy.h>
#include <dal/longan/dal_longan_port.h>
//#include <dal/longan/dal_longan_rate.h>
//#include <dal/longan/dal_longan_vlan.h>
#include <dal/longan/dal_longan_sds.h>
#include <rtk/port.h>
#include <rtk/default.h>
#include <private/drv/intr/intr.h>
#include <hal/phy/phy_rtl9300.h>

#include <hal/mac/miim_common_drv.h>
#include <dal/dal_linkMon.h>

#if defined(CONFIG_SDK_RTL8295R)  /* 95R MAC-POLL-PHY */
  #include <hal/phy/phy_rtl8295.h>
#endif

/*
 * Symbol Definition
 */
typedef struct dal_longan_mac_info_s {
    uint8   admin_enable[RTK_MAX_NUM_OF_PORTS];
    uint8   green_enable[RTK_MAX_NUM_OF_PORTS];
    rtk_port_swMacPollPhyStatus_t   swMacPollPhyStatus[RTK_MAX_NUM_OF_PORTS]; /* SW-MAC-POLL-PHY */
} dal_longan_mac_info_t;

#define PORT_SPEED_TO_RATELIMIT(_g)     (((_g) * 1000000)/16)

/*
 * Data Declaration
 */
static uint32               port_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         port_sem[RTK_MAX_NUM_OF_UNIT];
static osal_mutex_t         mac_force_sem[RTK_MAX_NUM_OF_UNIT];

static dal_longan_mac_info_t   *pMac_info[RTK_MAX_NUM_OF_UNIT];
static dal_link_change_callback_f   link_change_callback_f[RTK_MAX_NUM_OF_UNIT];


/*
 * Macro Definition
 */
/* port semaphore handling */
#define PORT_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(port_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_PORT), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define PORT_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(port_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_PORT), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

#define MAC_FORCE_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(mac_force_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_PORT), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define MAC_FORCE_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(mac_force_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_PORT), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

/*
 * Function Declaration
 */
 #if !defined(__BOOTLOADER__)
static int32 _dal_longan_port_init_config(uint32 unit);
#endif

extern int32 dal_waMon_phyReconfig_portMaskSet(uint32 unit, rtk_port_t port);
int32 _dal_longan_port_swMacPollPhyEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *penable);
int32 _dal_longan_port_swChgSerdesModeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *penable);

/* Module Name    : port     */
/* Sub-module Name: Global */

/* Function Name:
 *      dal_longan_portMapper_init
 * Description:
 *      Hook port module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook port module before calling any port APIs.
 */
int32
dal_longan_portMapper_init(dal_mapper_t *pMapper)
{
    pMapper->port_isolation_get = dal_longan_port_isolation_get;
    pMapper->port_isolation_set = dal_longan_port_isolation_set;
    pMapper->port_isolationExt_get = dal_longan_port_isolationExt_get;
    pMapper->port_isolationExt_set = dal_longan_port_isolationExt_set;
    pMapper->port_isolation_add = dal_longan_port_isolation_add;
    pMapper->port_isolation_del = dal_longan_port_isolation_del;
    pMapper->port_isolationRestrictRoute_get = dal_longan_port_isolationRestrictRoute_get;
    pMapper->port_isolationRestrictRoute_set = dal_longan_port_isolationRestrictRoute_set;
    pMapper->port_vlanBasedIsolationEntry_get = dal_longan_port_vlanBasedIsolationEntry_get;
    pMapper->port_vlanBasedIsolationEntry_set = dal_longan_port_vlanBasedIsolationEntry_set;
    pMapper->port_vlanBasedIsolationEgrBypass_get = dal_longan_port_vlanBasedIsolationEgrBypass_get;
    pMapper->port_vlanBasedIsolationEgrBypass_set = dal_longan_port_vlanBasedIsolationEgrBypass_set;

    /* port related function */
    pMapper->port_init = dal_longan_port_init;
    pMapper->port_link_get = dal_longan_port_link_get;
    pMapper->port_speedDuplex_get = dal_longan_port_speedDuplex_get;
    pMapper->port_flowctrl_get = dal_longan_port_flowctrl_get;
    pMapper->port_cpuPortId_get = dal_longan_port_cpuPortId_get;
    pMapper->port_adminEnable_get = dal_longan_port_adminEnable_get;
    pMapper->port_adminEnable_set = dal_longan_port_adminEnable_set;
    pMapper->port_backpressureEnable_get = dal_longan_port_backpressureEnable_get;
    pMapper->port_backpressureEnable_set = dal_longan_port_backpressureEnable_set;
    pMapper->port_linkChange_register = dal_longan_port_linkChange_register;
    pMapper->port_linkChange_unregister = dal_longan_port_linkChange_unregister;
    pMapper->port_txEnable_get = dal_longan_port_txEnable_get;
    pMapper->port_txEnable_set = dal_longan_port_txEnable_set;
    pMapper->port_rxEnable_get = dal_longan_port_rxEnable_get;
    pMapper->port_rxEnable_set = dal_longan_port_rxEnable_set;
    pMapper->port_specialCongest_set = dal_longan_port_specialCongest_set;
    pMapper->port_flowCtrlEnable_get = dal_longan_port_flowCtrlEnable_get;
    pMapper->port_flowCtrlEnable_set = dal_longan_port_flowCtrlEnable_set;
    pMapper->port_linkMedia_get = dal_longan_port_linkMedia_get;
    pMapper->port_fiberUnidirEnable_get = dal_longan_port_fiberUnidirEnable_get;
    pMapper->port_fiberUnidirEnable_set = dal_longan_port_fiberUnidirEnable_set;
    pMapper->port_phyAutoNegoEnable_set = dal_longan_port_phyAutoNegoEnable_set;
    pMapper->port_phyForceModeAbility_set = dal_longan_port_phyForceModeAbility_set;

    pMapper->_port_macForceLink_get = _dal_longan_port_macForceLink_get;
    pMapper->_port_macForceLink_set = _dal_longan_port_macForceLink_set;

    return RT_ERR_OK;
}


/* Function Name:
 *      _dal_longan_port_smiMacType_set
 * Description:
 *      configure SMI MAC Type
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      value         - MAC-Type register value
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int32
_dal_longan_port_smiMacType_set(uint32 unit, uint32 port, uint32 value)
{
    int32       ret;
    uint32      field;

    switch(port)
    {
        case 0 ... 3:
            field = LONGAN_MAC_P3_P0_TYPEf;
            break;
        case 4 ... 7:
            field = LONGAN_MAC_P7_P4_TYPEf;
            break;
        case 8 ... 11:
            field = LONGAN_MAC_P11_P8_TYPEf;
            break;
        case 12 ... 15:
            field = LONGAN_MAC_P15_P12_TYPEf;
            break;
        case 16 ... 19:
            field = LONGAN_MAC_P19_P16_TYPEf;
            break;
        case 20 ... 23:
            field = LONGAN_MAC_P23_P20_TYPEf;
            break;
        case 24:
            field = LONGAN_MAC_P24_TYPEf;
            break;
        case 25:
            field = LONGAN_MAC_P25_TYPEf;
            break;
        case 26:
            field = LONGAN_MAC_P26_TYPEf;
            break;
        case 27:
            field = LONGAN_MAC_P27_TYPEf;
            break;
        default:
            RT_LOG(LOG_DEBUG, (MOD_HAL), "unit %u macId %u error macId", unit, port);
            return RT_ERR_INPUT;
    }

    if ((ret = reg_field_write(unit, LONGAN_SMI_MAC_TYPE_CTRLr, field, &value)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_HAL), "unit %u macId %u set SMI_MAC_TYPE_CTRL %u fail %d", unit, port, value, ret);
    }
    return ret;
}

/* Function Name:
 *      _dal_longan_port_smiPollSel_set
 * Description:
 *      configure SMI Poll Select
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      value         - POLL_SEL register value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_dal_longan_port_smiPollSel_set(uint32 unit, uint32 smi_id, uint32 poll_sel)
{
    uint32      field;

    switch (smi_id)
    {
      case 0:
        field = LONGAN_SMI0_POLL_SELf;
        break;
      case 1:
        field = LONGAN_SMI1_POLL_SELf;
        break;
      case 2:
        field = LONGAN_SMI2_POLL_SELf;
        break;
      case 3:
        field = LONGAN_SMI3_POLL_SELf;
        break;
      default:
        RT_LOG(LOG_DEBUG, (MOD_HAL), "unit %u smi_id %u wrong", unit, smi_id);
        return RT_ERR_INPUT;
    }
    return reg_field_write(unit, LONGAN_SMI_GLB_CTRLr, field, &poll_sel);
}

/* Function Name:
 *      _dal_longan_port_smiPollSel_init
 * Description:
 *      Initialize SMI Polling attributes
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
void
_dal_longan_port_smiPollSel_init(uint32 unit)
{
    uint32      port, poll_sel, data;
    int32       ret;
    uint32      is_phyType_cust1 = FALSE;
    rtk_portmask_t prv_poll_pm;

    RTK_PORTMASK_RESET(prv_poll_pm);

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        /* configure smi private polling for giga/2.5g lite */
        switch (HWP_PHY_MODEL_BY_PORT(unit, port))
        {
          case RTK_PHYTYPE_RTL8214C:
          case RTK_PHYTYPE_RTL8212B:
          case RTK_PHYTYPE_RTL8214B:
          case RTK_PHYTYPE_RTL8214FB:
          case RTK_PHYTYPE_RTL8218B:
          case RTK_PHYTYPE_RTL8218FB:
          case RTK_PHYTYPE_RTL8214FC:
          case RTK_PHYTYPE_RTL8218D:
          case RTK_PHYTYPE_RTL8284:
            RTK_PORTMASK_PORT_SET(prv_poll_pm, port);
            break;
          default:
            break;
        }

        data = RTK_PORTMASK_WORD_GET(prv_poll_pm, 0);
        if ((ret = reg_field_write(unit, LONGAN_SMI_PRVTE_POLLING_CTRLr, LONGAN_SMI_PRVTE_POLLING27_0f, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        }

        /* configure mac type */
        switch (HWP_PHY_MODEL_BY_PORT(unit, port))
        {
          case RTK_PHYTYPE_RTL8295R_C22:
            /* configure to polling SerDes */
            _dal_longan_port_smiMacType_set(unit, port, 0x0); /* 0:10G/1G Fiber; 1:10G/2.5G PHY */
            break;
          default:
            break;
        }


        if (port != HWP_PHY_BASE_MACID(unit, port))
        {
            continue;
        }
        /* configure poll_sel */
        switch (HWP_PHY_MODEL_BY_PORT(unit, port))
        {
          case RTK_PHYTYPE_CUST1:
            is_phyType_cust1 = TRUE;
            poll_sel = 0;   /* STD */
            break;
          case RTK_PHYTYPE_RTL8295R:
            poll_sel = 1;   /* resolution reg */
            break;
          case RTK_PHYTYPE_RTL8284:
          case RTK_PHYTYPE_RTL8226:
            poll_sel = 0;   /* STD */
            break;
          default:
            continue;
        }
        if ((ret = _dal_longan_port_smiPollSel_set(unit, HWP_PORT_SMI(unit, port), poll_sel)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "unit %u macId %u SMI%u pollSel:%u", unit, port, HWP_PORT_SMI(unit, port), poll_sel);
        }
    } /* end HWP_PORT_TRAVS_EXCEPT_CPU */

    if (is_phyType_cust1 == TRUE)
    {
        data = 0x01010000;
        if ((ret = reg_write(unit, LONGAN_SMI_10GPHY_POLLING_REG0_CFGr, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "unit %u set SMI_10GPHY_POLLING_REG0_CFG fail %x", unit, ret);
        }

        data = 0x01E7C400;
        if ((ret = reg_write(unit, LONGAN_SMI_10GPHY_POLLING_REG9_CFGr, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "unit %u set SMI_10GPHY_POLLING_REG9_CFG fail %x", unit, ret);
        }

        data = 0x01E7E820;
        if ((ret = reg_write(unit, LONGAN_SMI_10GPHY_POLLING_REG10_CFGr, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "unit %u set SMI_10GPHY_POLLING_REG0 fail %x", unit, ret);
        }
    }
    else
    {
        data = 0x11FA400; /* 31.0xA400.8 */
        if ((ret = reg_write(unit, LONGAN_SMI_10GPHY_POLLING_REG0_CFGr, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "unit %u set LONGAN_SMI_10GPHY_POLLING_REG0_CFGr fail %x", unit, ret);
        }

        data = 0x13FA412; /* 31.0xA412.9 */
        if ((ret = reg_write(unit, LONGAN_SMI_10GPHY_POLLING_REG9_CFGr, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "unit %u set SMI_10GPHY_POLLING_REG9_CFG fail %x", unit, ret);
        }

        data = 0x17FA414; /* 31.0xA414.11 */
        if ((ret = reg_write(unit, LONGAN_SMI_10GPHY_POLLING_REG10_CFGr, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "unit %u set SMI_10GPHY_POLLING_REG0 fail %x", unit, ret);
        }
    }

}


/* Function Name:
 *      dal_longan_port_init
 * Description:
 *      Initialize port module of the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Module must be initialized before using all of APIs in this module
 */
int32
dal_longan_port_init(uint32 unit)
{
    int32   ret = RT_ERR_OK;

    RT_INIT_REENTRY_CHK(port_init[unit]);
    port_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    port_sem[unit] = osal_sem_mutex_create();
    if (0 == port_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PORT), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    mac_force_sem[unit] = osal_sem_mutex_create();
    if (0 == mac_force_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PORT), "mac force semaphore create failed");
        return RT_ERR_FAILED;
    }

    pMac_info[unit] = (dal_longan_mac_info_t *)osal_alloc(sizeof(dal_longan_mac_info_t));
    if (NULL == pMac_info[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PORT), "memory allocate failed");
        return RT_ERR_FAILED;
    }
    osal_memset(pMac_info[unit], 0, sizeof(dal_longan_mac_info_t));

    /* init callback function for link change */
    link_change_callback_f[unit] = 0;

    /* set init flag to complete init */
    port_init[unit] = INIT_COMPLETED;

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        /* configure SMI POLL SEL */
        _dal_longan_port_smiPollSel_init(unit);

        /* Initial CPU port MAC ID */
        if ((ret = dal_longan_port_cpuPortId_set(unit, (rtk_port_t)HWP_CPU_MACID(unit))) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }
    }

#if !defined(__BOOTLOADER__)
    /******************Port Settings Init***********************/
    if (( ret = _dal_longan_port_init_config(unit)) != RT_ERR_OK)
    {
        port_init[unit] = INIT_NOT_COMPLETED;
        osal_free(pMac_info[unit]);
        pMac_info[unit] = NULL;
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "Port default configuration init failed");
        return ret;
    }
#endif


#if !defined(__BOOTLOADER__)
    if (HWP_UNIT_VALID_LOCAL(unit))
    /* software MAC-Poll-PHY and Change-Serdes-Mode */
    {
        uint32          port, port_cnt = 0;
        rtk_enable_t    enable = DISABLED;
        rtk_portmask_t  scan_portmask;

        RTK_PORTMASK_RESET(scan_portmask);
        dal_linkMon_swScanPorts_get(unit, &scan_portmask);

        HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
        {
            if ( ((_dal_longan_port_swMacPollPhyEnable_get(unit, port, &enable) == RT_ERR_OK) && (enable == ENABLED)) ||
                 ((_dal_longan_port_swChgSerdesModeEnable_get(unit, port, &enable) == RT_ERR_OK) && (enable == ENABLED)) )
            {
                RTK_PORTMASK_PORT_SET(scan_portmask, port);
                port_cnt++;
            }
        }
        if (port_cnt > 0)
        {
            dal_linkMon_swScanPorts_set(unit, &scan_portmask);
            dal_linkMon_enable(1000000);
        }
    }
#endif /* __BOOTLOADER__ */

    return ret;
}/* end of dal_longan_port_init */

/* Function Name:
 *      _dal_longan_port_macForceLink_get
 * Description:
 *      Get mac force link status
 * Input:
 *      unit    - unit id
 *      port   - port ID
 * Output:
 *      pEnable   - force enable
 *      pLinkSts    - link status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      when mac force is disable,link status is not affected
 */
int32
_dal_longan_port_macForceLink_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable,rtk_port_linkStatus_t *pLinkSts)
{
    int ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pLinkSts), RT_ERR_NULL_POINTER);

    MAC_FORCE_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit,
                          LONGAN_MAC_FORCE_MODE_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          LONGAN_MAC_FORCE_ENf,
                          &val)) != RT_ERR_OK)
    {
        MAC_FORCE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    *pEnable = (val == 1) ? ENABLED : DISABLED;

    if ((ret = reg_array_field_read(unit,
                          LONGAN_MAC_FORCE_MODE_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          LONGAN_FORCE_LINK_ENf,
                          &val)) != RT_ERR_OK)
    {
        MAC_FORCE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    *pLinkSts = (val == 1) ? PORT_LINKUP : PORT_LINKDOWN;

    MAC_FORCE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of _dal_longan_port_macForceLink_get */


/* Function Name:
 *      _dal_longan_port_macForceLink_set
 * Description:
 *      configure mac force link status
 * Input:
 *      unit    - unit id
 *      port   - port ID
 *      forceEn   - force enable
 *      linkSts    - link status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
_dal_longan_port_macForceLink_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable,rtk_port_linkStatus_t linkSts)
{
    rtk_port_speed_t  speed;
    uint32  val,spd_sel;
    int ret;

    RT_LOG(LOG_INFO, (MOD_DAL|MOD_PORT), "unit=%d, port=%d forceEn=%u linkSts=%u", unit, port, enable, linkSts);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable != DISABLED && enable != ENABLED), RT_ERR_INPUT);

    if((ret =phy_9300_speed_get( unit, port, &speed)) != RT_ERR_OK)
    {
        return ret;
    }

    switch(speed)
    {
        case PORT_SPEED_1000M:
            spd_sel = 2;
            break;
        case PORT_SPEED_10G:
            spd_sel = 4;
            break;
        case PORT_SPEED_2_5G:
            spd_sel = 5;
            break;
        default:
            spd_sel = 2;
            break;
    }

    MAC_FORCE_SEM_LOCK(unit);

    if ((ret = reg_array_read(unit,
                          LONGAN_MAC_FORCE_MODE_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          &val)) != RT_ERR_OK)
    {
        MAC_FORCE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if(enable == ENABLED)
    {
        val |= (0x1<<0);

        val &= ~(0x7<<3);
        val |= (spd_sel<<3);
    }
    else
    {
        val &= ~(0x1<<0);
    }

    if(PORT_LINKUP == linkSts)
        val |= (0x1<<1);
    else
        val &= ~(0x1<<1);

    if ((ret = reg_array_write(unit,
                          LONGAN_MAC_FORCE_MODE_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          &val)) != RT_ERR_OK)
    {
        MAC_FORCE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    MAC_FORCE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of _dal_longan_port_macForceLink_set */

/* Function Name:
 *      dal_longan_port_link_get
 * Description:
 *      Get the link status of the specific port
 * Input:
 *      unit                  - unit id
 *      port                  - port id
 * Output:
 *      pStatus              - pointer to the link status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID         - invalid port id
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      The link status of the port is as following:
 *      - LINKDOWN
 *      - LINKUP
 */
int32
dal_longan_port_link_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus)
{
    int32   ret;
    uint32  val;
    rtk_enable_t    swMacPollPhyEn;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    PORT_SEM_LOCK(unit);

    if ((_dal_longan_port_swMacPollPhyEnable_get(unit, port, &swMacPollPhyEn) == RT_ERR_OK) && (swMacPollPhyEn == ENABLED))
    {
        if (PHY_RESL_STS_LINK(pMac_info[unit]->swMacPollPhyStatus[port].reslStatus) == PORT_LINKUP)
            *pStatus = PORT_LINKUP;
        else
            *pStatus = PORT_LINKDOWN;

        PORT_SEM_UNLOCK(unit);

        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pStatus=%d", *pStatus);
        return RT_ERR_OK;
    }

    /* get value from CHIP*/
    /* read twice for latency value */
    reg_field_read(unit, LONGAN_MAC_LINK_STSr, LONGAN_LINK_STS_28_0f, &val);
    if ((ret = reg_field_read(unit, LONGAN_MAC_LINK_STSr, LONGAN_LINK_STS_28_0f, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    val = (val & (1 << port)) ? TRUE : FALSE;
    /* translate chip's value to definition */
    if (TRUE == val)
    {
        *pStatus = PORT_LINKUP;
    }
    else
    {
        *pStatus = PORT_LINKDOWN;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pStatus=%d", *pStatus);

    return RT_ERR_OK;
}/* end of dal_longan_port_link_get */

/* Function Name:
 *      dal_longan_port_txEnable_set
 * Description:
 *      Set the TX enable status of the specific port
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      enable        - enable status of TX
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The TX enable status of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
int32
dal_longan_port_txEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable != DISABLED && enable != ENABLED), RT_ERR_INPUT);

    if (ENABLED == enable)
    {
        val = 1;
    }
    else
    {
        val = 0;
    }

    PORT_SEM_LOCK(unit);

    /* programming value on CHIP*/
    if ((ret = reg_array_field_write(unit,
                          LONGAN_MAC_L2_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          LONGAN_TX_ENf,
                          &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}/* end of dal_longan_port_txEnable_set */

/* Function Name:
 *      dal_longan_port_rxEnable_set
 * Description:
 *      Set the RX enable status of the specific port
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      enable         - enable status of RX
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID   - invalid port id
 *      RT_ERR_INPUT     - invalid input parameter
 * Note:
 *      The RX enable status of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
int32
dal_longan_port_rxEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable != DISABLED && enable != ENABLED), RT_ERR_INPUT);

    if (ENABLED == enable)
    {
        val = 1;
    }
    else
    {
        val = 0;
    }

    PORT_SEM_LOCK(unit);

    /* programming value on CHIP*/
    if ((ret = reg_array_field_write(unit,
                          LONGAN_MAC_L2_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          LONGAN_RX_ENf,
                          &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}/* end of dal_longan_port_rxEnable_set */


/* Function Name:
 *      dal_longan_port_txEnable_get
 * Description:
 *      Get the TX enable status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to the port TX status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The TX enable status of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
int32
dal_longan_port_txEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                      LONGAN_MAC_L2_PORT_CTRLr,
                      port,
                      REG_ARRAY_INDEX_NONE,
                      LONGAN_TX_ENf,
                      &value)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    if (1 == value)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}/* end of dal_longan_port_txEnable_get */

/* Function Name:
 *      dal_longan_port_rxEnable_get
 * Description:
 *      Get the RX enable status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to the port RX status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The RX enable status of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
int32
dal_longan_port_rxEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                      LONGAN_MAC_L2_PORT_CTRLr,
                      port,
                      REG_ARRAY_INDEX_NONE,
                      LONGAN_RX_ENf,
                      &value)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    if (1 == value)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}/* end of dal_longan_port_rxEnable_get */

/* Function Name:
 *      dal_longan_port_specialCongest_set
 * Description:
 *      Set the congest seconds of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      second - congest timer (seconds)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32
dal_longan_port_specialCongest_set(uint32 unit, rtk_port_t port, uint32 second)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, second=%d", unit, port, second);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((second > RTK_PORT_SPEC_CONGEST_TIME_MAX), RT_ERR_OUT_OF_RANGE);

    PORT_SEM_LOCK(unit);

    /* set value from CHIP*/
    if ((ret = reg_array_field_write(unit, LONGAN_SC_P_ENr, port, REG_ARRAY_INDEX_NONE, LONGAN_CNGST_SUST_TMR_LMTf, &second)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_port_specialCongest_set */

/* Function Name:
 *      dal_longan_port_speedDuplex_get
 * Description:
 *      Get the negotiated port speed and duplex status of the specific port
 * Input:
 *      unit                  - unit id
 *      port                  - port id
 * Output:
 *      pSpeed               - pointer to the port speed
 *      pDuplex              - pointer to the port duplex
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID         - invalid port id
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_PORT_LINKDOWN   - link down port status
 * Note:
 *      1. The speed type of the port is as following:
 *         - PORT_SPEED_10M
 *         - PORT_SPEED_100M
 *         - PORT_SPEED_1000M
 *
 *      2. The duplex mode of the port is as following:
 *         - HALF_DUPLEX
 *         - FULL_DUPLEX
 */
int32
dal_longan_port_speedDuplex_get(
    uint32            unit,
    rtk_port_t        port,
    rtk_port_speed_t  *pSpeed,
    rtk_port_duplex_t *pDuplex)
{
    int32   ret;
    uint32  speed;
    uint32  duplex;
    rtk_port_linkStatus_t  link_status;
    rtk_enable_t    swMacPollPhyEn;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDuplex), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    /* Check Link status */
    if ((ret = dal_longan_port_link_get(unit, port, &link_status)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* if link status is down, should not process anymore and return error */
    if (PORT_LINKDOWN == link_status)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "port link down");
        return RT_ERR_PORT_LINKDOWN;
    }

    PORT_SEM_LOCK(unit);

    if ((_dal_longan_port_swMacPollPhyEnable_get(unit, port, &swMacPollPhyEn) == RT_ERR_OK) && (swMacPollPhyEn == ENABLED))
    {
        *pDuplex = PHY_RESL_STS_DUPLEX(pMac_info[unit]->swMacPollPhyStatus[port].reslStatus);
        *pSpeed = PHY_RESL_STS_SPEED(pMac_info[unit]->swMacPollPhyStatus[port].reslStatus);
        PORT_SEM_UNLOCK(unit);
        return RT_ERR_OK;
    }

    /* get speed value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          LONGAN_MAC_LINK_SPD_STSr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          LONGAN_SPD_STS_28_0f,
                          &speed)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* get duplex value from CHIP*/
    if ((ret = reg_field_read(unit,
                              LONGAN_MAC_LINK_DUP_STSr,
                              LONGAN_DUP_STS_28_0f,
                              &duplex)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    duplex = (duplex & (1 << port)) ? 1 : 0;

    PORT_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    if (1 == duplex)
    {
        *pDuplex = PORT_FULL_DUPLEX;
    }
    else
    {
        *pDuplex = PORT_HALF_DUPLEX;
    }

    /* extract port's speed value */
    switch (speed)
    {
        case 0x0:
            *pSpeed = PORT_SPEED_10M;
            break;
        case 0x1:
            *pSpeed = PORT_SPEED_100M;
            break;
        case 0x2:
        case 0x7:
            *pSpeed = PORT_SPEED_1000M;
            break;
        case 0x3:
            *pSpeed = PORT_SPEED_500M;
            break;
        case 0x4:
            *pSpeed = PORT_SPEED_10G;
            break;
        case 0x5:
        case 0x8:
            *pSpeed = PORT_SPEED_2_5G;
            break;
        case 0x6:
            *pSpeed = PORT_SPEED_5G;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pSpeed=%d, pDuplex=%d",
           *pSpeed, *pDuplex);

    return RT_ERR_OK;
}/* end of dal_longan_port_speedDuplex_get */

/* Function Name:
 *      dal_longan_port_flowctrl_get
 * Description:
 *      Get the negotiated flow control status of the specific port
 * Input:
 *      unit         - unit id
 *      port         - port id
 * Output:
 *      pTxStatus     - pointer to the negotiation result of the Tx flow control
 *      pRxStatus     - pointer to the negotiation result of the Rx flow control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID         - invalid port id
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_PORT_LINKDOWN   - link down port status
 * Note:
 *      None
 */
int32
dal_longan_port_flowctrl_get(
    uint32            unit,
    rtk_port_t        port,
    uint32            *pTxStatus,
    uint32            *pRxStatus)
{
    int32   ret;
    uint32  rxPause, txPause, val;
    rtk_port_linkStatus_t  link_status;
    rtk_enable_t    swMacPollPhyEn;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTxStatus), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRxStatus), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    /* Check Link status */
    if ((ret = dal_longan_port_link_get(unit, port, &link_status)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* if link status is down, should not process anymore and return error */
    if (PORT_LINKDOWN == link_status)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "port link down");
        return RT_ERR_PORT_LINKDOWN;
    }

    PORT_SEM_LOCK(unit);

    if ((_dal_longan_port_swMacPollPhyEnable_get(unit, port, &swMacPollPhyEn) == RT_ERR_OK) && (swMacPollPhyEn == ENABLED))
    {
        *pTxStatus = PHY_RESL_STS_TXPAUSE(pMac_info[unit]->swMacPollPhyStatus[port].reslStatus);
        *pRxStatus = PHY_RESL_STS_RXPAUSE(pMac_info[unit]->swMacPollPhyStatus[port].reslStatus);
        PORT_SEM_UNLOCK(unit);
        return RT_ERR_OK;
    }


    if ((ret = reg_field_read(unit, LONGAN_MAC_RX_PAUSE_STSr, LONGAN_RX_PAUSE_STS_28_0f, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    rxPause = (val & (1 << port)) ? 1 : 0;

    if ((ret = reg_field_read(unit, LONGAN_MAC_TX_PAUSE_STSr, LONGAN_TX_PAUSE_STS_28_0f, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    txPause = (val & (1 << port)) ? 1 : 0;

    PORT_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    if(1 == txPause)
    {
        *pTxStatus = ENABLED;
    }
    else
    {
        *pTxStatus = DISABLED;
    }

    if(1 == rxPause)
    {
        *pRxStatus = ENABLED;
    }
    else
    {
        *pRxStatus = DISABLED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pTxStatus=%d, pRxStatus=%d",
           *pTxStatus, *pRxStatus);

    return RT_ERR_OK;
}/* end of dal_longan_port_flowctrl_get */

/* Function Name:
 *      dal_longan_port_cpuPortId_get
 * Description:
 *      Get CPU port id of the specific unit
 * Input:
 *      unit                 - unit id
 * Output:
 *      pPort               - pointer to CPU port id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_port_cpuPortId_get(uint32 unit, rtk_port_t *pPort)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPort), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, LONGAN_MAC_L2_CPU_PORT_CTRLr, LONGAN_CPU_PORTf, &value)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    switch (value)
    {
        case 0:
            *pPort = 28;
            break;
        case 1:
            *pPort = 27;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pPort=%d", *pPort);

    return RT_ERR_OK;
}/* end of dal_longan_port_cpuPortId_get */

/* Function Name:
 *      dal_longan_port_cpuPortId_set
 * Description:
 *      Set CPU port id of the specific unit
 * Input:
 *      unit - unit id
 *      port - CPU port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      None
 */
int32
dal_longan_port_cpuPortId_set(uint32 unit, rtk_port_t port)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    switch (port)
    {
        case 27:
            value = 1;
            break;
        case 28:
            value = 0;
            break;
        default:
            return RT_ERR_PORT_ID;
    }

    PORT_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, LONGAN_MAC_L2_CPU_PORT_CTRLr, LONGAN_CPU_PORTf, &value)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "port=%d", port);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_port_isolation_get
 * Description:
 *      Get the portmask of the port isolation
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pPortmask          - pointer to the portmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      1. Default value of each port is 1
 *      2. Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
dal_longan_port_isolation_get(uint32 unit, rtk_port_t port, rtk_portmask_t *pPortmask)
{
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    port_iso_entry_t port_iso_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    index = HAL_UNIT_TO_DEV_ID(unit) * 64 + port;

    PORT_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = table_read(unit, LONGAN_PORT_ISO_CTRLt, index, (uint32 *)&port_iso_entry)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "return failed ret value = %x", ret);
        return ret;
    }

    if ((ret = table_field_get(unit, LONGAN_PORT_ISO_CTRLt, LONGAN_PORT_ISO_CTRL_P_ISO_MBR_0tf, &(pPortmask->bits[0]), (uint32 *) &port_iso_entry)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "return failed ret value = %x", ret);
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pPortmask=0x%X", (*pPortmask).bits[0]);

    return RT_ERR_OK;
}/* end of dal_longan_port_isolation_get */

/* Function Name:
 *      dal_longan_port_isolation_set
 * Description:
 *      Set the portmask of the port isolation
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      pPortmask       - pointer to the portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID   - invalid port id
 *      RT_ERR_PORT_MASK - invalid port mask
 * Note:
 *      1. Default value of each port is 1
 *      2. Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
dal_longan_port_isolation_set(uint32 unit, rtk_port_t port, rtk_portmask_t *pPortmask)
{
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    port_iso_entry_t port_iso_entry;

    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, pPortmask) , RT_ERR_PORT_MASK);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Portmask=0x%X", (*pPortmask).bits[0]);

    osal_memset(&port_iso_entry, 0, sizeof(port_iso_entry_t));
    index = HAL_UNIT_TO_DEV_ID(unit) * 64 + port;

    PORT_SEM_LOCK(unit);

    if ((ret = table_field_set(unit, LONGAN_PORT_ISO_CTRLt, LONGAN_PORT_ISO_CTRL_P_ISO_MBR_0tf, &(pPortmask->bits[0]), (uint32 *) &port_iso_entry)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "return failed ret value = %x", ret);
        return ret;
    }

    if ((ret = table_write(unit, LONGAN_PORT_ISO_CTRLt, index, (uint32 *)&port_iso_entry)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "return failed ret value = %x", ret);
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}/* end of dal_longan_port_isolation_set */

/* Function Name:
 *      dal_longan_port_isolationExt_get
 * Description:
 *      Get the portmask of the port isolation
 * Input:
 *      unit                - unit id
 *      devID               - source dev id
 *      srcPort             - source port id
 * Output:
 *      pPortmask           - pointer to the portmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      In stacking system, each (unit, port) pair should be assigned the permit outgoing portmask of this device.
 */
int32
dal_longan_port_isolationExt_get(uint32 unit, uint32 devID, rtk_port_t srcPort, rtk_portmask_t *pPortmask)
{
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    port_iso_entry_t port_iso_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, devID=%d, port=%d", unit, devID, srcPort);
    RT_PARAM_CHK((devID >= HAL_MAX_NUM_OF_DEV(unit)), RT_ERR_DEV_ID);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);

    index = devID * 64 + srcPort;

    PORT_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = table_read(unit, LONGAN_PORT_ISO_CTRLt, index, (uint32 *)&port_iso_entry)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "return failed ret value = %x", ret);
        return ret;
    }

    if ((ret = table_field_get(unit, LONGAN_PORT_ISO_CTRLt, LONGAN_PORT_ISO_CTRL_P_ISO_MBR_0tf, &(pPortmask->bits[0]), (uint32 *) &port_iso_entry)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "return failed ret value = %x", ret);
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Portmask=0x%X", (*pPortmask).bits[0]);

    return RT_ERR_OK;
}/* end of dal_longan_port_isolationExt_get */

/* Function Name:
 *      dal_longan_port_isolationExt_set
 * Description:
 *      Set the port isolation portmask for packet received from specified unit and port.
 * Input:
 *      unit            - unit id
 *      devID           - source dev id
 *      srcPort         - source port id
 *      pPortmask       - pointer to the portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID   - invalid port id
 *      RT_ERR_PORT_MASK - invalid port mask
 * Note:
 *      In stacking system, each (unit, port) pair should be assigned the permit outgoing portmask of this device.
 */
int32
dal_longan_port_isolationExt_set(uint32 unit, uint32 devID, rtk_port_t srcPort, rtk_portmask_t *pPortmask)
{
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    port_iso_entry_t port_iso_entry;

    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((devID >= HAL_MAX_NUM_OF_DEV(unit)), RT_ERR_DEV_ID);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, devID=%d, port=%d, Portmask=0x%X", unit, devID, srcPort, (*pPortmask).bits[0]);

    index = devID * 64 + srcPort;
    osal_memset(&port_iso_entry, 0, sizeof(port_iso_entry_t));

    PORT_SEM_LOCK(unit);

    if ((ret = table_field_set(unit, LONGAN_PORT_ISO_CTRLt, LONGAN_PORT_ISO_CTRL_P_ISO_MBR_0tf, &(pPortmask->bits[0]), (uint32 *) &port_iso_entry)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "return failed ret value = %x", ret);
        return ret;
    }

    if ((ret = table_write(unit, LONGAN_PORT_ISO_CTRLt, index, (uint32 *)&port_iso_entry)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "return failed ret value = %x", ret);
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}/* end of dal_longan_port_isolationExt_set */

/* Function Name:
 *      dal_longan_port_isolation_add
 * Description:
 *      Add an isolation port to the certain port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      iso_port      - isolation port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      1. Default value of each port is 1
 *      2. Port and iso_port will be isolated when this API is called
 *      3. The iso_port to the relative portmask bit will be set to 1
 *      4. This API can not use sem lock since it takes use of dal layer API
 */
int32
dal_longan_port_isolation_add(uint32 unit, rtk_port_t port, rtk_port_t iso_port)
{
    int32 ret = RT_ERR_FAILED;
    rtk_portmask_t  portmask;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, iso_port=%d",
           unit, port, iso_port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, iso_port), RT_ERR_PORT_ID);

    osal_memset(&portmask, 0, sizeof(rtk_portmask_t));

    ret = dal_longan_port_isolation_get(unit, port, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    RTK_PORTMASK_PORT_SET(portmask, iso_port);

    ret = dal_longan_port_isolation_set(unit, port, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
}/* end of dal_longan_port_isolation_add */

/* Function Name:
 *      dal_longan_port_isolation_del
 * Description:
 *      Delete an existing isolation port of the certain port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      iso_port      - isolation port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      1. Default value of each port is 1
 *      2. Isolated status between the port and the iso_port is removed when this API is called
 *      3. The iso_port to the relative portmask bit will be set to 0
 *      4. This API can not use sem lock since it takes use of dal layer API
 */
int32
dal_longan_port_isolation_del(uint32 unit, rtk_port_t port, rtk_port_t iso_port)
{
    int32 ret = RT_ERR_FAILED;
    rtk_portmask_t  portmask;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, iso_port=%d",
           unit, port, iso_port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, iso_port), RT_ERR_PORT_ID);

    osal_memset(&portmask, 0, sizeof(rtk_portmask_t));

    ret = dal_longan_port_isolation_get(unit, port, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    RTK_PORTMASK_PORT_CLEAR(portmask, iso_port);

    ret = dal_longan_port_isolation_set(unit, port, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
}/* end of dal_longan_port_isolation_del */

/* Function Name:
 *      dal_longan_port_isolationRestrictRoute_get
 * Description:
 *      Get status of routed packet whether restrict by port isolation
 * Input:
 *      unit     - unit id
 * Output:
 *      pEnable - pointer to enable status of port isolation restrict route
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      None
 */
int32
dal_longan_port_isolationRestrictRoute_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32 ret = RT_ERR_FAILED;

    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    PORT_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, LONGAN_PORT_ISO_RESTRICT_ROUTE_CTRLr, LONGAN_RESTRICT_ROUTEf, pEnable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_port_isolationRestrictRoute_set
 * Description:
 *      Set status of routed packet whether restrict by port isolation
 * Input:
 *      unit     - unit id
 *      enable - enable status of port isolaiton restrict route
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      None
 */
int32
dal_longan_port_isolationRestrictRoute_set(uint32 unit, rtk_enable_t enable)
{
    int32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d,enable=%d", unit,enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    PORT_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, LONGAN_PORT_ISO_RESTRICT_ROUTE_CTRLr, LONGAN_RESTRICT_ROUTEf, &enable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_port_adminEnable_get
 * Description:
 *      Get port admin status of the specific port
 * Input:
 *      unit               - unit id
 *      port               - port id
 *      pEnable           - pointer to the port admin status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_port_adminEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d",
           unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);

    *pEnable = pMac_info[unit]->admin_enable[port];

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_longan_port_adminEnable_get */

/* Function Name:
 *      dal_longan_port_adminEnable_set
 * Description:
 *      Set port admin configuration of the specific port
 * Input:
 *      unit               - unit id
 *      port               - port id
 *      enable             - port admin configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_port_adminEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, port admin=%d",
           unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);
    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable != DISABLED && enable != ENABLED), RT_ERR_INPUT);

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    PORT_SEM_LOCK(unit);

    if (ENABLED == enable)
    {
        value = 0x1;
        /* programming value on CHIP*/
        if ((ret = reg_array_field_write(unit,
                          LONGAN_MAC_L2_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          LONGAN_TX_ENf,
                          &value)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "port %u", port);
            return ret;
        }
        if ((ret = reg_array_field_write(unit,
                          LONGAN_MAC_L2_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          LONGAN_RX_ENf,
                          &value)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "port %u", port);
            return ret;
        }
    }

    if (HWP_PHY_EXIST(unit, port) || HWP_SERDES_PORT(unit, port))
    {
        if ((ret = dal_phy_portEnable_set(unit, port, enable)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "port %d", port);
            return ret;
        }
    }

    if (DISABLED == enable)
    {
        /* programming value on CHIP*/
        value = 0x0;
        if ((ret = reg_array_field_write(unit,
                          LONGAN_MAC_L2_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          LONGAN_TX_ENf,
                          &value)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "port %d", port);
            return ret;
        }

        if ((ret = reg_array_field_write(unit,
                          LONGAN_MAC_L2_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          LONGAN_RX_ENf,
                          &value)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "port %d", port);
            return ret;
        }
    }

    pMac_info[unit]->admin_enable[port] = enable;
    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_port_adminEnable_set */

/* Function Name:
 *      dal_longan_port_backpressureEnable_get
 * Description:
 *      Get the half duplex backpressure enable status of the specific port
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      pEnable        - pointer to the enable status of backpressure in half duplex mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      1. The mac local loopback enable status of the port is as following:
 *         - DISABLE
 *         - ENABLE
 *      2. Used to support backpressure in half mode.
 */
int32
dal_longan_port_backpressureEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d",
           unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          LONGAN_MAC_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          LONGAN_BKPRES_ENf,
                          pEnable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_longan_port_backpressureEnable_get */

/* Function Name:
 *      dal_longan_port_backpressureEnable_set
 * Description:
 *      Set the half duplex backpressure enable status of the specific port
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      enable        - enable status of backpressure in half duplex mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      1. The mac local loopback enable status of the port is as following:
 *         - DISABLE
 *         - ENABLE
 *      2. Used to support backpressure in half mode.
 */
int32
dal_longan_port_backpressureEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, enable=%d",
           unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable != DISABLED && enable != ENABLED), RT_ERR_INPUT);

    PORT_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit,
                          LONGAN_MAC_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          LONGAN_BKPRES_ENf,
                          &enable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_port_backpressureEnable_set */

/* Function Name:
 *      dal_longan_port_linkChange_register
 * Description:
 *      Register callback function for notification of link change
 * Input:
 *      unit           - unit id
 *      link_change_callback      - Callback function for link change
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
dal_longan_port_linkChange_register(uint32 unit, dal_link_change_callback_f link_change_callback)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, link_change_callback=%x",
           unit, link_change_callback);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == link_change_callback), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);

    link_change_callback_f[unit] = link_change_callback;

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* End of dal_longan_port_linkChange_register */

/* Function Name:
 *      dal_longan_port_linkChange_unregister
 * Description:
 *      Unregister callback function for notification of link change
 * Input:
 *      unit           - unit id
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 */
int32
dal_longan_port_linkChange_unregister(uint32 unit)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d",
           unit);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */

    PORT_SEM_LOCK(unit);

    link_change_callback_f[unit] = NULL;

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* End of dal_longan_port_linkChange_unregister */

#if !defined(__BOOTLOADER__)
/* Function Name:
 *      _dal_longan_port_init_config
 * Description:
 *      Initialize default configuration for port module of the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      1. Module must be initialized before using all of APIs in this module
 */
static int32
_dal_longan_port_init_config(uint32 unit)
{
    hal_control_t   *pHalCtrl;
    rtk_port_t      port;
    int32           ret;

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
            return RT_ERR_FAILED;

        HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
        {

            /* Config PHY in port that PHY exist */
            if ((HWP_PHY_EXIST(unit, port) || HWP_SERDES_PORT(unit, port)) && !HWP_CASCADE_PORT(unit, port))
            {
                if ((ret = dal_longan_port_adminEnable_set(unit, port, RTK_DEFAULT_PORT_ADMIN_ENABLE)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PORT), "Port%u init enable port failed", port);
                    /* continue the process */
                    continue;
                }
            }

            if(  (HWP_10GE_SERDES_PORT(unit, port) &&  !HWP_CASCADE_PORT(unit, port))
              || (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R))
            {
                if ((ret = _dal_longan_port_macForceLink_set(unit, port, ENABLED, PORT_LINKDOWN)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PORT), "10G Port%u init force linkdown failed", port);
                    /* continue the process */
                    continue;
                }
            }
        }
    }

    return RT_ERR_OK;
}/* end of _dal_longan_port_init_config */
#endif

/* Function Name:
 *      dal_longan_port_vlanBasedIsolationEntry_get
 * Description:
 *      Get VLAN-based port isolation entry
 * Input:
 *      unit   - unit id
 *      index  - index id
 * Output:
 *      pEntry - pointer to vlan-based port isolation entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
int32
dal_longan_port_vlanBasedIsolationEntry_get(uint32 unit, uint32 index, rtk_port_vlanIsolationEntry_t* pEntry)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index >= HAL_MAX_NUM_OF_VLAN_PORT_ISO_ENTRY(unit), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, LONGAN_PORT_ISO_VB_ISO_PMSK_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, LONGAN_VALIDf, &pEntry->enable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, LONGAN_PORT_ISO_VB_ISO_PMSK_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, LONGAN_VB_ISO_MBRf, &pEntry->portmask.bits[0])) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, LONGAN_PORT_ISO_VB_ISO_PMSK_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, LONGAN_VID_LOWERf, &pEntry->vid)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, LONGAN_PORT_ISO_VB_ISO_PMSK_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, LONGAN_VID_UPPERf, &pEntry->vid_high)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "enable=%d, lower vid=%d,high_vid=%d, pPortmask=0x%X",
        pEntry->enable, pEntry->vid,pEntry->vid_high,(pEntry->portmask).bits[0]);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_port_vlanBasedIsolationEntry_set
 * Description:
 *      Set VLAN-based port isolation entry
 * Input:
 *      unit   - unit id
 *      index  - index id
 *      pEntry - pointer to vlan-based port isolation entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_VLAN_VID     - invalid vid
 *      RT_ERR_PORT_VLAN_ISO_VID_EXIST_IN_OTHER_IDX - vid exists in other entry
 * Note:
 *      None
 */
int32
dal_longan_port_vlanBasedIsolationEntry_set(uint32 unit, uint32 index, rtk_port_vlanIsolationEntry_t* pEntry)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, index=%d", unit, index);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "enable=%d, vid=%d, pPortmask=0x%X",
        pEntry->enable, pEntry->vid, (pEntry->portmask).bits[0]);


    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index >= HAL_MAX_NUM_OF_VLAN_PORT_ISO_ENTRY(unit), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pEntry->enable >= RTK_ENABLE_END), RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &pEntry->portmask) , RT_ERR_PORT_MASK);

    PORT_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit, LONGAN_PORT_ISO_VB_ISO_PMSK_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, LONGAN_VALIDf, &pEntry->enable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, LONGAN_PORT_ISO_VB_ISO_PMSK_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, LONGAN_VB_ISO_MBRf, &pEntry->portmask.bits[0])) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, LONGAN_PORT_ISO_VB_ISO_PMSK_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, LONGAN_VID_LOWERf, &pEntry->vid)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, LONGAN_PORT_ISO_VB_ISO_PMSK_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, LONGAN_VID_UPPERf, &pEntry->vid_high)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_port_vlanBasedIsolationEgrBypass_get
 * Description:
 *      Get Egress Port Bypass Status  of VLAN-based port isolation
 * Input:
 *      unit     - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to egress port bypass status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_port_vlanBasedIsolationEgrBypass_get(uint32 unit, rtk_port_t port,rtk_enable_t *pEnable)
{
    int32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                          LONGAN_PORT_ISO_VB_EGR_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          LONGAN_VB_ISO_EGR_BYPASSf,
                          pEnable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "ret=0x%x", ret);
        return ret;
    }
    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_port_vlanBasedIsolationEgrBypass_set
 * Description:
 *      Set Egress Port Bypass Status  of VLAN-based port isolation
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of linkdown green feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_longan_port_vlanBasedIsolationEgrBypass_set(uint32 unit, rtk_port_t port,rtk_enable_t enable)
{
    int32  ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, enable=%d", unit, port,enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_NULL_POINTER);

    /* set value to CHIP*/
    PORT_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                          LONGAN_PORT_ISO_VB_EGR_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          LONGAN_VB_ISO_EGR_BYPASSf,
                          &enable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "ret=0x%x", ret);
        return ret;
    }
    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_port_flowCtrlEnable_get
 * Description:
 *      Get the flow control status of the specific port
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      pEnable - pointer to the status of the flow control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID       - invalid unit id
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      The API get the flow control status by port based, no matter N-WAY is enabled or disabled.
 *      This API can not use sem lock since it takes use of dal layer API
 */
int32
dal_longan_port_flowCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t    nway_enable, flowctrl_enable;
    rtk_port_phy_ability_t  ability;
    rtk_port_speed_t    speed;
    rtk_port_duplex_t   duplex;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pEnable == NULL), RT_ERR_NULL_POINTER);

    if ((ret = dal_phy_portAutoNegoEnable_get(unit, port, &nway_enable)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }

    if (ENABLED == nway_enable)
    {
        if ((ret = dal_phy_portAutoNegoAbility_get(unit, port, &ability)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
            return ret;
        }
        if (ability.FC && ability.AsyFC)
            (*pEnable) = ENABLED;
        else
            (*pEnable) = DISABLED;
    }
    else
    {
        if ((ret = dal_phy_portForceModeAbility_get(unit, port, &speed, &duplex, &flowctrl_enable)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
            return ret;
        }
        (*pEnable) = flowctrl_enable;
    }

    return RT_ERR_OK;
} /* end of dal_longan_port_flowCtrlEnable_get */

int32
_dal_longan_port_macForceFc_set(int32 unit, rtk_port_t port, rtk_enable_t forceFc,
    rtk_enable_t txPause, rtk_enable_t rxPause)
{
    int ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d forceFC=%u txP=%u rxP=%u", unit, port, forceFc, txPause, rxPause);

    val = (forceFc == ENABLED) ? 1 : 0;
    RT_ERR_CHK(reg_array_field_write(unit, LONGAN_MAC_FORCE_MODE_CTRLr, port,
            REG_ARRAY_INDEX_NONE, LONGAN_MAC_FORCE_FC_ENf, &val), ret);

    if (forceFc == ENABLED)
    {
        val = (txPause == ENABLED) ? 1 : 0;
        RT_ERR_CHK(reg_array_field_write(unit, LONGAN_MAC_FORCE_MODE_CTRLr,
                port, REG_ARRAY_INDEX_NONE, LONGAN_TX_PAUSE_ENf, &val), ret);

        val = (rxPause == ENABLED) ? 1 : 0;
        RT_ERR_CHK(reg_array_field_write(unit, LONGAN_MAC_FORCE_MODE_CTRLr,
                port, REG_ARRAY_INDEX_NONE, LONGAN_RX_PAUSE_ENf, &val), ret);
    }

    return RT_ERR_OK;
}   /* end of _dal_longan_port_macForceFc_set */

/* Function Name:
 *      dal_longan_port_flowCtrlEnable_set
 * Description:
 *      Set the flow control status to the specific port
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      enable    - enable status of flow control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID       - invalid unit id
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      The API is apply the flow control status by port based, no matter N-WAY is enabled or disabled.
 *      This API can not use sem lock since it takes use of dal layer API
 */
int32
dal_longan_port_flowCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtk_enable_t                flowctrl_enable;
    rtk_port_phy_ability_t      ability;
    rtk_port_speed_t            speed;
    rtk_port_duplex_t           duplex;
    rtk_port_flowctrl_mode_t    fcMode;
    rtk_enable_t                nway_enable;
    int32                       ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, enable=%d",
           unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable != DISABLED) && (enable != ENABLED), RT_ERR_INPUT);

    if ((ret = dal_phy_portAutoNegoAbility_get(unit, port, &ability)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }
    if (ENABLED == enable)
    {
        ability.FC = 1;
        ability.AsyFC = 1;
    }
    else
    {
        ability.FC = 0;
        ability.AsyFC = 0;
    }
    if ((ret = dal_phy_portAutoNegoAbility_set(unit, port, &ability)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }

    fcMode.tx_pause = enable;
    fcMode.rx_pause = enable;

    if ((ret = dal_phy_portForceFlowctrlMode_set(unit, port, &fcMode)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }

    if ((ret = dal_phy_portForceModeAbility_get(unit, port, &speed, &duplex, &flowctrl_enable)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }

    if ((ret = dal_phy_portForceModeAbility_set(unit, port, speed, duplex, enable)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }

    RT_ERR_CHK(dal_phy_portAutoNegoEnable_get(unit, port, &nway_enable), ret);
    if (DISABLED == nway_enable)
    {
        PORT_SEM_LOCK(unit);
        /* Force flowcontrol */
        RT_ERR_HDL(_dal_longan_port_macForceFc_set(unit, port, ENABLED,
                fcMode.tx_pause, fcMode.rx_pause), ERR, ret);
        PORT_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;

ERR:
    PORT_SEM_UNLOCK(unit);
    return ret;
} /* end of dal_longan_port_flowCtrlEnable_set */

/* Function Name:
 *      dal_longan_port_linkDownPowerSavingEnable_get
 * Description:
 *      Get the statue of link-down power saving of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of link-down power saving
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_port_linkDownPowerSavingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);
    if ((ret = dal_phy_portLinkDownPowerSavingEnable_get(unit, port, pEnable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }
    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_port_linkDownPowerSavingEnable_get */

/* Function Name:
 *      dal_longan_port_linkDownPowerSavingEnable_set
 * Description:
 *      Set the statue of link-down power saving of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of link-down power saving
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - input parameter out of range
 * Note:
 *      None
 */
int32
dal_longan_port_linkDownPowerSavingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, enable=%d",
           unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable != DISABLED && enable != ENABLED), RT_ERR_INPUT);

    /* Configure if PHY supported green feature */
    PORT_SEM_LOCK(unit);
    if ((ret = dal_phy_portLinkDownPowerSavingEnable_set(unit, port, enable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }
    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_port_linkDownPowerSavingEnable_set */

/* Function Name:
 *      dal_longan_port_linkMedia_get
 * Description:
 *      Get link status and media
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of link-down power saving
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - input parameter out of range
 * Note:
 *      None
 */
int32
dal_longan_port_linkMedia_get(uint32 unit, rtk_port_t port,
    rtk_port_linkStatus_t *pStatus, rtk_port_media_t *pMedia)
{
    uint32  val;
    int32    ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    PORT_SEM_LOCK(unit);

    /* link status need to get twice */
    if ((ret = reg_field_read(unit, LONGAN_MAC_LINK_STSr, LONGAN_LINK_STS_28_0f, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, LONGAN_MAC_LINK_STSr, LONGAN_LINK_STS_28_0f, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if (0 == (val & (1UL<<port)))
    {
        *pStatus = PORT_LINKDOWN;
        /* media should be ignored when link down */
        /*pMedia = PORT_MEDIA_COPPER;*/
    }
    else
    {
        *pStatus = PORT_LINKUP;

        if ((ret = reg_field_read(unit, LONGAN_MAC_LINK_MEDIA_STSr, LONGAN_MEDIA_STS_28_0f, &val)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if (0 == (val & (1UL<<port)))
            *pMedia = PORT_MEDIA_COPPER;
        else
            *pMedia = PORT_MEDIA_FIBER;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_port_linkMedia_get */

/* Function Name:
 *      dal_longan_port_fiberUnidirEnable_get
 * Description:
 *      Get fiber unidirection enable status of the specific port
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      pEnable       - - pointer to the enable status of mac local loopback
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_longan_port_fiberUnidirEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32 val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d,port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!HWP_10GE_SERDES_PORT(unit, port) && !HWP_FIBER_PORT(unit, port)), RT_ERR_PORT_ID);

    /* function body */
    PORT_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, LONGAN_UNI_DIR_CTRLr, LONGAN_FIB_UNIDIR_ENf, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if (0 == (val & (1UL<<port)))
        *pEnable = DISABLED;
    else
        *pEnable = ENABLED;

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_port_fiberUnidirEnable_get */

/* Function Name:
 *      dal_longan_port_fiberUnidirEnable_set
 * Description:
 *      Set fiber unidirection enable status of the specific port
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      enable        - enable status of mac local loopback
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_longan_port_fiberUnidirEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32 val;
    uint32 data;
    uint32 spd_sel;
    rtk_port_speed_t speed;
    uint32 sds;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d,port=%d,enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);
    RT_PARAM_CHK((!HWP_10GE_SERDES_PORT(unit, port) && !HWP_FIBER_PORT(unit, port)), RT_ERR_PORT_ID);

    sds = HWP_PORT_SDSID(unit, port);
    if((ret =phy_speed_get( unit, port, &speed)) != RT_ERR_OK)
    {
        return ret;
    }

    switch(speed)
    {
        case PORT_SPEED_1000M:
            spd_sel = 2;
            break;
        case PORT_SPEED_10G:
            spd_sel = 4;
            break;
        default:
            spd_sel = 2;
            break;
    }

    /* function body */
    PORT_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, LONGAN_UNI_DIR_CTRLr, LONGAN_FIB_UNIDIR_ENf, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    if(HWP_10GE_SERDES_PORT(unit, port))
    {
        if(4 == spd_sel)
        {
            if(ENABLED == enable)
            {
                val |= 1UL << port;
                data = 1;
                SDS_FIELD_W(unit, sds, 6,29,0x7,0x7, data);
            }
            else
            {
                val &= ~(1UL << port);
                data = 0;
                SDS_FIELD_W(unit, sds, 6,29,0x7,0x7, data);
            }
        }
        else
        {
            if(ENABLED == enable)
            {
                val |= 1UL << port;
                data = 1;
                if((sds > 1) && (sds < 4))
                    reg_array_field_write(unit, LONGAN_SDS_INTF_CTRL0r,sds, REG_ARRAY_INDEX_NONE ,LONGAN_UNIDIR_TX_ABLE_S0f ,&data );
                else if((sds > 3) && (sds < 10))
                    reg_array_field_write(unit, LONGAN_SDS_INTF_CTRL1r,sds, REG_ARRAY_INDEX_NONE ,LONGAN_UNIDIR_TX_ABLE_S4f ,&data );


                SDS_FIELD_W(unit, sds, 2,0,0x5,0x5, data);
            }
            else
            {
                val &= ~(1UL << port);
                data = 0;
                if((sds > 1) && (sds < 4))
                    reg_array_field_write(unit, LONGAN_SDS_INTF_CTRL0r,sds ,REG_ARRAY_INDEX_NONE ,LONGAN_UNIDIR_TX_ABLE_S0f ,&data );
                else if((sds > 3) && (sds < 10))
                    reg_array_field_write(unit, LONGAN_SDS_INTF_CTRL1r,sds,REG_ARRAY_INDEX_NONE ,LONGAN_UNIDIR_TX_ABLE_S4f ,&data );


                SDS_FIELD_W(unit, sds, 2,0,0x5,0x5, data);
            }
        }
    }
    else
    {
        if ((ret = dal_phy_fiberUnidirEnable_set(unit, port, enable)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "port %u", port);
            return ret;
        }
        if(ENABLED == enable)
        {
            val |= 1UL << port;
        }
        else
        {
            val &= ~(1UL << port);
        }
    }
    if ((ret = reg_field_write(unit, LONGAN_UNI_DIR_CTRLr, LONGAN_FIB_UNIDIR_ENf, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_port_fiberUnidirEnable_set */


/* Function Name:
 *      dal_longan_port_phyAutoNegoEnable_set
 * Description:
 *      Set PHY ability of the specific port
 * Input:
 *      unit                 - unit id
 *      port                 - port id
 *      enable               - enable PHY auto negotiation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID        - invalid port id
 *      RT_ERR_INPUT          - input parameter out of range
 * Note:
 *      1. ENABLED : switch to PHY auto negotiation mode
 *         DISABLED: switch to PHY force mode
 *      2. Once the abilities of both auto-nego and force mode are set,
 *         you can freely switch the mode without calling ability setting API again
 */
int32
dal_longan_port_phyAutoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtk_port_flowctrl_mode_t    fcMode;
    rtk_enable_t                flowctrl_enable;
    rtk_port_speed_t            speed;
    rtk_port_duplex_t           duplex;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%u, port=%u, enable=%d", unit, port, enable);

    RT_ERR_CHK(dal_phy_portAutoNegoEnable_set(unit, port, enable), ret);

    if (ENABLED == enable)
    {
        PORT_SEM_LOCK(unit);
        /* Disable Force flowcontrol */
        RT_ERR_HDL(_dal_longan_port_macForceFc_set(unit, port, DISABLED,
                ENABLED, ENABLED), ERR, ret);
        PORT_SEM_UNLOCK(unit);

    }
    else
    {
        RT_ERR_CHK(dal_phy_portForceFlowctrlMode_get(unit, port, &fcMode), ret);
        RT_ERR_CHK(dal_phy_portForceModeAbility_get(unit, port, &speed, &duplex,
                &flowctrl_enable), ret);

        if (flowctrl_enable == DISABLED)
        {
            fcMode.tx_pause = DISABLED;
            fcMode.rx_pause = DISABLED;
        }

        /* Force flowcontrol */
        PORT_SEM_LOCK(unit);
        RT_ERR_HDL(_dal_longan_port_macForceFc_set(unit, port, ENABLED,
                fcMode.tx_pause, fcMode.rx_pause), ERR, ret);
        PORT_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;

ERR:
    PORT_SEM_UNLOCK(unit);
    return ret;
}   /* end of dal_longan_port_phyAutoNegoEnable_set */

/* Function Name:
 *      dal_longan_port_phyForceModeAbility_set
 * Description:
 *      Set the port speed/duplex mode/pause/asy_pause in the PHY force mode
 * Input:
 *      unit                  - unit id
 *      port                  - port id
 *      speed                 - port speed
 *      duplex                - port duplex mode
 *      flowControl           - enable flow control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID         - invalid port id
 *      RT_ERR_PHY_SPEED       - invalid PHY speed setting
 *      RT_ERR_PHY_DUPLEX      - invalid PHY duplex setting
 *      RT_ERR_INPUT           - invalid input parameter
 * Note:
 *
 */
int32
dal_longan_port_phyForceModeAbility_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_speed_t    speed,
    rtk_port_duplex_t   duplex,
    rtk_enable_t        flowControl)
{
    rtk_enable_t nway_enable;
    rtk_port_flowctrl_mode_t    fcMode;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%u,port=%u,speed=%u,duplex=%u,flowControl=%u", unit, port, speed, duplex, flowControl);

    RT_ERR_CHK(dal_phy_portForceModeAbility_set(unit, port, speed, duplex, flowControl), ret);

    RT_ERR_CHK(dal_phy_portAutoNegoEnable_get(unit, port, &nway_enable), ret);

    if (DISABLED == nway_enable)
    {
        RT_ERR_CHK(dal_phy_portForceFlowctrlMode_get(unit, port, &fcMode), ret);

        if (flowControl == DISABLED)
        {
            fcMode.tx_pause = DISABLED;
            fcMode.rx_pause = DISABLED;
        }

        /* Force flowcontrol */
        PORT_SEM_LOCK(unit);
        RT_ERR_HDL(_dal_longan_port_macForceFc_set(unit, port, ENABLED,
                fcMode.tx_pause, fcMode.rx_pause), ERR, ret);
        PORT_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;

ERR:
    PORT_SEM_UNLOCK(unit);
    return ret;
}   /* end of dal_longan_port_phyForceModeAbility_set */



/* Function Name:
 *      _dal_longan_port_macForce_set
 * Description:
 *      configure force mode
 * Input:
 *      unit    - unit id
 *      sdsId   - serdes ID
 *      mode   - serdes mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_CHIP_NOT_SUPPORTED   - serdes mode is not supported by the chip
 * Note:
 *      None
 */
void
_dal_longan_port_macForceMode_set(uint32 unit, rtk_port_t port,
                    rtk_port_swMacPollPhyStatus_t *pphyStatus_prev,
                    rtk_port_swMacPollPhyStatus_t *pphyStatus)
{
    rtk_port_speed_t    speed;
    uint32              reg_data;
    uint32              val;
    int32               ret = 0;

    if (osal_memcmp(pphyStatus, pphyStatus_prev, sizeof(rtk_port_swMacPollPhyStatus_t)) == 0)
    {
        return;
    }

    if ((ret = reg_array_read(unit, LONGAN_MAC_FORCE_MODE_CTRLr, port, REG_ARRAY_INDEX_NONE, &reg_data)) != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_DAL|MOD_PORT), "unit=%u, port=%u, MAC_FORCE_MODE_CTRL get fail %x", unit, port, ret);
        return;
    }

    RT_LOG(LOG_INFO, (MOD_DAL|MOD_PORT), "unit=%u, port=%u, orgMacForceMode=0x%X", unit, port, reg_data);

    val = 1;
    ret = reg_field_set(unit, LONGAN_MAC_FORCE_MODE_CTRLr, LONGAN_MAC_FORCE_ENf, &val, &reg_data);
    RT_LOG(LOG_INFO, (MOD_DAL|MOD_PORT), "unit=%u, port=%u, MAC_FORCE_EN=%u data=0x%X ret=%x", unit, port, val, reg_data, ret);

    val = 1;
    reg_field_set(unit, LONGAN_MAC_FORCE_MODE_CTRLr, LONGAN_MAC_FORCE_FC_ENf, &val, &reg_data);
    RT_LOG(LOG_INFO, (MOD_DAL|MOD_PORT), "unit=%u, port=%u, MAC_FORCE_FC_EN=%u data=0x%X ret=%x", unit, port, val, reg_data, ret);

    val = (PHY_RESL_STS_LINK(pphyStatus->reslStatus) == PORT_LINKUP) ? 1 : 0;
    reg_field_set(unit, LONGAN_MAC_FORCE_MODE_CTRLr, LONGAN_FORCE_LINK_ENf, &val, &reg_data);
    RT_LOG(LOG_INFO, (MOD_DAL|MOD_PORT), "unit=%u, port=%u, FORCE_LINK_EN=%u data=0x%X ret=%x", unit, port, val, reg_data, ret);

    val = (PHY_RESL_STS_DUPLEX(pphyStatus->reslStatus) == PORT_FULL_DUPLEX) ? 1 : 0;
    reg_field_set(unit, LONGAN_MAC_FORCE_MODE_CTRLr, LONGAN_DUP_SELf, &val, &reg_data);
    RT_LOG(LOG_INFO, (MOD_DAL|MOD_PORT), "unit=%u, port=%u, DUP_SEL=%u data=0x%X ret=%x", unit, port, val, reg_data, ret);

    val = PHY_RESL_STS_TXPAUSE(pphyStatus->reslStatus);
    reg_field_set(unit, LONGAN_MAC_FORCE_MODE_CTRLr, LONGAN_TX_PAUSE_ENf, &val, &reg_data);
    RT_LOG(LOG_INFO, (MOD_DAL|MOD_PORT), "unit=%u, port=%u, TX_PAUSE_EN=%u data=0x%X ret=%x", unit, port, val, reg_data, ret);

    val = PHY_RESL_STS_RXPAUSE(pphyStatus->reslStatus);
    reg_field_set(unit, LONGAN_MAC_FORCE_MODE_CTRLr, LONGAN_RX_PAUSE_ENf, &val, &reg_data);
    RT_LOG(LOG_INFO, (MOD_DAL|MOD_PORT), "unit=%u, port=%u, RX_PAUSE_EN=%u data=0x%X ret=%x", unit, port, val, reg_data, ret);

    val = (pphyStatus->media == PORT_MEDIA_FIBER) ? 1 : 0;
    reg_field_set(unit, LONGAN_MAC_FORCE_MODE_CTRLr, LONGAN_MEDIA_SELf, &val, &reg_data);
    RT_LOG(LOG_INFO, (MOD_DAL|MOD_PORT), "unit=%u, port=%u, MEDIA_SEL=%u data=0x%X ret=%x", unit, port, val, reg_data, ret);

#define FOR_AQ_DEBUG_PURPOSE_5G_2dot5G  1 /* this is request by AQ for their debug purpose, should remove later */

    speed = PHY_RESL_STS_SPEED(pphyStatus->reslStatus);
    switch(speed)
    {
      case PORT_SPEED_10G:
        val = 0x4;
        break;
      case PORT_SPEED_5G:
#if FOR_AQ_DEBUG_PURPOSE_5G_2dot5G
        {
            rt_serdesMode_t pserdesMode;
            dal_phy_portMacIntfSerdesMode_get(unit,port,&pserdesMode);
            if (RTK_MII_10GR == pserdesMode)
            {
                val = 0x4;
            }
            else
            {
                val = 0x6;
            }
        }
#else
        val = 0x6;
#endif
        break;
      case PORT_SPEED_2_5G:
#if FOR_AQ_DEBUG_PURPOSE_5G_2dot5G
        {
            rt_serdesMode_t pserdesMode;
            dal_phy_portMacIntfSerdesMode_get(unit,port,&pserdesMode);
            if (RTK_MII_10GR == pserdesMode)
            {
                val = 0x4;
            }
            else
            {
                val = 0x5;
            }
        }
#else
        val = 0x5;
#endif
        break;
      case PORT_SPEED_1000M:
        val = 0x2;
        break;
      case PORT_SPEED_500M:
        val = 0x3;
        break;
      case PORT_SPEED_100M:
        val = 0x1;
        break;
      case PORT_SPEED_10M:
        val = 0x0;
        break;
      default:
        val = 0x0;
        break;
    }
    reg_field_set(unit, LONGAN_MAC_FORCE_MODE_CTRLr, LONGAN_SPD_SELf, &val, &reg_data);
    RT_LOG(LOG_INFO, (MOD_DAL|MOD_PORT), "unit=%u, port=%u, SPD_SEL=%u data=0x%X ret=%x", unit, port, val, reg_data, ret);

    RT_LOG(LOG_INFO, (MOD_DAL|MOD_PORT), "unit=%u, port=%u, MacForceMode=0x%X resl=0x%X", unit, port, reg_data, pphyStatus->reslStatus);

    reg_array_write(unit, LONGAN_MAC_FORCE_MODE_CTRLr, port, REG_ARRAY_INDEX_NONE, &reg_data);

}


/* Function Name:
 *      _dal_longan_port_swMacPollPhyEnable_get
 * Description:
 *      Enable status of Software Polling of the port
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32
_dal_longan_port_swMacPollPhyEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *penable)
{
#if defined(CONFIG_SDK_RTL8295R) /* 95R MAC-POLL-PHY */
    if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R_C22)
    {
        *penable = ENABLED;
    }
    else
#endif /* CONFIG_SDK_RTL8295R */
#if defined(CONFIG_SDK_PHY_CUST1)
    if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_CUST1)
    {
        if((RTL9301_CHIP_ID_24G == HWP_CHIP_ID(unit) || RTL9303_CHIP_ID_8XG == HWP_CHIP_ID(unit)))
        {
            *penable = ENABLED;
        }
        else
        {
            *penable = DISABLED;
        }
    }
    else
#endif /* CONFIG_SDK_PHY_CUST1 */
    {
        *penable = DISABLED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_longan_port_swMacPollPhy
 * Description:
 *      software polling PHY status
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32
_dal_longan_port_swMacPollPhy(uint32 unit, rtk_port_t port)
{
    rtk_enable_t    enable;
    int32           ret;
    rtk_port_swMacPollPhyStatus_t   phyStatus;

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    PORT_SEM_LOCK(unit);

    if (_dal_longan_port_swMacPollPhyEnable_get(unit, port, &enable) != RT_ERR_OK)
    {
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
        goto EXIT;
    }
    if (enable == DISABLED)
    {
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
        goto EXIT;
    }

    osal_memset(&phyStatus, 0, sizeof(rtk_port_swMacPollPhyStatus_t));
    if ((ret = _dal_phy_portSwMacPollPhyStatus_get(unit, port, &phyStatus)) != RT_ERR_OK)
    {
        goto EXIT;
    }

    /* MAC Force Mode */
    _dal_longan_port_macForceMode_set(unit, port, &pMac_info[unit]->swMacPollPhyStatus[port], &phyStatus);

    /* record the PHY status */
    osal_memcpy(&pMac_info[unit]->swMacPollPhyStatus[port], &phyStatus, sizeof(rtk_port_swMacPollPhyStatus_t));

EXIT:
    PORT_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      _dal_longan_port_swChgSerdesModeEnable_get
 * Description:
 *      Enable status of Software change serdes mode
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32
_dal_longan_port_swChgSerdesModeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *penable)
{
#if defined(CONFIG_SDK_PHY_CUST1)
    if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_CUST1)
    {
        uint32      sdsId = HWP_PORT_SDSID(unit, port);

        if ((sdsId < RTK_MAX_SDS_PER_UNIT)
            && ((HWP_SDS_MODE(unit, sdsId) == RTK_MII_10GR) || (HWP_SDS_MODE(unit, sdsId) == RTK_MII_10GR1000BX_AUTO)))
        {
            *penable = ENABLED;
        }
        else
        {
            *penable = DISABLED;
        }
    }
    else
#endif /* CONFIG_SDK_PHY_CUST1 */
#if defined(CONFIG_SDK_RTL8226)
    if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8226)
    {
        *penable = ENABLED;
    }
    else
#endif
    {
        *penable = DISABLED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_longan_port_swAutoDetectSdsMode
 * Description:
 *
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32
_dal_longan_port_swAutoDetectSdsMode(uint32 unit, rtk_port_t port)
{
    int32 ret;
    uint32 val;
    uint32  speed;
    uint32          sdsId;
    rt_serdesMode_t pMode;
    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    if (HWP_PHY_MODEL_BY_PORT(unit, port) != RTK_PHYTYPE_RTL8226)
        return RT_ERR_OK;

    sdsId = HWP_PORT_SDSID(unit, port);
    PORT_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, LONGAN_PHY_LINK_STSr, LONGAN_LINK_STS_27_0f, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    /*check phy status link down*/
    if(0 == (val & (1UL<<port)))
    {
        if ((ret = dal_longan_sds_mode_set(unit, sdsId, RTK_MII_DISABLE)) != RT_ERR_OK)
            {
                PORT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_SDS), "");
                return ret;
            }
    }
    else /*check phy status link up*/
    {
        if ((ret = reg_array_field_read(unit,LONGAN_PHY_LINK_SPD_STSr, port,REG_ARRAY_INDEX_NONE, LONGAN_SPD_STS_27_0f, &speed)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if ((ret = dal_longan_sds_mode_get(unit, sdsId, &pMode)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_SDS), "");
            return ret;
        }

        if(0x5 == speed)
        {
            if(pMode == RTK_MII_HISGMII)
            {
                PORT_SEM_UNLOCK(unit);
                return RT_ERR_OK;
            }
            
            _dal_longan_port_macForceLink_set(unit,  port, ENABLED, PORT_LINKDOWN);
            
            if ((ret = dal_longan_sds_mode_set(unit, sdsId, RTK_MII_HISGMII)) != RT_ERR_OK)
            {
                _dal_longan_port_macForceLink_set(unit,  port, DISABLED, PORT_LINKDOWN);
                PORT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_SDS), "");
                return ret;
            }
            _dal_longan_port_macForceLink_set(unit,  port, DISABLED, PORT_LINKDOWN);
        }
        else if(0x2 == speed)
        {
             if(pMode == RTK_MII_SGMII)
            {
                PORT_SEM_UNLOCK(unit);
                return RT_ERR_OK;
            }
             _dal_longan_port_macForceLink_set(unit,  port, ENABLED, PORT_LINKDOWN);
             
            if ((ret = dal_longan_sds_mode_set(unit, sdsId, RTK_MII_SGMII)) != RT_ERR_OK)
            {
                _dal_longan_port_macForceLink_set(unit,  port, DISABLED, PORT_LINKDOWN);
                PORT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_SDS), "");
                return ret;
            }
            _dal_longan_port_macForceLink_set(unit,  port, DISABLED, PORT_LINKDOWN);
        }
    }

    PORT_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_port_init_check
 * Description:
 *      Check Port module init flag
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NOT_INIT
 * Note:
 *      None
 */
int32 dal_longan_port_init_check(uint32 unit)
{
    RT_INIT_CHK(port_init[unit]);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_port_sem_lock
 * Description:
 *      Lock Port Module Sem
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_SEM_LOCK_FAILED
 * Note:
 *      None
 */
int32 dal_longan_port_sem_lock(uint32 unit)
{
    PORT_SEM_LOCK(unit);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_port_sem_unlock
 * Description:
 *      Unlock Port Module Sem
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_SEM_LOCK_FAILED
 * Note:
 *      None
 */
int32 dal_longan_port_sem_unlock(uint32 unit)
{
    PORT_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}



