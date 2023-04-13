#include "ProtocolBoard.h"
#include "MsgSPIProcessAppExt.h"
//#include "rtc.h"
#include "string.h"
#include "frame.h"
#include "stdio.h"
#include "includes.h"
#include "lcd.h"	
#include "SDcontrol.h"
#include "UsartMsgProcessDrv.h"
#include "usart.h"

//#define BUFFERSIZE 1000
#define BUFFERSIZE 100
#define SPI2_CS_LO()	GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define SPI2_CS_HI()	GPIO_SetBits(GPIOB,GPIO_Pin_12)
#define ReadSPI2_CS() GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)

extern unsigned char g_rxdata[SERVER_DATA_NUM];
extern unsigned int g_cnt;
const unsigned char PollongStr[40]={0x68,0x68,0x01,0};
u8 SPI2_buffer_TX[BUFFERSIZE]={0};
u8 SPI_buffer_RX[BUFFERSIZE]={0};

u8 UART1_buffer_TX[BUFFERSIZE]={0};

static unsigned char FrameCnt=0;
static unsigned char FrameCntP=0;
//unsigned char BK_Message[17][100];//板卡信息存储；
//unsigned char BK_Message[100];//板卡信息存储；
//unsigned char PreBK_Message[100];//板卡信息存储；
void ProtocolBoardInterfaceInit(void)
{
	SPI_ConfigarationInit(SPIxRxen);
	TIM5_Init(10,7200);
}
unsigned char SPI_WriteByte(u8 byte)
{
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI2, byte);
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	return SPI_I2S_ReceiveData(SPI2);
}
unsigned char SPI_WriteFrame(unsigned char *txdata,unsigned char *rxdata,unsigned int lenth)
{
	OS_ERR err;
	unsigned char i=15;
	while((ReadSPI2_CS()==0)&&(i!=0))
	{
		i--;
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_HMSM_STRICT,&err);
	}
	if(i!=0)
	{
		SPI_ConfigarationInit(SPIxTxen);
		SPI2_CS_LO();
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_HMSM_STRICT,&err);
		for(i=0;i<lenth;i++)
		{
			rxdata[i]=SPI_WriteByte(txdata[i]);
		}
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_HMSM_STRICT,&err);
		SPI2_CS_HI();
		SPI_ConfigarationInit(SPIxRxen);
	}
	return 0;
}
void PollingFrameSendToProtocolBoard(void)
{
	unsigned char buf[7];
	unsigned char i=0;
	RTC_Get((char *)buf);
//		dwin_time_read((char *)buf);							
	memset(SPI2_buffer_TX,0,BUFFERSIZE);
	SPI2_buffer_TX[0]=0x68;
	SPI2_buffer_TX[1]=0x68;
	SPI2_buffer_TX[2]=0x1;
	SPI2_buffer_TX[3]=FrameCnt++;
	SPI2_buffer_TX[4]=0x12;
	SPI2_buffer_TX[5]=0x0;
	
	memcpy(&SPI2_buffer_TX[10],buf,6);
	SPI2_buffer_TX[16]=0;//20180327
	for(i=2;i<16;i++)
	{
		SPI2_buffer_TX[16]+=SPI2_buffer_TX[i];
	}
	SPI2_buffer_TX[17]=0x23;
	
	memset(SPI_buffer_RX,0,sizeof(SPI_buffer_RX));
	SPI_WriteFrame(SPI2_buffer_TX,SPI_buffer_RX,22);
}
void PrePollingFrameSendToProtocolBoard(void)
{
	unsigned char buf[7];
	unsigned char i=0;
	RTC_Get((char *)buf);
						
	UART1_buffer_TX[0]=0x68;
	UART1_buffer_TX[1]=0x68;
	UART1_buffer_TX[2]=0x1;
	UART1_buffer_TX[3]=FrameCntP++;
	UART1_buffer_TX[4]=0x12;
	UART1_buffer_TX[5]=0x0;
	
	memcpy(&UART1_buffer_TX[10],buf,6);
	UART1_buffer_TX[16]=0;
	for(i=2;i<16;i++)
	{
		UART1_buffer_TX[16]+=UART1_buffer_TX[i];
	}
	UART1_buffer_TX[17]=0x23;
	UARTx_Send_Data(USART1,UART1_buffer_TX,18);
}
void ConfirmFrameSendToProtocolBoard(void)
{
	unsigned char buf[7];
	unsigned char i=0;
	RTC_Get((char *)buf);
//	dwin_time_read((char *)buf);		
//					
	memset(SPI2_buffer_TX,0,BUFFERSIZE);
	SPI2_buffer_TX[0]=0x68;
	SPI2_buffer_TX[1]=0x68;
	SPI2_buffer_TX[2]=0x7;
	SPI2_buffer_TX[3]=FrameCnt++;
	SPI2_buffer_TX[4]=0x12;
	SPI2_buffer_TX[5]=0x0;
	
	memcpy(&SPI2_buffer_TX[10],buf,6);
	for(i=2;i<16;i++)
	{
		SPI2_buffer_TX[16]+=SPI2_buffer_TX[i];
	}
	SPI2_buffer_TX[17]=0x23;
	
	memset(SPI_buffer_RX,0,sizeof(SPI_buffer_RX));
	SPI_WriteFrame(SPI2_buffer_TX,SPI_buffer_RX,22);
}

void PreConfirmFrameSendToProtocolBoard(void)
{
	unsigned char buf[7];
	unsigned char i=0;
	RTC_Get((char *)buf);		

	UART1_buffer_TX[0]=0x68;
	UART1_buffer_TX[1]=0x68;
	UART1_buffer_TX[2]=0x7;
	UART1_buffer_TX[3]=FrameCnt++;
	UART1_buffer_TX[4]=0x12;
	UART1_buffer_TX[5]=0x0;
	
	memcpy(&UART1_buffer_TX[10],buf,6);
	for(i=2;i<16;i++)
	{
		UART1_buffer_TX[16]+=UART1_buffer_TX[i];
	}
	UART1_buffer_TX[17]=0x23;
	UARTx_Send_Data(USART1,UART1_buffer_TX,18);
}
void RequestFrameSendToProtocolBoard(unsigned int len)
{
	unsigned char buf[7];
	unsigned int i=0;
	if((len<BUFFERSIZE)&&(len>18))
	{
		RTC_Get((char *)buf);
		memset(SPI2_buffer_TX,0,BUFFERSIZE);
		SPI2_buffer_TX[0]=0x68;
		SPI2_buffer_TX[1]=0x68;
		SPI2_buffer_TX[2]=0x5;
		SPI2_buffer_TX[3]=FrameCnt++;
		SPI2_buffer_TX[4]=len;
		SPI2_buffer_TX[5]=0x0;
		memcpy(&SPI2_buffer_TX[len-8],buf,6);
		for(i=2;i<len-2;i++)
		{
			SPI2_buffer_TX[len-2]+=SPI2_buffer_TX[i];
		}
		SPI2_buffer_TX[len-1]=0x23;
		
		memset(SPI_buffer_RX,0,sizeof(SPI_buffer_RX));
		SPI_WriteFrame(SPI2_buffer_TX,SPI_buffer_RX,len+4);
	}
}
void PreRequestFrameSendToProtocolBoard(unsigned int len)
{
	unsigned char buf[7];
	unsigned int i=0;
	if((len<BUFFERSIZE)&&(len>18))
	{
		RTC_Get((char *)buf);
//		dwin_time_read((char *)buf);
		memset(SPI2_buffer_TX,0,BUFFERSIZE);
		SPI2_buffer_TX[0]=0x68;
		SPI2_buffer_TX[1]=0x68;
		SPI2_buffer_TX[2]=0x5;
		SPI2_buffer_TX[3]=FrameCnt++;
		SPI2_buffer_TX[4]=len;
		SPI2_buffer_TX[5]=0x0;
		memcpy(&SPI2_buffer_TX[len-8],buf,6);
		for(i=2;i<len-2;i++)
		{
			SPI2_buffer_TX[len-2]+=SPI2_buffer_TX[i];
		}
		SPI2_buffer_TX[len-1]=0x23;
		
		memset(SPI_buffer_RX,0,sizeof(SPI_buffer_RX));
		SPI_WriteFrame(SPI2_buffer_TX,SPI_buffer_RX,len+4);
	}
}
unsigned char DataRecieveFromeProtocolBoard_Integrity(unsigned char *data)
{
	unsigned int len=0;
	unsigned int i=0;
	unsigned char checksum=0;
	len=data[4]+data[5]*256;
	if((len<BUFFERSIZE)&&(len>=18))
	{
		for(i=2;i<len-2;i++)
		{
			checksum+=data[i];
		}
		if(len>20)
		{
			len=len;
			i=0;
		}
		if((data[0]==0x68)&&(data[1]==0x68)&&(data[len-1]==0x23)&&(checksum==data[len-2]))//
		{
			
			return 1;
		}
	}
	return 0;
}
unsigned char PreDataRecieveFromeProtocolBoard_Integrity(unsigned char *data,unsigned char length)
{
	unsigned int len=0;
	unsigned int i=0;
	unsigned char checksum=0;
	if(length>17)
	{
	len=data[4]+data[5]*256;
	if((len<BUFFERSIZE)&&(len>=18))
	{
		for(i=2;i<len-2;i++)
		{
			checksum+=data[i];
		}
		if(len>20)
		{
			len=len;
			i=0;
		}
		if((data[0]==0x68)&&(data[1]==0x68)&&(data[len-1]==0x23)&&(checksum==data[len-2]))//
		{
			
			return 1;
		}
	}
	}
	return 0;
}
//接收数据命令错误0
//巡检帧的后续数据传输1
//数据帧的后续数据传输0x11
//数据错误重发2
//本帧结束3
//本帧结束13
//线路故障4
unsigned char DataRecieveFromeProtocolBoard_Handle(unsigned char *data,unsigned int *plenth)
{
	static unsigned char destdata[SERVER_DATA_NUM];
	unsigned int len=0;
	unsigned int tmp=0;
	len=data[4]+data[5]*256;
	//巡检帧
	if((data[2]==0x13)&&(len==18))
	{
		tmp=data[6]+data[7]*256;
		if((0x0001==tmp)||(0x0002==tmp)||(0x0004==tmp))
		{
			*plenth=data[8]+data[9]*256;
			if(*plenth>BUFFERSIZE)
			{
				*plenth=0;
				return 0;
			}
			return 1;
		}
		else if(0x01ff==tmp)
		{
			return 0x2;
		}
		else if(data[10]!=0)//线路故障
		{
			return 4;
		}
		else {return 3;}
	}
	//数据帧
	else if(data[2]==0x15)
	{
//		ItemAnalyse(&data[16]);//判断是否解析编码表，编码表解析；
		ReportToServer(0x02,&data[16],len-18,destdata);
//		tmp=data[6]+data[7]*256;
//		if((0x0001==tmp)||(0x0002==tmp)||(0x0004==tmp))
//		{
//			*plenth=data[8]+data[9]*256;
//			if(*plenth>BUFFERSIZE)
//			{
//				*plenth=0;
//				return 0;
//			}
//			return 0x11;
//		}
//		else 
		if(0x01ff==tmp)
		{
			return 2;
		}
		else {return 0x13;}
	}
	//外接板卡信息帧；
	else if(data[2]==0x19)
	{	
//	  memcpy(&BK_Message,&data[16],100);
	}
	return 0;
}

//接收数据命令错误0
//巡检帧的后续数据传输1
//数据帧的后续数据传输0x11
//数据错误重发2
//本帧结束3
//本帧结束13
//线路故障4
unsigned char PreDataRecieveFromeProtocolBoard_Handle(unsigned char *data,unsigned int *plenth)
{
	static unsigned char destdata[SERVER_DATA_NUM];
	unsigned int len=0;
	unsigned int tmp=0;
	len=data[4]+data[5]*256;
	//巡检帧
	if((data[2]==0x13)&&(len==18))
	{
		tmp=data[6]+data[7]*256;
		if((0x0001==tmp)||(0x0002==tmp)||(0x0004==tmp))
		{
			*plenth=data[8]+data[9]*256;
			if(*plenth>BUFFERSIZE)
			{
				*plenth=0;
				return 0;
			}
			return 1;
		}
		else if(0x01ff==tmp)
		{
			return 0x2;
		}
		else if(data[10]!=0)//线路故障
		{
			return 4;
		}
		else {return 3;}
	}
	//数据帧
	else if(data[2]==0x15)
	{
//		ItemAnalyse(&data[16]);//判断是否解析编码表，编码表解析；
		ReportToServer(0x02,&data[16],len-18,destdata);
//		tmp=data[6]+data[7]*256;
//		if((0x0001==tmp)||(0x0002==tmp)||(0x0004==tmp))
//		{
//			*plenth=data[8]+data[9]*256;
//			if(*plenth>BUFFERSIZE)
//			{
//				*plenth=0;
//				return 0;
//			}
//			return 0x11;
//		}
//		else 
		if(0x01ff==tmp)
		{
			return 2;
		}
		else {return 0x13;}
	}
	//外接板卡信息帧；
	else if(data[2]==0x19)
	{	
//	  memcpy(&PreBK_Message,&data[16],100);
	}
	return 0;
}
//0接收数据错误
//1本帧结束数据接收正确
//4线路故障(若连续30s无应答，则返回线路故障)
unsigned char ProtocolBoardHandle(void)
{
	unsigned char flag=0;
	unsigned int lenth=0;
	static unsigned int PollingNoReplyCnt=0;
	static unsigned int FaultCnt=0;
	
	if(g_cnt==0)
	{
		PollingFrameSendToProtocolBoard();
		PollingNoReplyCnt++;
	}
	else// if(g_cnt>=18)
	{
		flag=DataRecieveFromeProtocolBoard_Integrity(g_rxdata);
		if(flag==1)
		{
			PollingNoReplyCnt=0;
			lenth=0;
			flag=DataRecieveFromeProtocolBoard_Handle(g_rxdata,&lenth);
			memset(g_rxdata,0,SERVER_DATA_NUM);
			g_cnt=0;
			if((flag==0x11)||(flag==0x13))
			{
				FaultCnt=0;
				ConfirmFrameSendToProtocolBoard();
			}
//			else if(flag==1)
//			{
//				RequestFrameSendToProtocolBoard(lenth);
//			}
//			else if(flag==2)
//			{
//				PollingFrameSendToProtocolBoard();
//			}
			if(flag==4)
			{
				FaultCnt++;
				if(FaultCnt>30)
				{
					FaultCnt=31;
					return 4;//线路故障
				}
			}
			else if(flag==0)
			{
				return 0;
			}
			return 1;//线路正常
		}else
		{
			memset(g_rxdata,0,SERVER_DATA_NUM);
			g_cnt=0;
			return 0;
		}
	}
	if(PollingNoReplyCnt>30)
	{
		PollingNoReplyCnt=31;
		return 4;
	}
	return 0;
}
//从协议板解析；
unsigned char PreProtocolBoardHandle(void)
{
	unsigned char flag=0;
	OS_ERR err;

	PrePollingFrameSendToProtocolBoard();//发送巡检；
	OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
	if(USART1_RX_Cont>0)//有数据
	{
		USART1_RX_EN=0;	//禁止接收；
		flag=PreDataRecieveFromeProtocolBoard_Integrity(USART1_RX_BUF,USART1_RX_Cont);//判断数据正确性；
		if(flag==1)//数据接收正确；
		{
			flag=PreDataRecieveFromeProtocolBoard_Handle(USART1_RX_BUF,&USART1_RX_Cont);
			if((flag==0x11)||(flag==0x13))
			{
				OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
				PreConfirmFrameSendToProtocolBoard();//接收确认帧发送
			}
			}
			USART1_RX_Cont=0;//清零；
			USART1_RX_EN=1;//使能接收；
	}
	return 0;
}



