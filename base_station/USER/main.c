#include "led.h"
#include "delay.h"
#include "sys.h"
#include "timer.h"
#include "key.h"
#include "usart.h"
#include "usart2.h"
#include "usart3.h"
#include "uart4.h"
#include "stmflash.h"
#include "buzzer.h"
#include "function.h"
#include "gprs.h"
#include "bluetooh.h"
//#include "watchdog.h"
#include "iwdg.h"
#include "rtc.h"
#include "bc95.h"
#include "os.h"
#include "memory.h"
//-------------------------------------------------------------//



//UCOSIII中以下优先级用户程序不能使用
//将这些优先级分配给了UCOSIII的5个系统内部任务
//优先级0：中断服务服务管理任务 OS_IntQTask()
//优先级1：时钟节拍任务 OS_TickTask()
//优先级2：定时任务 OS_TmrTask()
//优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
//优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()


//任务优先级
#define DETECT_TASK_PRIO		5
//任务堆栈大小	
#define DETECT_STK_SIZE 		384
//任务控制块
OS_TCB DETECTTaskTCB;
//任务堆栈	
CPU_STK DETECT_TASK_STK[DETECT_STK_SIZE];
//任务函数
void DETECT_task(void *p_arg);

//任务优先级
#define GPRSCONNECT_TASK_PRIO		8
//任务堆栈大小	
#define GPRSCONNECT_STK_SIZE 		512
//任务控制块
OS_TCB GPRSCONNECTTaskTCB;
//任务堆栈	
CPU_STK GPRSCONNECT_TASK_STK[GPRSCONNECT_STK_SIZE];
void GPRSCONNECT_task(void *p_arg);

//任务优先级
#define GPRSSTANDBY_TASK_PRIO		6
//任务堆栈大小
#define GPRSSTANDBY_STK_SIZE		512
//任务控制块
OS_TCB	GPRSSTANDBYTaskTCB;
//任务堆栈
CPU_STK GPRSSTANDBY_TASK_STK[GPRSSTANDBY_STK_SIZE];
//任务函数
void GPRSSTANDBY_task(void *p_arg);
u8 Receive_buf[1024];

u32 NB_LINKSTATUS=0;
extern u32 datastatus;

void flag_int(void)//变量初始化。
{
	unsigned char i=0;
	unsigned char memory_index[100]={0};//读取数据索引。0~5设备ID,6~7回路号，8~9部件号，10~11存储的节点数量。	
	STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,50); // 读取已注册设备数据	
//设置心跳周期大小
	bluetooh_scan_heart_time=20;//单片机搜索蓝牙标签时间重装初值，5秒（可以设置搜索时间间隔）。
	bluetooh_scan_heart_time=memory_index[60];//单片机搜索蓝牙标签时间重装初值，5秒（可以设置搜索时间间隔）。 	
	Bluetooh_scan_time=bluetooh_scan_heart_time;
	heartbeat_time=(memory_index[40]+memory_index[41]*256+memory_index[42]*256*256+memory_index[43]*256*256*256)*60;//心跳时间定义1分钟心跳一次。（1s进入定时器中断一次）
	Bluetooh_heartbeat_time=heartbeat_time;//重装蓝牙基站的心跳时间。
	timeout=50;//给服务器发送一帧数据的超时时间。如果超过这个时间，则认为发送时间太长。(单位为秒)
	SEND_DATA_to_server_time=timeout;//定时器中断是5ms进入一次。进入2000次，则为10秒钟。
	judge_fruit_time=2;//2秒检测一次对比是否离位。
	Bluetooh_Package_compare=judge_fruit_time;//重装初值。防止每次都进入做对比。浪费单片机资源	
	departure_time=(memory_index[50]+memory_index[51]*256+memory_index[52]*256*256+memory_index[53]*256*256*256)*60;//宏定义离位时间。memory_index[50]是分钟，乘以60就是秒。
	for(i=0;i<Bluetooh_amount;i++)//开机初始化重装蓝牙标签离位时间初始值。
	{
		Bluetooh_time[i]=0;
	}
	SEND_DATA_to_server_ok=0;//往服务器发送成功为0，发送失败为1.
	SEND_once=0;//重发次数等于0。.
	finger_point=0;//初始为0	
	SEND_DATA_to_server_GROUP=0;//初始为0	
	serial_number_counter=0;//流水号。
	last_serial_number=0;//流水号。
	
	mark_status=0x0000;//离位标签的标志位。按位来算。当为0的时候，表示离位。例如 mark_tatus 当前的值为：0100 0011  则说明8，6，5，4，3号标签发生了离位。
	mark_status_last=0x0000;//上次离位标签的标志位。	
	GM3_LINKA_TIME_LOSE=360;//，一秒一次，360秒就是6分钟。GPRS掉线时间。如果超过这个时间，则需要重启GPRS模块了。硬件检测LINK引脚，因为需要修改测试，所以做成全局变量。相当于宏定义。
	GM3_LINKA_TIME=GM3_LINKA_TIME_LOSE;//GPRS在线时间填充满。GPRS掉线时间。如果超过这个时间，则需要重启GPRS模块了。	
}
 int main(void)
 {
    OS_ERR err;
	CPU_SR_ALLOC();
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	//delay_init();	//延时函数初始化	
    //Watchdog_Init();//看门狗模块IO初始化	
    //DOG_EN=1;//使能看门狗     
 	//TIM2_Int_Init(499,719);		//5000微秒（5ms）进入一次。500hz。	
	//USART1_Init(115200);//配置
	//USART2_Init(9600);//GPRS	
	//USART3_Init(9600);
	UART4_Init(460800);
test_URAT4();
	printf("蓝牙基站测试\r\n");
	//Buzzer_Init();//Buzzer初始化	 
  ENABLE_IO_Init();//模块使能引脚初始化	
//	LoRa_EN=0; 	// 使能LoRa模块。低电平唤醒。高电平休眠。
	//M433_EN=0; 	// 使能433M模块。低电平唤醒。高电平休眠。
	//KEY_Init(); //按键初始化。	  
	//Bluetooh_Init();//蓝牙模块IO初始化
	//Bluetooh_Reset();//Bluetooh复位。
	
//	IWDG_Init(4,625);    //与分频数为64,重载值为625,溢出时间为1s。独立看门狗。		 
	printf("正在初始化GPRS模块，请稍候。。。\r\n");	 
	Buzzer_flag=4;
	GM3_LINKA_TIME_LOSE=360;//GPRS掉线时间。如果超过这个时间，则需要重启GPRS模块了。硬件检测LINK引脚，因为需要修改测试，所以做成全局变量。相当于宏定义。	 
	Star_Read_ID();//读取并打印设备ID	
	GPRS_Init();//GM3模块IO初始化
	//printf("GPRS模块初始化完毕。\r\n");		 
    GPRS_IO_Init();
	flag_int();//变量初始化。
//	dislocation_test();
	LED_Init();		//初始化与LED连接的硬件接口
//	Watchdog_ENABLE;//使能看门狗芯片。
	RTC_Init();//RTC初始化	
//	Watchdog_DISABLE;//关闭看门狗芯片。	
LED_flag=0;
//gprs_link();
DOG_EN=0;//使能看门狗

    OSInit(&err);
	CPU_Init();
	OS_CRITICAL_ENTER();	//进入临界区
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
    //创建一个存储分区
	Creat_Memory();
    //创建消息队列DATA_Msg
	Create_Msg();	
	OSTaskCreate((OS_TCB 	* )&DETECTTaskTCB,		
				 (CPU_CHAR	* )"DETECT task", 		
                 (OS_TASK_PTR )DETECT_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )DETECT_TASK_PRIO,     
                 (CPU_STK   * )&DETECT_TASK_STK[0],	
                 (CPU_STK_SIZE)DETECT_STK_SIZE/10,	
                 (CPU_STK_SIZE)DETECT_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
    //创建GPRS连接任务
	OSTaskCreate((OS_TCB 	* )&GPRSCONNECTTaskTCB,		
				 (CPU_CHAR	* )"GPRSCONNECT task", 		
                 (OS_TASK_PTR )GPRSCONNECT_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )GPRSCONNECT_TASK_PRIO,     	
                 (CPU_STK   * )&GPRSCONNECT_TASK_STK[0],	
                 (CPU_STK_SIZE)GPRSCONNECT_STK_SIZE/10,	
                 (CPU_STK_SIZE)GPRSCONNECT_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	
			 
	//创建GPRS监测任务
	OSTaskCreate((OS_TCB 	* )&GPRSSTANDBYTaskTCB,		
				 (CPU_CHAR	* )"GPRSSTANDBY test task", 		
                 (OS_TASK_PTR )GPRSSTANDBY_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )GPRSSTANDBY_TASK_PRIO,     	
                 (CPU_STK   * )&GPRSSTANDBY_TASK_STK[0],	
                 (CPU_STK_SIZE)GPRSSTANDBY_STK_SIZE/10,	
                 (CPU_STK_SIZE)GPRSSTANDBY_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);
    OS_CRITICAL_EXIT();	//退出临界区	
    OSStart(&err);  //开启UCOSIII

	while(1);

  /*while(1)
	{
		Watchdog_FEED();//喂狗

		USART1_COMAND_SCAN();//配置串口，用于配置基站ID或添加删除节点。
//		Feed_Dog();//喂狗		
		USART2_COMAND_SCAN();//连接的GM3模块/M26模块/NB-IOT模块，用于与服务器交互数据。
//		Feed_Dog();//喂狗		
		USART3_COMAND_SCAN();//连接的433/LoRa模块。
//		Feed_Dog();//喂狗		
		UART4_COMAND_SCAN(); //连接的蓝牙模块，用于接收蓝牙节点的数据。
		Watchdog_FEED();//喂狗			
//		Feed_Dog();//喂狗
		Judge_GM3_send_data();//判断基站是否需要通过GM3/M26/NB-IOT上传给服务器数据。
//		Feed_Dog();//喂狗		
		Bluetooh_SCAN();//刷新巡检蓝牙串口数据。
//		Feed_Dog();//喂狗		
		Judge_Erase_all_ID();	//判断网关是否需要擦除所有ID。
	}*/
 }
void DETECT_task(void *p_arg)
{
//    u32 timestamp;
    //OS_ERR err;
    //u16 data_size;
    //u32 data_size_all;
    //CPU_SR_ALLOC();
    while(1)
	{
		Watchdog_FEED();//喂狗

		USART1_COMAND_SCAN();//配置串口，用于配置基站ID或添加删除节点。
//		Feed_Dog();//喂狗		
		USART2_COMAND_SCAN();//连接的GM3模块/M26模块/NB-IOT模块，用于与服务器交互数据。
//		Feed_Dog();//喂狗		
		USART3_COMAND_SCAN();//连接的433/LoRa模块。
//		Feed_Dog();//喂狗		
		UART4_COMAND_SCAN(); //连接的蓝牙模块，用于接收蓝牙节点的数据。
		Watchdog_FEED();//喂狗			
//		Feed_Dog();//喂狗
		Judge_GM3_send_data();//判断基站是否需要通过GM3/M26/NB-IOT上传给服务器数据。
//		Feed_Dog();//喂狗		
		Bluetooh_SCAN();//刷新巡检蓝牙串口数据。
//		Feed_Dog();//喂狗		
		Judge_Erase_all_ID();	//判断网关是否需要擦除所有ID。
        delay_ms(10);
	}
}
void GPRSCONNECT_task(void *p_arg)
{
    static u32 last_heartbeatflag;
    static u32 last_check_flag;
    static u32 open_flag;
    //u8 data[]="AT+CPSMS?\r\n";
    //u8 data2[]="AT+CPSMS=1\r\n";
    OS_ERR err;
    u32 timestamp;
    OSTaskSuspend((OS_TCB*)&GPRSSTANDBYTaskTCB,&err);  
    while(1)
    {
        //Buzzer_flag=4;
        OpenGPRS_check();
        open_flag = gprs_send_start();
        if (open_flag == 0)
		{
            //ATGetMsg();
            timestamp=OSTimeGet(&err);
            printf("GPRS模块初始化完毕。\r\n");
            LED_flag=2;
            NB_LINKSTATUS=1;
            //last_heartbeatflag=timestamp;
            //GPIO_ResetBits(GPIOA,GPIO_Pin_0);
            ResetUartBuf(&uart_gprs);
			OSTaskResume((OS_TCB*)&GPRSSTANDBYTaskTCB,&err);
            OSTaskSuspend((OS_TCB*)&GPRSCONNECTTaskTCB,&err);
            printf("GPRS模块长时间未能发送成功数据，重启。\r\n");
            LED_flag=0;
            Buzzer_flag=4;
            NB_LINKSTATUS=0;
            //GPIO_SetBits(GPIOA,GPIO_Pin_0);
            reset_gprs(); 
		}
		#if MYDEBUG_A
        USART1_sendstr("*************NO  ACK  WRONG!!\r\n");
        #endif
        delay_ms(50);
    }
}
void GPRSSTANDBY_task(void *p_arg)
{
    u8 status;
    u32 timestamp;
    u32 length;
    static u32 data_size;
    OS_ERR err;
    //CPU_SR_ALLOC();
    while(1)
    { 
        status=0;
        delay_ms(50);
        timestamp=OSTimeGet(&err);
        GPRS_Detect_Handle(timestamp);
        if(data_size==0) data_size=PEND(&BT_Msg,Receive_buf,&STORAGE_MEM);
        if(data_size)
        {
            status=gprs_check_connect();
            if(status) goto GPRS_RESET;
            status=GPRS_buffer_send(Receive_buf,data_size);
            //USART1_SEND(Receive_buf,data_size);
            /*if(status) 
            {
            delay_ms(30000);
            status=GPRS_buffer_send(Receive_buf,data_size);
            }*/
            data_size=0;
        if(status) goto GPRS_RESET;
        }
         
        if(status||datastatus)
        {
            GPRS_RESET:
            status=0;
            datastatus=0;
            //SEND_DATA_to_server_ok=0;
            OSTaskResume((OS_TCB*)&GPRSCONNECTTaskTCB,&err);
            OSTaskSuspend((OS_TCB*)&GPRSSTANDBYTaskTCB,&err);
        }        
       
    }
}