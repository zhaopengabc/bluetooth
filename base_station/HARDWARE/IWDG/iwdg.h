#ifndef __IWDG_H
#define __IWDG_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//���Ź� ��������		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/5/30
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  
#define DOG_FEED PCout(1)//ι�����ţ���ת��ƽ�źż���ι����
#define DOG_EN PCout(2)	//���Ź�ʹ��оƬ���͵�ƽ���Ź��������ߵ�ƽ���Ź��������� 	

void IWDG_Init(u8 prer,u16 rlr);
void IWDG_Feed(void);

void Watchdog_Init(void); 
void Watchdog_FEED(void);//ι��

#endif
