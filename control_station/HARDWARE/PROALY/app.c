#include "app.h"
#include <includes.h>
#include <stdio.h> 
#include <string.h>
#include "ChoiceDevice.h"
#include "bsp_FB1000Y.h"
#include "MsgCANProcessAppExt.h"
#include "UsartMsgProcessAppExt.h"
#include "rtc.h"
#include "frame.h"
#include "ReceiveDataBuffer.h"
#include "DeviceCommon.h"

typedef	void (*FUN)(void);//��������������  
PluginInterface Plugin;//Э����������ʼ����
unsigned char PushButtonStatus=0;
volatile unsigned char SystemTime[6]={0x0C,0x0B,0x0A,0x09,0x08,0x12}; //ϵͳʱ�䣻
volatile HRINT16U MCLineLINK=0;//������·״̬������־��
//������·��⣺
unsigned char UpFaultNum=3;//������·���ϱ�־��=0������=1���ϣ�=3�޴˹��ܣ�
//�����ʱ�����壺
volatile unsigned int Sys_Second_100mS=0;
unsigned char MIM_flag=1;
unsigned char PZM_flag=0;
unsigned int  MIM_flag_Count=0;
unsigned int MClineFlaut_flag=0xFFFF0000;

/*�����ʱ���ص�����*/
void TmrCallback(void)//100m���Ӷ�ʱ��	
{
	NetHeart_SndCot++;
	Sys_Second_100mS++;	//100mS��־���������������ã���
//	Sys_WD_100mS++;	//100mS��־���������������ã���	
	/////���������ж�///////	
#ifdef MCLineJudge_Enable //������·���ꣻ
     MCLineLINK++;
	if(MCLineLINK>600)//60S�жϣ�
//		if(MCLineLINK>200)//20S�жϣ�
//	if(MCLineLINK>300)//30S�жϣ�
	{ 
		 MCLineLINK=601;
		 UpFaultNum=1;  //��·����
	}
	else
	{  UpFaultNum=0; }//��·������
#endif 	
	///////��������Ч����ʱ1h///////
	if(MIM_flag==1)
	{
		MIM_flag_Count++;
		if(MIM_flag_Count>=36000)
		{ MIM_flag=0;
		  PZM_flag=0;}
	}
	///////��������/////
//  SystemPeriod_Reset();
}
extern void tmrInspect_callback(void);    //Ѳ�춨ʱ���ص�����
void TIM2_IRQHandler(void)   //TIM3�ж�
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
			  TmrCallback();
				tmrInspect_callback();//CAN���ݽ��ճ�ʱ�жϣ�
				SystemPeriod_Reset();//ϵͳ����������
		}
}

void TIM2_Int_Init(unsigned short int arr, unsigned short int psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�
	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
	TIM_Cmd(TIM2, ENABLE); //ʹ��TIMx
}

//����uart2�������ݼ�ʱ����
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
static void TIM3_Int_Init(unsigned short int arr, unsigned short int psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�
	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
	TIM_Cmd(TIM3, ENABLE); //ʹ��TIMx
}

unsigned char RXframe[SERVER_DATA_NUM];
void gMessageSent(HRINT8U *Message,HRINT16U iSIZE)
{
	if(iSIZE>0)
	{
		ReportToServer(0x02,Message,iSIZE,RXframe);
	}
}

#ifdef XYB_CXY

void ProalyInit(void)
{
	
//	 	TIM2_Int_Init(1000,7200);	//��������100mS��		
	 	TIM3_Int_Init(100,7200);	//���ڽ���uart��ʱ����ʼ��,����10mS��
	
	//��ʼ��Э�鶨�壻
    Plugin=DevicePlugin_Install();
	  ((void(*)(FUN)) Plugin.cPLUGIN_Install)(0);//Э�������ʼ������ȡ�ӿڲ�����								
//��������CAN2;	
#ifdef CAN_Enable                                   
//     CAN1Init_Parameter(gCANSystemParameter);//CAN��ʼ����
	   CAN1_Mode_Init(CAN1,gCANSystemParameter);   //CANģʽ��ʼ��ʹ��
#endif	
								
//��������RS232������2;
#if defined RS232_Enable	 
	   USARTx_Mode_Init(USART2,gUARTSystemParameter);//����2��ʼ����
#endif	
								
//��������RS485������3;								
#if defined RS485_Enable  
     GPIO_RS485_Init();//RS485;
	   USARTx_Mode_Init(USART3,gUARTSystemParameter);//����2��ʼ����								
#endif
	
}
void ProalyRun(void)
{
	 ((void(*)(FUN)) Plugin.cPLUGIN_Run)(0);//Э���������

#ifdef MCLineJudge_Enable 	
				//��Ϣ��4
	 if(UpFaultNum==0x01)//������·���ϣ�
		{ MClineFlaut_flag=1;}  
	 else if (UpFaultNum==0x00)//������·������
		{ MClineFlaut_flag=0;} 
#endif		
}


/*CAN2�жϺ���*/ //����CAN�ӿ�
#ifdef CAN_Enable
void CAN1_RX0_IRQHandler(void)//USB_LP_CAN1_RX0_IRQHandler CAN2_RX1_IRQHandler
{ 
#ifdef CANBUFFER_Enable
   YH_CAN_RX0_IRQHandler();
#else		
	 ((void(*)(FUN)) Plugin.cPLUGIN_Interrupt)(0);
#endif	
	
#ifdef MCLineJudge_Enable 	
		  MCLineLINK=0; 
#endif	
}
#endif

/*UART2�жϺ���*/  //����RS232�ӿ�\����RS485�ӿ�\��ӡ������
#if defined  RS232_Enable
void USART2_IRQHandler(void)
{
#ifdef UARTBUFFER_Enable	
	YH_UART_IRQHandler();//ϵͳ�Զ������ݣ�
#else	
 ((void(*)(FUN)) Plugin.cPLUGIN_Interrupt)(0);//�Զ���������ݣ�	
#endif	
	
#ifdef MCLineJudge_Enable 	
		  MCLineLINK=0; //������·���ϣ�
#endif		
}
#endif

/*UART2�жϺ���*/  //����RS232�ӿ�\����RS485�ӿ�\��ӡ������
#if defined RS485_Enable
void USART3_IRQHandler(void)
{
#ifdef UARTBUFFER_Enable	
	YH_UART3_IRQHandler();//ϵͳ�Զ������ݣ�
#else	
 ((void(*)(FUN)) Plugin.cPLUGIN_Interrupt)(0);//�Զ���������ݣ�	
#endif	
	
#ifdef MCLineJudge_Enable 	
		  MCLineLINK=0; //������·���ϣ�
#endif		
}
#endif

#endif


void Time_System_Get(void)
{
	 RTC_Get((char *)&SystemTime);
	 printf("ϵͳʱ��:%d-%d-%d %d:%d:%d\n", SystemTime[5],SystemTime[4],SystemTime[3],SystemTime[2],SystemTime[1],SystemTime[0]);
}