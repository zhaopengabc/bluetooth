#include "server.h"
//#include "data_config.h"
//#include "gprs.h"
//#include "rtc.h"
#include "frame.h"
#include "w25q16.h"
#include "led.h"
#include "lcd_set.h"
#include "k2.h"
#include "voice.h"

#define SEND_FAILED_ON
//服务器接口方式
typedef enum
{
	ETHERNET_Mode=1,
	GPRS_Mode
}SerInterMode_TypeDef;
unsigned char Local_Addres[6];
unsigned char Local_SoftVer[2];
static Server_Data_Typedef Data_RecieveFromServer;
extern Task_Typedef Task[TASK_NUM];
//0:正常监视状态-测试状态  1：火警 
//2：故障  3：主电故障 4：备电故障
//5：与监控中心通信故障 6：监测连接线路故障
unsigned char gRunState=0;
//收到查岗命令后定时结束计数
static unsigned int DutyCnt=0;
//设置标志位，记录流水号
static unsigned int DutyFlag=0;
//收到初始化命令后10s钟执行计数
static unsigned int CmdInitCnt=0;
//没有火警信息高字节为0，火警信息传输中为1，
//传输完成收到应答为2，此时警情消除有效，按下警情消除,清零标志位
unsigned int HuoJingFlag=0;
//没有火警信息高字节为0，火警信息传输中为1，
//传输完成收到应答为2，此时警情消除有效，按下警情消除,清零标志位
unsigned int GuZhangFlag=0;
unsigned int JianGuanFlag=0;
unsigned int PingBiFlag=0;
unsigned int ManAlarmFlag=0;
unsigned int ReportToServer(unsigned char type,unsigned char *data,unsigned int len);
unsigned char Power_Detection(void);

	
void NetWork_State(void)
{
	unsigned char flag=0;
	static unsigned char f0cnt=0;
	static unsigned char f1cnt=0;
	if(Server_Interface_Mode==GPRS_Mode)
	{
		flag=GPRS_Work();
	}
	else
	{
		flag=NetK2_Work();
	}
	if(flag==0)
	{
		f1cnt=0;
		f0cnt++;
		if(f0cnt>=25)
		{
			f0cnt=25;
			gRunState&=~(1<<5);
		}
	}
	else
	{
		f0cnt=0;
		f1cnt++;
		if(f1cnt>=25)
		{
			f1cnt=25;
			gRunState|=1<<5;
		}
	}
}
void Power_State(void)
{
	unsigned char flag=0;
	flag=Power_Detection();
	flag&=3<<3;
	gRunState&=~(3<<3);
	gRunState|=flag;
}
void RunStateRefesh(void)
{
	//开机后前40s不报故障0.5s*80
	static unsigned int Cnt=0;
	//记录上次的运行状态，判定是否增加了故障类型
	static unsigned char flag=0;
	unsigned int len;
	unsigned char destdata[0x20];
	unsigned char tmp[0x20];
	LED_TWINKLE(GongZuo_LED,1,1,0xFF);
	gRunState|=0x01;
	if(Cnt<80)Cnt+=1;
	if(gRunState&0x78)
	{
		gRunState|=0x01<<2;
		LED_TWINKLE(ZongGuZhang_LED,1,1,120);
		if(((gRunState&0x78)>(flag&0x78))&&(Cnt>=80))
		{
			VoiceSet(VOICE_LOCALFAULT,VOICE_FOREVER);
			len=Device_RunningState(gRunState,destdata);
			ReportToServer(0x02,destdata,len);
			//保存信息
			memset(tmp,0,0x20);
			memcpy(tmp,destdata,3);
			memcpy(&tmp[0x16],&destdata[0x3],6);
			Writeflash_Infor(tmp,0x1c);
		}
	}
	else
	{
		gRunState&=~(0x01<<2);
		LED_TWINKLE(ZongGuZhang_LED,0,0,120);
		if(ReadVoiceType()==VOICE_LOCALFAULT) VoiceSet(VOICE_LOCALFAULT,VOICE_HALT);
	}
	if(Cnt>=80)flag=gRunState&0x7c;
	if((self_check_step>5)||(self_check_step<2))
	{
	if(gRunState&(0x01<<5))
	{
		LED_TWINKLE(WangLuoGuZhang_LED,1,1,0xFF);
	}
	else
	{
		LED_TWINKLE(WangLuoGuZhang_LED,0,0,0xFF);
	}
	if(gRunState&(0x01<<6))
	{
		LED_TWINKLE(XianLuGuZhang_LED,1,1,0xFF);
	}
	else
	{
		//判定为线路正常，无故障
		LED_TWINKLE(XianLuGuZhang_LED,0,0,120);
	}
	if(gRunState&(0x01<<3))
	{
		LED_TWINKLE(ZhuDianGuZhang_YLED,1,1,0xFF);
		LED_TWINKLE(ZhuDianGuZhang_GLED,0,0,0xFF);
	}
	else
	{
		LED_TWINKLE(ZhuDianGuZhang_GLED,1,1,0xFF);
		LED_TWINKLE(ZhuDianGuZhang_YLED,0,0,0xFF);
	}
	if(gRunState&(0x01<<4))
	{
		LED_TWINKLE(BeiDianGuZhang_YLED,1,1,0xFF);
		LED_TWINKLE(BeiDianGuZhang_GLED,0,0,0xFF);
	}
	else
	{
		LED_TWINKLE(BeiDianGuZhang_GLED,1,1,0xFF);
		LED_TWINKLE(BeiDianGuZhang_YLED,0,0,0xFF);
	}
	}
}
//每10ms进入1次计数
void CmdInit_Cnt(void)
{
	if(CmdInitCnt!=0)
	{
		CmdInitCnt--;
		if(CmdInitCnt==0)
		{
			NVIC_SystemReset();
		}
	}
}
void HeartBeat(void)
{
	unsigned int len;
	unsigned char destdata[0x20];
	len=Device_RunningState(gRunState,destdata);
	ReportToServer(0x02,destdata,len);
}
