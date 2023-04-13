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
//										CAN1初始化
************************************************************************************************/
CANParameterConfiguration  gCANSystemParameter =
							{
								0x00,	// CAN_SJW_1tq;
								0x02,	// CAN_BS2_2tq;
								0x01,	// CAN_BS1_3tq;
								599u,
								CAN_Mode_Normal     //选择正常模式，或者测试模式
							};

//CANParameterConfiguration  gCANSystemParameter;    //CAN2波特率设置结构体




void CAN_GPIO_Init(void)			//CAN的IO初始化
{
	GPIO_InitTypeDef 		GPIO_InitStructure;
	
    RCC_APB2PeriphClockCmd(CAN1_GPIO_RCC, ENABLE);		//使能PORTA时钟
    RCC_APB1PeriphClockCmd(CAN1_RCC, ENABLE);			//使能CAN1时钟
	
    GPIO_InitStructure.GPIO_Pin = CAN1_GPIO_TX;   		//TX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//复用推挽
    GPIO_Init(CAN1_GPIO, &GPIO_InitStructure);			//初始化IO

    GPIO_InitStructure.GPIO_Pin = CAN1_GPIO_RX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		//上拉输入
    GPIO_Init(CAN1_GPIO, &GPIO_InitStructure);			//初始化IO	
}



void NVIC_CAN_Configuration(void)    //CAN1中断初始化使能配置
{  
    NVIC_InitTypeDef NVIC_InitStructure;
        
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  
	//  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;  //@如果CAN引脚用的是PB8和PB9，也就是用重定义的引脚，接收中断用CAN1_RX1_IRQn。由于PA11和PA12也是USB的引脚，
																	//所以非互联型且带CAN控制器的微控制器的库文件在起名字时用了USB_LP_CAN1_RX0_IRQn。”
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;				//@如果用的CAN引脚是PA11和PA12，接收中断用CAN1_RX0_IRQn
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	
} 



void CAN1_Mode_Init(CAN_TypeDef* CANx,CANParameterConfiguration CANParameter)   //CAN模式初始化使能
{
    CAN_InitTypeDef        	CAN_InitStructure;
    CAN_FilterInitTypeDef  	CAN_FilterInitStructure;     
//	NVIC_InitTypeDef NVIC_InitStructure;
	
	CAN_GPIO_Init();	
	  //CAN单元设置
    CAN_InitStructure.CAN_TTCM = DISABLE;			//非时间触发通信模式
    CAN_InitStructure.CAN_ABOM = DISABLE;			//软件自动离线管理
    CAN_InitStructure.CAN_AWUM = DISABLE;			//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
    CAN_InitStructure.CAN_NART = ENABLE;			//禁止报文自动传送
    CAN_InitStructure.CAN_RFLM = DISABLE;		 	//报文不锁定,新的覆盖旧的
    CAN_InitStructure.CAN_TXFP = DISABLE;			//优先级由报文标识符决定
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;	       //模式设置： mode:0,普通模式;1,回环模式;
    //设置波特率
//    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;				//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
//    CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq; 			//Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~CAN_BS1_16tq
//    CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;				//Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~	CAN_BS2_8tq
//    CAN_InitStructure.CAN_Prescaler = 600u - 1u;      //分频系数(Fdiv)为brp+1
	    CAN_InitStructure.CAN_SJW = CANParameter.CANTsjw;				//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
		CAN_InitStructure.CAN_BS1 = CANParameter.CANTbs1; 			//Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~CAN_BS1_16tq
		CAN_InitStructure.CAN_BS2 = CANParameter.CANTbs2;				//Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~	CAN_BS2_8tq
		CAN_InitStructure.CAN_Prescaler = CANParameter.CANBrp;      //分频系数(Fdiv)为brp+1
	
    CAN_Init(CAN1, &CAN_InitStructure);        	//初始化CAN1

	//滤波器配置
    CAN_FilterInitStructure.CAN_FilterNumber = 0;	//过滤器0
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; 	//屏蔽位模式
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; 	//32位宽
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;	//32位ID
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000; //32位MASK
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0; //过滤器0关联到FIFO0
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; //激活过滤器0
    CAN_FilterInit(&CAN_FilterInitStructure);			//滤波器初始化
    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);				//FIFO0消息挂号中断允许.

    NVIC_CAN_Configuration();		//中断初始化使能
}



/******************
函数名：CAN_Send_One_Frame()
功能：发送一帧报文
输入参数：	参数1：需要发送的数据地址
			参数2：发送数据长度
			参数3：CAN的ID号
返回值：发送成功返回0，发送超时返回1
*******************/
static unsigned char CAN1_Send_One_Frame(unsigned char *msg, unsigned char len,unsigned int EID)
{ 
	 	OS_ERR err;
    CPU_SR_ALLOC();
	  unsigned char mbox;
    unsigned short int i = 0;
	
    CanTxMsg TxMessage;
    TxMessage.StdId = 0x00;		 		// 标准标识符
	  TxMessage.ExtId = EID;				// 设置扩展标示符(ID号)
    TxMessage.IDE = CAN_Id_Extended;	// 扩展帧, CAN_Id_Standard标准帧; CAN_Id_Extended扩展帧  扩展格式中ID0~ID17为Extension ID,而ID18~ID28为Base ID.
    TxMessage.RTR = CAN_RTR_Data;		// 数据帧
    TxMessage.DLC = len;		  		// 要发送的数据长度
    for(i = 0; i < len; i++)
	  {
        TxMessage.Data[i] = msg[i];
	  }
	  
    mbox = CAN_Transmit(CAN1, &TxMessage);

		//CAN发送，则CAN重启；
		if(mbox>2)
		{
			//FOR TEST
			printf("CAN1异常重启！\n\r");	
			OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); //延时10ms
		  CAN_DeInit(CAN1);
		  CAN1_Mode_Init(CAN1,gCANSystemParameter);   //CAN模式初始化使能	
		  return 1;			
		}
		
    i = 0;
    while((CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed) && (i < 0XFFF))
		i++;	//等待发送结束
    if(i >= 0XFFF)
		{
			return 1;
		}
		
    return 0;
}






/**********************************************************************************************************
*	函 数 名: CAN_Send_Frame()
*	功能说明: CAN1连续发送函数
*	形    参：	参数1：需要发送的数据地址
				参数2：发送数据长度
				参数3：协议板编号
*	返 回 值: 0,成功;1，失败
**********************************************************************************************************/
unsigned char CAN1_Send_Frame(unsigned char *msg, unsigned int len,unsigned char Board_Number)     //CAN1连续发送函数
{
	OS_ERR  err;
	u8 i = 0;
	u8 j = 0;
	u8	Flag = 0;
	u8	SendData[8];
	u8 Pack_sum=0;
	unsigned int Id=0;
	
	//计算总包数
	if(len%8==0)
		Pack_sum=len/8;
	else
		Pack_sum=(len/8) + 1;
	//组装ID号
		Id = Id|(Board_Number<<24);
		Id = Id|(Pack_sum<<8);
	//用户信息发送数据，第28位为1
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
*                            CAN1读取数据 
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


static void Reset_CANRxBuf(void)    //复位CAN接收数组
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

//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)
#ifdef XYB_DYJ
unsigned char Can_Send_Msg(unsigned char *msg, unsigned char len)
{
    unsigned char mbox;
    unsigned short int i = 0;
    CanTxMsg TxMessage;
    TxMessage.StdId = 0x00;
    TxMessage.ExtId = 0x1E000000;
    TxMessage.IDE = CAN_Id_Extended;	// 扩展帧 CAN_Id_Standard;标准帧
    TxMessage.RTR = CAN_RTR_Data;		// 数据帧
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
函数名：	CAN_RxISR
功能：		CAN中断转存数据（自动存储）
输入参数1：	接收到的数据指针
	参数2：	接收到的数据长度
	参数3：	接收到的数据ID
返回值：		无
*******************/
static unsigned char CAN_Temporary_buf[1024];		//定义缓存数组
static unsigned int CAN_Temporary_buf_Count=0;		//定义缓存数组
//void CAN_RxISR(unsigned char *buf,unsigned char len,unsigned int CAN_ID)
void CAN_RxISR_DYJ(unsigned char *buf,unsigned char len,unsigned int CAN_ID)
{

 
	unsigned char i=0;
	unsigned char Now_Frame_Cnt=0;						//当前帧
	unsigned char Last_Frame_Cnt=0;						//目标帧

	Now_Frame_Cnt=(unsigned char)CAN_ID;  				//取0-7位
	Last_Frame_Cnt =(unsigned char)((CAN_ID>>8)&0x000000ff); //取8-15位
	
	if(Now_Frame_Cnt<Last_Frame_Cnt)					//当前帧没有接收完
	{	
		for(i=0;i<len;i++)
		{
			CAN_Temporary_buf[CAN_Temporary_buf_Count++] = buf[i];
		}
	}
	else if(Now_Frame_Cnt==Last_Frame_Cnt)		//本帧最后一包数据
	{
		
		for(i=0;i<len;i++)
		{
			CAN_Temporary_buf[CAN_Temporary_buf_Count++] = buf[i];
		}
		//存数据
		CANRxBuf[CAN_Wr_Addr].ID_Number = (unsigned char)((CAN_ID>>24)&0x0f);			//当前帧接收板编号
		CANRxBuf[CAN_Wr_Addr].Lenth=CAN_Temporary_buf_Count;   //长度
		memcpy(CANRxBuf[CAN_Wr_Addr].data,CAN_Temporary_buf,CAN_Temporary_buf_Count);  //转存数据
		CANRxBuf[CAN_Wr_Addr].Flag=1;
		CAN_Wr_Addr++;
		if(CAN_Wr_Addr>=CAN_RX_GROUPNUM)CAN_Wr_Addr=0;
		if(CAN_Rd_Addr==CAN_Wr_Addr)   //@如果读写相等，说明写比读快了一个循环，此代码不能保证数据完全不丢失，只能保证最近接收的n-1帧数据正确
		{
			CAN_Rd_Addr++;
			if(CAN_Rd_Addr>=CAN_RX_GROUPNUM)CAN_Rd_Addr=0;
		}
		//数据清0
		CAN_Temporary_buf_Count=0;   	//数据转存后，清0
		memset(CAN_Temporary_buf,0,256);
	}
}






/******************
函数名：	YH_CAN1_RX0_IRQHandler
功能：		CAN中断接收用户信息数据函数
输入参数：	无
返回值：		无
*******************/
void YH_CAN1_RX0_IRQHandler(void)		//@中断接收数据，在CAN中断函数使用
{
//	unsigned char i=0;
	unsigned char lenth=0;
	unsigned int  CAN_Id=0;
  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

	Inspect_TimCount=0;			//@接收到数据后，定时计数清0恢复初值
	lenth = RxMessage.DLC;		//接收数据长度
	CAN_Id =RxMessage.ExtId;	//接收ID号
	CAN_RxISR_DYJ(RxMessage.Data,lenth,CAN_Id);
	
}
#ifdef XYB_DYJ
void CAN1_RX0_IRQHandler(void)
{
	YH_CAN1_RX0_IRQHandler();
}
#endif
//在周期函数中调用，对读取地址与写地址进行更新
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
函数名：	YH_CAN1_RX0_IRQHandler
功能：		CAN中断循环队列读数据
输入参数：	CAN返回数据指针
返回值：		返回CAN总线帧数据长度
*******************/
//unsigned int CAN1_Read(unsigned char *Rxdata);   //CAN1读取数据
void  CAN1_Read(CAN_Data_Typedef *S_Rxdata)   //CAN1读取数据
{
	if(CANRxBuf[CAN_Rd_Addr].Flag==1)
    {
		//读取数据
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