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

u8 IPMode=0;  //（1=手动，2=自动）
volatile unsigned char Server_Interface_Mode=2;

void NB_Init(void);//NB初始化
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
	if(Server_Interface_Mode==GPRS_Mode)//无线模式 心跳1小时；
	{
		CHECK_TIME=20;	
		NB_Init();//NB初始化		
		NetHeart_Cont=36000;//1小时心跳；			
	}
	else //有线模式 心跳5分钟；		
	{
		CHECK_TIME=70;
 		W5500_Init();                       //初始化W5500
		NetHeart_Cont=3000;//5分钟心跳；
	}
	memset(flagbuf,0x20,CHECK_TIME);//网络故障
}
unsigned int Net_Send_Infor(unsigned char *data,unsigned int Lenth)
{
	unsigned int SendFlag=0;

	if(Server_Interface_Mode==GPRS_Mode) 
	{
		GPRS_buffer_send(data,Lenth);//发送数据
	}
	else
	{
//		SendFlag=NetK2_Send_Infor((unsigned char *)data,Lenth);
				if(W5_RegistFlag!=1)//网络挂载成功
		    {		
	         do_tcp_client_tx((unsigned char *)data,Lenth);//W5500发送数据；
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
			if(MIM_flag==1){printf("NB接收%d个!\r\n",SendFlag);}
			NBcont=0; 
 		}
	}
	else
	{
			 if(W5_RegistFlag!=1)//网络挂载成功
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
	//无线模式
	if(Server_Interface_Mode==GPRS_Mode)
	{
		if(NB_RegistFlag!=1)//网络挂载成功
		{
			NBcount++;
			if(NBcount>10)//每10S判断网络状态；
			{
				NBcount=0; 
				NBflag=SaveNBflag;
				SaveNBflag=gprs_check_connect();//查看网络状态
				if(MIM_flag==1){printf("NBs=%d！\r\n",SaveNBflag);}
				if((NBflag!=0)&&(SaveNBflag!=0))//2次状态判断,则为真掉线；
				{ 
					if(MIM_flag==1)
					{printf("NB网络异常=%d！\r\n",ResumeNbr);}
					ResumeNbr++;
					
					if((ResumeNbr==1)||(ResumeNbr==90))//1.掉线，则马上重启1次；   2.每15MIN重启1次；没重启1次，NB使用3条通讯；
					{
						if(MIM_flag==1)
						{printf("NB新注册=%d！次数=%d\r\n",ResumeNbr,ResumeNbr);}
				 	   NetTask_Resume();
					}
					if(ResumeNbr>=90)//2.900s/10s=90 15min重启1次；NB使用3条通讯；
					{ResumeNbr=0;}
					
					NBflag=1;
				}
				else
				{NBflag=0;
				 if(MIM_flag==1)
				 {printf("NB网络正常！\r\n");}
				}
			}
			NBcont=Get_GPRS_Receive_Length(); //是否有数据
			if(NBcont>20)//如果NB有数据
			 { //触发接收任务；	
				      if(MIM_flag==1)				 
							{printf("NB收数%d个\r\n",NBcont);}	 
							OSTaskSemPost ((OS_TCB	*)&RecieveFromServerTaskTCB,
											 (OS_OPT	)OS_OPT_POST_NO_SCHED,
											 (OS_ERR *)&err);
			 }
		}
		flag=NBflag;
	}
	//有线模式
	else
	{	
				nbr=0xFF;
			  if(W5_RegistFlag!=1)//网络挂载成功
		    {		
				nbr=do_tcp_client_RxStatus();
				}
				if(MIM_flag==1)
				{ printf("W5s=%d！",nbr); }
				if(nbr==1)//如果W5500有数据
				{ //触发接收任务；
					NET_STATE_CHECK=1;
					nbr_cot=0;
					OSTaskSemPost ((OS_TCB	*)&RecieveFromServerTaskTCB,
												 (OS_OPT	)OS_OPT_POST_NO_SCHED,
												 (OS_ERR *)&err);
				}
				else if(nbr==0)//正常
				{
					NET_STATE_CHECK=1;
					nbr_cot=0;
				}
				else//网络故障；
				{
					NET_STATE_CHECK=0;

					flag=1;
					nbr_cot++;
					if(nbr_cot%10==0)//如果网络故障次数超出10次，那么触发W5500接收，对部分寄存器进行重新初始化。
					{					
						OSTaskSemPost ((OS_TCB	*)&RecieveFromServerTaskTCB, //接收函数，W5500在10S进行一次网络重连。
													 (OS_OPT	)OS_OPT_POST_NO_SCHED,
													 (OS_ERR *)&err);
					}
					if(nbr_cot%60==0)//如果网络故障次数超出20次。//最终程序可以放到20=》40.
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
	if(i>=CHECK_TIME)//如果70个都等于当前采集状态；
	{
	 if((flagbuf[count]&(1<<5))==0)//有网络
	 {
		RunState&= ~(0x01<<5);//正常；
//		printf("网络正常！\r\n");
	 }
	 else
	 {		
		 RunState|= 0x01<<5;//故障
//		 printf("网络故障！\r\n");
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
		{printf("网络异常重启！\r\n");}
		if(Server_Interface_Mode==GPRS_Mode)
		{
				NB_RegistFlag=1;
				GPRS_RESET_HIGH;			
				OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); //延时10ms
			  GPRS_RESET_LOW;
				OSTimeDlyHMSM(0,0,6,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时10ms
				flag=gprs_send_start();//挂载网络成功否
				if((flag==1)&&(Server_Interface_Mode==GPRS_Mode))//网络异常
				{
						printf("NB挂网失败！\n\r");	
						NB_RegistFlag=2;
				}
				else //正常；
				{
						NB_RegistFlag=0;
						printf("NB挂网成功！\n\r");					
				}
				NetTask_Suspend();
		}
		else
		{
				W5_RegistFlag=1;//其他W5500功能关闭
				//获取动态IP
			  if(IPMode!=1)//自动
  			{
					dhcp_ok=0;
					flag=get_DHCP();
					if(flag==1)
					{IP_Write_Flash();}//写入IP
				}
			  //延时10ms
				OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); 
  			//自动域名解析
	      flag=get_DNS();
				if(flag==1)
				{RemoteIP_Write_Flash();}//写入IP
					
       	W5_RegistFlag=0;
			  //新IP、新地址写入flash；
  			NetTask_Suspend();
		}
	}

}

unsigned char NetStatus(void)
{
  if(Server_Interface_Mode==GPRS_Mode)//NB通讯
	{
		if(NB_RegistFlag==0)//正常
		{return 0;}
		else
		{return 1;}
	}
	else//w5500通讯
	{
		if(NET_STATE_CHECK==1)//正常
		{return 0;}
		else
		{return 1;}	
	}
}

unsigned char NetStatus_Print(void)
{
 	printf("2.网络设置：%d  ",Server_Interface_Mode);
	
	if(Server_Interface_Mode==GPRS_Mode)
	{
		printf("无线模式。\n\r");
	}
	else
	{
		printf("有线模式。\n\r");		
	}
}
