/*  BEGIN_FILE_HDR
************************************************************************************************
*   NOTICE                              
************************************************************************************************
*   File Name       :  
************************************************************************************************
*   Project/Product : 
*   Title           : 
*   Author          : 
************************************************************************************************
*   Description     : 
*
************************************************************************************************
*   Limitations     : 
*
************************************************************************************************
*
************************************************************************************************
*   Revision History:
* 
*    Version     Date           Initials       CR#           Descriptions
*    --------    ----------     -----------    ----------    ---------------
*    1.0         2017/08/03     ning.zhao     N/A           Original                    
************************************************************************************************
* END_FILE_HDR*/

#ifndef USART_MSG_PROCESS_DRV_H
#define USART_MSG_PROCESS_DRV_H

/*Include files*/
#include "MsgCANProcessHWExt.h"
#include "MsgCANProcessCfgExt.h"
#include "MsgCANProcessAppExt.h"
/* support module */
#include <string.h>    
#include "stm32f10x_dma.h"
#include "HRSTDDEF.h"


#define HOST_DATA_NUM 0x200
 
#define CAN_RX_DATANUM 1024			//CAN数组长度
#define CAN_RX_GROUPNUM 3			//循环队列长度 
 
 
 
//服务器接口方式
typedef enum
{
	RS485_232_Mode=1,
	CAN_Mode
}HostInterMode_TypeDef;

typedef struct
{
	unsigned char ID_Number;
	unsigned char Flag;
	unsigned int Lenth;
	unsigned char data[CAN_RX_DATANUM];
}CAN_Data_Typedef;



extern unsigned char Host_Interface_Mode;

typedef struct
{
	unsigned char Flag;
	unsigned int Lenth;
	unsigned char data[HOST_DATA_NUM];
}Host_Data_Typedef;

extern Host_Data_Typedef Data_RecieveFromHost;

//内部函数导出声明
extern void YH_CAN1_RX0_IRQHandler(void);		//@中断接收数据，在CAN中断函数使用
extern unsigned char CAN1_Send_Frame(unsigned char *msg, unsigned int len,unsigned char Board_Number);     //CAN1连续发送函数
//extern unsigned int CAN1_Read(unsigned char *Rxdata);   //CAN1读取数据
extern void  CAN1_Read(CAN_Data_Typedef *S_Rxdata);   //CAN1读取数据







 


/*****following definitions can not be used outside this driver******/
/* macro definition */

/* data type definiton  */

#endif /* End of header file */
