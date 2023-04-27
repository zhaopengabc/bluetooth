#ifdef D026_HW200
/*
��д��¼��
1.20180611������У��ͣ�
2.20180611����д��������ɡ�
3.20181226: �յ����ݰ����̻ظ�51
������Ϣ��¼��
 ����������ѯ��ʽ
 ��ѯ��500ms  31
 �ظ���51  �������Ϸ����ظ�51�Ļ���һֱ���ʹ˰����ݣ�����51���²ŷ���һ������ֱ���Ϸ����£�ϵͳ�����縴λ����������ϻָ���Ҫ������һ�����ݣ��������ᷢ��һ���������ڻظ�51��
*/

//��������ó���
#include "D026_HW200.h" 
#include "UsartMsgProcessAppExt.h"
#include "MsgCANProcessAppExt.h"
#include "rtc.h"
#include "app.h"
#include  <string.h>
#include "ChoiceDevice.h"
#include "ReceiveDataBuffer.h"

#define GST200_HUOJING_CMD					0x15	//������
#define GST200_GUZHANG_CMD					0x21	//��������
#define GST200_LIANDONG_CMD					0x02	//����ģ������
#define GST200_FANKUI_CMD					  0x22	//����ģ�鷴��
#define GST200_PINGBI_CMD					  0xDE	//����ģ������
#define GST200_ZHUDIANYUAN_CMD 			0x31	//��/�������
#define GST200_FUWEI_CMD					  0x04	//��λ
#define GST200_XIAOYIN_CMD					0x70	//����

//�忨�ͺţ������ͺţ�����汾���ӿ�˵����
const unsigned char BKXH_msg[20]="XSBD-FB805XY-A";
const unsigned char XFZJ_msg[20]="HW200";
const unsigned char SOFT_msg[40]="FB805XY_HW200R_V2.2";
const unsigned char ITFS_msg[20]="RS232";
 
const unsigned short VERS_msg=11026;//21020=�汾V21+�����ͺ�D020��Զ��������Э��汾��

tMCSystemStatus  			    sMCSystemStatus_ZJ;   		//ϵͳ״̬���ݽṹ��
tMCSystemDeviceStatus 		sMCSystemDeviceStatus_ZJ;   //�豸״̬���ݽṹ��
tMCSystemOperationStatus 	sMCSystemOperationStatus_ZJ;  //������Ϣ���ݽṹ��

static void ZJ_Plugin_Init(void)//��ʼ��
{
    //��ʼ��UART������
	gUARTSystemParameter.USARTx_BaudRate=115200;
	gUARTSystemParameter.USARTx_WordLength=USART_WordLength_8b;
	gUARTSystemParameter.USARTx_StopBits=USART_StopBits_1;
	gUARTSystemParameter.USARTx_Parity=USART_Parity_No;
	gUARTSystemParameter.USARTx_HardwareFlowControl=USART_HardwareFlowControl_None;
//	gUARTSystemParameter.USARTx_Mode=USART_Mode_Tx;
	gUARTSystemParameter.USARTx_Mode=USART_Mode_Rx | USART_Mode_Tx;
	gUARTSystemParameter.UARTx_IRQn=USART3_IRQn;
	gUARTSystemParameter.NVIC_Priority=0;	
	gUARTSystemParameter.NVIC_SubPriority=3;		
}

//������������ȷ���ж�������ȷ����1�����󷵻�0��
unsigned char ZJ_Integrity(unsigned char *data,unsigned int lenth)
{
	unsigned char flag=0,i=0,sum=0;
	unsigned char Senddata=0x51;  
	
	for(i = 0; i < lenth-1; i++)
	{
			sum += data[i];
	}
		
	if((lenth >= 1)&&(sum==data[lenth-1]))
	{
			flag=1;
	}
	if(lenth > 1)
  {		
			UARTx_Send_Data(UartInterface,(unsigned char *)&Senddata,1);
	}
	return flag;
 
}

/***********************
0x82�������			---->0x0100
0x84�������			---->0x0200
***********************/
unsigned int GST200_SysStateConv(unsigned char state)
{
	unsigned int flag=0;
	switch(state)
	{
		case 0x82: flag=0x0100;break;
		case 0x84: flag=0x0200;break;
		default: break;
	}
	return flag;
}

unsigned int  ZJ_SysState_Analsis(unsigned char *data,unsigned int lenth)
{
	unsigned int len=12;
	unsigned short Status=0;
	
	sMCSystemStatus_ZJ.MCaddress        = 1;		//������ַ

	if(data[4] == 0x31)
	{
		//ϵͳ״̬
		sMCSystemStatus_ZJ.MCStatus0=GST200_SysStateConv(data[8]);
		sMCSystemStatus_ZJ.MCStatus1=GST200_SysStateConv(data[8])>>8;
	}
	
  sMCSystemStatus_ZJ.MCTimeDataYer=SystemTime[5];//��
	sMCSystemStatus_ZJ.MCTimeDataMon=SystemTime[4];//��
	sMCSystemStatus_ZJ.MCTimeDataDay=SystemTime[3];//��	
	sMCSystemStatus_ZJ.MCTimeDataHor=SystemTime[2];//Сʱ
	sMCSystemStatus_ZJ.MCTimeDataMin=SystemTime[1];//��
	sMCSystemStatus_ZJ.MCTimeDataSec=SystemTime[0];//��	
	
    RecieveMCSystemStatusAndSend(  sMCSystemStatus_ZJ.MCaddress,
																	 sMCSystemStatus_ZJ.MCStatus0,
                                   sMCSystemStatus_ZJ.MCStatus1,
                                   sMCSystemStatus_ZJ.MCTimeDataSec,
                                   sMCSystemStatus_ZJ.MCTimeDataMin,
                                   sMCSystemStatus_ZJ.MCTimeDataHor,
                                   sMCSystemStatus_ZJ.MCTimeDataDay,
                                   sMCSystemStatus_ZJ.MCTimeDataMon,
                                   sMCSystemStatus_ZJ.MCTimeDataYer);//ϵͳ״̬������
	return len;
}
/***********************
01��դ����		---->31
02����			---->31
03����			---->40
04�����ӿ�		---->51
0B�ֱ�			---->23
4C���ⱨ��		---->��Ϊû�ж�Ӧ�ģ��ݶ�Ϊ121
4D�𾯴���		---->121
0DѶ����		---->147
***********************/
unsigned char GST200_ComponentConversion(unsigned char type)
{
	unsigned char flag=0;
	switch(type)
	{
		case 0x01: flag=31;break;
		case 0x02: flag=31;break;
		case 0x03: flag=40;break;
		case 0x04: flag=51;break;
		case 0x0B: flag=23;break;
		case 0x0D: flag=121;break;
		case 0x4C: flag=121;break;
		case 0x4D: flag=147;break;
		default: break;
	}
	return flag;
}

/***********************
0x15��			---->0x0002
0x21����			---->0x0004
0xDE����			---->0x0008
0x02����			---->0x0020
0x22����			---->0x0040
***********************/
unsigned char GST200_CompStateConv(unsigned char state)
{
	unsigned int flag=0;
	switch(state)
	{
		case 0x15: flag=0x0002;break;
		case 0x21: flag=0x0004;break;
		case 0xDE: flag=0x0008;break;
		case 0x02: flag=0x0020;break;
		case 0x22: flag=0x0040;break;
		default: break;
	}
	return flag;
}
//data[2]=0x12 data[3]=0x34 data[4]=0x56
int HW200_MCDeviceCode(unsigned char *data,unsigned char length)
{
	unsigned char nbr[6];
	unsigned char nbrA;
	int B=0;
	memset(nbr,0,6);
	nbrA=data[0]&0xF0;
  nbrA=nbrA>>4;
	B=B+nbrA;
	B=B*10; //10
	nbrA=data[0]&0x0F;
	B=B+nbrA;
	B=B*10; //120

	nbrA=data[1]&0xF0;
  nbrA=nbrA>>4;
	B=B+nbrA;
	B=B*10;	//1230
	nbrA=data[1]&0x0F;
	B=B+nbrA;
	B=B*10; //12340	
	
	nbrA=data[2]&0xF0;
  nbrA=nbrA>>4;
	B=B+nbrA;
	B=B*10;	//123450
	nbrA=data[2]&0x0F;
	B=B+nbrA;
	         //123456
	
	return B;
}
//��������
unsigned int  ZJ_ComponentState_Analsis(unsigned char *data,unsigned int lenth)
{
	unsigned int len=48;
	unsigned int code=0;
  //ϵͳ��ַ
	sMCSystemDeviceStatus_ZJ.MCaddress        = 1;		//������ַ
	//��������
	sMCSystemDeviceStatus_ZJ.MCDevicetype1    = GST200_ComponentConversion(data[8]);
 
	sMCSystemDeviceStatus_ZJ.MCDeviceAddress0 = 0;					  //��·
	sMCSystemDeviceStatus_ZJ.MCDeviceAddress1 = 0;	
	sMCSystemDeviceStatus_ZJ.MCDeviceAddress2 = 0;					  //�豸��ַ
	sMCSystemDeviceStatus_ZJ.MCDeviceAddress3 = 0;
	code=HW200_MCDeviceCode(&data[5],3);//�û�����
	
	
	sMCSystemDeviceStatus_ZJ.MCDeviceStatus0  = GST200_CompStateConv(data[4]);		 
	sMCSystemDeviceStatus_ZJ.MCDeviceStatus1  = 0;
 

		sMCSystemDeviceStatus_ZJ.MCTimeDataYer=SystemTime[5];//��
		sMCSystemDeviceStatus_ZJ.MCTimeDataMon=SystemTime[4];//��
		sMCSystemDeviceStatus_ZJ.MCTimeDataDay=SystemTime[3];//��	
		sMCSystemDeviceStatus_ZJ.MCTimeDataHor=SystemTime[2];//Сʱ
		sMCSystemDeviceStatus_ZJ.MCTimeDataMin=SystemTime[1];//��
		sMCSystemDeviceStatus_ZJ.MCTimeDataSec=SystemTime[0];//��	
	
    RecieveMCDeviceStatusAndSend(  sMCSystemDeviceStatus_ZJ.MCaddress,
																	 sMCSystemDeviceStatus_ZJ.MCDeviceAddress0,
                                   sMCSystemDeviceStatus_ZJ.MCDeviceAddress1,
                                   sMCSystemDeviceStatus_ZJ.MCDeviceAddress2,
                                   sMCSystemDeviceStatus_ZJ.MCDeviceAddress3,
                                   sMCSystemDeviceStatus_ZJ.MCDeviceStatus0,
                                   sMCSystemDeviceStatus_ZJ.MCDeviceStatus1,
                                   sMCSystemDeviceStatus_ZJ.MCTimeDataSec,
                                   sMCSystemDeviceStatus_ZJ.MCTimeDataMin,
                                   sMCSystemDeviceStatus_ZJ.MCTimeDataHor,
                                   sMCSystemDeviceStatus_ZJ.MCTimeDataDay,
                                   sMCSystemDeviceStatus_ZJ.MCTimeDataMon,
                                   sMCSystemDeviceStatus_ZJ.MCTimeDataYer,
		                               sMCSystemDeviceStatus_ZJ.MCDevicetype1,
		                               sMCSystemDeviceStatus_ZJ.MCDevicetype2,
																	 code); //�豸״̬������
	return len;
}

/***********************
0x04��λ			---->0x01
0x70����			---->0x02
***********************/
unsigned char GST200_ActionConv(unsigned char state)
{
	unsigned char flag=0;
	switch(state)
	{
		case 0x04: flag=0x01;break;
		case 0x70: flag=0x02;break;
		default: break;
	}
	return flag;
}
//��������
unsigned int  ZJ_Action_Analsis(unsigned char *data,unsigned int lenth)
{
	unsigned int len=12;
 
	sMCSystemOperationStatus_ZJ.MCaddress       = 1;		//������ַ
	
	//ϵͳ״̬
	sMCSystemOperationStatus_ZJ.MCOperationType = GST200_ActionConv(data[4]);	//��λ,����

	sMCSystemOperationStatus_ZJ.MCTimeDataYer=SystemTime[5];//��
	sMCSystemOperationStatus_ZJ.MCTimeDataMon=SystemTime[4];//��
	sMCSystemOperationStatus_ZJ.MCTimeDataDay=SystemTime[3];//��	
	sMCSystemOperationStatus_ZJ.MCTimeDataHor=SystemTime[2];//Сʱ
	sMCSystemOperationStatus_ZJ.MCTimeDataMin=SystemTime[1];//��
	sMCSystemOperationStatus_ZJ.MCTimeDataSec=SystemTime[0];//��	
	
  RecieveMCOperationStatusAndSend(sMCSystemOperationStatus_ZJ.MCaddress,
		                              sMCSystemOperationStatus_ZJ.MCOperationType,
	                                sMCSystemOperationStatus_ZJ.MCTimeDataSec,
                                  sMCSystemOperationStatus_ZJ.MCTimeDataMin,
																	sMCSystemOperationStatus_ZJ.MCTimeDataHor,
																	sMCSystemOperationStatus_ZJ.MCTimeDataDay,
																	sMCSystemOperationStatus_ZJ.MCTimeDataMon,
																	sMCSystemOperationStatus_ZJ.MCTimeDataYer	
	                               );
	return len;
}
unsigned int ZJ_Analysis(unsigned char *data,unsigned int lenth)
{
	unsigned int len=0; 
	unsigned char Senddata=0x51;  
	//����״̬
	if((data[4]==GST200_HUOJING_CMD)||(data[4]==GST200_GUZHANG_CMD)||(data[4]==GST200_LIANDONG_CMD)||(data[4]==GST200_FANKUI_CMD)||(data[4]==GST200_PINGBI_CMD))
	{
		len=ZJ_ComponentState_Analsis(data,lenth);
	}
	//ϵͳ״̬
	else if(data[4]==GST200_ZHUDIANYUAN_CMD)
	{
		len=ZJ_SysState_Analsis(data,lenth);
	}
	//������Ϣ
	else if ((data[4]==GST200_FUWEI_CMD)||(data[4]==GST200_XIAOYIN_CMD))
	{
		len=ZJ_Action_Analsis(data,lenth);
	}
	
	if(len!=0)
	{
//		UARTx_Send_Data(UartInterface,(unsigned char *)&Senddata,1);
	}
	return len;
}

void ZJ_Plugin_Run(void)		//Э���������
{
	unsigned int Flag=0;
	unsigned int Flag_tmp=0;
	unsigned int len=0;
	unsigned char Senddata[3]={0x2b,0x2b,0x2b};
	//��ȡ����
	if(Flag_tmp == 0)
	{
	    UARTx_Send_Data(UartInterface,(unsigned char *)&Senddata,3);
		Flag_tmp = 1;
	}
	Data_RecieveFromHost.Lenth=RS485_Read(Data_RecieveFromHost.data);
	
	if(Data_RecieveFromHost.Lenth!=0)
	{
	//��ȷ���ж����������ݣ�ת����ʽ��
	Flag=ZJ_Integrity(Data_RecieveFromHost.data,Data_RecieveFromHost.Lenth);
	if(Flag==0)
	{
		
	}
	else
	{
		ZJ_Analysis(Data_RecieveFromHost.data,Data_RecieveFromHost.Lenth);   
	}
}	
		
	if(Sys_Second_100mS>4)//��Ѳ�죻
	{
		Sys_Second_100mS=0;
		UARTx_Send_Data(UartInterface,(unsigned char *)&Senddata,1);
	}
}


void ZJ_Plugin_Interrupt(void)//�ж����ݽ��գ�
{	
 	
}

PluginInterface D026_HW200_Plugin = 
				{
					&ZJ_Plugin_Init,					 
					&ZJ_Plugin_Run,		 
					&ZJ_Plugin_Interrupt	 
				};
#endif    











