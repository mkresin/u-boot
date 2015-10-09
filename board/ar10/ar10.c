/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <config.h>
#include <common.h>
#include <command.h>
#include <asm/addrspace.h>
#include <environment.h>
#include <nand.h>
#include <spi_flash.h>

#if !defined(DEBUG_ENABLE_BOOTSTRAP_PRINTF) && defined(CFG_BOOTSTRAP_CODE)                                                                            
#define printf                                                                                                                                        
#endif   

#ifndef CONFIG_ENV_SPI_BUS
# define CONFIG_ENV_SPI_BUS 0
#endif
#ifndef CONFIG_ENV_SPI_CS
# define CONFIG_ENV_SPI_CS      0
#endif
#ifndef CONFIG_ENV_SPI_MAX_HZ
# define CONFIG_ENV_SPI_MAX_HZ  1000000
#endif
#ifndef CONFIG_ENV_SPI_MODE
# define CONFIG_ENV_SPI_MODE    SPI_MODE_3
#endif

#ifdef CONFIG_NOR_FLASH_AUTO
void save_extra_env()
{
   extern flash_info_t flash_info[CFG_MAX_FLASH_BANKS];
   extern xway_flash_partition_map_info flash_map[];
   int i,j,total_db,total_part;
   int mtd_part;
   u32  tmp;
   char strTemp[32], strCommand[128];
   char strimg_addr[32],strimg_size[32],strimg_end[32];
   i=0;
   while(flash_map[i].size !=0){
    	 if(flash_map[i].size == flash_info[0].size>>20) break;
    	 i++;
    }
   total_db = 0;
   mtd_part = -1; 
   total_part = 0;
   for(j=0;j<XWAY_MAX_BLOCK_NUM;j++){
   	     if(strcmp(flash_map[i].block[j].blockName,"")!=0){
   	     	 total_db++;
   	     	 
   	     	 sprintf(strTemp,"data_block%d",j);
   	     	 if(!getenv(strTemp)) setenv(strTemp, flash_map[i].block[j].blockName);
           sprintf(strTemp,"f_%s_addr",flash_map[i].block[j].blockName);
           sprintf(strimg_addr,"0x%08x",flash_map[i].block[j].startAddr);
           if(!getenv(strTemp)) setenv(strTemp,strimg_addr);
           if(flash_map[i].block[j].mtdNum!=mtd_part){
   	     	    mtd_part = flash_map[i].block[j].mtdNum;
   	     	    total_part ++;
   	     	    sprintf(strTemp, "part%d_begin", j);
   	     	    if(!getenv(strTemp)) setenv(strTemp,strimg_addr);	
   	     	 }
           
           	sprintf(strTemp,"f_%s_size",flash_map[i].block[j].blockName);
            sprintf(strimg_size,"0x%08x",flash_map[i].block[j].sizeBytes);
            if(!getenv(strTemp)) setenv(strTemp,strimg_size);
            sprintf(strTemp,"f_%s_end",flash_map[i].block[j].blockName);
            sprintf(strimg_end,"0x%08x", flash_map[i].block[j].startAddr + flash_map[i].block[j].sizeBytes - 1);
            if(!getenv(strTemp)) setenv(strTemp,strimg_end);
             	
            if(strcmp(flash_map[i].block[j].blockName,"ubootconfig")==0) {
           	   sprintf(strCommand,"prot off 0x%08x %s;erase 0x%08x %s 1",\
           	           flash_map[i].block[j].startAddr,strimg_end,\
           	           flash_map[i].block[j].startAddr,strimg_end);
           	   sprintf(strTemp,"reset_uboot_config");
           	   if(!getenv(strTemp)) setenv(strTemp,strCommand);        
           	
            }
            
                       
        }
    } 
    sprintf(strTemp, "%d", total_db);
    if(!getenv(strTemp)) setenv("total_db", strTemp);
    sprintf(strTemp, "%d", total_part);
    if(!getenv(strTemp)) setenv("total_part", strTemp);
    sprintf(strTemp, "0x%08x", 0xb0000000 + flash_info[0].size);
    if(!getenv(strTemp)) setenv("flash_end", strTemp);
    sprintf(strTemp, "mtdparts");
    if(!getenv(strTemp)) setenv(strTemp,flash_map[i].mtdparts);	
    	
   return;
}
#endif



phys_size_t initdram(int board_type)
{
	return (1024*1024*CONFIG_IFX_MEMORY_SIZE);
}


int flash_probe(void)
{
     return 1;
}


void show_boot_progress(int arg)
{
  return;
}


int checkboard (void)
{

    printf("CLOCK CPU %dM RAM %dM\n",CPU_CLOCK_RATE/1000000,RAM_CLOCK_RATE/1000000);
   
    //*AR10_PMU_CLKGCR1_A |= ((1<<10)|(1<<5)) ; /*power up EBU*/ 
    REG32(0xBF102024) = REG32(0xBF102020) | 0x00000400; //writing 1 in the PMU_CLKGCR1_A_ADDR[10]
	REG32(0xBF102024) = REG32(0xBF102020) | 0x00000020; //writing 1 in the PMU_CLKGCR1_A_ADDR[5]
		
	*AR10_EBU_CLC = 0;
    *AR10_EBU_ADDR_SEL_0 = 0x10000011;
    *AR10_EBU_ADDR_SEL_1 = 0x14000021;  
	  return 0;
}


#ifdef CONFIG_NAND_FLASH
void nand_gpio_init(void)
{
	 /* GPIO 13 NAND_ALE*/
	 *AR10_GPIO_P0_DIR     |= 1 << 13;
	 *AR10_GPIO_P0_ALTSEL0 |= 1 << 13;
	 *AR10_GPIO_P0_ALTSEL1 &= ~(1 << 13);
	 *AR10_GPIO_P0_OD      |= 1 << 13;
	
	 /*GPIO 24 NAND_CLE */
	 *AR10_GPIO_P1_DIR     |= 1 << 8;
	 *AR10_GPIO_P1_ALTSEL0 |= 1 << 8;
	 *AR10_GPIO_P1_ALTSEL1 &= ~(1 << 8);
	 *AR10_GPIO_P1_OD      |= 1 << 8;
	 
	 /*GPIO 48, NAND Ready_Busy*/
	 /*GPIO 49, NAND_RD*/
	 /*GPIO 50, NAND_D1*/
	 /*GPIO 51, NAND_D0*/
	 /*GPIO 52, NAND_D2*/
	 /*GPIO 53, NAND_D2*/
	 /*GPIO 54, NAND_D6*/
	 /*GPIO 55, NAND_D5*/
	 /*GPIO 56, NAND_D5*/
	 /*GPIO 57, NAND_D3*/
	 /*GPIO 59, NAND_WR*/
	 /*GPIO 60, NAND_WP*/
	 /*GPIO 61, NAND_SE*/
	 
	 *AR10_GPIO_P3_DIR     = 0x3ffe;
	 *AR10_GPIO_P3_ALTSEL0 = 0x3fff;
	 *AR10_GPIO_P3_ALTSEL1 = 0;
	 *AR10_GPIO_P3_OD      = 0x3ffe;
#if 0	 
#ifdef CONFIG_NAND_CS0
    /*GPIO 58, NAND_CS*/
     *AR10_GPIO_P3_DIR     |= 1<<10;
	 *AR10_GPIO_P3_ALTSEL0 |= 1<<10;
	 *AR10_GPIO_P3_ALTSEL1 &= ~( 1<<10 );
#else
    /*GPIO 23, NAND_CS1*/
   *AR10_GPIO_P1_DIR     |= 1 << 7;
	 *AR10_GPIO_P1_ALTSEL0 |= 1 << 7;
	 *AR10_GPIO_P1_ALTSEL1 &= ~(1 << 7); 
#endif	 
#endif	
}
#endif



#ifdef CONFIG_LANTIQ_SPI

int spi_gpio_init(unsigned int cs)
{

    *AR10_PMU_CLKGCR1_A |=1<<8;
	
   /* p0.15 SPI_CS1(EEPROM) p1.6 SPI_CS2(SFLASH) p1.0 SPI_DIN p1.1 SPI_DOUT p1.2 SPI_CLK */
    *(AR10_GPIO_P0_DIR) = (*AR10_GPIO_P0_DIR)|(0xa000);
    *(AR10_GPIO_P1_DIR) = ((*AR10_GPIO_P1_DIR)|(0x46))&(~1);

    *(AR10_GPIO_P0_ALTSEL0) = (*AR10_GPIO_P0_ALTSEL0)|(0x8000);
    *(AR10_GPIO_P0_ALTSEL0) = (*AR10_GPIO_P0_ALTSEL0)&(~0x2000);
    *(AR10_GPIO_P0_ALTSEL1) = (*AR10_GPIO_P0_ALTSEL1)&(~0xa000);
    *(AR10_GPIO_P1_ALTSEL0) = ((*AR10_GPIO_P1_ALTSEL0)|(0x47));
    *(AR10_GPIO_P1_ALTSEL1) = (*AR10_GPIO_P1_ALTSEL1)&(~0x47);

    *AR10_GPIO_P0_OD = (*AR10_GPIO_P0_OD)|0xa000;
    *AR10_GPIO_P1_OD = (*AR10_GPIO_P1_OD)|0x0047;
    *AR10_GPIO_P0_OUT = (*AR10_GPIO_P0_OUT)|0x2000;

	 return 1;																					 
}
#endif

#ifdef CONFIG_TUNE_DDR
#ifdef CONFIG_BOOT_FROM_NOR
void save_ddr_param(void)
{
   int rcode;
	 ulong   ddr_magic=0x88888888;
	 ulong erase_addr1=0, erase_addr2=0;
	 volatile ulong* ptr=IFX_CFG_FLASH_DDR_CFG_START_ADDR;
	 ulong  ecc;
	 ulong  buffer[6];
	 erase_addr1 = IFX_CFG_FLASH_DDR_CFG_START_ADDR; 
	 erase_addr2 = IFX_CFG_FLASH_DDR_CFG_START_ADDR + IFX_CFG_FLASH_DDR_CFG_SIZE;
	 if(*(u32*)0xBE22FF20!=0x2)  /*0xBE1A5F20 contains the tuning status*/
	    {
		    /*no new parameter, return immediately*/
		    return;
	    }
  if(flash_sect_protect (0, erase_addr1, erase_addr2-1))
     {
		      printf("protect off error!\n");
		 }
									 
  if (flash_sect_erase (erase_addr1, erase_addr2-1))
     {
		       printf("erase error!\n");
		 }
	 rcode = flash_sect_erase(erase_addr1, erase_addr2-1);

    
   memcpy ((u8*)buffer,&ddr_magic,4);
	 memcpy ((u8*)(buffer+1),(u8*)0xBF401270,4);
	 memcpy ((u8*)(buffer+2),(u8*)0xBF401280,4);
	 memcpy ((u8*)(buffer+3),(u8*)0xBF4012B0,4);
	 memcpy ((u8*)(buffer+4),(u8*)0xBF4012C0,4);
	 ecc=(*(u32*)0xBF401270)^(*(u32*)0xBF401280)^(*(u32*)0xBF4012B0)^(*(u32*)0xBF4012C0);
	 memcpy ((u8*)(buffer+5),&ecc,4);

	 flash_write((char *)buffer, IFX_CFG_FLASH_DDR_CFG_START_ADDR, 24);/*one magic word,4 parameters,1cc,24bytes*/

	 //(void) flash_sect_protect (1, erase_addr1, erase_addr2-1);
										
     return;
}
#elif defined(CONFIG_BOOT_FROM_NAND)
extern nand_info_t nand_info[]; 
//extern int nand_write (struct mtd_info *mtd, loff_t to, size_t len, size_t * retlen, const u_char * buf);

void save_ddr_param(void)
{   
	   int rcode;
	   ulong   ddr_magic=0x88888888;
	   ulong erase_addr1=0, erase_addr2=0;
	   volatile ulong* ptr=IFX_CFG_FLASH_DDR_CFG_START_ADDR;
	   ulong  ecc;
	   ulong  buffer[6];
	   erase_addr1 = IFX_CFG_FLASH_DDR_CFG_START_ADDR; 
	   erase_addr2 = IFX_CFG_FLASH_DDR_CFG_START_ADDR + IFX_CFG_FLASH_DDR_CFG_SIZE;
	   nand_info_t *nand;
	   nand_write_options_t opts;
	   nand = &nand_info[0];
	   int srcLen=24;
	   if(*(u32*)0xBE22FF20!=0x2)  /*0xBE1A5F20 contains the tuning status*/
	    {
		    /*no new parameter, return immediately*/
		    return;
	    }
	   memcpy ((u8*)buffer,&ddr_magic,4);
	   memcpy ((u8*)(buffer+1),(u8*)0xBF401270,4);
	   memcpy ((u8*)(buffer+2),(u8*)0xBF401280,4);
	   memcpy ((u8*)(buffer+3),(u8*)0xBF4012B0,4);
	   memcpy ((u8*)(buffer+4),(u8*)0xBF4012C0,4);
	   ecc=(*(u32*)0xBF401270)^(*(u32*)0xBF401280)^(*(u32*)0xBF4012B0)^(*(u32*)0xBF4012C0);
	   memcpy ((u8*)(buffer+5),&ecc,4); 
	   
	   nand_write_partial(&nand_info[0], IFX_CFG_FLASH_DDR_CFG_START_ADDR, &srcLen, (u_char*)buffer);
	   return;
}
#else /*BOOT from SPI*/
void save_ddr_param(void)
{
	 int rcode;
	 ulong   ddr_magic=0x88888888;
	 ulong erase_addr1=0, erase_addr2=0;
	 volatile ulong* ptr=IFX_CFG_FLASH_DDR_CFG_START_ADDR;
	 ulong  ecc;
	 ulong  buffer[6];
	 static struct spi_flash *flash_spi;
	 flash_spi = spi_flash_probe(CONFIG_ENV_SPI_BUS, CONFIG_ENV_SPI_CS,
            CONFIG_ENV_SPI_MAX_HZ, CONFIG_ENV_SPI_MODE);
	
	 erase_addr1 = IFX_CFG_FLASH_DDR_CFG_START_ADDR; 
	 erase_addr2 = IFX_CFG_FLASH_DDR_CFG_START_ADDR + IFX_CFG_FLASH_DDR_CFG_SIZE;
	 if(*(u32*)0xBE22FF20!=0x2)   /*0xBE1A5F20 contains the tuning status*/
	    {
		    /*no new parameter, return immediately*/
		    return;
	    }
	 
	 memcpy ((u8*)buffer,&ddr_magic,4);
	 memcpy ((u8*)(buffer+1),(u8*)0xBF401270,4);
	 memcpy ((u8*)(buffer+2),(u8*)0xBF401280,4);
	 memcpy ((u8*)(buffer+3),(u8*)0xBF4012B0,4);
	 memcpy ((u8*)(buffer+4),(u8*)0xBF4012C0,4);
	 ecc=(*(u32*)0xBF401270)^(*(u32*)0xBF401280)^(*(u32*)0xBF4012B0)^(*(u32*)0xBF4012C0);
	 memcpy ((u8*)(buffer+5),&ecc,4);
 
	 spi_flash_write(flash_spi, erase_addr1, 24, (char *)buffer);   
	 printf("saved ddr param in flash!\n");    
	    
   return;
	 return; 
}
#endif
#endif


int board_eth_init(bd_t *bis)
{
   if (ar10_eth_initialize(bis)<0)
             return -1;

	return 0;
}
						 
