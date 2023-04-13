/*
DE_1000
DE_1010
*/
#include "includes.h"
#include "bsp_FB1000Y.h"
#include "lcd.h"
#include "SDcontrol.h"
//#include "CAN1ProtocolParsing.h"
#include "MsgCANProcessDrv.h"
#include "tcp_demo.h"
#include "dns.h"
#include "app.h"
#include "w5500_ctl.h"
#include "PrinterATcmd.h"
/************************************************
������־��
1.20180507 

************************************************/
//#define WD_T
//#define TK_T


//�������ȼ�
#define START_TASK_PRIO		3
//�����ջ��С
#define START_STK_SIZE 		128
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

//�������ȼ�
#define SENDTOSERVER_TASK_PRIO		4
//�����ջ��С
#define SENDTOSERVER_STK_SIZE 		0x200
//������ƿ�
OS_TCB SendToServerTaskTCB;
//�����ջ
CPU_STK SENDTOSERVER_TASK_STK[SENDTOSERVER_STK_SIZE];
void SendToServer_task(void *p_arg);

//�������ȼ�
#define RECIEVEFROMSERVER_TASK_PRIO		6
//�����ջ��С
#define RECIEVEFROMSERVER_STK_SIZE 		0x200
//������ƿ�
OS_TCB RecieveFromServerTaskTCB;
//�����ջ
CPU_STK RECIEVEFROMSERVER_TASK_STK[RECIEVEFROMSERVER_STK_SIZE];
void RecieveFromServer_task(void *p_arg);

//�������ȼ�
#define HOST_TASK_PRIO		7            
//�����ջ��С
#define HOST_STK_SIZE 		0x200
//������ƿ�
OS_TCB hostTaskTCB;
//�����ջ
CPU_STK HOST_TASK_STK[HOST_STK_SIZE];
void host_task(void *p_arg);

//�������ȼ�
#define PREHOST_TASK_PRIO		5
//�����ջ��С
#define PREHOST_STK_SIZE 		0x200
//������ƿ�
OS_TCB prehostTaskTCB;
//�����ջ
CPU_STK PREHOST_TASK_STK[PREHOST_STK_SIZE];
void prehost_task(void *p_arg);

//�������ȼ�
//#define KEY_TASK_PRIO		8
#define KEY_TASK_PRIO		2
//�����ջ��С
#define KEY_STK_SIZE 		128
//������ƿ�
OS_TCB keyTaskTCB;
//�����ջ
CPU_STK KEY_TASK_STK[KEY_STK_SIZE];
//������
void key_task(void *p_arg);
CPU_INT32U g_key=0;

//�������ȼ�
#define LCD_TASK_PRIO		9        
//�����ջ��С
#define LCD_STK_SIZE		0x200
//��Ϣ���д�С
#define OS_MSG_QTY_LCDNUM		10
//������ƿ�
OS_TCB	lcdTaskTCB;
//�����ջ
__align(8) CPU_STK	LCD_TASK_STK[LCD_STK_SIZE];
//������
void lcd_task(void *p_arg);

//�������ȼ�
#define PERIOD_TASK_PRIO		10
//�����ջ��С
#define PERIOD_STK_SIZE 		0x200 //0x200
//������ƿ�
OS_TCB PeriodTaskTCB;
//�����ջ
CPU_STK PERIOD_TASK_STK[PERIOD_STK_SIZE];
//������
void Period_task(void *p_arg);

//�������ȼ�
#define NET_TASK_PRIO		11
//�����ջ��С
#define NET_STK_SIZE 		0x200
//������ƿ�
OS_TCB NetTaskTCB;
//�����ջ
CPU_STK NET_TASK_STK[NET_STK_SIZE];
//������
void Net_task(void *p_arg);

OS_TMR tmrWdg;
 

static unsigned char tmrWdg_step=4;
static unsigned char tmrWdg_Flag=0;
const char lcdwdg_test[]="asl....";
//ȫ�ֱ���
unsigned char g_RunState=0;
	
#ifdef FB1010
const char FB1010_sv[20]="1010A_SV1.0_19����";//����=190416
#endif

#ifdef FB1030
const char FB1010_sv[20]="1030A_SV1.0_190327";
#endif

int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ�������
	bsp_init();
	
	OSInit(&err);		//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();//�����ٽ���
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
							(CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);  //����UCOSIII
	while(1);
}
void SendTaskFeed(void)
{
	if((tmrWdg_Flag&(0x1<<0))!=0)
	{
		tmrWdg_Flag&=~(0x1<<0);
#ifdef WD_T		
		printf("0ι_");
#endif		
	}
}
void RecieveTaskFeed(void)
{
	if((tmrWdg_Flag&(0x1<<1))!=0)
	{
		tmrWdg_Flag&=~(0x1<<1);
#ifdef WD_T				
		printf("1ι_");
#endif			
	}
}
void HostTaskFeed(void)
{
	if((tmrWdg_Flag&(0x1<<2))!=0)
	{
		tmrWdg_Flag&=~(0x1<<2);
#ifdef WD_T				
		printf("2ι_");
#endif			
	}
}

void lcdTaskFeed(void)
{
	if((tmrWdg_Flag&(0x1<<3))!=0)
	{
		tmrWdg_Flag&=~(0x1<<3);
#ifdef WD_T		
		printf("3ι_");
#endif			
	}
}
void PeridTaskFeed(void)
{
	if((tmrWdg_Flag&(0x1<<4))!=0)
	{
		tmrWdg_Flag&=~(0x1<<4);
#ifdef WD_T		
  	printf("4ι_");
#endif			
	}
}

void PreHostTaskFeed(void)
{
	if((tmrWdg_Flag&(0x1<<5))!=0)
	{
		tmrWdg_Flag&=~(0x1<<5);
#ifdef WD_T		
		printf("5ι_");
#endif			
	}
}
void tmrWdg_callback(void *p_tmr,void *p_arg)
{
	OS_ERR err;
	CPU_TS ts;
	static unsigned int cnt=0;
	
	if((tmrWdg_Flag&0x3f)!=0)
	{
//		if(tmrWdg_num<=3)tmrWdg_num++;
//		else tmrWdg_num=0;
		tmrWdg_Flag&=0x3f;
		cnt++;
		if(cnt>10)
		{
			cnt=0;
		}
	}
	else
	{
		cnt=0;
		IWDG_Feed();
		if(tmrWdg_step<5)
		{tmrWdg_step++;
		 if(tmrWdg_step==2){tmrWdg_step=3;}//����2��
		}
		else
		{tmrWdg_step=0;}
#ifdef WD_T		
		printf("%d��_",tmrWdg_step);
#endif			
	}
	
	
	switch(tmrWdg_step)
	{
		case 0:
			if((tmrWdg_Flag&(0x01<<0))==0)
			{
				tmrWdg_Flag|=0x1<<0;
				OSTaskSemPost ((OS_TCB	*)&SendToServerTaskTCB,
					(OS_OPT	)OS_OPT_POST_NO_SCHED,
					(OS_ERR *)&err);
			}
		break;
		case 1:
			if((tmrWdg_Flag&(0x01<<1))==0)
			{
				tmrWdg_Flag|=0x1<<1;
				OSTaskSemPost ((OS_TCB	*)&RecieveFromServerTaskTCB,
					(OS_OPT	)OS_OPT_POST_NO_SCHED,
					(OS_ERR *)&err);
			}
		break;
		case 2:
//			if((tmrWdg_Flag&(0x01<<2))==0)
//			{
//				tmrWdg_Flag|=0x1<<2;
////				OSTaskSemPost ((OS_TCB	*)&hostTaskTCB,
////					(OS_OPT	)OS_OPT_POST_NO_SCHED,
////					(OS_ERR *)&err);
//			}
		break;
		case 3:
			if((tmrWdg_Flag&(0x01<<3))==0)
			{
				tmrWdg_Flag|=0x1<<3;
				OS_TaskQPost((OS_TCB	*)&lcdTaskTCB,
							(void		*)&lcdwdg_test,
							(OS_MSG_SIZE)sizeof(lcdwdg_test),
							(OS_OPT		)OS_OPT_POST_FIFO,
							(CPU_TS		)ts,
							(OS_ERR		*)&err
							);
			}
		break;
		case 4:
			tmrWdg_Flag|=0x1<<4;
		break;
		case 5:
			tmrWdg_Flag|=0x1<<5;
		break;
		default:
		break;
	}
	
}
//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif
	
	OSTmrCreate((OS_TMR		*)&tmrWdg,
				(CPU_CHAR	*)"tmrWdg",
				(OS_TICK	)200,//2S
				(OS_TICK	)200,
				(OS_OPT		)OS_OPT_TMR_PERIODIC,
				(OS_TMR_CALLBACK_PTR)tmrWdg_callback,
				(void*	)0,
				(OS_ERR*	)&err);
				
	OS_CRITICAL_ENTER();	//�����ٽ���
	//����SERVER����
	OSTaskCreate((OS_TCB 	* )&SendToServerTaskTCB,
				 (CPU_CHAR	* )"send to server task",
                 (OS_TASK_PTR )SendToServer_task,		
                 (void		* )0,
                 (OS_PRIO	  )SENDTOSERVER_TASK_PRIO,
                 (CPU_STK   * )&SENDTOSERVER_TASK_STK[0],
                 (CPU_STK_SIZE)SENDTOSERVER_STK_SIZE/10,
                 (CPU_STK_SIZE)SENDTOSERVER_STK_SIZE,	
                 (OS_MSG_QTY  )0,
                 (OS_TICK	  )0,
                 (void   	* )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
	//����SERVER����
	OSTaskCreate((OS_TCB 	* )&RecieveFromServerTaskTCB,
				 (CPU_CHAR	* )"Recieve From server task",
                 (OS_TASK_PTR )RecieveFromServer_task,		
                 (void		* )0,
                 (OS_PRIO	  )RECIEVEFROMSERVER_TASK_PRIO,
                 (CPU_STK   * )&RECIEVEFROMSERVER_TASK_STK[0],
                 (CPU_STK_SIZE)RECIEVEFROMSERVER_STK_SIZE/10,
                 (CPU_STK_SIZE)RECIEVEFROMSERVER_STK_SIZE,	
                 (OS_MSG_QTY  )0,
                 (OS_TICK	  )0,
                 (void   	* )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
				 
//	//����HOST����
//	OSTaskCreate((OS_TCB 	* )&hostTaskTCB,
//				 (CPU_CHAR	* )"host task",
//                 (OS_TASK_PTR )host_task,
//                 (void		* )0,
//                 (OS_PRIO	  )HOST_TASK_PRIO,
//                 (CPU_STK   * )&HOST_TASK_STK[0],
//                 (CPU_STK_SIZE)HOST_STK_SIZE/10,
//                 (CPU_STK_SIZE)HOST_STK_SIZE,
//                 (OS_MSG_QTY  )0,
//                 (OS_TICK	  )0,
//                 (void   	* )0,
//                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
//                 (OS_ERR 	* )&err);

	//����PREHOST����
	OSTaskCreate((OS_TCB 	* )&prehostTaskTCB,
				 (CPU_CHAR	* )"prehost task",
                 (OS_TASK_PTR )prehost_task,
                 (void		* )0,
                 (OS_PRIO	  )PREHOST_TASK_PRIO,
                 (CPU_STK   * )&PREHOST_TASK_STK[0],
                 (CPU_STK_SIZE)PREHOST_STK_SIZE/10,
                 (CPU_STK_SIZE)PREHOST_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK	  )0,
                 (void   	* )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
								 
	//����key����
	OSTaskCreate((OS_TCB 	* )&keyTaskTCB,
				 (CPU_CHAR	* )"key task",
                 (OS_TASK_PTR )key_task,
                 (void		* )0,
                 (OS_PRIO	  )KEY_TASK_PRIO,
                 (CPU_STK   * )&KEY_TASK_STK[0],
                 (CPU_STK_SIZE)KEY_STK_SIZE/10,
                 (CPU_STK_SIZE)KEY_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK	  )0,
                 (void   	* )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
			 
	//����LCD����
	OSTaskCreate((OS_TCB 	* )&lcdTaskTCB,
				 (CPU_CHAR	* )"lcd test task",
                 (OS_TASK_PTR )lcd_task,
                 (void		* )0,
                 (OS_PRIO	  )LCD_TASK_PRIO,
                 (CPU_STK   * )&LCD_TASK_STK[0],
                 (CPU_STK_SIZE)LCD_STK_SIZE/10,
                 (CPU_STK_SIZE)LCD_STK_SIZE,
                 (OS_MSG_QTY  )OS_MSG_QTY_LCDNUM,
                 (OS_TICK	  )0,
                 (void   	* )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
				 
	//����Period����
	OSTaskCreate((OS_TCB 	* )&PeriodTaskTCB,
				 (CPU_CHAR	* )"Period test task",
                 (OS_TASK_PTR )Period_task,
                 (void		* )0,
                 (OS_PRIO	  )PERIOD_TASK_PRIO,
                 (CPU_STK   * )&PERIOD_TASK_STK[0],
                 (CPU_STK_SIZE)PERIOD_STK_SIZE/10,
                 (CPU_STK_SIZE)PERIOD_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK	  )0,
                 (void   	* )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
				 
	//����Net����
	OSTaskCreate((OS_TCB 	* )&NetTaskTCB,
				 (CPU_CHAR	* )"Net test task",
                 (OS_TASK_PTR )Net_task,
                 (void		* )0,
                 (OS_PRIO	  )NET_TASK_PRIO,
                 (CPU_STK   * )&NET_TASK_STK[0],
                 (CPU_STK_SIZE)NET_STK_SIZE/10,
                 (CPU_STK_SIZE)NET_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK	  )0,
                 (void   	* )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
				 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����
	OS_CRITICAL_EXIT();	//�����ٽ���
}
//server������ 
//����ִ�У�����ʱ�򣬾���W5500�������ݣ�
extern unsigned char NET_STATE_CHECK;
void SendToServer_task(void *p_arg)
{
	OS_ERR err;
	CPU_TS ts;
	unsigned char flag=0;
	p_arg = p_arg;
//	Net_Init();
	while(1)
	{
			OSTaskSemPend((OS_TICK	)0,
                      (OS_OPT)OS_OPT_PEND_BLOCKING,
                      (CPU_TS		*)&ts,
                      (OS_ERR		*)&err
					);

		for(;;)
		{
#ifdef TK_T			
			printf("1.��������������");
#endif
			
			//ι����
			SendTaskFeed();
	
			//����״̬�жϣ������쳣���˳�����
			if(((g_RunState&(1<<5))==0)&&(NetStatus()==0))//����������
			{  
			}
			else//�����쳣��
			{break;
			}
			
			//���ݷ��ͣ�		
			flag=SendFrameToServer();
			OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err);
			if(flag==0) break;//����ջΪ�գ�û�����ݷ��ͣ����˳���������
			else 
			{
				if((g_RunState&(1<<5))==0)//������
				{
				 OSTimeDlyHMSM(0,0,4,0,OS_OPT_TIME_HMSM_STRICT,&err);//3S-4S
				}
				else//������
				{
					OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
				}
			}

			OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err);
		}
	}
}
//����������ѯW5500���ճ��ȣ�������Ȳ�Ϊ0���򴥷��������
extern void BT_PeriodTask(void);
void RecieveFromServer_task(void *p_arg)
{
	OS_ERR err;
	CPU_TS ts;
	unsigned char flag=0;

	p_arg = p_arg;

		
	while(1)
	{
		OSTaskSemPend((OS_TICK	)0,
                      (OS_OPT)OS_OPT_PEND_BLOCKING,
                      (CPU_TS		*)&ts,
                      (OS_ERR		*)&err
					);
#ifdef TK_T		
		printf("2.��������������");
#endif
		printf("2.��������������");
		RecieveTaskFeed();
		
		BT_PeriodTask();     //Զ�����������ж���
				
		OSTimeDlyHMSM(0,0,1,100,OS_OPT_TIME_HMSM_STRICT,&err);
		flag=RecieveFromServer_Handle();
		if(flag!=0)
		{
		}
	}
}

unsigned int cnt;
extern unsigned char g_HostRecFlag;
//host������
//void host_task(void *p_arg)
//{
//	OS_ERR err;
//	CPU_TS ts;
//	unsigned char flag=0;
//	unsigned char i;
//	p_arg = p_arg;
//	SDdata_Init();
//	printf("ϵͳ������\n\r");	
////	g_RunState&=~(1<<6);				//��·����
//	while(1)
//	{
//		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err);
//		HostTaskFeed();
////		for(i=0;i<100;i++)
////		{
////			OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err);
////			if(g_HostRecFlag==1)
////			{
////				g_HostRecFlag=0;
////				break;
////			}
////		}
////		if(SDDataInFlagt==1){SD_DATAIN();SDDataInFlagt=0;} //ִ�б�����뺯����
////		flag=ProtocolBoardHandle();
////		if(flag==4)
////		{
////			g_RunState|=(1<<6);					//��·����
////		}
////		else if(flag==1)
////		{
////			g_RunState&=~(1<<6);				//��·����
////		}
//	}
//}
//prehost������������
//extern CANParameterConfiguration  gCAN1SystemParameter;
extern unsigned char ThreeInOne_ProtocolBoard_Run(void);
extern void TIM2_Int_Init(unsigned short int arr, unsigned short int psc);
#ifdef	XYB_DYJ	
const unsigned short VERS_msg=11000;//21020=�汾V21+�����ͺ�D020��Զ��������Э��汾��
#endif	
void prehost_task(void *p_arg)
{
	static	unsigned char pre_cont=0;
	OS_ERR err;
	CPU_TS ts;
	unsigned char flag=0;
	p_arg = p_arg;

//	SDdata_Init();
//	ProID_Init();
//  ID_Print();
 	printf("%s��\n\r",FB1010_sv);
	
	TIM2_Int_Init(1000,7200);	//��������100mS��	
#ifdef	XYB_DYJ	
	CAN1_Mode_Init(CAN1,gCANSystemParameter);   //CANģʽ��ʼ��ʹ��
#else	
	ProalyInit();	//Э�������ʼ����
#endif	
	
	while(1)
	{
#ifdef TK_T		
//		printf("3.��������������");
#endif
 		PreHostTaskFeed(); //����ι��������

#ifdef	XYB_DYJ	
		if(DataSendToStatus()==1) //����л𾯴洢�ռ䣬���ȡЭ��屨����Ϣ�� ���Ͽռ䣡//�������ݳ��ȣ����512���ֽڣ�
		{  
 			ThreeInOne_ProtocolBoard_Run();   		//Э������һ��CANЭ��ִ�к���
		}
#else		
    ProalyRun();//Э�����������
#endif	
		
		//������·�жϣ�	
		if(MClineFlaut_flag==0xFFFF0000) //��ʼ״̬��
		{ 
		}
		else if (MClineFlaut_flag==0)
		{
				g_RunState&=~(1<<6);				//��·�������̵���
		}
    else //����λ��1��
		{
        g_RunState|=(1<<6);					//��·���ϣ��Ƶ���
		}
		
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err);	
		pre_cont++;
		if(pre_cont>30)//��������ִ��300mS��Ԥ��100mS����������
		{  pre_cont=0;
		   OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);			
		}
		
	}
}


//key������
void key_task(void *p_arg)
{
	OS_ERR err;
	CPU_TS ts;
	static unsigned int Key0_count=0;
	unsigned int key=0;
	p_arg = p_arg;
	while(1)
	{	
#ifdef TK_T		
				printf("4.��������������");
#endif
				key=KEY_SCAN();
				if(key!=0)			//��������
				{
//	  			printf("K=%x\r\n",key);
					Key0_count=0;
					if((key&0x80000000)==0x80000000)		//Ϊ��Ч������
					{
						Buzzer_Action(5);
						g_key=key&0x7FFFFFFF;
						OS_TaskQPost((OS_TCB	*)&lcdTaskTCB,
									(void		*)&g_key,
									(OS_MSG_SIZE)sizeof(g_key),
									(OS_OPT		)OS_OPT_POST_FIFO,
									(CPU_TS		)ts,
									(OS_ERR		*)&err
									);
					}
				}
				else
				{
					Key0_count++;
				}
				
				OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ10ms
				
				if(Key0_count>50)
				{
					Key0_count=0;
					OSTaskSuspend(&keyTaskTCB,&err);
				}
 		
	}
}
//LCD��������
void lcd_task(void *p_arg)
{
	OS_MSG_SIZE size;
	CPU_TS ts;
	OS_ERR err;
	void *ptr;
	unsigned char flag=0;
	Server_Data_Typedef *pServerData=NULL;
//	unsigned char *pServer_Rd_Addr=NULL;
	unsigned char Rd_Addr=1;
	unsigned char *pServer_Rd_Addr=&Rd_Addr;
//	int i=0;
	p_arg = p_arg;	
//	refresh_lcd();
		
	while(1)
	{
		ptr=OSTaskQPend((OS_TICK	)0,
						(OS_OPT)OS_OPT_PEND_BLOCKING,
						(OS_MSG_SIZE *)&size,
						(CPU_TS		*)&ts,
						(OS_ERR		*)&err
						);
#ifdef TK_T	  
		printf("5.ָʾ������������");
#endif		

		if(ptr==&g_key)
		{
			key_Explain(*((unsigned int *)ptr));//�������ͣ�
//			refresh_lcd();
		}
		else if((ptr==&g_self_check_step)&&(*(unsigned char *)ptr!=0))//�Լ�
		{
 			lcd_SelfCheck(g_RunState);
		}
	  else if(ptr==&g_RunState)
		{
			RunState_Action(g_RunState);//LED�ƵĿ��ƣ�
		}
		else if(ptr==&lcdwdg_test)
		{
			lcdTaskFeed();//ι����
		}
		else
		{
			pServerData=NULL;
//			pServer_Rd_Addr=NULL;
			pServerData=IsReportAddr((unsigned char *)ptr,size,pServer_Rd_Addr);		
			if(pServerData!=NULL)
			{
				flag=Server_Action(pServerData,pServer_Rd_Addr);//���ݽ��ͣ�LED���ƣ����ȿ��ƣ�
//				if(flag>=1)refresh_lcd();//zhaoning�Ƿ�ˢ��Ļ��
			}
			else
			{
//				refresh_lcd();
			}
		}
	}
}
void RefreshRunLedState(void)
{
	OS_ERR err;
	CPU_TS ts;
	OS_TaskQPost((OS_TCB	*)&lcdTaskTCB,
				(void		*)&g_RunState,
				(OS_MSG_SIZE)sizeof(g_RunState),
				(OS_OPT		)OS_OPT_POST_FIFO,
				(CPU_TS		)ts,
				(OS_ERR		*)&err
				);
}

void NetTask_Resume(void);
void RunState_Handle(unsigned char state)
{
	static unsigned char laststate=0xff;
	if((state&0x78)>(laststate&0x78))
	{
		RunningStateFrame();
		if(((state&(0x01<<5))-(laststate&(0x01<<5)))==(0x01<<5))
		{
//			NetTask_Resume();
		}
	}
	if(laststate!=state)
	{
		laststate=state;
		RefreshRunLedState();
	}
}

//ÿ����ִ��һ�Σ�
void Period_task(void *p_arg)
{
	unsigned int Period_Cnt=0;
	unsigned char state=0;
	char data[0x20];
	unsigned int nbr=0;
	unsigned char W5500state=0;
	static unsigned char Initstate=0;
	
	OS_ERR err;
	p_arg = p_arg;
	
	OSTmrStart((OS_TMR		*)&tmrWdg,(OS_ERR*	)&err);
	
	while(1)
	{
#ifdef TK_T		
		printf("6.��������������");
#endif
		Period_Cnt++;
		PeridTaskFeed();
		
		ATcommond_Logic();//AT���������
//    if(Period_Cnt%100==0)//1s����һ�� ��ӡCPU�����ʣ�		
//		{ printf("~CP=%d��\n\r",OSStatTaskCPUUsage);		}
		
    if(Period_Cnt%60==0)//60s=1min  ��ӡϵͳʱ�䣻
    {
		   Time_System_Get();
	  }
		
		if(Period_Cnt%1==0)//ÿ���� ��70��=70S	
		{
			state=UpdataRunState(); //�ڲ��ж�5500�Ƿ���յ����ݣ��������������
			g_RunState&=~0x38;
			g_RunState|=state;
			state=g_RunState;
			RunState_Handle(state);
		}
		
//		if(Period_Cnt%NetHeart_Cont==0)//6000=60s=1min,��������ÿ����1�Σ�����W5500�ж�������ϵ���ʱ����100S��
		if(NetHeart_SndCot>=NetHeart_Cont)//100mS��ʱ��		
		{
			NetHeart_SndCot=0;
			
			if((g_RunState&(1<<5))==0)//����������
			{
			  HeartBeatFrame();//�Ϸ�������
			}	
			ResumeNbr=0;//NBע��������㣻
		}
		
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s

	}
}

//��������
void NetTask_Resume(void)
{
	OS_ERR err;
	OSTaskResume(&NetTaskTCB,&err);
}
void NetTask_Suspend(void)
{
	OS_ERR err;
	OSTaskSuspend(&NetTaskTCB,&err);
}
void Net_task(void *p_arg)
{
	p_arg = p_arg;
	Net_Restart();
}


/*****************************************************************************************************************							 
									//CAN1�жϺ���
*****************************************************************************************************************/
//void CAN1_RX0_IRQHandler(void)
//{ 
//   YH_CAN1_RX0_IRQHandler();	
//}
