/***************************************
//北京富邦
//Function:语音提示源文件
//Data:2017-02-16
//Author:luoxiuan

***************************************/

/********************************
//包含头文件
********************************/
#include "stm32f10x.h"
#include "voice.h"
#include "includes.h"
/******************************************************************************
//函数声明
*******************************************************************************/
void Voice_Send_Data(u8 add);    //发送语音地址，播放语音

/******************************************************************************
//变量定义
*******************************************************************************/
OS_TMR Voicetmr;
OS_TMR VoiceSet1Stmr;
OS_SEM Voice_Sem;
//static unsigned int voiceflag=1;
//暂存现在的声音，若为0x55则为未使用
static unsigned char VoiceType=VOICE_MUTE;
static unsigned char Set(unsigned char type);
/*************************
//函数定义
*************************/
static void delay_us(unsigned int us)
{
	unsigned int j,i;
	for(i=0;i<us;i++)
		for(j=0;j<9;j++);
}
/**********************************
//名称:	Key_GPIO_Init(void)
//功能:	按键GPIO口初始化配置
//入口参数:	无
//出口参数:	无
**********************************/
void Voice_GPIO_Init(void)    //语音GPIO口初始化配置
{
	OS_ERR err;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(NVC_BUSY_RCC|NVC_CLK_RCC|NVC_SDA_RCC, ENABLE);
	RCC_APB2PeriphClockCmd(NVC_ShutDown_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = NVC_BUSY_Pin;              //忙信号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //浮空输入 
    GPIO_Init(NVC_BUSY_GPIO,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = NVC_CLK_Pin;              //时钟信号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(NVC_CLK_GPIO,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = NVC_SDA_Pin;              //数据信号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(NVC_SDA_GPIO,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = NVC_ShutDown_Pin;          //数据信号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(NVC_ShutDown_GPIO,&GPIO_InitStructure);
	
	OSSemCreate((OS_SEM *)&Voice_Sem,
				(CPU_CHAR*	)"Voice_Sem",
				(OS_SEM_CTR )1,
				(OS_ERR*	)&err);
//初始状态
//	NVC_BUSY_Low();
	NVC_CLK_High();
	NVC_SDA_High();
//	NVC_ShutDown_High();
	NVC_ShutDown_Low();   //打开音频功放
	Set(VOICE_MUTE);
}
void Voice_Init(void)
{
	Voice_GPIO_Init();
//	Voice_Send_Data(0xe0);    //0xe0音量最小,0xe7音量最大
//	VoiceAction(VOICE_SELFCHECK,5);
}
/**********************************
//名称:	Voice_Send_Data(u8 add)
//功能:	语音芯片发送地址
//入口参数:	要发送的地址
//出口参数:	无
**********************************/
void Voice_Send_Data(u8 add)    //发送语音地址，播放语音
{
	OS_ERR err;
	u8 j=0;
//	NVC_ShutDown_Low();   //打开音频功放
	delay_us(10);     			 //延时10us
	NVC_CLK_Low();   
	OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_HMSM_STRICT,&err);     			//延时4-6ms
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
		NVC_CLK_High();			//上升沿，语音芯片接收数据
		delay_us(300);
		add>>=1;
	}
	delay_us(10);     			 
	NVC_CLK_High();				//数据线复位
	NVC_SDA_High();
//	NVC_ShutDown_High();		//关闭音频功放
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
入口参数：type-声音种类：静音、报警、本机故障、主机上报故障提示、查岗、自检共6种
		sec-维持时间单位为s；
注意：type先判断，若为VOICE_MUTE，直接停止声音，计数不赋值直接清零；
此函数可以把VOICE_DUTY提示声停止；Set(VOICE_MUTE);
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
//入口参数：声音种类，持续时间,不可以把VOICE_DUTY静音
unsigned char VoiceAction(unsigned char type,unsigned int sec)
{
	OS_ERR err;
	OS_STATE state;
	//若为相应提示声的VOICE_HALT或者VOICE_MUTE且不是VOICE_DUTY则停止提示声
	//防止静音将查岗声打断；
	
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
		//警报声优先于其他声音，若正在警报不被其他打断
		if(type==VOICE_ALARM)
		{
			Set(VOICE_ALARM);
		}
		//若VoiceSet(VOICE_DUTY,VOICE_HALT);依然可以停止VOICE_DUTY声
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
			(OS_TICK	)50,			//ASL........20180324手报按下后接着按下消音反应较慢，故减小了200这个时间为50
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
//		OSTimeDlyHMSM(0,0,10,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时10s
		VoiceAction(VOICE_LOCALFAULT,5);
//		OSTimeDlyHMSM(0,0,10,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时10s
		VoiceAction(VOICE_HOSTFAULT,5);
//		OSTimeDlyHMSM(0,0,10,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时10s
		VoiceAction(VOICE_DUTY,5);
//		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
		VoiceAction(VOICE_SELFCHECK,5);
//		OSTimeDlyHMSM(0,0,10,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时10s
		VoiceAction(VOICE_SELFCHECK,5);
//		OSTimeDlyHMSM(0,0,10,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时10s
		VoiceAction(VOICE_SELFCHECK,5);
	}
}
/*********************************** END*****************************************/








