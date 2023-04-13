#ifndef __UART5_H
#define __UART5_H

extern void (*pUsart5Rx_Isr)(unsigned char);
void Uart5_init(unsigned int bound,unsigned char parity);
unsigned char UART5_Send_Data(unsigned char *Data,unsigned int Lenth);
extern unsigned char uart5_receive_data[50];
extern unsigned char uart5_receive_data_inc;
#endif

