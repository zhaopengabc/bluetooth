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


//��1����ʩϵͳ״̬��Ϣ
typedef struct
{
	HRINT8U cTypeFlag;		 //���ͱ�־��
	HRINT8U cMessageNum;	 //��Ϣ������Ŀ
	
	HRINT8U cMCObjectType;//ϵͳ���ͱ�־��
	HRINT8U cMCaddress;   //ϵͳ��ַ��
	HRINT8U cMCStatus0;    //ϵͳ״̬��
	HRINT8U cMCStatus1;    //ϵͳ״̬��
	
	HRINT8U cTimeIdentification[6];	 //ʱ���ǩ��
}MCSystemStatus;


//��2����ʩ����״̬��Ϣ
typedef struct
{
	HRINT8U cTypeFlag;		 //���ͱ�־��
	HRINT8U cMessageNum;	 //��Ϣ������Ŀ
	
	HRINT8U cMCObjectType;//ϵͳ���ͱ�־��
	HRINT8U cMCaddress;//ϵͳ��ַ��
	HRINT8U cMCDeviceType;//�������ͣ�
	HRINT8U cMCDeviceAddress0; //������ַ��//HRINT32U cMCDeviceAddress; //������ַ��
	HRINT8U cMCDeviceAddress1;
	HRINT8U cMCDeviceAddress2;
	HRINT8U cMCDeviceAddress3;
	HRINT8U cMCDeviceStatus0;//����״̬��
	HRINT8U cMCDeviceStatus1;//����״̬��
 	HRINT8U cMCDeviceIllustrate[31];//����˵����
	
	HRINT8U cTimeIdentification[6];	 //ʱ���ǩ��
}MCSystemDeviceStatus;

//��4����ʩ������Ϣ
typedef struct
{
	HRINT8U cTypeFlag;		 //���ͱ�־��
	HRINT8U cMessageNum;	 //��Ϣ������Ŀ
	
	HRINT8U  cMCObjectType;//ϵͳ���ͱ�־��
	HRINT8U  cMCaddress; //ϵͳ��ַ��
	HRINT8U  cMCOperationType;//������־��
 	HRINT8U  cMCOperator;//����Ա��ţ�
	
	HRINT8U cTimeIdentification[6];	 //ʱ���ǩ��
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
	HRINT8U MCaddress;		 //���ͱ�־��
	HRINT8U MCOperationType;	 //��Ϣ������Ŀ
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

 
///////////�ӿں���/////////////
extern u8 RecieveMCSystemStatusAndSend(HRINT8U MCaddress,//������ַ��
																	HRINT8U MCStatus0,//����״̬��
                                  HRINT8U MCStatus1,//����״̬��
                                  HRINT8U MCTimeDataSec,//����ʱ�䣬�������û��ʱ����Ϣ�����0x00��
                                  HRINT8U MCTimeDataMin,//����ʱ�䣬�������û��ʱ����Ϣ�����0x00��
                                  HRINT8U MCTimeDataHor,//����ʱ�䣬�������û��ʱ����Ϣ�����0x00��
                                  HRINT8U MCTimeDataDay,//����ʱ�䣬�������û��ʱ����Ϣ�����0x00��
                                  HRINT8U MCTimeDataMon,//����ʱ�䣬�������û��ʱ����Ϣ�����0x00��
                                  HRINT8U MCTimeDataYer);//����ʱ�䣬�������û��ʱ����Ϣ�����0x00��
extern u8 RecieveMCDeviceStatusAndSend(HRINT8U MCaddress,//������ַ��
																	HRINT8U MCDeviceAddress0,//��·���ֽڣ�
                                  HRINT8U MCDeviceAddress1,//��·���ֽڣ�
                                  HRINT8U MCDeviceAddress2,//��ַ���ֽڣ�
                                  HRINT8U MCDeviceAddress3,//��ַ���ֽڣ�
                                  HRINT8U MCDeviceStatus0,//�豸״̬���ο����ꣻ
                                  HRINT8U MCDeviceStatus1,//�豸״̬���ο����ꣻ
                                  HRINT8U MCTimeDataSec,//����ʱ�䣬�������û��ʱ����Ϣ�����0x00��
                                  HRINT8U MCTimeDataMin,//����ʱ�䣬�������û��ʱ����Ϣ�����0x00��
                                  HRINT8U MCTimeDataHor,//����ʱ�䣬�������û��ʱ����Ϣ�����0x00��
                                  HRINT8U MCTimeDataDay,//����ʱ�䣬�������û��ʱ����Ϣ�����0x00��
                                  HRINT8U MCTimeDataMon,//����ʱ�䣬�������û��ʱ����Ϣ�����0x00��
                                  HRINT8U MCTimeDataYer,//����ʱ�䣬�������û��ʱ����Ϣ�����0x00��
                                  HRINT8U MCDevicetype1,//�����豸���ͣ�
                                  HRINT8U MCDevicetype2,//ԭʼ�豸���ͣ�
                                  HRINT32U MCDeviceCode //�û������
                                  //HRINT8U *pMCDeviceIllustrate
                                 ); //����������ԭʼ���ĵ��׵�ַ�������ָ�룬��С30������䵽����˵���Ϸ���
extern u8 RecieveMCOperationStatusAndSend(HRINT8U MCaddress,//������ַ��
	                                   HRINT8U MCOperationType,//������Ϣ��
                                     HRINT8U MCTimeDataSec,
                                     HRINT8U MCTimeDataMin,
                                     HRINT8U MCTimeDataHor,
                                     HRINT8U MCTimeDataDay,
                                     HRINT8U MCTimeDataMon,
                                     HRINT8U MCTimeDataYer);//����״̬������
extern unsigned char BCD2HEX(unsigned char BCDdata);
extern void Device_MSOSQCreate(void);
extern void Device_MSOSQRestart(void);

#endif
