#ifdef D026_HW200
/*
编写记录：
1.20180611：增加校验和；
2.20180611：编写并测试完成。
3.20181226: 收到数据包立刻回复51
主机信息记录：
 主机采用轮询方式
 查询：500ms  31
 回复：51  有数据上发不回复51的话就一直发送此包数据，发送51完事才发下一包数据直到上发完事（系统操作如复位，主备电故障恢复需要接受完一包数据，主机还会发送一包数据需在回复51）
*/

//主板测试用程序；
#include "D026_HW200.h" 
#include "UsartMsgProcessAppExt.h"
#include "MsgCANProcessAppExt.h"
#include "rtc.h"
#include "app.h"
#include  <string.h>
#include "ChoiceDevice.h"
#include "ReceiveDataBuffer.h"

#define GST200_HUOJING_CMD					0x15	//部件火警
#define GST200_GUZHANG_CMD					0x21	//部件故障
#define GST200_LIANDONG_CMD					0x02	//部件模块启动
#define GST200_FANKUI_CMD					  0x22	//部件模块反馈
#define GST200_PINGBI_CMD					  0xDE	//部件模块屏蔽
#define GST200_ZHUDIANYUAN_CMD 			0x31	//主/备电故障
#define GST200_FUWEI_CMD					  0x04	//复位
#define GST200_XIAOYIN_CMD					0x70	//消音

//板卡型号；主机型号；软件版本；接口说明；
const unsigned char BKXH_msg[20]="XSBD-FB805XY-A";
const unsigned char XFZJ_msg[20]="HW200";
const unsigned char SOFT_msg[40]="FB805XY_HW200R_V2.2";
const unsigned char ITFS_msg[20]="RS232";
 
const unsigned short VERS_msg=11026;//21020=版本V21+主机型号D020（远程升级，协议版本）

tMCSystemStatus  			    sMCSystemStatus_ZJ;   		//系统状态数据结构体
tMCSystemDeviceStatus 		sMCSystemDeviceStatus_ZJ;   //设备状态数据结构体
tMCSystemOperationStatus 	sMCSystemOperationStatus_ZJ;  //操作信息数据结构体

static void ZJ_Plugin_Init(void)//初始化
{
    //初始化UART参数；
	gUARTSystemParameter.USARTx_BaudRate=115200;
	gUARTSystemParameter.USARTx_WordLength=USART_WordLength_8b;
	gUARTSystemParameter.USARTx_StopBits=USART_StopBits_1;
	gUARTSystemParameter.USARTx_Parity=USART_Parity_No;
	gUARTSystemParameter.USARTx_HardwareFlowControl=USART_HardwareFlowControl_None;
//	gUARTSystemParameter.USARTx_Mode=USART_Mode_Tx;
	gUARTSystemParameter.USARTx_Mode=USART_Mode_Rx | USART_Mode_Tx;
	gUARTSystemParameter.UARTx_IRQn=USART3_IRQn;
	gUARTSystemParameter.NVIC_Priority=0;	
	gUARTSystemParameter.NVIC_SubPriority=3;		
}

//数据完整性正确性判定，若正确返回1，错误返回0；
unsigned char ZJ_Integrity(unsigned char *data,unsigned int lenth)
{
	unsigned char flag=0,i=0,sum=0;
	unsigned char Senddata=0x51;  
	
	for(i = 0; i < lenth-1; i++)
	{
			sum += data[i];
	}
		
	if((lenth >= 1)&&(sum==data[lenth-1]))
	{
			flag=1;
	}
	if(lenth > 1)
  {		
			UARTx_Send_Data(UartInterface,(unsigned char *)&Senddata,1);
	}
	return flag;
 
}

/***********************
0x82主电故障			---->0x0100
0x84备电故障			---->0x0200
***********************/
unsigned int GST200_SysStateConv(unsigned char state)
{
	unsigned int flag=0;
	switch(state)
	{
		case 0x82: flag=0x0100;break;
		case 0x84: flag=0x0200;break;
		default: break;
	}
	return flag;
}

unsigned int  ZJ_SysState_Analsis(unsigned char *data,unsigned int lenth)
{
	unsigned int len=12;
	unsigned short Status=0;
	
	sMCSystemStatus_ZJ.MCaddress        = 1;		//主机地址

	if(data[4] == 0x31)
	{
		//系统状态
		sMCSystemStatus_ZJ.MCStatus0=GST200_SysStateConv(data[8]);
		sMCSystemStatus_ZJ.MCStatus1=GST200_SysStateConv(data[8])>>8;
	}
	
  sMCSystemStatus_ZJ.MCTimeDataYer=SystemTime[5];//年
	sMCSystemStatus_ZJ.MCTimeDataMon=SystemTime[4];//月
	sMCSystemStatus_ZJ.MCTimeDataDay=SystemTime[3];//日	
	sMCSystemStatus_ZJ.MCTimeDataHor=SystemTime[2];//小时
	sMCSystemStatus_ZJ.MCTimeDataMin=SystemTime[1];//分
	sMCSystemStatus_ZJ.MCTimeDataSec=SystemTime[0];//秒	
	
    RecieveMCSystemStatusAndSend(  sMCSystemStatus_ZJ.MCaddress,
																	 sMCSystemStatus_ZJ.MCStatus0,
                                   sMCSystemStatus_ZJ.MCStatus1,
                                   sMCSystemStatus_ZJ.MCTimeDataSec,
                                   sMCSystemStatus_ZJ.MCTimeDataMin,
                                   sMCSystemStatus_ZJ.MCTimeDataHor,
                                   sMCSystemStatus_ZJ.MCTimeDataDay,
                                   sMCSystemStatus_ZJ.MCTimeDataMon,
                                   sMCSystemStatus_ZJ.MCTimeDataYer);//系统状态反馈；
	return len;
}
/***********************
01光栅测温		---->31
02感温			---->31
03感烟			---->40
04报警接口		---->51
0B手报			---->23
4C声光报警		---->因为没有对应的，暂定为121
4D火警传输		---->121
0D讯响器		---->147
***********************/
unsigned char GST200_ComponentConversion(unsigned char type)
{
	unsigned char flag=0;
	switch(type)
	{
		case 0x01: flag=31;break;
		case 0x02: flag=31;break;
		case 0x03: flag=40;break;
		case 0x04: flag=51;break;
		case 0x0B: flag=23;break;
		case 0x0D: flag=121;break;
		case 0x4C: flag=121;break;
		case 0x4D: flag=147;break;
		default: break;
	}
	return flag;
}

/***********************
0x15火警			---->0x0002
0x21故障			---->0x0004
0xDE屏蔽			---->0x0008
0x02启动			---->0x0020
0x22反馈			---->0x0040
***********************/
unsigned char GST200_CompStateConv(unsigned char state)
{
	unsigned int flag=0;
	switch(state)
	{
		case 0x15: flag=0x0002;break;
		case 0x21: flag=0x0004;break;
		case 0xDE: flag=0x0008;break;
		case 0x02: flag=0x0020;break;
		case 0x22: flag=0x0040;break;
		default: break;
	}
	return flag;
}
//data[2]=0x12 data[3]=0x34 data[4]=0x56
int HW200_MCDeviceCode(unsigned char *data,unsigned char length)
{
	unsigned char nbr[6];
	unsigned char nbrA;
	int B=0;
	memset(nbr,0,6);
	nbrA=data[0]&0xF0;
  nbrA=nbrA>>4;
	B=B+nbrA;
	B=B*10; //10
	nbrA=data[0]&0x0F;
	B=B+nbrA;
	B=B*10; //120

	nbrA=data[1]&0xF0;
  nbrA=nbrA>>4;
	B=B+nbrA;
	B=B*10;	//1230
	nbrA=data[1]&0x0F;
	B=B+nbrA;
	B=B*10; //12340	
	
	nbrA=data[2]&0xF0;
  nbrA=nbrA>>4;
	B=B+nbrA;
	B=B*10;	//123450
	nbrA=data[2]&0x0F;
	B=B+nbrA;
	         //123456
	
	return B;
}
//部件解析
unsigned int  ZJ_ComponentState_Analsis(unsigned char *data,unsigned int lenth)
{
	unsigned int len=48;
	unsigned int code=0;
  //系统地址
	sMCSystemDeviceStatus_ZJ.MCaddress        = 1;		//主机地址
	//部件类型
	sMCSystemDeviceStatus_ZJ.MCDevicetype1    = GST200_ComponentConversion(data[8]);
 
	sMCSystemDeviceStatus_ZJ.MCDeviceAddress0 = 0;					  //回路
	sMCSystemDeviceStatus_ZJ.MCDeviceAddress1 = 0;	
	sMCSystemDeviceStatus_ZJ.MCDeviceAddress2 = 0;					  //设备地址
	sMCSystemDeviceStatus_ZJ.MCDeviceAddress3 = 0;
	code=HW200_MCDeviceCode(&data[5],3);//用户编码
	
	
	sMCSystemDeviceStatus_ZJ.MCDeviceStatus0  = GST200_CompStateConv(data[4]);		 
	sMCSystemDeviceStatus_ZJ.MCDeviceStatus1  = 0;
 

		sMCSystemDeviceStatus_ZJ.MCTimeDataYer=SystemTime[5];//年
		sMCSystemDeviceStatus_ZJ.MCTimeDataMon=SystemTime[4];//月
		sMCSystemDeviceStatus_ZJ.MCTimeDataDay=SystemTime[3];//日	
		sMCSystemDeviceStatus_ZJ.MCTimeDataHor=SystemTime[2];//小时
		sMCSystemDeviceStatus_ZJ.MCTimeDataMin=SystemTime[1];//分
		sMCSystemDeviceStatus_ZJ.MCTimeDataSec=SystemTime[0];//秒	
	
    RecieveMCDeviceStatusAndSend(  sMCSystemDeviceStatus_ZJ.MCaddress,
																	 sMCSystemDeviceStatus_ZJ.MCDeviceAddress0,
                                   sMCSystemDeviceStatus_ZJ.MCDeviceAddress1,
                                   sMCSystemDeviceStatus_ZJ.MCDeviceAddress2,
                                   sMCSystemDeviceStatus_ZJ.MCDeviceAddress3,
                                   sMCSystemDeviceStatus_ZJ.MCDeviceStatus0,
                                   sMCSystemDeviceStatus_ZJ.MCDeviceStatus1,
                                   sMCSystemDeviceStatus_ZJ.MCTimeDataSec,
                                   sMCSystemDeviceStatus_ZJ.MCTimeDataMin,
                                   sMCSystemDeviceStatus_ZJ.MCTimeDataHor,
                                   sMCSystemDeviceStatus_ZJ.MCTimeDataDay,
                                   sMCSystemDeviceStatus_ZJ.MCTimeDataMon,
                                   sMCSystemDeviceStatus_ZJ.MCTimeDataYer,
		                               sMCSystemDeviceStatus_ZJ.MCDevicetype1,
		                               sMCSystemDeviceStatus_ZJ.MCDevicetype2,
																	 code); //设备状态反馈；
	return len;
}

/***********************
0x04复位			---->0x01
0x70消音			---->0x02
***********************/
unsigned char GST200_ActionConv(unsigned char state)
{
	unsigned char flag=0;
	switch(state)
	{
		case 0x04: flag=0x01;break;
		case 0x70: flag=0x02;break;
		default: break;
	}
	return flag;
}
//操作解析
unsigned int  ZJ_Action_Analsis(unsigned char *data,unsigned int lenth)
{
	unsigned int len=12;
 
	sMCSystemOperationStatus_ZJ.MCaddress       = 1;		//主机地址
	
	//系统状态
	sMCSystemOperationStatus_ZJ.MCOperationType = GST200_ActionConv(data[4]);	//复位,消音

	sMCSystemOperationStatus_ZJ.MCTimeDataYer=SystemTime[5];//年
	sMCSystemOperationStatus_ZJ.MCTimeDataMon=SystemTime[4];//月
	sMCSystemOperationStatus_ZJ.MCTimeDataDay=SystemTime[3];//日	
	sMCSystemOperationStatus_ZJ.MCTimeDataHor=SystemTime[2];//小时
	sMCSystemOperationStatus_ZJ.MCTimeDataMin=SystemTime[1];//分
	sMCSystemOperationStatus_ZJ.MCTimeDataSec=SystemTime[0];//秒	
	
  RecieveMCOperationStatusAndSend(sMCSystemOperationStatus_ZJ.MCaddress,
		                              sMCSystemOperationStatus_ZJ.MCOperationType,
	                                sMCSystemOperationStatus_ZJ.MCTimeDataSec,
                                  sMCSystemOperationStatus_ZJ.MCTimeDataMin,
																	sMCSystemOperationStatus_ZJ.MCTimeDataHor,
																	sMCSystemOperationStatus_ZJ.MCTimeDataDay,
																	sMCSystemOperationStatus_ZJ.MCTimeDataMon,
																	sMCSystemOperationStatus_ZJ.MCTimeDataYer	
	                               );
	return len;
}
unsigned int ZJ_Analysis(unsigned char *data,unsigned int lenth)
{
	unsigned int len=0; 
	unsigned char Senddata=0x51;  
	//部件状态
	if((data[4]==GST200_HUOJING_CMD)||(data[4]==GST200_GUZHANG_CMD)||(data[4]==GST200_LIANDONG_CMD)||(data[4]==GST200_FANKUI_CMD)||(data[4]==GST200_PINGBI_CMD))
	{
		len=ZJ_ComponentState_Analsis(data,lenth);
	}
	//系统状态
	else if(data[4]==GST200_ZHUDIANYUAN_CMD)
	{
		len=ZJ_SysState_Analsis(data,lenth);
	}
	//操作信息
	else if ((data[4]==GST200_FUWEI_CMD)||(data[4]==GST200_XIAOYIN_CMD))
	{
		len=ZJ_Action_Analsis(data,lenth);
	}
	
	if(len!=0)
	{
//		UARTx_Send_Data(UartInterface,(unsigned char *)&Senddata,1);
	}
	return len;
}

void ZJ_Plugin_Run(void)		//协议解析启动
{
	unsigned int Flag=0;
	unsigned int Flag_tmp=0;
	unsigned int len=0;
	unsigned char Senddata[3]={0x2b,0x2b,0x2b};
	//读取数据
	if(Flag_tmp == 0)
	{
	    UARTx_Send_Data(UartInterface,(unsigned char *)&Senddata,3);
		Flag_tmp = 1;
	}
	Data_RecieveFromHost.Lenth=RS485_Read(Data_RecieveFromHost.data);
	
	if(Data_RecieveFromHost.Lenth!=0)
	{
	//正确性判定，解析数据，转换格式；
	Flag=ZJ_Integrity(Data_RecieveFromHost.data,Data_RecieveFromHost.Lenth);
	if(Flag==0)
	{
		
	}
	else
	{
		ZJ_Analysis(Data_RecieveFromHost.data,Data_RecieveFromHost.Lenth);   
	}
}	
		
	if(Sys_Second_100mS>4)//发巡检；
	{
		Sys_Second_100mS=0;
		UARTx_Send_Data(UartInterface,(unsigned char *)&Senddata,1);
	}
}


void ZJ_Plugin_Interrupt(void)//中断数据接收；
{	
 	
}

PluginInterface D026_HW200_Plugin = 
				{
					&ZJ_Plugin_Init,					 
					&ZJ_Plugin_Run,		 
					&ZJ_Plugin_Interrupt	 
				};
#endif    











