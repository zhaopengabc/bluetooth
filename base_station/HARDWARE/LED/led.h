#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define LED0 PBout(5)	// ����ָʾ�� 	

//#define LoRa_EN PBout(1)	// 
#define M433_EN PAout(11)	// ����ָʾ�� 	
//#define BT_RESET PDout(2)	// ����ģ�鸴λ 	���͵�ƽ��λ��
void ENABLE_IO_Init(void);//ʹ�����ų�ʼ��	
void LED_Init(void);//��ʼ��	
void LED_SCAN(void);//LED
extern unsigned char LED_flag;//LED��˸��־λ��
#endif
