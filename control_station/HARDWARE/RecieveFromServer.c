#include "RecieveFromServer.h"
#include "string.h"
#include <stdio.h>
#include "frame.h"
#include "net.h"
#include "ServerAction.h"
#include "rtc.h"
#include "lcd.h"
#include "ServerAction.h"
extern unsigned char correction_time_way;//У��ʱ�䷽ʽ����Ϊ�ֶ����Զ���0:�ֶ���1���Զ���
void Reset_RecieveFromServerData(Server_Data_Typedef *pServer_Data)
{
	memset(pServer_Data->data,0,SERVER_DATA_NUM);
	pServer_Data->Lenth=0;
	pServer_Data->Flag=0;
}
//���ݲ���ȷ,���ͷ���֡�������ط���
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
//��ʼ��ϵͳӦ���ϴ���Ϣ���ٽ�������������ʱ���趨Ϊ�յ�������10s
unsigned int InitCmd(unsigned char *data,unsigned int lenth)
{
	if((data[28]==1)&&(data[24]==3)&&(data[25]==0))//��ʼ��,û�г�ʼ��ֻ�������ˣ�Ӧ����FLASH������
	{
		return 1;
	}
	return 0;
}
//�����������
//����0����Ҫ�ظ�ȷ�ϣ�����1�ظ�ȷ��֡
unsigned int ServerData_Control(unsigned char *data,unsigned int lenth,unsigned char *destdata)
{
	unsigned int len=0;
	switch(data[27])
	{
		//Ѳ��
		case 81:
			//�رղ�ڣ�
      ChaGangYingDa_stop();
		  //�Ϸ����������Ϸ�״̬
			HeartBeatFrame(); 
			break;		
		//��ʼ���û���Ϣ����װ��
		case 89:
			len=InitCmd(data,lenth);
			break;
		//ͬ��ʱ�ӣ�
		case 90:
			if((data[28]==1)&&(data[24]==8)&&(data[25]==0))
			{
//				RTC_Set((char *)&data[29]);
//				dwin_time_set((unsigned char *)&data[29]);
			}
			len=1;
			break;
		//������
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
//�����������
unsigned int ServerData_Request(unsigned char *data,unsigned int lenth,unsigned char *destdata)
{
	unsigned int len=0;
	unsigned char destdataA[10]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	
	switch(data[27])
	{
		//��ȡ�û���Ϣ����װ������״̬
		case 81:
			if((data[28]==1)&&(data[24]==3)&&(data[25]==0))
			{
				len=Device_RunningState(destdataA);
			}
			break;
		//��ȡ�û���Ϣ����װ�ò�����Ϣ��¼
		case 84:
			if((data[28]==1)&&(data[24]==9)&&(data[25]==0))
			{
				
			}
			break;
		//��ȡ�û���Ϣ����װ������汾
		case 85:
			if((data[28]==1)&&(data[24]==3)&&(data[25]==0))
			{
				len=Device_Version(destdataA);
			}
			break;
		//��ȡ�û���Ϣ����װ���������
		case 86:
			if((data[28]==1)&&(data[24]==3)&&(data[25]==0))
			{
				
			}
			break;
		//��ȡ�û���Ϣ����װ��ϵͳʱ��
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
//���ݽ���������������Ż�destdata�У����ؽ�������Ҫִ�еĲ���
unsigned int ServerData_Analysis(unsigned char *data,unsigned int lenth,unsigned char *destdata)
{
	unsigned int len=0;
	char rtcdata[0x20];
	unsigned char flag=0;
	unsigned char i=0;
	unsigned char checksum=0;
	
	switch(data[26])//ͨ���ж������������鿴ÿ��������Ԫ�صĸ���
	{
	 case 1://��������
			len=ServerData_Control(data,lenth,destdata);
			break;
	 case 3://ȷ��,�ж�ҵ����ˮ���Ƿ�һ�����һ�£���������ͻ���
//		  printf("ȷ�Ϻ�=%d��\n\r",(data[2]+data[3]*256));
			IsEquel(data[2]+data[3]*256);
			//��У�����ȷ�����ж�
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
	 case 4://����
			len=ServerData_Request(data,lenth,destdata);
			break;
	 case 6://����
			len=0;
			//��Ϊ����֡��������������ͣ�
			break;
	 default:
			break;
	}
	return len;
}
//�Խ���������ִ������
unsigned char Execute_Command(unsigned char *data,unsigned int len)
{
	return 1;
}
//������ȷ
unsigned char RecieveFromServer_Correct_Action(unsigned char *data,unsigned int lenth)
{
	static unsigned char destdata[SERVER_DATA_NUM];
	unsigned int len=0;
	
	//���ݽ����ֽⲢִ����Ӧ����
	len=ServerData_Analysis(data,lenth,destdata);
	if(len!=0)
	{
		Data_TransToServer(destdata,len);
		return 1;
	}
	return 0;
}
//������������ȷ���ж�������ȷ������ҵ����ˮ�ţ�����ȷ������0��
unsigned char BT_Logic(unsigned char *data,unsigned int dataLength);
extern unsigned char BT_HeartFlag;
//���ݷְ���ͬʱ�յ�40 40��41 41��ͷ�����ݰ�,�ְ�״̬��
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
		//��ͷ�����ֽڣ���β�����ֽڣ�У���һ���ֽڡ�	
		if((data[0]==0x40)&&(data[1]==0x40)&&(data[lenth-1]==0x23)&&(data[lenth-2]==0x23))
		{
			type=1;
			BT_HeartFlag=1;
		}		
		
		//Զ������
		if((data[0]==0x41)&&(data[1]==0x41))
		{ 
			BT_Logic(data,lenth); //�ر�Զ������
		}
    else//�жϰ��м��Ƿ���������
    {
        Pdata=strstr((char *)data, HeadD1); //IsStrInclude
			  if(Pdata!=NULL)//����ҵ�41 41
			  {
				 GetLength=*(Pdata+5);//��ȡ����
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
////   if(Pdata!=NULL)//����ҵ�41 41
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
//		//Զ������
//		if((data[0]==0x41)&&(data[1]==0x41))
//		{ 
//			BT_Logic(data,lenth);
//		}		
//		//��ͷ�����ֽڣ���β�����ֽڣ�У���һ���ֽڡ�	
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
		if(Flag==0)		//���ݴ��������ط���
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

