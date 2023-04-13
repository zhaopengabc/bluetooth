#include "frame.h"
#include "includes.h"
#include "lcd.h"

#define Soft_Version 0x10

extern unsigned char Local_SoftVer[2];
extern unsigned char Local_Addres[6];
extern unsigned char g_RunState;

//serial_number:业务流水号。	command：控制单元命令字节。
unsigned int ReportToServer_InformationFrame(unsigned int serial_number,unsigned char *data,unsigned int lenth,unsigned char *destdata)
{
	u16 i=0;
	u8 sum=0;
	
	if(lenth>=SERVER_DATA_NUM)return 0;
	destdata[0]=0x40;
	destdata[1]=0x40;
	destdata[2]=serial_number%256;
	destdata[3]=serial_number/256;
//	destdata[4]=Local_SoftVer[0];		//主版本号
//	destdata[5]=Local_SoftVer[1];		//用户版本号
	destdata[4]=0x01;		         //主版本号
	destdata[5]=Soft_Version;		//用户版本号
	//时间标签	,读取内部RTC的时间。
	RTC_Get((char *)&destdata[6]);
//	dwin_time_read((char *)&destdata[6]);
	//源地址	。监控中心或者用户信息传输装置。
	memcpy(&destdata[12],Local_Addres,6);
	//目的地址	。监控中心或者用户信息传输装置。
	memset(&destdata[18],0,6);
	destdata[24]=lenth%256;			//数据单元长度低位
	destdata[25]=lenth/256;			//数据单元长度高位
	destdata[26]=2;//控制单元的命令字节
	//应用数据单元
	memcpy(&destdata[27],data,lenth);
	for(i=2;i<27+lenth;i++)
	{
		sum=sum+destdata[i];
	}
	destdata[27+lenth]=sum;
	destdata[28+lenth]=35;
	destdata[29+lenth]=35;
	memcpy(destdata,destdata,lenth+30);
	return 30+lenth;
}
//确认否认帧 command=3确认帧，=6否认帧
//serial_number:业务流水号。	command：控制单元命令字节 ，返回数据放到destdata中
unsigned int ReplyToServer_ConfirmationFrame(unsigned int serial_number,unsigned char command,unsigned char *destdata)
{
	u16 i=0;
	u8 sum=0;

	destdata[0]=0x40;
	destdata[1]=0x40;
	destdata[2]=serial_number%256;
	destdata[3]=serial_number/256;
//	destdata[4]=Local_SoftVer[0];
//	destdata[5]=Local_SoftVer[1];
	destdata[4]=0x01;		         //主版本号
	destdata[5]=Soft_Version;		//用户版本号
	RTC_Get((char *)&destdata[6]);
//	dwin_time_read((char *)&destdata[6]);	
	memcpy(&destdata[12],Local_Addres,6);
	memset(&destdata[18],0,6);
	destdata[24]=0;
	destdata[25]=0;
	destdata[26]=command;
	for(i=2;i<27;i++)
	{		
		sum=sum+destdata[i];
	}
	destdata[27]=sum;
	destdata[28]=35;	
	destdata[29]=35;
	memcpy(destdata,destdata,30);
	return 30;
}
//应答，返回查询的信息帧
//返回要发送的数据长度
//serial_number:业务流水号。	command：控制单元命令字节 ，返回数据放到destdata中
unsigned int ReplyToServer_InformationFrame(unsigned int serial_number,unsigned char *data,unsigned int lenth,unsigned char *destdata)
{
	u16 i=0;
	u8 sum=0;
	if(lenth>=SERVER_DATA_NUM-30)return 0;
	destdata[0]=0x40;
	destdata[1]=0x40;
	destdata[2]=serial_number%256;
	destdata[3]=serial_number/256;
//	destdata[4]=Local_SoftVer[0];
//	destdata[5]=Local_SoftVer[1];
	destdata[4]=0x01;		         //主版本号
	destdata[5]=Soft_Version;		 //用户版本号
	
	RTC_Get((char *)&destdata[6]);
//	dwin_time_read((char *)&destdata[6]);	
	memcpy(&destdata[12],Local_Addres,6);
	memset(&destdata[18],0,6);
	destdata[24]=lenth%256;
	destdata[25]=lenth/256;
	destdata[26]=5;					//应答，返回查询信息
	memcpy(&destdata[27],data,lenth);
	for(i=2;i<27+lenth;i++)
	{
		sum=sum+destdata[i];
	}
	destdata[27+lenth]=sum;
	destdata[28+lenth]=35;	
	destdata[29+lenth]=35;
	memcpy(destdata,destdata,lenth+30);
	return 30+lenth;
}
//上传用户信息传输装置运行状态
unsigned char Device_RunningState(unsigned char *buf)
{
	//0:正常监视状态-测试状态  1：火警 
	//2：故障  3：主电故障 4：备电故障
	//5：与监控中心通信故障 6：监测连接线路故障
	buf[0]=21;
	buf[1]=1;
	buf[2]=g_RunState|0x01;//正常监控
	RTC_Get((char *)&buf[3]);
//	dwin_time_read((char *)&buf[3]);	
	return 9;
}
//上传用户信息传输装置操作信息
unsigned char Device_Operation_Information(unsigned char *buf,unsigned char Operation,unsigned char level)
{
	//0:复位  1：消音  2：手动报警
	//3：警情消除  4：自检 
	//5：查岗应答  6：测试
	buf[0]=24;
	buf[1]=1;
	buf[2]=Operation;
	buf[3]=level;
	RTC_Get((char *)&buf[4]);
//	dwin_time_read((char *)&buf[4]);	
	return 10;
}
//上传用户信息传输装置软件版本号
extern const unsigned short VERS_msg;
unsigned char Device_Version(unsigned char *buf)
{
	buf[0]=25;
	buf[1]=1;
//	buf[2]=Local_SoftVer[0];
//	buf[3]=Local_SoftVer[1];
	buf[2]=VERS_msg;
	buf[3]=VERS_msg>>8;
	return 4;
}
//上传用户信息传输装置系统时间
unsigned char Device_SysTime(unsigned char *buf)
{
	buf[0]=28;
	buf[1]=1;
	RTC_Get((char *)&buf[2]);
//	dwin_time_read((char *)&buf[2]);	
	return 8;
}

unsigned int ReportToServer(unsigned char type,unsigned char *data,unsigned int len,unsigned char *destdata)
{
	static unsigned int Tx_FrameCnt=0;
	unsigned int Tx_Cnt=0;
	CPU_SR_ALLOC();
	OS_CRITICAL_ENTER();
	Tx_FrameCnt++;
	if((Tx_FrameCnt&0x0000ffff)==0)Tx_FrameCnt=1;
	Tx_Cnt=Tx_FrameCnt;
	OS_CRITICAL_EXIT();
	len=ReportToServer_InformationFrame(Tx_Cnt,data,len,destdata);
	Data_TransToServer(destdata,len);
	return Tx_FrameCnt;
}

