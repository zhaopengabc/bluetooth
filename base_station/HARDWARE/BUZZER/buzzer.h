#ifndef __BUZZER_H
#define __BUZZER_H
#include "sys.h"
#define Buzzer_ON     GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define Buzzer_OFF    GPIO_ResetBits(GPIOB,GPIO_Pin_6)
extern unsigned char Buzzer_flag;//��������������
void Buzzer_Init(void);//Buzzer��ʼ��
void Buzzer_SCAN(void);//������
#endif
