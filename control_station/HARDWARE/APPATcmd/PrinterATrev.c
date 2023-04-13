#include "PrinterATrev.h"
#include "stm32f10x.h" 
#include <string.h>
#include <stdio.h> 

#define UART1_RX_DATANUM 200
#define TASK_UART1_CNT 4  //40mS数据接收等待；
#define UART1_RX_GROUPNUM 3

typedef struct
{
  unsigned char Flag;
	unsigned int Lenth;
	unsigned char data[UART1_RX_DATANUM];
}Uart1_Data_Typedef;
static Uart1_Data_Typedef Uart1RxBuf[UART1_RX_GROUPNUM];
static unsigned char Uart1_Rx_Cnt=0;
static unsigned char Uart1_Wr_Addr=0;//写位置；
static unsigned char Uart1_Rd_Addr=0;

//串口uart1接收数据计时器；	
void TIM4_Int_Init(unsigned short int arr, unsigned short int psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE ); //使能指定的TIM3中断,允许更新中断
	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
	TIM_Cmd(TIM4, ENABLE);  //使能TIMx
}

//在周期函数中调用，对读取地址与写地址进行更新
void Uart1_TimCnt(void)
{
	if(Uart1_Rx_Cnt!=0)//接收到字节；
	{
			 Uart1_Rx_Cnt--;//减去1；
		if(Uart1_Rx_Cnt==0)//减到0；表示一帧接收完毕；
		{
			Uart1RxBuf[Uart1_Wr_Addr].Flag=1;//读取一个帧成功完成；
			Uart1_Wr_Addr++;//存储下一帧的序号；
			if(Uart1_Wr_Addr>=UART1_RX_GROUPNUM)Uart1_Wr_Addr=0;//大于3个复位；
			if(Uart1_Rd_Addr==Uart1_Wr_Addr)
			{
				Uart1_Rd_Addr++;
				if(Uart1_Rd_Addr>=UART1_RX_GROUPNUM)Uart1_Rd_Addr=0;
			}
		}
	}
}
//定时器4中断服务程序
void TIM4_IRQHandler(void)   //TIM3中断
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //清除TIMx更新中断标志 
		    Uart1_TimCnt(); 
		}
}

void Reset_Uart1RxBuf(void)
{
	unsigned char i;
	Uart1_Rd_Addr=0;
	Uart1_Wr_Addr=0;
	for(i=0;i<UART1_RX_GROUPNUM;i++)
	{
				Uart1RxBuf[i].Flag=0;
        Uart1RxBuf[i].Lenth=0;
        memset(Uart1RxBuf[i].data,0,UART1_RX_DATANUM);
	}
}

//在中断函数中调用，自动存储
void Uart1_RxISR(unsigned char buf)
{
    Uart1_Rx_Cnt=TASK_UART1_CNT;//复位字节读取的延时时间；
	
    Uart1RxBuf[Uart1_Wr_Addr].Flag=0;//清零；
	
 if(Uart1RxBuf[Uart1_Wr_Addr].Lenth>=UART1_RX_DATANUM-1) //长度超出;
		{Uart1RxBuf[Uart1_Wr_Addr].Lenth=0;}//复位；
 
    Uart1RxBuf[Uart1_Wr_Addr].data[Uart1RxBuf[Uart1_Wr_Addr].Lenth++] = buf;//装填；
}
//数据UART数据接收中断；
void USART1_IRQHandler(void)
{
  unsigned char data; 
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
			USART_ClearITPendingBit(USART1, USART_IT_RXNE);
			data=USART1->DR;
			Uart1_RxISR(data);
    }	
}
//接收数据读取；
unsigned int Uart1_Rx_Read(unsigned char *Rxdata)
{
	unsigned int tmp=0;
	if(Uart1RxBuf[Uart1_Rd_Addr].Flag==1)//如果接收buff里面有数据；
    {
				//读取数据
				tmp=Uart1RxBuf[Uart1_Rd_Addr].Lenth;//获取长度；
				memcpy(Rxdata,Uart1RxBuf[Uart1_Rd_Addr].data,Uart1RxBuf[Uart1_Rd_Addr].Lenth);//提取数据；
        Uart1RxBuf[Uart1_Rd_Addr].Flag=0;//清零；
        Uart1RxBuf[Uart1_Rd_Addr].Lenth=0;//清零；
        memset(Uart1RxBuf[Uart1_Rd_Addr].data,0,UART1_RX_DATANUM);//清零；
        Uart1_Rd_Addr++;//下次读取；
        if(Uart1_Rd_Addr>=UART1_RX_GROUPNUM)Uart1_Rd_Addr=0;//清零；
    }
	return tmp;
}