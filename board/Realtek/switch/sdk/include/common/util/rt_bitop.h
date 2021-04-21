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
 * Purpose : BIT operation utility
 *
 * Feature :
 *
 */

#ifndef __COMMON_RT_BITOP_H__
#define __COMMON_RT_BITOP_H__

/*
 * Include Files
 */
#include <common/type.h>

/*
 * Symbol Definition
 */

#define MASK_BIT_LEN			32

/* bit masp */
#define BITMASK_1B   0x00000001
#define BITMASK_2B   0x00000003
#define BITMASK_3B   0x00000007
#define BITMASK_4B   0x0000000F
#define BITMASK_5B   0x0000001F
#define BITMASK_6B   0x0000003F
#define BITMASK_7B   0x0000007F
#define BITMASK_8B   0x000000FF
#define BITMASK_9B   0x000001FF
#define BITMASK_10B  0x000003FF
#define BITMASK_11B  0x000007FF
#define BITMASK_12B  0x00000FFF
#define BITMASK_13B  0x00001FFF
#define BITMASK_14B  0x00003FFF
#define BITMASK_15B  0x00007FFF
#define BITMASK_16B  0x0000FFFF
#define BITMASK_17B  0x0001FFFF
#define BITMASK_18B  0x0003FFFF
#define BITMASK_19B  0x0007FFFF
#define BITMASK_20B  0x000FFFFF
#define BITMASK_21B  0x001FFFFF
#define BITMASK_22B  0x003FFFFF
#define BITMASK_23B  0x007FFFFF
#define BITMASK_24B  0x00FFFFFF
#define BITMASK_25B  0x01FFFFFF
#define BITMASK_26B  0x03FFFFFF
#define BITMASK_27B  0x07FFFFFF
#define BITMASK_28B  0x0FFFFFFF
#define BITMASK_29B  0x1FFFFFFF
#define BITMASK_30B  0x3FFFFFFF
#define BITMASK_31B  0x7FFFFFFF
#define BITMASK_32B  0xFFFFFFFF


/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

/* structure of bitmap */
typedef uint32                                  rtk_bitmap_t;

/* bitmap width in bits */
#define BITMAP_WIDTH                            (sizeof(rtk_bitmap_t) * 8)

/* Input number of indexes to this macro, it returns maximum number of rtk_bitmap_t that required for the array.
 * E.g.     rtk_bitmap_t        xxx[BITMAP_ARRAY_CNT(XXX_MAX_NUM)];
 */
#define BITMAP_ARRAY_CNT(_num_of_indexes)       ((_num_of_indexes + (BITMAP_WIDTH - 1)) / BITMAP_WIDTH)

/* Input number of indexes to this macro, it returns total array size in bytes.
 * E.g.     rtk_bitmap_t        *pxxx = osal_alloc(BITMAP_ARRAY_SIZE(XXX_MAX_NUM));
 */
#define BITMAP_ARRAY_SIZE(_num_of_indexes)      (BITMAP_ARRAY_CNT(_num_of_indexes) * sizeof(rtk_bitmap_t))

/*
 * Bitmap operations
 *  array  - rtk_bitmap_t array.
 *  index  - index number.
 *  length - bitmap array size.
 */
#define BITMAP_IS_SET(array, index)\
    (array[index/BITMAP_WIDTH] & (0x1 << (index%BITMAP_WIDTH)))
#define BITMAP_IS_CLEAR(array, index)\
    !(array[index/BITMAP_WIDTH] & (0x1 << (index%BITMAP_WIDTH)))
#define BITMAP_SET(array, index) do {\
    array[index/BITMAP_WIDTH] = \
    (array[index/BITMAP_WIDTH] | (0x1 << (index%BITMAP_WIDTH))); } while(0)
#define BITMAP_CLEAR(array, index) do {\
    array[index/BITMAP_WIDTH] = \
    (array[index/BITMAP_WIDTH] & ~(0x1 << (index%BITMAP_WIDTH))); } while(0)

#define BITMAP_ASSIGN(dstArray, srcArray, length) \
do { \
    osal_memcpy(dstArray, srcArray, length*sizeof(rtk_bitmap_t)); \
} while(0)

#define BITMAP_RESET(dstArray, length) \
do { \
    osal_memset(dstArray, 0, length*sizeof(rtk_bitmap_t)); \
} while(0)

#define BITMAP_COMPARE(dstArray, srcArray, length)  osal_memcmp(dstArray, srcArray, length*sizeof(rtk_bitmap_t))

#define BITMAP_AND(dstArray, srcArray, length) \
do { \
    uint32  array_index;\
    for (array_index = 0; array_index < length; array_index++)\
    {\
        dstArray[array_index] &= srcArray[array_index];\
    }\
} while(0)

#define BITMAP_OR(dstArray, srcArray, length) \
do { \
    uint32  array_index;\
    for (array_index = 0; array_index < length; array_index++)\
    {\
        dstArray[array_index] |= srcArray[array_index];\
    }\
} while(0)

#define BITMAP_XOR(dstArray, srcArray, length) \
do { \
    uint32  array_index;\
    for (array_index = 0; array_index < length; array_index++)\
    {\
        dstArray[array_index] ^= srcArray[array_index];\
    }\
} while(0)

#define BITMAP_REMOVE(dstArray, srcArray, length) \
do { \
    uint32  array_index;\
    for (array_index = 0; array_index < length; array_index++)\
    {\
        dstArray[array_index] &= ~srcArray[array_index];\
    }\
} while(0)

#define BITMAP_REVERT(dstArray, length) \
do { \
    uint32  array_index;\
    for (array_index = 0; array_index < length; array_index++)\
    {\
        dstArray[array_index] = ~dstArray[array_index];\
    }\
} while(0)

/*
 * Function Declaration
 */
/* Function Name:
 *      bitop_numberOfSetBits
 * Description:
 *      caculate how much bit is set in this word
 * Input:
 *      value       - the word to be handled
 * Output:
 *      None
 * Return:
 *      number bits are set
 * Note:
 *      None
 */
extern uint32
rt_bitop_numberOfSetBits(rtk_bitmap_t value);

/* Function Name:
 *      bitop_numberOfSetBitsInArray
 * Description:
 *      caculate how much bit is set in this array
 * Input:
 *      pArray      - the word to be handled
 *      arraySize   - Size of Array
 * Output:
 *      None
 * Return:
 *      number bits are set
 * Note:
 *      None
 */
extern uint32
bitop_numberOfSetBitsInArray(rtk_bitmap_t *pArray, uint32 arraySize);

/* Function Name:
 *      rt_bitop_findFirstBit
 * Description:
 *      find first bits
 * Input:
 *      value       - the word to be handled
 * Output:
 *      None
 * Return:
 *          position of first bit
 *          if not bit is set, will return (-1)
 * Note:
 *      None
 */
extern int32
rt_bitop_findFirstBit(rtk_bitmap_t value);

/* Function Name:
 *      rt_bitop_findFirstBitInArray
 * Description:
 *      find first bits in Array
 * Input:
 *      pArray       - the array to be handled
 * Output:
 *      None
 * Return:
 *          position of first bit
 *          if not bit is set, will return (-1)
 * Note:
 *      None
 */
extern int32
rt_bitop_findFirstBitInAaray(rtk_bitmap_t *pArray, uint32 arraySize);

/* Function Name:
 *      rt_bitop_findFirstZeroBitInAaray
 * Description:
 *      find first zero bit in Array
 * Input:
 *      pArray       - the array to be handled
 * Output:
 *
 * Return:
 *          position of first zero bit
 *          if not bit is set, will return (-1)
 * Note:
 *      None
 */
extern int32
rt_bitop_findFirstZeroBitInAaray(rtk_bitmap_t *pArray, uint32 arraySize);

/* Function Name:
 *      rt_bitop_findLastBitInArray
 * Description:
 *      find last bits in Array
 * Input:
 *      pArray       - the array to be handled
 * Output:
 *      None
 * Return:
 *          position of last bit
 *          if not bit is set, will return (-1)
 * Note:
 *      None
 */
extern int32
rt_bitop_findLastBitInAaray(rtk_bitmap_t *pArray, uint32 arraySize);

/* Function Name:
 *      rt_bitop_findIdxBitInAaray
 * Description:
 *      find the idx bits in Array
 * Input:
 *      pArray       - the array to be handled
 * Output:
 *
 * Return:
 *          position of the idx bit
 *          if not bit is set, will return (-1)
 * Note:
 *      None
 */
extern int32
rt_bitop_findIdxBitInAaray(rtk_bitmap_t *pArray, uint32 arraySize, uint32 idx);

#endif /* __COMMON_RT_BITOP_H__ */
