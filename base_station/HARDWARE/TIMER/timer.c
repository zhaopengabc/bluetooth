#include "timer.h"
#include "led.h"
#include "usart.h"
#include "usart2.h"
#include "usart3.h"
#include "uart4.h"
#include "sys.h"
#include "buzzer.h"
#include "key.h"
#include "stmflash.h"
#include "gprs.h"
#include "os.h"
#include "stmflash.h"
//��ʱ��2ͨ��1���벶������
//arr,psc.
//Tout= ((arr+1)*(psc+1))/Tclk��	 
//Tclk�� TIM3 ������ʱ��Ƶ�ʣ���λΪ Mhz����
//Tout�� TIM3 ���ʱ�䣨��λΪ s����
//���ӣ�	 TIM3_Int_Init(4999,7199);
//��Tout=����4900+1��*��7199+1����/72000000=0.5S
//������һ�ζ�ʱ���жϵ�ʱ��Ϊ0.5�룬��500ms��	 	 
//	TIM3_Int_Init(130,71);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms  
//TIM3_Int_Init(99,71);//100΢�����һ�Ρ�10Khz��	 
//TIM3_Int_Init(99,719);//1000΢�루1ms������һ�Ρ�1Khz��
//TIM3_Int_Init(499,719);//5000΢�루5ms������һ�Ρ�500hz��	 
//TIM3_Int_Init(999,719);//10000΢��(10ms����һ�Ρ�100hz��
unsigned char TIM2_IRQ_flag=0;
unsigned int Send_Overtime=0;//���ػ��߷������������ݳ�ʱʱ����
unsigned int time_step=0;
unsigned int Bluetooh_Package_compare=0;//�������ݰ����бȽϵ���ʱ�ݶ�2��Ƚ�һ�Ρ�
unsigned int Bluetooh_scan_time=0;//������վ��ʱɨ��ʱ�䣬��0��ʱ�����ɨ�裬ɨ��������¸�ֵ��
unsigned int Bluetooh_heartbeat_time=0;//������վ�ϴ�����վ����������5�����Ϸ�һ�Ρ�
unsigned int Bluetooh_time[Bluetooh_Max_NO];//���������100��������ǩ.���е���ʱ��ʱ���������鶼���е���ʱ��
unsigned int send_time=0;//���в�ѯ�Ƿ���Ҫ����������������
unsigned int KEY0_DN_S_effective_time=0;//�̰�������Чʱ�䡣
unsigned int KEY0_DN_L_effective_time=0;//����������Чʱ�䡣
TIM_ICInitTypeDef  TIM2_ICInitStructure;
void TIM2_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//ʹ��TIM2ʱ��
	//��ʼ����ʱ��2 TIM2	 
	TIM_TimeBaseStructure.TIM_Period = arr; //�趨�������Զ���װֵ 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//Ԥ��Ƶ��   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM2, //TIM2
		TIM_IT_Update ,
		ENABLE  //ʹ��
		);
	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 
  TIM_Cmd(TIM2,ENABLE ); 	//ʹ�ܶ�ʱ��2
}
//��ʱ��5�жϷ������
// 5ms�ж�����
void TIM2_IRQHandler(void)
{
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
//	unsigned char i;	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //����жϱ�־λ

			if(time_step==0)//1�����һ�Ρ�
			{
				TIM2_IRQ_flag=1;//1����1һ�Ρ�				
				time_step=200;
				timer_flag_int();//ϵͳ�ڱ�־λ�����Լ�������
			}
			else
			{
				time_step--;
			}
			if(Send_Overtime>0)//��������ʱ������ʱ�á�
			{
				Send_Overtime--;
			}
			USART1_RX_SCAN();//��ʱ����ʱɨ�贮�ڱ�־λ�ı仯����ʱ�Ľ��д���
			USART2_RX_SCAN();//��ʱ����ʱɨ�贮�ڱ�־λ�ı仯����ʱ�Ľ��д���
			USART3_RX_SCAN();//��ʱ����ʱɨ�贮�ڱ�־λ�ı仯����ʱ�Ľ��д���
			UART4_RX_SCAN(); //��ʱ����ʱɨ�贮�ڱ�־λ�ı仯����ʱ�Ľ��д���
			//Buzzer_SCAN();	//������ɨ��
			LED_SCAN();			//LED
			KEY_SCAN();			//����ɨ��
//			if( (KEY0_DN_S_effective_time==0)&&(KEY0_DN_S==1) )//���30����û�н������ã�����ն̰�����־λ��
//			{
//				KEY0_DN_S=0;//������־λ����ա�	
//			}
//			else
//			{
//				KEY0_DN_S_effective_time--;
//			}
//			if( (KEY0_DN_L_effective_time==0)&&(KEY0_DN_L==1) )//���5����û�н������ã�����ճ�������־λ��
//			{
//				KEY0_DN_L=0;//������־λ����ա�
//			}
//			else
//			{
//				KEY0_DN_L_effective_time--;
//			}				
//			if(READ_GPRS_POWER_STATUS==0)
//			{
//				LED0=0;
//			}
//			else
//			{
//				LED0=1;				
//			}
		}
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
}

void timer_flag_int(void)//ϵͳ�ڱ�־λ�����Լ�������
{
	unsigned char i=0;
	if(send_time>0)//���в�ѯ�Ƿ���Ҫ����������������
	{
		send_time=send_time-1;
	}			
	if(Bluetooh_heartbeat_time>0)//������վ�ϴ�����վ����������
	{
		Bluetooh_heartbeat_time=Bluetooh_heartbeat_time-1;
	}
	if(Bluetooh_scan_time>0)//����������������������ǩ�豸����ʱ��10��һ�Ρ�
	{
		Bluetooh_scan_time=Bluetooh_scan_time-1;
	}
	if(Bluetooh_Package_compare>0)//���˼��������ǩ��λ״̬��ʱ���ˡ�5����һ�Ρ�
	{
		Bluetooh_Package_compare=Bluetooh_Package_compare-1;
	}
	if(SEND_DATA_to_server_time>0)//���������������ݵ���ʱ������������ʱ��û�з��ͳɹ��������·��͡�
	{
		SEND_DATA_to_server_time=SEND_DATA_to_server_time-1;
	}
	for(i=0;i<Bluetooh_amount;i++)//�ж��ٸ���ǩ�ͽ��м��Ρ�
	{
		if(Bluetooh_time[i]>0)//ÿ��������ǩ����������ʱ�������ʱû�б����֣�����Ϊ������ǩ��λ�ˡ�Ȼ��ͨ��GM3����433����LoRa���ͳ�ȥ����վ���߷�������
		{
			Bluetooh_time[i]=Bluetooh_time[i]-1;
		}		
	}			
}
