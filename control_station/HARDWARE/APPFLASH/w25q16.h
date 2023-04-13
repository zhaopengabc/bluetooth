#ifndef __W25Q16_H
#define __W25Q16_H
#include "spi_flash.h"

#define RECORD_AREA			0x40000
#define CONFIG_AREA			0x50000
#define FLASH_VIRBLOCK_LEN	0x20

#define UPDATE_CONFIG_CMD		0x00
#define SERVER_CONFIG_CMD		0x10		//���߷�����������
#define SERVER_INTERFACE_CONFIG_CMD	0x30		//�������ӿڷ�ʽ�����ߣ����ߣ�
#define K2_IP_CONFIG_CMD		  0x40  //���߷�����IP��ַ��
#define GPRS_SN_CONFIG_CMD		0x50  //���ߵ�MAC��ַ��
#define GPRS_IMEI_CONFIG_CMD	0x60  //���ߵ�local_ip+subnet+gateway+dns_server��
#define GPRS_PHONE_CONFIG_CMD	0x70  //IP�Զ���IP�ֶ��л�
#define HOST_CONFIG_CMD			0x80    //BT���ܱ�־λ
#define CAN_CONFIG_CMD			0x90
#define RS485_CONFIG_CMD		0xa0   
#define LCD_CONFIG_CMD			0xb0
#define LCD_VOICE_CMD			  0xb2
#define LCD_MIMA_CMD			  0xb4
#define VER_CONFIG_CMD			0xc0
#define TIMING_CMD				  0xd0
#define ITEM_CONFIG_CMD			0xe0  //�û������������
#define ProID_CONFIG_CMD		0xe4  //Э�鿨ע���־λ��
#define ID_CONFIG_CMD			  0xf0  //����ID��
#define SERVER_PORT_CMD		  0x2E	//���߷������˿ڣ�

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

