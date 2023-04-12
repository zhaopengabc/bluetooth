#ifndef __LED1_H
#define __LED1_H
#include "sys.h"

#define HuoJing_LED			0
#define JianGuan_LED		1
#define GuZhang_LED			2
#define PingBi_LED			3
#define GongZuo_LED			6
#define ZongGuZhang_LED		7

#define ZhuJiTongXin_GLED	9
#define ZhuJiTongXin_YLED	8
#define WangLuoTongXin_GLED	10
#define WangLuoTongXin_YLED	11
#define ZhuDianGuZhang_GLED	12
#define ZhuDianGuZhang_YLED	13
#define BeiDianGuZhang_GLED	15
#define BeiDianGuZhang_YLED	14

#define WangLuoGuZhang_LED	4
#define XianLuGuZhang_LED	5
#define JingQingXiaoChu_LED	21
#define GuZhangXiaoChu_LED	20
#define BenJiZiJian_LED		19
#define XiaoYin_LED			18
#define ShouDongBaoJing_LED	17
#define ChaGangYingDa_LED	16

//端口定义
#define SCK_595  PEout(13)	// 	 
#define RCK_595  PEout(15)	// 
#define DATA_595 PEout(14)	// 

//extern u8 hc595_status1;//第一个 74HC595 输入量的值
//extern u8 hc595_status2;//第二个 74HC595 输入量的值
//extern u8 hc595_status3;//第三个 74HC595 输入量的值

//extern u8 LED_DATA[24][3]; //灯有24个。每个灯有3个变量。灯闪烁时亮的时间。灯闪烁时灭的时间。灯闪烁的次数。
//extern u8 LED_ON_TIME[24];//灯打开的时间。
//extern u8 LED_OFF_TIME[24];//灯关闭的时间。
//extern u8 LED_status[24];//共24个灯。每个灯的状态标志位。1为点亮，0为灭。

//extern u16 keep_time_heartbeat;//灯维持时间倒计时的心跳		

void HC595_Init(void);//初始化		 
void HC595SendData(u8 SendVal);
void refurbish_hc595(void);//刷新74HC595
void LED_TWINKLE(u8 number,u8 on_time,u8 off_time,u8 keep_time);//灯闪烁函数

void LED_SCAN(void);//灯扫描	
void delay_hc595(u16 once_hc595);//刷新74HC595的时候的nop
#endif

/***********************end of file**************************/
