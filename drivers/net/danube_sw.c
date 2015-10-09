/*
 * DANUBE internal switch ethernet driver.
 *
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
//906111:joelin 2008/06/11 program for Tantos chip for Easy50712 V3.0 EVM board , porting from kernel tanotos driver ifx_sw_init    

#include <common.h>


#include <malloc.h>
#include <net.h>
#include <config.h>
#include <asm/addrspace.h>
#include <asm/pinstrap.h>
#include <asm/ifx_tantos.h> //906111:joelin 

#define CONFIG_EASY50712V3 1 //906111:joelin 
#define MII_MODE 1
#define REV_MII_MODE 2

#define TX_CHAN_NO   7
#define RX_CHAN_NO   6

#define NUM_RX_DESC	PKTBUFSRX                  
#define NUM_TX_DESC	8
#define MAX_PACKET_SIZE 	1536
#define TOUT_LOOP	100
#define PHY0_ADDR       1 /*fixme: set the correct value here*/ 

#define DMA_WRITE_REG(reg, value) *((volatile u32 *)reg) = (u32)value
#define DMA_READ_REG(reg, value)    value = (u32)*((volatile u32*)reg)

#define SW_WRITE_REG(reg, value)  *((volatile u32*)reg) = (u32)value  	
#define SW_READ_REG(reg, value)   value = (u32)*((volatile u32*)reg)
#define TANTOS_CHIP_ID 0x2599

typedef struct
{
	union
	{
		struct
		{
			volatile u32 OWN                 :1;
			volatile u32 C	                 :1;
			volatile u32 Sop                 :1;
			volatile u32 Eop	         :1;
			volatile u32 reserved		 :3;
			volatile u32 Byteoffset		 :2; 
			volatile u32 reserve             :7;
			volatile u32 DataLen             :16;
		}field;

		volatile u32 word;
	}status;
	
	volatile u32 DataPtr;
} danube_rx_descriptor_t;

typedef struct
{
	union
	{
		struct
		{
			volatile u32 OWN                 :1;
			volatile u32 C	                 :1;
			volatile u32 Sop                 :1;
			volatile u32 Eop	         :1;
			volatile u32 Byteoffset		 :5; 
			volatile u32 reserved            :7;
			volatile u32 DataLen             :16;
		}field;

		volatile u32 word;
	}status;
	
	volatile u32 DataPtr;
} danube_tx_descriptor_t;




static danube_rx_descriptor_t rx_des_ring[NUM_RX_DESC] __attribute__ ((aligned(8)));
static danube_tx_descriptor_t tx_des_ring[NUM_TX_DESC] __attribute__ ((aligned(8)));
static int tx_num, rx_num;

int danube_switch_init(struct eth_device *dev, bd_t * bis);
int danube_switch_send(struct eth_device *dev, volatile void *packet,int length);
int danube_switch_recv(struct eth_device *dev);
void danube_switch_halt(struct eth_device *dev);
static void danube_init_switch_chip(int mode);
static void danube_dma_init(void);

static unsigned int ifx_sw_read(unsigned int addr, unsigned int *dat); //906111:joelin 
static int ifx_sw_write(unsigned int addr, unsigned int dat); //906111:joelin 
int tantos_vlan_add(int port_base, int port, int fid); //906111:joelin 
int tantos_vlan_del(int port_base, int port); //906111:joelin 
void danube_set_tmii_mode(int port); //906111:joelin 
void tantos_reset(void); //906111:joelin 

int danube_eth_init(bd_t * bis)
{
	struct eth_device *dev;
#ifdef CONFIG_EASY50712V3	
	unsigned short chipid; //pliu: 2008030501 from EASY50712V3 uboot
#endif      
#if 0
	printf("Entered danube_switch_initialize()\n");
#endif
 
	if (!(dev = (struct eth_device *) malloc (sizeof *dev)))
	{
		printf("Failed to allocate memory\n");
		return 0;
	}
	memset(dev, 0, sizeof(*dev));

	danube_dma_init();
	danube_init_switch_chip(REV_MII_MODE);
        
#ifdef CLK_OUT2_25MHZ
       *DANUBE_GPIO_P0_DIR=0x0000ae78;
       *DANUBE_GPIO_P0_ALTSEL0=0x00008078; 
       //joelin for Mii-1       *DANUBE_GPIO_P0_ALTSEL1=0x80000080;
       *DANUBE_GPIO_P0_ALTSEL1=0x80000000; //joelin for Mii-1 
       *DANUBE_CGU_IFCCR=0x00400010;
       *DANUBE_GPIO_P0_OD=0x0000ae78;
#endif        
       
        /*patch for 6996*/
	
	*DANUBE_RCU_RST_REQ |=1;
   //joelin 2009/06/11 extend the delay for Easy50712 V3.0 EVM     mdelay(200);
    mdelay(800) ;//joelin
	*DANUBE_RCU_RST_REQ &=(unsigned long)~1; 
	mdelay(1);
	/*while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
	*DANUBE_PPE_ETOP_MDIO_ACC =0x80123602;
	*/
	/*while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
	*DANUBE_PPE_ETOP_MDIO_ACC =0x80123602;
	*/
/*Switch Enable. (when WAIT_INIT=1)
When WAIT_INIT is enabled, this bit allows the CPU to control the
TANTOS enable or disable. The default is disable and the TANTOS will
wait for the CPU to enable this bit.*/
    mdelay(10);
    while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
    *DANUBE_PPE_ETOP_MDIO_ACC =0x80e18000;
    mdelay (10) ;
	/***************/
	sprintf(dev->name, "danube Switch");
	dev->init = danube_switch_init;
	dev->halt = danube_switch_halt;
	dev->send = danube_switch_send;
	dev->recv = danube_switch_recv;

	eth_register(dev);

#if 0
	printf("Leaving danube_switch_initialize()\n");
#endif
#ifdef CONFIG_EASY50712V3	

#define VINAX_VE_CHIPID 0x12C 
static unsigned int danube_chipid=0 ;
//#define DANUBE_MPS_CHIPID                       ((volatile u32*)(DANUBE_MPS + 0x0344))
//#define DANUBE_MPS_CHIPID_VERSION_GET(value)    (((value) >> 28) & ((1 << 4) - 1))

		danube_chipid=*DANUBE_MPS_CHIPID ;
       //printf("Danube Chip ID  0x%04x\n", danube_chipid);		
       danube_chipid = DANUBE_MPS_CHIPID_PARTNUM_GET(*DANUBE_MPS_CHIPID);
       

      // printf("Danube Chip ID  0x%04x\n", danube_chipid);

//906111:joelin : program for Tantos chip , porting from tanotos driver ifx_sw_init   +		
/*
	For Tantos-3G on EASY50712V3-VE board
	P0~P3 :  LAN port
	P4        :  EASY50712V3 ==>WAN Port , Twinpass board ==>RGMII connect to Giga PHY.
	P5        :  MII connect to TP-VE's MII1
	P6        :  MII connect to TP-VE's MII0
	Two VLAN group
*/       
 	   unsigned int val;
  	  ifx_sw_read(TANTOS_REG_CI1, &val);  
 
	  if (val!=0x2599) {
		  //Danube old V1.x board
			//for danube old board without tantos chip
		#if 0
    	printf("Leaving danube_switch_initialize()\n");
		#endif
    while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
    *DANUBE_PPE_ETOP_MDIO_ACC =0x8001840F;
    while((*DANUBE_PPE_ETOP_MDIO_ACC)&0x80000000);
    *DANUBE_PPE_ETOP_MDIO_ACC =0x8003840F;
    while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
    *DANUBE_PPE_ETOP_MDIO_ACC =0x8005840F;
    //while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
    //*DANUBE_PPE_ETOP_MDIO_ACC =0x8006840F;
    while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
    *DANUBE_PPE_ETOP_MDIO_ACC =0x8007840F;
    while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
    *DANUBE_PPE_ETOP_MDIO_ACC =0x8008840F;
    while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
    *DANUBE_PPE_ETOP_MDIO_ACC =0x8001840F;
        while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
    *DANUBE_PPE_ETOP_MDIO_ACC =0x80123602;
#ifdef CLK_OUT2_25MHZ
        while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
        *DANUBE_PPE_ETOP_MDIO_ACC =0x80334000;
#endif

    //pliu: 2008030501: from Tantos bootloader -
		} //if not tantos chip
		else //Tantos chip Danube V3 board or Vinax-VE board ? EASY50712V3;
		{//EASY50712V3 board
    		//printf("Setting default TANTOS registers for EASY50712V3 \n");
   			//printf("Tantos Chip ID  0x%04x\n", val);
 
            /* Set MII to TMII mode. */
            if (danube_chipid==VINAX_VE_CHIPID) {
						//printf("Easy80800/80700 VINAX-E/VINAX-VE \n");
						danube_set_tmii_mode(0);
						}
				else {	//printf("Easy50712 V3.0 XWAY DANUBE FAMILY \n");      
            			danube_set_tmii_mode(0);
						danube_set_tmii_mode(1);
					 }
	    /* Winder add, 20061104.
	       P5 and P6 is connected to MII0 and MII1 on EASY50712V3.
	       We have to force link this two port. */
	    /*
	       Warning: If P5 or P6 does not connect to MII0/MII1 any more, 
	       please remove the force link.
	    */
	    /*port 5 PAS = 0x01 force unauthorized*/
	    ifx_sw_write(TANTOS_REG_P5BCTL, 0x4);
	    /*port 6 PAS = 0x01 force unauthorized*/
	    ifx_sw_write(TANTOS_REG_P6BCTL, 0x4);		
 
 	    /* P5SPD = 1 P6SPD = 1 --> 1000MPS */
	   //only for Twinpass board but not for Easy50712 V3// ifx_sw_write(TANTOS_REG_RGMIICR, 0x773);
	     /* P5SPD = 1 P6SPD = 1 --> 100MPS for EASY50712V3*/
		ifx_sw_write(TANTOS_REG_RGMIICR, 0x777);


	    /* Software workaround. */
	    tantos_reset();
	    
            /* winder add for POF */
   //joelin         ifx_sw_write(TANTOS_REG_MIIWD, 0x80a6);	    
   //joelin        ifx_sw_write(TANTOS_REG_MIIAC, 0x0412);    
        /*
           Winder, 20061024, we should move this setting to other place.
           use proc to config maybe a good idea.
        */
        /*
           P0, P1, P2, P3 and P6 as a group.
           P4 and P5 as another group.
        */
        tantos_vlan_init();
        tantos_vlan_add(0, 0, 0);
        tantos_vlan_add(0, 1, 0);
        tantos_vlan_add(0, 2, 0);
        tantos_vlan_add(0, 3, 0);
		tantos_vlan_add(0, 6, 0);

        tantos_vlan_add(1, 0, 0);
        tantos_vlan_add(1, 1, 0);
        tantos_vlan_add(1, 2, 0);
        tantos_vlan_add(1, 3, 0);
        tantos_vlan_add(1, 6, 0);

        tantos_vlan_add(2, 0, 0);
        tantos_vlan_add(2, 1, 0);
        tantos_vlan_add(2, 2, 0);
        tantos_vlan_add(2, 3, 0);
        tantos_vlan_add(2, 6, 0);

        tantos_vlan_add(3, 0, 0);
        tantos_vlan_add(3, 1, 0);
        tantos_vlan_add(3, 2, 0);
        tantos_vlan_add(3, 3, 0);
        tantos_vlan_add(3, 6, 0);

        tantos_vlan_add(6, 0, 0);
        tantos_vlan_add(6, 1, 0);
        tantos_vlan_add(6, 2, 0);
        tantos_vlan_add(6, 3, 0);
        tantos_vlan_add(6, 6, 0);
        
        
        tantos_vlan_add(4, 4, 1);
        tantos_vlan_add(4, 5, 1);
        
        tantos_vlan_add(5, 4, 1);
        tantos_vlan_add(5, 5, 1);	
        }//EASY50712V3
            	
//906111:joelin : program for Tantos chip , porting from tanotos driver ifx_sw_init   -		
  	
//803132:joelin 	}
#else // CONFIG_EASY50712V3	
//for danube old board without tantos chip
#if 0
	printf("Leaving danube_switch_initialize()\n");
#endif
	while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
	*DANUBE_PPE_ETOP_MDIO_ACC =0x8001840F;
	while((*DANUBE_PPE_ETOP_MDIO_ACC)&0x80000000);
	*DANUBE_PPE_ETOP_MDIO_ACC =0x8003840F;
	while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
	*DANUBE_PPE_ETOP_MDIO_ACC =0x8005840F;
	//while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
	//*DANUBE_PPE_ETOP_MDIO_ACC =0x8006840F;
	while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
	*DANUBE_PPE_ETOP_MDIO_ACC =0x8007840F;
	while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
	*DANUBE_PPE_ETOP_MDIO_ACC =0x8008840F;
	while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
	*DANUBE_PPE_ETOP_MDIO_ACC =0x8001840F;
        while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
	*DANUBE_PPE_ETOP_MDIO_ACC =0x80123602; 
#ifdef CLK_OUT2_25MHZ         
        while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
        *DANUBE_PPE_ETOP_MDIO_ACC =0x80334000;
#endif
 
	//pliu: 2008030501: from Tantos bootloader -
#endif // CONFIG_EASY50712V3	
	return 1;
}
//906111:joelin -start
#ifdef CONFIG_EASY50712V3	
static int ifx_sw_write(unsigned int addr, unsigned int dat)
{
 
   *DANUBE_PPE32_ETOP_MDIO_ACC = ((addr<<16)&0x3ff0000)|dat|0x80000000;
   while ((*DANUBE_PPE32_ETOP_MDIO_ACC )&0x80000000){};
    return 0;
 
}
static unsigned int ifx_sw_read(unsigned int addr, unsigned int *dat)
{
   //addr=((addr<<16)|(1<<21))&0x3ff0000;
   addr=(addr<<16)&0x3ff0000;
   *DANUBE_PPE32_ETOP_MDIO_ACC =(0xC0000000|addr);
   while ((*DANUBE_PPE32_ETOP_MDIO_ACC)&0x80000000){};
   *dat=((*DANUBE_PPE32_ETOP_MDIO_ACC)&0x0FFFF);
    return *dat;
}
	
/*
  add a port to certain vlan
*/
//int tantos_vlan_add(int port, int vlan_id)
int tantos_vlan_add(int port_base, int port, int fid)
{

    int reg = 0;

    int ifx_sw_max_port_num = 6;
    if ((port < 0) || (port > ifx_sw_max_port_num) || (port_base < 0) ||
        (port_base > ifx_sw_max_port_num))
    {
        printf("Port number or filterid ERROR!!\n");
        return -1;
    }
    if (fid > 3)
    {
         printf("FID ERROR!!\n");
        return -1;
    }

    // printk("port_base %d, port %d, fid %d\n", port_base, port, fid);
    switch(port_base)
    {
        case 0:
            ifx_sw_read(TANTOS_REG_P0PBVM, &reg);
            reg = (fid << 14)|( 1 << port)|(reg&0x3fff);
            ifx_sw_write(TANTOS_REG_P0PBVM, reg);
            break;
        case 1:
            ifx_sw_read(TANTOS_REG_P1PBVM, &reg);
            reg = (fid << 14)|( 1 << port)|(reg&0x3fff);
            ifx_sw_write(TANTOS_REG_P1PBVM, reg);
            break;
        case 2:
            ifx_sw_read(TANTOS_REG_P2PBVM, &reg);
            reg = (fid << 14)|( 1 << port)|(reg&0x3fff);
            ifx_sw_write(TANTOS_REG_P2PBVM, reg);
            break;
        case 3:
            ifx_sw_read(TANTOS_REG_P3PBVM, &reg);
            reg = (fid << 14)|( 1 << port)|(reg&0x3fff);
            ifx_sw_write(TANTOS_REG_P3PBVM, reg);
            break;
        case 4:
            ifx_sw_read(TANTOS_REG_P4PBVM, &reg);
            reg = (fid << 14)|( 1 << port)|(reg&0x3fff);
            ifx_sw_write(TANTOS_REG_P4PBVM, reg);
            break;
        case 5:
            ifx_sw_read(TANTOS_REG_P5PBVM, &reg);
            reg = (fid << 14)|( 1 << port)|(reg&0x3fff);
            ifx_sw_write(TANTOS_REG_P5PBVM, reg);
            break;
        case 6:
            ifx_sw_read(TANTOS_REG_P6PBVM, &reg);
            reg = (fid << 14)|( 1 << port)|(reg&0x3fff);
            ifx_sw_write(TANTOS_REG_P6PBVM, reg);
            break;
        default:
            /* Should not be here. */
            break;
    }

    return 0;
}
/* 
  delete a given port from certain vlan
*/
int tantos_vlan_del(int port_base, int port)
{
    unsigned int reg = 0;
    int ifx_sw_max_port_num = 6;

    if ((port < 0) || (port > ifx_sw_max_port_num) || (port < 0) || (port > ifx_sw_max_port_num))
    {
        printf("Port number or filterid ERROR!!\n");
        return -1;
    }

    // printk("port_base %d, port %d\n", port_base, port);

    switch(port_base)
    {
        case 0:
            ifx_sw_read(TANTOS_REG_P0PBVM, &reg);
            reg &= ~( 1 << port);
            reg &= 0x3fff; // Clean FID.
            ifx_sw_write(TANTOS_REG_P0PBVM, reg);
            break;
        case 1:
            ifx_sw_read(TANTOS_REG_P1PBVM, &reg);
            reg &= ~( 1 << port);
            reg &= 0x3fff; // Clean FID.
            ifx_sw_write(TANTOS_REG_P1PBVM, reg);
            break;
        case 2:
            ifx_sw_read(TANTOS_REG_P2PBVM, &reg);
            reg &= ~( 1 << port);
            reg &= 0x3fff; // Clean FID.
            ifx_sw_write(TANTOS_REG_P2PBVM, reg);
            break;
        case 3:
            ifx_sw_read(TANTOS_REG_P3PBVM, &reg);
            reg &= ~( 1 << port);
            reg &= 0x3fff; // Clean FID.
            ifx_sw_write(TANTOS_REG_P3PBVM, reg);
            break;
        case 4:
            ifx_sw_read(TANTOS_REG_P4PBVM, &reg);
            reg &= ~( 1 << port);
            reg &= 0x3fff; // Clean FID.
            ifx_sw_write(TANTOS_REG_P4PBVM, reg);
            break;
        case 5:
            ifx_sw_read(TANTOS_REG_P5PBVM, &reg);
            reg &= ~( 1 << port);
            reg &= 0x3fff; // Clean FID.
            ifx_sw_write(TANTOS_REG_P5PBVM, reg);
            break;
        case 6:
            ifx_sw_read(TANTOS_REG_P6PBVM, &reg);
            reg &= ~( 1 << port);
            reg &= 0x3fff; // Clean FID.
            ifx_sw_write(TANTOS_REG_P6PBVM, reg);
            break;
        default:
            /* Should not be here. */
            break;
    }

    return 0;


}
void danube_set_tmii_mode(int port)
{
    if (port == 0)
    {
        *ETOP_CFG |= 1 << 2;
        //printf("Set MII0 Turbo\n");
    }
    else if (port == 1)
    {
    *ETOP_CFG |= 1 << 5;
        //printf("Set MII1 Turbo\n");
    }
   // else
   // printk("%s:Wrong port number!!\n", __FUNCTION__); // should not be here.
}

/*
 * PHY reset.
 */
void tantos_reset(void)
{
    int wbusy;

    /* PHY reset from P0 to P4. */
    ifx_sw_write(TANTOS_REG_MIIWD, 0x8000);
    /* P0 */
    ifx_sw_write(TANTOS_REG_MIIAC, 0x0400);
    while(ifx_sw_read(TANTOS_REG_MIIAC, &wbusy) & 0x8000);
    //joelin mdelay(1);
    mdelay(10); //joelin
    /* P1 */
    ifx_sw_write(TANTOS_REG_MIIAC, 0x0420);
    while(ifx_sw_read(TANTOS_REG_MIIAC, &wbusy) & 0x8000);
    //joelin mdelay(1);
    mdelay(10); //joelin
    /* P2 */
    ifx_sw_write(TANTOS_REG_MIIAC, 0x0440);
    while(ifx_sw_read(TANTOS_REG_MIIAC, &wbusy) & 0x8000);
    //joelin mdelay(1);
    mdelay(10); //joelin
    /* P3 */
    ifx_sw_write(TANTOS_REG_MIIAC, 0x0460);
    while(ifx_sw_read(TANTOS_REG_MIIAC, &wbusy) & 0x8000);
     //joelin mdelay(1);
    mdelay(10); //joelin
#ifndef CONFIG_EASY50712V3_VDSL2
    /* p4 */
    ifx_sw_write(TANTOS_REG_MIIAC, 0x0480);
    while(ifx_sw_read(TANTOS_REG_MIIAC, &wbusy) & 0x8000);
    mdelay(1);
#endif
/* 
	For Tantos-2G on Vinax-VE board
	P0~P3 :  LAN port
	P4        :  POF port
	P5        :  RGMII connect to Giga PHY.
	P6        :  MII connect to Vinax-VE's MII0
	One VLAN group
*/	    
#ifdef  CONFIG_VINAX_VE   
    /* p4 */
    ifx_sw_write(TANTOS_REG_MIIAC, 0x0480);
    while(ifx_sw_read(TANTOS_REG_MIIAC, &wbusy) & 0x8000);
    mdelay(1);
    
    /* p5 */
    ifx_sw_write(TANTOS_REG_MIIAC, 0x04A0);
    while(ifx_sw_read(TANTOS_REG_MIIAC, &wbusy) & 0x8000);
    mdelay(1);    
#endif
/*Switch Enable. (when WAIT_INIT=1)
When WAIT_INIT is enabled, this bit allows the CPU to control the
TANTOS enable or disable. The default is disable and the TANTOS will
wait for the CPU to enable this bit.*/
    mdelay(10);
    ifx_sw_write(TANTOS_REG_SOFTRST, 0x8000);
 
}
/*
  initialize a VLAN
  clear all VLAN bits
*/
int tantos_vlan_init(void)
{
    int i,j;
    unsigned int val;
    
    //remove vlan_id
    for(i = 0; i< IFX_TANTOS_MAX_PORT; i++)
        for(j = 0; j<IFX_TANTOS_MAX_PORT; j++)
            tantos_vlan_del(i, j);

    // bypass VLAN tag.
    for(i=0;i<IFX_TANTOS_MAX_PORT;i++)
    {
        ifx_sw_read(TANTOS_REG_P0PBVM + i * 0x20, &val);
        ifx_sw_write(TANTOS_REG_P0PBVM + i * 0x20, val | BIT_7);
    }

    return 1;
}
#endif // CONFIG_EASY50712V3	
//906111:joelin -end

int danube_switch_init(struct eth_device *dev, bd_t * bis)
{
	int i;
	int datalen, cache_linesize;
	tx_num=0;
	rx_num=0;
	
		/* Reset DMA 
		 */
//	serial_puts("i \n\0");

       *DANUBE_DMA_CS=RX_CHAN_NO;
       *DANUBE_DMA_CCTRL=0x2;/*fix me, need to reset this channel first?*/
       *DANUBE_DMA_CPOLL= 0x80000040;
       /*set descriptor base*/
       *DANUBE_DMA_CDBA=(u32)rx_des_ring;
       *DANUBE_DMA_CDLEN=NUM_RX_DESC;
       *DANUBE_DMA_CIE = 0;
       *DANUBE_DMA_CCTRL=0x30000;
	 	
       *DANUBE_DMA_CS=TX_CHAN_NO;
       *DANUBE_DMA_CCTRL=0x2;/*fix me, need to reset this channel first?*/
       *DANUBE_DMA_CPOLL= 0x80000040;
       *DANUBE_DMA_CDBA=(u32)tx_des_ring;
       *DANUBE_DMA_CDLEN=NUM_TX_DESC;  
       *DANUBE_DMA_CIE = 0;
       *DANUBE_DMA_CCTRL=0x30100;


	
	asm("sync");
	
	
	for(i=0;i < NUM_RX_DESC; i++)
	{
		danube_rx_descriptor_t * rx_desc = KSEG1ADDR(&rx_des_ring[i]);
		rx_desc->status.word=0;	
		rx_desc->status.field.OWN=1;
		rx_desc->status.field.DataLen=PKTSIZE_ALIGN;   /* 1536  */	
		rx_desc->DataPtr=(u32)KSEG1ADDR(NetRxPackets[i]);
	  memcpy((u8*)&rx_des_ring[i],rx_desc,8);
	}
  for(i=0;i < NUM_TX_DESC; i++)
	{
		danube_tx_descriptor_t * tx_desc = KSEG1ADDR(&tx_des_ring[i]);
		memset(tx_desc, 0, sizeof(tx_des_ring[0]));
	}
		/* turn on DMA rx & tx channel
		 */
	 *DANUBE_DMA_CS=RX_CHAN_NO;
	 *DANUBE_DMA_CCTRL|=1;/*reset and turn on the channel*/


	
	return 1;
}

void danube_switch_halt(struct eth_device *dev)
{
        int i; 
        for(i=0;i<8;i++)
	{
	   *DANUBE_DMA_CS=i;
	   *DANUBE_DMA_CCTRL&=~1;/*stop the dma channel*/
	}
//	udelay(1000000);
}

int danube_switch_send(struct eth_device *dev, volatile void *packet,int length)
{

	int                    	i;
	int 		 	res = -1;
	int datalen, cache_linesize;
	danube_tx_descriptor_t * tx_desc= KSEG1ADDR(&tx_des_ring[tx_num]);
	
	if (length <= 0)
	{
		printf ("%s: bad packet size: %d\n", dev->name, length);
		goto Done;
	}
	
	for(i=0; tx_desc->status.field.OWN==1; i++)
	{
		if(i>=TOUT_LOOP)
		{
			printf("NO Tx Descriptor...");
			goto Done;
		}
	}

	//serial_putc('s');
 
	tx_desc->status.field.Sop=1;

	tx_desc->status.field.Eop=1;
	
	tx_desc->status.field.C=0;
	
	tx_desc->DataPtr = (u32)KSEG1ADDR(packet);
	
	if(length<60)
		tx_desc->status.field.DataLen = 60;
	else
		tx_desc->status.field.DataLen = (u32)length;	
	
	
	cache_linesize = dcache_linesize_mips32() ;
	
	datalen = cache_linesize *((tx_desc->status.field.DataLen/cache_linesize)+1);
	
	dcache_writeback_invalidate(datalen, cache_linesize, packet );
	
	tx_desc->status.field.OWN=1;
			 	
	res=length;
	tx_num++;
        if(tx_num==NUM_TX_DESC) tx_num=0;
	*DANUBE_DMA_CS=TX_CHAN_NO;
	  
	if(!(*DANUBE_DMA_CCTRL & 1))
	*DANUBE_DMA_CCTRL|=1;
        
Done:
	return res;
}

int danube_switch_recv(struct eth_device *dev)
{
  
	int                    length  = 0;
  int datalen, cache_linesize;
	danube_rx_descriptor_t * rx_desc;
        int anchor_num=0;
	int i;
	
	for (;;)
	{
		   
	        rx_desc = KSEG1ADDR(&rx_des_ring[rx_num]);

	        if ((rx_desc->status.field.C == 0) || (rx_desc->status.field.OWN == 1))
		{
		   break;
		}
		
                
		length = rx_desc->status.field.DataLen;
		if (length)
		{	
	
		     cache_linesize = dcache_linesize_mips32() ;
			
			 datalen = cache_linesize*((rx_desc->status.field.DataLen/cache_linesize)+1);
	   
			 dcache_hit_invalidate(datalen,cache_linesize, NetRxPackets[rx_num]);
		  
			NetReceive((void*)KSEG1ADDR(NetRxPackets[rx_num]), length - 4);
			
		//	serial_putc('*');
		}
		else
		{
			printf("Zero length!!!\n");
		}
    
		rx_desc->status.field.Sop=0;
		
		rx_desc->status.field.Eop=0;
		
		rx_desc->status.field.C=0;
		
		rx_desc->status.field.DataLen=PKTSIZE_ALIGN;
		
		rx_desc->status.field.OWN=1;
		
		rx_num++;
		if(rx_num==NUM_RX_DESC) rx_num=0;
   
	}

	return length;
}


static void danube_init_switch_chip(int mode)
{
        int i;
        /*get and set mac address for MAC*/
        static unsigned char addr[6];
        char *tmp,*end; 
	tmp = getenv ("ethaddr");
	if (NULL == tmp) {
		printf("Can't get environment ethaddr!!!\n");
	//	return NULL;
	} else {
		printf("ethaddr=%s\n", tmp);
	}
        *DANUBE_PMU_PWDCR = *DANUBE_PMU_PWDCR & 0xFFFFEFDF;
        *DANUBE_PPE32_ETOP_MDIO_CFG &= ~0x6;
        *DANUBE_PPE32_ENET_MAC_CFG = 0x187;
  
  // turn on port0, set to rmii and turn off port1.
	if(mode==REV_MII_MODE)
		{
		 *DANUBE_PPE32_ETOP_CFG = (*DANUBE_PPE32_ETOP_CFG & 0xfffffffc) | 0x0000000a;
                } 
	else if (mode == MII_MODE)
	        {
		*DANUBE_PPE32_ETOP_CFG = (*DANUBE_PPE32_ETOP_CFG & 0xfffffffc) | 0x00000008;
                }	  

	*DANUBE_PPE32_ETOP_IG_PLEN_CTRL = 0x4005ee; // set packetlen.
        *ENET_MAC_CFG|=1<<11;/*enable the crc*/
	return;
}


static void danube_dma_init(void)
{
        int i;
//	serial_puts("d \n\0");

        *DANUBE_PMU_PWDCR &=~(1<<DANUBE_PMU_DMA_SHIFT);/*enable DMA from PMU*/
		/* Reset DMA 
		 */
	*DANUBE_DMA_CTRL|=1; 
        *DANUBE_DMA_IRNEN=0;/*disable all the interrupts first*/

	/* Clear Interrupt Status Register 		 
	*/
	*DANUBE_DMA_IRNCR=0xfffff;
	/*disable all the dma interrupts*/
	*DANUBE_DMA_IRNEN=0;
	/*disable channel 0 and channel 1 interrupts*/
	
	  *DANUBE_DMA_CS=RX_CHAN_NO;
	  *DANUBE_DMA_CCTRL=0x2;/*fix me, need to reset this channel first?*/
          *DANUBE_DMA_CPOLL= 0x80000040;
          /*set descriptor base*/
          *DANUBE_DMA_CDBA=(u32)rx_des_ring;
          *DANUBE_DMA_CDLEN=NUM_RX_DESC;
          *DANUBE_DMA_CIE = 0;
          *DANUBE_DMA_CCTRL=0x30000;
	 	
	*DANUBE_DMA_CS=TX_CHAN_NO;
	*DANUBE_DMA_CCTRL=0x2;/*fix me, need to reset this channel first?*/
        *DANUBE_DMA_CPOLL= 0x80000040;
	*DANUBE_DMA_CDBA=(u32)tx_des_ring;
        *DANUBE_DMA_CDLEN=NUM_TX_DESC;  
	*DANUBE_DMA_CIE = 0;
	*DANUBE_DMA_CCTRL=0x30100;
	/*enable the poll function and set the poll counter*/
	//*DANUBE_DMA_CPOLL=DANUBE_DMA_POLL_EN | (DANUBE_DMA_POLL_COUNT<<4);
	/*set port properties, enable endian conversion for switch*/
	*DANUBE_DMA_PS=0;
	*DANUBE_DMA_PCTRL|=0xf<<8;/*enable 32 bit endian conversion*/

	return;
}



