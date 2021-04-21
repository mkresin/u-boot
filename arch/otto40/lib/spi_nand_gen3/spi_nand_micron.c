#include <spi_nand/spi_nand_ctrl.h>
#include <spi_nand/spi_nand_common.h>
#include <util.h>
#ifdef CONFIG_SPI_NAND_FLASH_INIT_FIRST
    #include <spi_nand/spi_nand_util.h>
#endif
#if defined(CONFIG_UNDER_UBOOT) && !defined(CONFIG_SPI_NAND_FLASH_INIT_FIRST)
    #include <spi_nand/spi_nand_blr_util.h>
    #include <spi_nand/spi_nand_symb_func.h>        
#endif

/***********************************************
  *  MICRON's ID Definition
  ***********************************************/
#define MID_MICRON          (0x2C)
#define DID_MT29F1G01ABAFDX (0x14)
#define DID_MT29F2G01ABAGDX (0x24)
#define DID_MT29F4G01ADAGDX (0x36)

// policy decision
    //input: #define NSU_PROHIBIT_QIO, or NSU_PROHIBIT_DIO  (in project/info.in)
    //       #define NSU_MICRON_USING_QIO, NSU_MICRON_USING_DIO, NSU_MICRON_USING_SIO  (in project/info.in)
    //       #define NSU_DRIVER_IN_ROM, IS_RECYCLE_SECTION_EXIST (in template/info.in)
    //       #define NSU_USING_SYMBOL_TABLE_FUNCTION (in project/info.in)  

    //output: #define __DEVICE_REASSIGN, __DEVICE_USING_SIO, __DEVICE_USING_DIO, and __DEVICE_USING_QIO
    //        #define __SECTION_INIT_PHASE, __SECTION_INIT_PHASE_DATA
    //        #define __SECTION_RUNTIME, __SECTION_RUNTIME_DATA

#ifdef NSU_DRIVER_IN_ROM
    #define __SECTION_INIT_PHASE      SECTION_SPI_NAND
    #define __SECTION_INIT_PHASE_DATA SECTION_SPI_NAND_DATA
    #define __SECTION_RUNTIME         SECTION_SPI_NAND
    #define __SECTION_RUNTIME_DATA    SECTION_SPI_NAND_DATA
    #if defined(NSU_MICRON_USING_QIO) || defined(NSU_MICRON_USING_DIO)
        #error 'lplr should not run at ...'
    #endif
    #ifdef IS_RECYCLE_SECTION_EXIST
        #error 'lplr should not have recycle section ...'
    #endif
    #define __DEVICE_USING_SIO 1
    #define __DEVICE_USING_DIO 0
    #define __DEVICE_USING_QIO 0
#else
    #ifdef NSU_USING_SYMBOL_TABLE_FUNCTION
        #define __DEVICE_REASSIGN 1
    #endif
    #ifdef IS_RECYCLE_SECTION_EXIST
        #define __SECTION_INIT_PHASE        SECTION_RECYCLE
        #define __SECTION_INIT_PHASE_DATA   SECTION_RECYCLE_DATA
        #define __SECTION_RUNTIME           SECTION_UNS_TEXT
        #define __SECTION_RUNTIME_DATA      SECTION_UNS_RO
    #else
        #define __SECTION_INIT_PHASE
        #define __SECTION_INIT_PHASE_DATA
        #define __SECTION_RUNTIME
        #define __SECTION_RUNTIME_DATA
    #endif

    #ifdef NSU_MICRON_USING_QIO
        #if defined(NSU_PROHIBIT_QIO) && defined(NSU_PROHIBIT_DIO)
            #define __DEVICE_USING_SIO 1
            #define __DEVICE_USING_DIO 0
            #define __DEVICE_USING_QIO 0
        #elif defined(NSU_PROHIBIT_QIO) 
            #define __DEVICE_USING_SIO 0
            #define __DEVICE_USING_DIO 1
            #define __DEVICE_USING_QIO 0
        #else
            #define __DEVICE_USING_SIO 0
            #define __DEVICE_USING_DIO 0
            #define __DEVICE_USING_QIO 1
        #endif
    #elif defined(NSU_MICRON_USING_DIO)
        #if defined(NSU_PROHIBIT_DIO)
            #define __DEVICE_USING_SIO 1
            #define __DEVICE_USING_DIO 0
            #define __DEVICE_USING_QIO 0
        #else
            #define __DEVICE_USING_SIO 0
            #define __DEVICE_USING_DIO 1
            #define __DEVICE_USING_QIO 0
        #endif
    #else
        #define __DEVICE_USING_SIO 1
        #define __DEVICE_USING_DIO 0
        #define __DEVICE_USING_QIO 0
    #endif
#endif

#ifdef CONFIG_SPI_NAND_FLASH_INIT_FIRST
#if __DEVICE_USING_QIO
__SECTION_INIT_PHASE_DATA
spi_nand_cmd_info_t micron_qio_cmd_info = {
    .w_cmd = PROGRAM_LOAD_X4_OP,
    .w_addr_io = SIO_WIDTH,
    .w_data_io = QIO_WIDTH,
    .r_cmd = FAST_READ_QIO_OP,
    .r_addr_io = QIO_WIDTH,
    .r_data_io = QIO_WIDTH,
    .r_dummy_cycles  = 16,
};
#endif

#if !__DEVICE_REASSIGN
    __SECTION_INIT_PHASE_DATA spi_nand_model_info_t micron_stack2die_model;
#endif

__SECTION_INIT_PHASE_DATA
spi_nand_flash_info_t micron_chip_info[] = {
    {
        .man_id              = MID_MICRON, 
        .dev_id              = DID_MT29F1G01ABAFDX,
        ._num_block          = SNAF_MODEL_NUM_BLK_1024,
        ._num_page_per_block = SNAF_MODEL_NUM_PAGE_64,
        ._page_size          = SNAF_MODEL_PAGE_SIZE_2048B,
        ._spare_size         = SNAF_MODEL_SPARE_SIZE_64B,
        ._oob_size           = SNAF_MODEL_OOB_SIZE(24),
        ._ecc_ability        = ECC_MODEL_6T,        
        #if __DEVICE_REASSIGN
            ._ecc_encode     = VZERO,
            ._ecc_decode     = VZERO,
            ._reset          = VZERO,
            ._cmd_info       = VZERO,
            ._model_info     = VZERO,
        #else
            ._ecc_encode     = ecc_encode_bch,
            ._ecc_decode     = ecc_decode_bch,
            ._reset          = nsu_reset_spi_nand_chip,
            ._model_info     = &snaf_rom_general_model,
            #if __DEVICE_USING_SIO
                ._cmd_info   = &nsc_sio_cmd_info,
            #elif __DEVICE_USING_DIO
                ._cmd_info   = &nsc_dio_cmd_info,
            #elif __DEVICE_USING_QIO
                ._cmd_info   = &micron_qio_cmd_info,
            #endif
        #endif
    },
    {
        .man_id              = MID_MICRON, 
        .dev_id              = DID_MT29F2G01ABAGDX,
        ._num_block          = SNAF_MODEL_NUM_BLK_2048,
        ._num_page_per_block = SNAF_MODEL_NUM_PAGE_64,
        ._page_size          = SNAF_MODEL_PAGE_SIZE_2048B,
        ._spare_size         = SNAF_MODEL_SPARE_SIZE_64B,
        ._oob_size           = SNAF_MODEL_OOB_SIZE(24),
        ._ecc_ability        = ECC_MODEL_6T,        
        #if __DEVICE_REASSIGN
            ._ecc_encode     = VZERO,
            ._ecc_decode     = VZERO,
            ._reset          = VZERO,
            ._cmd_info       = VZERO,
            ._model_info     = VZERO,
        #else
            ._ecc_encode     = ecc_encode_bch,
            ._ecc_decode     = ecc_decode_bch,
            ._reset          = nsu_reset_spi_nand_chip,
            ._model_info     = &snaf_rom_general_model,
            #if __DEVICE_USING_SIO
                ._cmd_info   = &nsc_sio_cmd_info,
            #elif __DEVICE_USING_DIO
                ._cmd_info   = &nsc_dio_cmd_info,
            #elif __DEVICE_USING_QIO
                ._cmd_info   = &micron_qio_cmd_info,
            #endif
        #endif
    },
#if defined(NSU_DRIVER_IN_ROM) || (defined(CONFIG_UNDER_UBOOT) && defined(CONFIG_SPI_NAND_FLASH_INIT_FIRST))
    {
        .man_id              = MID_MICRON, 
        .dev_id              = DID_MT29F4G01ADAGDX,
        ._num_block          = SNAF_MODEL_NUM_BLK_4096,
        ._num_page_per_block = SNAF_MODEL_NUM_PAGE_64,
        ._page_size          = SNAF_MODEL_PAGE_SIZE_2048B,
        ._spare_size         = SNAF_MODEL_SPARE_SIZE_64B,
        ._oob_size           = SNAF_MODEL_OOB_SIZE(24),
        ._ecc_ability        = ECC_MODEL_6T,        
        #if __DEVICE_REASSIGN
            ._ecc_encode     = VZERO,
            ._ecc_decode     = VZERO,
            ._reset          = VZERO,
            ._cmd_info       = VZERO,
            ._model_info     = VZERO,
        #else
            ._ecc_encode     = ecc_encode_bch,
            ._ecc_decode     = ecc_decode_bch,
            ._reset          = nsu_reset_spi_nand_chip,
            ._model_info     = &micron_stack2die_model,
            #if __DEVICE_USING_SIO
                ._cmd_info   = &nsc_sio_cmd_info,
            #elif __DEVICE_USING_DIO
                ._cmd_info   = &nsc_dio_cmd_info,
            #elif __DEVICE_USING_QIO
                ._cmd_info   = &micron_qio_cmd_info,
            #endif
        #endif
    },
#endif
    {//This is for Default
        .man_id              = MID_MICRON, 
        .dev_id              = DEFAULT_DATA_BASE,
        ._num_block          = SNAF_MODEL_NUM_BLK_1024,
        ._num_page_per_block = SNAF_MODEL_NUM_PAGE_64,
        ._page_size          = SNAF_MODEL_PAGE_SIZE_2048B,
        ._spare_size         = SNAF_MODEL_SPARE_SIZE_64B,
        ._oob_size           = SNAF_MODEL_OOB_SIZE(24),
        ._ecc_ability        = ECC_MODEL_6T,
        #if __DEVICE_REASSIGN
            ._ecc_encode     = VZERO,
            ._ecc_decode     = VZERO,
            ._reset          = VZERO,
            ._cmd_info       = VZERO,
            ._model_info     = VZERO,
        #else
            ._ecc_encode     = ecc_encode_bch,
            ._ecc_decode     = ecc_decode_bch,
            ._reset          = nsu_reset_spi_nand_chip,
            ._cmd_info       = &nsc_sio_cmd_info,
            ._model_info     = &snaf_rom_general_model,
        #endif
    },
};
#endif // CONFIG_SPI_NAND_FLASH_INIT_FIRST

#if __DEVICE_USING_QIO
__SECTION_INIT_PHASE void
micron_quad_enable(u32_t cs)
{
    u32_t feature_addr=0xB0;
    u32_t value = 1;
    value |= nsu_get_feature_reg(cs, feature_addr);
    nsu_set_feature_reg(cs, feature_addr, value);
}
#endif


__SECTION_INIT_PHASE u32_t
micron_read_id(u32_t cs)
{
    u32_t man_addr = 0x00;
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(2));
    u32_t r_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(2));
    u32_t ret = nsu_read_spi_nand_id(cs, man_addr, w_io_len, r_io_len);
    return ((ret>>16)&0xFFFF);
}

__SECTION_INIT_PHASE void
micron_select_die(u32_t cs, u32_t die_num)
{
    nsu_set_feature_reg(cs, 0xD0, (die_num<<6));
}

#if defined(NSU_DRIVER_IN_ROM) || (defined(CONFIG_UNDER_UBOOT) && defined(CONFIG_SPI_NAND_FLASH_INIT_FIRST))
__SECTION_RUNTIME void
micron_stack2die_page_read(spi_nand_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr)
{
    snaf_page_read(info, dma_addr, nsu_s2d_addr_trans(info, blk_pge_addr, micron_select_die));
}

__SECTION_RUNTIME s32_t
micron_stack2die_page_write(spi_nand_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr)
{
    return snaf_page_write(info, dma_addr, nsu_s2d_addr_trans(info, blk_pge_addr, micron_select_die));
}

__SECTION_RUNTIME s32_t
micron_stack2die_page_read_with_ecc_decode(spi_nand_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr, void *p_eccbuf)
{
    return snaf_page_read_with_ecc_decode(info, dma_addr, nsu_s2d_addr_trans(info, blk_pge_addr, micron_select_die), p_eccbuf);
}

__SECTION_RUNTIME s32_t
micron_stack2die_page_write_with_ecc_encode(spi_nand_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr, void *p_eccbuf)
{
    return snaf_page_write_with_ecc_encode(info, dma_addr, nsu_s2d_addr_trans(info, blk_pge_addr, micron_select_die), p_eccbuf);
}

__SECTION_RUNTIME void
micron_stack2die_pio_read_data(spi_nand_flash_info_t *info, void *ram_addr, u32_t wr_bytes, u32_t blk_pge_addr, u32_t col_addr)
{
    snaf_pio_read_data(info, ram_addr, wr_bytes, nsu_s2d_addr_trans(info, blk_pge_addr, micron_select_die), col_addr);
}

__SECTION_RUNTIME s32_t
micron_stack2die_pio_write_data(spi_nand_flash_info_t *info, void *ram_addr, u32_t wr_bytes, u32_t blk_pge_addr, u32_t col_addr)
{
    return snaf_pio_write_data(info, ram_addr, wr_bytes, nsu_s2d_addr_trans(info, blk_pge_addr, micron_select_die), col_addr);
}

__SECTION_RUNTIME s32_t
micron_stack2die_block_erase(spi_nand_flash_info_t *info, u32_t blk_pge_addr)
{
    return nsu_block_erase(info, nsu_s2d_addr_trans(info, blk_pge_addr, micron_select_die));
}

__SECTION_INIT_PHASE_DATA 
spi_nand_model_info_t micron_stack2die_model = {
    ._pio_write = micron_stack2die_pio_write_data,
    ._pio_read = micron_stack2die_pio_read_data,
    ._page_read = micron_stack2die_page_read,
    ._page_write = micron_stack2die_page_write,
    ._page_read_ecc = micron_stack2die_page_read_with_ecc_decode,
    ._page_write_ecc = micron_stack2die_page_write_with_ecc_encode,
    ._block_erase = micron_stack2die_block_erase,
    ._wait_spi_nand_ready = nsc_wait_spi_nand_oip_ready,
};
#endif //defined(NSU_DRIVER_IN_ROM) || (defined(CONFIG_UNDER_UBOOT) && defined(CONFIG_SPI_NAND_FLASH_INIT_FIRST))

__SECTION_INIT_PHASE void
micron_stack2die_specific_setting(u32_t cs)
{
    //For StackDie, preset the Die1's status as Die0
    micron_select_die(cs,1);
    nsu_block_unprotect(cs);
    nsu_disable_on_die_ecc(cs);

    //Recover to Die0
    micron_select_die(cs,0);
}

#ifdef CONFIG_SPI_NAND_FLASH_INIT_FIRST
__SECTION_INIT_PHASE spi_nand_flash_info_t *
probe_micron_spi_nand_chip(void)
{
    nsu_reset_spi_nand_chip(0);    
    u32_t rdid = micron_read_id(0);
    u8_t mid = ((rdid>>8)&0xFF);
    u8_t did = (rdid&0xFF);
    if(MID_MICRON != mid) return VZERO;

    u32_t i;   
    for(i=0 ; i<ELEMENT_OF_SNAF_INFO(micron_chip_info) ; i++){
        if( (micron_chip_info[i].dev_id == did) || (micron_chip_info[i].dev_id == DEFAULT_DATA_BASE)){
            #if __DEVICE_REASSIGN
                #if __DEVICE_USING_SIO
                    micron_chip_info[i]._cmd_info = _nsu_cmd_info_ptr;
                #elif __DEVICE_USING_DIO
                    micron_chip_info[i]._cmd_info = _nsu_dio_cmd_info_ptr;
                #elif __DEVICE_USING_QIO
                    micron_chip_info[i]._cmd_info = &micron_qio_cmd_info;
                #endif 
                micron_chip_info[i]._model_info = &nsu_model_info;
                micron_chip_info[i]._reset = _nsu_reset_ptr;
                micron_chip_info[i]._ecc_encode= _nsu_ecc_encode_ptr;
                micron_chip_info[i]._ecc_decode= _nsu_ecc_decode_ptr;
            #endif       
            #if __DEVICE_USING_QIO
                micron_quad_enable(0);
            #endif
            nsu_block_unprotect(0);
            nsu_enable_on_die_ecc(0);
            if(DID_MT29F4G01ADAGDX == did){
                micron_stack2die_specific_setting(0);
            }           
            return &micron_chip_info[i];
        }
    }
    return VZERO;
}
REG_SPI_NAND_PROBE_FUNC(probe_micron_spi_nand_chip);
#endif   // CONFIG_SPI_NAND_FLASH_INIT_FIRST
#ifdef CONFIG_SPI_NAND_FLASH_INIT_REST
int
micron_init_rest(void)
{
    u32_t cs=1; 

    // check ID, cs0 and cs1 should be identical
    u32_t rdid = micron_read_id(cs);
    //u32_t cs0_id = (_spi_nand_info->man_id<<8) | _spi_nand_info->dev_id;
    if(((rdid>>8)!=MID_MICRON) && ((rdid&0xFF)!=_spi_nand_info->dev_id)) 
      { return 0; }

    // reset
    nsu_reset_spi_nand_chip(cs);

    // multi-IO enabled
#if __DEVICE_USING_QIO
    micron_quad_enable(cs);
#endif  // __DEVICE_USING_QIO
    
    // misc
    nsu_block_unprotect(cs);
    nsu_enable_on_die_ecc(cs);
    if(DID_MT29F4G01ADAGDX == _spi_nand_info->dev_id){
                micron_stack2die_specific_setting(cs);
    }

    return 1;
}
REG_SPI_NAND_INIT_REST_FUNC(micron_init_rest);
#endif // CONFIG_SPI_NAND_FLASH_INIT_REST
