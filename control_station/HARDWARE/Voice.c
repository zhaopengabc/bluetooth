/***************************************
//��������
//Function:������ʾԴ�ļ�
//Data:2017-02-16
//Author:luoxiuan

***************************************/

/********************************
//����ͷ�ļ�
********************************/
#include "stm32f10x.h"
#include "voice.h"
#include "includes.h"
/******************************************************************************
//��������
*******************************************************************************/
void Voice_Send_Data(u8 add);    //����������ַ����������

/******************************************************************************
//��������
*******************************************************************************/
OS_TMR Voicetmr;
OS_TMR VoiceSet1Stmr;
OS_SEM Voice_Sem;
//static unsigned int voiceflag=1;
//�ݴ����ڵ���������Ϊ0x55��Ϊδʹ��
static unsigned char VoiceType=VOICE_MUTE;
static unsigned char Set(unsigned char type);
/*************************
//��������
*************************/
static void delay_us(unsigned int us)
{
	unsigned int j,i;
	for(i=0;i<us;i++)
		for(j=0;j<9;j++);
}
/**********************************
//����:	Key_GPIO_Init(void)
//����:	����GPIO�ڳ�ʼ������
//��ڲ���:	��
//���ڲ���:	��
**********************************/
void Voice_GPIO_Init(void)    //����GPIO�ڳ�ʼ������
{
	OS_ERR err;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(NVC_BUSY_RCC|NVC_CLK_RCC|NVC_SDA_RCC, ENABLE);
	RCC_APB2PeriphClockCmd(NVC_ShutDown_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = NVC_BUSY_Pin;              //æ�ź�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //�������� 
    GPIO_Init(NVC_BUSY_GPIO,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = NVC_CLK_Pin;              //ʱ���ź�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(NVC_CLK_GPIO,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = NVC_SDA_Pin;              //�����ź�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(NVC_SDA_GPIO,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = NVC_ShutDown_Pin;          //�����ź�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(NVC_ShutDown_GPIO,&GPIO_InitStructure);
	
	OSSemCreate((OS_SEM *)&Voice_Sem,
				(CPU_CHAR*	)"Voice_Sem",
				(OS_SEM_CTR )1,
				(OS_ERR*	)&err);
//��ʼ״̬
//	NVC_BUSY_Low();
	NVC_CLK_High();
	NVC_SDA_High();
//	NVC_ShutDown_High();
	NVC_ShutDown_Low();   //����Ƶ����
	Set(VOICE_MUTE);
}
void Voice_Init(void)
{
	Voice_GPIO_Init();
//	Voice_Send_Data(0xe0);    //0xe0������С,0xe7�������
//	VoiceAction(VOICE_SELFCHECK,5);
}
/**********************************
//����:	Voice_Send_Data(u8 add)
//����:	����оƬ���͵�ַ
//��ڲ���:	Ҫ���͵ĵ�ַ
//���ڲ���:	��
**********************************/
void Voice_Send_Data(u8 add)    //����������ַ����������
{
	OS_ERR err;
	u8 j=0;
//	NVC_ShutDown_Low();   //����Ƶ����
	delay_us(10);     			 //��ʱ10us
	NVC_CLK_Low();   
	OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_HMSM_STRICT,&err);     			//��ʱ4-6ms
	for(j=0;j<8;j++)
	{
		if(add&0x01)
		{
			NVC_SDA_High();
		}
		else
		{
			NVC_SDA_Low();
		}
		NVC_CLK_Low();
		delay_us(300);
		NVC_CLK_High();			//�����أ�����оƬ��������
		delay_us(300);
		add>>=1;
	}
	delay_us(10);     			 
	NVC_CLK_High();				//�����߸�λ
	NVC_SDA_High();
//	NVC_ShutDown_High();		//�ر���Ƶ����
}
unsigned char ReadVoiceType(void)
{
	return VoiceType;
}
void VoiceSet1Stmr_callback(void *p_tmr,void *p_arg)
{
	OS_ERR err;
	OSSemPost((OS_SEM*	)&Voice_Sem,
				(OS_OPT 	)OS_OPT_POST_1,
				(OS_ERR*	)&err);
}
/****************************
��ڲ�����type-�������ࣺ�������������������ϡ������ϱ�������ʾ����ڡ��Լ칲6��
		sec-ά��ʱ�䵥λΪs��
ע�⣺type���жϣ���ΪVOICE_MUTE��ֱ��ֹͣ��������������ֱֵ�����㣻
�˺������԰�VOICE_DUTY��ʾ��ֹͣ��Set(VOICE_MUTE);
****************************/
static unsigned char Set(unsigned char type)
{
	CPU_SR_ALLOC();
	
	OS_CRITICAL_ENTER();
	if(type!=VOICE_MUTE)
	{
		Voice_Send_Data(type);
		Voice_Send_Data(0xf2);
		VoiceType=type;
	}
	else
	{
		Voice_Send_Data(VOICE_MUTE);
		VoiceType=VOICE_MUTE;
	}
	OS_CRITICAL_EXIT();
	return 1;
}
void Voicetmr_callback(void *p_tmr,void *p_arg)
{
	OS_ERR err;
	OS_STATE state;
	
	Set(VOICE_MUTE);
	
	state=OSTmrStateGet((OS_TMR		*)&Voicetmr,(OS_ERR*	)&err);
	if(state!=OS_TMR_STATE_UNUSED)
	{
		OSTmrDel((OS_TMR		*)&Voicetmr,(OS_ERR*	)&err);
	}
}
//��ڲ������������࣬����ʱ��,�����԰�VOICE_DUTY����
unsigned char VoiceAction(unsigned char type,unsigned int sec)
{
	OS_ERR err;
	OS_STATE state;
	//��Ϊ��Ӧ��ʾ����VOICE_HALT����VOICE_MUTE�Ҳ���VOICE_DUTY��ֹͣ��ʾ��
	//��ֹ�������������ϣ�
	
	OSSemPend(	(OS_SEM*	)&Voice_Sem,
				(OS_TICK	) 0,
				(OS_OPT		) OS_OPT_PEND_BLOCKING,
				(CPU_TS    *) 0,
				(OS_ERR*	)&err);
	
	if(((VoiceType!=VOICE_DUTY)&&(type==VOICE_MUTE))||((sec==VOICE_HALT)&&(VoiceType==type)))
	{
		Set(VOICE_MUTE);
	}else
	{
		//���������������������������ھ��������������
		if(type==VOICE_ALARM)
		{
			Set(VOICE_ALARM);
		}
		//��VoiceSet(VOICE_DUTY,VOICE_HALT);��Ȼ����ֹͣVOICE_DUTY��
		else if((type==VOICE_DUTY)&&(VoiceType!=VOICE_ALARM))
		{
			Set(VOICE_DUTY);
		}
		else if((type==VOICE_HOSTFAULT)&&(VoiceType!=VOICE_ALARM)&&(VoiceType!=VOICE_DUTY))
		{
			Set(VOICE_HOSTFAULT);
		}
		else if((type==VOICE_LOCALFAULT)&&((VoiceType==VOICE_MUTE)||(VoiceType==VOICE_SELFCHECK)))
		{
			Set(VOICE_LOCALFAULT);
		}
		else if((type==VOICE_SELFCHECK)&&((VoiceType==VOICE_MUTE)))
		{
			Set(VOICE_SELFCHECK);
		}
		
		state=OSTmrStateGet((OS_TMR		*)&Voicetmr,(OS_ERR*	)&err);
		if(state!=OS_TMR_STATE_UNUSED)
		{
			OSTmrDel((OS_TMR		*)&Voicetmr,(OS_ERR*	)&err);
		}
		OSTmrCreate((OS_TMR		*)&Voicetmr,
				(CPU_CHAR	*)"Voicetmr",
				(OS_TICK	)sec*200,
				(OS_TICK	)0,
				(OS_OPT		)OS_OPT_TMR_ONE_SHOT,
				(OS_TMR_CALLBACK_PTR)Voicetmr_callback,
				(void*	)0,
				(OS_ERR*	)&err);
		OSTmrStart((OS_TMR		*)&Voicetmr,(OS_ERR*	)&err);
	}
	state=OSTmrStateGet((OS_TMR		*)&VoiceSet1Stmr,(OS_ERR*	)&err);
	if(state!=OS_TMR_STATE_UNUSED)
	{
		OSTmrDel((OS_TMR		*)&VoiceSet1Stmr,(OS_ERR*	)&err);
	}
	OSTmrCreate((OS_TMR		*)&VoiceSet1Stmr,
			(CPU_CHAR	*)"VoiceSet1Stmr",
			(OS_TICK	)50,			//ASL........20180324�ֱ����º���Ű���������Ӧ�������ʼ�С��200���ʱ��Ϊ50
			(OS_TICK	)0,
			(OS_OPT		)OS_OPT_TMR_ONE_SHOT,
			(OS_TMR_CALLBACK_PTR)VoiceSet1Stmr_callback,
			(void*	)0,
			(OS_ERR*	)&err);
	OSTmrStart((OS_TMR		*)&VoiceSet1Stmr,(OS_ERR*	)&err);
			
	return VoiceType;
}
void Voice_Test(void)
{
//	OS_ERR err;
	while(1)
	{
		VoiceAction(VOICE_ALARM,5);
//		OSTimeDlyHMSM(0,0,10,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ10s
		VoiceAction(VOICE_LOCALFAULT,5);
//		OSTimeDlyHMSM(0,0,10,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ10s
		VoiceAction(VOICE_HOSTFAULT,5);
//		OSTimeDlyHMSM(0,0,10,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ10s
		VoiceAction(VOICE_DUTY,5);
//		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
		VoiceAction(VOICE_SELFCHECK,5);
//		OSTimeDlyHMSM(0,0,10,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ10s
		VoiceAction(VOICE_SELFCHECK,5);
//		OSTimeDlyHMSM(0,0,10,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ10s
		VoiceAction(VOICE_SELFCHECK,5);
	}
}
/*********************************** END*****************************************/








