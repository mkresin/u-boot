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
 * $Revision: 96094 $
 * $Date: 2019-03-29 17:59:48 +0800 (Fri, 29 Mar 2019) $
 *
 * Purpose : Mapper Layer is used to seperate different kind of software or hardware platform
 *
 * Feature : Just dispatch information to Multiplex layer
 *
 */
#ifndef __DAL_MAPPER_H__
#define __DAL_MAPPER_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <dal/dal_common.h>
#include <rtk/l2.h>
#include <rtk/diag.h>
#include <rtk/sds.h>
/*
#include <rtk/vlan.h>
#include <rtk/l3.h>
#include <rtk/tunnel.h>
#include <rtk/trunk.h>
#include <rtk/stp.h>
#include <rtk/rate.h>
#include <rtk/qos.h>
#include <rtk/trap.h>
#include <rtk/stat.h>
#include <rtk/switch.h>
#include <rtk/mirror.h>
#include <rtk/flowctrl.h>
#include <rtk/sec.h>
#include <rtk/oam.h>
#include <rtk/pie.h>
#include <rtk/eee.h>
#include <rtk/led.h>
#include <rtk/time.h>
#include <rtk/acl.h>
#include <rtk/mpls.h>
#include <rtk/stack.h>
#include <rtk/vxlan.h>
#include <rtk/openflow.h>
#include <rtk/mcast.h>
#include <rtk/ipmcast.h>
#include <rtk/bpe.h>
*/

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
typedef struct dal_mapper_s {
    int32   (*_init)(uint32 );

    /* L2 */
    int32   (*l2_init)(uint32 );
    int32   (*l2_flushLinkDownPortAddrEnable_get)(uint32 , rtk_enable_t *);
    int32   (*l2_flushLinkDownPortAddrEnable_set)(uint32 , rtk_enable_t );
    int32   (*l2_ucastAddr_flush)(uint32 , rtk_l2_flushCfg_t *);
    int32   (*l2_macLearningCnt_get)(uint32, rtk_l2_macLimitType_t, rtk_l2_macCnt_t *);
    int32   (*l2_limitLearningNum_get)(uint32, rtk_l2_macLimitType_t, rtk_l2_macCnt_t *);
    int32   (*l2_limitLearningNum_set)(uint32, rtk_l2_macLimitType_t, rtk_l2_macCnt_t *);
    int32   (*l2_limitLearningAction_get)(uint32, rtk_l2_macLimitType_t, rtk_l2_macLimitAction_t *);
    int32   (*l2_limitLearningAction_set)(uint32, rtk_l2_macLimitType_t, rtk_l2_macLimitAction_t *);
    int32   (*l2_fidLimitLearningEntry_get)(uint32 , uint32 , rtk_l2_fidMacLimitEntry_t *);
    int32   (*l2_fidLimitLearningEntry_set)(uint32 , uint32 , rtk_l2_fidMacLimitEntry_t *);
    int32   (*l2_fidLearningCnt_reset)(uint32 , uint32 );
    int32   (*l2_agingTime_get)(uint32, rtk_l2_ageTimeType_t, uint32 *);
    int32   (*l2_agingTime_set)(uint32, rtk_l2_ageTimeType_t, uint32);
    int32   (*l2_portAgingEnable_get)(uint32 , rtk_port_t , rtk_enable_t *);
    int32   (*l2_portAgingEnable_set)(uint32 , rtk_port_t , rtk_enable_t);
    int32   (*l2_trkAgingEnable_get)(uint32, rtk_trk_t, rtk_enable_t *);
    int32   (*l2_trkAgingEnable_set)(uint32, rtk_trk_t, rtk_enable_t);
    int32   (*l2_hashAlgo_get)(uint32 , uint32 *);
    int32   (*l2_hashAlgo_set)(uint32 , uint32 );
    int32   (*l2_bucketHashAlgo_get)(uint32, uint32, uint32 *);
    int32   (*l2_bucketHashAlgo_set)(uint32, uint32, uint32);
    int32   (*l2_vlanMode_get)(uint32 , rtk_port_t , rtk_l2_vlanMode_t *);
    int32   (*l2_vlanMode_set)(uint32 , rtk_port_t , rtk_l2_vlanMode_t );
    int32   (*l2_learningFullAction_get)(uint32, rtk_action_t *);
    int32   (*l2_learningFullAction_set)(uint32, rtk_action_t);
    int32   (*l2_portNewMacOp_get)(uint32 , rtk_port_t , rtk_l2_newMacLrnMode_t *, rtk_action_t *);
    int32   (*l2_portNewMacOp_set)(uint32 , rtk_port_t , rtk_l2_newMacLrnMode_t , rtk_action_t );
    int32   (*l2_addr_init)(uint32 , rtk_vlan_t , rtk_mac_t *, rtk_l2_ucastAddr_t *);
    int32   (*l2_addr_add)(uint32 , rtk_l2_ucastAddr_t *);
    int32   (*l2_addr_del)(uint32 , rtk_vlan_t , rtk_mac_t *);
    int32   (*l2_addr_get)(uint32 , rtk_l2_ucastAddr_t *);
    int32   (*l2_addr_set)(uint32 , rtk_l2_ucastAddr_t *);
    int32   (*l2_addr_delAll)(uint32 , uint32 );
    int32   (*l2_nextValidAddr_get)(uint32 , int32 *, uint32 , rtk_l2_ucastAddr_t *);
    int32   (*l2_mcastAddr_init)(uint32 , rtk_vlan_t , rtk_mac_t *, rtk_l2_mcastAddr_t *);
    int32   (*l2_mcastAddr_add)(uint32 , rtk_l2_mcastAddr_t *);
    int32   (*l2_mcastAddr_del)(uint32 , rtk_vlan_t , rtk_mac_t *);
    int32   (*l2_mcastAddr_get)(uint32 , rtk_l2_mcastAddr_t *);
    int32   (*l2_mcastAddr_set)(uint32 , rtk_l2_mcastAddr_t *);
    int32   (*l2_mcastAddr_addByIndex)(uint32 , rtk_l2_mcastAddr_t *);
    int32   (*l2_mcastAddr_setByIndex)(uint32 , rtk_l2_mcastAddr_t *);
    int32   (*l2_mcastAddr_delIgnoreIndex)(uint32 , rtk_vlan_t , rtk_mac_t *);
    int32   (*l2_nextValidMcastAddr_get)(uint32 , int32 *, rtk_l2_mcastAddr_t *);
    int32   (*l2_ipmcMode_get)(uint32 , rtk_l2_ipmcMode_t *);
    int32   (*l2_ipmcMode_set)(uint32 , rtk_l2_ipmcMode_t );
    int32   (*l2_ipMcastAddrExt_init)(uint32 , rtk_l2_ipMcastHashKey_t * , rtk_l2_ipMcastAddr_t *);
    int32   (*l2_ipMcastAddr_add)(uint32 , rtk_l2_ipMcastAddr_t *);
    int32   (*l2_ipMcastAddr_del)(uint32 , ipaddr_t , ipaddr_t , rtk_vlan_t );
    int32   (*l2_ipMcastAddr_get)(uint32 , rtk_l2_ipMcastAddr_t *);
    int32   (*l2_ipMcastAddr_set)(uint32 , rtk_l2_ipMcastAddr_t *);
    int32   (*l2_ipMcastAddr_addByIndex)(uint32 , rtk_l2_ipMcastAddr_t *);
    int32   (*l2_ipMcastAddr_setByIndex)(uint32 , rtk_l2_ipMcastAddr_t *);
    int32   (*l2_ipMcastAddr_delIgnoreIndex)(uint32 , ipaddr_t , ipaddr_t , rtk_vlan_t );
    int32   (*l2_nextValidIpMcastAddr_get)(uint32 , int32 *, rtk_l2_ipMcastAddr_t *);
    int32   (*l2_ipMcastAddrChkEnable_get)(uint32 , rtk_enable_t *);
    int32   (*l2_ipMcastAddrChkEnable_set)(uint32 , rtk_enable_t );
    int32   (*l2_ipMcstFidVidCompareEnable_get)(uint32 , rtk_enable_t *);
    int32   (*l2_ipMcstFidVidCompareEnable_set)(uint32 , rtk_enable_t );
    int32   (*l2_ip6mcMode_get)(uint32 , rtk_l2_ipmcMode_t *);
    int32   (*l2_ip6mcMode_set)(uint32 , rtk_l2_ipmcMode_t );
    int32   (*l2_ip6CareByte_get)(uint32 , rtk_l2_ip6_careByte_type_t , uint32 *);
    int32   (*l2_ip6CareByte_set)(uint32 , rtk_l2_ip6_careByte_type_t , uint32);
    int32   (*l2_ip6McastAddrExt_init)(uint32 , rtk_l2_ip6McastHashKey_t * , rtk_l2_ip6McastAddr_t *);
    int32   (*l2_ip6McastAddr_add)(uint32 , rtk_l2_ip6McastAddr_t *);
    int32   (*l2_ip6McastAddr_del)(uint32 , rtk_ipv6_addr_t , rtk_ipv6_addr_t , rtk_vlan_t );
    int32   (*l2_ip6McastAddr_get)(uint32 , rtk_l2_ip6McastAddr_t *);
    int32   (*l2_ip6McastAddr_set)(uint32 , rtk_l2_ip6McastAddr_t *);
    int32   (*l2_ip6McastAddr_addByIndex)(uint32 , rtk_l2_ip6McastAddr_t *);
    int32   (*l2_ip6McastAddr_setByIndex)(uint32 , rtk_l2_ip6McastAddr_t *);
    int32   (*l2_ip6McastAddr_delIgnoreIndex)(uint32 , rtk_ipv6_addr_t , rtk_ipv6_addr_t , rtk_vlan_t );
    int32   (*l2_nextValidIp6McastAddr_get)(uint32 , int32 *, rtk_l2_ip6McastAddr_t *);
    int32   (*l2_mcastFwdIndex_alloc)(uint32 , int32 *);
    int32   (*l2_mcastFwdIndex_free)(uint32 , int32 );
    int32   (*l2_mcastFwdIndexFreeCount_get)(uint32 , uint32 *);
    int32   (*l2_mcastFwdPortmaskEntry_get)(uint32 , int32 , rtk_portmask_t *);
    int32   (*l2_mcastFwdPortmaskEntry_set)(uint32 , int32 , rtk_portmask_t *);
    int32   (*l2_cpuMacAddr_add)(uint32 , rtk_vlan_t , rtk_mac_t *);
    int32   (*l2_cpuMacAddr_del)(uint32 , rtk_vlan_t , rtk_mac_t *);
    int32   (*l2_portMoveAction_get)(uint32, rtk_l2_portMoveType_t, rtk_l2_portMoveAct_t *);
    int32   (*l2_portMoveAction_set)(uint32, rtk_l2_portMoveType_t, rtk_l2_portMoveAct_t *);
    int32   (*l2_portMoveLearn_get)(uint32, rtk_l2_portMoveType_t, rtk_l2_portMoveLrn_t *);
    int32   (*l2_portMoveLearn_set)(uint32, rtk_l2_portMoveType_t, rtk_l2_portMoveLrn_t *);
    int32   (*l2_legalPortMoveFlushAddrEnable_get)(uint32 , rtk_port_t , rtk_enable_t *);
    int32   (*l2_legalPortMoveFlushAddrEnable_set)(uint32 , rtk_port_t , rtk_enable_t );
    int32   (*l2_staticPortMoveAction_get)(uint32 , rtk_port_t , rtk_action_t *);
    int32   (*l2_staticPortMoveAction_set)(uint32 , rtk_port_t , rtk_action_t );
    int32   (*l2_lookupMissFloodPortMask_get)(uint32 , rtk_l2_lookupMissType_t , rtk_portmask_t *);
    int32   (*l2_lookupMissFloodPortMask_set)(uint32 , rtk_l2_lookupMissType_t , rtk_portmask_t *);
    int32   (*l2_lookupMissFloodPortMask_add)(uint32 , rtk_l2_lookupMissType_t , rtk_port_t );
    int32   (*l2_lookupMissFloodPortMask_del)(uint32 , rtk_l2_lookupMissType_t , rtk_port_t );
    int32   (*l2_lookupMissFloodPortMask_setByIndex)(uint32 , rtk_l2_lookupMissType_t , uint32, rtk_portmask_t *);
    int32   (*l2_lookupMissFloodPortMaskIdx_get)(uint32 , rtk_l2_lookupMissType_t , uint32 *);
    int32   (*l2_lookupMissFloodPortMaskIdx_set)(uint32 , rtk_l2_lookupMissType_t , uint32 );
    int32   (*l2_portLookupMissAction_get)(uint32 , rtk_port_t , rtk_l2_lookupMissType_t , rtk_action_t *);
    int32   (*l2_portLookupMissAction_set)(uint32 , rtk_port_t , rtk_l2_lookupMissType_t , rtk_action_t );
    int32   (*l2_portUcastLookupMissAction_get)(uint32, rtk_port_t, rtk_action_t *);
    int32   (*l2_portUcastLookupMissAction_set)(uint32, rtk_port_t, rtk_action_t);
    int32   (*l2_srcPortEgrFilterMask_get)(uint32 , rtk_portmask_t *);
    int32   (*l2_srcPortEgrFilterMask_set)(uint32 , rtk_portmask_t *);
    int32   (*l2_srcPortEgrFilterMask_add)(uint32 , rtk_port_t );
    int32   (*l2_srcPortEgrFilterMask_del)(uint32 , rtk_port_t );
    int32   (*l2_exceptionAddrAction_get)(uint32 , rtk_l2_exceptionAddrType_t , rtk_action_t *);
    int32   (*l2_exceptionAddrAction_set)(uint32 , rtk_l2_exceptionAddrType_t , rtk_action_t );
    int32   (*l2_addrEntry_get)(uint32 , uint32, rtk_l2_entry_t *);
    int32   (*l2_conflictAddr_get)(uint32 , rtk_l2_entry_t * , rtk_l2_entry_t * , uint32 , uint32 *);
    int32   (*l2_zeroSALearningEnable_get)(uint32 , rtk_enable_t *);
    int32   (*l2_zeroSALearningEnable_set)(uint32 , rtk_enable_t);
    int32   (*l2_secureMacMode_get)(uint32, rtk_enable_t *);
    int32   (*l2_secureMacMode_set)(uint32, rtk_enable_t);
    int32   (*l2_portDynamicPortMoveForbidEnable_get)(uint32 , rtk_port_t , rtk_enable_t *);
    int32   (*l2_portDynamicPortMoveForbidEnable_set)(uint32 , rtk_port_t , rtk_enable_t);
    int32   (*l2_trkDynamicPortMoveForbidEnable_get)(uint32, rtk_trk_t, rtk_enable_t *);
    int32   (*l2_trkDynamicPortMoveForbidEnable_set)(uint32, rtk_trk_t, rtk_enable_t );
    int32   (*l2_portMacFilterEnable_get)(uint32 , rtk_port_t , rtk_l2_macFilterMode_t , rtk_enable_t *);
    int32   (*l2_portMacFilterEnable_set)(uint32 , rtk_port_t , rtk_l2_macFilterMode_t , rtk_enable_t);
    int32   (*l2_hwNextValidAddr_get)(uint32, int32 *, rtk_l2_nextValidType_t , rtk_l2_entry_t  *);
    int32   (*l2_portCtrl_get)(uint32, rtk_port_t, rtk_l2_portCtrlType_t, int32 *);
    int32   (*l2_portCtrl_set)(uint32, rtk_port_t, rtk_l2_portCtrlType_t, int32);
    int32   (*l2_status_get)(uint32, rtk_l2_stsType_t, uint32, uint32 *);
    int32   (*l2_stkLearningEnable_get)(uint32, rtk_enable_t *);
    int32   (*l2_stkLearningEnable_set)(uint32, rtk_enable_t);
    int32   (*l2_stkKeepUcastEntryValid_get)(uint32, rtk_enable_t *);
    int32   (*l2_stkKeepUcastEntryValid_set)(uint32, rtk_enable_t);
    int32   (*l2_entryCnt_get)(uint32, rtk_l2_entryType_t, uint32 *);
    int32   (*l2_hashIdx_get)(uint32, rtk_l2_macHashIdx_t *);

    /* PORT */
    int32   (*port_init)(uint32 );
    int32   (*port_link_get)(uint32 , rtk_port_t , rtk_port_linkStatus_t *);
    int32   (*port_speedDuplex_get)(uint32 , rtk_port_t , rtk_port_speed_t *, rtk_port_duplex_t *);
    int32   (*port_flowctrl_get)(uint32 , rtk_port_t , uint32 *, uint32 *);
    int32   (*port_phyAutoNegoEnable_get)(uint32 , rtk_port_t , rtk_enable_t *);
    int32   (*port_phyAutoNegoEnable_set)(uint32 , rtk_port_t , rtk_enable_t );
    int32   (*port_phyAutoNegoAbility_get)(uint32 , rtk_port_t , rtk_port_phy_ability_t *);
    int32   (*port_phyAutoNegoAbility_set)(uint32 , rtk_port_t , rtk_port_phy_ability_t *);
    int32   (*port_phyForceModeAbility_get)(uint32 , rtk_port_t , rtk_port_speed_t *, rtk_port_duplex_t *, rtk_enable_t *);
    int32   (*port_phyForceModeAbility_set)(uint32 , rtk_port_t , rtk_port_speed_t , rtk_port_duplex_t , rtk_enable_t );
    int32   (*port_phyForceFlowctrlMode_get)(uint32 , rtk_port_t , rtk_port_flowctrl_mode_t *);
    int32   (*port_phyForceFlowctrlMode_set)(uint32 , rtk_port_t , rtk_port_flowctrl_mode_t *);
    int32   (*port_phyMasterSlave_get)(uint32 , rtk_port_t, rtk_port_masterSlave_t *, rtk_port_masterSlave_t *);
    int32   (*port_phyMasterSlave_set)(uint32 , rtk_port_t, rtk_port_masterSlave_t );
    int32   (*port_phyReg_get)(uint32 , rtk_port_t , uint32 , rtk_port_phy_reg_t , uint32 *);
    int32   (*port_phyReg_set)(uint32 , rtk_port_t , uint32 , rtk_port_phy_reg_t , uint32 );
    int32   (*port_phyExtParkPageReg_get)(uint32 , rtk_port_t , uint32 , uint32 , uint32 , rtk_port_phy_reg_t , uint32 *);
    int32   (*port_phyExtParkPageReg_set)(uint32 , rtk_port_t , uint32 , uint32 , uint32 , rtk_port_phy_reg_t , uint32);
    int32   (*port_phymaskExtParkPageReg_set)(uint32 , rtk_portmask_t *, uint32 , uint32 , uint32 , rtk_port_phy_reg_t , uint32);
    int32   (*port_phyMmdReg_get)(uint32 , rtk_port_t , uint32 , uint32 , uint32 *);
    int32   (*port_phyMmdReg_set)(uint32 , rtk_port_t , uint32 , uint32 , uint32);
    int32   (*port_phymaskMmdReg_set)(uint32 , rtk_portmask_t *, uint32 , uint32 , uint32);
    int32   (*port_cpuPortId_get)(uint32 , rtk_port_t *);
    int32   (*port_isolation_get)(uint32 , rtk_port_t , rtk_portmask_t *);
    int32   (*port_isolation_set)(uint32 , rtk_port_t , rtk_portmask_t *);
    int32   (*port_isolationExt_get)(uint32 , uint32 , rtk_port_t , rtk_portmask_t *);
    int32   (*port_isolationExt_set)(uint32 , uint32 , rtk_port_t , rtk_portmask_t *);
    int32   (*port_isolation_add)(uint32 , rtk_port_t , rtk_port_t );
    int32   (*port_isolation_del)(uint32 , rtk_port_t , rtk_port_t );
    int32   (*port_isolationRestrictRoute_get)(uint32 , rtk_enable_t *);
    int32   (*port_isolationRestrictRoute_set)(uint32 , rtk_enable_t );
    int32   (*port_phyComboPortMedia_get)(uint32 , rtk_port_t , rtk_port_media_t *);
    int32   (*port_phyComboPortMedia_set)(uint32 , rtk_port_t , rtk_port_media_t );
    int32   (*port_adminEnable_get)(uint32 , rtk_port_t , rtk_enable_t *);
    int32   (*port_adminEnable_set)(uint32 , rtk_port_t , rtk_enable_t );
    int32   (*port_backpressureEnable_get)(uint32 , rtk_port_t , rtk_enable_t *);
    int32   (*port_backpressureEnable_set)(uint32 , rtk_port_t , rtk_enable_t );
    int32   (*port_linkChange_register)(uint32, dal_link_change_callback_f);
    int32   (*port_linkChange_unregister)(uint32);
    int32   (*port_greenEnable_get)(uint32 , rtk_port_t , rtk_enable_t *);
    int32   (*port_greenEnable_set)(uint32 , rtk_port_t , rtk_enable_t );
    int32   (*port_txEnable_get)(uint32 , rtk_port_t , rtk_enable_t *);
    int32   (*port_txEnable_set)(uint32 , rtk_port_t , rtk_enable_t );
    int32   (*port_rxEnable_get)(uint32 , rtk_port_t , rtk_enable_t *);
    int32   (*port_rxEnable_set)(uint32 , rtk_port_t , rtk_enable_t );
    int32   (*port_specialCongest_set)(uint32 , rtk_port_t , uint32 );
    int32   (*port_phyCrossOverMode_get)(uint32 , rtk_port_t , rtk_port_crossOver_mode_t *);
    int32   (*port_phyCrossOverMode_set)(uint32 , rtk_port_t , rtk_port_crossOver_mode_t );
    int32   (*port_phyCrossOverStatus_get)(uint32 , rtk_port_t , rtk_port_crossOver_status_t *);
    int32   (*port_flowCtrlEnable_get)(uint32 , rtk_port_t , rtk_enable_t *);
    int32   (*port_flowCtrlEnable_set)(uint32 , rtk_port_t , rtk_enable_t );
    int32   (*port_phyComboPortFiberMedia_get)(uint32 , rtk_port_t , rtk_port_fiber_media_t *);
    int32   (*port_phyComboPortFiberMedia_set)(uint32 , rtk_port_t , rtk_port_fiber_media_t );
    int32   (*port_linkMedia_get)(uint32 , rtk_port_t , rtk_port_linkStatus_t *, rtk_port_media_t *);
    int32   (*port_linkDownPowerSavingEnable_get)(uint32 , rtk_port_t , rtk_enable_t *);
    int32   (*port_linkDownPowerSavingEnable_set)(uint32 , rtk_port_t , rtk_enable_t );
    int32   (*port_vlanBasedIsolationEntry_get)(uint32 , uint32 , rtk_port_vlanIsolationEntry_t *);
    int32   (*port_vlanBasedIsolationEntry_set)(uint32 , uint32 , rtk_port_vlanIsolationEntry_t *);
    int32   (*port_vlanBasedIsolation_vlanSource_get)(uint32 , rtk_port_vlanIsolationSrc_t *);
    int32   (*port_vlanBasedIsolation_vlanSource_set)(uint32 , rtk_port_vlanIsolationSrc_t);
    int32   (*port_vlanBasedIsolationEgrBypass_get)(uint32 , rtk_port_t, rtk_enable_t *);
    int32   (*port_vlanBasedIsolationEgrBypass_set)(uint32 , rtk_port_t, rtk_enable_t);
    int32   (*port_gigaLiteEnable_get)(uint32 , rtk_port_t , rtk_enable_t *);
    int32   (*port_gigaLiteEnable_set)(uint32 , rtk_port_t , rtk_enable_t );
    int32   (*port_phy2pt5gLiteEnable_get)(uint32 , rtk_port_t , rtk_enable_t *);
    int32   (*port_phy2pt5gLiteEnable_set)(uint32 , rtk_port_t , rtk_enable_t );
    int32   (*port_downSpeedEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*port_downSpeedEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*port_fiberDownSpeedEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*port_fiberDownSpeedEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*port_fiberNwayForceLinkEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*port_fiberNwayForceLinkEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*port_fiberUnidirEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*port_fiberUnidirEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*port_fiberOAMLoopBackEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*port_10gMedia_set)(uint32, rtk_port_t, rtk_port_10gMedia_t);
    int32   (*port_10gMedia_get)(uint32, rtk_port_t, rtk_port_10gMedia_t *);
    int32   (*port_phyLoopBackEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*port_phyLoopBackEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*port_phyFiberTxDis_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*port_phyFiberTxDisPin_set)(uint32, rtk_port_t, uint32);
    int32   (*port_fiberRxEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*port_fiberRxEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*port_phyIeeeTestMode_set)(uint32, rtk_port_t, rtk_port_phyTestMode_t *);
    int32   (*port_phyPolar_get)(uint32, rtk_port_t, rtk_port_phyPolarCtrl_t *);
    int32   (*port_phyPolar_set)(uint32, rtk_port_t, rtk_port_phyPolarCtrl_t *);
    int32   (*port_phyEyeMonitor_start)(uint32, rtk_port_t, uint32, uint32);
    int32   (*port_imageFlash_load)(uint32, rtk_port_t, uint32, uint8 *);
    int32   (*port_phySds_get)(uint32, rtk_port_t, rtk_sdsCfg_t *);
    int32   (*port_phySds_set)(uint32, rtk_port_t, rtk_sdsCfg_t *);
    int32   (*port_phySdsRxCaliStatus_get)(uint32, rtk_port_t, uint32, rtk_port_phySdsRxCaliStatus_t *);
    int32   (*port_phyReset_set)(uint32, rtk_port_t);
    int32   (*port_phyLinkStatus_get)(uint32, rtk_port_t, rtk_port_linkStatus_t *);
    int32   (*port_phyPeerAutoNegoAbility_get)(uint32, rtk_port_t, rtk_port_phy_ability_t *);
    int32   (*port_phyMacIntfSerdesMode_get)(uint32, rtk_port_t, rt_serdesMode_t *);
    int32   (*port_phyLedMode_set)(uint32 unit, rtk_port_t port, rtk_phy_ledMode_t *);
    int32   (*port_phyLedCtrl_get)(uint32, rtk_port_t, rtk_phy_ledCtrl_t *);
    int32   (*port_phyLedCtrl_set)(uint32, rtk_port_t, rtk_phy_ledCtrl_t *);
    int32   (*port_phyMacIntfSerdesLinkStatus_get)(uint32, rtk_port_t, rtk_phy_macIntfSdsLinkStatus_t *);
    int32   (*port_phySdsEyeParam_get)(uint32, rtk_port_t, uint32, rtk_sds_eyeParam_t *);
    int32   (*port_phySdsEyeParam_set)(uint32, rtk_port_t, uint32, rtk_sds_eyeParam_t *);
    int32   (*port_phyMdiLoopbackEnable_get)(uint32 , rtk_port_t , rtk_enable_t *);
    int32   (*port_phyMdiLoopbackEnable_set)(uint32 , rtk_port_t , rtk_enable_t );
    int32   (*_port_macForceLink_get)(uint32, rtk_port_t, rtk_enable_t*, rtk_port_linkStatus_t*);
    int32   (*_port_macForceLink_set)(uint32, rtk_port_t, rtk_enable_t, rtk_port_linkStatus_t);

    /* Diagnostic */
    int32   (*diag_init)(uint32 );
    int32   (*diag_portRtctResult_get)(uint32, rtk_port_t, rtk_rtctResult_t *);
    int32   (*diag_rtctEnable_set)(uint32, rtk_portmask_t *);
    int32   (*diag_table_read)(uint32 , uint32 , uint32 , uint32 *, uint32 *, uint32 *);

    /* SerDes */
    int32   (*sds_init)(uint32 );
    int32   (*sds_symErr_get)(uint32, uint32, rtk_sds_symErr_t *);
    int32   (*sds_symErr_clear)(uint32, uint32);
    int32   (*sds_linkSts_get)(uint32, uint32, rtk_sds_linkSts_t *);
    int32   (*sds_testModeCnt_get)(uint32, uint32, uint32 *);
    int32   (*sds_testMode_set)(uint32, uint32, rtk_sds_testMode_t);
    int32   (*sds_rx_rst)(uint32, uint32);
    int32   (*sds_leq_get)(uint32, uint32, uint32 *);
    int32   (*sds_leq_set)(uint32, uint32, uint32);
    int32   (*sds_leq_adapt)(uint32, uint32);
    int32   (*sds_xsgNwayEn_set)(uint32, uint32, rtk_enable_t);
    int32   (*sds_cmuBand_get)(uint32, uint32, uint32 *);
    int32   (*sds_cmuBand_set)(uint32, uint32, rtk_enable_t, uint32);
    int32   (*sds_eyeMonitor_start)(uint32, uint32, uint32);
    int32   (*sds_eyeParam_get)(uint32, uint32, rtk_sds_eyeParam_t *);
    int32   (*sds_eyeParam_set)(uint32, uint32, rtk_sds_eyeParam_t);
    int32   (*sds_rxCaliConf_get)(uint32, uint32, rtk_sds_rxCaliConf_t *);
    int32   (*sds_rxCaliConf_set)(uint32, uint32, rtk_sds_rxCaliConf_t);
    int32   (*_sds_mode_get)(uint32, uint32, rtk_serdesMode_t *);
    int32   (*_sds_mode_set)(uint32, uint32, rtk_serdesMode_t);

#if (defined(CONFIG_SDK_APP_DIAG_EXT) && defined (CONFIG_SDK_RTL9300))
    int32   (*diag_table_reg_field_get)(uint32, rtk_diag_debug_t *);
    int32   (*diag_table_reg_field_set)(uint32, rtk_diag_debug_t *);
#endif



}__attribute__((aligned(sizeof(int)))) dal_mapper_t ;


/*
 * Macro Definition
 */


/*
 * Function Declaration
 */

/* Module Name : */


#endif /* __DAL_MAPPER_H __ */
