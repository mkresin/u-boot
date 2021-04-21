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
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 * Purpose : Define the PHY hardware related function
 *
 * Feature : SDK common utility
 *
 */

#ifndef __IOAL_PHY_RESET_H__
#define __IOAL_PHY_RESET_H__


/* Function Name:
 *      ioal_phy_reset
 * Description:
 *      Reset all PHY chips
 * Input:
 *      unit    -   unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   Reset success
 *      RT_ERR_CHIP_NOT_SUPPORTED   Not support hardware reset PHY
 * Note:
 *    None
 */
extern int32 ioal_phy_reset(uint32 unit);

#endif /* __IOAL_PHY_RESET_H__ */
