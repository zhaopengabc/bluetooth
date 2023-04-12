#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "stmflash.h"
extern unsigned char TIM2_IRQ_flag;
extern unsigned int Send_Overtime;//网关或者服务器发送数据超时时间检测
extern unsigned int Bluetooh_Package_compare;//蓝牙数据包进行比较倒计时暂定5秒比较一次。
extern unsigned int Bluetooh_scan_time;//蓝牙基站定时扫描时间，到0的时候进行扫描，扫描完成重新赋值。
extern unsigned int Bluetooh_heartbeat_time;//蓝牙基站上传给基站的心跳包。5分钟上发一次。
extern unsigned int Bluetooh_time[Bluetooh_Max_NO];//假设最多有8个蓝牙标签.进行倒计时的时候，整个数组都进行倒计时。	
extern unsigned int send_time;//进行查询是否需要给服务器发送数据
extern unsigned int GM3_LINKA_TIME;//GPRS失联时间倒计时。如果失联时间超过一定时间，就重启。
extern unsigned int KEY0_DN_S_effective_time;//短按按键有效时间。
extern unsigned int KEY0_DN_L_effective_time;//长按按键有效时间。
void TIM2_Int_Init(u16 arr,u16 psc);
void timer_flag_int(void);//系统内标志位进行自减操作。
#endif
