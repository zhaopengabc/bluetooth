/*  BEGIN_FILE_HDR
************************************************************************************************
*   NOTICE                              
************************************************************************************************
*   File Name       :  
************************************************************************************************
*   Project/Product : 
*   Title           : 
*   Author          : 
************************************************************************************************
*   Description     : 
*
************************************************************************************************
*   Limitations     : 
*
************************************************************************************************
*
************************************************************************************************
*   Revision History:
* 
*    Version     Date           Initials       CR#           Descriptions
*    --------    ----------     -----------    ----------    ---------------
*    1.0         2017/08/03     ning.zhao     N/A           Original                    
************************************************************************************************
include files */
#include "MsgSPIProcessDrv.h"
#include "includes.h"
#include "SendToServerDataType.h"
#include "stm32f10x_tim.h"

unsigned char g_rxdata[SERVER_DATA_NUM];
unsigned int g_cnt;
static unsigned char rxdata[SERVER_DATA_NUM];
static unsigned int cnt=0;
extern OS_TCB hostTaskTCB;
//OS_TMR SpiRxtmr;
void TIM5_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //时钟使能
	
	TIM_UpdateRequestConfig(TIM5, TIM_UpdateSource_Regular);
    //定时器TIM3初始化
    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位

    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE ); //使能指定的TIM3中断,允许更新中断

    //中断优先级NVIC设置
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

    TIM_Cmd(TIM5, DISABLE);  //使能TIMx
}
unsigned char g_HostRecFlag=0;
void TIM5_IRQHandler(void)   //TIM5中断
{
	OS_ERR err;
//	CPU_TS ts;
    if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
    {
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update  );  //清除TIMx更新中断标志
		TIM_Cmd(TIM5, DISABLE);
		g_cnt=cnt;
		if(g_cnt>SERVER_DATA_NUM) g_cnt=10;
		memcpy(g_rxdata,rxdata,g_cnt);
		memset(rxdata,0,SERVER_DATA_NUM);
//		if(cnt>20)
//		{
//			cnt=15;
//		}
		cnt=0;
//		OSTaskSemPost((OS_TCB	*)&hostTaskTCB,
//						(OS_OPT	)OS_OPT_POST_NO_SCHED,
//						(OS_ERR *)&err);
			g_HostRecFlag=1;
    }
}
static void Update_Timer(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
}
void NVIC_SPI(FunctionalState state)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = state;
	NVIC_Init(&NVIC_InitStructure);
	SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, state);
}
void SPI_ConfigarationInit(unsigned char SPIxState)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,DISABLE );
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,DISABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE );
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
	SPI_Cmd(SPI2, DISABLE);
	SPI_I2S_DeInit(SPI2);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	if(SPIxState==SPIxTxen)
	{
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	}
	else
	{
//		SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
		SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
	}
	SPI_Init(SPI2, &SPI_InitStructure);
	if(SPIxState==SPIxTxen)
	{
		NVIC_SPI(DISABLE);
	}
	else
	{
		NVIC_SPI(ENABLE);
	}
	SPI_Cmd(SPI2, ENABLE);
}

//void SpiRxtmr_callback(void *p_tmr,void *p_arg)
//{
//	OS_ERR err;
//	CPU_TS ts;
//	g_cnt=cnt;
//	memcpy(g_rxdata,rxdata,g_cnt);
//	memset(rxdata,0,g_cnt);
//	cnt=0;
//	OSTmrDel((OS_TMR		*)&SpiRxtmr,(OS_ERR*	)&err);
//	OS_TaskQPost((OS_TCB	*)&hostTaskTCB,
//				(void		*)g_rxdata,
//				(OS_MSG_SIZE)g_cnt,
//				(OS_OPT		)OS_OPT_POST_FIFO,
//				(CPU_TS		)ts,
//				(OS_ERR		*)&err
//				);
//}

void SPI2_IRQHandler(void)
{
//	OS_ERR err;
//	OS_STATE state;
	if(SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_RXNE) != RESET)
	{
		rxdata[cnt]=SPI_I2S_ReceiveData(SPI2);
		cnt++;
		if(cnt>=0x400)cnt=0;
		
		Update_Timer(100,7200);
		TIM_Cmd(TIM5, ENABLE);
//		state=OSTmrStateGet((OS_TMR		*)&SpiRxtmr,(OS_ERR*	)&err);
//		if(state!=OS_TMR_STATE_UNUSED)
//		{
//			OSTmrDel((OS_TMR		*)&SpiRxtmr,(OS_ERR*	)&err);
//		}
//		OSTmrCreate((OS_TMR		*)&SpiRxtmr,
//				(CPU_CHAR	*)"SpiRxtmr",
//				(OS_TICK	)10,
//				(OS_TICK	)10,
//				(OS_OPT		)OS_OPT_TMR_ONE_SHOT,
//				(OS_TMR_CALLBACK_PTR)SpiRxtmr_callback,
//				(void*	)0,
//				(OS_ERR*	)&err);
//		OSTmrStart((OS_TMR		*)&SpiRxtmr,(OS_ERR*	)&err);
	}
	else if(SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_ERR) != RESET)
	{
		SPI_I2S_ReceiveData(SPI2);
	}
}
