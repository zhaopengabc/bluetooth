#include "CAN1ProtocolParsing.h"
#include "rtc.h"
#include <string.h>
#include "MsgCANProcessDrv.h"
#include <stdio.h> 
#include "includes.h"
#include "frame.h"
#include "lcd.h"
#include "ProtocolBoard.h"
#include "w25q16.h"
#include "lcd.h"
#include "led.h"

#ifdef XYB_DYJ
const unsigned char SOFT_msg[40]="FB805XY_DYJ_V2.1";
#endif

//变量定义
ProtocolBoardRegistered S_ProtocolBoardRegistered;   //协议板注册结构体
InspectState S_InspectState[15];					 //巡检状态结构体数组
unsigned int Inspect_TimCount=0;					 //巡检软件定时器计算
unsigned int Inspect_NowNumber=1;					 //巡检当前协议板编号
unsigned char Registered_Allow=1;					 //本程序许允许注册标志位

Registered_InforDataStr S_Registered_InforDataStr[ProtocolBoard_CNT];	//协议板注册信息
RegisteredState 		S_RegisteredState[ProtocolBoard_CNT];			//协议板注册状态结构体			
unsigned int 			Registered_NowNumber=1;							//巡检当前协议板编号

unsigned char Registered_Start=0;				//开启注册
unsigned char Registered_AllOK=0;				//全部注册完成
//线路判断
//unsigned int MClineFlaut_flag=0xFFFF0000;
unsigned int CanLineCount=500;	
/*
***************************************************************************************
函数名：		CheckSumCalc()
功能： 		求和函数
输入参数1：	需要求和的数据的指针，
	参数2：	数据长度
返回值：	求和值
**************************************************************************************
*/
unsigned char CheckSumCalc(unsigned char * apData_i,unsigned int awDataLen_i)  //求和
{
    unsigned int i;
    unsigned char dChecksum = 0u;
    for(i = 0u; i < awDataLen_i; i++)
    {
        dChecksum += apData_i[i];
    }
    return dChecksum;
}



/*************************************************
//函数名		：Sned_InspectionToProtocolBoard（）
//功能		：发送巡检指令函数
//输出参数1	：需要访问的协议板编号
//返回值		：0,成功;1，失败
*************************************************/
unsigned char PriorityDataFlag=0x01;//火警没空间；
unsigned char Data_SendToServerFlag=0x02;//火警没空间；
unsigned char SendOne_InspectionToProtocolBoard(unsigned char BoardNumber)   		//发送巡检指令函数
{
	//装载ID号
	unsigned char buf[20];
	unsigned char length;
	unsigned char Timbuf[7];
	unsigned char success=0;
	
	RTC_Get((char *)Timbuf);   //获取当前时间
	
	buf[0]=0x0c;     		//信息体长度
	buf[1]=0x00;
	buf[2]=0x04;			//巡检注册命令

	buf[3]=0xc3;			//巡检类型标志c3
	buf[4]=0x01;			//信息体对象1
	
	buf[5]=0x01;			//信息体
	buf[6]=BoardNumber;		
	
	buf[7]=0x00;	   //8位中01表示火警没有空间，02表示其他信息没有空间；
	buf[7]|=PriorityDataFlag;
	buf[7]|=Data_SendToServerFlag;
	
	buf[8]=0x01;

	memcpy(&buf[9],Timbuf,6);   	//装载时间
	buf[15]=CheckSumCalc(buf,15);	//校验和
	length=16;						//本帧数据长度
	
	success=CAN1_Send_Frame(buf,length,BoardNumber);     //CAN1连续发送函数
	
	return success;
}







/*************************************************
//函数名		：Sned_RegisterToProtocolBoard（）
//功能		：发送注册指令函数
//输出参数1	：需要访问的协议板编号
//返回值		：0,成功;1，失败
*************************************************/
unsigned char SendOne_RegisterToProtocolBoard(unsigned char BoardNumber)   		//发送注册指令函数
{
	//装载ID号
	unsigned char buf[20];
	unsigned char length;
	unsigned char Timbuf[7];
	unsigned char success=0;
	
	RTC_Get((char *)Timbuf);   //获取当前时间
	
	buf[0]=0x0b;     			//信息体长度
	buf[1]=0x00;
	buf[2]=0x04;				//巡检注册命令
	
	buf[3]=0xc1;				//注册类型标志c1
	buf[4]=0x01;				//信息体对象1
	
	buf[5]=0x01;				//信息体
	buf[6]=BoardNumber;		
	buf[7]=0x01;		

	memcpy(&buf[8],Timbuf,6);   	//装载时间
	buf[14]=CheckSumCalc(buf,14);	//校验和
	length=15;						//本帧数据长度
	
	success=CAN1_Send_Frame(buf,length,BoardNumber);     //CAN1连续发送函数
	
	return success;
}




/*************************************************
//函数名		：tmrInspect_callback（）
//功能		：//巡检定时器回调函数
//输出参数1	：系统设置
		  2 ：系统设置
//返回值		：无
*************************************************/
void tmrInspect_callback(void)    //巡检定时器回调函数
{
	Inspect_TimCount++;	
}




/*************************************************
//函数名		：Registered_ProtocolBoard（）
//功能		：协议版发送注册指令函数
//输出参数1	：无

//返回值		：1,完成，0还在注册中
*************************************************/
unsigned char Registered_ProtocolBoard(void)    	//协议版发送注册指令函数
{
	unsigned char Res=0;
	OS_ERR err;
	if(S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegistered_Operation==RegNo)    //判断当前协议板注册操作是否完成
	{
		//发送注册信息
		if(S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegisteredSendState==0)	//如果注册信息没有发送
		{
			SendOne_RegisterToProtocolBoard(Registered_NowNumber);   						//发送注册指令函数
			Inspect_TimCount=0;																//定时器赋初值
			OSTimeDlyHMSM(0,0,0,1,OS_OPT_TIME_HMSM_STRICT,&err);							//@等待1ms
			S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegisteredSendState=1;
		}
		//判断超时和发送次数
		if((S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegisteredReceiveTimOut==0)&&(S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegisteredReceiveOK==0))
		{
			//判断超时计数
			if(Inspect_TimCount>=ReceiveTimeOut)   //
			{
				S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegistered_SendTimes++;   		//发送次数加1
				if(S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegistered_SendTimes<2)
					S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegisteredSendState=0;		//再次发送标志
				else
				{
					S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegisteredReceiveTimOut=1;  //发送超时
				  S_ProtocolBoardRegistered.Reg_data[Registered_NowNumber-1]=0;
				}
			}
		}
		else
		{
			S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegistered_Operation=RegYes;		//当前协议板注册操作完成
			Registered_NowNumber++;   //注册下一块协议板
		}
	}
	
	if(Registered_NowNumber==ProtocolBoard_CNT)   //如果注册操作全部完成
	{
		memset(&S_RegisteredState[0].ProtocolBoardRegistered_Operation,0,sizeof(S_RegisteredState));   //结构体数组清0
		Registered_Start=0;			//退出注册
		Registered_NowNumber=1;		//注册编号复位
		Res=1;						//注册执行完成，
	}
	return Res;
}










/*************************************************
//函数名		：Cycle_Send_InspectionToProtocolBoard（）
//功能		：循环发送巡检指令
//输出参数1	：协议板注册结构体

//返回值		：0,成功;1，失败
*************************************************/
void  Cycle_Send_InspectionToProtocolBoard(ProtocolBoardRegistered S_ProtocolBoardRegis)   		//循环发送巡检指令函数
{
	OS_ERR err;
	unsigned int a=0;
	
	S_ProtocolBoardRegis.Reg_data[0]=RegYes;
	S_ProtocolBoardRegis.Reg_data[1]=RegYes;
//	S_ProtocolBoardRegis.Reg_data[2]=RegYes;
	
	if(S_ProtocolBoardRegis.Reg_data[Inspect_NowNumber-1]==RegYes)  			 //如果当前协议板已经注册
	{
		//协议板注册，下面执行巡检
		if(S_InspectState[Inspect_NowNumber-1].ProtocolBoardInspectSendState==0)  //如果巡检指令没有发送，则发送巡检
		{
			SendOne_InspectionToProtocolBoard(Inspect_NowNumber);   //发送巡检指令函数
			Inspect_TimCount=0;										//定时器赋初值
			Registered_Allow=0;										//禁止注册
			OSTimeDlyHMSM(0,0,0,1,OS_OPT_TIME_HMSM_STRICT,&err);	   //@等待1ms
			S_InspectState[Inspect_NowNumber-1].ProtocolBoardInspectSendState=1;//发送状态置位
		}
		//巡检未超时 并且 未接收到完整数据
		if((S_InspectState[Inspect_NowNumber-1].ProtocolBoardInspectReceiveTimOut==0)&&((S_InspectState[Inspect_NowNumber-1].ProtocolBoardInspectReceiveOK==0)))
		{
			//判断超时 判断
			if(Inspect_TimCount>=ReceiveTimeOut)    
			{
				S_InspectState[Inspect_NowNumber-1].ProtocolBoardInspectReceiveTimOut=1;
				Inspect_TimCount=0;//20180514 标志位清零；
			}
		}
		else //如果超时 或 接收到完整数据
		{
			Inspect_NowNumber++;    //如果超时，或者接收完成，巡检下一协议板
			Registered_Allow=1;		  //允许注册
//			if(Inspect_NowNumber>=16)
			if(Inspect_NowNumber>=2)				
			{		
		    Inspect_NowNumber=1;  //如果超出最大，循环
			  memset(&S_InspectState[0].ProtocolBoardInspectSendState,0,sizeof(S_InspectState));   //结构体数组清0
			}			
		}
	}
	else   //如果当前协议板没有注册
	{
		//没有注册，巡检下一个协议板。
		Inspect_NowNumber++;
//		if(Inspect_NowNumber>=16)
		if(Inspect_NowNumber>=2)		
		{
			Inspect_NowNumber=1;  //如果超出最大，循环
			memset(&S_InspectState[0].ProtocolBoardInspectSendState,0,sizeof(S_InspectState));   //结构体数组清0
		}
	}
}










/*************************************************
//函数名		：Sned_ConfirmToProtocolBoard（）
//功能		：发送确认指令函数
//输出参数1	：需要访问的协议板编号
//返回值		：0,成功;1，失败
*************************************************/
unsigned char Send_ConfirmToProtocolBoard(unsigned char BoardNumber)   		//发送确认指令函数
{
	//装载ID号
	unsigned char buf[20];
	unsigned char length;
	unsigned char Timbuf[7];
	unsigned char success=0;
	
	RTC_Get((char *)Timbuf);   //获取当前时间
	
	buf[0]=0x0C;     			//信息体长度
	buf[1]=0x00;
	buf[2]=0x03;				//确认命令
	
	buf[3]=0xc5;				//确认标志c5
	buf[4]=0x01;				//信息体对象1
	
	buf[5]=0x01;				//信息体
	buf[6]=BoardNumber;		
	buf[7]=0x01;
	buf[8]=0x01;

	memcpy(&buf[9],Timbuf,6);   	//装载时间
	buf[15]=CheckSumCalc(buf,15);	//校验和
	length=16;						//本帧数据长度
	
	success=CAN1_Send_Frame(buf,length,BoardNumber);     //CAN1连续发送函数
	
	return success;
}





/*************************************************
//函数名	：CAN1_DataRecieve_Integrity（）
//功能		：判断CAN1接收数据的正确性
//输出参数1	：接收的数据指针
	  参数2	：接收的数据长度
//返回值	：1,成功;0，失败
*************************************************/
unsigned char CAN1_DataRecieve_Integrity(unsigned char *date,unsigned int len)    //CAN1接收数据完整性判断
{
	unsigned char CRC_sum=0;
	
	if(len<1)
	{return 0;}
	
	CRC_sum=CheckSumCalc(date,(len-1));  //求和
	
	if(CRC_sum==date[len-1])
	{
		return 1;
	}
	else
	{	
		return 0;
	}
}




/*************************************************
//函数名	：ReceiveRegistered_Data（）
//功能		：接收注册数据
//输出参数1	：接收数据指针
//    参数2	：接收数据长度
//返回值		：无
*************************************************/
static void  ReceiveRegistered_Data(unsigned char *data,unsigned char len)   				//注册数据
{
		memcpy(&S_Registered_InforDataStr[Registered_NowNumber-1].Board_Type,&data[4],140);    //转存数据
		S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegisteredReceiveOK=1;		//接收完成标志
		S_ProtocolBoardRegistered.Reg_data[Registered_NowNumber-1]=RegYes;					//标注已注册
}

/*************************************************
//函数名	：ReceiveAlarm_Failure_Data（）
//功能		：接收部件运行数据
//输出参数1	：接收数据指针
//    参数2	：接收数据长度
//返回值		：无
*************************************************/
static void  ReceiveInspection(unsigned char *data,unsigned int len)   //接收巡检或者注册数据
{
	if(data[3]==0xc2)  															//接收数据是注册信息
	{
		ReceiveRegistered_Data(data,len); 		
	}	
	
	if(data[3]==0xc4)
	{
		S_InspectState[Inspect_NowNumber-1].ProtocolBoardInspectReceiveOK=1;    //当前帧接收完成
		//判断主机通讯，应该在此处。
		if(data[8]==0x01)//主机线路正常；
		{
			 MClineFlaut_flag&=0x0000FFFF;		
		   MClineFlaut_flag&=~(0x0001<<(Inspect_NowNumber));	
		}			
		else if(data[8]==0x02)//主机线路故障；
		{
		   MClineFlaut_flag&=0x0000FFFF;	
		   MClineFlaut_flag|=(0x0001<<(Inspect_NowNumber));
		}		
		else if(data[8]==0x03)//无线路判断功能；
		{
			
		}
	}
}
	




/*************************************************
//函数名	：ReceiveAlarm_Failure_Data（）
//功能		：接收部件运行数据
//输出参数1	：接收数据指针
//    参数2	：接收数据长度
//返回值		：无
*************************************************/
static unsigned char Redata[512];//发送数据空间；
void  ReceiveAlarm_Failure_Data(unsigned char *data,unsigned int len)   //接收部件运行数据（火警、故障）
{
	switch(data[3])
	{
    case 0x01:		
    case 0x04:
		case 0x02:
			ReportToServer(0x02,&data[3],len-4,Redata);
//			printf("收=%d！\n\r",data[8]);
			break;	
		default:
			break;
	}
}




/*************************************************
//函数名		：ThreeInOne_ProtocolBoard_Handle（）
//功能		：CAN总线协议解析
//输出参数1	：接收数据指针
//    参数2	：接收数据长度
//返回值		：无
*************************************************/
unsigned char ThreeInOne_ProtocolBoard_Handle(unsigned char *buf,unsigned int len,unsigned char ID)   		//协议三合一板CAN协议解析
{ 
	switch(buf[2])
	{
		case 0x02:  //上发的主机运行状态数据（控制命令字节）
		{
			ReceiveAlarm_Failure_Data(buf,len);   	//接收部件运行数据
			Send_ConfirmToProtocolBoard(ID);		    //发送确认指令函数
		  S_InspectState[Inspect_NowNumber-1].ProtocolBoardInspectReceiveOK=1;    //当前帧接收完成			20180514
			break;
		}
		case 0x05:  //协议板应答用户信息巡检指令
		{	
			ReceiveInspection(buf,len);   		
			break;
		}
		default :
			break ;	
	}
	return 0;
}






/*************************************************
//函数名		：ThreeInOne_ProtocolBoard_Run（）
//功能		：CAN总线协议执行函数
//输出参数1	：无
//返回值		：无
*************************************************/
unsigned short register_flag=0x0000;//需要显示的板卡信息  //注册成功标志位。1的时候，注册成功，0的时候注册失败。
//注册协议板完成标志位；
unsigned char RegisteProComplete=0;
unsigned char BK_Message[17][100];//板卡信息存储；

extern void RefreshRunLedState(void);
unsigned char ThreeInOne_ProtocolBoard_Run(void)   		//CAN总线协议执行函数
{
	unsigned int length=0,RegisterSave=0;
	unsigned char  Res=0;
	unsigned char i=0;
  CAN_Data_Typedef S_CAN_Rxdata;

	///////////////发送数据///////////////
	//发送注册信息
//	if((Registered_Start==1)&&(Registered_Allow==1))							//如果开启了注册命令，并且允许注册，则发送注册指令。巡检过程中，不能注册。
//	{
//		//线路绿灯，黄灯都不亮，复位灯
//	  MClineFlaut_flag=0xFFFF0000;
//		
//		Res=Registered_ProtocolBoard();    										      //协议版发送注册指令 15个板子注册。
//		if(Res==1)
//		{
//			Registered_AllOK=1;  //对外结构标志，发出注册标志后，等待此标志位置1，然后读取数据，数据已经装置在结构体里，读取数据后此标志置0
//			register_flag=0;
//			
//			for(i=0;i<15;i++)
//			{
//				//装载板卡注册编号和注册数据
//				if(S_ProtocolBoardRegistered.Reg_data[i]==1)  	//如果板卡注册成功
//				{
//					register_flag|=(0x01<<(i+1));
//					memcpy(BK_Message[i+1],&S_Registered_InforDataStr[i].Hardware_Version[0],100);
//				}
//			}
//			RegisterSave=register_flag;
//      Writeflash_Config((unsigned char *)&RegisterSave,ProID_CONFIG_CMD);//保存协议卡注册标志位；                                                               //			
//		  RegisteProComplete=1;
//		}
//	}
//	else   //发送巡检信息
//	{	
	  Cycle_Send_InspectionToProtocolBoard(S_ProtocolBoardRegistered);
// 	}   		//循环发送巡检指令函数
	
	///////////////接收数据解析///////////////
	S_CAN_Rxdata.Lenth=0;
	CAN1_Read((CAN_Data_Typedef *)&S_CAN_Rxdata.ID_Number);   //CAN1读取数据
	if(S_CAN_Rxdata.Lenth!=0)
	{
		if(CAN1_DataRecieve_Integrity(S_CAN_Rxdata.data,  S_CAN_Rxdata.Lenth))     						//CAN1接收数据完整性判断
		{
			//完整性正确,进入解析函数
			ThreeInOne_ProtocolBoard_Handle(S_CAN_Rxdata.data,  S_CAN_Rxdata.Lenth,  S_CAN_Rxdata.ID_Number);   //协议三合一板CAN协议解析	
		  CanLineCount=500;//5S			
		}
		S_CAN_Rxdata.Lenth=0;
	}
	return 0;
}


void ProID_Init(void)
{
	unsigned char buf[10];
	unsigned int flag=0;
	unsigned char i=0;
  unsigned short	B;
	memset(buf,0,10);
	
	Readflash_Config(buf,ProID_CONFIG_CMD);
	flag=buf[3];
	flag=flag<<8;
	flag=flag+buf[2];
	flag=flag<<8;
	flag=flag+buf[1];
	flag=flag<<8;
	flag=flag+buf[0];
	
	register_flag=flag;
	
	for(i=0;i<15;i++)
	{
		B=0;
		B=(0x0001<<(i+1));
		if((register_flag&B)!=0)
		{
	   S_ProtocolBoardRegistered.Reg_data[i]=1;
		}
	}
}		

unsigned char CAN_LINE_CHECK(void)
{
		CanLineCount--;
		if(CanLineCount<=1)
		{ 
			CanLineCount=2;
			if(register_flag!=0)					//有协议板
	    { return 0;	}				//线路故障
		  else
		  { return 1;}				//线路正常
		}
		else 
		{
				return 1;				  //线路正常
		}
}


void CAN1Init(void)
{


}	
