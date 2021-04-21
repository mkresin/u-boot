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
 * $Revision: 80411 $
 * $Date: 2017-07-06 17:27:33 +0800 (Thu, 06 Jul 2017) $
 *
 * Purpose : DRV APIs definition.
 *
 * Feature : SPI relative API
 *
 */
#ifndef __DRV_SPI_DEV4_H__
#define __DRV_SPI_DEV4_H__

/*
* Include Files
*/
//#include <common/rt_error.h>

/*
* Symbol Definition
*/
#define RTL9300_INT_SPI_CTRL1_DATA_WIDTH_OFFSET                     (19)
#define RTL9300_INT_SPI_CTRL1_DATA_WIDTH_MASK                       (0x1FF << RTL9300_INT_SPI_CTRL1_DATA_WIDTH_OFFSET)
#define RTL9300_INT_SPI_CTRL1_ADDR_WIDTH_OFFSET                     (17)
#define RTL9300_INT_SPI_CTRL1_ADDR_WIDTH_MASK                       (0x3 << RTL9300_INT_SPI_CTRL1_ADDR_WIDTH_OFFSET)

/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

int32 spi_dev4Reg_read(uint32 unit, uint32 mAddrs, uint8 *buff);
int32 spi_dev4Reg_write(uint32 unit, uint32 mAddrs, uint8 *buff);
int32 spi_dev4_init(uint32 unit);


#endif /* __DRV_SPI_DEV4_H__ */


