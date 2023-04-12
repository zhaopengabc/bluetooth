#ifndef __BLUETOOH_H
#define __BLUETOOH_H
#include "sys.h"
#define BLUETOOH_RST PAout(12)// PA12
//#define BLUETOOH_RST     GPIO_SetBits(GPIOB,GPIO_Pin_6)
//#define Buzzer_OFF    GPIO_ResetBits(GPIOB,GPIO_Pin_6)
void Bluetooh_Init(void);//Bluetooh≥ı ºªØ
void Bluetooh_Reset(void);//Bluetooh∏¥Œª°£
#endif
