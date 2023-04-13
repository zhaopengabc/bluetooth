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
更新日志：
1.20180507 

************************************************/
//#define WD_T
//#define TK_T


//任务优先级
#define START_TASK_PRIO		3
//任务堆栈大小
#define START_STK_SIZE 		128
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

//任务优先级
#define SENDTOSERVER_TASK_PRIO		4
//任务堆栈大小
#define SENDTOSERVER_STK_SIZE 		0x200
//任务控制块
OS_TCB SendToServerTaskTCB;
//任务堆栈
CPU_STK SENDTOSERVER_TASK_STK[SENDTOSERVER_STK_SIZE];
void SendToServer_task(void *p_arg);

//任务优先级
#define RECIEVEFROMSERVER_TASK_PRIO		6
//任务堆栈大小
#define RECIEVEFROMSERVER_STK_SIZE 		0x200
//任务控制块
OS_TCB RecieveFromServerTaskTCB;
//任务堆栈
CPU_STK RECIEVEFROMSERVER_TASK_STK[RECIEVEFROMSERVER_STK_SIZE];
void RecieveFromServer_task(void *p_arg);

//任务优先级
#define HOST_TASK_PRIO		7            
//任务堆栈大小
#define HOST_STK_SIZE 		0x200
//任务控制块
OS_TCB hostTaskTCB;
//任务堆栈
CPU_STK HOST_TASK_STK[HOST_STK_SIZE];
void host_task(void *p_arg);

//任务优先级
#define PREHOST_TASK_PRIO		5
//任务堆栈大小
#define PREHOST_STK_SIZE 		0x200
//任务控制块
OS_TCB prehostTaskTCB;
//任务堆栈
CPU_STK PREHOST_TASK_STK[PREHOST_STK_SIZE];
void prehost_task(void *p_arg);

//任务优先级
//#define KEY_TASK_PRIO		8
#define KEY_TASK_PRIO		2
//任务堆栈大小
#define KEY_STK_SIZE 		128
//任务控制块
OS_TCB keyTaskTCB;
//任务堆栈
CPU_STK KEY_TASK_STK[KEY_STK_SIZE];
//任务函数
void key_task(void *p_arg);
CPU_INT32U g_key=0;

//任务优先级
#define LCD_TASK_PRIO		9        
//任务堆栈大小
#define LCD_STK_SIZE		0x200
//消息队列大小
#define OS_MSG_QTY_LCDNUM		10
//任务控制块
OS_TCB	lcdTaskTCB;
//任务堆栈
__align(8) CPU_STK	LCD_TASK_STK[LCD_STK_SIZE];
//任务函数
void lcd_task(void *p_arg);

//任务优先级
#define PERIOD_TASK_PRIO		10
//任务堆栈大小
#define PERIOD_STK_SIZE 		0x200 //0x200
//任务控制块
OS_TCB PeriodTaskTCB;
//任务堆栈
CPU_STK PERIOD_TASK_STK[PERIOD_STK_SIZE];
//任务函数
void Period_task(void *p_arg);

//任务优先级
#define NET_TASK_PRIO		11
//任务堆栈大小
#define NET_STK_SIZE 		0x200
//任务控制块
OS_TCB NetTaskTCB;
//任务堆栈
CPU_STK NET_TASK_STK[NET_STK_SIZE];
//任务函数
void Net_task(void *p_arg);

OS_TMR tmrWdg;
 

static unsigned char tmrWdg_step=4;
static unsigned char tmrWdg_Flag=0;
const char lcdwdg_test[]="asl....";
//全局变量
unsigned char g_RunState=0;
	
#ifdef FB1010
const char FB1010_sv[20]="1010A_SV1.0_19本地";//本地=190416
#endif

#ifdef FB1030
const char FB1010_sv[20]="1030A_SV1.0_190327";
#endif

int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断分组配置
	bsp_init();
	
	OSInit(&err);		//初始化UCOSIII
	OS_CRITICAL_ENTER();//进入临界区
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
							(CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);  //开启UCOSIII
	while(1);
}
void SendTaskFeed(void)
{
	if((tmrWdg_Flag&(0x1<<0))!=0)
	{
		tmrWdg_Flag&=~(0x1<<0);
#ifdef WD_T		
		printf("0喂_");
#endif		
	}
}
void RecieveTaskFeed(void)
{
	if((tmrWdg_Flag&(0x1<<1))!=0)
	{
		tmrWdg_Flag&=~(0x1<<1);
#ifdef WD_T				
		printf("1喂_");
#endif			
	}
}
void HostTaskFeed(void)
{
	if((tmrWdg_Flag&(0x1<<2))!=0)
	{
		tmrWdg_Flag&=~(0x1<<2);
#ifdef WD_T				
		printf("2喂_");
#endif			
	}
}

void lcdTaskFeed(void)
{
	if((tmrWdg_Flag&(0x1<<3))!=0)
	{
		tmrWdg_Flag&=~(0x1<<3);
#ifdef WD_T		
		printf("3喂_");
#endif			
	}
}
void PeridTaskFeed(void)
{
	if((tmrWdg_Flag&(0x1<<4))!=0)
	{
		tmrWdg_Flag&=~(0x1<<4);
#ifdef WD_T		
  	printf("4喂_");
#endif			
	}
}

void PreHostTaskFeed(void)
{
	if((tmrWdg_Flag&(0x1<<5))!=0)
	{
		tmrWdg_Flag&=~(0x1<<5);
#ifdef WD_T		
		printf("5喂_");
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
		 if(tmrWdg_step==2){tmrWdg_step=3;}//跳过2；
		}
		else
		{tmrWdg_step=0;}
#ifdef WD_T		
		printf("%d开_",tmrWdg_step);
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
//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
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
				
	OS_CRITICAL_ENTER();	//进入临界区
	//创建SERVER任务
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
	//创建SERVER任务
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
				 
//	//创建HOST任务
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

	//创建PREHOST任务
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
								 
	//创建key任务
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
			 
	//创建LCD任务
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
				 
	//创建Period任务
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
				 
	//创建Net任务
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
				 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务
	OS_CRITICAL_EXIT();	//进入临界区
}
//server任务函数 
//任务执行，发送时候，就用W5500发送数据；
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
			printf("1.发送任务启动！");
#endif
			
			//喂狗；
			SendTaskFeed();
	
			//网络状态判断，网络异常则退出任务；
			if(((g_RunState&(1<<5))==0)&&(NetStatus()==0))//网络正常；
			{  
			}
			else//网络异常；
			{break;
			}
			
			//数据发送；		
			flag=SendFrameToServer();
			OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err);
			if(flag==0) break;//发送栈为空，没有数据发送，则退出发送任务；
			else 
			{
				if((g_RunState&(1<<5))==0)//有网络
				{
				 OSTimeDlyHMSM(0,0,4,0,OS_OPT_TIME_HMSM_STRICT,&err);//3S-4S
				}
				else//无网络
				{
					OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
				}
			}

			OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err);
		}
	}
}
//周期任务轮询W5500接收长度，如果长度不为0，则触发这个任务；
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
		printf("2.接收任务启动！");
#endif
		printf("2.接收任务启动！");
		RecieveTaskFeed();
		
		BT_PeriodTask();     //远程升级周期判定；
				
		OSTimeDlyHMSM(0,0,1,100,OS_OPT_TIME_HMSM_STRICT,&err);
		flag=RecieveFromServer_Handle();
		if(flag!=0)
		{
		}
	}
}

unsigned int cnt;
extern unsigned char g_HostRecFlag;
//host任务函数
//void host_task(void *p_arg)
//{
//	OS_ERR err;
//	CPU_TS ts;
//	unsigned char flag=0;
//	unsigned char i;
//	p_arg = p_arg;
//	SDdata_Init();
//	printf("系统启动！\n\r");	
////	g_RunState&=~(1<<6);				//线路正常
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
////		if(SDDataInFlagt==1){SD_DATAIN();SDDataInFlagt=0;} //执行编码表导入函数。
////		flag=ProtocolBoardHandle();
////		if(flag==4)
////		{
////			g_RunState|=(1<<6);					//线路故障
////		}
////		else if(flag==1)
////		{
////			g_RunState&=~(1<<6);				//线路正常
////		}
//	}
//}
//prehost从主机任务函数
//extern CANParameterConfiguration  gCAN1SystemParameter;
extern unsigned char ThreeInOne_ProtocolBoard_Run(void);
extern void TIM2_Int_Init(unsigned short int arr, unsigned short int psc);
#ifdef	XYB_DYJ	
const unsigned short VERS_msg=11000;//21020=版本V21+主机型号D020（远程升级，协议版本）
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
 	printf("%s！\n\r",FB1010_sv);
	
	TIM2_Int_Init(1000,7200);	//解析周期100mS；	
#ifdef	XYB_DYJ	
	CAN1_Mode_Init(CAN1,gCANSystemParameter);   //CAN模式初始化使能
#else	
	ProalyInit();	//协议解析初始化；
#endif	
	
	while(1)
	{
#ifdef TK_T		
//		printf("3.解析任务启动！");
#endif
 		PreHostTaskFeed(); //任务喂狗函数；

#ifdef	XYB_DYJ	
		if(DataSendToStatus()==1) //如果有火警存储空间，则获取协议板报警信息； 故障空间！//接收数据长度，最大512个字节；
		{  
 			ThreeInOne_ProtocolBoard_Run();   		//协议三合一板CAN协议执行函数
		}
#else		
    ProalyRun();//协议解析启动；
#endif	
		
		//主机线路判断；	
		if(MClineFlaut_flag==0xFFFF0000) //初始状态；
		{ 
		}
		else if (MClineFlaut_flag==0)
		{
				g_RunState&=~(1<<6);				//线路正常，绿灯亮
		}
    else //任意位置1；
		{
        g_RunState|=(1<<6);					//线路故障，黄灯亮
		}
		
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err);	
		pre_cont++;
		if(pre_cont>30)//解析任务执行300mS，预留100mS给其他任务；
		{  pre_cont=0;
		   OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);			
		}
		
	}
}


//key任务函数
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
				printf("4.按键任务启动！");
#endif
				key=KEY_SCAN();
				if(key!=0)			//按键按下
				{
//	  			printf("K=%x\r\n",key);
					Key0_count=0;
					if((key&0x80000000)==0x80000000)		//为有效按键？
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
				
				OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //延时10ms
				
				if(Key0_count>50)
				{
					Key0_count=0;
					OSTaskSuspend(&keyTaskTCB,&err);
				}
 		
	}
}
//LCD测试任务
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
		printf("5.指示灯任务启动！");
#endif		

		if(ptr==&g_key)
		{
			key_Explain(*((unsigned int *)ptr));//按键解释；
//			refresh_lcd();
		}
		else if((ptr==&g_self_check_step)&&(*(unsigned char *)ptr!=0))//自检
		{
 			lcd_SelfCheck(g_RunState);
		}
	  else if(ptr==&g_RunState)
		{
			RunState_Action(g_RunState);//LED灯的控制；
		}
		else if(ptr==&lcdwdg_test)
		{
			lcdTaskFeed();//喂狗；
		}
		else
		{
			pServerData=NULL;
//			pServer_Rd_Addr=NULL;
			pServerData=IsReportAddr((unsigned char *)ptr,size,pServer_Rd_Addr);		
			if(pServerData!=NULL)
			{
				flag=Server_Action(pServerData,pServer_Rd_Addr);//数据解释，LED控制；喇叭控制；
//				if(flag>=1)refresh_lcd();//zhaoning是否刷屏幕。
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

//每秒钟执行一次；
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
		printf("6.周期任务启动！");
#endif
		Period_Cnt++;
		PeridTaskFeed();
		
		ATcommond_Logic();//AT命令解析；
//    if(Period_Cnt%100==0)//1s计算一次 打印CPU利用率；		
//		{ printf("~CP=%d！\n\r",OSStatTaskCPUUsage);		}
		
    if(Period_Cnt%60==0)//60s=1min  打印系统时间；
    {
		   Time_System_Get();
	  }
		
		if(Period_Cnt%1==0)//每秒检测 共70次=70S	
		{
			state=UpdataRunState(); //内部判断5500是否接收到数据，网络连接情况；
			g_RunState&=~0x38;
			g_RunState|=state;
			state=g_RunState;
			RunState_Handle(state);
		}
		
//		if(Period_Cnt%NetHeart_Cont==0)//6000=60s=1min,心跳必须每分钟1次，否则W5500判断网络故障的用时大于100S。
		if(NetHeart_SndCot>=NetHeart_Cont)//100mS计时；		
		{
			NetHeart_SndCot=0;
			
			if((g_RunState&(1<<5))==0)//网络正常；
			{
			  HeartBeatFrame();//上发心跳包
			}	
			ResumeNbr=0;//NB注册次数清零；
		}
		
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s

	}
}

//网络重启
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
									//CAN1中断函数
*****************************************************************************************************************/
//void CAN1_RX0_IRQHandler(void)
//{ 
//   YH_CAN1_RX0_IRQHandler();	
//}
