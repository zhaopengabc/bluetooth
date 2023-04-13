#include "net.h"
#include "k2.h"
#include "gprs.h"
#include "w25q16.h"
#include "w5500_ctl.h"
#include "tcp_demo.h"
#include "sys.h"
#include "includes.h"
#include "bc95.h"
#include "SendToServerDataType.h"
#include "dhcp.h"
#include "dns.h"
//#define DEBOUNCE_TIMES 70
//#define CHECK_TIME     20
//#define CHECK_TIME     70
#define GPRS_Mode		2  

u8 IPMode=0;  //��1=�ֶ���2=�Զ���
volatile unsigned char Server_Interface_Mode=2;

void NB_Init(void);//NB��ʼ��
static unsigned int NBcont=0; 
volatile unsigned int NetHeart_Cont=5000; 
volatile unsigned int NetHeart_SndCot=0; 

extern unsigned char MIM_flag;
static unsigned char CHECK_TIME=70;
static unsigned char flagbuf[80];

void Net_Init(void)
{
	unsigned char buf[0x20];
	Readflash_Config(buf,SERVER_INTERFACE_CONFIG_CMD);
	Server_Interface_Mode=buf[0];
	if(Server_Interface_Mode==GPRS_Mode)//����ģʽ ����1Сʱ��
	{
		CHECK_TIME=20;	
		NB_Init();//NB��ʼ��		
		NetHeart_Cont=36000;//1Сʱ������			
	}
	else //����ģʽ ����5���ӣ�		
	{
		CHECK_TIME=70;
 		W5500_Init();                       //��ʼ��W5500
		NetHeart_Cont=3000;//5����������
	}
	memset(flagbuf,0x20,CHECK_TIME);//�������
}
unsigned int Net_Send_Infor(unsigned char *data,unsigned int Lenth)
{
	unsigned int SendFlag=0;

	if(Server_Interface_Mode==GPRS_Mode) 
	{
		GPRS_buffer_send(data,Lenth);//��������
	}
	else
	{
//		SendFlag=NetK2_Send_Infor((unsigned char *)data,Lenth);
				if(W5_RegistFlag!=1)//������سɹ�
		    {		
	         do_tcp_client_tx((unsigned char *)data,Lenth);//W5500�������ݣ�
				}
	}
	return SendFlag;
}

unsigned int Net_Rx_Read(unsigned char *Rxdata)
{
	unsigned int SendFlag=0;
	if(Server_Interface_Mode==GPRS_Mode)
	{
		if((NB_RegistFlag==0)&&(NBcont>20))
		{
//		  SendFlag=Get_GPRS_Rec(Rxdata);
			SendFlag=Get_GPRS_Receive(Rxdata,SERVER_DATA_NUM);
			if(MIM_flag==1){printf("NB����%d��!\r\n",SendFlag);}
			NBcont=0; 
 		}
	}
	else
	{
			 if(W5_RegistFlag!=1)//������سɹ�
		   {		
	     	SendFlag=do_tcp_client_rx(Rxdata);
			 }
	}
	return SendFlag;
}

extern OS_TCB RecieveFromServerTaskTCB;
extern unsigned char NET_STATE_CHECK;

void NET_SET(unsigned char data) 
{
	unsigned char i=0;

	if((data==1)||(data==0))
	{
		for(i=0;i<CHECK_TIME;i++)
		{
			flagbuf[i]|=data<<5;
		}
	}
}

u32 gprs_check_connect(void);
void NetTask_Resume(void);
volatile unsigned char ResumeNbr=0;

unsigned char NetWork_State(void)
{
	static unsigned char count=0;
//	static unsigned char flagbuf[CHECK_TIME];
	unsigned char i=0;
	unsigned char flag=0;
	static unsigned char RunState=0;
	static unsigned char nbr=0,nbr_cot=0;
	OS_ERR err;
	unsigned char cnt=0;	
	static unsigned char NBcount=0;
	static unsigned char NBflag=1,SaveNBflag=1;
	
	count++;
	if(count>=CHECK_TIME)count=0;
	//����ģʽ
	if(Server_Interface_Mode==GPRS_Mode)
	{
		if(NB_RegistFlag!=1)//������سɹ�
		{
			NBcount++;
			if(NBcount>10)//ÿ10S�ж�����״̬��
			{
				NBcount=0; 
				NBflag=SaveNBflag;
				SaveNBflag=gprs_check_connect();//�鿴����״̬
				if(MIM_flag==1){printf("NBs=%d��\r\n",SaveNBflag);}
				if((NBflag!=0)&&(SaveNBflag!=0))//2��״̬�ж�,��Ϊ����ߣ�
				{ 
					if(MIM_flag==1)
					{printf("NB�����쳣=%d��\r\n",ResumeNbr);}
					ResumeNbr++;
					
					if((ResumeNbr==1)||(ResumeNbr==90))//1.���ߣ�����������1�Σ�   2.ÿ15MIN����1�Σ�û����1�Σ�NBʹ��3��ͨѶ��
					{
						if(MIM_flag==1)
						{printf("NB��ע��=%d������=%d\r\n",ResumeNbr,ResumeNbr);}
				 	   NetTask_Resume();
					}
					if(ResumeNbr>=90)//2.900s/10s=90 15min����1�Σ�NBʹ��3��ͨѶ��
					{ResumeNbr=0;}
					
					NBflag=1;
				}
				else
				{NBflag=0;
				 if(MIM_flag==1)
				 {printf("NB����������\r\n");}
				}
			}
			NBcont=Get_GPRS_Receive_Length(); //�Ƿ�������
			if(NBcont>20)//���NB������
			 { //������������	
				      if(MIM_flag==1)				 
							{printf("NB����%d��\r\n",NBcont);}	 
							OSTaskSemPost ((OS_TCB	*)&RecieveFromServerTaskTCB,
											 (OS_OPT	)OS_OPT_POST_NO_SCHED,
											 (OS_ERR *)&err);
			 }
		}
		flag=NBflag;
	}
	//����ģʽ
	else
	{	
				nbr=0xFF;
			  if(W5_RegistFlag!=1)//������سɹ�
		    {		
				nbr=do_tcp_client_RxStatus();
				}
				if(MIM_flag==1)
				{ printf("W5s=%d��",nbr); }
				if(nbr==1)//���W5500������
				{ //������������
					NET_STATE_CHECK=1;
					nbr_cot=0;
					OSTaskSemPost ((OS_TCB	*)&RecieveFromServerTaskTCB,
												 (OS_OPT	)OS_OPT_POST_NO_SCHED,
												 (OS_ERR *)&err);
				}
				else if(nbr==0)//����
				{
					NET_STATE_CHECK=1;
					nbr_cot=0;
				}
				else//������ϣ�
				{
					NET_STATE_CHECK=0;

					flag=1;
					nbr_cot++;
					if(nbr_cot%10==0)//���������ϴ�������10�Σ���ô����W5500���գ��Բ��ּĴ����������³�ʼ����
					{					
						OSTaskSemPost ((OS_TCB	*)&RecieveFromServerTaskTCB, //���պ�����W5500��10S����һ������������
													 (OS_OPT	)OS_OPT_POST_NO_SCHED,
													 (OS_ERR *)&err);
					}
					if(nbr_cot%60==0)//���������ϴ�������20�Ρ�//���ճ�����Էŵ�20=��40.
					{
						NetTask_Resume();
          }
				}

	}
	
	flagbuf[count]=flag<<5;
	
	for(i=0;i<CHECK_TIME;i++)
	{
		if((flagbuf[i]&(1<<5))!=(flagbuf[count]&(1<<5)))break;
	}
	if(i>=CHECK_TIME)//���70�������ڵ�ǰ�ɼ�״̬��
	{
	 if((flagbuf[count]&(1<<5))==0)//������
	 {
		RunState&= ~(0x01<<5);//������
//		printf("����������\r\n");
	 }
	 else
	 {		
		 RunState|= 0x01<<5;//����
//		 printf("������ϣ�\r\n");
	 }
	}
	return RunState;
}
void NetTask_Suspend(void);

extern u8 dhcp_ok;
extern void IP_Write_Flash(void);
extern void RemoteIP_Write_Flash(void);

void Net_Restart(void)
{
	unsigned char flag=0,status=0,W5_Flag=0;
	OS_ERR err;

	 if(Server_Interface_Mode==GPRS_Mode)
	 {}
   else
	 {			
		NetTask_Suspend();
	 }			
	
	while(1)
	{
//		if(MIM_flag==1)
		{printf("�����쳣������\r\n");}
		if(Server_Interface_Mode==GPRS_Mode)
		{
				NB_RegistFlag=1;
				GPRS_RESET_HIGH;			
				OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ10ms
			  GPRS_RESET_LOW;
				OSTimeDlyHMSM(0,0,6,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ10ms
				flag=gprs_send_start();//��������ɹ���
				if((flag==1)&&(Server_Interface_Mode==GPRS_Mode))//�����쳣
				{
						printf("NB����ʧ�ܣ�\n\r");	
						NB_RegistFlag=2;
				}
				else //������
				{
						NB_RegistFlag=0;
						printf("NB�����ɹ���\n\r");					
				}
				NetTask_Suspend();
		}
		else
		{
				W5_RegistFlag=1;//����W5500���ܹر�
				//��ȡ��̬IP
			  if(IPMode!=1)//�Զ�
  			{
					dhcp_ok=0;
					flag=get_DHCP();
					if(flag==1)
					{IP_Write_Flash();}//д��IP
				}
			  //��ʱ10ms
				OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); 
  			//�Զ���������
	      flag=get_DNS();
				if(flag==1)
				{RemoteIP_Write_Flash();}//д��IP
					
       	W5_RegistFlag=0;
			  //��IP���µ�ַд��flash��
  			NetTask_Suspend();
		}
	}

}

unsigned char NetStatus(void)
{
  if(Server_Interface_Mode==GPRS_Mode)//NBͨѶ
	{
		if(NB_RegistFlag==0)//����
		{return 0;}
		else
		{return 1;}
	}
	else//w5500ͨѶ
	{
		if(NET_STATE_CHECK==1)//����
		{return 0;}
		else
		{return 1;}	
	}
}

unsigned char NetStatus_Print(void)
{
 	printf("2.�������ã�%d  ",Server_Interface_Mode);
	
	if(Server_Interface_Mode==GPRS_Mode)
	{
		printf("����ģʽ��\n\r");
	}
	else
	{
		printf("����ģʽ��\n\r");		
	}
}
