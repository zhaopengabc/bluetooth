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

typedef	void (*FUN)(void);//函数变量声明；  
PluginInterface Plugin;//协议解析插件初始化；
unsigned char PushButtonStatus=0;
volatile unsigned char SystemTime[6]={0x0C,0x0B,0x0A,0x09,0x08,0x12}; //系统时间；
volatile HRINT16U MCLineLINK=0;//主机线路状态计数标志；
//主机线路监测：
unsigned char UpFaultNum=3;//主机线路故障标志，=0正常；=1故障；=3无此功能；
//软件定时器定义：
volatile unsigned int Sys_Second_100mS=0;
unsigned char MIM_flag=1;
unsigned char PZM_flag=0;
unsigned int  MIM_flag_Count=0;
unsigned int MClineFlaut_flag=0xFFFF0000;

/*软件定时器回调函数*/
void TmrCallback(void)//100m秒钟定时；	
{
	NetHeart_SndCot++;
	Sys_Second_100mS++;	//100mS标志计数（主机解析用）；
//	Sys_WD_100mS++;	//100mS标志计数（主机解析用）；	
	/////主机连接判断///////	
#ifdef MCLineJudge_Enable //主机线路监测宏；
     MCLineLINK++;
	if(MCLineLINK>600)//60S判断；
//		if(MCLineLINK>200)//20S判断；
//	if(MCLineLINK>300)//30S判断；
	{ 
		 MCLineLINK=601;
		 UpFaultNum=1;  //线路故障
	}
	else
	{  UpFaultNum=0; }//线路正常；
#endif 	
	///////密码用有效倒计时1h///////
	if(MIM_flag==1)
	{
		MIM_flag_Count++;
		if(MIM_flag_Count>=36000)
		{ MIM_flag=0;
		  PZM_flag=0;}
	}
	///////定期重启/////
//  SystemPeriod_Reset();
}
extern void tmrInspect_callback(void);    //巡检定时器回调函数
void TIM2_IRQHandler(void)   //TIM3中断
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx更新中断标志 
			  TmrCallback();
				tmrInspect_callback();//CAN数据接收超时判断；
				SystemPeriod_Reset();//系统周期重启；
		}
}

void TIM2_Int_Init(unsigned short int arr, unsigned short int psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE ); //使能指定的TIM3中断,允许更新中断
	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级1级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
	TIM_Cmd(TIM2, ENABLE); //使能TIMx
}

//串口uart2接收数据计时器；
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
static void TIM3_Int_Init(unsigned short int arr, unsigned short int psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE ); //使能指定的TIM3中断,允许更新中断
	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
	TIM_Cmd(TIM3, ENABLE); //使能TIMx
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
	
//	 	TIM2_Int_Init(1000,7200);	//解析周期100mS；		
	 	TIM3_Int_Init(100,7200);	//串口接收uart计时器初始化,周期10mS；
	
	//初始化协议定义；
    Plugin=DevicePlugin_Install();
	  ((void(*)(FUN)) Plugin.cPLUGIN_Install)(0);//协议解析初始化，提取接口参数；								
//定义主机CAN2;	
#ifdef CAN_Enable                                   
//     CAN1Init_Parameter(gCANSystemParameter);//CAN初始化；
	   CAN1_Mode_Init(CAN1,gCANSystemParameter);   //CAN模式初始化使能
#endif	
								
//定义主机RS232，串口2;
#if defined RS232_Enable	 
	   USARTx_Mode_Init(USART2,gUARTSystemParameter);//串口2初始化；
#endif	
								
//定义主机RS485，串口3;								
#if defined RS485_Enable  
     GPIO_RS485_Init();//RS485;
	   USARTx_Mode_Init(USART3,gUARTSystemParameter);//串口2初始化；								
#endif
	
}
void ProalyRun(void)
{
	 ((void(*)(FUN)) Plugin.cPLUGIN_Run)(0);//协议解析任务；

#ifdef MCLineJudge_Enable 	
				//信息体4
	 if(UpFaultNum==0x01)//主机线路故障；
		{ MClineFlaut_flag=1;}  
	 else if (UpFaultNum==0x00)//主机线路正常；
		{ MClineFlaut_flag=0;} 
#endif		
}


/*CAN2中断函数*/ //主机CAN接口
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

/*UART2中断函数*/  //主机RS232接口\主机RS485接口\打印机串口
#if defined  RS232_Enable
void USART2_IRQHandler(void)
{
#ifdef UARTBUFFER_Enable	
	YH_UART_IRQHandler();//系统自动收数据；
#else	
 ((void(*)(FUN)) Plugin.cPLUGIN_Interrupt)(0);//自定义接收数据；	
#endif	
	
#ifdef MCLineJudge_Enable 	
		  MCLineLINK=0; //主机线路故障；
#endif		
}
#endif

/*UART2中断函数*/  //主机RS232接口\主机RS485接口\打印机串口
#if defined RS485_Enable
void USART3_IRQHandler(void)
{
#ifdef UARTBUFFER_Enable	
	YH_UART3_IRQHandler();//系统自动收数据；
#else	
 ((void(*)(FUN)) Plugin.cPLUGIN_Interrupt)(0);//自定义接收数据；	
#endif	
	
#ifdef MCLineJudge_Enable 	
		  MCLineLINK=0; //主机线路故障；
#endif		
}
#endif

#endif


void Time_System_Get(void)
{
	 RTC_Get((char *)&SystemTime);
	 printf("系统时间:%d-%d-%d %d:%d:%d\n", SystemTime[5],SystemTime[4],SystemTime[3],SystemTime[2],SystemTime[1],SystemTime[0]);
}