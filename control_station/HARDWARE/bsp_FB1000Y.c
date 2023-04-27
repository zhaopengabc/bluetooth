#include "bsp_FB1000Y.h"
#include "MsgCANProcessAppExt.h"
//#include "CAN1ProtocolParsing.h" 
	
//void TIM3_Int_Init(unsigned short int arr, unsigned short int psc)
//{
//    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//    NVIC_InitTypeDef NVIC_InitStructure;

//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

//    //定时器TIM3初始化
//    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
//    TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值
//    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
//    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
//    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位

//    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE ); //使能指定的TIM3中断,允许更新中断

//    //中断优先级NVIC设置
//    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
//    NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

//    TIM_Cmd(TIM3, ENABLE);  //使能TIMx
//}


/*软件重启*/
void SysSoftReset(void)
{
	printf("周期软件重启！\r\n");		
	__set_FAULTMASK(1); 
	NVIC_SystemReset(); 
}
static unsigned int tmr_ms=0;//1S
static unsigned int tmr_sec=0;//1S
static unsigned int tmr_day=0;//1天=86400S
void SystemPeriod_Reset(void)
{
	  tmr_ms++; 
	  if(tmr_ms>100)//1S到；
		{  tmr_ms=0;
		   tmr_sec++;	 			
		}	
	  if(tmr_sec>86400)	//1天到；	
		{  tmr_sec=0;	
			 tmr_day++;	
		}
	  if(tmr_day>6)	//7天到；	
    {	 tmr_day=0;
       SysSoftReset();//软件复位；			
		}
}

//定时器3中断服务程序 10MS
//void TIM3_IRQHandler(void)   //TIM3中断
//{
//    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
//    {
//        TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志
////			  tmrInspect_callback();//CAN数据接收超时判断；
//        SystemPeriod_Reset();//系统周期重启；
//		}
//}
void TIM4_Int_Init(unsigned short int arr, unsigned short int psc);

void bsp_init(void)
{
	delay_init();       //延时初始化
//	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x6000);        //中断向量表重映射、关闭远程升级
	delay_ms(1000);     //滤除，无上电，灯闪现象
	TIM4_Int_Init(100,7200);	//串口接收uart1计时器初始化,周期10mS；
	uart_init(115200);    //串口波特率设置
	LED_Init();         //LED初始化
	KEY_Init();
 // IWDG_Init(IWDG_Prescaler_256, 2500);//打开看门狗
  Buzzer_Init();
	Voice_Init();
	PowerDetec_Init();
	W25q16_Init();
	RTC_Init();
//	LCD_Init();`
	Net_Init();
	
//  Clear_LCD_Num();20181031
//	ProtocolBoardInterfaceInit();
//	CAN1_Mode_Init(CAN1,gCAN1SystemParameter);   //CAN模式初始化使能
//	TIM3_Int_Init(100, 7200);//10mS: CAN超时判断；系统周期重启；每3天重启；
}
