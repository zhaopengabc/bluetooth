#ifndef __USART2_H
#define __USART2_H
extern unsigned char usart2_receive_data[50];
extern unsigned char usart2_receive_data_inc;
void GM3_Send_Data(unsigned char* data);  
void Usart2_init(unsigned int bound,unsigned char parity);
unsigned char USART2_Send_Data(unsigned char *Data,unsigned int Lenth);
extern void (*pUsart2Rx_Isr)(unsigned char);
#endif

