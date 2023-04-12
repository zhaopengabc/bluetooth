#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

//#define KEY0  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)//读取按键0
#define KEY0  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)//读取按键0
//#define GPRS_LINK  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)//连接网络成功则为高，否则为低。
//#define GPRS_WORK  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)//GM3工作状态指示灯，电平一秒一次变化。
extern unsigned char KEY_flag;//当 KEY_flag=1的时候，表示短按。 KEY_flag=2的时候，表示长按。
extern unsigned char KEY0_DN_L;//定义全局变量。按键被长按了
extern unsigned char KEY0_DN_S;//定义全局变量。按键被短按了
void KEY_Init(void);//按键初始化
//void GM3_IO_Init(void);//GM3模块IO初始化
void KEY_SCAN(void);//按键扫描		
#endif
