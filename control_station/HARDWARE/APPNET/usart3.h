#ifndef __USART3_H
#define __USART3_H

void Usart3_init(unsigned int bound,unsigned int parity,unsigned int stopbits);
unsigned char USART3_Send_Data(unsigned char *Data,unsigned int Lenth);
extern void (*pUsart3Rx_Isr)(unsigned char);
#endif

