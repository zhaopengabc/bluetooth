#include "RecieveFromServer.h"
#include "string.h"
#include <stdio.h>
#include "frame.h"
#include "net.h"
#include "ServerAction.h"
#include "rtc.h"
#include "lcd.h"
#include "ServerAction.h"
extern unsigned char correction_time_way;//校正时间方式。分为手动和自动。0:手动。1：自动。
void Reset_RecieveFromServerData(Server_Data_Typedef *pServer_Data)
{
	memset(pServer_Data->data,0,SERVER_DATA_NUM);
	pServer_Data->Lenth=0;
	pServer_Data->Flag=0;
}
//数据不正确,发送否认帧，请求重发；
unsigned char RecieveFromServer_Error_Action(unsigned char *data,unsigned int lenth)
{
	return 0;
}
unsigned int DutyCmd(unsigned char *data,unsigned int lenth)
{
	if((data[28]==1)&&(data[24]==3)&&(data[25]==0))
	{
		IsOnDuty_Action();
		return 1;
	}
	return 0;
}
//初始化系统应先上传信息，再进行重启，重启时间设定为收到命令后的10s
unsigned int InitCmd(unsigned char *data,unsigned int lenth)
{
	if((data[28]==1)&&(data[24]==3)&&(data[25]==0))//初始化,没有初始化只是重启了，应擦除FLASH再重启
	{
		return 1;
	}
	return 0;
}
//控制命令解析
//返回0不需要回复确认，返回1回复确认帧
unsigned int ServerData_Control(unsigned char *data,unsigned int lenth,unsigned char *destdata)
{
	unsigned int len=0;
	switch(data[27])
	{
		//巡检
		case 81:
			//关闭查岗；
      ChaGangYingDa_stop();
		  //上发心跳包，上发状态
			HeartBeatFrame(); 
			break;		
		//初始化用户信息传输装置
		case 89:
			len=InitCmd(data,lenth);
			break;
		//同步时钟；
		case 90:
			if((data[28]==1)&&(data[24]==8)&&(data[25]==0))
			{
//				RTC_Set((char *)&data[29]);
//				dwin_time_set((unsigned char *)&data[29]);
			}
			len=1;
			break;
		//查岗命令；
		case 91:
			len=DutyCmd(data,lenth);
			break;
		default :break;
	}
	if(len!=0)
	{
		len=ReplyToServer_ConfirmationFrame(data[2]+data[3]*256,3,destdata);
	}
	return len;
}
//请求命令解析
unsigned int ServerData_Request(unsigned char *data,unsigned int lenth,unsigned char *destdata)
{
	unsigned int len=0;
	unsigned char destdataA[10]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	
	switch(data[27])
	{
		//读取用户信息传输装置运行状态
		case 81:
			if((data[28]==1)&&(data[24]==3)&&(data[25]==0))
			{
				len=Device_RunningState(destdataA);
			}
			break;
		//读取用户信息传输装置操作信息记录
		case 84:
			if((data[28]==1)&&(data[24]==9)&&(data[25]==0))
			{
				
			}
			break;
		//读取用户信息传输装置软件版本
		case 85:
			if((data[28]==1)&&(data[24]==3)&&(data[25]==0))
			{
				len=Device_Version(destdataA);
			}
			break;
		//读取用户信息传输装置配置情况
		case 86:
			if((data[28]==1)&&(data[24]==3)&&(data[25]==0))
			{
				
			}
			break;
		//读取用户信息传输装置系统时间
		case 88:
			if((data[28]==1)&&(data[24]==3)&&(data[25]==0))
			{
				len=Device_SysTime(destdataA);
			}
			break;
		default :break;
	}
	if(len!=0)
	{
		len=ReplyToServer_InformationFrame(data[3]*256+data[2],destdataA,len,destdata);
	}
	return len;
}
static unsigned char Str_Is_Equal(unsigned char *dest,unsigned char *source,unsigned char length)
{
	unsigned char i=0;
	unsigned char flag=1;
	for(i=0;i<length;i++)
	{
		if(*dest++!=*source++)
		{
			flag=0;
			break;
		}
	}
	return flag;
}
//数据解析，将解析结果放回destdata中，返回解析后需要执行的操作
unsigned int ServerData_Analysis(unsigned char *data,unsigned int lenth,unsigned char *destdata)
{
	unsigned int len=0;
	char rtcdata[0x20];
	unsigned char flag=0;
	unsigned char i=0;
	unsigned char checksum=0;
	
	switch(data[26])//通过判断数据类型来查看每个数组中元素的个数
	{
	 case 1://控制命令
			len=ServerData_Control(data,lenth,destdata);
			break;
	 case 3://确认,判定业务流水号是否一致如果一致，则清除发送缓存
//		  printf("确认号=%d！\n\r",(data[2]+data[3]*256));
			IsEquel(data[2]+data[3]*256);
			//若校验和正确，则判定
			if(correction_time_way==1)
			{
				memset(rtcdata,0,0x20);
				RTC_Get(rtcdata);
//				dwin_time_read(rtcdata);
				flag=Str_Is_Equal((unsigned char *)&rtcdata[2],(unsigned char *)&data[8],4);
				checksum=0;
				for(i=2;i<27;i++)
				{
					checksum+=data[i];
				}
				if(flag==1)
				{
					if((rtcdata[1]>data[7])&&((rtcdata[1]-data[7]>2)&&(rtcdata[1]-data[7]<58)))
					{
						flag=0;
					}
					else if((rtcdata[1]<data[7])&&((data[7]-rtcdata[1]>2)&&(data[7]-rtcdata[1]<58)))
					{
						flag=0;
					}else
					{}
				}
				if((flag==0)&&(checksum==data[27]))
				{
//					rtcdata[0x10]=rtcdata[5];
//					rtcdata[0x11]=rtcdata[4];
//					rtcdata[0x12]=rtcdata[3];
//					rtcdata[0x13]=rtcdata[2];
//					rtcdata[0x14]=rtcdata[1];
//					rtcdata[0x15]=rtcdata[0];
          //20180627					
					rtcdata[0x10]=data[6];
					rtcdata[0x11]=data[7];
					rtcdata[0x12]=data[8];
					rtcdata[0x13]=data[9];
					rtcdata[0x14]=data[10];
					rtcdata[0x15]=data[11];
					RTC_Set((char *)&rtcdata[0x10]);
//					dwin_time_set((const char *)&rtcdata[0x10]);					
				}
			}
			len=0;
			break;
	 case 4://请求
			len=ServerData_Request(data,lenth,destdata);
			break;
	 case 6://否认
			len=0;
			//若为否认帧不作处理继续发送；
			break;
	 default:
			break;
	}
	return len;
}
//对解析的数据执行命令
unsigned char Execute_Command(unsigned char *data,unsigned int len)
{
	return 1;
}
//数据正确
unsigned char RecieveFromServer_Correct_Action(unsigned char *data,unsigned int lenth)
{
	static unsigned char destdata[SERVER_DATA_NUM];
	unsigned int len=0;
	
	//数据解析分解并执行相应动作
	len=ServerData_Analysis(data,lenth,destdata);
	if(len!=0)
	{
		Data_TransToServer(destdata,len);
		return 1;
	}
	return 0;
}
//数据完整性正确性判定，若正确，返回业务流水号，不正确，返回0；
unsigned char BT_Logic(unsigned char *data,unsigned int dataLength);
extern unsigned char BT_HeartFlag;
//数据分包；同时收到40 40和41 41开头的数据包,分包状态：
//40 40 + 41 41
//41 41 + 40 40
unsigned int ServerData_Integrity(unsigned char *data,unsigned int lenth)
{
	unsigned int  type=0;
	
	char HeadD1[2]={0x41,0x41};
	char *Pdata=NULL;
	unsigned int GetLength;
	
	if(lenth>25)
	{
		//包头两个字节，包尾两个字节，校验和一个字节。	
		if((data[0]==0x40)&&(data[1]==0x40)&&(data[lenth-1]==0x23)&&(data[lenth-2]==0x23))
		{
			type=1;
			BT_HeartFlag=1;
		}		
		
		//远程升级
		if((data[0]==0x41)&&(data[1]==0x41))
		{ 
			BT_Logic(data,lenth); //关闭远程升级
		}
    else//判断包中间是否有升级包
    {
        Pdata=strstr((char *)data, HeadD1); //IsStrInclude
			  if(Pdata!=NULL)//如果找到41 41
			  {
				 GetLength=*(Pdata+5);//获取长度
				 GetLength=GetLength<<8;	
				 GetLength=GetLength+*(Pdata+4);
					
				 GetLength=GetLength+4;
					
				 if(GetLength>20)
				 {
					 if((*(Pdata+GetLength-2)==0x23)&&(*(Pdata+GetLength-1)==0x23))
					 {BT_Logic((unsigned char *)Pdata,GetLength);}
			   }	
			  }			
    }				
	}
	
	return type;
}
//{
////	 char HeadD1[3]={0x41,0x41,0x00};
////	 char HeadD2[3]={0x40,0x40,0x00};	 
////	 char EndDat[3]={0x23,0x23,0x00};	 	 
////	 char *Pdata=NULL;	
////	 char *Hdata=NULL;	
////	 unsigned int GetLength;
////	  
////	 Pdata=strstr((char *)data, HeadD1); 
////   if(Pdata!=NULL)//如果找到41 41
////	 {
////		 GetLength=*(Pdata+3);
////		 GetLength=GetLength<<8;	
////		 GetLength=GetLength+*(Pdata+2);
////		 GetLength=Hdata-Pdata+2;
////	   BT_Logic((unsigned char *)Pdata,GetLength); 
////	 }
//	 	 
//	if(lenth>25)
//	{
//		//远程升级
//		if((data[0]==0x41)&&(data[1]==0x41))
//		{ 
//			BT_Logic(data,lenth);
//		}		
//		//包头两个字节，包尾两个字节，校验和一个字节。	
//		if((data[0]==0x40)&&(data[1]==0x40)&&(data[lenth-1]==0x23)&&(data[lenth-2]==0x23))
//		{
//			return 1;
//		}
//	}
//	
//	return 0;
//}
unsigned char RecieveFromServer_Handle(void)
{
	unsigned int Flag=0;
	Server_Data_Typedef Data_RecieveFromServer;

	Reset_RecieveFromServerData(&Data_RecieveFromServer);
	Data_RecieveFromServer.Lenth=Net_Rx_Read(Data_RecieveFromServer.data);

	if(Data_RecieveFromServer.Lenth!=0)
    {
		Flag=ServerData_Integrity(Data_RecieveFromServer.data,Data_RecieveFromServer.Lenth);
		if(Flag==0)		//数据错误，请求重发；
		{
			Flag=RecieveFromServer_Error_Action(Data_RecieveFromServer.data,Data_RecieveFromServer.Lenth);
		}
		else
		{
			Flag=RecieveFromServer_Correct_Action(Data_RecieveFromServer.data,Data_RecieveFromServer.Lenth);
		}
    }
	return Flag;
}

