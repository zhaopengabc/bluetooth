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

//��������
ProtocolBoardRegistered S_ProtocolBoardRegistered;   //Э���ע��ṹ��
InspectState S_InspectState[15];					 //Ѳ��״̬�ṹ������
unsigned int Inspect_TimCount=0;					 //Ѳ�������ʱ������
unsigned int Inspect_NowNumber=1;					 //Ѳ�쵱ǰЭ�����
unsigned char Registered_Allow=1;					 //������������ע���־λ

Registered_InforDataStr S_Registered_InforDataStr[ProtocolBoard_CNT];	//Э���ע����Ϣ
RegisteredState 		S_RegisteredState[ProtocolBoard_CNT];			//Э���ע��״̬�ṹ��			
unsigned int 			Registered_NowNumber=1;							//Ѳ�쵱ǰЭ�����

unsigned char Registered_Start=0;				//����ע��
unsigned char Registered_AllOK=0;				//ȫ��ע�����
//��·�ж�
//unsigned int MClineFlaut_flag=0xFFFF0000;
unsigned int CanLineCount=500;	
/*
***************************************************************************************
��������		CheckSumCalc()
���ܣ� 		��ͺ���
�������1��	��Ҫ��͵����ݵ�ָ�룬
	����2��	���ݳ���
����ֵ��	���ֵ
**************************************************************************************
*/
unsigned char CheckSumCalc(unsigned char * apData_i,unsigned int awDataLen_i)  //���
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
//������		��Sned_InspectionToProtocolBoard����
//����		������Ѳ��ָ���
//�������1	����Ҫ���ʵ�Э�����
//����ֵ		��0,�ɹ�;1��ʧ��
*************************************************/
unsigned char PriorityDataFlag=0x01;//��û�ռ䣻
unsigned char Data_SendToServerFlag=0x02;//��û�ռ䣻
unsigned char SendOne_InspectionToProtocolBoard(unsigned char BoardNumber)   		//����Ѳ��ָ���
{
	//װ��ID��
	unsigned char buf[20];
	unsigned char length;
	unsigned char Timbuf[7];
	unsigned char success=0;
	
	RTC_Get((char *)Timbuf);   //��ȡ��ǰʱ��
	
	buf[0]=0x0c;     		//��Ϣ�峤��
	buf[1]=0x00;
	buf[2]=0x04;			//Ѳ��ע������

	buf[3]=0xc3;			//Ѳ�����ͱ�־c3
	buf[4]=0x01;			//��Ϣ�����1
	
	buf[5]=0x01;			//��Ϣ��
	buf[6]=BoardNumber;		
	
	buf[7]=0x00;	   //8λ��01��ʾ��û�пռ䣬02��ʾ������Ϣû�пռ䣻
	buf[7]|=PriorityDataFlag;
	buf[7]|=Data_SendToServerFlag;
	
	buf[8]=0x01;

	memcpy(&buf[9],Timbuf,6);   	//װ��ʱ��
	buf[15]=CheckSumCalc(buf,15);	//У���
	length=16;						//��֡���ݳ���
	
	success=CAN1_Send_Frame(buf,length,BoardNumber);     //CAN1�������ͺ���
	
	return success;
}







/*************************************************
//������		��Sned_RegisterToProtocolBoard����
//����		������ע��ָ���
//�������1	����Ҫ���ʵ�Э�����
//����ֵ		��0,�ɹ�;1��ʧ��
*************************************************/
unsigned char SendOne_RegisterToProtocolBoard(unsigned char BoardNumber)   		//����ע��ָ���
{
	//װ��ID��
	unsigned char buf[20];
	unsigned char length;
	unsigned char Timbuf[7];
	unsigned char success=0;
	
	RTC_Get((char *)Timbuf);   //��ȡ��ǰʱ��
	
	buf[0]=0x0b;     			//��Ϣ�峤��
	buf[1]=0x00;
	buf[2]=0x04;				//Ѳ��ע������
	
	buf[3]=0xc1;				//ע�����ͱ�־c1
	buf[4]=0x01;				//��Ϣ�����1
	
	buf[5]=0x01;				//��Ϣ��
	buf[6]=BoardNumber;		
	buf[7]=0x01;		

	memcpy(&buf[8],Timbuf,6);   	//װ��ʱ��
	buf[14]=CheckSumCalc(buf,14);	//У���
	length=15;						//��֡���ݳ���
	
	success=CAN1_Send_Frame(buf,length,BoardNumber);     //CAN1�������ͺ���
	
	return success;
}




/*************************************************
//������		��tmrInspect_callback����
//����		��//Ѳ�춨ʱ���ص�����
//�������1	��ϵͳ����
		  2 ��ϵͳ����
//����ֵ		����
*************************************************/
void tmrInspect_callback(void)    //Ѳ�춨ʱ���ص�����
{
	Inspect_TimCount++;	
}




/*************************************************
//������		��Registered_ProtocolBoard����
//����		��Э��淢��ע��ָ���
//�������1	����

//����ֵ		��1,��ɣ�0����ע����
*************************************************/
unsigned char Registered_ProtocolBoard(void)    	//Э��淢��ע��ָ���
{
	unsigned char Res=0;
	OS_ERR err;
	if(S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegistered_Operation==RegNo)    //�жϵ�ǰЭ���ע������Ƿ����
	{
		//����ע����Ϣ
		if(S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegisteredSendState==0)	//���ע����Ϣû�з���
		{
			SendOne_RegisterToProtocolBoard(Registered_NowNumber);   						//����ע��ָ���
			Inspect_TimCount=0;																//��ʱ������ֵ
			OSTimeDlyHMSM(0,0,0,1,OS_OPT_TIME_HMSM_STRICT,&err);							//@�ȴ�1ms
			S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegisteredSendState=1;
		}
		//�жϳ�ʱ�ͷ��ʹ���
		if((S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegisteredReceiveTimOut==0)&&(S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegisteredReceiveOK==0))
		{
			//�жϳ�ʱ����
			if(Inspect_TimCount>=ReceiveTimeOut)   //
			{
				S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegistered_SendTimes++;   		//���ʹ�����1
				if(S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegistered_SendTimes<2)
					S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegisteredSendState=0;		//�ٴη��ͱ�־
				else
				{
					S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegisteredReceiveTimOut=1;  //���ͳ�ʱ
				  S_ProtocolBoardRegistered.Reg_data[Registered_NowNumber-1]=0;
				}
			}
		}
		else
		{
			S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegistered_Operation=RegYes;		//��ǰЭ���ע��������
			Registered_NowNumber++;   //ע����һ��Э���
		}
	}
	
	if(Registered_NowNumber==ProtocolBoard_CNT)   //���ע�����ȫ�����
	{
		memset(&S_RegisteredState[0].ProtocolBoardRegistered_Operation,0,sizeof(S_RegisteredState));   //�ṹ��������0
		Registered_Start=0;			//�˳�ע��
		Registered_NowNumber=1;		//ע���Ÿ�λ
		Res=1;						//ע��ִ����ɣ�
	}
	return Res;
}










/*************************************************
//������		��Cycle_Send_InspectionToProtocolBoard����
//����		��ѭ������Ѳ��ָ��
//�������1	��Э���ע��ṹ��

//����ֵ		��0,�ɹ�;1��ʧ��
*************************************************/
void  Cycle_Send_InspectionToProtocolBoard(ProtocolBoardRegistered S_ProtocolBoardRegis)   		//ѭ������Ѳ��ָ���
{
	OS_ERR err;
	unsigned int a=0;
	
	S_ProtocolBoardRegis.Reg_data[0]=RegYes;
	S_ProtocolBoardRegis.Reg_data[1]=RegYes;
//	S_ProtocolBoardRegis.Reg_data[2]=RegYes;
	
	if(S_ProtocolBoardRegis.Reg_data[Inspect_NowNumber-1]==RegYes)  			 //�����ǰЭ����Ѿ�ע��
	{
		//Э���ע�ᣬ����ִ��Ѳ��
		if(S_InspectState[Inspect_NowNumber-1].ProtocolBoardInspectSendState==0)  //���Ѳ��ָ��û�з��ͣ�����Ѳ��
		{
			SendOne_InspectionToProtocolBoard(Inspect_NowNumber);   //����Ѳ��ָ���
			Inspect_TimCount=0;										//��ʱ������ֵ
			Registered_Allow=0;										//��ֹע��
			OSTimeDlyHMSM(0,0,0,1,OS_OPT_TIME_HMSM_STRICT,&err);	   //@�ȴ�1ms
			S_InspectState[Inspect_NowNumber-1].ProtocolBoardInspectSendState=1;//����״̬��λ
		}
		//Ѳ��δ��ʱ ���� δ���յ���������
		if((S_InspectState[Inspect_NowNumber-1].ProtocolBoardInspectReceiveTimOut==0)&&((S_InspectState[Inspect_NowNumber-1].ProtocolBoardInspectReceiveOK==0)))
		{
			//�жϳ�ʱ �ж�
			if(Inspect_TimCount>=ReceiveTimeOut)    
			{
				S_InspectState[Inspect_NowNumber-1].ProtocolBoardInspectReceiveTimOut=1;
				Inspect_TimCount=0;//20180514 ��־λ���㣻
			}
		}
		else //�����ʱ �� ���յ���������
		{
			Inspect_NowNumber++;    //�����ʱ�����߽�����ɣ�Ѳ����һЭ���
			Registered_Allow=1;		  //����ע��
//			if(Inspect_NowNumber>=16)
			if(Inspect_NowNumber>=2)				
			{		
		    Inspect_NowNumber=1;  //����������ѭ��
			  memset(&S_InspectState[0].ProtocolBoardInspectSendState,0,sizeof(S_InspectState));   //�ṹ��������0
			}			
		}
	}
	else   //�����ǰЭ���û��ע��
	{
		//û��ע�ᣬѲ����һ��Э��塣
		Inspect_NowNumber++;
//		if(Inspect_NowNumber>=16)
		if(Inspect_NowNumber>=2)		
		{
			Inspect_NowNumber=1;  //����������ѭ��
			memset(&S_InspectState[0].ProtocolBoardInspectSendState,0,sizeof(S_InspectState));   //�ṹ��������0
		}
	}
}










/*************************************************
//������		��Sned_ConfirmToProtocolBoard����
//����		������ȷ��ָ���
//�������1	����Ҫ���ʵ�Э�����
//����ֵ		��0,�ɹ�;1��ʧ��
*************************************************/
unsigned char Send_ConfirmToProtocolBoard(unsigned char BoardNumber)   		//����ȷ��ָ���
{
	//װ��ID��
	unsigned char buf[20];
	unsigned char length;
	unsigned char Timbuf[7];
	unsigned char success=0;
	
	RTC_Get((char *)Timbuf);   //��ȡ��ǰʱ��
	
	buf[0]=0x0C;     			//��Ϣ�峤��
	buf[1]=0x00;
	buf[2]=0x03;				//ȷ������
	
	buf[3]=0xc5;				//ȷ�ϱ�־c5
	buf[4]=0x01;				//��Ϣ�����1
	
	buf[5]=0x01;				//��Ϣ��
	buf[6]=BoardNumber;		
	buf[7]=0x01;
	buf[8]=0x01;

	memcpy(&buf[9],Timbuf,6);   	//װ��ʱ��
	buf[15]=CheckSumCalc(buf,15);	//У���
	length=16;						//��֡���ݳ���
	
	success=CAN1_Send_Frame(buf,length,BoardNumber);     //CAN1�������ͺ���
	
	return success;
}





/*************************************************
//������	��CAN1_DataRecieve_Integrity����
//����		���ж�CAN1�������ݵ���ȷ��
//�������1	�����յ�����ָ��
	  ����2	�����յ����ݳ���
//����ֵ	��1,�ɹ�;0��ʧ��
*************************************************/
unsigned char CAN1_DataRecieve_Integrity(unsigned char *date,unsigned int len)    //CAN1���������������ж�
{
	unsigned char CRC_sum=0;
	
	if(len<1)
	{return 0;}
	
	CRC_sum=CheckSumCalc(date,(len-1));  //���
	
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
//������	��ReceiveRegistered_Data����
//����		������ע������
//�������1	����������ָ��
//    ����2	���������ݳ���
//����ֵ		����
*************************************************/
static void  ReceiveRegistered_Data(unsigned char *data,unsigned char len)   				//ע������
{
		memcpy(&S_Registered_InforDataStr[Registered_NowNumber-1].Board_Type,&data[4],140);    //ת������
		S_RegisteredState[Registered_NowNumber-1].ProtocolBoardRegisteredReceiveOK=1;		//������ɱ�־
		S_ProtocolBoardRegistered.Reg_data[Registered_NowNumber-1]=RegYes;					//��ע��ע��
}

/*************************************************
//������	��ReceiveAlarm_Failure_Data����
//����		�����ղ�����������
//�������1	����������ָ��
//    ����2	���������ݳ���
//����ֵ		����
*************************************************/
static void  ReceiveInspection(unsigned char *data,unsigned int len)   //����Ѳ�����ע������
{
	if(data[3]==0xc2)  															//����������ע����Ϣ
	{
		ReceiveRegistered_Data(data,len); 		
	}	
	
	if(data[3]==0xc4)
	{
		S_InspectState[Inspect_NowNumber-1].ProtocolBoardInspectReceiveOK=1;    //��ǰ֡�������
		//�ж�����ͨѶ��Ӧ���ڴ˴���
		if(data[8]==0x01)//������·������
		{
			 MClineFlaut_flag&=0x0000FFFF;		
		   MClineFlaut_flag&=~(0x0001<<(Inspect_NowNumber));	
		}			
		else if(data[8]==0x02)//������·���ϣ�
		{
		   MClineFlaut_flag&=0x0000FFFF;	
		   MClineFlaut_flag|=(0x0001<<(Inspect_NowNumber));
		}		
		else if(data[8]==0x03)//����·�жϹ��ܣ�
		{
			
		}
	}
}
	




/*************************************************
//������	��ReceiveAlarm_Failure_Data����
//����		�����ղ�����������
//�������1	����������ָ��
//    ����2	���������ݳ���
//����ֵ		����
*************************************************/
static unsigned char Redata[512];//�������ݿռ䣻
void  ReceiveAlarm_Failure_Data(unsigned char *data,unsigned int len)   //���ղ����������ݣ��𾯡����ϣ�
{
	switch(data[3])
	{
    case 0x01:		
    case 0x04:
		case 0x02:
			ReportToServer(0x02,&data[3],len-4,Redata);
//			printf("��=%d��\n\r",data[8]);
			break;	
		default:
			break;
	}
}




/*************************************************
//������		��ThreeInOne_ProtocolBoard_Handle����
//����		��CAN����Э�����
//�������1	����������ָ��
//    ����2	���������ݳ���
//����ֵ		����
*************************************************/
unsigned char ThreeInOne_ProtocolBoard_Handle(unsigned char *buf,unsigned int len,unsigned char ID)   		//Э������һ��CANЭ�����
{ 
	switch(buf[2])
	{
		case 0x02:  //�Ϸ�����������״̬���ݣ����������ֽڣ�
		{
			ReceiveAlarm_Failure_Data(buf,len);   	//���ղ�����������
			Send_ConfirmToProtocolBoard(ID);		    //����ȷ��ָ���
		  S_InspectState[Inspect_NowNumber-1].ProtocolBoardInspectReceiveOK=1;    //��ǰ֡�������			20180514
			break;
		}
		case 0x05:  //Э���Ӧ���û���ϢѲ��ָ��
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
//������		��ThreeInOne_ProtocolBoard_Run����
//����		��CAN����Э��ִ�к���
//�������1	����
//����ֵ		����
*************************************************/
unsigned short register_flag=0x0000;//��Ҫ��ʾ�İ忨��Ϣ  //ע��ɹ���־λ��1��ʱ��ע��ɹ���0��ʱ��ע��ʧ�ܡ�
//ע��Э�����ɱ�־λ��
unsigned char RegisteProComplete=0;
unsigned char BK_Message[17][100];//�忨��Ϣ�洢��

extern void RefreshRunLedState(void);
unsigned char ThreeInOne_ProtocolBoard_Run(void)   		//CAN����Э��ִ�к���
{
	unsigned int length=0,RegisterSave=0;
	unsigned char  Res=0;
	unsigned char i=0;
  CAN_Data_Typedef S_CAN_Rxdata;

	///////////////��������///////////////
	//����ע����Ϣ
//	if((Registered_Start==1)&&(Registered_Allow==1))							//���������ע�������������ע�ᣬ����ע��ָ�Ѳ������У�����ע�ᡣ
//	{
//		//��·�̵ƣ��Ƶƶ���������λ��
//	  MClineFlaut_flag=0xFFFF0000;
//		
//		Res=Registered_ProtocolBoard();    										      //Э��淢��ע��ָ�� 15������ע�ᡣ
//		if(Res==1)
//		{
//			Registered_AllOK=1;  //����ṹ��־������ע���־�󣬵ȴ��˱�־λ��1��Ȼ���ȡ���ݣ������Ѿ�װ���ڽṹ�����ȡ���ݺ�˱�־��0
//			register_flag=0;
//			
//			for(i=0;i<15;i++)
//			{
//				//װ�ذ忨ע���ź�ע������
//				if(S_ProtocolBoardRegistered.Reg_data[i]==1)  	//����忨ע��ɹ�
//				{
//					register_flag|=(0x01<<(i+1));
//					memcpy(BK_Message[i+1],&S_Registered_InforDataStr[i].Hardware_Version[0],100);
//				}
//			}
//			RegisterSave=register_flag;
//      Writeflash_Config((unsigned char *)&RegisterSave,ProID_CONFIG_CMD);//����Э�鿨ע���־λ��                                                               //			
//		  RegisteProComplete=1;
//		}
//	}
//	else   //����Ѳ����Ϣ
//	{	
	  Cycle_Send_InspectionToProtocolBoard(S_ProtocolBoardRegistered);
// 	}   		//ѭ������Ѳ��ָ���
	
	///////////////�������ݽ���///////////////
	S_CAN_Rxdata.Lenth=0;
	CAN1_Read((CAN_Data_Typedef *)&S_CAN_Rxdata.ID_Number);   //CAN1��ȡ����
	if(S_CAN_Rxdata.Lenth!=0)
	{
		if(CAN1_DataRecieve_Integrity(S_CAN_Rxdata.data,  S_CAN_Rxdata.Lenth))     						//CAN1���������������ж�
		{
			//��������ȷ,�����������
			ThreeInOne_ProtocolBoard_Handle(S_CAN_Rxdata.data,  S_CAN_Rxdata.Lenth,  S_CAN_Rxdata.ID_Number);   //Э������һ��CANЭ�����	
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
			if(register_flag!=0)					//��Э���
	    { return 0;	}				//��·����
		  else
		  { return 1;}				//��·����
		}
		else 
		{
				return 1;				  //��·����
		}
}


void CAN1Init(void)
{


}	
