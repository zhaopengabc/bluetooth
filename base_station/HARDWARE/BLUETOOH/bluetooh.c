#include "bluetooh.h" 
#include "delay.h"
#include "iwdg.h"
//初始化PA12为输出口		    
//Bluetooh IO初始化
void Bluetooh_Init(void)//Bluetooh初始化
{ 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PB端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //BUZZER-->PB.6 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.6
 GPIO_SetBits(GPIOA,GPIO_Pin_12);						 //PA.12 输出高
}
void Bluetooh_Reset(void)//Bluetooh复位。
{ 
	BLUETOOH_RST=0;
    Watchdog_FEED();
	delay_ms(500);
    Watchdog_FEED();
	delay_ms(500);
    Watchdog_FEED();	
	delay_ms(500);
    Watchdog_FEED();
	delay_ms(500);
    Watchdog_FEED();    
	BLUETOOH_RST=1;	
}














