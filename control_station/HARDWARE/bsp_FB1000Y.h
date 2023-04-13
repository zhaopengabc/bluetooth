#ifndef __BSP_FB1000Y_H
#define __BSP_FB1000Y_H

#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "buzzer.h"
#include "voice.h"
#include "rtc.h"
#include "w25q16.h"
#include "net.h"
#include "mainpower.h"
#include "ProtocolBoard.h"
#include "SendToServer.h"
#include "RecieveFromServer.h"
#include "ServerAction.h"
#include "SendToServerDataType.h"
#include "lcd.h"
#include "ProtocolBoard.h"
#include "wdg.h"
#include "stm32f10x.h"

void bsp_init(void);
extern void macLED1_TOGGLE1(void);
void SystemPeriod_Reset(void);
#endif
