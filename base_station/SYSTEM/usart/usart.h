#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
extern unsigned char receive_once;//蓝牙离位基站一次搜索搜索到的设备数量。
/***************************串口结合定时器接收数据***************************/
extern unsigned char USART1_RX_FLAG;       	//置标志位，告诉定时器开始收数了。
extern unsigned char USART1_RX_TIME;       	//接收字符串时的时间间隔
extern unsigned int  USART1_RX_NO;         	//接收字符串字符的总数量
extern unsigned char USART1_GROUP;   				//最大接收缓存数组个数	
extern unsigned char USART1_RX_BUF[10][100];//接收缓冲二维数组。
extern unsigned char USART1_finger_point;   //数组消息循环中指针指向的位置。
/***************************************************************************/
//如果想串口中断接收，请不要注释以下宏定义
void USART1_Init(u32 bound);//串口初始化
void USART1_RX_SCAN(void);//定时器定时扫描串口标志位的变化，及时的进行处理。
void USART1_COMAND_SCAN(void);//刷新读取串口1数据。查看是否有命令过来需要处理。 
void USART1_SEND(unsigned char* USART_TX_BUF,unsigned int size); // USART1_TX_BUF代表要输出的数组。size代表输出数组的长度。 
void USART1_Data_analyze(unsigned char *USART1_RXD,unsigned char lenth);//串口1收数数据解析.USART1_RXD是数据数组，lenth是数据长度。
#endif


