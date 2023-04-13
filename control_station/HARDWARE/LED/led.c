#include "stm32f10x.h"
#include "includes.h"
#include "led.h"
#include "lcd.h"
//宏定义
#define NOPS_DELAY	40
#define LEDNUM_MAX	24

//定义变量
OS_TMR ledtmr[LEDNUM_MAX];
OS_TMR ledtoggletmr;
OS_MUTEX Led_Mutex;
static unsigned int Hc595_Toggle=0;
const unsigned char tab[24]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
//声明函数
unsigned int ReadLedToggleStatus(void)
{
	return Hc595_Toggle;
}
//延时
static void delay_hc595(unsigned int n_nops)
{
	unsigned char nop_flag=0;
	unsigned int i=0;
	for(i=0;i<n_nops;i++)
	{
		nop_flag=~nop_flag;
	}
}
static void HC595SendData(unsigned char SendVal)
{
	unsigned char i=0;
	for(i=0;i<8;i++) 
	{
		if((SendVal<<i)&0x80)
			DATA_595=1;					//最高位与SendVal左移的最高位 进行逻辑运算
		else 
			DATA_595=0;					//如果为真 MOSIO = 1  
		delay_hc595(NOPS_DELAY);
		
		RCK_595=0;
		delay_hc595(NOPS_DELAY);
		RCK_595=1;
	}
}
#ifdef FB1010
//更新LED状态
//执行一次时间为250us左右，delay_hc595(NOPS_DELAY)时长为10us
static void Update_LEDStatus(unsigned int status)
{
	status&=0x00ffffff;
	
	HC595SendData(status>>16);
	HC595SendData(status>>8);
	HC595SendData(status);
	
	SCK_595=0;
	delay_hc595(NOPS_DELAY);
	SCK_595=1;
	//刷新完毕，默认高电平
	DATA_595=1;
}
#endif


#ifdef FB1030
//更新LED状态
//执行一次时间为250us左右，delay_hc595(NOPS_DELAY)时长为10us  2和5不好使
static void Update_LEDStatus(unsigned int status)
{
	status&=0x00ffffff;
	//1工作灯
//	if((status&GongZuo_LED)==0)
//	{GPIO_ResetBits(GPIOB,GPIO_Pin_0 |GPIO_Pin_1 |GPIO_Pin_3 |GPIO_Pin_4 |GPIO_Pin_5 |GPIO_Pin_6 |GPIO_Pin_7 |GPIO_Pin_8 | GPIO_Pin_9);
//	 GPIO_ResetBits(GPIOC,GPIO_Pin_5 );
//	}//亮		工作灯
//	else
//  {GPIO_SetBits(GPIOB,GPIO_Pin_0 |GPIO_Pin_1 |GPIO_Pin_3 |GPIO_Pin_4 |GPIO_Pin_5 |GPIO_Pin_6 |GPIO_Pin_7 |GPIO_Pin_8 | GPIO_Pin_9);
//	 GPIO_SetBits(GPIOC,GPIO_Pin_5 );
//	}	 //灭	
	
	//1工作灯
	if((status&GongZuo_LED)==0)
	{GPIO_ResetBits(GPIOB,GPIO_Pin_3);}//亮		工作灯
	else
  {GPIO_SetBits(GPIOB,GPIO_Pin_3);}	 //灭
	
  //2主机火警灯 手动报警灯
	if((status&HuoJing_LED)==0)
	{GPIO_ResetBits(GPIOB,GPIO_Pin_4);}//亮		 
	else
  {GPIO_SetBits(GPIOB,GPIO_Pin_4);}	 //灭
	
  //3主机状态灯-故障-屏蔽-状态
	if((status&GuZhang_LED)==0)
	{GPIO_ResetBits(GPIOB,GPIO_Pin_5);}//亮		工作灯
	else
  {GPIO_SetBits(GPIOB,GPIO_Pin_5);}	 //灭
	
  //4主机通讯
	if((status&ZhuJiTongXin_GLED)==0)
	{GPIO_ResetBits(GPIOB,GPIO_Pin_6);}//亮		工作灯
	else
  {GPIO_SetBits(GPIOB,GPIO_Pin_6);}	 //灭

 //5查岗
	if((status&ChaGangYingDa_LED)==0)
	{GPIO_ResetBits(GPIOB,GPIO_Pin_7);}//亮		工作灯
	else
  {GPIO_SetBits(GPIOB,GPIO_Pin_7);}	 //灭

	//6网络状态 8是红色 9是绿色
	if((status&WangLuoTongXin_GLED)==0)
	{	GPIO_SetBits(GPIOB,GPIO_Pin_8);
		GPIO_ResetBits(GPIOB,GPIO_Pin_9);}//亮		工作灯
	else
  { GPIO_SetBits(GPIOB,GPIO_Pin_9);
		GPIO_ResetBits(GPIOB,GPIO_Pin_8);}//亮
	
// //6网络状态 8是红色 9是绿色
//	if((status&WangLuoTongXin_GLED)==0)
//	{GPIO_ResetBits(GPIOB,GPIO_Pin_8);}//亮		工作灯
//	else
//  {GPIO_SetBits(GPIOB,GPIO_Pin_8);}	 //灭

//	if((status&WangLuoTongXin_YLED)==0)
//	{GPIO_ResetBits(GPIOB,GPIO_Pin_9);}//亮		工作灯
//	else
//  {GPIO_SetBits(GPIOB,GPIO_Pin_9);}	 //灭
}
#endif

//对ledn设置为ledaction状态
//返回值：0--入口参数不匹配；1--led状态设置成功；
unsigned char ave_Hc595_Flag=0;
unsigned int Save_Hc595_Status=0;
unsigned int Save_Hc595_Toggle=0;
static void LedSet(unsigned int ledn,unsigned char ledaction)
{
	OS_ERR err;
	static unsigned int Hc595_Status=0xffffffff;
	unsigned char i=0;
	unsigned int sd=0;
	unsigned int nbr=0;
	
	CPU_SR_ALLOC();
	if((ledn>LED_ALL)||(ledn==0)||(ledaction>LED_TOGGLE))
	{
	}
	else
	{
		//如果自检启动，保存现有状态；	
		if((g_self_check_step>0)&&(ave_Hc595_Flag==0))
		{
		  Save_Hc595_Status=Hc595_Status;
		  Save_Hc595_Toggle=Hc595_Toggle;		
			ave_Hc595_Flag=1;
		}
		
		OS_CRITICAL_ENTER();	//进入临界区
		switch(ledaction)
		{
			case LED_OFF:
				Hc595_Status|=ledn;
				Hc595_Toggle&=~ledn;
			  //记住 火警、故障、屏蔽、状态指示灯；
			break;
			case LED_ON:
				Hc595_Status&=~ledn;
				Hc595_Toggle&=~ledn;
			break;
			case LED_TOGGLE:
				Hc595_Status^=ledn;
				Hc595_Toggle|=ledn;
			break;
			default:			break;
		}		

		//如果自检完成，恢复原来状态；	
		if((g_self_check_step==0)&&(ave_Hc595_Flag==1))
		{	//不操作，网络正常，网络故障，总故障；
//			Hc595_Status=Save_Hc595_Status;
//		    Hc595_Toggle=Save_Hc595_Toggle;	
//			ave_Hc595_Flag=0;
						
			sd=Save_Hc595_Status;//历史回放；
			sd=sd&0xFFFFFFFC;//0、1位清零；
			nbr=(Hc595_Status&0x00000003);//获取0、1值
			sd=sd+nbr;//加入0、1；
			sd=sd&0xFFFFBFFF;//14位放零；
			nbr=(Hc595_Status&0x00004000);//14位结果；
			sd|=nbr;//加进去；
			Hc595_Status=sd;
			
		    Hc595_Toggle=Save_Hc595_Toggle;	
			ave_Hc595_Flag=0;
		}
		
		OS_CRITICAL_EXIT();		//退出临界区
		
		OSMutexPend((OS_MUTEX*	)&Led_Mutex,
					(OS_TICK	) 0,
					(OS_OPT		) OS_OPT_PEND_BLOCKING,
					(CPU_TS    *) 0,
					(OS_ERR*	)&err);
		Update_LEDStatus(Hc595_Status);
		OSMutexPost((OS_MUTEX*)&Led_Mutex,
					(OS_OPT 	)OS_OPT_POST_NONE,
					(OS_ERR*	)&err);
	}
}
void ledtmr_callback(void *p_tmr,void *p_arg)
{
	OS_ERR err;
	unsigned char *pbuf;
	unsigned char buf=0;
//	printf("T_END\r\n");
	pbuf=p_arg;
	buf=*pbuf;
	LedSet((0x01<<buf),LED_OFF);
	OSTmrDel((OS_TMR		*)&ledtmr[buf],(OS_ERR*	)&err);
}
void ledtoggletmr_callback(void *p_tmr,void *p_arg)
{
	OS_ERR err;
	unsigned int toggle;
	unsigned char i=0;
	OS_TICK tick_time;
	CPU_SR_ALLOC();
	
	OS_CRITICAL_ENTER();	//进入临界区
	toggle=Hc595_Toggle;
	OS_CRITICAL_EXIT();		//退出临界区
	if(0!=toggle)
	{
		//读取定时器剩余时间是否很少，若小于100ms不闪烁,
		//防止出现闪一下的现象导致效果不好
		for(i=0;i<24;i++)
		{
			if((0x01<<i)&toggle)
			{
				tick_time=OSTmrRemainGet((OS_TMR		*)&ledtmr[i],(OS_ERR*	)&err);
				if((tick_time<=20)&&(tick_time!=0))
				{
					toggle&=~(0x01<<i);
				}
			}
		}
		LedSet(toggle,LED_TOGGLE);
	}
	else
	{
		OSTmrDel((OS_TMR		*)&ledtoggletmr,(OS_ERR*	)&err);
	}
}

//led初始化
#ifdef FB1010
void LED_Init(void)
{
	OS_ERR err;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能GPIOF时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOF时钟

	/* 配置拨码开关1引脚 */	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); // 改变指定管脚的映射,JTAG-DP禁用,SW-DP使能
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 ;				 //LED0-->PA.8 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.8
	GPIO_SetBits(GPIOB,GPIO_Pin_7 | GPIO_Pin_8 );						 //PA.8 输出高
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 
	GPIO_SetBits(GPIOA,GPIO_Pin_15);						  
	
	OSMutexCreate((OS_MUTEX*)&Led_Mutex,
				(CPU_CHAR*	)"Led_Mutex",
				(OS_ERR*	)&err);
	LedSet(LED_ALL&(~(GongZuo_LED)),LED_OFF);
	LedSet(GongZuo_LED,LED_ON);
}
#endif

#ifdef FB1030
//led初始化
void LED_Init(void)
{
	OS_ERR err;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能GPIOF时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOF时钟

	/* 配置拨码开关1引脚 */	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); // 改变指定管脚的映射,JTAG-DP禁用,SW-DP使能
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1 |GPIO_Pin_3 |GPIO_Pin_4 |GPIO_Pin_5 |GPIO_Pin_6 |GPIO_Pin_7 |GPIO_Pin_8 | GPIO_Pin_9 ;				 //LED0-->PA.8 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.8
	GPIO_ResetBits(GPIOB,GPIO_Pin_0 |GPIO_Pin_1 |GPIO_Pin_3 |GPIO_Pin_4 |GPIO_Pin_5 |GPIO_Pin_6 |GPIO_Pin_7 |GPIO_Pin_8 | GPIO_Pin_9 );		
 		  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5  ;				 //LED0-->PA.8 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.8
	GPIO_ResetBits(GPIOC,GPIO_Pin_5 );		
	
	OSMutexCreate((OS_MUTEX*)&Led_Mutex,
				(CPU_CHAR*	)"Led_Mutex",
				(OS_ERR*	)&err);
	LedSet(LED_ALL&(~(GongZuo_LED)),LED_OFF);
	LedSet(GongZuo_LED,LED_ON);
}
#endif
//nmsec为100ms的整数倍
unsigned char Leds_Action(unsigned int ledn,unsigned char ledaction,unsigned int n100msec)
{
	OS_ERR err;
	OS_STATE state;
	unsigned char i=0;
	
//	if((ledn==WangLuoTongXin_YLED)&&(ledaction==1))//1
//	{i=0;}
//	 if(ledn==WangLuoTongXin_GLED)//2
//	{i=0;}
//	 if(ledn==ZongGuZhang_LED)//2
//	{i=0;}	
	
	
	if((ledn<=LED_ALL)&&(ledn!=0)&&(ledaction<=LED_TOGGLE))
	{
		if(ledaction==LED_OFF)
		{
			LedSet(ledn,LED_OFF);
		}
		else
		{
			if(ledaction==LED_ON)
			{
				LedSet(ledn,LED_ON);
			}
			else if(ledaction==LED_TOGGLE)
			{
				LedSet(ledn,LED_TOGGLE);
				
				state=OSTmrStateGet((OS_TMR		*)&ledtoggletmr,(OS_ERR*	)&err);
				if(state==OS_TMR_STATE_UNUSED)
				{
					OSTmrCreate((OS_TMR		*)&ledtoggletmr,
							(CPU_CHAR	*)"ledtoggletmr",
							(OS_TICK	)50,
							(OS_TICK	)50,
							(OS_OPT		)OS_OPT_TMR_PERIODIC,
							(OS_TMR_CALLBACK_PTR)ledtoggletmr_callback,
							(void*	)0,
							(OS_ERR*	)&err);
					OSTmrStart((OS_TMR		*)&ledtoggletmr,(OS_ERR*	)&err);
				}
			}
		
			for(i=0;i<24;i++)
			{
				if((0x01<<i)&ledn)
				{
					state=OSTmrStateGet((OS_TMR		*)&ledtmr[i],(OS_ERR*	)&err);
					if(state!=OS_TMR_STATE_UNUSED)
					{
						OSTmrDel((OS_TMR		*)&ledtmr[i],(OS_ERR*	)&err);
					}
					if(n100msec!=0)
					{
//						printf("T_BEG\r\n");
						OSTmrCreate((OS_TMR		*)&ledtmr[i],
								(CPU_CHAR	*)"tmrx",
								(OS_TICK	)n100msec*20,
								(OS_TICK	)0,
								(OS_OPT		)OS_OPT_TMR_ONE_SHOT,
								(OS_TMR_CALLBACK_PTR)ledtmr_callback,
								(void*	)&tab[i],
								(OS_ERR*	)&err);
						OSTmrStart((OS_TMR		*)&ledtmr[i],(OS_ERR*	)&err);
					}
				}
			}
		}
	}
	else
	{
		return 0;
	}
	return 1;
}
void LED_Test(void)
{
	OS_ERR err;
	while(1)
	{
		Leds_Action(HuoJing_LED,LED_ON,2);//测试火警灯，亮；
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
		Leds_Action(JianGuan_LED,LED_ON,2);
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
		Leds_Action(GuZhang_LED,LED_ON,2);
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
		Leds_Action(PingBi_LED,LED_ON,2);
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
		Leds_Action(GongZuo_LED,LED_ON,2);
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
		Leds_Action(ZongGuZhang_LED,LED_ON,2);
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
		Leds_Action(ZhuJiTongXin_GLED,LED_ON,2);
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
		Leds_Action(WangLuoTongXin_GLED,LED_ON,2);
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
		
		Leds_Action(ZhuDianGuZhang_GLED,LED_ON,2);
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
//		Leds_Action(XianLuGuZhang_LED,LED_ON,2);
//		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
//		Leds_Action(WangLuoGuZhang_LED,LED_ON,2);
//		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
		Leds_Action(BeiDianGuZhang_GLED,LED_ON,2);
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
		Leds_Action(JingQingXiaoChu_LED,LED_ON,2);
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
//		Leds_Action(GuZhangXiaoChu_LED,LED_ON,2);
//		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
//		Leds_Action(BenJiZiJian_LED,LED_ON,2);
//		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
		Leds_Action(XiaoYin_LED,LED_ON,2);
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
		Leds_Action(ShouDongBaoJing_LED,LED_ON,2);
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
		Leds_Action(ChaGangYingDa_LED,LED_ON,2);
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
		
		Leds_Action(ZhuJiTongXin_YLED,LED_ON,2);
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
		Leds_Action(WangLuoTongXin_YLED,LED_ON,2);
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
		Leds_Action(ZhuDianGuZhang_YLED,LED_ON,2);
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
		Leds_Action(BeiDianGuZhang_YLED,LED_ON,2);
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
		
	}
//	for(;;)
//	{
//		Leds_Action(LED_ALL,LED_ON,100);
//		OSTimeDlyHMSM(0,0,30,0,OS_OPT_TIME_HMSM_STRICT,&err);
//		Leds_Action(LED_ALL,LED_TOGGLE,100);
//		OSTimeDlyHMSM(0,0,30,0,OS_OPT_TIME_HMSM_STRICT,&err);
//	}
}

//#define HuoJing_LED		   (0x00000001<<4) //1 OK
//#define JianGuan_LED		 (0x00000001<<7) //2 OK 状态           
//#define GuZhang_LED			 (0x00000001<<5) //3 OK
//#define PingBi_LED		   (0x00000001<<6) //4 OK

void SaveLed(void)
{
	
	
}

void RefreshLed(void)
{
	
	
}
