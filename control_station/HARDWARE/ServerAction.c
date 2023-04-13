#include "ServerAction.h"
#include "SendToServerDataType.h"
#include "string.h"
#include "led.h"
#include "w25q16.h"
#include "voice.h"
#include "buzzer.h"
#include "frame.h"
#include "includes.h"
#include "mainpower.h"
#include "net.h"
#include "lcd.h"
#include "app.h"
//#include "CAN1ProtocolParsing.h"

#define MINUTES_5 1800 //3600=12min  1200=6分钟  
#define SEC_10 1800    //6分钟 #define SEC_10 100 上发失败：闪烁应在5分钟以上，上报成功：常亮5分钟以上；

static unsigned char destdata[SERVER_DATA_NUM];
static unsigned int LastTx_Frame;
extern OS_TCB SendToServerTaskTCB;

//没有火警信息高字节为0，火警信息传输中为1，
//传输完成收到应答为2，此时警情消除有效，按下警情消除,清零标志位
static unsigned int HuoJingFlag=0;
//没有火警信息高字节为0，火警信息传输中为1，
//传输完成收到应答为2，此时警情消除有效，按下警情消除,清零标志位
static unsigned int GuZhangFlag=0;
static unsigned int JianGuanFlag=0;
static unsigned int PingBiFlag=0;
static unsigned int ManAlarmFlag=0;
static unsigned int DutyFlag=0;

extern void selfchecktimer_on(unsigned int nsec);

void Write_LastFrameCnt(unsigned int FrameCnt)
{
	unsigned int *pFrameCnt=&LastTx_Frame;
	*pFrameCnt=FrameCnt;
}
//对data中的第i个对象进行存储操作，在解析数据后循环调用
//若存储成功返回1，存储失败返回0,已经完成存储返回2；
unsigned int Server_SaveInfor(unsigned char *data,unsigned int i)
{
	unsigned int flag=0;
	unsigned char buf[0x20];
	memset(buf,0,0x20);
	buf[1]=0x1;
	switch(data[27])
	{
		case 1:
			buf[0]=data[27];
			memcpy(&buf[2],&data[10*i+29],4);
			memcpy(&buf[0x16],&data[10*i+33],6);
		break;
		case 2:
			buf[0]=data[27];
			memcpy(&buf[2],&data[46*i+29],9);
		  //编码表的存储；
		  if((data[46*i+38]==0xB8)&&(data[46*i+40]==0x01)&&(data[46*i+41]==0x04))//如果有编码表；
			{buf[11]=0x11;//标志位；
			 buf[5]=data[46*i+42];
			 buf[6]=data[46*i+43];
			 buf[7]=data[46*i+44];
			 buf[8]=data[46*i+45];				
			}	
			memcpy(&buf[0x16],&data[46*i+69],6);
//			printf("存=%d！\n\r",data[32]);
		break;
		case 4:
			buf[0]=data[27];
			memcpy(&buf[2],&data[10*i+29],4);
			memcpy(&buf[0x16],&data[10*i+33],6);
		break;
		case 21:
			buf[0]=data[27];
			buf[1]=data[28];
			memcpy(&buf[2],&data[29],1);
			memcpy(&buf[0x16],&data[30],6);
		break;
		case 24:
			buf[0]=data[27];
			memcpy(&buf[2],&data[8*i+29],2);
			memcpy(&buf[0x16],&data[8*i+31],6);
		break;
		default:break;
	}
	W25q16_Lock();
	if(buf[0]!=0)//ZHAONIG
	{
		if(buf[1]==0)
		{
		flag=2;
		}
		else
		{
		flag=Writeflash_Infor(buf,0x1C);
		}
	}
	W25q16_UnLock();
	return flag;
}

//查岗应答停止
void ChaGangYingDa_stop(void)
{
		DutyFlag=0;
		Leds_Action(ChaGangYingDa_LED,LED_OFF,0);
		if(ReadVoiceType()==VOICE_DUTY)VoiceAction(VOICE_DUTY,VOICE_HALT);
}
void LedVoice_Action(unsigned char *data,unsigned int i)
{
	unsigned int Txcnt=data[2]+data[3]*256;
	if((data[26]==5)&&(data[27]==21))//回复巡检帧打断查岗状态
	{
		DutyFlag=0;
		Leds_Action(ChaGangYingDa_LED,LED_OFF,0);
		if(ReadVoiceType()==VOICE_DUTY)VoiceAction(VOICE_DUTY,VOICE_HALT);
	}
	if(((data[27]==1)&&(data[31+i*10]&0x02))||((data[27]==2)&&(data[36+i*46]&0x02))	\
						||((data[27]==4)&&(data[31+i*10]&0x04)))//火警
	{
		selfcheck_stop();
		HuoJingFlag=Txcnt;
		HuoJingFlag|=0x01000000;
		Leds_Action(HuoJing_LED,LED_TOGGLE,SEC_10);//有火警，闪烁10秒钟；
		Leds_Action(JingQingXiaoChu_LED,LED_TOGGLE,SEC_10);
		VoiceAction(VOICE_ALARM,VOICE_FOREVER);
	}
	else if(((data[27]==1)&&((data[30+i*11]&0x04)||(data[32+i*10]&0x07)))\
				||((data[27]==2)&&((data[36+i*46]&0x04)||(data[37+i*46]&0x01))))//故障
	{
		GuZhangFlag=Txcnt;
		GuZhangFlag|=0x01000000;
		Leds_Action(GuZhang_LED,LED_TOGGLE,SEC_10);
//		Leds_Action(GuZhangXiaoChu_LED,LED_TOGGLE,SEC_10);
		VoiceAction(VOICE_HOSTFAULT,VOICE_FOREVER);
	}
	else if(((data[27]==1)&&(data[31+i*10]&0x08))||((data[27]==2)&&(data[36+i*46]&0x08)))//屏蔽
	{
		PingBiFlag=Txcnt;
		PingBiFlag|=0x01000000;
		Leds_Action(PingBi_LED,LED_TOGGLE,SEC_10);
	}
	else if(((data[27]==1)&&(data[31+i*10]&0x70))||((data[27]==2)&&(data[36+i*46]&0x70)))//动作，启动、监管、反馈
	{
		JianGuanFlag=Txcnt;
		JianGuanFlag|=0x01000000;
		Leds_Action(JianGuan_LED,LED_TOGGLE,SEC_10);
	}
	else if(data[27]==24)
	{
		switch(data[29])
		{
			case 0x01:		break;	//复位
			case 0x02:		//消音
				Leds_Action(XiaoYin_LED,LED_ON,SEC_10);
				VoiceAction(VOICE_MUTE,VOICE_HALT);
				if(DutyFlag!=0)
				{
					//判定是否有DutyFlag,若有则执行查岗声继续相应，静音后返回查岗动作
					Leds_Action(ChaGangYingDa_LED,LED_TOGGLE,MINUTES_5*2);
					if(ReadVoiceType()!=VOICE_DUTY)VoiceAction(VOICE_DUTY,MINUTES_5*2);
				}
			break;
			case 0x04:
				if(ManAlarmFlag&0x01000000)
				{
					selfcheck_stop();
					Leds_Action(ShouDongBaoJing_LED,LED_TOGGLE,MINUTES_5);
				}
				VoiceAction(VOICE_ALARM,VOICE_FOREVER);
			break;	//手报
			case 0x08:
				Leds_Action(JingQingXiaoChu_LED,LED_OFF,SEC_10);
				if(ReadVoiceType()==VOICE_ALARM)VoiceAction(VOICE_ALARM,VOICE_HALT);
				if(DutyFlag!=0)
				{
					//判定是否有DutyFlag,若有则执行查岗声继续相应，静音后返回查岗动作
					Leds_Action(ChaGangYingDa_LED,LED_TOGGLE,MINUTES_5*2);
					if(ReadVoiceType()!=VOICE_DUTY)VoiceAction(VOICE_DUTY,MINUTES_5*2);
				}
			 else
			  {

			  }
			break;	//警情消除
			case 0x10:
//				VoiceAction(VOICE_SELFCHECK,2);
			break;	//自检
			case 0x20:
				Leds_Action(ChaGangYingDa_LED,LED_OFF,0);
				if(ReadVoiceType()==VOICE_DUTY)VoiceAction(VOICE_DUTY,VOICE_HALT);
			break;	//查岗，上发查岗应答操作，打断查岗动作
			case 0x40:		break;	//测试
			case 0x80:		//故障消除
//				Leds_Action(GuZhangXiaoChu_LED,LED_OFF,SEC_10);
				if(ReadVoiceType()==VOICE_HOSTFAULT)VoiceAction(VOICE_HOSTFAULT,VOICE_HALT);
			break;	
			default:		break;
		}
	}
}
//对解析的信息进行命令执行
unsigned char Server_Action(Server_Data_Typedef *pServerData,unsigned char *pServer_Rd_Addr)
{
	static unsigned int ZNcnt=0;
	OS_ERR err;
//	unsigned char *pSendToServer_Rd_Addr;
	unsigned int flag=0;
	unsigned int i=0;
	unsigned char cnt=3;
	unsigned char save_flag=0;
	unsigned char Rd_Addr;
	
//	CPU_SR_ALLOC();
	Rd_Addr=*pServer_Rd_Addr;
	//zhaoning心跳包，确认帧
	if((pServerData[Rd_Addr].data[26]==03)||(pServerData[Rd_Addr].data[26]==06)||((pServerData[Rd_Addr].data[27]==21)&&(pServerData[Rd_Addr].data[28]==0)))
	{
		OSTaskSemPost ((OS_TCB	*)&SendToServerTaskTCB,
			(OS_OPT	)OS_OPT_POST_NO_SCHED,
			(OS_ERR *)&err);
	  return 0;
	}
	else
	{
	for(i=0;(i<pServerData[Rd_Addr].data[28])&&(i<21);i++)
	{
		cnt=3;
		do
		{ 
			flag=Server_SaveInfor(pServerData[Rd_Addr].data,i);
			cnt--;
		}
		while((flag==0)&&(cnt!=0));
		if(flag==1)
		{
			LedVoice_Action(pServerData[Rd_Addr].data,i);
			save_flag++;
		}
		//若已经存储则不再操作；
		else if(flag==2)
		{
			ZNcnt=2;
		}
		else
		{
		  ZNcnt=0;	
		}
	}
	if(save_flag>0)
	{
		OSTaskSemPost ((OS_TCB	*)&SendToServerTaskTCB,
			(OS_OPT	)OS_OPT_POST_NO_SCHED,
			(OS_ERR *)&err);
	}else
	{
//		Reset_RdSendToServerData(pServerData,pServer_Rd_Addr);
    //清零；
//		OS_CRITICAL_ENTER();	
		pServerData[Rd_Addr].data[2]=0;
		pServerData[Rd_Addr].data[3]=0;	
		pServerData[Rd_Addr].Lenth=0;
		pServerData[Rd_Addr].Flag=0;		
//		OS_CRITICAL_EXIT();	
	}
	return save_flag;
	}
}
void SendSuccessedAction(void)
{
//	RefreshRunLedState();
//	if((self_check_step>5)||(self_check_step<2))
//	{
//		Leds_Action(WangLuoTongXin_GLED,LED_TOGGLE,SEC_10);
//		Leds_Action(WangLuoTongXin_YLED,LED_OFF,0);
//	}
}
void SendFailedAction(void)
{
//	if((self_check_step>5)||(self_check_step<2))
//	{
//		Leds_Action(WangLuoTongXin_GLED,LED_OFF,0);
	
//		Leds_Action(WangLuoTongXin_YLED,LED_ON,MINUTES_5);
//		Leds_Action(WangLuoTongXin_YLED,LED_TOGGLE,0);
//	}
	
//	Leds_Action(WangLuoTongXin_YLED,LED_TOGGLE,0);
	
//	#ifdef SEND_FAILED_ON
//	VoiceAction(VOICE_SELFCHECK,2);
//	#endif
}
//判定是否与Frame号一致如果一致，返回1，并清除缓存停止发送
unsigned char IsEquel(unsigned int num)
{
	unsigned char *pRd_Addr=NULL;
	Server_Data_Typedef *pServer_Data=NULL;
    //判定存放在哪个缓冲区，若一帧数据包含火警及故障等，则存放在火警帧中，
    //应清除火警缓存区，
  unsigned char flag=0;

	if((ManAlarmFlag&0x01000000)&&((ManAlarmFlag&0x0000ffff)==num))
	{
		ManAlarmFlag=0x02000000;
		Leds_Action(ShouDongBaoJing_LED,LED_ON,MINUTES_5);
		flag=3;
	}
	if((HuoJingFlag&0x01000000)&&((HuoJingFlag&0x0000ffff)==num))
	{
		HuoJingFlag=0x02000000;
		Leds_Action(HuoJing_LED,LED_ON,MINUTES_5);//服务器确认后，常亮5分钟；
		Leds_Action(JingQingXiaoChu_LED,LED_ON,MINUTES_5);
		if(flag<2)flag=2;
	}
	if((GuZhangFlag&0x01000000)&&((GuZhangFlag&0x0000ffff)==num))
	{
		GuZhangFlag=0x02000000;
		Leds_Action(GuZhang_LED,LED_ON,MINUTES_5);
//		Leds_Action(GuZhangXiaoChu_LED,LED_ON,MINUTES_5);
	}
	if((JianGuanFlag&0x01000000)&&((JianGuanFlag&0x0000ffff)==num))
	{
		JianGuanFlag=0x02000000;
		Leds_Action(JianGuan_LED,LED_ON,MINUTES_5);
	}
	if((PingBiFlag&0x01000000)&&((PingBiFlag&0x0000ffff)==num))
	{
		PingBiFlag=0x02000000;
		Leds_Action(PingBi_LED,LED_ON,MINUTES_5);
	}
//	printf("num=%d LastTx_Frame=%d！\n\r",num,LastTx_Frame);
	if(num==LastTx_Frame)
	{
		 LastTx_Frame=LastTx_Frame+1;//By ZhaoNing 防止一个流水号重复进入；
//		if((self_check_step>5)||(self_check_step<2))
//		{
//			Leds_Action(WangLuoTongXin_GLED,LED_ON,MINUTES_5);
 	if(g_self_check_step==0)//自检情况下，指示灯不操作；
 	{
			Leds_Action(WangLuoTongXin_YLED,LED_OFF,0);		
			Leds_Action(WangLuoTongXin_GLED,LED_ON,0);	
	}		
//		}
		////////////////////
//		pServer_Data=pSendToServerData(&pRd_Addr);	
//		if((pRd_Addr!=NULL)&&(pServer_Data!=NULL))
//		{
//			Reset_RdSendToServerData(pServer_Data,pRd_Addr);
//			return 1;
//		}
		////////////////////对应序列号清零；
		  SendToServerData_Reset(num);
		  return 1;
	}
	return 0;
}
void LastTx_Frame_Increase(void)
{
  LastTx_Frame=LastTx_Frame+1;//By ZhaoNing 防止一个流水号重复进入；
}
OS_TMR ResteKeytmr;
static unsigned char ResetStep=1;
void ResteKeytmr_callback(void *p_tmr,void *p_arg)
{
	OS_ERR err;
	OS_STATE state;
	if(p_arg==&ResetStep)
	{
//		LCD_SET_OFF();
		state=OSTmrStateGet((OS_TMR		*)&ResteKeytmr,(OS_ERR*	)&err);
		if(state!=OS_TMR_STATE_UNUSED)
		{
			OSTmrDel((OS_TMR		*)&ResteKeytmr,(OS_ERR*	)&err);
		}
		OSTmrCreate((OS_TMR		*)&ResteKeytmr,
				(CPU_CHAR	*)"ResteKeytmr",
//				(OS_TICK	)400,
				(OS_TICK	)400,
				(OS_TICK	)0,
				(OS_OPT		)OS_OPT_TMR_ONE_SHOT,
				(OS_TMR_CALLBACK_PTR)ResteKeytmr_callback,
				(void*		)0,
				(OS_ERR*	)&err);
		OSTmrStart((OS_TMR		*)&ResteKeytmr,(OS_ERR*	)&err);
	}
	else
	{
//	清零缓存
	printf("系统复位！\n\r");	
	Clear_LCD_Num();
	NVIC_SystemReset();
	}
}
void Reste_Key(void)
{
	OS_ERR err;
	OS_STATE state;
	unsigned char buf[0x20];
	unsigned int len;
//20180423关闭屏幕；
//	LCD_SET_OFF();
//	back_light_change(10);//调节亮度，关闭背光。
//	上发操作状态
	len=Device_Operation_Information(buf,OPERATION_RESET,2);
	ReportToServer(0x02,buf,len,destdata);
//	back_light_change(10);//调节亮度，关闭背光。	
	state=OSTmrStateGet((OS_TMR		*)&ResteKeytmr,(OS_ERR*	)&err);
//	back_light_change(10);//调节亮度，关闭背光。	
	if(state!=OS_TMR_STATE_UNUSED)
	{
		OSTmrDel((OS_TMR		*)&ResteKeytmr,(OS_ERR*	)&err);
	}
	OSTmrCreate((OS_TMR		*)&ResteKeytmr,
			(CPU_CHAR	*)"ResteKeytmr",
			(OS_TICK	)100,
			(OS_TICK	)0,
			(OS_OPT		)OS_OPT_TMR_ONE_SHOT,
			(OS_TMR_CALLBACK_PTR)ResteKeytmr_callback,
			(void*		)&ResetStep,
			(OS_ERR*	)&err);
	OSTmrStart((OS_TMR		*)&ResteKeytmr,(OS_ERR*	)&err);
}
//上传测试/正常工作的切换
void SystemMod_Key(void)
{
	unsigned char buf[0x20];
	unsigned int len;
//	if((ReadVoiceType()!=VOICE_MUTE)&&(ReadVoiceType()!=VOICE_DUTY)) //读取声音作用不太清楚
//	{
		if(System_Mod==1)
		{
			len=Device_Operation_Information(buf,OPERATION_WORK,3);
		}
		else
		{
			len=Device_Operation_Information(buf,OPERATION_TEST,3);
	  }
		ReportToServer(0x02,buf,len,destdata);
//	}
}

void FaultMute_Key(void)
{
	unsigned char buf[0x20];
	unsigned int len;
	if((ReadVoiceType()!=VOICE_MUTE)&&(ReadVoiceType()!=VOICE_DUTY))
	{
		len=Device_Operation_Information(buf,OPERATION_SILENCE,2);
		ReportToServer(0x02,buf,len,destdata);
	}
}
//返回线路故障与网络故障状态标志
//返回0为正常，1为故障
//Bit1:线路故障 Bit0:网络故障标志
unsigned char SelfCheck_Key(void)
{
	unsigned char buf[0x20];
	unsigned int len;
	len=Device_Operation_Information(buf,OPERATION_SELFCHECK,2);
	ReportToServer(0x02,buf,len,destdata);
//	1s后执行自检任务
	selfchecktimer_on(10);
	return 0;
}
void JingQingXiaoChu_Key(void)
{
	unsigned char buf[0x20];
	unsigned char len;
	if(HuoJingFlag==0x02000000)
	{
		HuoJingFlag=0;
		len=Device_Operation_Information(buf,OPERATION_ALARMREMOVE,2);
		ReportToServer(0x02,buf,len,destdata);
	}
}
void GuZhangXiaoChu_Key(void)
{
	unsigned char buf[0x20];
	unsigned char len;
	if(GuZhangFlag==0x02000000)
	{
		GuZhangFlag=0;
		len=Device_Operation_Information(buf,OPERATION_FAULTREMOVE,2);
		ReportToServer(0x02,buf,len,destdata);
	}
}
void ManAlarm_Key(void)
{
	unsigned char buf[0x20];
	unsigned int len;
	unsigned int cnt;
	len=Device_Operation_Information(buf,OPERATION_MANALARM,2);
	cnt=ReportToServer(0x02,buf,len,destdata);
	ManAlarmFlag=cnt;
	ManAlarmFlag|=0x01<<24;

}
//按键函数中查岗键按下时调用
void Duty_Key(void)
{
	unsigned char buf[0x20];
	unsigned int len;
	if(DutyFlag&0xff000000)
	{
		DutyFlag=0;
		len=Device_Operation_Information(buf,OPERATION_RESPONSE,2);
		ReportToServer(0x02,buf,len,destdata);
	}
}
OS_TMR DutyVoicetmr;
void DutyVoicetmr_callback(void *p_tmr,void *p_arg)
{
	DutyFlag=0;//查岗状态时间到，查岗结束
	Leds_Action(ChaGangYingDa_LED,LED_OFF,0);
	if(ReadVoiceType()==VOICE_DUTY)VoiceAction(VOICE_DUTY,VOICE_HALT);
}
void IsOnDuty_Action(void)
{
	OS_ERR err;
	OS_STATE state;
	//打开声音，时间设置为10min，若data[29]大于10设置为data[29]
	//data[29]为查岗应答的时间单位为min
//	if((data[29]<=10)||(data[29]>20)) DutyCnt=10*60*100;
//	else DutyCnt=data[29]*60*100;
	Leds_Action(ChaGangYingDa_LED,LED_TOGGLE,MINUTES_5*2);
	VoiceAction(VOICE_DUTY,VOICE_FOREVER);
	DutyFlag|=0x01<<24;
	//定时器回调函数时间到之后，若没有按键则清除声光及标志位
	state=OSTmrStateGet((OS_TMR		*)&DutyVoicetmr,(OS_ERR*	)&err);
	if(state!=OS_TMR_STATE_UNUSED)
	{
		OSTmrDel((OS_TMR		*)&DutyVoicetmr,(OS_ERR*	)&err);
	}
	OSTmrCreate((OS_TMR		*)&DutyVoicetmr,
			(CPU_CHAR	*)"DutyVoicetmr",
			(OS_TICK	)200*700,
			(OS_TICK	)0,
			(OS_OPT		)OS_OPT_TMR_ONE_SHOT,
			(OS_TMR_CALLBACK_PTR)DutyVoicetmr_callback,
			(void*	)0,
			(OS_ERR*	)&err);
	OSTmrStart((OS_TMR		*)&DutyVoicetmr,(OS_ERR*	)&err);
}

void SelfCheckStep1(void)
{
//	CPU_SR_ALLOC();
//	OS_CRITICAL_ENTER();	//进入临界区
//	Leds_Action(LED_ALL&(~(ZhuJiTongXin_YLED|WangLuoTongXin_YLED|ZhuDianGuZhang_YLED|BeiDianGuZhang_YLED)),LED_ON,10);
//	Leds_Action(LED_ALL&(~(ZhuDianGuZhang_YLED|BeiDianGuZhang_YLED|WangLuoTongXin_YLED|ZhuJiTongXin_YLED|CeShi_LED)),LED_ON,10);
	  Leds_Action(LED_ALL&(~(ZhuDianGuZhang_GLED|BeiDianGuZhang_GLED|WangLuoTongXin_GLED|ZhuJiTongXin_GLED|GongZuo_LED)),LED_ON,20);
//		OS_CRITICAL_EXIT();	//进入临界区
}
void SelfCheckStep2(void)
{
//	CPU_SR_ALLOC();
//	OS_CRITICAL_ENTER();	//进入临界区
//	Leds_Action(ZhuJiTongXin_YLED|WangLuoTongXin_YLED|ZhuDianGuZhang_YLED|BeiDianGuZhang_YLED,LED_ON,10);
//	Leds_Action(ZhuDianGuZhang_YLED|BeiDianGuZhang_YLED|WangLuoTongXin_YLED|ZhuJiTongXin_YLED|CeShi_LED,LED_ON,10);	
  Leds_Action(ZhuDianGuZhang_GLED|BeiDianGuZhang_GLED|WangLuoTongXin_GLED|ZhuJiTongXin_GLED|GongZuo_LED,LED_ON,20);	
//	OS_CRITICAL_EXIT();	//进入临界区
	Buzzer_Action(20);
}


void SelfCheckStep3(void)
{
  Leds_Action(0,LED_ON,20);	
 
	Buzzer_Action(20);
}

//void RefreshRunLedState(void);
extern unsigned char g_RunState;
unsigned char NET_STATE_CHECK=0;
extern void NET_SET(unsigned char data);
void NetCHECK(void)
{
		//网络状态
//		if(g_RunState&(0x01<<5))//故障
		if(NET_STATE_CHECK==0)//故障
	    {
			NET_SET(1);
			g_RunState|= 0x01<<5;
			Leds_Action(WangLuoTongXin_GLED,LED_OFF,0);	
			Leds_Action(WangLuoTongXin_YLED,LED_ON,0);	
		}
		else//正常
		{
			NET_SET(0);
			g_RunState&= ~(0x01<<5);//正常；			
			Leds_Action(WangLuoTongXin_YLED,LED_OFF,0);	
			Leds_Action(WangLuoTongXin_GLED,LED_ON,0);
		}
}

void SelfCheckStep4(void)
{
	VoiceAction(VOICE_SELFCHECK,2);
}
void RunState_Action(unsigned char RunState)
{
	static unsigned char LastRunState=0;
// 	printf("LED_Fresh=%x！\n\r",RunState);//刷新状态；	
	//工作模式
	if(System_Mod==1)
	{
		Leds_Action(CeShi_LED,LED_OFF,0);//关闭测试LED
		if((ReadLedToggleStatus()&GongZuo_LED)==0)
			Leds_Action(GongZuo_LED,LED_TOGGLE,0);
  }
  else//测试模式
  {
		Leds_Action(GongZuo_LED,LED_OFF,0);//关闭工作LED
		if((ReadLedToggleStatus()&CeShi_LED)==0)
			Leds_Action(CeShi_LED,LED_TOGGLE,0);
  }		
	
//	if(g_self_check_step==0)//自检情况下，指示灯不操作；
//	{
		//网络状态
		if(RunState&(0x01<<5))//故障
		{
			Leds_Action(WangLuoTongXin_GLED,LED_OFF,0);	
			Leds_Action(WangLuoTongXin_YLED,LED_ON,0);	
		}
		else//正常
		{
			Leds_Action(WangLuoTongXin_YLED,LED_OFF,0);	
			Leds_Action(WangLuoTongXin_GLED,LED_ON,0);
		}
//	}
	//
	if(RunState&(0x01<<6))
	{
		Leds_Action(ZhuJiTongXin_YLED,LED_ON,0);
		Leds_Action(ZhuJiTongXin_GLED,LED_OFF,0);
	}
	else
	{ //20180424补充主机线路初始化，关闭灯；
		if(MClineFlaut_flag==0xFFFF0000)
		{
			Leds_Action(ZhuJiTongXin_YLED,LED_OFF,0);
			Leds_Action(ZhuJiTongXin_GLED,LED_OFF,0);
		}
		else
		{
			Leds_Action(ZhuJiTongXin_YLED,LED_OFF,0);
			Leds_Action(ZhuJiTongXin_GLED,LED_ON,0);
		}
	}
	
	if(RunState&(0x01<<3))
	{
		Leds_Action(ZhuDianGuZhang_GLED,LED_OFF,0);		
		Leds_Action(ZhuDianGuZhang_YLED,LED_ON,0);
	}
	else
	{
		Leds_Action(ZhuDianGuZhang_YLED,LED_OFF,0);		
		Leds_Action(ZhuDianGuZhang_GLED,LED_ON,0);
	}
	
	if(RunState&(0x01<<4))
	{
		Leds_Action(BeiDianGuZhang_GLED,LED_OFF,0);		
		Leds_Action(BeiDianGuZhang_YLED,LED_ON,0);
	}
	else
	{
		Leds_Action(BeiDianGuZhang_YLED,LED_OFF,0);		
		Leds_Action(BeiDianGuZhang_GLED,LED_ON,0);
	}
	
	if(RunState&0x78)
	{
		Leds_Action(ZongGuZhang_LED,LED_ON,0);
	}
	else
	{
		Leds_Action(ZongGuZhang_LED,LED_OFF,0);
		if(ReadVoiceType()==VOICE_LOCALFAULT) VoiceAction(VOICE_LOCALFAULT,VOICE_HALT);
	}
	if(RunState!=LastRunState)
	{
		if((RunState&0x78)>(LastRunState&0x78))
		{
			if(ReadVoiceType()!=VOICE_LOCALFAULT)VoiceAction(VOICE_LOCALFAULT,VOICE_FOREVER);
		}
		RunState=LastRunState;
	}
}
//每100ms执行一次，读取网络状态（bit5），主电状态（bit3），备电状态（bit4），线路状态（bit6）
//unsigned char UpdataRunState(unsigned char *pRunState)
//{
////	//开机后前40s不报故障0.5s*80
////	static unsigned int Cnt=0;
//	static unsigned char destdata[SERVER_DATA_NUM];
//	unsigned char buf[0x20];
//	//记录上次的运行状态，判定是否增加了故障类型
//	static unsigned char laststate=0;
//	unsigned char state=0;
//	unsigned int len;
//	unsigned char flag=0;
//	
////	if(Cnt<40)
////	{
////		Cnt+=1;
////	}
////	else if(Cnt>=40)
//	{
//		state=Power_State();
//		state|=NetWork_State();
//		if(((state&0x78)>(laststate&0x78)))
//		{
//			flag=1;
//			if(((state&(0x01<<5))-(laststate&(0x01<<5)))==(0x01<<5))flag=2;
//		}
//		laststate=state&0x78;
//		
//		*pRunState&=~0x78;
//		*pRunState|=laststate;
//		if(flag>=1)
//		{
//			len=Device_RunningState(buf);
//			ReportToServer(0x02,buf,len,destdata);
//			if(flag==2)
//			{
//				NetTask_Resume();
//			}
//		}
//	}
//	return laststate;
//}
unsigned char UpdataRunState(void)
{
	unsigned char state=0;
#ifdef FB1010	
	state=Power_State();
#endif
	state|=NetWork_State();
	return (state&0x38);
}
void RunningStateFrame(void)
{
	static unsigned char destdata[SERVER_DATA_NUM];
	unsigned char buf[0x20];
	unsigned int len;
	len=Device_RunningState(buf);
	ReportToServer(0x02,buf,len,destdata);
}
void HeartBeatFrame(void)
{
	static unsigned char destdata[SERVER_DATA_NUM];
	unsigned char buf[0x20];
	unsigned int len;
	len=Device_RunningState(buf);
	buf[1]=0;		//将心跳包区别于上报状态包，不进行存储，触发显示  //ZHAONIG
	ReportToServer(0x02,buf,len,destdata);
}

void VoiceActionClear(void)
{
	 unsigned char flag=0;
	      
	 flag=ReadVoiceType();
	
	 if(flag!=VOICE_MUTE)
	 {
				Leds_Action(XiaoYin_LED,LED_ON,SEC_10);
				VoiceAction(VOICE_MUTE,VOICE_HALT);
				if(DutyFlag!=0)
				{
					//判定是否有DutyFlag,若有则执行查岗声继续相应，静音后返回查岗动作
					Leds_Action(ChaGangYingDa_LED,LED_TOGGLE,MINUTES_5*2);
					if(ReadVoiceType()!=VOICE_DUTY)VoiceAction(VOICE_DUTY,MINUTES_5*2);
				}
		}		
}



