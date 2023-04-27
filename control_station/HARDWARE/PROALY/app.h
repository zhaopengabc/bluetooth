#ifndef __PROALY_H
#define __PROALY_H

#include "stm32f10x.h"  
#include "HRSTDDEF.h"

#define UartInterface                   USART3
#define UartInterface_485               USART2

extern unsigned char PushButtonStatus;
extern volatile unsigned char SystemTime[6];
extern volatile unsigned int Sys_Second_100mS;
extern unsigned int MClineFlaut_flag;

void ProalyInit(void);
void ProalyRun(void);
void gMessageSent(HRINT8U *Message,HRINT16U iSIZE);
void Time_System_Get(void);
#endif





