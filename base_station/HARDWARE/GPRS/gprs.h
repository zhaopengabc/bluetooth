#ifndef GPRS_H
#define GPRS_H
#include "stdio.h"	
#include "sys.h"  

//#define GM3_LINKA 							GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)//Socket A 状态， 高电平为已连接， 低电平为未连接
#define READ_GPRS_POWER_STATUS 	GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)//GM3的WORK引脚。模块正常启动后， 电平一秒变化一次， 高低电平交替变换， 指示灯呈闪烁状态。
#define GPRS_MOS_POWER_ENABLE   GPIO_SetBits(GPIOB,GPIO_Pin_12)		//GPRS模块MOS管控制电源接通
#define GPRS_MOS_POWER_DISABLE  GPIO_ResetBits(GPIOB,GPIO_Pin_12) //GPRS模块MOS管控制电源断开

#define GPRS_SOFT_POWER_ENABLE  GPIO_SetBits(GPIOC,GPIO_Pin_6)		//GPRS模块软件控制电源接通
#define GPRS_SOFT_POWER_DISABLE GPIO_ResetBits(GPIOC,GPIO_Pin_6) //GPRS模块软件控制电源断开

#define GPRS_REST_ENABLE     GPIO_SetBits(GPIOB,GPIO_Pin_15)	  //GPRS复位（低电平复位）//因为在通讯板上面做了电压反向，所以这里就是高电平了。
#define GPRS_REST_DISABLE    GPIO_ResetBits(GPIOB,GPIO_Pin_15)  //GPRS不复位（高电平不复位）

extern unsigned int GM3_LINKA_TIME_LOSE;//GPRS掉线时间。如果超过这个时间，则需要重启GPRS模块了。硬件检测LINK引脚，因为需要修改测试，所以做成全局变量。相当于宏定义。


void GPRS_Init(void);
unsigned char gm3_send_cmd(unsigned char *cmd,unsigned char *ack,u16 waittime);
unsigned char *gm3_check_cmd(unsigned char *str,unsigned char *receive_data);//判断 receive_data 这个数组中是否含有 str 这个字符串。
extern unsigned char sendata_flag;
extern unsigned int GM3_LINKA_TIME;//GPRS掉线时间。如果超过这个时间，则需要重启GPRS模块了。
void GPRS_RESTAR(void);//GPRS重启。
#endif

