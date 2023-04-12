#ifndef __BUZZER_H
#define __BUZZER_H
#include "sys.h"
#define Buzzer_ON     GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define Buzzer_OFF    GPIO_ResetBits(GPIOB,GPIO_Pin_6)
extern unsigned char Buzzer_flag;//蜂鸣器鸣叫类型
void Buzzer_Init(void);//Buzzer初始化
void Buzzer_SCAN(void);//蜂鸣器
#endif
