#ifndef  REVEIVEDATABUFFER_C
#define  REVEIVEDATABUFFER_C
//����uart2�������ݣ�
#include "ReceiveDataBuffer.h"
#include "DeviceCommon.h"
#include "app.h"
#include  <string.h>
//#include "PrinterATcmd.h"
#include "MsgCANProcessAppExt.h"

////���ݽ��ջ���//////////
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
//��ӡ����ʱ�ã�
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
//�����ں����е��ã��Զ�ȡ��ַ��д��ַ���и���
void Uart4_TimCnt(void)
{
	if(Uart4_Rx_Cnt!=0)//���յ��ֽڣ�
	{
			 Uart4_Rx_Cnt--;//��ȥ1��
		if(Uart4_Rx_Cnt==0)//����0����ʾһ֡������ϣ�
		{ 		
			Uart4RxBuf[Uart4_Wr_Addr].Flag=1;//��ȡһ��֡�ɹ���ɣ�
			Uart4_Wr_Addr++;//�洢��һ֡����ţ�
			if(Uart4_Wr_Addr>=UART4_RX_GROUPNUM)Uart4_Wr_Addr=0;//����3����λ��
			if(Uart4_Rd_Addr==Uart4_Wr_Addr)
			{
				Uart4_Rd_Addr++;
				if(Uart4_Rd_Addr>=UART4_RX_GROUPNUM)Uart4_Rd_Addr=0;
			}
		}
	}
}
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
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

//���жϺ����е��ã��Զ��洢
void Uart4_RxISR(unsigned char buf)
{
#ifdef SYS_DYJ 
    Uart4_Rx_Cnt=Uart_DDS;
#else	
    Uart4_Rx_Cnt=TASK_UART4_CNT;//��λ�ֽڶ�ȡ����ʱʱ�䣻
#endif	
	
    Uart4RxBuf[Uart4_Wr_Addr].Flag=0;//���㣻	
 if(Uart4RxBuf[Uart4_Wr_Addr].Lenth>=UART4_RX_DATANUM-1) //���ȳ���;
	  Uart4RxBuf[Uart4_Wr_Addr].Lenth=0;//��λ�� 
    Uart4RxBuf[Uart4_Wr_Addr].data[Uart4RxBuf[Uart4_Wr_Addr].Lenth++] = buf;//װ�
}

unsigned char UART4_Send_Data(unsigned char *Data,unsigned int Lenth)
{
	unsigned int i=0;

	for(i=0;i<Lenth;i++)
	{
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
		//���͵�byte0Ϊ��ַ��Ϣ��λ9Ϊ1��
		if(i==0) 
		{
			USART_SendData(USART3, Data[0]|0x100);
		}
		else USART_SendData(USART3, Data[i]);
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	}

	return 1;
}
//�ж����ݴ���
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
//�ж����ݴ���
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
//���ݶ�ȡ��
unsigned int Uart4_Rx_Read(unsigned char *Rxdata)
{
	unsigned int tmp=0;
	if(Uart4RxBuf[Uart4_Rd_Addr].Flag==1)//�������buff���������ݣ�
    {
		//��ȡ����
		tmp=Uart4RxBuf[Uart4_Rd_Addr].Lenth;//��ȡ���ȣ�
		memcpy(Rxdata,Uart4RxBuf[Uart4_Rd_Addr].data,Uart4RxBuf[Uart4_Rd_Addr].Lenth);//��ȡ���ݣ�
        Uart4RxBuf[Uart4_Rd_Addr].Flag=0;//���㣻
        Uart4RxBuf[Uart4_Rd_Addr].Lenth=0;//���㣻
        memset(Uart4RxBuf[Uart4_Rd_Addr].data,0,UART4_RX_DATANUM);//���㣻
        Uart4_Rd_Addr++;//�´ζ�ȡ��
        if(Uart4_Rd_Addr>=UART4_RX_GROUPNUM)Uart4_Rd_Addr=0;//���㣻
    }
	return tmp;
}
//485���ݶ�ȡ
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

//can����һ������
#ifdef XYB_CXY
unsigned char CAN2_Send_One_Frame(unsigned char *msg, unsigned char len,unsigned int EID,uint32_t CAN_Id)
{
    unsigned char mbox;
    unsigned short int i = 0;
    CanTxMsg TxMessage;
    TxMessage.StdId = 0x00;		 // ��׼��ʶ��
	  TxMessage.ExtId = EID;		 // ������չ��ʾ��
    TxMessage.IDE = CAN_Id;		   // ��չ֡ CAN_Id_Standard;��׼֡ CAN_Id_Extended
    TxMessage.RTR = CAN_RTR_Data;		// ����֡
    TxMessage.DLC = len;
    for(i = 0; i < len; i++)
	{
        TxMessage.Data[i] = msg[i];
	}
    mbox = CAN_Transmit(CAN1, &TxMessage);
		if(mbox>2)
		{
			CAN_DeInit(CAN1);
		  CAN1_Mode_Init(CAN1,gCANSystemParameter);   //CANģʽ��ʼ��ʹ��
		  return 1;			
		}	
	
    i = 0;
    while((CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed) && (i < 0XFFF))i++;
    if(i >= 0XFFF)return 0;
    return 1;
}
#endif
//���жϺ����е��ã��Զ��洢
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

extern unsigned int Inspect_TimCount;					 //Ѳ�������ʱ������
extern void CAN_RxISR_DYJ(unsigned char *buf,unsigned char len,unsigned int CAN_ID);
void YH_CAN_RX0_IRQHandler(void)
{
	
//#ifdef XYB_DYJ //��ӡ��	
//	//	unsigned char i=0;
//	unsigned char lenth=0;
//	unsigned int  CAN_Id=0;
//    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

//	Inspect_TimCount=0;			//@���յ����ݺ󣬶�ʱ������0�ָ���ֵ
//	lenth = RxMessage.DLC;		//�������ݳ���
//	CAN_Id =RxMessage.ExtId;	//����ID��
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

////�����ں����е��ã��Զ�ȡ��ַ��д��ַ���и���
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
		//��ȡ����
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
//PB1-RS485_SWITCH,�͵�ƽ����״̬�ߵ�ƽ����״̬��
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

//�ж������Ƿ�æ����һ֡���ݻ�û�н�������������Ϊ0����
//����Ϊ���߷�æ����0�������ж����߿��з���1
unsigned char Read_Uart4State(void)
{
	if(Uart4_Rx_Cnt==0)return 1;
	else return 0;
}

//����ǰ�ж������Ƿ�æ����æ�򷵻�0������ʧ��
//������1�����ͳɹ���

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
