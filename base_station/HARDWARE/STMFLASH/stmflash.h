#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////////////////////////
//用户根据自己的需要设置
#define STM32_FLASH_SIZE 256 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 1              //使能FLASH写入(0，不是能;1，使能)
//////////////////////////////////////////////////////////////////////////////////////////////////////
//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址
#define NUMBER_ADDRESS ((unsigned long)0X08020000)  // 这个地址存储的数组分配：（0~5设备ID,6~7回路号，8~9部件号，10~11存储的节点数量）前6个字节是蓝牙基站的6位ID号码。后面9，10个字节是存储的设备的数量。
#define DEVICE_ADDRESS ((unsigned long)0X08020800)  // 存储的节点ID，6个字节一个。依次。
//STM32F103RCT6属于大容量芯片。内部rom为128KB.每页能存储2048个字节。
//#define NUMBER_ADDRESS ((unsigned long)0X08050000)  // 1个字节，储存注册的个数
//#define DEVICE_ADDRESS ((unsigned long)0X08050800)  // 设备地址开始，6个字节一组数。前三个字节代表设备的地址。第四个字节代表设备类型。第五个字节代表回路号，第六个字节代表部件号。
////STM32F103RCT6属于大容量芯片。内部rom为128KB.每页能存储2048个字节。

#define Bluetooh_Max_NO 16

//extern unsigned int  Bluetooh_Max_NO;//蓝牙基站中最富哦存储的标签数量。
extern unsigned char Gateway_ID[6];//网关ID
extern unsigned char Bluetooh_amount;//蓝牙标签总数量
u16 STMFLASH_ReadHalfWord(u32 faddr);		  //读出半字  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//指定地址开始写入指定长度的数据
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//指定地址开始读取指定长度数据
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//从指定地址开始读出指定长度的数据
unsigned char Judge_Register(unsigned char *data);//判断合法性。是否已经在EEPROM中有存储注册,如果返回1，则是合法设备。如果返回0，则是非法设备。
//unsigned int Save_NODE_ID(unsigned char *data);//保存设备ID
void Star_Read_ID(void);//读取并打印设备ID
void Judge_Erase_all_ID(void);//判断网关是否需要擦除所有ID。	
//测试写入
void Test_Write(u32 WriteAddr,u16 WriteData);				
void Refurbish_Star_Read_ID(void);	//刷新全局的标志位。
#endif

















