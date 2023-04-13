/*
**************************************************************************************************
* @file    		w5500_conf.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-02-14
* @brief  		配置MCU，移植W5500程序需要修改的文件，配置W5500的MAC和IP地址
**************************************************************************************************
*/
#include <stdio.h> 
#include <string.h>

#include "w5500_conf.h"
//#include "bsp_i2c_ee.h"
//#include "bsp_spi_flash.h"
#include "utility.h"
#include "w5500.h"
#include "dhcp.h"
//#include "bsp_TiMbase.h"
 #include "os.h"

CONFIG_MSG  ConfigMsg;																	/*配置结构体*/
EEPROM_MSG_STR EEPROM_MSG;															/*EEPROM存储信息结构体*/

/*定义MAC地址,如果多块W5500网络适配板在同一现场工作，请使用不同的MAC地址*/
uint8 mac[6]={0x00,0xA1,0x00,0x1B,0x7B,0x29};


uint8 local_ip[4]={0,0,0,0};											/*定义W5500默认IP地址*/
uint8 subnet[4]={0,0,0,0};												/*定义W5500默认子网掩码*/
uint8 gateway[4]={0,0,0,0};												/*定义W5500默认网关*/
uint8 dns_server[4]={0,0,0,0};									  /*定义W5500默认DNS*/

//#ifdef IDnbr1 
/*定义默认IP信息*/
//uint8 local_ip[4]={192,168,1,81};											/*定义W5500默认IP地址*/
//uint8 subnet[4]={255,255,255,0};											/*定义W5500默认子网掩码*/
//uint8 gateway[4]={192,168,1,1};												/*定义W5500默认网关*/
//uint8 dns_server[4]={192,168,1,1};									  /*定义W5500默认DNS*/
//#endif   

//#ifdef IDnbr2 
///*定义默认IP信息*/
//uint8 local_ip[4]={192,168,1,82};												/*定义W5500默认IP地址*/
//uint8 subnet[4]={255,255,255,0};												/*定义W5500默认子网掩码*/
//uint8 gateway[4]={192,168,1,103};												/*定义W5500默认网关*/
//uint8 dns_server[4]={192,168,1,103};									  /*定义W5500默认DNS*/
//#endif

//#ifdef IDnbr3 
///*定义默认IP信息*/
//uint8 local_ip[4]={192,168,1,83};												/*定义W5500默认IP地址*/
//uint8 subnet[4]={255,255,255,0};												/*定义W5500默认子网掩码*/
//uint8 gateway[4]={192,168,1,103};												/*定义W5500默认网关*/
//uint8 dns_server[4]={192,168,1,103};									  /*定义W5500默认DNS*/
//#endif

//uint8 local_ip[4]={172,16,10,4};												/*定义W5500默认IP地址*/
//uint8 subnet[4]={255,255,255,0};												/*定义W5500默认子网掩码*/
//uint8 gateway[4]={172,16,10,1};													/*定义W5500默认网关*/
//uint8 dns_server[4]={211,167,230,100};									/*定义W5500默认DNS*/

uint16 local_port=5000;	                       					/*定义本地端口*/

//*定义远端IP信息*/
//uint8  remote_ip[4]={124,192,224,226};							  /*远端IP地址*/
//uint16 remote_port=8010;														  /*远端端口号*/

//uint8  remote_ip[4]={192,168,1,4};
//uint16 remote_port=4446;																
uint8  remote_ip[4]={0,0,0,0};											/*远端IP地址*/
uint16 remote_port=0;																/*远端端口号*/

/*定义远端IP信息*/
//uint8  remote_ip[4]={124,192,224,226};											/*远端IP地址*/
//uint16 remote_port=8010;																/*远端端口号*/



/*IP配置方法选择，请自行选择*/
uint8	ip_from=IP_FROM_DEFINE;				

uint8 dhcp_ok=0;																				/*dhcp成功获取IP*/
uint32	ms=0;																						/*毫秒计数*/
uint32	dhcp_time= 0;																		/*DHCP运行计数*/
vu8	ntptimer = 0;																				/*NPT秒计数*/

/**
*@brief		配置W5500的IP地址
*@param		无
*@return	无
*/
void set_w5500_ip(void)
{	
		
 /*复制定义的配置信息到配置结构体*/
	memcpy(ConfigMsg.mac, mac, 6);
	memcpy(ConfigMsg.lip,local_ip,4);
	memcpy(ConfigMsg.sub,subnet,4);
	memcpy(ConfigMsg.gw,gateway,4);
	memcpy(ConfigMsg.dns,dns_server,4);
	
	/*使用DHCP获取IP参数，需调用DHCP子函数*/		
	/*复制DHCP获取的配置信息到配置结构体*/
	if(dhcp_ok==1)
	{
		printf("1.DHCP成功获取IP!\r\n");		 
		memcpy(ConfigMsg.lip,DHCP_GET.lip, 4);
		memcpy(ConfigMsg.sub,DHCP_GET.sub, 4);
		memcpy(ConfigMsg.gw,DHCP_GET.gw, 4);
		memcpy(ConfigMsg.dns,DHCP_GET.dns,4);
	}
	else
	{
		printf("1.DHCP不成功或未获取!\r\n");
	}
		
	/*以下配置信息，根据需要选用*/	
	ConfigMsg.sw_ver[0]=FW_VER_HIGH;
	ConfigMsg.sw_ver[1]=FW_VER_LOW;	

	/*将IP配置信息写入W5500相应寄存器*/	
	setSUBR(ConfigMsg.sub);
	setGAR(ConfigMsg.gw);
	setSIPR(ConfigMsg.lip);
	
	getSIPR (local_ip);			
	printf("1.IP地址: %d.%d.%d.%d\r\n", local_ip[0],local_ip[1],local_ip[2],local_ip[3]);
	getSUBR(subnet);
	printf("2.子网掩码:%d.%d.%d.%d\r\n", subnet[0],subnet[1],subnet[2],subnet[3]);
	getGAR(gateway);
	printf("3.默认网关:%d.%d.%d.%d\r\n", gateway[0],gateway[1],gateway[2],gateway[3]);
	printf("4.DNS服务器:%d.%d.%d.%d\r\n", dns_server[0],dns_server[1],dns_server[2],dns_server[3]);
}

/**
*@brief		配置W5500的MAC地址
*@param		无
*@return	无
*/
void set_w5500_mac(void)
{
	memcpy(ConfigMsg.mac, mac, 6);
	setSHAR(ConfigMsg.mac);	/**/
	memcpy(DHCP_GET.mac, mac, 6);
}

void SPI2_FLASH_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable SPI1 and GPIO clocks */
  /*!< SPI_FLASH_SPI_CS_GPIO, SPI_FLASH_SPI_MOSI_GPIO, 
       SPI_FLASH_SPI_MISO_GPIO, SPI_FLASH_SPI_DETECT_GPIO 
       and SPI_FLASH_SPI_SCK_GPIO Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);

  /*!< SPI_FLASH_SPI Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
 
  
  /*!< Configure SPI_FLASH_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /*!< Configure SPI_FLASH_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /*!< Configure SPI_FLASH_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /*!< Configure SPI_FLASH_SPI_CS_PIN pin: SPI_FLASH Card CS pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Deselect the FLASH: Chip Select high */
//  SPI_FLASH_CS_HIGH();

  /* SPI1 configuration */
  // W25X16: data input on the DIO pin is sampled on the rising edge of the CLK. 
  // Data on the DO and DIO pins are clocked out on the falling edge of CLK.
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);
  SPI_Cmd(SPI2, ENABLE);
  /* Enable SPI1  */
//  SPI_Cmd(SPI2, ENABLE);
}
/**
*@brief		配置W5500的GPIO接口
*@param		无
*@return	无
*/
void gpio_for_w5500_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI2_FLASH_Init();																			 /*初始化STM32 SPI2接口*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
 
	/*定义RESET引脚*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;					   /*选择要控制的GPIO引脚*/		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 /*设置引脚速率为50MHz */		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		 /*设置引脚模式为通用推挽输出*/	
	GPIO_Init(GPIOC, &GPIO_InitStructure);							 /*调用库函数，初始化GPIO*/
	GPIO_SetBits(GPIOC, GPIO_Pin_6);		

	/*定义INT引脚*/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;						 /*选择要控制的GPIO引脚*/		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 /*设置引脚速率为50MHz*/		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;				 /*设置引脚模式为通用推挽模拟上拉输入*/		
	GPIO_Init(GPIOC, &GPIO_InitStructure);							 /*调用库函数，初始化GPIO*/
}

/**
*@brief		W5500片选信号设置函数
*@param		val: 为“0”表示片选端口为低，为“1”表示片选端口为高
*@return	无
*/
void wiz_cs(uint8_t val)
{
	if (val == LOW) 
	{
		GPIO_ResetBits(GPIOB, WIZ_SCS); 
	}
	else if (val == HIGH)
	{
		GPIO_SetBits(GPIOB, WIZ_SCS); 
	}
}

/**
*@brief		设置W5500的片选端口SCSn为低
*@param		无
*@return	无
*/
void iinchip_csoff(void)
{
	wiz_cs(LOW);
}

/**
*@brief		设置W5500的片选端口SCSn为高
*@param		无
*@return	无
*/
void iinchip_cson(void)
{	
   wiz_cs(HIGH);
}

/**
*@brief		W5500复位设置函数
*@param		无
*@return	无
*/
void delay_us1(unsigned int n)
{
  unsigned  int j,g;
  for(j=0;j<n;j++)
    {
      for(g=0;g<4;g++)  //1US			
        {
						__NOP();
						__NOP();
						__NOP();
						__NOP();
						__NOP();
						__NOP();				
        }	 
    }
}

void reset_w5500(void)
{
 		OS_ERR err;
	/*使用野火ISO标准开发板*/
//	#ifdef STM32F103ZET6
		GPIO_ResetBits(GPIOC, WIZ_RESET);
		delay_us1(2);  
		GPIO_SetBits(GPIOC, WIZ_RESET);
//		delay_us1(1600000);
// 		OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err);
	  delay_ms(1600);	
//	#endif
	
	/*使用野火MINI开发板*/
//	#ifdef STM32F103VET6
//		GPIO_ResetBits(GPIOE, WIZ_RESET);
//		delay_us(2);  
//		GPIO_SetBits(GPIOE, WIZ_RESET);
//		delay_ms(1600);	
//	#endif
}

u8 SPI2_FLASH_SendByte(u8 byte)
{
//	CPU_SR_ALLOC();
//	OS_CRITICAL_ENTER();//进入临界区
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI2, byte);

  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
//	OS_CRITICAL_EXIT();	//退出临界区	 
  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI2);

}

/**
*@brief		STM32 SPI1读写8位数据
*@param		dat：写入的8位数据
*@return	无
*/
uint8  IINCHIP_SpiSendData(uint8 dat)
{
   return(SPI2_FLASH_SendByte(dat));
}

/**
*@brief		写入一个8位数据到W5500
*@param		addrbsb: 写入数据的地址
*@param   data：写入的8位数据
*@return	无
*/
void IINCHIP_WRITE( uint32 addrbsb,  uint8 data)
{
   iinchip_csoff();                              		
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);	
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4);  
   IINCHIP_SpiSendData(data);                   
   iinchip_cson();                            
}

/**
*@brief		从W5500读出一个8位数据
*@param		addrbsb: 写入数据的地址
*@param   data：从写入的地址处读取到的8位数据
*@return	无
*/
uint8 IINCHIP_READ(uint32 addrbsb)
{
   uint8 data = 0;
   iinchip_csoff();                            
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8))    ;
   data = IINCHIP_SpiSendData(0x00);            
   iinchip_cson();                               
   return data;    
}

/**
*@brief		向W5500写入len字节数据
*@param		addrbsb: 写入数据的地址
*@param   buf：写入字符串
*@param   len：字符串长度
*@return	len：返回字符串长度
*/
uint16 wiz_write_buf(uint32 addrbsb,uint8* buf,uint16 len)
{
   uint16 idx = 0;
   if(len == 0) printf("Unexpected2 length 0\r\n");
   iinchip_csoff();                               
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4); 
   for(idx = 0; idx < len; idx++)
   {
     IINCHIP_SpiSendData(buf[idx]);
   }
   iinchip_cson();                           
   return len;  
}

/**
*@brief		从W5500读出len字节数据
*@param		addrbsb: 读取数据的地址
*@param 	buf：存放读取数据
*@param		len：字符串长度
*@return	len：返回字符串长度
*/
uint16 wiz_read_buf(uint32 addrbsb, uint8* buf,uint16 len)
{
  uint16 idx = 0;
  if(len == 0)
  {
    printf("Unexpected2 length 0\r\n");
  }
  iinchip_csoff();                                
  IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
  IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
  IINCHIP_SpiSendData( (addrbsb & 0x000000F8));    
  for(idx = 0; idx < len; idx++)                   
  {
    buf[idx] = IINCHIP_SpiSendData(0x00);
  }
  iinchip_cson();                                  
  return len;
}

/**
*@brief		写配置信息到EEPROM
*@param		无
*@return	无
*/
void write_config_to_eeprom(void)
{
	uint16 dAddr=0;
//	ee_WriteBytes(ConfigMsg.mac,dAddr,(uint8)EEPROM_MSG_LEN);				
	delay_ms(10);																							
}

/**
*@brief		从EEPROM读配置信息
*@param		无
*@return	无
*/
void read_config_from_eeprom(void)
{
//	ee_ReadBytes(EEPROM_MSG.mac,0,EEPROM_MSG_LEN);
	delay_us(10);
}

/**
*@brief		STM32定时器2初始化
*@param		无
*@return	无
*/
void timer2_init(void)
{
//	TIM2_Configuration();																		/* TIM2 定时配置 */
//	TIM2_NVIC_Configuration();															/* 定时器的中断优先级 */
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);		/* TIM2 重新开时钟，开始计时 */
}

/**
*@brief		dhcp用到的定时器初始化
*@param		无
*@return	无
*/
void dhcp_timer_init(void)
{
			timer2_init();																	
}

/**
*@brief		ntp用到的定时器初始化
*@param		无
*@return	无
*/
void ntp_timer_init(void)
{
			timer2_init();																	
}

/**
*@brief		定时器2中断函数
*@param		无
*@return	无
*/
void timer2_isr(void)
{
	ms++;	
  if(ms>=1000)
  {  
    ms=0;
    dhcp_time++;																					/*DHCP定时加1S*/
		#ifndef	__NTP_H__
		ntptimer++;																						/*NTP重试时间加1S*/
		#endif
  }

}
/**
*@brief		STM32系统软复位函数
*@param		无
*@return	无
*/
void reboot(void)
{
  pFunction Jump_To_Application;
  uint32 JumpAddress;
  printf(" 系统重启中……\r\n");
  JumpAddress = *(vu32*) (0x00000004);
  Jump_To_Application = (pFunction) JumpAddress;
  Jump_To_Application();
}
