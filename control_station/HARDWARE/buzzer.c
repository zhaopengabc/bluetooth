#include "buzzer.h"
#include "includes.h"
//////////////////////////////////////////////////////////////////////////////////
//��ʼ��PF8Ϊ�����		    
//BEEP IO��ʼ��
OS_TMR tmr_buzzer;
//OS_MUTEX Buzzer_Mutex;
void Buzzer(unsigned char State)
{
	if(State==0) GPIO_ResetBits(GPIOC,GPIO_Pin_9);
	else GPIO_SetBits(GPIOC,GPIO_Pin_9);
}
void tmr_buzzer_callback(void *p_tmr,void *p_arg)
{
	OS_ERR err;
	Buzzer(0);
	OSTmrDel((OS_TMR		*)&tmr_buzzer,(OS_ERR*	)&err);
//	OSMutexPost((OS_MUTEX*)&Buzzer_Mutex,
//				(OS_OPT 	)OS_OPT_POST_NONE,
//				(OS_ERR*	)&err);
}
void Buzzer_Init(void)//Buzzer��ʼ��
{
//	OS_ERR err;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//ʹ��GPIOFʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 //LED0-->PA.8 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8
	
	GPIO_ResetBits(GPIOC,GPIO_Pin_9);						 //PA.8 ����� 
//	GPIO_SetBits(GPIOC,GPIO_Pin_9);						 //PA.8 ����� 	
//	OSMutexCreate((OS_MUTEX*)&Buzzer_Mutex,
//				(CPU_CHAR*	)"Buzzer_Mutex",
//				(OS_ERR*	)&err);
}
//n10msecΪ10ms��������
unsigned char Buzzer_Action(unsigned int n10msec)
{
	OS_ERR err;
	OS_STATE state;
	
//	OSMutexPend((OS_MUTEX*	)&Buzzer_Mutex,
//				(OS_TICK	) 0,
//				(OS_OPT		) OS_OPT_PEND_BLOCKING,
//				(CPU_TS    *) 0,
//				(OS_ERR*	)&err);
	state=OSTmrStateGet((OS_TMR		*)&tmr_buzzer,(OS_ERR*	)&err);
	if(state!=OS_TMR_STATE_UNUSED)
	{
		OSTmrDel((OS_TMR		*)&tmr_buzzer,(OS_ERR*	)&err);
	}
	OSTmrCreate((OS_TMR		*)&tmr_buzzer,
				(CPU_CHAR	*)"tmr_buzzer",
				(OS_TICK	)n10msec*2,
				(OS_TICK	)0,
				(OS_OPT		)OS_OPT_TMR_ONE_SHOT,
				(OS_TMR_CALLBACK_PTR)tmr_buzzer_callback,
				(void*	)0,
				(OS_ERR*	)&err);
	OSTmrStart(&tmr_buzzer,&err);
	Buzzer(1);
//	OSMutexPost((OS_MUTEX*)&Buzzer_Mutex,
//				(OS_OPT 	)OS_OPT_POST_NONE,
//				(OS_ERR*	)&err);
	return 1;
}

void Buzzer_Test(void)
{
	OS_ERR err;
	while(1)
	{
		Buzzer_Action(50);
		OSTimeDlyHMSM(0,0,5,0,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}







