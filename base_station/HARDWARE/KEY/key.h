#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

//#define KEY0  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)//��ȡ����0
#define KEY0  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)//��ȡ����0
//#define GPRS_LINK  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)//��������ɹ���Ϊ�ߣ�����Ϊ�͡�
//#define GPRS_WORK  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)//GM3����״ָ̬ʾ�ƣ���ƽһ��һ�α仯��
extern unsigned char KEY_flag;//�� KEY_flag=1��ʱ�򣬱�ʾ�̰��� KEY_flag=2��ʱ�򣬱�ʾ������
extern unsigned char KEY0_DN_L;//����ȫ�ֱ�����������������
extern unsigned char KEY0_DN_S;//����ȫ�ֱ������������̰���
void KEY_Init(void);//������ʼ��
//void GM3_IO_Init(void);//GM3ģ��IO��ʼ��
void KEY_SCAN(void);//����ɨ��		
#endif
