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

//�˿ڶ���
#define SCK_595  PEout(13)	// 	 
#define RCK_595  PEout(15)	// 
#define DATA_595 PEout(14)	// 

//extern u8 hc595_status1;//��һ�� 74HC595 ��������ֵ
//extern u8 hc595_status2;//�ڶ��� 74HC595 ��������ֵ
//extern u8 hc595_status3;//������ 74HC595 ��������ֵ

//extern u8 LED_DATA[24][3]; //����24����ÿ������3������������˸ʱ����ʱ�䡣����˸ʱ���ʱ�䡣����˸�Ĵ�����
//extern u8 LED_ON_TIME[24];//�ƴ򿪵�ʱ�䡣
//extern u8 LED_OFF_TIME[24];//�ƹرյ�ʱ�䡣
//extern u8 LED_status[24];//��24���ơ�ÿ���Ƶ�״̬��־λ��1Ϊ������0Ϊ��

//extern u16 keep_time_heartbeat;//��ά��ʱ�䵹��ʱ������		

void HC595_Init(void);//��ʼ��		 
void HC595SendData(u8 SendVal);
void refurbish_hc595(void);//ˢ��74HC595
void LED_TWINKLE(u8 number,u8 on_time,u8 off_time,u8 keep_time);//����˸����

void LED_SCAN(void);//��ɨ��	
void delay_hc595(u16 once_hc595);//ˢ��74HC595��ʱ���nop
#endif

/***********************end of file**************************/
