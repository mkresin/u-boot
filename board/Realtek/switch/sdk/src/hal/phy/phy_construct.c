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
 * $Revision: 97417 $
 * $Date: 2019-06-05 16:36:16 +0800 (Wed, 05 Jun 2019) $
 *
 * Purpose : Setup PHY parameters
 *
 * Feature : Setup PHY parameters functions
 *
 */

/*
 * Include Files
 */
#include <dal/dal_construct.h>
#include <hal/phy/construct/conftypes.h>
#include <hal/phy/construct/conf_rtl8218d.c>
#include <rtk/customer/param/conf_rtl8218d_cust.c>

#if (defined(CONFIG_SDK_RTL8218D))
  #include <hal/phy/phy_rtl8218d.h>
#endif

#if defined(CONFIG_SDK_RTL9300)
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <hal/chipdef/longan/rtk_longan_table_struct.h>
#endif

#if defined(CONFIG_SDK_RTL8226)
  #include <hal/phy/nic_rtl8226/nic_rtl8226.h>
  #include <hal/phy/nic_rtl8226/nic_rtl8226_init.h>
#endif

#include <hal/mac/miim_common_drv.h>


#define PRINT_PHY_INITIAL_MESSGE(phy_name,macID)   CNSTRT_MSG("        %s (MacID=%u)\n",phy_name,macID)
#define PHY_CONSTRUCT_RV_WRITE(unit, port, crv)                 phy_construct_confcodeRv_write(unit, port, crv, (sizeof(crv)/sizeof(confcode_rv_t)))
#define PHY_CONSTRUCT_PRV_WRITE(unit, phy_base_macId, cprv)     phy_construct_confcodePrv_write(unit, phy_base_macId, cprv, (sizeof(cprv)/sizeof(confcode_prv_t)))


/* Function Name:
 *      phy_construct_field_read
 * Description:
 *      Read PHY register field configuration
 * Input:
 *      unit - unit id
 *      port - port id
 *      page - page number
 *      reg  - register
 *      endBit    - end bit of configure field
 *      startBit  - start bit of configure field
 * Output:
 *      data - PHY register field Configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
phy_construct_field_read(uint32 unit, uint32 port, uint32 page, uint32 reg,
    uint32 endBit, uint32 startBit, uint32 *data)
{
    uint32  configVal, len, mask;
    int32   ret;

    RT_PARAM_CHK((NULL == data), RT_ERR_NULL_POINTER);

    if (endBit < startBit)
        return RT_ERR_INPUT;

    RT_ERR_CHK(RTK_MII_READ(unit, port, page, reg, &configVal), ret);

    len = endBit - startBit + 1;

    if (16 == len)
        *data = configVal;
    else
    {
        mask = (1 << len) - 1;
        *data = (configVal >> startBit) & mask;
    }

    return RT_ERR_OK;
}   /* end of phy_construct_field_read */

/* Function Name:
 *      phy_construct_field_write
 * Description:
 *      Write PHY register field configuration
 * Input:
 *      unit - unit id
 *      port - port id
 *      page - page number
 *      reg  - register
 *      endBit    - end bit of configure field
 *      startBit  - start bit of configure field
 *      data - PHY register field Configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
phy_construct_field_write(uint32 unit, uint32 port, uint32 page, uint32 reg,
    uint32 endBit, uint32 startBit, uint32 data)
{
    uint32  configVal, len, mask;
    int32   ret;

    len = endBit - startBit + 1;

    if (endBit < startBit)
        return RT_ERR_INPUT;

    if (16 == len)
        configVal = data;
    else
    {
        mask = (1 << len) - 1;

        RT_ERR_CHK(RTK_MII_READ(unit, port, page, reg, &configVal), ret);

        configVal &= ~(mask << startBit);
        configVal |= (data << startBit);
    }

    RT_ERR_CHK(RTK_MII_WRITE(unit, port, page, reg, configVal), ret);

    return ret;
}   /* end of phy_construct_field_write */


/* Function Name:
 *      phy_construct_confcodeRv_write
 * Description:
 *      Construct the PHY with config code
 * Input:
 *      unit - unit id
 *      port - port id
 *      confcode_rv_t   - config code
 *      enty_num - entry number of the patch code
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
phy_construct_confcodeRv_write(uint32 unit, uint32 port, confcode_rv_t *pCc, uint32 enty_num)
{
    int32       i;

    for (i = 0; i < enty_num; i++)
    {
        RTK_MII_WRITE(unit, port, HAL_MIIM_FIX_PAGE, pCc[i].reg, pCc[i].val);
    }
}


/* Function Name:
 *      phy_construct_confcodePrv_write
 * Description:
 *      Construct the PHY with chip based config code
 * Input:
 *      unit - unit id
 *      phy_base_macId - base MAC ID of the PHY
 *      confcode_prv_t   - PHY config code
 *      enty_num - entry number of the patch code
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
phy_construct_confcodePrv_write(uint32 unit, uint32 phy_base_macId, confcode_prv_t *pPcc, uint32 enty_num)
{
    int32       i;

    for (i = 0; i < enty_num; i++)
    {
        RTK_MII_WRITE(unit, (phy_base_macId + pPcc[i].phy), HAL_MIIM_FIX_PAGE, pPcc[i].reg, pPcc[i].val);
    }
}


#if defined(CONFIG_SDK_RTL8218D)
/* Function Name:
 *      _rtl8218d_serdesMode_get
 * Description:
 *      Get SerDes mode of the PHY
 * Input:
 *      unit
 *      phy_base_macId - the macId of PHY0 of the RTL8218d
 * Output:
 *      pMode - serdes mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAIED
 * Note:
 *      None
 */
int32
_rtl8218d_serdesMode_get(uint32 unit, uint8 phy_base_macId, rt_serdesMode_t *pMode)
{
    uint32      data, blk;

    RTK_MII_READ(unit, phy_base_macId, 0, 30, &blk);
    RTK_MII_WRITE(unit, phy_base_macId, 0, 30, 8);
    RTK_MII_READ(unit, phy_base_macId, 0x260, 18, &data);
    RTK_MII_WRITE(unit, phy_base_macId, 0, 30, blk);

    switch (data & 0xF0)
    {
      case 0xD0:
        *pMode = RTK_MII_QSGMII;
        break;
      case 0xB0:
        *pMode = RTK_MII_XSGMII;
        break;
      default:
        RTK_DBG_PRINT(0, "unit %u 8218D phy port %u serdes mode: %X unknown type.\n", unit, (uint32)phy_base_macId, (data & 0xF0));
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_RTL9300)

/* Function Name:
 *      rtl8218d_rtl9300_config
 * Description:
 *      Configuration code for RTL8218d.
 * Input:
 *      unit
 *      phy_base_macId - the macId of PHY0 of the RTL8218d
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
static void _rtl8218d_rtl9300_config(uint32 unit, uint8 phy_base_macId)
{
    rt_serdesMode_t mode;
    phy_type_t      chip_id;
    uint16          chip_rev_id;
    int32           ret;

    if ((ret = _rtl8218d_serdesMode_get(unit, phy_base_macId, &mode)) != RT_ERR_OK)
        return;

    if ((ret = phy_id_get(unit, phy_base_macId, &chip_id, &chip_rev_id)) != RT_ERR_OK)
        return;

    if(RTK_PHYTYPE_RTL8218D_NMP == chip_id)
    {
        switch(mode)
        {
            case RTK_MII_QSGMII:
                PHY_CONSTRUCT_PRV_WRITE(unit, phy_base_macId, rtl8218d_bT_qsgmii_rtl9300_perchip);
                break;
            case RTK_MII_XSGMII:
                PHY_CONSTRUCT_PRV_WRITE(unit, phy_base_macId, rtl8218d_bT_xsgmii_rtl9300_perchip);
                break;
            default:
                return;
        }
    }
    else
    {
        switch(mode)
        {
            case RTK_MII_QSGMII:
                PHY_CONSTRUCT_PRV_WRITE(unit, phy_base_macId, rtl8218d_a_qsgmii_rtl9300_perchip);
                break;
            case RTK_MII_XSGMII:
                PHY_CONSTRUCT_PRV_WRITE(unit, phy_base_macId, rtl8218d_a_xsgmii_rtl9300_perchip);
                break;
            default:
                return;
        }
    }
}

#endif /* CONFIG_SDK_RTL9300 */

void
_rtl8218d_phy_config(uint32 unit, uint8 phy_base_macId)
{
    uint32      portnum, port;
    uint32      i = 0;
    uint32      value;
    phy_type_t  chip_id;
    uint16      chip_rev_id;
    int32       ret;

    if ((ret = phy_id_get(unit, phy_base_macId, &chip_id, &chip_rev_id)) != RT_ERR_OK)
    {
        return;
    }

    portnum = HWP_PHY_BASE_PHY_MAX(unit, phy_base_macId);
    if(RTK_PHYTYPE_RTL8218D_NMP == chip_id)
    {
        for (i = 0; i < portnum; i++)
        {
            port = phy_base_macId + i;

            RTK_MII_READ(unit, port, HAL_MIIM_FIX_PAGE, 0x0, &value);

            PHY_CONSTRUCT_RV_WRITE(unit, port, rtl8218d_bT_1000FM_INRX_perport);

            PHY_CONSTRUCT_RV_WRITE(unit, port, rtl8218d_bT_1000FS_INRX_perport);

            PHY_CONSTRUCT_RV_WRITE(unit, port, rtl8218d_bT_100_INRX_perport);

            PHY_CONSTRUCT_RV_WRITE(unit, port, rtl8218d_bT_green_perport);

            RTK_MII_WRITE(unit, port, HAL_MIIM_FIX_PAGE, 0x0, value);
        } /* end for */
    }
    else
    {
        switch (chip_rev_id)
        {
          case 0:
            {
                for (i = 0; i < portnum; i++)
                {
                    port = phy_base_macId + i;

                    /* backup reg 0 value */
                    RTK_MII_READ(unit, port, HAL_MIIM_FIX_PAGE, 0x0, &value);

                    PHY_CONSTRUCT_RV_WRITE(unit, port, rtl8218d_a_gphy_1000FM_INRX_perport);

                    PHY_CONSTRUCT_RV_WRITE(unit, port, rtl8218d_a_gphy_100F_INRX_perport);

                    PHY_CONSTRUCT_RV_WRITE(unit, port, rtl8218d_a_gphy_green_perport);

                    /* restore reg 0 value */
                    RTK_MII_WRITE(unit, port, HAL_MIIM_FIX_PAGE, 0x0, value);

                    PHY_CONSTRUCT_RV_WRITE(unit, port, rtl8218d_a_gphy_rtct_y_open_ini_perport);
                    PHY_CONSTRUCT_RV_WRITE(unit, port, rtl8218d_a_gphy_rtct_y_mismatch_ini_perport);
                    PHY_CONSTRUCT_RV_WRITE(unit, port, rtl8218d_a_gphy_rtct_y_lndrv_ini_perport);
                    PHY_CONSTRUCT_RV_WRITE(unit, port, rtl8218d_a_gphy_rtct_y_normal_cablen_perport);
                }/* end for */
            }
            break;
          case 1:
          default:
            {
                for (i = 0; i < portnum; i++)
                {
                    port = phy_base_macId + i;

                    PHY_CONSTRUCT_RV_WRITE(unit, port, rtl8218d_a_gphy_rtct_y_open_ini_perport);
                    PHY_CONSTRUCT_RV_WRITE(unit, port, rtl8218d_a_gphy_rtct_y_mismatch_ini_perport);
                    PHY_CONSTRUCT_RV_WRITE(unit, port, rtl8218d_a_gphy_rtct_y_lndrv_ini_perport);
                    PHY_CONSTRUCT_RV_WRITE(unit, port, rtl8218d_a_gphy_rtct_y_normal_cablen_perport);

                    PHY_CONSTRUCT_RV_WRITE(unit, port, rtl8218d_b_gphy_1000FM_INRX_perport);

                }/* end for */

                PHY_CONSTRUCT_PRV_WRITE(unit, phy_base_macId, rtl8218d_b_gphy_gdac_ib_dn_10_perchip);

                phy_8218d_sram_patch(unit, phy_base_macId);

                phy_8218d_nc_init(unit, phy_base_macId);
            }
            break;
        }/* end switch */
    }
}

void
_rtl8218d_sdsBasic_config(uint32 unit, uint8 phy_base_macId)
{
    rt_serdesMode_t mode;
    int32           ret;

    if ((ret = _rtl8218d_serdesMode_get(unit, phy_base_macId, &mode)) != RT_ERR_OK)
        return;

    switch(mode)
    {
        case RTK_MII_QSGMII:
            PHY_CONSTRUCT_PRV_WRITE(unit, phy_base_macId, rtl8218d_a_qsgmii_basic_perchip);
            break;
        case RTK_MII_XSGMII:
            PHY_CONSTRUCT_PRV_WRITE(unit, phy_base_macId, rtl8218d_a_xsgmii_basic_perchip);
            break;
        default:
            return;
    }

    return;
}

uint32      rtl8218d_cust_conf_init = FALSE;
phy_conf_per_hwp_t  *p_rtl8218d_cust_conf = NULL;
/* Function Name:
 *      _rtl8218d_cust_conf
 * Description:
 *      Apply customer's configuration for RTL8218d.
 * Input:
 *      unit
 *      phy_base_macId - the macId of PHY0 of the RTL8218d
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
_rtl8218d_cust_conf(uint32 unit, uint8 phy_base_macId)
{
    phy_conf_per_hwp_t  **pp_phy_conf;
    phy_confcode_pprfv_t *phy_param, *p_phy_param;
    uint32      i;

    if (rtl8218d_cust_conf_init == FALSE)
    {
        /* Search if there is customer's 8218D config for current board  */
        for (pp_phy_conf = conf_rtl8218d_cust_board_list; *pp_phy_conf != NULL; pp_phy_conf++)
        {
            if ((*pp_phy_conf)->hwp_id == HWP_IDENTIFIER_ID())
            {
                p_rtl8218d_cust_conf = *pp_phy_conf;
                break;
            }
        }
        rtl8218d_cust_conf_init = TRUE;
    }

    if (p_rtl8218d_cust_conf == NULL)
    {
        return;
    }

    if (unit >= RTK_MAX_NUM_OF_UNIT_LOCAL)
    {
        return;
    }

    if (p_rtl8218d_cust_conf->phy_conf_per_unit[unit] == NULL)
    {
        return;
    }

    /* search if there is PHY parameter for this PHY by using phy_base_macId */
    phy_param = NULL;
    for (i = 0; i < RTK_MAX_PHY_PER_UNIT; i++)
    {
        if (p_rtl8218d_cust_conf->phy_conf_per_unit[unit]->phy_ent[i].base_mac_id_of_this_phy == PHY_CONFCODE_MACID_END)
            break;

        if (p_rtl8218d_cust_conf->phy_conf_per_unit[unit]->phy_ent[i].base_mac_id_of_this_phy != phy_base_macId)
            continue;

        phy_param = p_rtl8218d_cust_conf->phy_conf_per_unit[unit]->phy_ent[i].phy_param;
        break;
    }

    if (phy_param == NULL)
    {
        return;
    }

    /* Apply customer's PHY parameter */
    p_phy_param = phy_param;
    while (p_phy_param->phy_port != PHY_CONFCODE_PHYPORT_END)
    {
        RT_LOG(LOG_TRACE, (MOD_HAL|MOD_PORT), "18D CUST CONF U%u,P%u,0x%X.0x%X.[%u:%u]=0x%X", unit, (phy_base_macId + p_phy_param->phy_port), p_phy_param->page, p_phy_param->reg, (uint32)p_phy_param->endBit, (uint32)p_phy_param->startBit, p_phy_param->value);
        phy_construct_field_write(unit, (phy_base_macId + p_phy_param->phy_port), p_phy_param->page, p_phy_param->reg, p_phy_param->endBit, p_phy_param->startBit, p_phy_param->value);
        p_phy_param++;
    }

}


void rtl8218d_config(uint32 unit, uint8 phy_base_macId)
{
    if (HWP_PHY_MODEL_BY_PORT(unit, phy_base_macId) == RTK_PHYTYPE_RTL8218D_NMP)
    {
        rtk_enable_t    enable;
        if (phy_8218d_efuseSwLoadEnable_get(unit, phy_base_macId, &enable) == RT_ERR_OK)
        {
            if (enable == ENABLED)
            {
                phy_8218d_efuseSw_load(unit, phy_base_macId);
            }
        }
    }


#if defined(CONFIG_SDK_RTL8380)
    if (HWP_8330_FAMILY_ID(unit) || HWP_8380_FAMILY_ID(unit))
    {
        _rtl8218d_rtl8380_config(unit,phy_base_macId);
    }
    else
#endif
#if defined(CONFIG_SDK_RTL8390)
    if (HWP_8390_50_FAMILY(unit))
    {
        _rtl8218d_rtl8390_config(unit,phy_base_macId);
    }
    else
#endif
#if defined(CONFIG_SDK_RTL9300)
    if(HWP_9300_FAMILY_ID(unit))
    {
        _rtl8218d_rtl9300_config(unit,phy_base_macId);
    }
    else
#endif
#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        _rtl8218d_rtl9310_config(unit,phy_base_macId);
    }
    else
#endif
    {
        _rtl8218d_sdsBasic_config(unit,phy_base_macId);
    }

    _rtl8218d_phy_config(unit, phy_base_macId);

    _rtl8218d_cust_conf(unit, phy_base_macId);

}
#endif


#if defined(CONFIG_SDK_RTL8226)
/* Function Name:
 *      rtl8226_config
 * Description:
 *      Configuration code for RTL8226
 * Input:
 *      unit    - unit ID
 *      baseMacId    - base port ID of the PHY
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtl8226_config(uint32 unit, uint8 macId)
{
    HANDLE  hDevice;
    BOOLEAN result;
    PHY_LINK_ABILITY phylinkability;
    PHY_SERDES_POLARITY_SWAP polarityswap;
    BOOL    enable;
    uint32  sds_id, sds_mode;

    hDevice.unit = unit;
    hDevice.port = macId;

    // Set initial speed as 2.5G, enable all speed cap.
    osal_memset(&phylinkability, 0x0, sizeof(phylinkability));

    phylinkability.Half_10 = TRUE;
    phylinkability.Full_10 = TRUE;

    phylinkability.Half_100 = TRUE;
    phylinkability.Full_100 = TRUE;

    phylinkability.Full_1000 = TRUE;

    phylinkability.adv_2_5G = TRUE;

    phylinkability.FC = TRUE;
    phylinkability.AsyFC = TRUE;

    // We use USB RTL8156 to vevify, argument singlephy is set to FALSE.
    // If you use RTL8226, singlephy must set to TRUE.
    if ((result = Rtl8226_phy_init(hDevice, &phylinkability, TRUE)) != SUCCESS)
    {
        RT_LOG(LOG_TRACE, (MOD_HAL|MOD_PORT), "unit %u port %u RTL8226 phyinit status = 0x%x", unit, macId, result);
    }

    if (HWP_PORT_PHY_MDI_PIN_SWAP(unit, macId) == 1)
    {
        enable = TRUE;
        Rtl8226_mdiSwapEnable_set(hDevice, enable);
    }

    if ((sds_id = HWP_PORT_SDSID(unit, macId)) != HWP_NONE)
    {
        osal_memset(&polarityswap, 0, sizeof(PHY_SERDES_POLARITY_SWAP));
        if ((SERDES_POLARITY_CHANGE == HWP_SDS_RX_POLARITY(unit, sds_id)) || (SERDES_POLARITY_CHANGE == HWP_SDS_TX_POLARITY(unit, sds_id)))
        {
            polarityswap.RX_SWAP = (SERDES_POLARITY_CHANGE == HWP_SDS_RX_POLARITY(unit, sds_id)) ? 1 : 0;
            polarityswap.TX_SWAP = (SERDES_POLARITY_CHANGE == HWP_SDS_TX_POLARITY(unit, sds_id)) ? 1 : 0;
            Rtl8226_serdes_polarity_swap(hDevice, &polarityswap);
        }

        sds_mode = HWP_SDS_MODE(unit, sds_id);
        if ((sds_mode == RTK_MII_HISGMII) || (sds_mode == RTK_MII_SGMII))
        {
            if ((result = Rtl8226_serdes_option_set(hDevice, 1 /* HiSGMII/SGMII */)) != SUCCESS)
            {
                RT_LOG(LOG_TRACE, (MOD_HAL|MOD_PORT), "unit %u port %u RTL8226 Rtl8226_serdes_option_set status = 0x%x", unit, macId, result);
            }
        }
    }

    return;
}
#endif /* end defined(CONFIG_SDK_RTL8226) */

void
phy_construct_config_init(uint32 unit)
{
    uint32      i;
    uint8       baseMacId;

    CNSTRT_PRINT("%s()\n",__FUNCTION__);

    HWP_PHY_TRAVS(unit,i)
    {
        baseMacId = HWP_PHY_BASE_MACID_BY_IDX(unit,i);

        switch (HWP_PHY_MODEL_BY_PORT(unit, baseMacId))
        {
          #if defined(CONFIG_SDK_RTL8226)
          case RTK_PHYTYPE_RTL8226:
            rtl8226_config(unit, baseMacId);
            break;
          #endif/* end of defined(CONFIG_SDK_RTL8226) */
          #if (defined(CONFIG_SDK_RTL8218D))
          case RTK_PHYTYPE_RTL8218D:
            rtl8218d_config(unit, baseMacId);
            break;
		  #endif/* end of defined(CONFIG_SDK_RTL8218D) */
          default:
            break;
        }/* end switch */
    } /* end HWP_PHY_TRAVS */
}




