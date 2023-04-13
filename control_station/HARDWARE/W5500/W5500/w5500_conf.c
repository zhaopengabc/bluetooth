/*
**************************************************************************************************
* @file    		w5500_conf.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-02-14
* @brief  		����MCU����ֲW5500������Ҫ�޸ĵ��ļ�������W5500��MAC��IP��ַ
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

CONFIG_MSG  ConfigMsg;																	/*���ýṹ��*/
EEPROM_MSG_STR EEPROM_MSG;															/*EEPROM�洢��Ϣ�ṹ��*/

/*����MAC��ַ,������W5500�����������ͬһ�ֳ���������ʹ�ò�ͬ��MAC��ַ*/
uint8 mac[6]={0x00,0xA1,0x00,0x1B,0x7B,0x29};


uint8 local_ip[4]={0,0,0,0};											/*����W5500Ĭ��IP��ַ*/
uint8 subnet[4]={0,0,0,0};												/*����W5500Ĭ����������*/
uint8 gateway[4]={0,0,0,0};												/*����W5500Ĭ������*/
uint8 dns_server[4]={0,0,0,0};									  /*����W5500Ĭ��DNS*/

//#ifdef IDnbr1 
/*����Ĭ��IP��Ϣ*/
//uint8 local_ip[4]={192,168,1,81};											/*����W5500Ĭ��IP��ַ*/
//uint8 subnet[4]={255,255,255,0};											/*����W5500Ĭ����������*/
//uint8 gateway[4]={192,168,1,1};												/*����W5500Ĭ������*/
//uint8 dns_server[4]={192,168,1,1};									  /*����W5500Ĭ��DNS*/
//#endif   

//#ifdef IDnbr2 
///*����Ĭ��IP��Ϣ*/
//uint8 local_ip[4]={192,168,1,82};												/*����W5500Ĭ��IP��ַ*/
//uint8 subnet[4]={255,255,255,0};												/*����W5500Ĭ����������*/
//uint8 gateway[4]={192,168,1,103};												/*����W5500Ĭ������*/
//uint8 dns_server[4]={192,168,1,103};									  /*����W5500Ĭ��DNS*/
//#endif

//#ifdef IDnbr3 
///*����Ĭ��IP��Ϣ*/
//uint8 local_ip[4]={192,168,1,83};												/*����W5500Ĭ��IP��ַ*/
//uint8 subnet[4]={255,255,255,0};												/*����W5500Ĭ����������*/
//uint8 gateway[4]={192,168,1,103};												/*����W5500Ĭ������*/
//uint8 dns_server[4]={192,168,1,103};									  /*����W5500Ĭ��DNS*/
//#endif

//uint8 local_ip[4]={172,16,10,4};												/*����W5500Ĭ��IP��ַ*/
//uint8 subnet[4]={255,255,255,0};												/*����W5500Ĭ����������*/
//uint8 gateway[4]={172,16,10,1};													/*����W5500Ĭ������*/
//uint8 dns_server[4]={211,167,230,100};									/*����W5500Ĭ��DNS*/

uint16 local_port=5000;	                       					/*���屾�ض˿�*/

//*����Զ��IP��Ϣ*/
//uint8  remote_ip[4]={124,192,224,226};							  /*Զ��IP��ַ*/
//uint16 remote_port=8010;														  /*Զ�˶˿ں�*/

//uint8  remote_ip[4]={192,168,1,4};
//uint16 remote_port=4446;																
uint8  remote_ip[4]={0,0,0,0};											/*Զ��IP��ַ*/
uint16 remote_port=0;																/*Զ�˶˿ں�*/

/*����Զ��IP��Ϣ*/
//uint8  remote_ip[4]={124,192,224,226};											/*Զ��IP��ַ*/
//uint16 remote_port=8010;																/*Զ�˶˿ں�*/



/*IP���÷���ѡ��������ѡ��*/
uint8	ip_from=IP_FROM_DEFINE;				

uint8 dhcp_ok=0;																				/*dhcp�ɹ���ȡIP*/
uint32	ms=0;																						/*�������*/
uint32	dhcp_time= 0;																		/*DHCP���м���*/
vu8	ntptimer = 0;																				/*NPT�����*/

/**
*@brief		����W5500��IP��ַ
*@param		��
*@return	��
*/
void set_w5500_ip(void)
{	
		
 /*���ƶ����������Ϣ�����ýṹ��*/
	memcpy(ConfigMsg.mac, mac, 6);
	memcpy(ConfigMsg.lip,local_ip,4);
	memcpy(ConfigMsg.sub,subnet,4);
	memcpy(ConfigMsg.gw,gateway,4);
	memcpy(ConfigMsg.dns,dns_server,4);
	
	/*ʹ��DHCP��ȡIP�����������DHCP�Ӻ���*/		
	/*����DHCP��ȡ��������Ϣ�����ýṹ��*/
	if(dhcp_ok==1)
	{
		printf("1.DHCP�ɹ���ȡIP!\r\n");		 
		memcpy(ConfigMsg.lip,DHCP_GET.lip, 4);
		memcpy(ConfigMsg.sub,DHCP_GET.sub, 4);
		memcpy(ConfigMsg.gw,DHCP_GET.gw, 4);
		memcpy(ConfigMsg.dns,DHCP_GET.dns,4);
	}
	else
	{
		printf("1.DHCP���ɹ���δ��ȡ!\r\n");
	}
		
	/*����������Ϣ��������Ҫѡ��*/	
	ConfigMsg.sw_ver[0]=FW_VER_HIGH;
	ConfigMsg.sw_ver[1]=FW_VER_LOW;	

	/*��IP������Ϣд��W5500��Ӧ�Ĵ���*/	
	setSUBR(ConfigMsg.sub);
	setGAR(ConfigMsg.gw);
	setSIPR(ConfigMsg.lip);
	
	getSIPR (local_ip);			
	printf("1.IP��ַ: %d.%d.%d.%d\r\n", local_ip[0],local_ip[1],local_ip[2],local_ip[3]);
	getSUBR(subnet);
	printf("2.��������:%d.%d.%d.%d\r\n", subnet[0],subnet[1],subnet[2],subnet[3]);
	getGAR(gateway);
	printf("3.Ĭ������:%d.%d.%d.%d\r\n", gateway[0],gateway[1],gateway[2],gateway[3]);
	printf("4.DNS������:%d.%d.%d.%d\r\n", dns_server[0],dns_server[1],dns_server[2],dns_server[3]);
}

/**
*@brief		����W5500��MAC��ַ
*@param		��
*@return	��
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
*@brief		����W5500��GPIO�ӿ�
*@param		��
*@return	��
*/
void gpio_for_w5500_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI2_FLASH_Init();																			 /*��ʼ��STM32 SPI2�ӿ�*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
 
	/*����RESET����*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;					   /*ѡ��Ҫ���Ƶ�GPIO����*/		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 /*������������Ϊ50MHz */		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		 /*��������ģʽΪͨ���������*/	
	GPIO_Init(GPIOC, &GPIO_InitStructure);							 /*���ÿ⺯������ʼ��GPIO*/
	GPIO_SetBits(GPIOC, GPIO_Pin_6);		

	/*����INT����*/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;						 /*ѡ��Ҫ���Ƶ�GPIO����*/		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 /*������������Ϊ50MHz*/		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;				 /*��������ģʽΪͨ������ģ����������*/		
	GPIO_Init(GPIOC, &GPIO_InitStructure);							 /*���ÿ⺯������ʼ��GPIO*/
}

/**
*@brief		W5500Ƭѡ�ź����ú���
*@param		val: Ϊ��0����ʾƬѡ�˿�Ϊ�ͣ�Ϊ��1����ʾƬѡ�˿�Ϊ��
*@return	��
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
*@brief		����W5500��Ƭѡ�˿�SCSnΪ��
*@param		��
*@return	��
*/
void iinchip_csoff(void)
{
	wiz_cs(LOW);
}

/**
*@brief		����W5500��Ƭѡ�˿�SCSnΪ��
*@param		��
*@return	��
*/
void iinchip_cson(void)
{	
   wiz_cs(HIGH);
}

/**
*@brief		W5500��λ���ú���
*@param		��
*@return	��
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
	/*ʹ��Ұ��ISO��׼������*/
//	#ifdef STM32F103ZET6
		GPIO_ResetBits(GPIOC, WIZ_RESET);
		delay_us1(2);  
		GPIO_SetBits(GPIOC, WIZ_RESET);
//		delay_us1(1600000);
// 		OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err);
	  delay_ms(1600);	
//	#endif
	
	/*ʹ��Ұ��MINI������*/
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
//	OS_CRITICAL_ENTER();//�����ٽ���
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI2, byte);

  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
//	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI2);

}

/**
*@brief		STM32 SPI1��д8λ����
*@param		dat��д���8λ����
*@return	��
*/
uint8  IINCHIP_SpiSendData(uint8 dat)
{
   return(SPI2_FLASH_SendByte(dat));
}

/**
*@brief		д��һ��8λ���ݵ�W5500
*@param		addrbsb: д�����ݵĵ�ַ
*@param   data��д���8λ����
*@return	��
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
*@brief		��W5500����һ��8λ����
*@param		addrbsb: д�����ݵĵ�ַ
*@param   data����д��ĵ�ַ����ȡ����8λ����
*@return	��
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
*@brief		��W5500д��len�ֽ�����
*@param		addrbsb: д�����ݵĵ�ַ
*@param   buf��д���ַ���
*@param   len���ַ�������
*@return	len�������ַ�������
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
*@brief		��W5500����len�ֽ�����
*@param		addrbsb: ��ȡ���ݵĵ�ַ
*@param 	buf����Ŷ�ȡ����
*@param		len���ַ�������
*@return	len�������ַ�������
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
*@brief		д������Ϣ��EEPROM
*@param		��
*@return	��
*/
void write_config_to_eeprom(void)
{
	uint16 dAddr=0;
//	ee_WriteBytes(ConfigMsg.mac,dAddr,(uint8)EEPROM_MSG_LEN);				
	delay_ms(10);																							
}

/**
*@brief		��EEPROM��������Ϣ
*@param		��
*@return	��
*/
void read_config_from_eeprom(void)
{
//	ee_ReadBytes(EEPROM_MSG.mac,0,EEPROM_MSG_LEN);
	delay_us(10);
}

/**
*@brief		STM32��ʱ��2��ʼ��
*@param		��
*@return	��
*/
void timer2_init(void)
{
//	TIM2_Configuration();																		/* TIM2 ��ʱ���� */
//	TIM2_NVIC_Configuration();															/* ��ʱ�����ж����ȼ� */
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);		/* TIM2 ���¿�ʱ�ӣ���ʼ��ʱ */
}

/**
*@brief		dhcp�õ��Ķ�ʱ����ʼ��
*@param		��
*@return	��
*/
void dhcp_timer_init(void)
{
			timer2_init();																	
}

/**
*@brief		ntp�õ��Ķ�ʱ����ʼ��
*@param		��
*@return	��
*/
void ntp_timer_init(void)
{
			timer2_init();																	
}

/**
*@brief		��ʱ��2�жϺ���
*@param		��
*@return	��
*/
void timer2_isr(void)
{
	ms++;	
  if(ms>=1000)
  {  
    ms=0;
    dhcp_time++;																					/*DHCP��ʱ��1S*/
		#ifndef	__NTP_H__
		ntptimer++;																						/*NTP����ʱ���1S*/
		#endif
  }

}
/**
*@brief		STM32ϵͳ��λ����
*@param		��
*@return	��
*/
void reboot(void)
{
  pFunction Jump_To_Application;
  uint32 JumpAddress;
  printf(" ϵͳ�����С���\r\n");
  JumpAddress = *(vu32*) (0x00000004);
  Jump_To_Application = (pFunction) JumpAddress;
  Jump_To_Application();
}
