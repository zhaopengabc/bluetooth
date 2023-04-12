#ifndef _MEMORY_H
#define _MEMORY_H
#include "os.h"
#include "stm32f10x_it.h"
#include "usart.h"
#include <string.h>

void POST(OS_Q* Original_MSG,u32 length,u8* str,OS_MEM* memory);
u32 PEND(OS_Q* Original_MSG,u8* str,OS_MEM* memory);
u32 Creat_Memory(void);
u32 Create_Msg(void);

#define BT_Q_NUM 20
//#define CAN_Q_NUM 20
//#define COMP_Q_NUM 20
//#define ANAL_Q_NUM 20
//#define DOWN_Q_NUM 20
//#define ACK_Q_NUM 20

extern OS_Q BT_Msg;
//extern OS_Q CAN_Msg;
//extern OS_Q COMP_Msg;
//extern OS_Q ANAL_Msg;
//extern OS_Q DOWN_Msg;
//extern OS_Q ACK_Msg;
extern OS_MEM STORAGE_MEM;	
//extern OS_MEM DOWNLOAD_MEM;

#endif
