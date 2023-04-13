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
* END_FILE_HDR*/

/* include files */
#include "MsgCANProcessDrv.h"
#include "includes.h"
#include "CAN1ProtocolParsing.h"

/************************************************************************************************
//										CAN1��ʼ��
************************************************************************************************/
CANParameterConfiguration  gCANSystemParameter =
							{
								0x00,	// CAN_SJW_1tq;
								0x02,	// CAN_BS2_2tq;
								0x01,	// CAN_BS1_3tq;
								599u,
								CAN_Mode_Normal     //ѡ������ģʽ�����߲���ģʽ
							};

//CANParameterConfiguration  gCANSystemParameter;    //CAN2���������ýṹ��




void CAN_GPIO_Init(void)			//CAN��IO��ʼ��
{
	GPIO_InitTypeDef 		GPIO_InitStructure;
	
    RCC_APB2PeriphClockCmd(CAN1_GPIO_RCC, ENABLE);		//ʹ��PORTAʱ��
    RCC_APB1PeriphClockCmd(CAN1_RCC, ENABLE);			//ʹ��CAN1ʱ��
	
    GPIO_InitStructure.GPIO_Pin = CAN1_GPIO_TX;   		//TX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//��������
    GPIO_Init(CAN1_GPIO, &GPIO_InitStructure);			//��ʼ��IO

    GPIO_InitStructure.GPIO_Pin = CAN1_GPIO_RX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		//��������
    GPIO_Init(CAN1_GPIO, &GPIO_InitStructure);			//��ʼ��IO	
}



void NVIC_CAN_Configuration(void)    //CAN1�жϳ�ʼ��ʹ������
{  
    NVIC_InitTypeDef NVIC_InitStructure;
        
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  
	//  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;  //@���CAN�����õ���PB8��PB9��Ҳ�������ض�������ţ������ж���CAN1_RX1_IRQn������PA11��PA12Ҳ��USB�����ţ�
																	//���Էǻ������Ҵ�CAN��������΢�������Ŀ��ļ���������ʱ����USB_LP_CAN1_RX0_IRQn����
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;				//@����õ�CAN������PA11��PA12�������ж���CAN1_RX0_IRQn
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // �����ȼ�Ϊ1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	
} 



void CAN1_Mode_Init(CAN_TypeDef* CANx,CANParameterConfiguration CANParameter)   //CANģʽ��ʼ��ʹ��
{
    CAN_InitTypeDef        	CAN_InitStructure;
    CAN_FilterInitTypeDef  	CAN_FilterInitStructure;     
//	NVIC_InitTypeDef NVIC_InitStructure;
	
	CAN_GPIO_Init();	
	  //CAN��Ԫ����
    CAN_InitStructure.CAN_TTCM = DISABLE;			//��ʱ�䴥��ͨ��ģʽ
    CAN_InitStructure.CAN_ABOM = DISABLE;			//����Զ����߹���
    CAN_InitStructure.CAN_AWUM = DISABLE;			//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
    CAN_InitStructure.CAN_NART = ENABLE;			//��ֹ�����Զ�����
    CAN_InitStructure.CAN_RFLM = DISABLE;		 	//���Ĳ�����,�µĸ��Ǿɵ�
    CAN_InitStructure.CAN_TXFP = DISABLE;			//���ȼ��ɱ��ı�ʶ������
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;	       //ģʽ���ã� mode:0,��ͨģʽ;1,�ػ�ģʽ;
    //���ò�����
//    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;				//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
//    CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq; 			//Tbs1=tbs1+1��ʱ�䵥λCAN_BS1_1tq ~CAN_BS1_16tq
//    CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;				//Tbs2=tbs2+1��ʱ�䵥λCAN_BS2_1tq ~	CAN_BS2_8tq
//    CAN_InitStructure.CAN_Prescaler = 600u - 1u;      //��Ƶϵ��(Fdiv)Ϊbrp+1
	    CAN_InitStructure.CAN_SJW = CANParameter.CANTsjw;				//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
		CAN_InitStructure.CAN_BS1 = CANParameter.CANTbs1; 			//Tbs1=tbs1+1��ʱ�䵥λCAN_BS1_1tq ~CAN_BS1_16tq
		CAN_InitStructure.CAN_BS2 = CANParameter.CANTbs2;				//Tbs2=tbs2+1��ʱ�䵥λCAN_BS2_1tq ~	CAN_BS2_8tq
		CAN_InitStructure.CAN_Prescaler = CANParameter.CANBrp;      //��Ƶϵ��(Fdiv)Ϊbrp+1
	
    CAN_Init(CAN1, &CAN_InitStructure);        	//��ʼ��CAN1

	//�˲�������
    CAN_FilterInitStructure.CAN_FilterNumber = 0;	//������0
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; 	//����λģʽ
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; 	//32λ��
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;	//32λID
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000; //32λMASK
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0; //������0������FIFO0
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; //���������0
    CAN_FilterInit(&CAN_FilterInitStructure);			//�˲�����ʼ��
    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);				//FIFO0��Ϣ�Һ��ж�����.

    NVIC_CAN_Configuration();		//�жϳ�ʼ��ʹ��
}



/******************
��������CAN_Send_One_Frame()
���ܣ�����һ֡����
���������	����1����Ҫ���͵����ݵ�ַ
			����2���������ݳ���
			����3��CAN��ID��
����ֵ�����ͳɹ�����0�����ͳ�ʱ����1
*******************/
static unsigned char CAN1_Send_One_Frame(unsigned char *msg, unsigned char len,unsigned int EID)
{ 
	 	OS_ERR err;
    CPU_SR_ALLOC();
	  unsigned char mbox;
    unsigned short int i = 0;
	
    CanTxMsg TxMessage;
    TxMessage.StdId = 0x00;		 		// ��׼��ʶ��
	  TxMessage.ExtId = EID;				// ������չ��ʾ��(ID��)
    TxMessage.IDE = CAN_Id_Extended;	// ��չ֡, CAN_Id_Standard��׼֡; CAN_Id_Extended��չ֡  ��չ��ʽ��ID0~ID17ΪExtension ID,��ID18~ID28ΪBase ID.
    TxMessage.RTR = CAN_RTR_Data;		// ����֡
    TxMessage.DLC = len;		  		// Ҫ���͵����ݳ���
    for(i = 0; i < len; i++)
	  {
        TxMessage.Data[i] = msg[i];
	  }
	  
    mbox = CAN_Transmit(CAN1, &TxMessage);

		//CAN���ͣ���CAN������
		if(mbox>2)
		{
			//FOR TEST
			printf("CAN1�쳣������\n\r");	
			OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ10ms
		  CAN_DeInit(CAN1);
		  CAN1_Mode_Init(CAN1,gCANSystemParameter);   //CANģʽ��ʼ��ʹ��	
		  return 1;			
		}
		
    i = 0;
    while((CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed) && (i < 0XFFF))
		i++;	//�ȴ����ͽ���
    if(i >= 0XFFF)
		{
			return 1;
		}
		
    return 0;
}






/**********************************************************************************************************
*	�� �� ��: CAN_Send_Frame()
*	����˵��: CAN1�������ͺ���
*	��    �Σ�	����1����Ҫ���͵����ݵ�ַ
				����2���������ݳ���
				����3��Э�����
*	�� �� ֵ: 0,�ɹ�;1��ʧ��
**********************************************************************************************************/
unsigned char CAN1_Send_Frame(unsigned char *msg, unsigned int len,unsigned char Board_Number)     //CAN1�������ͺ���
{
	OS_ERR  err;
	u8 i = 0;
	u8 j = 0;
	u8	Flag = 0;
	u8	SendData[8];
	u8 Pack_sum=0;
	unsigned int Id=0;
	
	//�����ܰ���
	if(len%8==0)
		Pack_sum=len/8;
	else
		Pack_sum=(len/8) + 1;
	//��װID��
		Id = Id|(Board_Number<<24);
		Id = Id|(Pack_sum<<8);
	//�û���Ϣ�������ݣ���28λΪ1
	Id=Id|(0x01<<28);
	
	while(len--)
	{
		SendData[i++] = msg[j++];
		if(len == 0)
		{
			Id++;
			Flag = CAN1_Send_One_Frame(SendData , i , Id);
			OSTimeDly (20, OS_OPT_TIME_DLY, & err );//200mS
			return 0;
		}
		if(i == 8)
		{
			Id++;
			Flag = CAN1_Send_One_Frame(SendData , i , Id);
			OSTimeDly (20, OS_OPT_TIME_DLY, & err );
			i = 0;
		}
	}
	return 1;
}





/***************************************************************************************
*
*                            CAN1��ȡ���� 
*
*****************************************************************************************/

#ifdef XYB_DYJ
CanRxMsg RxMessage;
#else
extern CanRxMsg RxMessage;
#endif

static CAN_Data_Typedef CANRxBuf[CAN_RX_GROUPNUM];
static unsigned char CAN_Rx_Cnt=0;
static unsigned char CAN_Wr_Addr=0;
static unsigned char CAN_Rd_Addr=0;


static void Reset_CANRxBuf(void)    //��λCAN��������
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

//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)
#ifdef XYB_DYJ
unsigned char Can_Send_Msg(unsigned char *msg, unsigned char len)
{
    unsigned char mbox;
    unsigned short int i = 0;
    CanTxMsg TxMessage;
    TxMessage.StdId = 0x00;
    TxMessage.ExtId = 0x1E000000;
    TxMessage.IDE = CAN_Id_Extended;	// ��չ֡ CAN_Id_Standard;��׼֡
    TxMessage.RTR = CAN_RTR_Data;		// ����֡
    TxMessage.DLC = len;
    for(i = 0; i < len; i++)
	{
        TxMessage.Data[i] = msg[i];
	}
    mbox = CAN_Transmit(CAN1, &TxMessage);
    i = 0;
    while((CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed) && (i < 0XFFF))i++;
    if(i >= 0XFFF)return 0;
    return 1;
}
#endif



/******************
��������	CAN_RxISR
���ܣ�		CAN�ж�ת�����ݣ��Զ��洢��
�������1��	���յ�������ָ��
	����2��	���յ������ݳ���
	����3��	���յ�������ID
����ֵ��		��
*******************/
static unsigned char CAN_Temporary_buf[1024];		//���建������
static unsigned int CAN_Temporary_buf_Count=0;		//���建������
//void CAN_RxISR(unsigned char *buf,unsigned char len,unsigned int CAN_ID)
void CAN_RxISR_DYJ(unsigned char *buf,unsigned char len,unsigned int CAN_ID)
{

 
	unsigned char i=0;
	unsigned char Now_Frame_Cnt=0;						//��ǰ֡
	unsigned char Last_Frame_Cnt=0;						//Ŀ��֡

	Now_Frame_Cnt=(unsigned char)CAN_ID;  				//ȡ0-7λ
	Last_Frame_Cnt =(unsigned char)((CAN_ID>>8)&0x000000ff); //ȡ8-15λ
	
	if(Now_Frame_Cnt<Last_Frame_Cnt)					//��ǰ֡û�н�����
	{	
		for(i=0;i<len;i++)
		{
			CAN_Temporary_buf[CAN_Temporary_buf_Count++] = buf[i];
		}
	}
	else if(Now_Frame_Cnt==Last_Frame_Cnt)		//��֡���һ������
	{
		
		for(i=0;i<len;i++)
		{
			CAN_Temporary_buf[CAN_Temporary_buf_Count++] = buf[i];
		}
		//������
		CANRxBuf[CAN_Wr_Addr].ID_Number = (unsigned char)((CAN_ID>>24)&0x0f);			//��ǰ֡���հ���
		CANRxBuf[CAN_Wr_Addr].Lenth=CAN_Temporary_buf_Count;   //����
		memcpy(CANRxBuf[CAN_Wr_Addr].data,CAN_Temporary_buf,CAN_Temporary_buf_Count);  //ת������
		CANRxBuf[CAN_Wr_Addr].Flag=1;
		CAN_Wr_Addr++;
		if(CAN_Wr_Addr>=CAN_RX_GROUPNUM)CAN_Wr_Addr=0;
		if(CAN_Rd_Addr==CAN_Wr_Addr)   //@�����д��ȣ�˵��д�ȶ�����һ��ѭ�����˴��벻�ܱ�֤������ȫ����ʧ��ֻ�ܱ�֤������յ�n-1֡������ȷ
		{
			CAN_Rd_Addr++;
			if(CAN_Rd_Addr>=CAN_RX_GROUPNUM)CAN_Rd_Addr=0;
		}
		//������0
		CAN_Temporary_buf_Count=0;   	//����ת�����0
		memset(CAN_Temporary_buf,0,256);
	}
}






/******************
��������	YH_CAN1_RX0_IRQHandler
���ܣ�		CAN�жϽ����û���Ϣ���ݺ���
���������	��
����ֵ��		��
*******************/
void YH_CAN1_RX0_IRQHandler(void)		//@�жϽ������ݣ���CAN�жϺ���ʹ��
{
//	unsigned char i=0;
	unsigned char lenth=0;
	unsigned int  CAN_Id=0;
  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

	Inspect_TimCount=0;			//@���յ����ݺ󣬶�ʱ������0�ָ���ֵ
	lenth = RxMessage.DLC;		//�������ݳ���
	CAN_Id =RxMessage.ExtId;	//����ID��
	CAN_RxISR_DYJ(RxMessage.Data,lenth,CAN_Id);
	
}
#ifdef XYB_DYJ
void CAN1_RX0_IRQHandler(void)
{
	YH_CAN1_RX0_IRQHandler();
}
#endif
//�����ں����е��ã��Զ�ȡ��ַ��д��ַ���и���
//Void CAN_TimCnt(void)
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

 
/******************
��������	YH_CAN1_RX0_IRQHandler
���ܣ�		CAN�ж�ѭ�����ж�����
���������	CAN��������ָ��
����ֵ��		����CAN����֡���ݳ���
*******************/
//unsigned int CAN1_Read(unsigned char *Rxdata);   //CAN1��ȡ����
void  CAN1_Read(CAN_Data_Typedef *S_Rxdata)   //CAN1��ȡ����
{
	if(CANRxBuf[CAN_Rd_Addr].Flag==1)
    {
		//��ȡ����
		S_Rxdata->Lenth=CANRxBuf[CAN_Rd_Addr].Lenth;
		S_Rxdata->ID_Number=CANRxBuf[CAN_Rd_Addr].ID_Number;
		memcpy(&S_Rxdata->data[0],CANRxBuf[CAN_Rd_Addr].data,CANRxBuf[CAN_Rd_Addr].Lenth);				
        CANRxBuf[CAN_Rd_Addr].Flag=0;
        CANRxBuf[CAN_Rd_Addr].Lenth=0;
        memset(CANRxBuf[CAN_Rd_Addr].data,0,CAN_RX_DATANUM);
        CAN_Rd_Addr++;
        if(CAN_Rd_Addr>=CAN_RX_GROUPNUM)CAN_Rd_Addr=0;
    }
   
}