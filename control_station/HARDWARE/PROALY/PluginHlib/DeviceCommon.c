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
                                  HRINT8U MCTimeDataYer)//ϵͳ״̬������
{
	if((MCStatus0==0)&&(MCStatus1==0))
	{ return 0; }
	
  iMCSystemStatus.cTypeFlag=0x01; //���ͱ�־��ϵͳ״̬��
	iMCSystemStatus.cMessageNum=0x01; //��Ϣ������Ŀ;1����
	
	iMCSystemStatus.cMCObjectType=0x01;//ϵͳ���ͱ�־ ���ֱ�������
	//ϵͳ��ַ��//iMCSystemStatus.cMCaddress=MCaddress; 
   iMCSystemStatus.cMCaddress=PushButtonStatus<<4;	 
	 if(MCaddress==0)//δ������
	 {iMCSystemStatus.cMCaddress=iMCSystemStatus.cMCaddress+1;
	 }
	 else if (MCaddress>15)//����15�Ż�����Ϊ15�Ż���
	 {iMCSystemStatus.cMCaddress=iMCSystemStatus.cMCaddress+15;
	 }//������ַ��
	 else
	 {iMCSystemStatus.cMCaddress=iMCSystemStatus.cMCaddress+MCaddress;
	 }//1~15�Ż���
	 
  iMCSystemStatus.cMCStatus0=MCStatus0;
	iMCSystemStatus.cMCStatus1=MCStatus1;

	//ʱ����Ϣ��ȡ��
	iMCSystemStatus.cTimeIdentification[5]=MCTimeDataYer;//��
	iMCSystemStatus.cTimeIdentification[4]=MCTimeDataMon;//��
	iMCSystemStatus.cTimeIdentification[3]=MCTimeDataDay;//��	
	iMCSystemStatus.cTimeIdentification[2]=MCTimeDataHor;//Сʱ
	iMCSystemStatus.cTimeIdentification[1]=MCTimeDataMin;//��
	iMCSystemStatus.cTimeIdentification[0]=MCTimeDataSec;//�� 
		
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
                                  ) //�豸״̬������
{
	HRINT8U i;
	if((MCDeviceStatus0==0)&&(MCDeviceStatus1==0))
	{ return 0; }
	
  iMCSystemDeviceStatus.cTypeFlag=0x02; //���ͱ�־������״̬��
	iMCSystemDeviceStatus.cMessageNum=0x01; //��Ϣ������Ŀ;1��;
	
	iMCSystemDeviceStatus.cMCObjectType=0x01;//ϵͳ���ͱ�־ ���ֱ�������
   //ϵͳ��ַ��	iMCSystemDeviceStatus.cMCaddress=MCaddress; //������ַ��
   iMCSystemDeviceStatus.cMCaddress=PushButtonStatus<<4;	 
	 if(MCaddress==0)//δ������
	 {iMCSystemDeviceStatus.cMCaddress=iMCSystemDeviceStatus.cMCaddress+1;
	 }
	 else if (MCaddress>15)//����15�Ż�����Ϊ15�Ż���
	 {iMCSystemDeviceStatus.cMCaddress=iMCSystemDeviceStatus.cMCaddress+15;
	 }//������ַ��
	 else
	 {iMCSystemDeviceStatus.cMCaddress=iMCSystemDeviceStatus.cMCaddress+MCaddress;
	 }//1~15�Ż���
	
	iMCSystemDeviceStatus.cMCDeviceType=MCDevicetype1;       //��ȡ���ͣ�	
	
	iMCSystemDeviceStatus.cMCDeviceAddress0=MCDeviceAddress0;//�豸��ַ��
	iMCSystemDeviceStatus.cMCDeviceAddress1=MCDeviceAddress1;//�豸��ַ��
	iMCSystemDeviceStatus.cMCDeviceAddress2=MCDeviceAddress2;//�豸��ַ��
	iMCSystemDeviceStatus.cMCDeviceAddress3=MCDeviceAddress3;//�豸��ַ��

  iMCSystemDeviceStatus.cMCDeviceStatus0=MCDeviceStatus0; 				
  iMCSystemDeviceStatus.cMCDeviceStatus1=MCDeviceStatus1; 

	for(i=0;i<31;i++)//31�����ݲ���˵��,���㣻
	{iMCSystemDeviceStatus.cMCDeviceIllustrate[i]=0x00;}

if(MCDeviceCode!=0)	
{
  //˵����31B	�ϴ���������
	iMCSystemDeviceStatus.cMCDeviceIllustrate[0]=0xB8;
	iMCSystemDeviceStatus.cMCDeviceIllustrate[1]=0x01;//��Ϣ��1����
	iMCSystemDeviceStatus.cMCDeviceIllustrate[2]=0x01;//����Ϊ���û����룻
	iMCSystemDeviceStatus.cMCDeviceIllustrate[3]=0x04;//4���ֽڣ�
	iMCSystemDeviceStatus.cMCDeviceIllustrate[4]=MCDeviceCode;	
	iMCSystemDeviceStatus.cMCDeviceIllustrate[5]=MCDeviceCode>>8;	
	iMCSystemDeviceStatus.cMCDeviceIllustrate[6]=MCDeviceCode>>16;
	iMCSystemDeviceStatus.cMCDeviceIllustrate[7]=MCDeviceCode>>24;	
}
  //ʱ�䣻
	iMCSystemDeviceStatus.cTimeIdentification[5]=MCTimeDataYer;//��
	iMCSystemDeviceStatus.cTimeIdentification[4]=MCTimeDataMon;//��
	iMCSystemDeviceStatus.cTimeIdentification[3]=MCTimeDataDay;//��	
	iMCSystemDeviceStatus.cTimeIdentification[2]=MCTimeDataHor;//Сʱ
	iMCSystemDeviceStatus.cTimeIdentification[1]=MCTimeDataMin;//��
	iMCSystemDeviceStatus.cTimeIdentification[0]=MCTimeDataSec;//�� 
	//���ݷ��ͣ�	
	gMessageSent((HRINT8U *)&iMCSystemDeviceStatus,48u); 		
}

u8 RecieveMCOperationStatusAndSend  (HRINT8U MCaddress,
	                                   HRINT8U MCOperationType,
                                     HRINT8U MCTimeDataSec,
                                     HRINT8U MCTimeDataMin,
                                     HRINT8U MCTimeDataHor,
                                     HRINT8U MCTimeDataDay,
                                     HRINT8U MCTimeDataMon,
                                     HRINT8U MCTimeDataYer)//����״̬������
{
	if(MCOperationType==0)
	{ return 0; }
	
	iMCSystemOperationStatus.cTypeFlag=0x04;     //���ͱ�־������״̬��
	iMCSystemOperationStatus.cMessageNum=0x01;	 //��Ϣ������Ŀ;
	
	 iMCSystemOperationStatus.cMCObjectType=0x01;//ϵͳ���ͱ�־ ���ֱ������� 
    //ϵͳ��ַ��	iMCSystemOperationStatus.cMCaddress=MCaddress;//ϵͳ��ַ��	
   iMCSystemOperationStatus.cMCaddress=PushButtonStatus<<4;	 
	 if(MCaddress==0)//δ������
	 {iMCSystemOperationStatus.cMCaddress=iMCSystemOperationStatus.cMCaddress+1;
	 }
	 else if (MCaddress>15)//����15�Ż�����Ϊ15�Ż���
	 {iMCSystemOperationStatus.cMCaddress=iMCSystemOperationStatus.cMCaddress+15;
	 }//������ַ��
	 else
	 {iMCSystemOperationStatus.cMCaddress=iMCSystemOperationStatus.cMCaddress+MCaddress;
	 }//1~15�Ż���

	iMCSystemOperationStatus.cMCOperationType=MCOperationType;//������־��	
	iMCSystemOperationStatus.cMCOperator=0x00;//����Ա��ţ�
		
	iMCSystemOperationStatus.cTimeIdentification[5]=MCTimeDataYer;//��
	iMCSystemOperationStatus.cTimeIdentification[4]=MCTimeDataMon;//��
	iMCSystemOperationStatus.cTimeIdentification[3]=MCTimeDataDay;//��	
	iMCSystemOperationStatus.cTimeIdentification[2]=MCTimeDataHor;//Сʱ
	iMCSystemOperationStatus.cTimeIdentification[1]=MCTimeDataMin;//��
	iMCSystemOperationStatus.cTimeIdentification[0]=MCTimeDataSec;//��	
 
	gMessageSent((HRINT8U *)&iMCSystemOperationStatus, (12u) );		
}

unsigned char BCD2HEX(unsigned char BCDdata)
{
	return ((BCDdata>>4)*10+(BCDdata&0x0f));
}
 
 
