#ifndef __UART4_H
#define __UART4_H	 
#include "stdio.h"	
#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口2驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/3/29
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
extern u16 mark_status;//离位标签的标志位。按位来算。当为0的时候，表示离位。例如 mark_tatus 当前的值为：0100 0011  则说明8，6，5，4，3号标签发生了离位。
extern u16 mark_status_last;//上次离位标签的标志位。
extern unsigned int departure_time; 			//宏定义离位时间。
extern unsigned int judge_fruit_time;//检测数组中的数据。判断是否有离位。防止每次都进入做对比。浪费单片机资源
extern unsigned int bluetooh_scan_heart_time; //蓝牙基站巡检时间
/***************************串口结合定时器接收数据***************************/
extern unsigned char UART4_RX_FLAG;       	//置标志位，告诉定时器开始收数了。
extern unsigned char UART4_RX_TIME;       	//接收字符串时的时间间隔
extern unsigned int  UART4_RX_NO;         	//接收字符串字符的总数量
extern unsigned char UART4_GROUP;   				//最大接收缓存数组个数	
extern unsigned char UART4_RX_BUF[10][100];//接收缓冲二维数组。
extern unsigned char UART4_RX_BUF_middle[100]; 	//接收缓冲二维数组。
extern unsigned char UART4_finger_point;   //数组消息循环中指针指向的位置。
/***************************************************************************/
//extern u8 dispose_flag;//串口处理函数正在工作。
//如果想串口中断接收，请不要注释以下宏定义
void UART4_Init(u32 bound);
void test_URAT4(void);
void u4_printf(char* fmt,...);//串口2打印函数 
//串口2,输出字符串函数
void UART4_SEND(unsigned char* UART4_TX_BUF,unsigned int size); 
void UART4_RX_SCAN(void);//定时器定时扫描串口标志位的变化，及时的进行处理。
void UART4_COMAND_SCAN(void);//刷新读取串口1数据。查看是否有命令过来需要处理。 
void UART4_SEND(unsigned char* UART4_TX_BUF,unsigned int size); // USART1_TX_BUF代表要输出的数组。size代表输出数组的长度。
void Bluetooh_SCAN(void);//刷新巡检蓝牙串口数据。
void dislocation_test(void);//短按键后，发送一帧离位报警。包括所有的节点信息。
void UART4_Data_analyze(unsigned char *UART4_RXD,unsigned char lenth);//串口4收数数据解析.UART4_RXD是数据数组，lenth是数据长度。
void get_all_status(void);
#endif











