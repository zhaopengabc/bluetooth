#ifndef  REVEIVEDATABUFFER_C
#define  REVEIVEDATABUFFER_C
//串口uart2接收数据；
#include "ReceiveDataBuffer.h"
#include "DeviceCommon.h"
#include "app.h"
#include  <string.h>
//#include "PrinterATcmd.h"
#include "MsgCANProcessAppExt.h"

////数据接收缓存//////////
#define UART4_RX_DATANUM 1000
#define UART4_RX_GROUPNUM 3
#define TASK_UART4_CNT 4 //4=40mS

#define RS485_Tx() GPIO_SetBits(GPIOA, GPIO_Pin_8)
#define RS485_Rx() GPIO_ResetBits(GPIOA, GPIO_Pin_8)

 /***************************************************************************************
*
*                            uart2
*
*****************************************************************************************/
typedef struct
{
  unsigned char Flag;
	unsigned int Lenth;
	unsigned char data[UART4_RX_DATANUM];
}Uart4_Data_Typedef;

Uart4_Data_Typedef Uart4RxBuf[UART4_RX_GROUPNUM];
static unsigned char Uart4_Rx_Cnt=0;
static unsigned char Uart4_Wr_Addr=0;
static unsigned char Uart4_Rd_Addr=0;
unsigned char Host_Interface_Mode=RS485_232_Mode;
Host_Data_Typedef Data_RecieveFromHost;
//打印机延时用；
//void delay_us(unsigned int n)
//{
//  unsigned  int j,g;
//  for(j=0;j<n;j++)
//    {
//      for(g=0;g<4;g++)  //1US			
//        {
//						__NOP();
//						__NOP();
//						__NOP();
//						__NOP();
//						__NOP();
//						__NOP();				
//        }	 
//    }
//}
//在周期函数中调用，对读取地址与写地址进行更新
void Uart4_TimCnt(void)
{
	if(Uart4_Rx_Cnt!=0)//接收到字节；
	{
			 Uart4_Rx_Cnt--;//减去1；
		if(Uart4_Rx_Cnt==0)//减到0；表示一帧接收完毕；
		{ 		
			Uart4RxBuf[Uart4_Wr_Addr].Flag=1;//读取一个帧成功完成；
			Uart4_Wr_Addr++;//存储下一帧的序号；
			if(Uart4_Wr_Addr>=UART4_RX_GROUPNUM)Uart4_Wr_Addr=0;//大于3个复位；
			if(Uart4_Rd_Addr==Uart4_Wr_Addr)
			{
				Uart4_Rd_Addr++;
				if(Uart4_Rd_Addr>=UART4_RX_GROUPNUM)Uart4_Rd_Addr=0;
			}
		}
	}
}
//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志 
		    Uart4_TimCnt(); 
		}
}

void Reset_Uart4RxBuf(void)
{
	unsigned char i;
	Uart4_Rd_Addr=0;
	Uart4_Wr_Addr=0;
	for(i=0;i<UART4_RX_GROUPNUM;i++)
	{
				Uart4RxBuf[i].Flag=0;
        Uart4RxBuf[i].Lenth=0;
        memset(Uart4RxBuf[i].data,0,UART4_RX_DATANUM);
	}
}

//在中断函数中调用，自动存储
void Uart4_RxISR(unsigned char buf)
{
#ifdef SYS_DYJ 
    Uart4_Rx_Cnt=Uart_DDS;
#else	
    Uart4_Rx_Cnt=TASK_UART4_CNT;//复位字节读取的延时时间；
#endif	
	
    Uart4RxBuf[Uart4_Wr_Addr].Flag=0;//清零；	
 if(Uart4RxBuf[Uart4_Wr_Addr].Lenth>=UART4_RX_DATANUM-1) //长度超出;
	  Uart4RxBuf[Uart4_Wr_Addr].Lenth=0;//复位； 
    Uart4RxBuf[Uart4_Wr_Addr].data[Uart4RxBuf[Uart4_Wr_Addr].Lenth++] = buf;//装填；
}

unsigned char UART4_Send_Data(unsigned char *Data,unsigned int Lenth)
{
	unsigned int i=0;

	for(i=0;i<Lenth;i++)
	{
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
		//发送的byte0为地址信息，位9为1；
		if(i==0) 
		{
			USART_SendData(USART3, Data[0]|0x100);
		}
		else USART_SendData(USART3, Data[i]);
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	}

	return 1;
}
//中断数据处理；
void YH_UART_IRQHandler(void)
{
	unsigned char data;
 
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		data=USART2->DR;
		Uart4_RxISR(data);
    }
}
//中断数据处理；
void YH_UART3_IRQHandler(void)
{
	unsigned char data;
 
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		data=USART3->DR;
		Uart4_RxISR(data);
    }
}
//数据读取；
unsigned int Uart4_Rx_Read(unsigned char *Rxdata)
{
	unsigned int tmp=0;
	if(Uart4RxBuf[Uart4_Rd_Addr].Flag==1)//如果接收buff里面有数据；
    {
		//读取数据
		tmp=Uart4RxBuf[Uart4_Rd_Addr].Lenth;//获取长度；
		memcpy(Rxdata,Uart4RxBuf[Uart4_Rd_Addr].data,Uart4RxBuf[Uart4_Rd_Addr].Lenth);//提取数据；
        Uart4RxBuf[Uart4_Rd_Addr].Flag=0;//清零；
        Uart4RxBuf[Uart4_Rd_Addr].Lenth=0;//清零；
        memset(Uart4RxBuf[Uart4_Rd_Addr].data,0,UART4_RX_DATANUM);//清零；
        Uart4_Rd_Addr++;//下次读取；
        if(Uart4_Rd_Addr>=UART4_RX_GROUPNUM)Uart4_Rd_Addr=0;//清零；
    }
	return tmp;
}
//485数据读取
unsigned int RS485_Read(unsigned char *Rxdata)
{
  unsigned int tmp;
	tmp=Uart4_Rx_Read(Rxdata);
	return tmp;
}
/***************************************************************************************
*
*                            CAN2
*
*****************************************************************************************/
typedef struct
{
  unsigned char Flag;
	unsigned int Lenth;
	unsigned char data[CAN_RX_DATANUM];
	unsigned int ID;	
}CAN_Data_Typedef;

#ifdef XYB_CXY
CanRxMsg RxMessage;
#else
extern CanRxMsg RxMessage;
#endif

static CAN_Data_Typedef CANRxBuf[CAN_RX_GROUPNUM];
static unsigned char CAN_Rx_Cnt=0;
static unsigned char CAN_Wr_Addr=0;
static unsigned char CAN_Rd_Addr=0;

static void Reset_CANRxBuf(void)
{
	unsigned char i;
	CAN_Rd_Addr=0;
	CAN_Wr_Addr=0;
	for(i=0;i<CAN_RX_GROUPNUM;i++)
	{
		CANRxBuf[i].Flag=0;
        CANRxBuf[i].Lenth=0;
        memset(CANRxBuf[i].data,0,CAN_RX_DATANUM);
	}
}

//can发送一组数据
#ifdef XYB_CXY
unsigned char CAN2_Send_One_Frame(unsigned char *msg, unsigned char len,unsigned int EID,uint32_t CAN_Id)
{
    unsigned char mbox;
    unsigned short int i = 0;
    CanTxMsg TxMessage;
    TxMessage.StdId = 0x00;		 // 标准标识符
	  TxMessage.ExtId = EID;		 // 设置扩展标示符
    TxMessage.IDE = CAN_Id;		   // 扩展帧 CAN_Id_Standard;标准帧 CAN_Id_Extended
    TxMessage.RTR = CAN_RTR_Data;		// 数据帧
    TxMessage.DLC = len;
    for(i = 0; i < len; i++)
	{
        TxMessage.Data[i] = msg[i];
	}
    mbox = CAN_Transmit(CAN1, &TxMessage);
		if(mbox>2)
		{
			CAN_DeInit(CAN1);
		  CAN1_Mode_Init(CAN1,gCANSystemParameter);   //CAN模式初始化使能
		  return 1;			
		}	
	
    i = 0;
    while((CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed) && (i < 0XFFF))i++;
    if(i >= 0XFFF)return 0;
    return 1;
}
#endif
//在中断函数中调用，自动存储
void CAN_RxISR(unsigned char *buf,unsigned char len,unsigned char id)
{
	unsigned char i=0;
	CANRxBuf[CAN_Wr_Addr].Lenth=len;
	CANRxBuf[CAN_Wr_Addr].ID=id;	
	for(i=0;i<len;i++)
	{
		CANRxBuf[CAN_Wr_Addr].data[i] = buf[i];
	}
    CANRxBuf[CAN_Wr_Addr].Flag=1;
	CAN_Wr_Addr++;
	if(CAN_Wr_Addr>=CAN_RX_GROUPNUM)CAN_Wr_Addr=0;
	if(CAN_Rd_Addr==CAN_Wr_Addr)
	{
		CAN_Rd_Addr++;
		if(CAN_Rd_Addr>=CAN_RX_GROUPNUM)CAN_Rd_Addr=0;
	}
}

extern unsigned int Inspect_TimCount;					 //巡检软件定时器计算
extern void CAN_RxISR_DYJ(unsigned char *buf,unsigned char len,unsigned int CAN_ID);
void YH_CAN_RX0_IRQHandler(void)
{
	
//#ifdef XYB_DYJ //打印机	
//	//	unsigned char i=0;
//	unsigned char lenth=0;
//	unsigned int  CAN_Id=0;
//    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

//	Inspect_TimCount=0;			//@接收到数据后，定时计数清0恢复初值
//	lenth = RxMessage.DLC;		//接收数据长度
//	CAN_Id =RxMessage.ExtId;	//接收ID号
//	CAN_RxISR_DYJ(RxMessage.Data,lenth,CAN_Id);
//#endif	
#ifdef XYB_CXY 	
	unsigned char i=0;
	unsigned char lenth=0;
	unsigned int id=0;	
	
  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

	lenth = RxMessage.DLC;
	id=RxMessage.ExtId;//1219
	CAN_RxISR(RxMessage.Data,lenth,id);
#endif	
}

////在周期函数中调用，对读取地址与写地址进行更新
//void CAN_TimCnt(void)
//{
//	if(CAN_Rx_Cnt!=0)
//	{
//		CAN_Rx_Cnt--;
//		if(CAN_Rx_Cnt==0)
//		{
//			CANRxBuf[CAN_Wr_Addr].Flag=1;
//			CAN_Wr_Addr++;
//			if(CAN_Wr_Addr>=CAN_RX_GROUPNUM)CAN_Wr_Addr=0;
//			if(CAN_Rd_Addr==CAN_Wr_Addr)
//			{
//				CAN_Rd_Addr++;
//				if(CAN_Rd_Addr>=CAN_RX_GROUPNUM)CAN_Rd_Addr=0;
//			}
//		}
//	}
//}

 
unsigned int CAN_Read(unsigned char *Rxdata)
{
	unsigned int tmp=0;
	unsigned char id=0;
	if(CANRxBuf[CAN_Rd_Addr].Flag==1)
    {
		//1219
		id=	CANRxBuf[CAN_Rd_Addr].ID >> 5;
		//读取数据
		tmp=CANRxBuf[CAN_Rd_Addr].Lenth;
		memcpy(Rxdata,CANRxBuf[CAN_Rd_Addr].data,CANRxBuf[CAN_Rd_Addr].Lenth);
		memcpy(Rxdata+tmp,&id,1);	//1219		  
			  CANRxBuf[CAN_Rd_Addr].Flag=0;
        CANRxBuf[CAN_Rd_Addr].Lenth=0;
        memset(CANRxBuf[CAN_Rd_Addr].data,0,CAN_RX_DATANUM);
        CAN_Rd_Addr++;
        if(CAN_Rd_Addr>=CAN_RX_GROUPNUM)CAN_Rd_Addr=0;
    }
	return tmp+1;//1219
}
 /***************************************************************************************
*
*                            RS485
*
*****************************************************************************************/
//PB1-RS485_SWITCH,低电平接收状态高电平发送状态；
void GPIO_RS485_Init(void)
{
	  //SWITCH
    GPIO_InitTypeDef GPIO_InitStructure;
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    RS485_Rx();
}

//判定总线是否忙；若一帧数据还没有结束（计数器不为0），
//则认为总线繁忙返回0，否则判定总线空闲返回1
unsigned char Read_Uart4State(void)
{
	if(Uart4_Rx_Cnt==0)return 1;
	else return 0;
}

//发送前判定总线是否忙，若忙则返回0，发送失败
//若返回1，发送成功；

unsigned char RS485_Send_Data(unsigned char *Data,unsigned int Lenth)
{
	if(Read_Uart4State()==1)
	{
		RS485_Tx();
		delay_ms(1);
		UART4_Send_Data(Data,Lenth);
		RS485_Rx();
		return 1;
	}
	return 0;
}

#endif
