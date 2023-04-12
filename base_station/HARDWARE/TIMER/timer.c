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
//定时器2通道1输入捕获配置
//arr,psc.
//Tout= ((arr+1)*(psc+1))/Tclk；	 
//Tclk： TIM3 的输入时钟频率（单位为 Mhz）。
//Tout： TIM3 溢出时间（单位为 s）。
//例子：	 TIM3_Int_Init(4999,7199);
//则，Tout=（（4900+1）*（7199+1））/72000000=0.5S
//即进入一次定时器中断的时间为0.5秒，即500ms。	 	 
//	TIM3_Int_Init(130,71);//10Khz的计数频率，计数到5000为500ms  
//TIM3_Int_Init(99,71);//100微秒进入一次。10Khz。	 
//TIM3_Int_Init(99,719);//1000微秒（1ms）进入一次。1Khz。
//TIM3_Int_Init(499,719);//5000微秒（5ms）进入一次。500hz。	 
//TIM3_Int_Init(999,719);//10000微秒(10ms进入一次。100hz。
unsigned char TIM2_IRQ_flag=0;
unsigned int Send_Overtime=0;//网关或者服务器发送数据超时时间检测
unsigned int time_step=0;
unsigned int Bluetooh_Package_compare=0;//蓝牙数据包进行比较倒计时暂定2秒比较一次。
unsigned int Bluetooh_scan_time=0;//蓝牙基站定时扫描时间，到0的时候进行扫描，扫描完成重新赋值。
unsigned int Bluetooh_heartbeat_time=0;//蓝牙基站上传给基站的心跳包。5分钟上发一次。
unsigned int Bluetooh_time[Bluetooh_Max_NO];//假设最多有100个蓝牙标签.进行倒计时的时候，整个数组都进行倒计时。
unsigned int send_time=0;//进行查询是否需要给服务器发送数据
unsigned int KEY0_DN_S_effective_time=0;//短按按键有效时间。
unsigned int KEY0_DN_L_effective_time=0;//长按按键有效时间。
TIM_ICInitTypeDef  TIM2_ICInitStructure;
void TIM2_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//使能TIM2时钟
	//初始化定时器2 TIM2	 
	TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//预分频器   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM2, //TIM2
		TIM_IT_Update ,
		ENABLE  //使能
		);
	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 
  TIM_Cmd(TIM2,ENABLE ); 	//使能定时器2
}
//定时器5中断服务程序
// 5ms中断周期
void TIM2_IRQHandler(void)
{
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
//	unsigned char i;	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //清除中断标志位

			if(time_step==0)//1秒进入一次。
			{
				TIM2_IRQ_flag=1;//1秒置1一次。				
				time_step=200;
				timer_flag_int();//系统内标志位进行自减操作。
			}
			else
			{
				time_step--;
			}
			if(Send_Overtime>0)//发送数据时，倒计时用。
			{
				Send_Overtime--;
			}
			USART1_RX_SCAN();//定时器定时扫描串口标志位的变化，及时的进行处理。
			USART2_RX_SCAN();//定时器定时扫描串口标志位的变化，及时的进行处理。
			USART3_RX_SCAN();//定时器定时扫描串口标志位的变化，及时的进行处理。
			UART4_RX_SCAN(); //定时器定时扫描串口标志位的变化，及时的进行处理。
			//Buzzer_SCAN();	//蜂鸣器扫描
			LED_SCAN();			//LED
			KEY_SCAN();			//按键扫描
//			if( (KEY0_DN_S_effective_time==0)&&(KEY0_DN_S==1) )//如果30秒内没有进行配置，则清空短按键标志位。
//			{
//				KEY0_DN_S=0;//按键标志位被清空。	
//			}
//			else
//			{
//				KEY0_DN_S_effective_time--;
//			}
//			if( (KEY0_DN_L_effective_time==0)&&(KEY0_DN_L==1) )//如果5秒内没有进行配置，则清空长按键标志位。
//			{
//				KEY0_DN_L=0;//按键标志位被清空。
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
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
}

void timer_flag_int(void)//系统内标志位进行自减操作。
{
	unsigned char i=0;
	if(send_time>0)//进行查询是否需要给服务器发送数据
	{
		send_time=send_time-1;
	}			
	if(Bluetooh_heartbeat_time>0)//蓝牙基站上传给基站的心跳包。
	{
		Bluetooh_heartbeat_time=Bluetooh_heartbeat_time-1;
	}
	if(Bluetooh_scan_time>0)//蓝牙串口搜索下面蓝牙标签设备倒计时。10秒一次。
	{
		Bluetooh_scan_time=Bluetooh_scan_time-1;
	}
	if(Bluetooh_Package_compare>0)//到了检测蓝牙标签离位状态的时候了。5秒检测一次。
	{
		Bluetooh_Package_compare=Bluetooh_Package_compare-1;
	}
	if(SEND_DATA_to_server_time>0)//往服务器发送数据倒计时。如果超过这个时间没有发送成功，则重新发送。
	{
		SEND_DATA_to_server_time=SEND_DATA_to_server_time-1;
	}
	for(i=0;i<Bluetooh_amount;i++)//有多少个标签就进行几次。
	{
		if(Bluetooh_time[i]>0)//每个蓝牙标签计数器倒计时，如果超时没有被发现，则认为蓝牙标签离位了。然后通过GM3或者433或者LoRa发送出去给基站或者服务器。
		{
			Bluetooh_time[i]=Bluetooh_time[i]-1;
		}		
	}			
}
