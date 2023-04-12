#include "bluetooh.h" 
#include "delay.h"
#include "iwdg.h"
//��ʼ��PA12Ϊ�����		    
//Bluetooh IO��ʼ��
void Bluetooh_Init(void)//Bluetooh��ʼ��
{ 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PB�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //BUZZER-->PB.6 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.6
 GPIO_SetBits(GPIOA,GPIO_Pin_12);						 //PA.12 �����
}
void Bluetooh_Reset(void)//Bluetooh��λ��
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














