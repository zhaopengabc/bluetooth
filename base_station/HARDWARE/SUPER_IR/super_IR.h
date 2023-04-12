#ifndef __SUPER_IR_H
#define __SUPER_IR_H
#include "sys.h"
/**********************宏定义****************************************/
#define MAX_TB_TIME 500          //定义一个处理时间，数量为50*周期，当没有上升沿的时候，处理这个

/**********************全局变量**************************************/
extern unsigned long int RF_address;     // 全局变量，用于存储
extern unsigned char Rf_ok_flag;         // 全局变量，接收到一个完整的遥控命令后置1,通知解码程序可以解码了
extern unsigned char RF_data;            // 全局变量，用于存放数据

/**********************函数声明***************************************/
extern void RF_GPIO_Init(void);  				 // 433M IO初始化
extern void DecodingEv1527(void);				 // 433M 轮询扫描

#endif
