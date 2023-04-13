#ifndef __W25Q16_H
#define __W25Q16_H
#include "spi_flash.h"

#define RECORD_AREA			0x40000
#define CONFIG_AREA			0x50000
#define FLASH_VIRBLOCK_LEN	0x20

#define UPDATE_CONFIG_CMD		0x00
#define SERVER_CONFIG_CMD		0x10		//有线服务器域名；
#define SERVER_INTERFACE_CONFIG_CMD	0x30		//服务器接口方式，有线，无线；
#define K2_IP_CONFIG_CMD		  0x40  //有线服务器IP地址；
#define GPRS_SN_CONFIG_CMD		0x50  //有线的MAC地址；
#define GPRS_IMEI_CONFIG_CMD	0x60  //有线的local_ip+subnet+gateway+dns_server；
#define GPRS_PHONE_CONFIG_CMD	0x70  //IP自动，IP手动切换
#define HOST_CONFIG_CMD			0x80    //BT功能标志位
#define CAN_CONFIG_CMD			0x90
#define RS485_CONFIG_CMD		0xa0   
#define LCD_CONFIG_CMD			0xb0
#define LCD_VOICE_CMD			  0xb2
#define LCD_MIMA_CMD			  0xb4
#define VER_CONFIG_CMD			0xc0
#define TIMING_CMD				  0xd0
#define ITEM_CONFIG_CMD			0xe0  //用户编码表条数；
#define ProID_CONFIG_CMD		0xe4  //协议卡注册标志位；
#define ID_CONFIG_CMD			  0xf0  //本机ID；
#define SERVER_PORT_CMD		  0x2E	//有线服务器端口；

void W25q16_Init(void);
void Read_Record(unsigned char *buf);
unsigned char Writeflash_Infor(u8* pBuffer, u16 NumByteToWrite);
unsigned char Readflash_Infor(u8* pBuffer, unsigned int sn);
unsigned char Readflash_Config(unsigned char *data,unsigned char cmd);
unsigned char Writeflash_Config(unsigned char *data,unsigned char cmd);
unsigned int Clear_LCD_Num(void);
void W25q16_Lock(void);
void W25q16_UnLock(void);
extern unsigned char SD_Writeflash_Infor(u8* pBuffer, u16 NumByteToWrite);
extern unsigned char SD_Readflash_Infor(u8* pBuffer, unsigned int sn);
extern void Config_LcdCopy(unsigned char *destination);
#endif

