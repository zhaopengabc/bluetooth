#include "frame.h"
#include "includes.h"
#include "lcd.h"

#define Soft_Version 0x10

extern unsigned char Local_SoftVer[2];
extern unsigned char Local_Addres[6];
extern unsigned char g_RunState;

//serial_number:ҵ����ˮ�š�	command�����Ƶ�Ԫ�����ֽڡ�
unsigned int ReportToServer_InformationFrame(unsigned int serial_number,unsigned char *data,unsigned int lenth,unsigned char *destdata)
{
	u16 i=0;
	u8 sum=0;
	
	if(lenth>=SERVER_DATA_NUM)return 0;
	destdata[0]=0x40;
	destdata[1]=0x40;
	destdata[2]=serial_number%256;
	destdata[3]=serial_number/256;
//	destdata[4]=Local_SoftVer[0];		//���汾��
//	destdata[5]=Local_SoftVer[1];		//�û��汾��
	destdata[4]=0x01;		         //���汾��
	destdata[5]=Soft_Version;		//�û��汾��
	//ʱ���ǩ	,��ȡ�ڲ�RTC��ʱ�䡣
	RTC_Get((char *)&destdata[6]);
//	dwin_time_read((char *)&destdata[6]);
	//Դ��ַ	��������Ļ����û���Ϣ����װ�á�
	memcpy(&destdata[12],Local_Addres,6);
	//Ŀ�ĵ�ַ	��������Ļ����û���Ϣ����װ�á�
	memset(&destdata[18],0,6);
	destdata[24]=lenth%256;			//���ݵ�Ԫ���ȵ�λ
	destdata[25]=lenth/256;			//���ݵ�Ԫ���ȸ�λ
	destdata[26]=2;//���Ƶ�Ԫ�������ֽ�
	//Ӧ�����ݵ�Ԫ
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
//ȷ�Ϸ���֡ command=3ȷ��֡��=6����֡
//serial_number:ҵ����ˮ�š�	command�����Ƶ�Ԫ�����ֽ� ���������ݷŵ�destdata��
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
	destdata[4]=0x01;		         //���汾��
	destdata[5]=Soft_Version;		//�û��汾��
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
//Ӧ�𣬷��ز�ѯ����Ϣ֡
//����Ҫ���͵����ݳ���
//serial_number:ҵ����ˮ�š�	command�����Ƶ�Ԫ�����ֽ� ���������ݷŵ�destdata��
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
	destdata[4]=0x01;		         //���汾��
	destdata[5]=Soft_Version;		 //�û��汾��
	
	RTC_Get((char *)&destdata[6]);
//	dwin_time_read((char *)&destdata[6]);	
	memcpy(&destdata[12],Local_Addres,6);
	memset(&destdata[18],0,6);
	destdata[24]=lenth%256;
	destdata[25]=lenth/256;
	destdata[26]=5;					//Ӧ�𣬷��ز�ѯ��Ϣ
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
//�ϴ��û���Ϣ����װ������״̬
unsigned char Device_RunningState(unsigned char *buf)
{
	//0:��������״̬-����״̬  1���� 
	//2������  3��������� 4���������
	//5����������ͨ�Ź��� 6�����������·����
	buf[0]=21;
	buf[1]=1;
	buf[2]=g_RunState|0x01;//�������
	RTC_Get((char *)&buf[3]);
//	dwin_time_read((char *)&buf[3]);	
	return 9;
}
//�ϴ��û���Ϣ����װ�ò�����Ϣ
unsigned char Device_Operation_Information(unsigned char *buf,unsigned char Operation,unsigned char level)
{
	//0:��λ  1������  2���ֶ�����
	//3����������  4���Լ� 
	//5�����Ӧ��  6������
	buf[0]=24;
	buf[1]=1;
	buf[2]=Operation;
	buf[3]=level;
	RTC_Get((char *)&buf[4]);
//	dwin_time_read((char *)&buf[4]);	
	return 10;
}
//�ϴ��û���Ϣ����װ������汾��
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
//�ϴ��û���Ϣ����װ��ϵͳʱ��
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

