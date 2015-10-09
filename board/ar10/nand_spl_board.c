#include <config.h>
#include <common.h>
#include <command.h>
#include <asm/addrspace.h>
#include <asm/ar10.h>
#include <environment.h>
#include <nand.h>



void nand_gpio_init(void)
{

}
									  

void *malloc(unsigned int size)
{
   
    return NULL;
}

static void nand_read_page(u32 page_addr, u32 dest_addr)
{
   int i;
   u8 *tmp;
   u8 col_addr_num;
     u8 page_addr_num;

   if(CONFIG_NAND_PAGE_SIZE<=0x200){
     col_addr_num=1;

     if(CONFIG_NAND_FLASH_SIZE<32){
        page_addr_num=2;
      }else
        page_addr_num=3;
   }else{
     col_addr_num=2;

     if(CONFIG_NAND_FLASH_SIZE<128){
          page_addr_num=2;
      }else{
        page_addr_num=3;
      }
   }
   NAND_CE_SET;

   NAND_SETCLE;
   NAND_WRITE(WRITE_CMD,0);
   NAND_CLRCLE;
   NAND_SETALE;
   for(i=0;i<col_addr_num;i++){
    NAND_WRITE(WRITE_ADDR,0);
  }
   for(i=0;i<page_addr_num;i++){
     NAND_WRITE(WRITE_ADDR,(u8)((page_addr>>(i*8)) & 0xff ));
   }
   NAND_CLRALE;

   NAND_SETCLE;
   if(CONFIG_NAND_PAGE_SIZE>0x200){
     NAND_WRITE(WRITE_CMD,0x30);
   }
  NAND_CLRCLE;
  while(!NAND_READY){}

   /* Read page */
   tmp = (u8*)dest_addr;
   for (i = 0; i < CONFIG_NAND_PAGE_SIZE; i++)
   {
     NAND_READ(READ_DATA, *tmp++);
   }
   NAND_CE_CLEAR;

   while(!NAND_READY){}
}

void nand_spl_init(void)
{
     nand_boot();
	
}
