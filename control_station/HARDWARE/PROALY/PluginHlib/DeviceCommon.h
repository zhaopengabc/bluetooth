#ifndef  DeviceCommon_DEF_SET
#define  DeviceCommon_DEF_SET

//DeviceCommon.h
#include "HRSTDDEF.h"
#include <includes.h>

#define CP_RecieveMCSystemStatus_CMD          (0x01u)
#define CP_RecieveMCDeviceStatus_CMD          (0x02u)
#define CP_RecieveMCOperationStatus_CMD       (0x04u)

typedef	void (*PLUGIN_Install)(void); //Create a plugin
typedef	void (*PLUGIN_Run)(void);     //Run a plugin 
typedef	void (*PLUGIN_Interrupt)(void);     //Run a plugin 
	
typedef struct
{
	PLUGIN_Install cPLUGIN_Install; //Create a plugin
	PLUGIN_Run cPLUGIN_Run;     //Run a plugin 
	PLUGIN_Interrupt cPLUGIN_Interrupt;     //Run a plugin 
}PluginInterface;


//（1）设施系统状态信息
typedef struct
{
	HRINT8U cTypeFlag;		 //类型标志；
	HRINT8U cMessageNum;	 //信息对象数目
	
	HRINT8U cMCObjectType;//系统类型标志；
	HRINT8U cMCaddress;   //系统地址；
	HRINT8U cMCStatus0;    //系统状态；
	HRINT8U cMCStatus1;    //系统状态；
	
	HRINT8U cTimeIdentification[6];	 //时间标签；
}MCSystemStatus;


//（2）设施部件状态信息
typedef struct
{
	HRINT8U cTypeFlag;		 //类型标志；
	HRINT8U cMessageNum;	 //信息对象数目
	
	HRINT8U cMCObjectType;//系统类型标志；
	HRINT8U cMCaddress;//系统地址；
	HRINT8U cMCDeviceType;//部件类型；
	HRINT8U cMCDeviceAddress0; //部件地址；//HRINT32U cMCDeviceAddress; //部件地址；
	HRINT8U cMCDeviceAddress1;
	HRINT8U cMCDeviceAddress2;
	HRINT8U cMCDeviceAddress3;
	HRINT8U cMCDeviceStatus0;//部件状态；
	HRINT8U cMCDeviceStatus1;//部件状态；
 	HRINT8U cMCDeviceIllustrate[31];//部件说明；
	
	HRINT8U cTimeIdentification[6];	 //时间标签；
}MCSystemDeviceStatus;

//（4）设施操作信息
typedef struct
{
	HRINT8U cTypeFlag;		 //类型标志；
	HRINT8U cMessageNum;	 //信息对象数目
	
	HRINT8U  cMCObjectType;//系统类型标志；
	HRINT8U  cMCaddress; //系统地址；
	HRINT8U  cMCOperationType;//操作标志；
 	HRINT8U  cMCOperator;//操作员编号；
	
	HRINT8U cTimeIdentification[6];	 //时间标签；
}MCSystemOperationStatus;

typedef struct
{
	HRINT8U MCaddress;
	HRINT8U MCStatus0;
  HRINT8U MCStatus1;
  HRINT8U MCTimeDataSec;
  HRINT8U MCTimeDataMin;
  HRINT8U MCTimeDataHor;
  HRINT8U MCTimeDataDay;
  HRINT8U MCTimeDataMon;
  HRINT8U MCTimeDataYer;
}tMCSystemStatus;

typedef struct
{
  HRINT8U MCaddress;
	HRINT8U MCDeviceAddress0;
  HRINT8U MCDeviceAddress1;
  HRINT8U MCDeviceAddress2;
  HRINT8U MCDeviceAddress3;
  HRINT8U MCDeviceStatus0;
  HRINT8U MCDeviceStatus1;
  HRINT8U MCTimeDataSec;
  HRINT8U MCTimeDataMin;
  HRINT8U MCTimeDataHor;
  HRINT8U MCTimeDataDay;
  HRINT8U MCTimeDataMon;
  HRINT8U MCTimeDataYer;
	HRINT8U MCDevicetype1;
	HRINT8U MCDevicetype2;
  HRINT32U MCDeviceCode;
}tMCSystemDeviceStatus;

typedef struct
{
	HRINT8U MCaddress;		 //类型标志；
	HRINT8U MCOperationType;	 //信息对象数目
	HRINT8U MCTimeDataSec;
  HRINT8U MCTimeDataMin;
  HRINT8U MCTimeDataHor;
  HRINT8U MCTimeDataDay;
  HRINT8U MCTimeDataMon;
  HRINT8U MCTimeDataYer;
}tMCSystemOperationStatus; 


extern MCSystemStatus iMCSystemStatus;
extern MCSystemDeviceStatus iMCSystemDeviceStatus;
extern MCSystemOperationStatus iMCSystemOperationStatus;

 
///////////接口函数/////////////
extern u8 RecieveMCSystemStatusAndSend(HRINT8U MCaddress,//主机地址；
																	HRINT8U MCStatus0,//主机状态；
                                  HRINT8U MCStatus1,//主机状态；
                                  HRINT8U MCTimeDataSec,//解析时间，如果主机没有时间信息，填充0x00；
                                  HRINT8U MCTimeDataMin,//解析时间，如果主机没有时间信息，填充0x00；
                                  HRINT8U MCTimeDataHor,//解析时间，如果主机没有时间信息，填充0x00；
                                  HRINT8U MCTimeDataDay,//解析时间，如果主机没有时间信息，填充0x00；
                                  HRINT8U MCTimeDataMon,//解析时间，如果主机没有时间信息，填充0x00；
                                  HRINT8U MCTimeDataYer);//解析时间，如果主机没有时间信息，填充0x00；
extern u8 RecieveMCDeviceStatusAndSend(HRINT8U MCaddress,//主机地址；
																	HRINT8U MCDeviceAddress0,//回路低字节；
                                  HRINT8U MCDeviceAddress1,//回路高字节；
                                  HRINT8U MCDeviceAddress2,//地址低字节；
                                  HRINT8U MCDeviceAddress3,//地址高字节；
                                  HRINT8U MCDeviceStatus0,//设备状态，参考国标；
                                  HRINT8U MCDeviceStatus1,//设备状态，参考国标；
                                  HRINT8U MCTimeDataSec,//解析时间，如果主机没有时间信息，填充0x00；
                                  HRINT8U MCTimeDataMin,//解析时间，如果主机没有时间信息，填充0x00；
                                  HRINT8U MCTimeDataHor,//解析时间，如果主机没有时间信息，填充0x00；
                                  HRINT8U MCTimeDataDay,//解析时间，如果主机没有时间信息，填充0x00；
                                  HRINT8U MCTimeDataMon,//解析时间，如果主机没有时间信息，填充0x00；
                                  HRINT8U MCTimeDataYer,//解析时间，如果主机没有时间信息，填充0x00；
                                  HRINT8U MCDevicetype1,//国标设备类型；
                                  HRINT8U MCDevicetype2,//原始设备类型；
                                  HRINT32U MCDeviceCode //用户编码表；
                                  //HRINT8U *pMCDeviceIllustrate
                                 ); //将主机发的原始报文的首地址放入这个指针，大小30个，填充到部件说明上发；
extern u8 RecieveMCOperationStatusAndSend(HRINT8U MCaddress,//主机地址；
	                                   HRINT8U MCOperationType,//操作信息；
                                     HRINT8U MCTimeDataSec,
                                     HRINT8U MCTimeDataMin,
                                     HRINT8U MCTimeDataHor,
                                     HRINT8U MCTimeDataDay,
                                     HRINT8U MCTimeDataMon,
                                     HRINT8U MCTimeDataYer);//操作状态反馈；
extern unsigned char BCD2HEX(unsigned char BCDdata);
extern void Device_MSOSQCreate(void);
extern void Device_MSOSQRestart(void);

#endif
