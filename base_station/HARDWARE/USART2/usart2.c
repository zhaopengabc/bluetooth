#include "usart.h"
#include "usart2.h"
#include "usart3.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	   
#include "buzzer.h" 
#include "stmflash.h"
#include "timer.h"
#include "function.h"
#include "gprs.h"//检测GPRS连接引脚定义
#include "led.h"
#include "rtc.h"
#include "bc95.h"
#include "os.h"
#include "memory.h"
#include "uart4.h"
//#include "led.h"
//unsigned int send_lenth;//数组中元素的个数。

unsigned char GM3_LINKA=0;
extern AT_link_ENUM NB_Link_step;
extern u32 NB_LINKSTATUS;
u32 datastatus=0;

unsigned char data[500]={0x00};	
unsigned char send_message[100]={0x00};//发送数据数组。
//设置心跳周期大小
unsigned int heartbeat_time=0;//心跳时间定义1分钟心跳一次。
unsigned int timeout=0;//定时器中断是5ms进入一次。进入2000次，则为10秒钟。给服务器发送一帧数据的超时时间。如果超过这个时间，则认为发送时间太长。

unsigned int SEND_DATA_to_server_time=0;//往服务器发送数据倒计时。如果超过这个时间没有发送成功，则重新发送。
unsigned char SEND_DATA_to_server_ok=0;//发送成功为0，发送失败为1.
unsigned char finger_point=0;//数组消息循环中指针指向的位置。
unsigned char SEND_DATA_to_server_GROUP=0;//网关需要给服务器发送数组的组编号到达在什么位置。
unsigned char send_data[20][80]={0x00};//定义20组数据，用于重发时堆栈用。如果是蓝牙离位基站上传最多8个标签，则一包最多字节为400字节。做好预留，则410个字节。
unsigned char SEND_once=0;//判断重发次数。
unsigned short 	last_serial_number=0;//上一次的业务流水号
unsigned short  serial_number_counter=0;//业务流水号

/***************************串口结合定时器接收数据***************************/
unsigned char USART2_RX_FLAG=0;         //置标志位，告诉定时器开始收数了,也说明串口正在收数据，不要做其他操作打断。
unsigned char USART2_RX_TIME=0;       	//接收字符串时的时间间隔
unsigned int 	USART2_RX_NO=0;          	//接收字符串字符的总数量
unsigned char USART2_GROUP=0;   				//最大接收缓存数组个数	
unsigned char USART2_RX_BUF[10][100]={0x00}; 	//接收缓冲二维数组。
unsigned char USART2_RX_BUF_middle[100]={0x00}; 	//接收缓冲二维数组。
unsigned char USART2_finger_point=0;    //数组消息循环中指针指向的位置。
/***************************************************************************/
//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void USART2_Init(u32 bound)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);// GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

 	USART_DeInit(USART2);                                //复位串口1
	 //USART2_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;            //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	     //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);               //初始化PA2
 
	//USART2_RX	  PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;            //PA.3   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入  GPIO_Mode_IN_FLOATING
	GPIO_Init(GPIOA, &GPIO_InitStructure);               //初始化PA3
	
	USART_InitStructure.USART_BaudRate = bound;                                    //一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;                            //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	               //收发模式
  
	USART_Init(USART2, &USART_InitStructure); //初始化串口	2 
	USART_Cmd(USART2, ENABLE);                    //使能串口 	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断	
	
	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器		
}   
//串口2,printf 函数
//确保一次发送数据不超过USART2_MAX_SEND_LEN字节
void u2_printf(char* fmt,...)  
{  
	u16 i,j; 
	u8 USART2_TX_BUF[200]={0x00}; 			//发送缓冲,最大USART2_MAX_SEND_LEN字节
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(USART2,USART2_TX_BUF[j]); 
	} 
}

//串口2,输出函数
void USART2_SEND(unsigned char* USART2_TX_BUF,unsigned int size) // USART2_TX_BUF代表要输出的数组。size代表输出数组的长度。 
{  
	u16 i; 
	for(i=0;i<size;i++)							//循环发送数据
	{
//	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(USART2,USART2_TX_BUF[i]); 
	} 
}
//通过判断接收连续2个字符之间的时间差不大于20ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//USART_RX_BUF_middle[0]代表数组中元素的数量。例如：0x05 0x01 0x02 0x03 0x04 0x05	 
void USART2_IRQHandler(void)
{
    u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
/******************************************************************************************************************/	
			//USART2_RX_FLAG=1;//置标志位，告诉定时器可以开始为我工作了。时间间隔判断一组数据是否收完。
			USART2_RX_TIME=5;//重新装初值。开始倒计时，用来判断是否接受完了一组数据。5次进入定时器中断。
            Res =	USART_ReceiveData(USART2);
			UartRxISR(&uart_gprs, Res);
			/*USART2_RX_BUF_middle[USART2_RX_NO+1] = USART_ReceiveData(USART2);//读取接收到的数据
            UartRxISR(&uart_gprs, USART2_RX_BUF_middle[USART2_RX_NO+1]);            
			if(USART2_RX_NO<98)
			{
				USART2_RX_NO++;//每组串口数据收到的数据个数累加.不得超过数组定义的数量。		
			}
			else
			{//按照溢出处理
				USART2_RX_NO=98;
			}
			USART2_RX_BUF_middle[0]=USART2_RX_NO;*/
/******************************************************************************************************************/			
    }
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
}
void USART2_RX_SCAN(void)//定时器定时扫描串口标志位的变化，及时的进行处理。
{
	unsigned char i=0;
	if(USART2_RX_FLAG==1)//判断是否正在接收串口数据
	{
//		USART2_RX_TIME--;//将倒计时寄存器内的数值减一。
//		if(USART2_RX_TIME<=1)//说明一串数中两个数据的间隔超过了设定的时间了，判定数据接收完毕。
//		{//当接收的字节之间的时间超过接收3~5个字节的时间的时候则认为本次接收完成，接收完成标志置1.
			USART2_RX_FLAG=0;//将串口开始计时标志位数据清零	
			USART2_RX_TIME=5;//将USART_RX_TIME的值填满，防止在减1的时候出现负值。
			//USART2_RX_NO=0;//串口接收数据长度清理。			
/*******************压栈数据包数组*********************/	
			//如果这个位置的元素数量不为0，则向前走一格。
			for(i=0;i<USART2_RX_BUF_middle[0]+1;i++)//压栈数据
			{
				USART2_RX_BUF[USART2_GROUP][i]=USART2_RX_BUF_middle[i];
			}
			if(USART2_GROUP>=9)//群组个数不得超过定义的数组的长度。循环填充。0~19~0~19
			{
				USART2_GROUP=0;//群组数量
			}
			else
			{//按照溢出处理
				USART2_GROUP++;//群组的个数加1
			}
            if(USART2_RX_BUF[USART2_GROUP][0]>0)//没来得及处理的数据已经形成一个环了，需要将最开始的一帧数据清掉了。
			{
				USART2_finger_point=USART2_GROUP;//将指向指针的位置与 USART2_finger_point 一样一样。（不管此时finger_point的位置！）
			}
//		}
	}		
}
void USART2_COMAND_SCAN(void)//刷新读取串口1数据。查看是否有命令过来需要处理。 
{
	unsigned int i=0;
	unsigned int lenth=0;//串口收完一组数据的长度。
	unsigned char USART2_RXD[101]={0x00};//定义发送数组			
	if( ( (USART2_RX_BUF[USART2_finger_point][0]!=0x00)&&(USART2_RX_FLAG==0) )==1 )//如果发现串口收数数组中有数据了，并且串口正闲置。
	{
		//每次都是将第一组的数据提取出来。遵循先进先出原则。FIFO。
		for(i=0;i<USART2_RX_BUF[USART2_finger_point][0];i++)//USART2_RX_BUF[USART2_GROUP][0]是USART2_RX_BUF[j][i]中数据的长度。取出，使用。如果有7个数，则USART2_RX_BUF[USART1_GROUP][0]=7.
		{
			USART2_RXD[i]=USART2_RX_BUF[USART2_finger_point][i+1];//	将即将要处理的数据取出来。	
			USART2_RXD[i+1]=0x00;//添加结束符
		}
		lenth=USART2_RX_BUF[USART2_finger_point][0];//这个数组中元素的个数。
		USART2_RX_BUF[USART2_finger_point][0]=0x00;//清空这个数组中元素的个数。
		if(USART2_finger_point>=9)//群组个数不得超过定义的数组的长度。循环填充。0~9~0~9
		{
			USART2_finger_point=0;//群组的指针变到头位置。
		}
		else
		{
			USART2_finger_point++;//群组的指针加1。
		}
//USART2_SEND(USART2_RXD,lenth);
//			u2_printf("%s",USART2_RXD);//调试用，将刚才收到的数据打印出来。
/************************************************************************************/		
		USART2_Data_analyze(USART2_RXD,lenth);//串口2收数数据解析.USART2_RXD是数据数组，lenth是数据长度。		
//	USART1_SEND(USART2_RXD,lenth);//测试		
	}	
}
extern unsigned char memory_index[200];
void USART2_Data_analyze(unsigned char *USART2_RXD,unsigned char lenth)//串口2收数数据解析.USART2_RXD是数据数组，lenth是数据长度。
{
	unsigned int i=0,j=0;	
	unsigned char sum_check=0;//校验和。	
	unsigned short 	liushui_NO=0;//计算业务流水号用。
	unsigned char NODE_ID_u8[20]={0x00};//设备ID号码。
	unsigned int  NODE_ID_u16[20]={0x00};
	unsigned char printf_data[50]={0x00};//测试打印用的数组
    u32 time_save;
//对服务器下发的数据进行数据分析。如果收到了确认帧，将SEND_DATA_to_server_ok置1.
	if(  (USART2_RXD[0]==0x40)&&(USART2_RXD[1]==0x40)&&(USART2_RXD[lenth-2]==0x23)&&(USART2_RXD[lenth-1]==0x23) )//如果包头包尾正确。
	{//判断服务器下发的数据是否合法正确。
		//计算校验和，并比较校验和是否正确。服务器~网关。
		sum_check=sumcheck(USART2_RXD,2,lenth-4);//计算校验和。第二个字节到第倒数第二个字节相加
		if(sum_check==USART2_RXD[lenth-3])//如果服务器返回的数据包中校验和和业务流水号都正确。
		{
			RTC_Get();//查看是否需要同步时间。			
			if( (USART2_RXD[7]!=calendar.min)||(USART2_RXD[8]!=calendar.hour)||(USART2_RXD[9]!=calendar.w_date)||(USART2_RXD[10]!=calendar.w_month)||(USART2_RXD[11]!=calendar.w_year) )
			{//只查看年月日时分即可，如果不一致就校准。暂不将秒纳入比较范围。
				RTC_Set(USART2_RXD[11]+2000, USART2_RXD[10], USART2_RXD[9], USART2_RXD[8], USART2_RXD[7], USART2_RXD[6]);//设置RTC时间为2011年11月11日11时11分11秒	
//				RTC_Set(2011, 11, 11, 11, 11, 11);//设置RTC时间为2011年11月11日11时11分11秒	
			}
			switch(USART2_RXD[26])//判断数据命令类型
			{   
			 case 0x01://时间同步					 
		//网关更新本地时间。
					break;	 
			 case 0x02://发送数据
															
					break;	
			 case 0x03://确认		
		//提取业务流水号			
					liushui_NO=USART2_RXD[3];//流水号的高位
					liushui_NO=liushui_NO<<8;
					liushui_NO=liushui_NO+USART2_RXD[2];//流水号的低位		 
					if(liushui_NO>=last_serial_number)//如果是回复的刚才网关主动上发的回复帧。
					{
						printf("收到服务器应答。业务流水号：%d\r\n",liushui_NO);				
						Buzzer_flag=1;
						SEND_DATA_to_server_time=0;
						SEND_DATA_to_server_ok=0;
						SEND_once=0;//发送次数清零。	
		//				send_lenth=0;			
					}
					break;	 
			 case 0x04://请求
                 if(USART2_RXD[27]==62)
                 {
                     printf("收到服务器查询指令。业务流水号：%d\r\n",liushui_NO);	
                     printf("即将向服务器发送所有标签离位在位状态\r\n");	
                     get_all_status();
                     Buzzer_flag=1;
                 }
					break;	
			 case 0x05://应答
		//提取业务流水号
					liushui_NO=USART2_RXD[3];//流水号的高位
					liushui_NO=liushui_NO<<8;
					liushui_NO=liushui_NO+USART2_RXD[2];//流水号的低位
					if(liushui_NO>=last_serial_number)//如果是回复的刚才网关主动上发的回复帧。
					{
						SEND_DATA_to_server_time=0;
						SEND_DATA_to_server_ok=0;
						SEND_once=0;//发送次数清零。	
		//				send_lenth=0;
					}
					break;
			 case 0x06://否认

					break;
			 case 0x80://设置
		//		解析应用数据单元中的内容
					if(USART2_RXD[27]==0x93)//如果是添加节点
					{
						if( (USART2_RXD[28]==0)&&(USART2_RXD[29]==0)&&(USART2_RXD[30]==0)&&(USART2_RXD[31]==0)&&(USART2_RXD[32]==0)&&(USART2_RXD[33]==0) )
						{//如果发现目标ID都是0.则认为是设置网关ID。
							//提取设备ID。
							for(i=0;i<6;i++)
							{
								NODE_ID_u8[i]=USART2_RXD[35+i];//提取要设置的网关的ID。
							}
		//修改网关ID
							i=Save_ROUTER_ID(NODE_ID_u8);//保存网关ID.如果保存成功。返回1。如果保存失败，则返回0。
							if(i==1)//如果网关ID写入成功。
							{
								handle_arrays_u8(NODE_ID_u8,8,printf_data);//比如将0x25分开就是0x32和0x35.。0xAB分开就是0x40,0x41. 
								printf("网关ID写入成功。网关ID： ");			
								USART1_SEND(printf_data,12);//打印网关ID
								printf("\r\n");	
//								Buzzer_flag=2;							
							}
							else
							{
								printf("网关ID写入失败，请重新写入。\r\n");			
//								Buzzer_flag=2;										
							}	
						}
						else//添加网关节点ID
						{
							if(USART2_RXD[34]==1)//添加一个节点的ID
							{
								for(i=0;i<6;i++)
								{
									NODE_ID_u8[i]=USART2_RXD[35+i];//提取要设置的网关节点的ID。
								}	
								i=Save_NODE_ID(NODE_ID_u8);//NODE_ID_u16的长度为6位，保存的时候保存10位。后面四位备用。
								if(i==0xFFFF)//保存设备ID失败
								{
									printf("保存设备ID失败");
								}
								else//保存设备ID成功。
								{//打印设备注册成功
//									Buzzer_flag=4;//蜂鸣器长响一下。
									printf("网关中已注册设备ID个数为：%d\r\n",i);
									Printf_device_type(NODE_ID_u8[0]);//测试打印设备类型。根据国标。						
									printf("注册成功。设备ID：");
									handle_arrays_u8(NODE_ID_u8,6,printf_data);//ID打印出来是以0x3x 开头的。所以需要打印20位。							
									USART1_SEND(printf_data,12);	
									printf("\r\n");									
								}								
							}
							else//如果添加多个节点ID
							{
								
							}
						}
					}
					else if(USART2_RXD[27]==0x94)//如果是删除节点
					{
						if(USART2_RXD[34]==1)//删除一个节点的ID
						{
							for(i=0;i<6;i++)//提取出要删除的设备ID。
							{
								NODE_ID_u8[i]=USART2_RXD[35+i];//提取要删除的网关节点的ID。
							}
		//					DATA_change_u8_to_u16(NODE_ID_u8,6,NODE_ID_u16);//将两个8位数据转换成一个16位数据。比如将：0x25,0x5F变成0x255F
							if(NODE_ID_u16[4]==0x0000)//如果节点没有被注册
							{
								printf("网关中无此ID，不需要删除。\r\n");					
							}
							else//设备已经被注册。则查看是巡检信息还是报警信息。
							{
								printf("网关中无此ID，不需要删除。\r\n");		
		///************删ID用************/							
								Delete_NODE_ID(NODE_ID_u8);//删除ID。
								handle_arrays_u8(NODE_ID_u8,6,printf_data);//测试用。
								printf("网关中设备ID:");	
								USART1_SEND(printf_data,12);
								printf(" 删除成功。\r\n");	
		///*****************************/								
							}
						}
						else//删除多个节点的ID
						{
							
						}		
					}
                    else if(USART2_RXD[27]==0x95)//如果是配置参数
                    {
                        if(USART2_RXD[36]==0x89)
                        {
                            time_save=USART2_RXD[39]*256+USART2_RXD[38];
                            time_save/=10;
                            STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,100); // 读取已注册设备数据
                            memory_index[50]=time_save&0x000000FF;
                            memory_index[51]=(time_save&0x0000FF00)>>8;
                            memory_index[52]=(time_save&0x00FF0000)>>16;
                            memory_index[53]=(time_save&0xFF000000)>>24;
                            STMFLASH_Write(NUMBER_ADDRESS,(u16*)memory_index,100);//将变动后的数据索引重新写入eeprom	
                            printf("判定蓝牙离位标签离位时间间隔： %d 分钟\r\n",time_save);
                        }
                    }
					break;
			 default:
					break;
			}
		}
	}	
}
void Judge_GM3_send_data(void)//判断/蓝牙基站是否需要通过GM3上传给服务器数据。	
{
	unsigned int i=0;
	unsigned char test[10]={0x00};
/*************在此处检测GPRS状态。***************/	
	/*if(TIM2_IRQ_flag==1)//GPRS未连接,且到了检测时间了。	
	{
		TIM2_IRQ_flag=0;//清空标志位。
		if(GM3_LINKA==1)//如果模块没能连接网络。
		{
			if(GM3_LINKA_TIME>1)
			{
				GM3_LINKA_TIME=GM3_LINKA_TIME-1;
			}
			else
			{
				printf("GPRS模块长时间未连接到网络，重启。\r\n");
				GPRS_RESTAR();//GPRS重启。
			}			
		}
		else//如果GPRS在线，则时间加1.
		{
			LED_flag=2;//	
			if(GM3_LINKA_TIME<GM3_LINKA_TIME_LOSE)
			{
				GM3_LINKA_TIME=GM3_LINKA_TIME+1;
			}		
		}		
	}*/

	if(Bluetooh_heartbeat_time==0)//如果到了发送心跳的时间。则发送心跳。
	{
		Bluetooh_heartbeat_time=heartbeat_time;//重装蓝牙基站的心跳时间。
		get_all_status();
	}
	if(NB_LINKSTATUS)//如果模块连接到了网络。
	{
		if(send_time==0)//2秒进入一次，防止频繁进入。
		{
			send_time=2;//重装初值
			if( (send_data[finger_point][0]!=0x00)&&(SEND_DATA_to_server_ok==0) )//遵循先进先出原则。FIFO。消息队列数组中有数据，且上一组数据已经发送成功。			
			{//如果这组数据的元素数量不为0.说明数组中有数据。发送时间已经清零（没有数组发送失败），数据发送已经成功，发送次数已经为0说明目前队列空闲。
	//取出数组的的长度。
	//			send_lenth=lenth;
				for(i=0;i<send_data[finger_point][0]+1;i++)//取出数据。
				{
					send_message[i]=send_data[finger_point][i];//将即将要处理的数据取出来。
				}
				send_data[finger_point][0]=0x00;//清空这个位置的数据长度。在后续环形堆栈的时候就表明这帧数据已经被处理了。可以继续填充新的数据了。			
				if(finger_point>=19)//正在处理的数组指针不得超过定义的数组的总个数。循环依次进行处理。0~19~0~19
				{
					finger_point=0;//群组数量
				}
				else
				{//按照溢出处理
					finger_point++;//数组指针的位置加1
				}
	//			USART1_SEND(send_message,send_message[0]+1);
				Router_send_data_to_server(send_message);//网关将数据发送给服务器。
			}
		}
		if( (SEND_DATA_to_server_ok==1)&&(SEND_DATA_to_server_time==0) )//如果正在往服务器发送数据,且发送时间超时了。
		{//对发送结果进行判定。
			SEND_once=SEND_once+1;//重发次数加1.重发次数不得超过三次.	
			if(SEND_once>=2)//发送次数超过了3.认为重发结束，继续下一组数据的发送。
			{
				SEND_once=0;//发送次数清零。
				SEND_DATA_to_server_ok=0;//强制标记发送成功标志为0.不然，永远无法进入下一组消息的发送。
				printf("GPRS模块长时间未能发送成功数据，重启。\r\n");
				GPRS_RESTAR();//GPRS重启。
                datastatus=1;
			}
			else
			{
				Router_send_data_to_server(send_message);//网关将数据发送给服务器。	
			}
		}	
	}
}
void Router_send_data_to_server(unsigned char *message)//网关发送数据给服务器
{
	unsigned char leave_no=0;//离位标签数量
	unsigned int i=0;
	unsigned int j=0;	
	unsigned int t=0;		
	unsigned char printf_data[40]={0x00};
	unsigned int step=0;//发送数据中元素的个数。
	unsigned int step_middle=0;	
	unsigned short 	last_serial_number_middle=0;//上一次的业务流水号	
	Bluetooh_heartbeat_time=heartbeat_time;//重装蓝牙基站的心跳时间。
	SEND_DATA_to_server_ok=1;//发送失败标志位置1。发送成功后清零。
	SEND_DATA_to_server_time=timeout;//定时器中断是5ms进入一次。进入2000次，则为10秒钟。
	last_serial_number_middle=message[3];//当前发送数据的流水号。0x00 0x00 ~ 0xFF 0xFF.滚动。低位在前	
	last_serial_number_middle=last_serial_number_middle<<8;
	last_serial_number=last_serial_number_middle+message[2];
/*************通过串口发送数据给服务器***************/
//通过这个函数解析后输出：
//40 40 01 00 02 00 00 00 00 00 00 00 01 02 03 04 05 06 00 00 00 00 00 00 03 00 02 15 01 01 34 23 23 	（数据） 		
// 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32	（序号）
//解析：
//40 40 （0~1）包头
//01 00 （2~3）业务流水号。（低位在前）
//00 02 （4~5）协议版本号。
//00 00 00 00 00 00 （6~11）时间标签
//01 02 03 04 05 06 （12~17）网关ID
//00 00 00 00 00 00 （18~23）目的地址
//03 00 （24~25）应用数据单元长度
//02 （26）命令字节（发送数据）
//15 （27）//类型标识 1字节上传建筑消防设施部件运行状态 上行
//01 （28）//信息对象数目,1个。
//01 （29）//运行状态为正常。
//34 （30）//校验和
//23 23（31~32）//数据包尾	
//USART1_SEND(message,message[0]+1);
// 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33
//21 06 01 00 FB 00 00 00 01 36 FB 00 00 00 01 40 FB 00 00 00 01 41 FB 00 00 00 01 44 FB 00 00 00 01 45 
/*************测试用***************/
	if(message[1]==0x01)//心跳
	{
		printf("蓝牙基站发送心跳，业务流水号：%d\r\n",last_serial_number);//测试	
	}
	if(message[1]==0x06)//如果是离位或者恢复报警。
	{
		printf("蓝牙基站发送离位/恢复，业务流水号：%d\r\n",last_serial_number);//测试	
////		离位标签ID数量为：
		leave_no=(message[0]-3)/7;
		printf("状态变动标签ID数量为：%d\r\n",leave_no);//测试
		printf("/=======================================/");
		printf("\r\n");							//测试用。		
		for(i=0;i<leave_no;i++)
		{
			for(j=0;j<6;j++)
			{
				data[j]=message[5+7*i+j];
			}
			if(message[4+7*i]==0x02)
			{
				printf("离位:");
			}
//			else if(message[4+7*i]==0x05)
			else
			{
				printf("恢复:");
			}
			handle_arrays_u8(data,6,printf_data);//测试用。
			USART1_SEND(printf_data,12);		//测试用。
			printf("\r\n");							//测试用。					
		}
		printf("/=======================================/");
		printf("\r\n");							//测试用。			
	}
/**********************************/
if(message[1]==0x01)//心跳
{
	//通过这个函数解析后输出：
	//40 40 01 00 01 20 00 00 00 00 00 00 B2 23 96 60 40 01 00 00 00 00 00 00 03 00 02 15 01 01 4A 23 23
	// 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32		
	//解析：
	//40 40 (0~1)包头
	//01 00 (2~3)业务流水号。（低位在前）
	//00 02 (4~5)协议版本号。
	//00 00 00 00 00 00 (6~11)时间标签
	//B2 23 96 60 40 01 (12~17)网关ID
	//00 00 00 00 00 00 (18~23)目的地址
	//03 00 (24~25)应用数据单元长度
	//02 (26)命令字节（发送数据）
	//15 (27)//类型标识 1字节上传建筑消防设施部件运行状态 上行
	//01 (28)//信息对象数目,1个。
	//01 (29)//运行状态为正常。
	//4A (30)//校验和
	//23 23 (31~32)//数据包尾
		// 启动符[2字节]
		data[0]=0x40;
		data[1]=0x40;
		/***控制单元*****************/
		// 业务流水号[2字节]
		data[2]=message[2];//流水号。0x00 0x00 ~ 0xFF 0xFF.滚动。低位在前	
		data[3]=message[3];				
		// 协议版本号[2字节]
		data[4]=0x01;//低位在前
		data[5]=0x20;
		// 时间标签[6字节]
		for(i=0;i<6;i++)
		{
//		package_data[2]=serial_number_counter_middle;//流水号。0x00 0x00 ~ 0xFF 0xFF.滚动。低位在前	
//		package_data[3]=serial_number_counter_middle>>8;		
//		package_data[4]=package_data[1];//秒
//		package_data[5]=package_data[2];//分
//		package_data[6]=package_data[3];//时
//		package_data[7]=package_data[4];//日			
//		package_data[8]=package_data[5];//月	
//		package_data[9]=package_data[6];//年					
			data[6+i]=message[4+i];
		}
		// 源地址[6字节] 11 ed e3 9d ee 1e
		for (i = 0; i < 6; i++)// 蓝牙离位基站的6位ID
		{
			data[12+i]=Gateway_ID[i];
		}	
		// 目的地址[6字节] 00 00 00 00 00 00
		for (i = 0; i < 6; i++)
		{
			data[18+i]=0x00;
		}
		// 应用数据单元长度[2字节]低位在前传输。
		data[24]=0x03;
		data[25]=0x00;
		// 命令字节[1字节]
		data[26]=0x02;//发送数据
		/*******应用数据单元***********/		
		data[27]=0x15;//类型标识 1字节上传建筑消防设施部件运行状态 上行
		data[28]=0x01;//信息对象数目
		//------信息体----------------//
		data[29]=0x01;//运行状态为正常。
		/********校验和***************/
		// 校验和
		data[30]=sumcheck(data,2,29);//计算校验和.	
		/********结束符***************/
		// 结束符
		data[31]=0x23;
		data[32]=0x23;
		step=33;
	//	USART1_SEND(data,33);//调试用。		
	//	printf("\r\n");	
	//	USART2_SEND(data,33);//通过串口2发送给GM3。			
	//	UART4_SEND(data,33);//调试用。		
		/********************/				
	/*******************数据压栈打包，准备上发服务器。*********************/
}
if(message[1]==0x06)//离位或恢复报警
{
	//输入的数组例如： 24 FB 00 00 00 00 14 FB 00 00 00 00 15 FB 00 00 00 00 16 FB 00 00 00 00 17  表示这四个标签离位。
	/*****************************************蓝牙基站发送离位信息给服务器***************************************************/
	
	
//40 40 3b 00 01 20 00 00 00 00 00 00 b2 23 96 60 40 0b 00 00 00 00 00 00 30 00 02 02 01 02 01 b2 00 00 00 00 05 00 fb 00 00 00 01 55 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 b2 23 23	
// 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77  
	//解析：
	//40 40 (0,1)包头
	//3b 00 (2,3)业务流水号。（低位在前）
	//01 20 (4,5)协议版本号。
	//00 00 00 00 00 00 (6,7,8,9,10,11)时间标签
	//b2 23 96 60 40 0b (12,13,14,15,16,17)网关ID
	//00 00 00 00 00 00 (18,19,20,21,22,23)目的地址
	//30 00 (24,25)应用数据单元长度
	//02 (26)命令字节
	//02(27)//类型标识 1字节上传建筑消防设施部件运行状态 上行
	//01(28)//信息对象数目,1个。
	//02(29)//系统类型标识	 1字节 火灾报警系统 01 
	//01(30)//系统地址	1字节 系统地址=01
	//B2(31)//部件类型 
	//00 00(32,33)//回路号。低位在前。 
	//00 00(34,35)//部件号。	低位在前。	 
	//05 00(36,37)//部件状态。	低位在前。
	//00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00（38~68）//补齐31个字节 
	//00 00 00 00 00 00（69~74）//补齐6位时间
	//b2(75)//校验和 
	//23 23(76，77)//数据包尾		
	

//delay_ms(20);
		// 启动符[2字节]
		data[0]=0x40;
		data[1]=0x40;
		/***控制单元*****************/ 		
		// 业务流水号[2字节]
		data[2]=message[2];//流水号。0x00 0x00 ~ 0xFF 0xFF.滚动。低位在前	
		data[3]=message[3];	
		// 协议版本号[2字节]
		data[4]=0x01;//低位在前
		data[5]=0x20;
		// 时间标签[6字节]
//		for(i=0;i<6;i++)
//		{
//			data[6+i]=0x00;
//		}

		data[6]=message[message[0]-5];//秒
		data[7]=message[message[0]-4];//分		
		data[8]=message[message[0]-3];//时			
		data[9]=message[message[0]-2];//日			
		data[10]=message[message[0]-1];//月	
		data[11]=message[message[0]-0];//年				

		// 源地址[6字节] B2 23 96 60 40 01
		for (i = 0; i < 6; i++)// 蓝牙基站的6位ID
		{
			data[12+i]=Gateway_ID[i];
		}
		// 目的地址[6字节] 00 00 00 00 00 00
		for (i = 0; i < 6; i++)
		{
			data[18+i]=0x00;
		}
		// 应用数据单元长度[2字节] 1C 00。共28个字节，换算成十六进制就是0x1C。低位在前传输。
	////////////////////////////////////	data[24]=0x1C;
	////////////////////////////////////	data[25]=0x00;
		// 命令字节[1字节]
		data[26]=0x02;//命令字节为发送数据，上传建筑消防设施部件运行状态。
		/*******应用数据单元***********/
	//数据单元标识符。
		data[27]=0x02;//类型标识 1字节上传建筑消防设施部件运行状态 上行
//		data[28]=5;//信息对象数目
		data[28]=(message[0]-9)/7;//信息对象数目		
		//------信息体----------------//
		step=29;
//		for(t=0;t<5;t++)
		for(t=0;t<(message[0]-9)/7;t++)//离位或恢复组的个数。		
		{
			data[step]=0x02;//系统类型标识	 1字节 火灾报警系统 01																//29						
			step=step+1;
			data[step]=0x01;//系统地址	1字节 系统地址=01																				//30
			step=step+1;
			data[step]=0xB2;//部件类型为蓝牙离位基站，0xB2.（数据隐藏在部件ID里面，第一个字节。）//31
			step=step+1;		
			data[step]=0x00;//回路号。低位在前。																								//32
			step=step+1;		
			data[step]=0x00;																																	//33
			step=step+1;
			data[step]=0x00;//部件号。	低位在前。																								//34
			step=step+1;		
			data[step]=0x00;																																	//35
			step=step+1;	
			data[step]=message[4+7*t];//0x02为离位，0x05为恢复。																//36
			step=step+1;
			data[step]=0x00;																																	//37
			step=step+1;		
		//~~~~~~~~~~部件说明（共31个字节）~~~~~~~~~~~~~//
			for (i = 0; i < 6; i++)//部件的6位ID（蓝牙离位基站的ID）.//38
			{
				data[step+i]=message[5+7*t+i];
			}
			step=step+6;	
			for (i = 0; i < 25; i++)//补齐31个字节。
			{
				data[step+i]=0x00;
			}
			step=step+25;		
		//----------补齐6位时间------------//		
			for(i=0;i<6;i++)
			{//补齐6个时间字节。
				data[step]=0x00;
				step=step+1;
			}	
		}
	// 应用数据单元长度[2字节],低位在前传输。
step_middle=step-27;		
		data[24]=step_middle;
		data[25]=step_middle>>8;		
		
		
//		data[24]=step-27;
//		data[25]=0x00;
		/********校验和***************/
		// 校验和
		data[step]=sumcheck(data,2,step-1);//计算校验和.
		step=step+1;
		/********结束符***************/
		// 结束符
		data[step]=0x23;
		step=step+1;
		data[step]=0x23;
		step=step+1;//数组中元素总共的数量。
	//	USART1_SEND(data,step);//调试用。		
	//	printf("\r\n");		
	//	USART2_SEND(data,step);//通过串口2发送给GM3。			
	//	UART4_SEND(data,step);//调试用。				
	/****************************************/	
	//	USART1_SEND(data,lenth+3);//调试用，通过串口1发送给GM3.		
}
	 POST(&BT_Msg,step,data,&STORAGE_MEM);//GPRS_senddata(data,step);//USART2_SEND(data,step);//通过串口2发送给GM3。	
//	USART1_SEND(data,step);//测试
//	printf("\r\n");//测试	
}
void package_send_data(unsigned char *input_data)//	data是数据数组。数组中有多少组数据的算法就是数组中元素的个数减1然后除以6。数组的第一个元素是数组中元素个数。size代表数组中元素的个数。status代表状态。0x01是正常/巡检/心跳.0x06是离位报警。
{//这是使用GSM/M26/NB-IOT发送数据的方式。压栈数据包数组。
//输入的数组例如： 24 FB 00 00 00 00 14 FB 00 00 00 00 15 FB 00 00 00 00 16 FB 00 00 00 00 17  表示这四个标签离位。
/*****************************************蓝牙基站发送离位信息给服务器***************************************************/
//40 40 01 00 01 20 00 00 00 00 00 00 B2 23 96 60 40 01 00 00 00 00 00 00 38 00 02 02 04 02 01 B2 00 00 00 00 06 00 B2 23 96 60 40 01 04 04 06 FB 00 00 00 00 14 04 06 FB 00 00 00 00 15 04 06 FB 00 00 00 00 16 04 06 FB 00 00 00 00 17 00 00 00 00 00 00 A3 23 23 
// 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 
//解析：
//40 40 (0,1)包头
//01 00 (2,3)业务流水号。（低位在前）
//01 20 (4,5)协议版本号。
//00 00 00 00 00 00 (6,7,8,9,10,11)时间标签
//01 02 03 04 05 06 (12,13,14,15,16,17)网关ID
//00 00 00 00 00 00 (18,19,20,21,22,23)目的地址
//38 00 (24,25)应用数据单元长度
//02 (26)命令字节
//02(27)//类型标识 1字节上传建筑消防设施部件运行状态 上行
//04(28)//信息对象数目,1个。		
//02(29)//系统类型标识	 1字节 火灾报警系统 01 
//01(30)//系统地址	1字节 系统地址=01
//B2(31)//部件类型 
//00 00(32,33)//回路号。低位在前。 
//00 00(34,35)//部件号。	低位在前。	 
//06 00(36,37)//部件状态。	低位在前。
//28 23 96 20 00 01(38,39,40,41,42,43)//部件的6位ID
//04(44)//上报参数数量
//04(45)//参数类型(离位）
//06(46)//参数长度（每个ID有6个字节）
//FB 00 00 00 00 14(47~52)//离位标签的6位ID 
//04(53)//参数类型(离位） 
//06(54)//参数长度（每个ID有6个字节）  
//FB 00 00 00 00 15(55~60)//离位标签的6位ID   
//04(61)//参数类型(离位）   
//06(62)//参数长度（每个ID有6个字节）  
//FB 00 00 00 00 16(63~68)//离位标签的6位ID   
//04(69)//参数类型(离位）   
//06(70)//参数长度（每个ID有6个字节）  
//FB 00 00 00 00 17(71~76)//离位标签的6位ID
//00 00 00 00 00 00(77~82)//时间。
//A3(83)//校验和 
//23 23(84，85)//数据包尾	


//离位：			
//FB 00 00 00 02 15			
//FB 00 00 00 00 30			
//恢复：
//FB 00 00 00 01 11 
//FB 00 00 00 00 45		
//1C代表的是后面元素的数量，换算成十进制为28.
// 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 
//1C 05 FB 00 00 00 02 15 02 FB 00 00 00 01 11 02 FB 00 00 00 00 45 05 FB 00 00 00 00 30 	




	unsigned int i = 0;
	unsigned int j = 0;	
//	unsigned int lenth = 0;	
	unsigned int step = 0;
	unsigned char package_data[100]={0x00};//太大了。。。	
	unsigned short  serial_number_counter_middle=0;//业务流水号
/******************整理数据包**********************/
	if(serial_number_counter>=0xFFFE)//业务流水号的自加。
	{
		serial_number_counter=0;
	}
	else
	{
		serial_number_counter=serial_number_counter+1;
	} 		
	if(input_data[0]==0xFF)//心跳
	{
		// 启动符[2字节]
		package_data[0]=3;
		package_data[1]=0x01;//标志类型为心跳。	
		/***控制单元*****************/
		// 业务流水号[2字节]
		serial_number_counter_middle=serial_number_counter;
		package_data[2]=serial_number_counter_middle;//流水号。0x00 0x00 ~ 0xFF 0xFF.滚动。低位在前	
		package_data[3]=serial_number_counter_middle>>8;		
		package_data[4]=package_data[1];//秒
		package_data[5]=package_data[2];//分
		package_data[6]=package_data[3];//时
		package_data[7]=package_data[4];//日			
		package_data[8]=package_data[5];//月	
		package_data[9]=package_data[6];//年		
		step=10;
	//	USART1_SEND(data,33);//调试用。
	}
	else
	{
		// 类型标志[1字节]。
		package_data[1]=0x06;		
		// 业务流水号[2字节]
		serial_number_counter_middle=serial_number_counter;	
		package_data[2]=serial_number_counter_middle;//流水号。0x00 0x00 ~ 0xFF 0xFF.滚动。低位在前
		package_data[3]=serial_number_counter_middle>>8;
		// 离位ID压入。
		for (i = 0; i < input_data[0]; i++)//部件的状态和ID（蓝牙离位基站的ID）.
		{
			package_data[4+i]=input_data[1+i];
		}

		RTC_Get();//获取RTC时间，加入时间戳。	
////calendar.w_year=calendar.w_year%2000;	//年，取年份的后两位。
////calendar.w_month;	//月
////calendar.w_date;	//日
////calendar.hour;		//小时
////calendar.min;			//分钟
////calendar.sec;			//秒钟	
//		package_data[1]=calendar.sec;//秒
//		package_data[2]=calendar.min;//分
//		package_data[3]=calendar.hour;//时
//		package_data[4]=calendar.w_date;//日			
//		package_data[5]=calendar.w_month;//月	
//		package_data[6]=calendar.w_year;//年				

		package_data[4+input_data[0]]=calendar.sec;//秒
		package_data[5+input_data[0]]=calendar.min;//分
		package_data[6+input_data[0]]=calendar.hour;//时
		package_data[7+input_data[0]]=calendar.w_date;//日			
		package_data[8+input_data[0]]=calendar.w_month;//月	
		package_data[9+input_data[0]]=calendar.w_year;//年			

		// 数组中元素的个数[1字节]。
//		package_data[0]=input_data[0]+3;//主要是加入了流水号。
		package_data[0]=input_data[0]+9;//主要是加入了流水号和时间。
		step=package_data[0]+1;//数据包中元素总个数。	
//	USART1_SEND(data,step);//调试用，通过串口1发送给GM3.			
	}
//	LoRa_433_Pack_DATA(package_data);//package_data[0]代表数组Package_DATA中元素的个数+1.	
/*******************压栈数据包数组*********************/	
	//如果这个位置的元素数量不为0，则向前走一格。
	for(i=0;i<step;i++)//压栈数据
	{
		send_data[SEND_DATA_to_server_GROUP][i]=package_data[i];
	}
	if(SEND_DATA_to_server_GROUP>=19)//群组个数不得超过定义的数组的长度。循环填充。0~19~0~19
	{
		SEND_DATA_to_server_GROUP=0;//群组数量
	}
	else
	{//按照溢出处理
		SEND_DATA_to_server_GROUP++;//群组的个数加1
	}
    if(send_data[SEND_DATA_to_server_GROUP][0]>0)//没来得及处理的数据已经形成一个环了，需要将最开始的一帧数据清掉了。
	{
		finger_point=SEND_DATA_to_server_GROUP;//将指向指针的位置与 SEND_DATA_to_server_GROUP 一样一样。（不管此时finger_point的位置！）
	}
/****************************************/	
//	USART1_SEND(data,step);//调试用，通过串口1发送给GM3.	
}



///=======================================/[2018-05-11 07:39:51.491]

//蓝牙基站发送心跳，业务流水号：209[2018-05-11 11:08:02.464]
//蓝牙基站发送心跳，业务流水号：464[2018-05-12 03:24:47.384]
//蓝牙基站发送心跳，业务流水号：654[2018-05-12 06:36:10.614]
//GPRS模块长时间未能发送成功数据，重启。[2018-05-12 06:36:30.712]

//蓝牙基站发送心跳，业务流水号：654[2018-05-12 06:36:36.821]
//蓝牙基站发送心跳，业务流水号：705[2018-05-12 07:28:24.605]
//蓝牙基站发送心跳，业务流水号：719[2018-05-12 07:42:48.838]
//GPRS模块长时间未能发送成功数据，重启。[2018-05-12 07:43:08.936]



//40 40 04 00 01 20 00 00 00 00 00 00 98 25 0A 30 40 08 00 00 00 00 00 00 30 00 02 02 01 02 01 B2 00 00 00 00 02 00 FB 00 00 00 01 50 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 9C 23 23 
//40 40 05 00 01 20 00 00 00 00 00 00 98 25 0A 30 40 08 00 00 00 00 00 00 30 00 02 02 01 02 01 B2 00 00 00 00 05 00 FB 00 00 00 01 50 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 A0 23 23 





