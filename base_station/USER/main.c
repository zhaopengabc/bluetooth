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



//UCOSIII���������ȼ��û�������ʹ��
//����Щ���ȼ��������UCOSIII��5��ϵͳ�ڲ�����
//���ȼ�0���жϷ������������� OS_IntQTask()
//���ȼ�1��ʱ�ӽ������� OS_TickTask()
//���ȼ�2����ʱ���� OS_TmrTask()
//���ȼ�OS_CFG_PRIO_MAX-2��ͳ������ OS_StatTask()
//���ȼ�OS_CFG_PRIO_MAX-1���������� OS_IdleTask()


//�������ȼ�
#define DETECT_TASK_PRIO		5
//�����ջ��С	
#define DETECT_STK_SIZE 		384
//������ƿ�
OS_TCB DETECTTaskTCB;
//�����ջ	
CPU_STK DETECT_TASK_STK[DETECT_STK_SIZE];
//������
void DETECT_task(void *p_arg);

//�������ȼ�
#define GPRSCONNECT_TASK_PRIO		8
//�����ջ��С	
#define GPRSCONNECT_STK_SIZE 		512
//������ƿ�
OS_TCB GPRSCONNECTTaskTCB;
//�����ջ	
CPU_STK GPRSCONNECT_TASK_STK[GPRSCONNECT_STK_SIZE];
void GPRSCONNECT_task(void *p_arg);

//�������ȼ�
#define GPRSSTANDBY_TASK_PRIO		6
//�����ջ��С
#define GPRSSTANDBY_STK_SIZE		512
//������ƿ�
OS_TCB	GPRSSTANDBYTaskTCB;
//�����ջ
CPU_STK GPRSSTANDBY_TASK_STK[GPRSSTANDBY_STK_SIZE];
//������
void GPRSSTANDBY_task(void *p_arg);
u8 Receive_buf[1024];

u32 NB_LINKSTATUS=0;
extern u32 datastatus;

void flag_int(void)//������ʼ����
{
	unsigned char i=0;
	unsigned char memory_index[100]={0};//��ȡ����������0~5�豸ID,6~7��·�ţ�8~9�����ţ�10~11�洢�Ľڵ�������	
	STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,50); // ��ȡ��ע���豸����	
//�����������ڴ�С
	bluetooh_scan_heart_time=20;//��Ƭ������������ǩʱ����װ��ֵ��5�루������������ʱ��������
	bluetooh_scan_heart_time=memory_index[60];//��Ƭ������������ǩʱ����װ��ֵ��5�루������������ʱ�������� 	
	Bluetooh_scan_time=bluetooh_scan_heart_time;
	heartbeat_time=(memory_index[40]+memory_index[41]*256+memory_index[42]*256*256+memory_index[43]*256*256*256)*60;//����ʱ�䶨��1��������һ�Ρ���1s���붨ʱ���ж�һ�Σ�
	Bluetooh_heartbeat_time=heartbeat_time;//��װ������վ������ʱ�䡣
	timeout=50;//������������һ֡���ݵĳ�ʱʱ�䡣����������ʱ�䣬����Ϊ����ʱ��̫����(��λΪ��)
	SEND_DATA_to_server_time=timeout;//��ʱ���ж���5ms����һ�Ρ�����2000�Σ���Ϊ10���ӡ�
	judge_fruit_time=2;//2����һ�ζԱ��Ƿ���λ��
	Bluetooh_Package_compare=judge_fruit_time;//��װ��ֵ����ֹÿ�ζ��������Աȡ��˷ѵ�Ƭ����Դ	
	departure_time=(memory_index[50]+memory_index[51]*256+memory_index[52]*256*256+memory_index[53]*256*256*256)*60;//�궨����λʱ�䡣memory_index[50]�Ƿ��ӣ�����60�����롣
	for(i=0;i<Bluetooh_amount;i++)//������ʼ����װ������ǩ��λʱ���ʼֵ��
	{
		Bluetooh_time[i]=0;
	}
	SEND_DATA_to_server_ok=0;//�����������ͳɹ�Ϊ0������ʧ��Ϊ1.
	SEND_once=0;//�ط���������0��.
	finger_point=0;//��ʼΪ0	
	SEND_DATA_to_server_GROUP=0;//��ʼΪ0	
	serial_number_counter=0;//��ˮ�š�
	last_serial_number=0;//��ˮ�š�
	
	mark_status=0x0000;//��λ��ǩ�ı�־λ����λ���㡣��Ϊ0��ʱ�򣬱�ʾ��λ������ mark_tatus ��ǰ��ֵΪ��0100 0011  ��˵��8��6��5��4��3�ű�ǩ��������λ��
	mark_status_last=0x0000;//�ϴ���λ��ǩ�ı�־λ��	
	GM3_LINKA_TIME_LOSE=360;//��һ��һ�Σ�360�����6���ӡ�GPRS����ʱ�䡣����������ʱ�䣬����Ҫ����GPRSģ���ˡ�Ӳ�����LINK���ţ���Ϊ��Ҫ�޸Ĳ��ԣ���������ȫ�ֱ������൱�ں궨�塣
	GM3_LINKA_TIME=GM3_LINKA_TIME_LOSE;//GPRS����ʱ���������GPRS����ʱ�䡣����������ʱ�䣬����Ҫ����GPRSģ���ˡ�	
}
 int main(void)
 {
    OS_ERR err;
	CPU_SR_ALLOC();
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	//delay_init();	//��ʱ������ʼ��	
    //Watchdog_Init();//���Ź�ģ��IO��ʼ��	
    //DOG_EN=1;//ʹ�ܿ��Ź�     
 	//TIM2_Int_Init(499,719);		//5000΢�루5ms������һ�Ρ�500hz��	
	//USART1_Init(115200);//����
	//USART2_Init(9600);//GPRS	
	//USART3_Init(9600);
	UART4_Init(460800);
test_URAT4();
	printf("������վ����\r\n");
	//Buzzer_Init();//Buzzer��ʼ��	 
  ENABLE_IO_Init();//ģ��ʹ�����ų�ʼ��	
//	LoRa_EN=0; 	// ʹ��LoRaģ�顣�͵�ƽ���ѡ��ߵ�ƽ���ߡ�
	//M433_EN=0; 	// ʹ��433Mģ�顣�͵�ƽ���ѡ��ߵ�ƽ���ߡ�
	//KEY_Init(); //������ʼ����	  
	//Bluetooh_Init();//����ģ��IO��ʼ��
	//Bluetooh_Reset();//Bluetooh��λ��
	
//	IWDG_Init(4,625);    //���Ƶ��Ϊ64,����ֵΪ625,���ʱ��Ϊ1s���������Ź���		 
	printf("���ڳ�ʼ��GPRSģ�飬���Ժ򡣡���\r\n");	 
	Buzzer_flag=4;
	GM3_LINKA_TIME_LOSE=360;//GPRS����ʱ�䡣����������ʱ�䣬����Ҫ����GPRSģ���ˡ�Ӳ�����LINK���ţ���Ϊ��Ҫ�޸Ĳ��ԣ���������ȫ�ֱ������൱�ں궨�塣	 
	Star_Read_ID();//��ȡ����ӡ�豸ID	
	GPRS_Init();//GM3ģ��IO��ʼ��
	//printf("GPRSģ���ʼ����ϡ�\r\n");		 
    GPRS_IO_Init();
	flag_int();//������ʼ����
//	dislocation_test();
	LED_Init();		//��ʼ����LED���ӵ�Ӳ���ӿ�
//	Watchdog_ENABLE;//ʹ�ܿ��Ź�оƬ��
	RTC_Init();//RTC��ʼ��	
//	Watchdog_DISABLE;//�رտ��Ź�оƬ��	
LED_flag=0;
//gprs_link();
DOG_EN=0;//ʹ�ܿ��Ź�

    OSInit(&err);
	CPU_Init();
	OS_CRITICAL_ENTER();	//�����ٽ���
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
    //����һ���洢����
	Creat_Memory();
    //������Ϣ����DATA_Msg
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
    //����GPRS��������
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
			 
	//����GPRS�������
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
    OS_CRITICAL_EXIT();	//�˳��ٽ���	
    OSStart(&err);  //����UCOSIII

	while(1);

  /*while(1)
	{
		Watchdog_FEED();//ι��

		USART1_COMAND_SCAN();//���ô��ڣ��������û�վID�����ɾ���ڵ㡣
//		Feed_Dog();//ι��		
		USART2_COMAND_SCAN();//���ӵ�GM3ģ��/M26ģ��/NB-IOTģ�飬������������������ݡ�
//		Feed_Dog();//ι��		
		USART3_COMAND_SCAN();//���ӵ�433/LoRaģ�顣
//		Feed_Dog();//ι��		
		UART4_COMAND_SCAN(); //���ӵ�����ģ�飬���ڽ��������ڵ�����ݡ�
		Watchdog_FEED();//ι��			
//		Feed_Dog();//ι��
		Judge_GM3_send_data();//�жϻ�վ�Ƿ���Ҫͨ��GM3/M26/NB-IOT�ϴ������������ݡ�
//		Feed_Dog();//ι��		
		Bluetooh_SCAN();//ˢ��Ѳ�������������ݡ�
//		Feed_Dog();//ι��		
		Judge_Erase_all_ID();	//�ж������Ƿ���Ҫ��������ID��
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
		Watchdog_FEED();//ι��

		USART1_COMAND_SCAN();//���ô��ڣ��������û�վID�����ɾ���ڵ㡣
//		Feed_Dog();//ι��		
		USART2_COMAND_SCAN();//���ӵ�GM3ģ��/M26ģ��/NB-IOTģ�飬������������������ݡ�
//		Feed_Dog();//ι��		
		USART3_COMAND_SCAN();//���ӵ�433/LoRaģ�顣
//		Feed_Dog();//ι��		
		UART4_COMAND_SCAN(); //���ӵ�����ģ�飬���ڽ��������ڵ�����ݡ�
		Watchdog_FEED();//ι��			
//		Feed_Dog();//ι��
		Judge_GM3_send_data();//�жϻ�վ�Ƿ���Ҫͨ��GM3/M26/NB-IOT�ϴ������������ݡ�
//		Feed_Dog();//ι��		
		Bluetooh_SCAN();//ˢ��Ѳ�������������ݡ�
//		Feed_Dog();//ι��		
		Judge_Erase_all_ID();	//�ж������Ƿ���Ҫ��������ID��
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
            printf("GPRSģ���ʼ����ϡ�\r\n");
            LED_flag=2;
            NB_LINKSTATUS=1;
            //last_heartbeatflag=timestamp;
            //GPIO_ResetBits(GPIOA,GPIO_Pin_0);
            ResetUartBuf(&uart_gprs);
			OSTaskResume((OS_TCB*)&GPRSSTANDBYTaskTCB,&err);
            OSTaskSuspend((OS_TCB*)&GPRSCONNECTTaskTCB,&err);
            printf("GPRSģ�鳤ʱ��δ�ܷ��ͳɹ����ݣ�������\r\n");
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