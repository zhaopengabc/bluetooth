#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////////////////////////
//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 256 	 		//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1              //ʹ��FLASHд��(0��������;1��ʹ��)
//////////////////////////////////////////////////////////////////////////////////////////////////////
//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
#define NUMBER_ADDRESS ((unsigned long)0X08020000)  // �����ַ�洢��������䣺��0~5�豸ID,6~7��·�ţ�8~9�����ţ�10~11�洢�Ľڵ�������ǰ6���ֽ���������վ��6λID���롣����9��10���ֽ��Ǵ洢���豸��������
#define DEVICE_ADDRESS ((unsigned long)0X08020800)  // �洢�Ľڵ�ID��6���ֽ�һ�������Ρ�
//STM32F103RCT6���ڴ�����оƬ���ڲ�romΪ128KB.ÿҳ�ܴ洢2048���ֽڡ�
//#define NUMBER_ADDRESS ((unsigned long)0X08050000)  // 1���ֽڣ�����ע��ĸ���
//#define DEVICE_ADDRESS ((unsigned long)0X08050800)  // �豸��ַ��ʼ��6���ֽ�һ������ǰ�����ֽڴ����豸�ĵ�ַ�����ĸ��ֽڴ����豸���͡�������ֽڴ����·�ţ��������ֽڴ������š�
////STM32F103RCT6���ڴ�����оƬ���ڲ�romΪ128KB.ÿҳ�ܴ洢2048���ֽڡ�

#define Bluetooh_Max_NO 16

//extern unsigned int  Bluetooh_Max_NO;//������վ���Ŷ�洢�ı�ǩ������
extern unsigned char Gateway_ID[6];//����ID
extern unsigned char Bluetooh_amount;//������ǩ������
u16 STMFLASH_ReadHalfWord(u32 faddr);		  //��������  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//ָ����ַ��ʼ��ȡָ����������
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
unsigned char Judge_Register(unsigned char *data);//�жϺϷ��ԡ��Ƿ��Ѿ���EEPROM���д洢ע��,�������1�����ǺϷ��豸���������0�����ǷǷ��豸��
//unsigned int Save_NODE_ID(unsigned char *data);//�����豸ID
void Star_Read_ID(void);//��ȡ����ӡ�豸ID
void Judge_Erase_all_ID(void);//�ж������Ƿ���Ҫ��������ID��	
//����д��
void Test_Write(u32 WriteAddr,u16 WriteData);				
void Refurbish_Star_Read_ID(void);	//ˢ��ȫ�ֵı�־λ��
#endif

















