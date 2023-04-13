#ifndef __TCP_DEMO_H
#define __TCP_DEMO_H
#include "types.h"

typedef struct
{
unsigned int leg;	
unsigned char data[512];
}TCP_DATA;

extern uint16 W5500_tcp_server_port;
void do_tcp_server(void);//TCP Server回环演示函数
unsigned int do_tcp_client_tx(unsigned char *TXdata,unsigned int length) ;
unsigned int do_tcp_client_rx(unsigned char *RXdata);
unsigned int do_tcp_client_RxStatus(void);
unsigned int do_tcp_client_RxRes(void);
#endif 

