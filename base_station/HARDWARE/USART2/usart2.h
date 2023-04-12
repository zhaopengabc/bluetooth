#ifndef __USART2_H
#define __USART2_H	
#include "stdio.h"	
#include "sys.h"  
//extern unsigned int send_lenth;//数组中元素的个数。
extern unsigned char send_message[100];//发送数据数组。
extern unsigned char SEND_once;//判断重发次数。
//设置心跳周期大小
extern unsigned int heartbeat_time;//心跳时间定义1分钟心跳一次。
extern unsigned int timeout;//定时器中断是5ms进入一次。进入2000次，则为10秒钟。给服务器发送一帧数据的超时时间。如果超过这个时间，则认为发送时间太长。
//////////////////////////////////////////////////////////////////////////////////
extern unsigned int SEND_DATA_to_server_time;//往服务器发送数据倒计时。如果超过这个时间没有发送成功，则重新发送。
extern unsigned char SEND_DATA_to_server_GROUP;//网关需要给服务器发送数组的组数。
extern unsigned char SEND_DATA_to_server_ok;//发送成功为0，发送失败为1.

/***************************串口结合定时器接收数据***************************/
extern unsigned char USART2_RX_FLAG;       	//置标志位，告诉定时器开始收数了。
extern unsigned char USART2_RX_TIME;       	//接收字符串时的时间间隔
extern unsigned int  USART2_RX_NO;         	//接收字符串字符的总数量
extern unsigned char USART2_GROUP;   				//最大接收缓存数组个数	
extern unsigned char USART2_RX_BUF[10][100];//接收缓冲二维数组。
extern unsigned char USART2_RX_BUF_middle[100]; 	//接收缓冲二维数组。
extern unsigned char USART2_finger_point;   //数组消息循环中指针指向的位置。
/***************************************************************************/
extern unsigned char finger_point;//数组消息循环中指针指向的位置。
extern unsigned char send_data[20][80];//定义20组数据，用于重发时堆栈用。如果是蓝牙离位基站上传最多8个标签，则一包最多字节为400字节。做好预留，则410个字节。
extern unsigned short  last_serial_number;//上一次的业务流水号
extern unsigned short  serial_number_counter;//业务流水号。	


//extern u8 dispose_flag;//串口处理函数正在工作。
//如果想串口中断接收，请不要注释以下宏定义
void USART2_Init(u32 bound);
void u2_printf(char* fmt,...);//串口2打印函数 
//串口2,输出字符串函数
void USART2_SEND(unsigned char* USART2_TX_BUF,unsigned int size);
void USART2_RX_SCAN(void);//定时器定时扫描串口标志位的变化，及时的进行处理。
void USART2_COMAND_SCAN(void);//刷新读取串口1数据。查看是否有命令过来需要处理。 
void Judge_GM3_send_data(void);//判断网关是否需要通过GM3上传给服务器数据。
void Router_send_data_to_server(unsigned char *message);//网关发送数据给服务器
void package_send_data(unsigned char *data);//	data是数据数组。数组中有多少组数据的算法就是数组中元素的个数减1然后除以6。数组的第一个元素是数组中元素个数。size代表数组中元素的个数
void USART2_Data_analyze(unsigned char *USART2_RXD,unsigned char lenth);//串口2收数数据解析.USART2_RXD是数据数组，lenth是数据长度。
#endif













