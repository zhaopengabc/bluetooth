#include "SendToServerDataType.h"
#include "string.h"
#include "includes.h"
#include "CAN1ProtocolParsing.h"

Server_Data_Typedef Data_SendToServer[SERVER_GROUPNUM];//���ϼ�������
volatile unsigned char Server_Wr_Addr=0;
volatile unsigned char Server_Rd_Addr=0;

Server_Data_Typedef PriorityDataSendToServer[PRIORITY_GROUPNUM];//��
volatile unsigned char SerPrio_Wr_Addr=0;
volatile unsigned char SerPrio_Rd_Addr=0;

Server_Data_Typedef ManAlarmDataSendToServer[MANALARM_GROUPNUM];//�ֱ���
volatile unsigned char ManAlarm_Wr_Addr=0;
volatile unsigned char ManAlarm_Rd_Addr=0;

extern OS_TCB	lcdTaskTCB;
Server_Data_Typedef * IsReportAddr(unsigned char *data,unsigned int lenth,unsigned char *pServer_Rd_Addr)
{
	Server_Data_Typedef *pServerData=NULL;
	unsigned char i=0;
	for(i=0;i<SERVER_GROUPNUM;i++)
	{
		pServerData=(Server_Data_Typedef *)&Data_SendToServer[i];
		if((data==pServerData->data)&&(lenth==pServerData->Lenth))
		{
			pServerData=(Server_Data_Typedef *)&Data_SendToServer;
			*pServer_Rd_Addr=i;
			return pServerData;
		}
	}
	for(i=0;i<PRIORITY_GROUPNUM;i++)
	{
		pServerData=(Server_Data_Typedef *)&PriorityDataSendToServer[i];
		if((data==pServerData->data)&&(lenth==pServerData->Lenth))
		{
			pServerData=(Server_Data_Typedef *)&PriorityDataSendToServer;
			*pServer_Rd_Addr=i;
			return pServerData;
		}
	}
	for(i=0;i<MANALARM_GROUPNUM;i++)
	{
		pServerData=(Server_Data_Typedef *)&ManAlarmDataSendToServer[i];
		if((data==pServerData->data)&&(lenth==pServerData->Lenth))
		{
			pServerData=(Server_Data_Typedef *)&ManAlarmDataSendToServer;
			*pServer_Rd_Addr=i;
			return pServerData;
		}
	}
	return NULL;
}

//������漰��־λ�������񴥷�ʱ��
void Reset_SendToServerData(void)
{
	unsigned char i;
	Server_Rd_Addr=0;
	Server_Wr_Addr=0;
	for(i=0;i<SERVER_GROUPNUM;i++)
	{
		memset(Data_SendToServer[i].data,0,SERVER_DATA_NUM);
		Data_SendToServer[i].Lenth=0;
		Data_SendToServer[i].Flag=0;
	}
	SerPrio_Rd_Addr=0;
	SerPrio_Wr_Addr=0;
	for(i=0;i<PRIORITY_GROUPNUM;i++)
	{
		memset(PriorityDataSendToServer[i].data,0,SERVER_DATA_NUM);
		PriorityDataSendToServer[i].Lenth=0;
		PriorityDataSendToServer[i].Flag=0;
	}
	ManAlarm_Rd_Addr=0;
	ManAlarm_Wr_Addr=0;
	for(i=0;i<MANALARM_GROUPNUM;i++)
	{
		memset(ManAlarmDataSendToServer[i].data,0,SERVER_DATA_NUM);
		ManAlarmDataSendToServer[i].Lenth=0;
		ManAlarmDataSendToServer[i].Flag=0;
	}
}

//���㣻
unsigned char SendToServerData_Reset(unsigned int num)
{
	unsigned char i=0,data2=0,data3=0;
//	CPU_SR_ALLOC();

	data2=num;
	data3=num>>8;

//	OS_CRITICAL_ENTER();	
	///////
	for(i=0;i<MANALARM_GROUPNUM;i++)
	{
		if((ManAlarmDataSendToServer[i].data[2]==data2)&&(ManAlarmDataSendToServer[i].data[3]==data3))
		{
				ManAlarmDataSendToServer[i].Lenth=0;
				ManAlarmDataSendToServer[i].Flag=0;
				ManAlarmDataSendToServer[i].data[2]=0;
				ManAlarmDataSendToServer[i].data[3]=0;
			  return 1;
		}
		else
		{}
	}
	///////
	for(i=0;i<PRIORITY_GROUPNUM;i++)
	{
		if((PriorityDataSendToServer[i].data[2]==data2)&&(PriorityDataSendToServer[i].data[3]==data3))
		{
				PriorityDataSendToServer[i].Lenth=0;
				PriorityDataSendToServer[i].Flag=0;
				PriorityDataSendToServer[i].data[2]=0;
				PriorityDataSendToServer[i].data[3]=0;
			  return 2;
		}
		else
		{}
	}	
	///////
	for(i=0;i<SERVER_GROUPNUM;i++)
	{
		if((Data_SendToServer[i].data[2]==data2)&&(Data_SendToServer[i].data[3]==data3))
		{
				Data_SendToServer[i].Lenth=0;
				Data_SendToServer[i].Flag=0;
				Data_SendToServer[i].data[2]=0;
				Data_SendToServer[i].data[3]=0;
			  return 3;
		}
		else
		{}
	}		

//	OS_CRITICAL_EXIT();	//�����ٽ���
	return 0;
}

void Reset_RdSendToServerData(Server_Data_Typedef *pSendToServer_Data,unsigned char *pSendToServer_Rd_Addr)
{
	CPU_SR_ALLOC();
	unsigned char Rd_Addr;
	OS_CRITICAL_ENTER();
	Rd_Addr=*pSendToServer_Rd_Addr;
//	memset(pSendToServer_Data[Rd_Addr].data,0,SERVER_DATA_NUM);
	pSendToServer_Data[Rd_Addr].data[2]=0;
	pSendToServer_Data[Rd_Addr].data[3]=0;
	pSendToServer_Data[Rd_Addr].Lenth=0;
	pSendToServer_Data[Rd_Addr].Flag=0;
	Rd_Addr++;
	if(Rd_Addr>=SERVER_GROUPNUM)Rd_Addr=0;
	*pSendToServer_Rd_Addr=Rd_Addr;
//	printf("RD++ RD=%d,%d��\n\r",Rd_Addr,SerPrio_Rd_Addr);
	OS_CRITICAL_EXIT();	//�����ٽ���
}
//�ж��Ƿ�Ϊ�ֱ�֡��֡�������򷵻�1����������ȷ��ͻ����еȴ�����
//�������򷵻�0���ŵ���ͨ�����еȴ�����
unsigned char IsPriorityFrame(unsigned char *data,unsigned int lenth)
{
	unsigned char i=0;
	//�ܳ���Ϊ30+������Ŀ*46+2
	if((data[27]==0x02)&&(lenth==data[28]*46+32))
	{
		for(i=0;i<data[28];i++)
		{
			if(data[36+46*i]&0x02) return 1;
		}
	}
	else if((data[27]==0x01)&&(lenth==data[28]*10+32))
	{
		for(i=0;i<data[28];i++)
		{
			if(data[31+10*i]&0x02) return 1;
		}
	}
	else if((data[27]==0x04)&&(lenth==42))
	{
		if(data[31]&0x04) return 1;
	}
	else if((data[27]==21)&&(lenth==39))
	{
		if(data[29]&0x02) return 1;
	}
	else if((data[27]==24)&&(lenth==40))
	{
		if(data[29]&0x04) return 2;
	}
	return 0;
}
//�����ݳ���Ϊlenth��data���ݷ��뷢�ͻ����ϱ�������
//�������ɹ�����1������ʧ�ܷ���0
unsigned int Data_TransToServer(unsigned char *data,unsigned int lenth)
{
	OS_ERR err;
	CPU_TS ts;
	CPU_SR_ALLOC();
	unsigned char flag=0;
	unsigned char buf=0;
	OS_CRITICAL_ENTER();
	flag=IsPriorityFrame(data,lenth);
	if(flag==2)
	{
		if(ManAlarmDataSendToServer[ManAlarm_Wr_Addr].Flag==0)//�����һ��д��ռ���0���Ǿ�д�룬���򶪵���
		{ 
		//��Ϊ�������ݣ������ط������ط�3�Σ���Ϊ�ظ���ѯ��ȷ�Ϸ���ֻ����1�Σ�
		//�ֶ��������ֻ���ط�û�е��η���
		ManAlarmDataSendToServer[ManAlarm_Wr_Addr].Flag=4;
		ManAlarmDataSendToServer[ManAlarm_Wr_Addr].Lenth=lenth;
		memset(ManAlarmDataSendToServer[ManAlarm_Wr_Addr].data,0,SERVER_DATA_NUM);
		memcpy(ManAlarmDataSendToServer[ManAlarm_Wr_Addr].data,data,lenth);
		buf=ManAlarm_Wr_Addr;
		ManAlarm_Wr_Addr++;
		if(ManAlarm_Wr_Addr>=MANALARM_GROUPNUM)ManAlarm_Wr_Addr=0;
//		if(ManAlarm_Rd_Addr==ManAlarm_Wr_Addr) //��ȡ�ĵ�ַ���ٴ˴��ۼӣ�
//		{
//			ManAlarm_Rd_Addr++;
//			if(ManAlarm_Rd_Addr>=MANALARM_GROUPNUM)ManAlarm_Rd_Addr=0;
//		}
		OS_TaskQPost((OS_TCB	*)&lcdTaskTCB,
					(void		*)ManAlarmDataSendToServer[buf].data,
					(OS_MSG_SIZE)ManAlarmDataSendToServer[buf].Lenth,
					(OS_OPT		)OS_OPT_POST_FIFO,
					(CPU_TS		)ts,
					(OS_ERR		*)&err
					);
	 }				
	}else if(flag==1)
	{
		if(PriorityDataSendToServer[SerPrio_Wr_Addr].Flag==0)//�����һ��д��ռ���0���Ǿ�д�룬���򶪵���
		{ 
		PriorityDataSendToServer[SerPrio_Wr_Addr].Flag=4;
		PriorityDataSendToServer[SerPrio_Wr_Addr].Lenth=lenth;
		memset(PriorityDataSendToServer[SerPrio_Wr_Addr].data,0,SERVER_DATA_NUM);
		memcpy(PriorityDataSendToServer[SerPrio_Wr_Addr].data,data,lenth);
		buf=SerPrio_Wr_Addr;
		    SerPrio_Wr_Addr++;
		 if(SerPrio_Wr_Addr>=PRIORITY_GROUPNUM)
			  SerPrio_Wr_Addr=0;		 
//		if(SerPrio_Rd_Addr==SerPrio_Wr_Addr)//��ȡ�ĵ�ַ���ٴ˴��ۼӣ�
//		{
//			SerPrio_Rd_Addr++;
//			if(SerPrio_Rd_Addr>=PRIORITY_GROUPNUM)SerPrio_Rd_Addr=0;
//		}		 
		OS_TaskQPost((OS_TCB	*)&lcdTaskTCB,
					(void		*)PriorityDataSendToServer[buf].data,
					(OS_MSG_SIZE)PriorityDataSendToServer[buf].Lenth,
					(OS_OPT		)OS_OPT_POST_FIFO,
					(CPU_TS		)ts,
					(OS_ERR		*)&err
					);
		 }
	}
	else
	{
		if(Data_SendToServer[Server_Wr_Addr].Flag==0)//�����һ��д��ռ���0���Ǿ�д�룬���򶪵���
		{ 
		////////////////////////////////////////����пռ�
		if(data[26]==2) Data_SendToServer[Server_Wr_Addr].Flag=4;
		else Data_SendToServer[Server_Wr_Addr].Flag=2;
		Data_SendToServer[Server_Wr_Addr].Lenth=lenth;
		memset(Data_SendToServer[Server_Wr_Addr].data,0,SERVER_DATA_NUM);
		memcpy(Data_SendToServer[Server_Wr_Addr].data,data,lenth);
		buf=Server_Wr_Addr;
		Server_Wr_Addr++;
		if(Server_Wr_Addr>=SERVER_GROUPNUM)Server_Wr_Addr=0;
//		if(Server_Rd_Addr==Server_Wr_Addr)//��ȡ�ĵ�ַ���ٴ˴��ۼӣ�
//		{
//			Server_Rd_Addr++;
//			if(Server_Rd_Addr>=SERVER_GROUPNUM)Server_Rd_Addr=0;
//		}
		if((data[26]==2)||((data[26]==5)&&(data[27]==21)))
		{
			OS_TaskQPost((OS_TCB	*)&lcdTaskTCB,
						(void		*)Data_SendToServer[buf].data,
						(OS_MSG_SIZE)Data_SendToServer[buf].Lenth,
						(OS_OPT		)OS_OPT_POST_FIFO,
						(CPU_TS		)ts,
						(OS_ERR		*)&err
						);
		}
		////////////////////////////////////////
		}
	}
	OS_CRITICAL_EXIT();	//�����ٽ���
	return lenth;
}

Server_Data_Typedef *pSendToServerData(unsigned char **pRd_Addr)
{
	unsigned char i=0;
	Server_Data_Typedef *pServer_Data=NULL;
	pServer_Data=NULL;
	/////////////////////////////
	if(ManAlarmDataSendToServer[ManAlarm_Rd_Addr].Flag!=0)
	{
		pServer_Data=(Server_Data_Typedef *)&ManAlarmDataSendToServer;
		*pRd_Addr=(unsigned char *)&ManAlarm_Rd_Addr;
		return pServer_Data;
	}
	else
	{
		if((ManAlarmDataSendToServer[0].Flag!=0)||(ManAlarmDataSendToServer[1].Flag!=0)||(ManAlarmDataSendToServer[2].Flag!=0))//�����ֱ�
		{ 
			for(i=0;i<MANALARM_GROUPNUM;i++)//ѭ��3��
			{
						ManAlarm_Rd_Addr++;//��1����
					 if(ManAlarm_Rd_Addr>=MANALARM_GROUPNUM)
					 { ManAlarm_Rd_Addr=0; }

					if(ManAlarmDataSendToServer[ManAlarm_Rd_Addr].Flag!=0)//��Ȼ�У�
					{
						pServer_Data=(Server_Data_Typedef *)&ManAlarmDataSendToServer;//��ȡ��ַ
						*pRd_Addr=(unsigned char *)&ManAlarm_Rd_Addr;
						return pServer_Data;//���أ�
					}	
		  }			
		}		
	}
	///////////////////////////// 
	if(PriorityDataSendToServer[SerPrio_Rd_Addr].Flag!=0)
	{ 		 
		pServer_Data=(Server_Data_Typedef *)&PriorityDataSendToServer;
		*pRd_Addr=(unsigned char *)&SerPrio_Rd_Addr;
		return pServer_Data;
	}
	else
	{
		if((PriorityDataSendToServer[0].Flag!=0)||(PriorityDataSendToServer[1].Flag!=0)||(PriorityDataSendToServer[2].Flag!=0))//���л�
		{
 			for(i=0;i<PRIORITY_GROUPNUM;i++)//ѭ��3��
			{
						  SerPrio_Rd_Addr++;//��1����
					 if(SerPrio_Rd_Addr>=PRIORITY_GROUPNUM)
					 {  SerPrio_Rd_Addr=0; }

					if(PriorityDataSendToServer[SerPrio_Rd_Addr].Flag!=0)//��Ȼ�У�
					{
						pServer_Data=(Server_Data_Typedef *)&PriorityDataSendToServer;
						*pRd_Addr=(unsigned char *)&SerPrio_Rd_Addr;
						return pServer_Data;//���أ�
					}	
		  }
		}
  }
	/////////////////////////////
	if(Data_SendToServer[Server_Rd_Addr].Flag!=0)
	{   
		pServer_Data=(Server_Data_Typedef *)&Data_SendToServer;	
		*pRd_Addr=(unsigned char *)&Server_Rd_Addr;
		return pServer_Data;
	}
	else
	{
		if((Data_SendToServer[0].Flag!=0)||(Data_SendToServer[1].Flag!=0)||(Data_SendToServer[2].Flag!=0))
		{
			for(i=0;i<SERVER_GROUPNUM;i++)//ѭ��3��
			{
						  Server_Rd_Addr++;//��1����
					 if(Server_Rd_Addr>=SERVER_GROUPNUM)
					 {  Server_Rd_Addr=0; }

					if(Data_SendToServer[Server_Rd_Addr].Flag!=0)//��Ȼ�У�
					{
						pServer_Data=(Server_Data_Typedef *)&Data_SendToServer;	
						*pRd_Addr=(unsigned char *)&Server_Rd_Addr;
						return pServer_Data;//���أ�
					}	
		  }
		}
	}
	return pServer_Data;
}

//unsigned char DataSendToStatus(void)
//{
//	 unsigned char i=0;
//	
//   for(i=0;i<PRIORITY_GROUPNUM;i++)
//	 {
//		  if((PriorityDataSendToServer[i].Flag==0)&&(PriorityDataSendToServer[i].Lenth==0))
//			{  
//			   return 1;
//			}
//	 }
//	 
//	 return 0;	
//}

//

unsigned char DataSendToStatus(void)
{
	  unsigned char i=0;
    PriorityDataFlag=0x01;//��û�ռ䣻
    Data_SendToServerFlag=0x02;//��û�ռ䣻			
   
		 if(SerPrio_Wr_Addr>=PRIORITY_GROUPNUM)
		 {
				SerPrio_Wr_Addr=0;
		 }		
		 if(PriorityDataSendToServer[SerPrio_Wr_Addr].Flag==0)//��ǰд�������пռ�
		 { 
			PriorityDataFlag=0;//�пռ�
		 }

		 
		 	if(Server_Wr_Addr>=SERVER_GROUPNUM)
			{
				Server_Wr_Addr=0;
			}	
		 if(Data_SendToServer[Server_Wr_Addr].Flag==0)//��ǰд�������пռ�
		 { 
			Data_SendToServerFlag=0;//�пռ�
		 }
 
	 return 1;	
		
}

