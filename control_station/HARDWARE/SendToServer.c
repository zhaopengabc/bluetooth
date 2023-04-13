#include "SendToServer.h"
#include "frame.h"
#include "net.h"
#include "ServerAction.h"

unsigned char SendToServer_Infor(const char *data,unsigned int Lenth,unsigned int *pFrameCnt)
{
	unsigned char SendFlag=0;
	SendFlag=Net_Send_Infor((unsigned char *)data,Lenth);
	*pFrameCnt=data[2]+(data[3]<<8);
	return SendFlag;
}
unsigned char SendToServerRetrans(Server_Data_Typedef *pSendToServer_Data,unsigned char *pSendToServer_Rd_Addr)
{
	unsigned int SendFlag=0;
	unsigned char Rd_Addr=0;
	unsigned int Tx_Frame=0;
	Rd_Addr=*pSendToServer_Rd_Addr;
	
	if(pSendToServer_Data[Rd_Addr].Flag<=4)
	{		
		if(pSendToServer_Data[Rd_Addr].Flag!=1)
		{	
			SendFlag=SendToServer_Infor((char *)pSendToServer_Data[Rd_Addr].data,pSendToServer_Data[Rd_Addr].Lenth,(unsigned int*)&Tx_Frame);
			Write_LastFrameCnt(Tx_Frame);
		}
		
		if(SendFlag!=0)
		{
			if(pSendToServer_Data[Rd_Addr].data[26]==0x02)
			{
				SendSuccessedAction();//网络通讯灯
			}
		}
		
		if(pSendToServer_Data[Rd_Addr].Flag!=0) 
		{ pSendToServer_Data[Rd_Addr].Flag--;
		}
		
		if(pSendToServer_Data[Rd_Addr].Flag==0)//发了4次;发完了；
		{ 
			LastTx_Frame_Increase();
			if(pSendToServer_Data[Rd_Addr].data[26]==0x02)
			{
				SendFailedAction();//网络通讯灯
			}
			Reset_RdSendToServerData(pSendToServer_Data,pSendToServer_Rd_Addr);//读地址加1；
		}	
	}
	else
	{
		pSendToServer_Data[Rd_Addr].Flag=0;
	}
	return 1;
}
unsigned char SendFrameToServer(void)
{
	unsigned char *pRd_Addr=NULL;
	Server_Data_Typedef *pServer_Data=NULL;
	pRd_Addr=NULL;

	pServer_Data=pSendToServerData(&pRd_Addr);//获取需要发的地址

	if((pRd_Addr!=NULL)&&(pServer_Data!=NULL))
	{ 
		SendToServerRetrans(pServer_Data,pRd_Addr);
		return 1;
	}
	else
	{

	}
	return 0;
}

