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
 * Purpose : Definition those public Port APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Port
 *
 */

#ifndef __DAL_LONGAN_PORT_H__
#define __DAL_LONGAN_PORT_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <dal/dal_common.h>
#include <rtk/port.h>
#include <dal/dal_mapper.h>

/* Function Name:
 *      dal_longan_port_10gMedia_set
 * Description:
 *      Set 10G port media of the specific port
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - port media
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) The media value is as following:
 *          - PORT_10GMEDIA_FIBER_10G,
 *          - PORT_10GMEDIA_FIBER_1G,
 *          - PORT_10GMEDIA_DAC_50CM,
 *          - PORT_10GMEDIA_DAC_100CM,
 *          - PORT_10GMEDIA_DAC_300CM,
 */
int32
dal_longan_port_10gMedia_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media);

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */


/*
 * Function Declaration
 */


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
extern int32
dal_longan_portMapper_init(dal_mapper_t *pMapper);


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
extern int32
dal_longan_port_init(uint32 unit);

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
extern int32
_dal_longan_port_macForceLink_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable,rtk_port_linkStatus_t *pLinkSts);

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
extern int32
_dal_longan_port_macForceLink_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable,rtk_port_linkStatus_t linkSts);

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
extern int32
dal_longan_port_link_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus);

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
extern int32
dal_longan_port_txEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

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
extern int32
dal_longan_port_rxEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

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
extern int32
dal_longan_port_txEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

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
extern int32
dal_longan_port_rxEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

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
 * Note:
 *      None
 */
extern int32
dal_longan_port_specialCongest_set(uint32 unit, rtk_port_t port, uint32 second);

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
extern int32
dal_longan_port_speedDuplex_get(
    uint32            unit,
    rtk_port_t        port,
    rtk_port_speed_t  *pSpeed,
    rtk_port_duplex_t *pDuplex);

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
    uint32            *pRxStatus);

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
extern int32
dal_longan_port_cpuPortId_get(uint32 unit, rtk_port_t *pPort);

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
extern int32
dal_longan_port_cpuPortId_set(uint32 unit, rtk_port_t port);

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
extern int32
dal_longan_port_isolation_get(uint32 unit, rtk_port_t port, rtk_portmask_t *pPortmask);

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
extern int32
dal_longan_port_isolation_set(uint32 unit, rtk_port_t port, rtk_portmask_t *pPortmask);

/* Function Name:
 *      dal_longan_port_isolationExt_get
 * Description:
 *      Get the portmask of the port isolation
 * Input:
 *      unit                - unit id
 *      devID               - source unit id
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
extern int32
dal_longan_port_isolationExt_get(uint32 unit, uint32 devID, rtk_port_t srcPort, rtk_portmask_t *pPortmask);

/* Function Name:
 *      dal_longan_port_isolationExt_set
 * Description:
 *      Set the port isolation portmask for packet received from specified unit and port.
 * Input:
 *      unit            - unit id
 *      devID           - source unit id
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
extern int32
dal_longan_port_isolationExt_set(uint32 unit, uint32 devID, rtk_port_t srcPort, rtk_portmask_t *pPortmask);

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
 */
extern int32
dal_longan_port_isolation_add(uint32 unit, rtk_port_t port, rtk_port_t iso_port);

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
 */
extern int32
dal_longan_port_isolation_del(uint32 unit, rtk_port_t port, rtk_port_t iso_port);

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
extern int32
dal_longan_port_isolationRestrictRoute_get(uint32 unit, rtk_enable_t *pEnable);

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
extern int32
dal_longan_port_isolationRestrictRoute_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_longan_port_adminEnable_get
 * Description:
 *      Get port admin status of the specific port
 * Input:
 *      unit               - unit id
 *      port               - port id
 *      pEnabled          - pointer to the port admin status
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
extern int32
dal_longan_port_adminEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnabled);

/* Function Name:
 *      dal_longan_port_adminEnable_set
 * Description:
 *      Set port admin configuration of the specific port
 * Input:
 *      unit               - unit id
 *      port               - port id
 *      enabled            - port admin configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_longan_port_adminEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enabled);

/* Function Name:
 *      dal_longan_port_backpressureEnable_get
 * Description:
 *      Get the half duplex backpressure enable status of the specific port
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      pEnable       - - pointer to the enable status of backpressure in half duplex mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1. The mac local loopback enable status of the port is as following:
 *         - DISABLE
 *         - ENABLE
 *      2. Used to support backpressure in half mode.
 */
extern int32
dal_longan_port_backpressureEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

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
extern int32
dal_longan_port_backpressureEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

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
extern int32
dal_longan_port_linkChange_register(uint32 unit, dal_link_change_callback_f link_change_callback);

/* Function Name:
 *      dal_longan_port_linkChange_register
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
extern int32
dal_longan_port_linkChange_unregister(uint32 unit);

/* Function Name:
 *      dal_longan_port_vlanBasedIsolationEntry_get
 * Description:
 *      Set the statue of linkdown green feature of the specific port in the specific unit
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
extern int32
dal_longan_port_vlanBasedIsolationEntry_get(uint32 unit, uint32 index, rtk_port_vlanIsolationEntry_t* pEntry);

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
extern int32
dal_longan_port_vlanBasedIsolationEntry_set(uint32 unit, uint32 index, rtk_port_vlanIsolationEntry_t* pEntry);

/* Function Name:
 *      dal_longan_port_vlanBasedIsolation_vlanSource_get
 * Description:
 *      Get comparing VID type of VLAN-based port isolation
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of linkdown green feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
extern int32
dal_longan_port_vlanBasedIsolation_vlanSource_get(uint32 unit, rtk_port_vlanIsolationSrc_t *pVlanSrc);

/* Function Name:
 *      dal_longan_port_vlanBasedIsolation_vlanSource_set
 * Description:
 *      Set comparing VID type of VLAN-based port isolation
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
extern int32
dal_longan_port_vlanBasedIsolation_vlanSource_set(uint32 unit, rtk_port_vlanIsolationSrc_t vlanSrc);

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
extern int32
dal_longan_port_vlanBasedIsolationEgrBypass_get(uint32 unit, rtk_port_t port,rtk_enable_t *pEnable);

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
extern int32
dal_longan_port_vlanBasedIsolationEgrBypass_set(uint32 unit, rtk_port_t port,rtk_enable_t enable);

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
 */
extern int32
dal_longan_port_flowCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

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
 */
extern int32
dal_longan_port_flowCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

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
extern int32
dal_longan_port_linkMedia_get(uint32 unit, rtk_port_t port,
    rtk_port_linkStatus_t *pStatus, rtk_port_media_t *pMedia);

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
extern int32
dal_longan_port_fiberUnidirEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

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
extern int32
dal_longan_port_fiberUnidirEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

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
extern int32
dal_longan_port_phyAutoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

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
 *      None
 */
extern int32
dal_longan_port_phyForceModeAbility_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_speed_t    speed,
    rtk_port_duplex_t   duplex,
    rtk_enable_t        flowControl);

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
extern int32 dal_longan_port_init_check(uint32 unit);

/* Function Name:
 *      _dal_longan_port_swMacPollPhy
 * Description:
 *      polling 8295R link status for 96M
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
extern int32
_dal_longan_port_swMacPollPhy(uint32 unit, rtk_port_t port);

/* Function Name:
 *      _dal_longan_port_swChgSerdesMode
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
extern int32
_dal_longan_port_swChgSerdesMode(uint32 unit, rtk_port_t port);

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
extern int32
_dal_longan_port_swAutoDetectSdsMode(uint32 unit, rtk_port_t port);

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
extern int32 dal_longan_port_sem_lock(uint32 unit);

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
extern int32 dal_longan_port_sem_unlock(uint32 unit);


#endif /* __DAL_LONGAN_PORT_H__ */
