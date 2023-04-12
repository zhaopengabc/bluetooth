#include "led.h"
unsigned char LED_flag=0;//LED��˸��־λ��
unsigned int time_led=0;
//��ʼ��PB1��PB9Ϊ�����.��ʹ���������ڵ�ʱ��		    
//433��LoRaģ��ʹ�����ų�ʼ��
void ENABLE_IO_Init(void)//ʹ�����ų�ʼ��	
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PA�˿�ʱ��

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	//�˿�����, �������
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz	
 GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 GPIO_SetBits(GPIOA,GPIO_Pin_11); 						 //PA.11 ����� 	
}

//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure; 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PC�˿�ʱ��
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	//LED0-->PB.5 �˿�����, �������
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz	
 GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 GPIO_SetBits(GPIOB,GPIO_Pin_5); 						 //PB.5 ����� 	
}
void LED_SCAN()//LED
{

//			if(GPRS_WORK==1)//������ӷ������ɹ���
//			{
//				LED3=0;//������������ɹ�LED��
//			}
//			else
//			{
//				LED3=1;
//			}			
//			if(GPRS_LINK==1)//������ӷ������ɹ���
//			{
//				LED2=0;//������������ɹ�LED��
//			}
//			else
//			{
//				LED2=1;
//			}

//	static unsigned int time_led=0;
  switch(LED_flag)//����LED����״̬��
	{
	 case 0://���				 
			LED0=1;
			break;	 
	 case 1://����
	 		LED0=0;								
			break;	 		 		
	 case 2://500ms����500ms��
			time_led++;
			if(time_led>100)
			{
				LED0=~LED0;
				time_led=0;
			}	 											
			break;
	 case 3://200ms����200ms��
			time_led++;
			if(time_led>40)
			{
				LED0=~LED0;
				time_led=0;
			}	 	 											
			break;
	 case 4://��תLED״̬��
			LED0=~LED0;							
			break;			
	 default:
			LED0=0;	
	 		break;
	}
}












