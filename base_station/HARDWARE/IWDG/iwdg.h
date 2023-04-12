#ifndef __IWDG_H
#define __IWDG_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//看门狗 驱动代码		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/5/30
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  
#define DOG_FEED PCout(1)//喂狗引脚，翻转电平信号即可喂狗。
#define DOG_EN PCout(2)	//看门狗使能芯片。低电平看门狗工作，高电平看门狗不工作。 	

void IWDG_Init(u8 prer,u16 rlr);
void IWDG_Feed(void);

void Watchdog_Init(void); 
void Watchdog_FEED(void);//喂狗

#endif
