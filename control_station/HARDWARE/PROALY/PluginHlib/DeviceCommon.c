#include "DeviceCommon.h"
#include "rtc.h"
#include "app.h"
#include "SDcontrol.h"

MCSystemStatus iMCSystemStatus={0};
MCSystemDeviceStatus iMCSystemDeviceStatus={0};
MCSystemOperationStatus iMCSystemOperationStatus={0};

u8 RecieveMCSystemStatusAndSend(HRINT8U MCaddress,
																	HRINT8U MCStatus0,
                                  HRINT8U MCStatus1,
                                  HRINT8U MCTimeDataSec,
                                  HRINT8U MCTimeDataMin,
                                  HRINT8U MCTimeDataHor,
                                  HRINT8U MCTimeDataDay,
                                  HRINT8U MCTimeDataMon,
                                  HRINT8U MCTimeDataYer)//系统状态反馈；
{
	if((MCStatus0==0)&&(MCStatus1==0))
	{ return 0; }
	
  iMCSystemStatus.cTypeFlag=0x01; //类型标志；系统状态；
	iMCSystemStatus.cMessageNum=0x01; //信息对象数目;1个；
	
	iMCSystemStatus.cMCObjectType=0x01;//系统类型标志 火灾报警器；
	//系统地址；//iMCSystemStatus.cMCaddress=MCaddress; 
   iMCSystemStatus.cMCaddress=PushButtonStatus<<4;	 
	 if(MCaddress==0)//未解析；
	 {iMCSystemStatus.cMCaddress=iMCSystemStatus.cMCaddress+1;
	 }
	 else if (MCaddress>15)//大于15号机，则为15号机；
	 {iMCSystemStatus.cMCaddress=iMCSystemStatus.cMCaddress+15;
	 }//主机地址；
	 else
	 {iMCSystemStatus.cMCaddress=iMCSystemStatus.cMCaddress+MCaddress;
	 }//1~15号机；
	 
  iMCSystemStatus.cMCStatus0=MCStatus0;
	iMCSystemStatus.cMCStatus1=MCStatus1;

	//时间信息获取；
	iMCSystemStatus.cTimeIdentification[5]=MCTimeDataYer;//年
	iMCSystemStatus.cTimeIdentification[4]=MCTimeDataMon;//月
	iMCSystemStatus.cTimeIdentification[3]=MCTimeDataDay;//日	
	iMCSystemStatus.cTimeIdentification[2]=MCTimeDataHor;//小时
	iMCSystemStatus.cTimeIdentification[1]=MCTimeDataMin;//分
	iMCSystemStatus.cTimeIdentification[0]=MCTimeDataSec;//秒 
		
	gMessageSent((HRINT8U *)&iMCSystemStatus,12u);
}

u8 RecieveMCDeviceStatusAndSend(HRINT8U MCaddress,
																	HRINT8U MCDeviceAddress0,
                                  HRINT8U MCDeviceAddress1,
                                  HRINT8U MCDeviceAddress2,
                                  HRINT8U MCDeviceAddress3,
                                  HRINT8U MCDeviceStatus0,
                                  HRINT8U MCDeviceStatus1,
                                  HRINT8U MCTimeDataSec,
                                  HRINT8U MCTimeDataMin,
                                  HRINT8U MCTimeDataHor,
                                  HRINT8U MCTimeDataDay,
                                  HRINT8U MCTimeDataMon,
                                  HRINT8U MCTimeDataYer,
                                  HRINT8U MCDevicetype1,
                                  HRINT8U MCDevicetype2,
                                  HRINT32U MCDeviceCode
                                  ) //设备状态反馈；
{
	HRINT8U i;
	if((MCDeviceStatus0==0)&&(MCDeviceStatus1==0))
	{ return 0; }
	
  iMCSystemDeviceStatus.cTypeFlag=0x02; //类型标志；部件状态；
	iMCSystemDeviceStatus.cMessageNum=0x01; //信息对象数目;1个;
	
	iMCSystemDeviceStatus.cMCObjectType=0x01;//系统类型标志 火灾报警器；
   //系统地址；	iMCSystemDeviceStatus.cMCaddress=MCaddress; //主机地址；
   iMCSystemDeviceStatus.cMCaddress=PushButtonStatus<<4;	 
	 if(MCaddress==0)//未解析；
	 {iMCSystemDeviceStatus.cMCaddress=iMCSystemDeviceStatus.cMCaddress+1;
	 }
	 else if (MCaddress>15)//大于15号机，则为15号机；
	 {iMCSystemDeviceStatus.cMCaddress=iMCSystemDeviceStatus.cMCaddress+15;
	 }//主机地址；
	 else
	 {iMCSystemDeviceStatus.cMCaddress=iMCSystemDeviceStatus.cMCaddress+MCaddress;
	 }//1~15号机；
	
	iMCSystemDeviceStatus.cMCDeviceType=MCDevicetype1;       //获取类型；	
	
	iMCSystemDeviceStatus.cMCDeviceAddress0=MCDeviceAddress0;//设备地址；
	iMCSystemDeviceStatus.cMCDeviceAddress1=MCDeviceAddress1;//设备地址；
	iMCSystemDeviceStatus.cMCDeviceAddress2=MCDeviceAddress2;//设备地址；
	iMCSystemDeviceStatus.cMCDeviceAddress3=MCDeviceAddress3;//设备地址；

  iMCSystemDeviceStatus.cMCDeviceStatus0=MCDeviceStatus0; 				
  iMCSystemDeviceStatus.cMCDeviceStatus1=MCDeviceStatus1; 

	for(i=0;i<31;i++)//31个数据部件说明,清零；
	{iMCSystemDeviceStatus.cMCDeviceIllustrate[i]=0x00;}

if(MCDeviceCode!=0)	
{
  //说明符31B	上传编码表命令；
	iMCSystemDeviceStatus.cMCDeviceIllustrate[0]=0xB8;
	iMCSystemDeviceStatus.cMCDeviceIllustrate[1]=0x01;//信息体1个；
	iMCSystemDeviceStatus.cMCDeviceIllustrate[2]=0x01;//类型为：用户编码；
	iMCSystemDeviceStatus.cMCDeviceIllustrate[3]=0x04;//4个字节；
	iMCSystemDeviceStatus.cMCDeviceIllustrate[4]=MCDeviceCode;	
	iMCSystemDeviceStatus.cMCDeviceIllustrate[5]=MCDeviceCode>>8;	
	iMCSystemDeviceStatus.cMCDeviceIllustrate[6]=MCDeviceCode>>16;
	iMCSystemDeviceStatus.cMCDeviceIllustrate[7]=MCDeviceCode>>24;	
}
  //时间；
	iMCSystemDeviceStatus.cTimeIdentification[5]=MCTimeDataYer;//年
	iMCSystemDeviceStatus.cTimeIdentification[4]=MCTimeDataMon;//月
	iMCSystemDeviceStatus.cTimeIdentification[3]=MCTimeDataDay;//日	
	iMCSystemDeviceStatus.cTimeIdentification[2]=MCTimeDataHor;//小时
	iMCSystemDeviceStatus.cTimeIdentification[1]=MCTimeDataMin;//分
	iMCSystemDeviceStatus.cTimeIdentification[0]=MCTimeDataSec;//秒 
	//数据发送；	
	gMessageSent((HRINT8U *)&iMCSystemDeviceStatus,48u); 		
}

u8 RecieveMCOperationStatusAndSend  (HRINT8U MCaddress,
	                                   HRINT8U MCOperationType,
                                     HRINT8U MCTimeDataSec,
                                     HRINT8U MCTimeDataMin,
                                     HRINT8U MCTimeDataHor,
                                     HRINT8U MCTimeDataDay,
                                     HRINT8U MCTimeDataMon,
                                     HRINT8U MCTimeDataYer)//操作状态反馈；
{
	if(MCOperationType==0)
	{ return 0; }
	
	iMCSystemOperationStatus.cTypeFlag=0x04;     //类型标志；操作状态；
	iMCSystemOperationStatus.cMessageNum=0x01;	 //信息对象数目;
	
	 iMCSystemOperationStatus.cMCObjectType=0x01;//系统类型标志 火灾报警器； 
    //系统地址；	iMCSystemOperationStatus.cMCaddress=MCaddress;//系统地址；	
   iMCSystemOperationStatus.cMCaddress=PushButtonStatus<<4;	 
	 if(MCaddress==0)//未解析；
	 {iMCSystemOperationStatus.cMCaddress=iMCSystemOperationStatus.cMCaddress+1;
	 }
	 else if (MCaddress>15)//大于15号机，则为15号机；
	 {iMCSystemOperationStatus.cMCaddress=iMCSystemOperationStatus.cMCaddress+15;
	 }//主机地址；
	 else
	 {iMCSystemOperationStatus.cMCaddress=iMCSystemOperationStatus.cMCaddress+MCaddress;
	 }//1~15号机；

	iMCSystemOperationStatus.cMCOperationType=MCOperationType;//操作标志；	
	iMCSystemOperationStatus.cMCOperator=0x00;//操作员编号；
		
	iMCSystemOperationStatus.cTimeIdentification[5]=MCTimeDataYer;//年
	iMCSystemOperationStatus.cTimeIdentification[4]=MCTimeDataMon;//月
	iMCSystemOperationStatus.cTimeIdentification[3]=MCTimeDataDay;//日	
	iMCSystemOperationStatus.cTimeIdentification[2]=MCTimeDataHor;//小时
	iMCSystemOperationStatus.cTimeIdentification[1]=MCTimeDataMin;//分
	iMCSystemOperationStatus.cTimeIdentification[0]=MCTimeDataSec;//秒	
 
	gMessageSent((HRINT8U *)&iMCSystemOperationStatus, (12u) );		
}

unsigned char BCD2HEX(unsigned char BCDdata)
{
	return ((BCDdata>>4)*10+(BCDdata&0x0f));
}
 
 
