#ifndef __ATREV_H
#define __ATREV_H
#include "stm32f10x.h" 

extern unsigned int Uart1_Rx_Read(unsigned char *Rxdata);
extern void Reset_Uart1RxBuf(void);
#endif

