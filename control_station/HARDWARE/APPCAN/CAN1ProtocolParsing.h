#ifndef _CAN1ProtocolParsing_h_
#define _CAN1ProtocolParsing_h_


/********************************************************
//					宏定义
********************************************************/
#define  RegYes				1        	//已注册
#define  RegNo				0			//未注册

#define  ReceiveTimeOut		20			//等待超时计数时间
#define	 ProtocolBoard_CNT	16			//协议板数量

/********************************************************
//					结构体定义
********************************************************/

//注册信息接收结构体
typedef struct
{	
    unsigned char  	Board_Type;   					//协议板类型
    unsigned char  	Hardware_Version[20];   		//硬件版本
	unsigned char  	Software_Version[40];   		//软件版本
	unsigned char 	BT_Version[30];   				//BT版本
	unsigned char  	Host_Interface[20];   			//配接主机接口	
	unsigned char  	Host_Machine_Type[20];   		//主机型号
	unsigned char  	Reserved_Date[10];   			//保留
//	unsigned char  	TimeData[6];   					//时间
	
}Registered_InforDataStr;



//协议板发送巡检状态结构体
typedef	struct
{
	unsigned char ProtocolBoardRegistered_Operation;			//当前协议板注册操作状态
	unsigned char ProtocolBoardRegistered_SendTimes;			//当前协议板注册发送次数
	
	unsigned char ProtocolBoardRegisteredSendState;				//注册发送状态
	unsigned char ProtocolBoardRegisteredReceiveTimOut;			//接收超时
	unsigned char ProtocolBoardRegisteredReceiveOK;				//接收完成
}RegisteredState;




//协议板注册与否结构体
typedef  union
{
	unsigned char Reg_data[15];
	struct
	{
		unsigned char Registered_OneIsOrNo;
		unsigned char Registered_TwoIsOrNo;
		unsigned char Registered_ThreeIsOrNo;
		unsigned char Registered_FourIsOrNo;
		unsigned char Registered_FiveIsOrNo;
		unsigned char Registered_SixIsOrNo;
		unsigned char Registered_SevenIsOrNo;
		unsigned char Registered_EightIsOrNo;
		unsigned char Registered_nineIsOrNo;
		unsigned char Registered_TenIsOrNo;
		unsigned char Registered_ElevenIsOrNo;
		unsigned char Registered_TwelveIsOrNo;
		unsigned char Registered_ThirteenIsOrNo;
		unsigned char Registered_FourteenIsOrNo;
		unsigned char Registered_FifteenIsOrNo;
	}Regis;
}ProtocolBoardRegistered;






//协议板发送巡检状态结构体
typedef	struct
{
	unsigned char ProtocolBoardInspectSendState;			//巡检发送状态
	unsigned char ProtocolBoardInspectReceiveTimOut;		//接收超时
	unsigned char ProtocolBoardInspectReceiveOK;			//接收完成
}InspectState;









//内部声明导出
extern InspectState S_InspectState[15];					//巡检状态结构体数组
extern unsigned int Inspect_TimCount;					//巡检软件定时器计数
extern unsigned char Registered_Start;				//开启注册
extern unsigned int CanLineCount;	//CAN线计数器
extern unsigned char PriorityDataFlag;//火警空间；
extern unsigned char Data_SendToServerFlag;//火警空间；

extern unsigned char Send_InspectionToProtocolBoard(unsigned char BoardNumber);   		//发送巡检函数
extern unsigned char Send_RegisterToProtocolBoard(unsigned char BoardNumber);   		//发送注册指令函数
extern unsigned char Send_ConfirmToProtocolBoard(unsigned char BoardNumber);   			//发送确认指令函数
extern void tmrInspect_callback(void);    //巡检定时器回调函数
extern unsigned char ThreeInOne_ProtocolBoard_Run(void);   		//协议三合一板CAN协议执行函数
extern void ProID_Init(void);
extern unsigned int MClineFlaut_flag;
extern unsigned char CAN_LINE_CHECK(void);
#endif
