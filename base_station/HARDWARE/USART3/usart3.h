#ifndef __USART3_H
#define __USART3_H	
#include "stdio.h"	
#include "sys.h"  						  
/***************************串口结合定时器接收数据***************************/   
extern unsigned char USART3_RX_FLAG;       	//置标志位，告诉定时器开始收数了。
extern unsigned char USART3_RX_TIME;       	//接收字符串时的时间间隔
extern unsigned int  USART3_RX_NO;         	//接收字符串字符的总数量
extern unsigned char USART3_GROUP;   				//最大接收缓存数组个数	
extern unsigned char USART3_RX_BUF[10][100];//接收缓冲二维数组。
extern unsigned char USART3_RX_BUF_middle[100]; 	//接收缓冲二维数组。
extern unsigned char USART3_finger_point;   //数组消息循环中指针指向的位置。
/***************************************************************************/
extern unsigned char send_success_flag;//往网关或者服务器发送数据成功标志位。成功，返回1。失败，返回0。
//extern u8 dispose_flag;//串口处理函数正在工作。
//如果想串口中断接收，请不要注释以下宏定义
void USART3_Init(u32 bound);
void u3_printf(char* fmt,...);//串口2打印函数 
//串口2,输出字符串函数
void USART3_SEND(unsigned char* USART3_TX_BUF,unsigned int size); 
void USART3_RX_SCAN(void);//定时器定时扫描串口标志位的变化，及时的进行处理。
void USART3_COMAND_SCAN(void);//刷新读取串口1数据。查看是否有命令过来需要处理。 
unsigned char Wireless_Send_Data(unsigned char *data,unsigned char size);//data是发送的数组。size是数据长度。
void LoRa_433_Pack_DATA(unsigned char *data);//Package_DATA[0]代表数组Package_DATA中元素的个数+1.	数组中有多少组数据的算法就是数组中元素的个数除以6。
void USART3_Data_analyze(unsigned char *USART3_RXD,unsigned char lenth);//串口3收数数据解析.USART3_RXD是数据数组，lenth是数据长度。
#endif



























