#include "usart.h"
#include "usart2.h"
#include "usart3.h"
#include "uart4.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"
#include "led.h"
#include "key.h"
#include "stmflash.h"
#include "buzzer.h"
#include "delay.h"
#include "function.h"
#include "timer.h"
#include "rtc.h"
unsigned char receive_once=0;//蓝牙离位基站一次搜索搜索到的设备数量。
u16 mark_status=0xFFFF;//离位标签的标志位。按位来算。当为0的时候，表示离位。例如 mark_tatus 当前的值为：0100 0011  则说明8，6，5，4，3号标签发生了离位。
u16 mark_status_last=0xFFFF;//上次离位标签的标志位。	
//////////////////////////////////////////////////////////////////////////////////
unsigned int departure_time; 			//宏定义离位时间。
unsigned int bluetooh_scan_heart_time; //蓝牙基站巡检时间
unsigned int judge_fruit_time;//检测数组中的数据。判断是否有离位。防止每次都进入做对比。浪费单片机资源
//串口接收缓存区 	
/***************************串口结合定时器接收数据***************************/
unsigned char UART4_RX_FLAG=0;         //置标志位，告诉定时器开始收数了,也说明串口正在收数据，不要做其他操作打断。
unsigned char UART4_RX_TIME=0;       	//接收字符串时的时间间隔
unsigned int 	UART4_RX_NO=0;          	//接收字符串字符的总数量
unsigned char UART4_GROUP=0;   				//最大接收缓存数组个数	
unsigned char UART4_RX_BUF[10][100]={0x00}; 	//接收缓冲二维数组。
unsigned char UART4_RX_BUF_middle[100]={0x00}; 	//接收缓冲二维数组。
unsigned char UART4_finger_point=0;    //数组消息循环中指针指向的位置。
/***************************************************************************/

//初始化IO 串口3
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void UART4_Init(u32 bound)
{ 	
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	// GPIOD时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE); //串口4时钟使能
 	USART_DeInit(UART4);  //复位串口4
		 //UART4_TX   PC.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化PC.10
    //UART4_RX	  PC.11GPIO_Mode_IPU
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入   GPIO_Mode_IPU
  GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化PC.11
	
	USART_InitStructure.USART_BaudRate = bound;//波特率一般设置为115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  
	USART_Init(UART4, &USART_InitStructure); //初始化串口	4  
	USART_Cmd(UART4, ENABLE);                    //使能串口 	
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启中断
	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}
void test_URAT4(void)
{
	char send_data[128]="+++";
	char recv_data[128];
	int i = 0;
	int j = 0;
	for(i =0;i<3;i++)
	{
		USART_SendData(UART4,send_data[i]);
	}
	for(j =0;j<3;j++)
	{
		recv_data[j]=USART_ReceiveData(UART4);
	}
}

//串口4,printf 函数
//确保一次发送数据不超过USART4_MAX_SEND_LEN字节
void u4_printf(char* fmt,...)  
{
	u16 i,j; 
	u8 UART4_TX_BUF[200]={0x00}; 			//发送缓冲,最大UART4_MAX_SEND_LEN字节
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)UART4_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)UART4_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
	  while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(UART4,UART4_TX_BUF[j]); 
	} 
}
//串口4,输出函数
void UART4_SEND(unsigned char* UART4_TX_BUF,unsigned int size) // UART4_TX_BUF代表要输出的数组。size代表输出数组的长度。 
{  
	u16 i; 
	for(i=0;i<size;i++)							//循环发送数据
	{
		USART_SendData(UART4,UART4_TX_BUF[i]); 
	} 
}
//通过判断接收连续2个字符之间的时间差不大于20ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//USART_RX_BUF_middle[0]代表数组中元素的数量。例如：0x05 0x01 0x02 0x03 0x04 0x05
void UART4_IRQHandler(void)
{
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
/******************************************************************************************************************/	
			UART4_RX_FLAG=1;//置标志位，告诉定时器可以开始为我工作了。时间间隔判断一组数据是否收完。
			UART4_RX_TIME=5;//重新装初值。开始倒计时，用来判断是否接受完了一组数据。5次进入定时器中断。
			UART4_RX_BUF_middle[UART4_RX_NO+1] = USART_ReceiveData(UART4);//读取接收到的数据				
			if(UART4_RX_NO<98)
			{
				UART4_RX_NO++;//每组串口数据收到的数据个数累加.不得超过数组定义的数量。		
			}
			else
			{//按照溢出处理
				UART4_RX_NO=98;
			}
			UART4_RX_BUF_middle[0]=UART4_RX_NO;
/******************************************************************************************************************/			
    }
}
void UART4_RX_SCAN(void)//定时器定时扫描串口标志位的变化，及时的进行处理。
{
	unsigned char i=0;
	if(UART4_RX_FLAG==1)//判断是否正在接收串口数据
	{
		UART4_RX_TIME--;//将倒计时寄存器内的数值减一。
		if(UART4_RX_TIME<=3)//说明一串数中两个数据的间隔超过了设定的时间了，判定数据接收完毕。
		{//当接收的字节之间的时间超过接收3~5个字节的时间的时候则认为本次接收完成，接收完成标志置1.
			UART4_RX_FLAG=0;//将串口开始计时标志位数据清零	
			UART4_RX_TIME=5;//将USART_RX_TIME的值填满，防止在减1的时候出现负值。
			UART4_RX_NO=0;//串口接收数据长度清理。			
/*******************压栈数据包数组*********************/	
			//如果这个位置的元素数量不为0，则向前走一格。		
			for(i=0;i<UART4_RX_BUF_middle[0]+1;i++)//压栈数据
			{
				UART4_RX_BUF[UART4_GROUP][i]=UART4_RX_BUF_middle[i];
			}
			if(UART4_GROUP>=9)//群组个数不得超过定义的数组的长度。循环填充。0~19~0~19
			{
				UART4_GROUP=0;//群组数量
			}
			else
			{//按照溢出处理
				UART4_GROUP++;//群组的个数加1
			}
			if(UART4_RX_BUF[UART4_GROUP][0]!=0x00)//没来得及处理的数据已经形成一个环了，需要将最开始的一帧数据清掉了。
			{
				UART4_finger_point=UART4_GROUP;//将指向指针的位置与 UART4_finger_point 一样一样。（不管此时finger_point的位置！）
			}	            
		}
	}		
}

void UART4_COMAND_SCAN(void)//刷新读取串口3数据。查看是否有命令过来需要处理。 
{
	unsigned int i=0;
	unsigned int lenth=0;//串口收完一组数据的长度。
	unsigned char UART4_RXD[101]={0x00};//定义发送数组				
	if( ( (UART4_RX_BUF[UART4_finger_point][0]!=0x00)&&(UART4_RX_FLAG==0) )==1 )//如果发现串口收数数组中有数据了，并且串口收数部分正闲置。
	{
		//每次都是将第一组的数据提取出来。因为先进先出原则。FIFO。
		for(i=0;i<UART4_RX_BUF[UART4_finger_point][0];i++)//UART4_RX_BUF[UART4_GROUP][0]是UART4_RX_BUF[j][i]中数据的长度。取出，使用。如果有7个数，则UART4_RX_BUF[USART1_GROUP][0]=7.
		{
			UART4_RXD[i]=UART4_RX_BUF[UART4_finger_point][i+1];//	将即将要处理的数据取出来。	
			UART4_RXD[i+1]=0x00;//添加结束符
		}
		lenth=UART4_RX_BUF[UART4_finger_point][0];//这个数组中元素的个数。
		UART4_RX_BUF[UART4_finger_point][0]=0x00;//清空这个数组中元素的个数。
		if(UART4_finger_point>=9)//群组个数不得超过定义的数组的长度。循环填充。0~9~0~9
		{
			UART4_finger_point=0;//群组的指针变到头位置。
		}
		else
		{
			UART4_finger_point++;//群组的指针加1。
		}//
//		printf("%s\r\n",UART4_RXD);
				USART1_SEND(UART4_RXD,lenth);
//				printf("\r\n");
	UART4_Data_analyze(UART4_RXD,lenth);//串口4收数数据解析.UART4_RXD是数据数组，lenth是数据长度。	
	}
/************************************************************************************/			
}
void UART4_Data_analyze(unsigned char *UART4_RXD,unsigned char lenth)//串口4收数数据解析.UART4_RXD是数据数组，lenth是数据长度。
{
	unsigned int i=0;
//	unsigned int lenth=0;//串口收完一组数据的长度。
//	unsigned char sum_check=0;//校验和
	unsigned char NODE_ID_u8[20]={0x00};//标签6位ID	
	unsigned char NODE_ID_u8_middle[20]={0x00};//标签6位ID		
//	unsigned short  NODE_ID_u16[20]={0x00};//标签6位ID		
//	unsigned char middle_data[20]={0x00};//中间数据		
	unsigned char printf_data[20]={0x00};	
//单片机发送  AT+StartScan  后。搜索附近的蓝牙设备，主机蓝牙模块如果收到了单片机发送的命令，则首先返回  AT+ok			
//AT+ok\r\n
//41 54 2B 6F 6B 0D 0A
//如果搜索到了蓝牙标签，则主机蓝牙模块发送给单片机下面的数据。其中  A4C1389653AA  是蓝牙标签的MAC地址。FB0000000005  是设备名。
//AT+Scan=A4C1389653AA,FB0000000005\r\n
//A   T  +  S  c  a  n  =  A  4  C  1  3  8  9  6  5  3  A  A  ,  F  B  0  0  0  0  0  0  0  0  0  5 \r \n			
//41 54 2B 53 63 61 6E 3D 41 34 43 31 33 38 39 36 35 33 41 41 2C 46 42 30 30 30 30 30 30 30 30 30 35 0D 0A			
// 0 	1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34
	
//AT+ADV_TIMEOUT=300   是设置的发送周期为300.
//AT+ADVINT=50  是设置的发送数据的周期。例如从模块收到主模块发送的AT+StartScan后，会每50ms从模块就发送一次本身ID，如果AT+ADV_TIMEOUT=300，则timeout/50=6重发次数。
//AT+ADV_CYCLE=10  就是唤醒周期是10秒	

//从模块可能收到了主模块发送的AT+StartScan命令，但是从模块发送的本身ID可能会因为环境因素，造成发送的数据无法被主模块接收到。所以增加从模块的发送次数。
//	
//发送一次搜索命令，当主模块收到16个设备的时候，就停止搜索了。
//搜索频率应该跟时间关系不大，因为是按照搜索到的设备数量来决定停止还是继续的。
//在程序中设置一下，当搜索到的设备大于等于15的时候，就开始新的一轮搜索。
	
	
			if(  (UART4_RXD[0]==0x41)&&(UART4_RXD[3]==0x53)&&(UART4_RXD[7]==0x3D)&&(UART4_RXD[20]==0x2C)&&(UART4_RXD[21]==0x46)&&(UART4_RXD[22]==0x42)&&(UART4_RXD[34]==0x0A)  )//如果包头包尾正确,特定位置的,正确。
			{
				receive_once=receive_once+1;//搜索到了一个蓝牙设备。
//				ID_lenth=lenth-23;//长度为总的长度减去0x0D,0x0A的长度2，减去逗号之前的长度21.
				//提取蓝牙标签的ID。每个标签有6位ID。因为显示原因。是12位的。
				for(i=0;i<12;i++)
				{
					NODE_ID_u8_middle[i]=UART4_RXD[i+21];
				}
				DATA_change(NODE_ID_u8_middle,12,NODE_ID_u8);
				DATA_change_u8_to_u16(NODE_ID_u8,12,judge_data_u16);//将6个8位的元素数组转换成3个16位元素的数组。				
				Judge_ID(judge_data_u16);//判断合法性。是否已经在EEPROM中有存储注册,如果返回数据中的NODE_ID[6]和NODE_ID[7]都是非0x00的值，则是已经注册设备所在的位置。如果返回NODE_ID[6]和NODE_ID[7]都为0x00，则是未注册设备。	
				if(judge_data_u16[4]==0x0000)//如果节点没有被注册
				{
					
				}
				else//设备已经被注册过。0xFF是一个不可能的值。如果被搜索到，说明设备没有离位，则清空这个设备的所有标志位，进入下一轮的搜索环节。
				{//judge_data_u16[3]  是返回的当前找到的蓝牙标签ID的地址。
					if(judge_data_u16[3]<16)
					{
//										switch(NODE_ID_u16[3])
//											{
//												case 0://
//													mark_tatus=mark_tatus&0xFE;						
//												break;
//												case 1://
//													mark_tatus=mark_tatus&0xFD;			
//												break;
//												case 2://
//													mark_tatus=mark_tatus&0xFB;						
//												break;	
//												case 3://
//													mark_tatus=mark_tatus&0xF7;						
//												break;
//												case 4://
//													mark_tatus=mark_tatus&0xEF;						
//												break;	
//												case 5://
//													mark_tatus=mark_tatus&0xDF;						
//												break;
//												case 6://
//													mark_tatus=mark_tatus&0xBF;										
//												break;	
//												case 7://
//													mark_tatus=mark_tatus&0x7F;				
//												break;		
//												default:
//												break;
//											}	
						Bluetooh_time[judge_data_u16[3]]=departure_time;//重新赋值。如果时间超时，则重新赋值。现在是20秒巡检一次，如果三次巡检，即240秒内都发现不了蓝牙标签，则认为蓝牙标签离位了。
						handle_arrays_u8(NODE_ID_u8,6,printf_data);//测试用。打印出当前搜索到的蓝牙标签ID。
						printf("搜索到：");	
						USART1_SEND(printf_data,12);		//测试用。
						printf("\r\n");							//测试用。		
					}
					else
					{
						printf("数据越界，视为无效数据。\r\n");							//测试用。	
					}	
				}
			}
}

void Bluetooh_SCAN(void)//刷新巡检蓝牙串口数据。 
{
//	static unsigned char Package_DATA_last[100]={0};//上一次的数据打包	。

	static u16	mark_flag=0;
	static u16	mark_flag_last=0;	
	
	unsigned char Package_DATA[120]={0x00};	//离位标签数据堆栈用。
    unsigned char Package_DATA2[100]={0x00};	//离位标签数据堆栈用。
	unsigned char Bluetooh_ID[20]={0x00};	
//	unsigned char printf_data[100]={0x00};
	unsigned char data_send[20]={0x00};
	unsigned char i=0;
	unsigned char j=1;
	unsigned char k=0;	
	if( (Bluetooh_scan_time==0)||(receive_once>=15) )//单片机给蓝牙芯片串口发送数据倒计时。发送巡检命令。AT+StartScan
	{
		receive_once=0;//搜索次数清零
		Bluetooh_scan_time=bluetooh_scan_heart_time;//单片机搜索蓝牙标签时间重装初值。10秒搜索一次。		
//		Buzzer_flag=3;
		data_send[0]=0x41;
		data_send[1]=0x54;
		data_send[2]=0x2B;
		data_send[3]=0x53;		
		data_send[4]=0x74;
		data_send[5]=0x61;	
		data_send[6]=0x72;
		data_send[7]=0x74;
		data_send[8]=0x53;
		data_send[9]=0x63;		
		data_send[10]=0x61;
		data_send[11]=0x6E;	
		data_send[12]=0x0D;
		data_send[13]=0x0A;	
		UART4_SEND(data_send,14);//向蓝牙模块发送搜索命令。  AT+StartScan\r\n	
		printf("/******************/");
		printf("\r\n");
	}
	if(Bluetooh_Package_compare==0)//到了检测蓝牙标签离位状态的时候了。n秒检测一次，定时器内进行自减。
	{
		Bluetooh_Package_compare=judge_fruit_time;//重装初值。防止每次都进入做对比。浪费单片机资源
//		Package_DATA[0]=0;
		for(i=0;i<Bluetooh_amount;i++)//  Bluetooh_amount 是指蓝牙基站保存了有多少个蓝牙离位标签。检测离位标签倒计时是否到了。比较次数。
		{
			if(Bluetooh_time[i]==0)//每个蓝牙标签计数器倒计时，如果超时没有被发现，则认为蓝牙标签离位了。然后通过GM3或者433或者LoRa发送出去给基站或者服务器。
			{
				switch(i)//如若发生离位，就将这个位置清零！
				{		
					case 0://
						mark_status=mark_status&0xFFFE;						
					break;
					case 1://
						mark_status=mark_status&0xFFFD;			
					break;
					case 2://
						mark_status=mark_status&0xFFFB;						
					break;	
					case 3://
						mark_status=mark_status&0xFFF7;						
					break;
					case 4://
						mark_status=mark_status&0xFFEF;						
					break;	
					case 5://
						mark_status=mark_status&0xFFDF;						
					break;
					case 6://
						mark_status=mark_status&0xFFBF;										
					break;	
					case 7://
						mark_status=mark_status&0xFF7F;
					break;
                    case 8://
						mark_status=mark_status&0xFEFF;						
					break;
					case 9://
						mark_status=mark_status&0xFDFF;			
					break;
					case 10://
						mark_status=mark_status&0xFBFF;						
					break;	
					case 11://
						mark_status=mark_status&0xF7FF;						
					break;
					case 12://
						mark_status=mark_status&0xEFFF;						
					break;	
					case 13://
						mark_status=mark_status&0xDFFF;						
					break;
					case 14://
						mark_status=mark_status&0xBFFF;										
					break;	
					case 15://
						mark_status=mark_status&0x7FFF;
					break;
					default:
					break;
				}				
			}
			else
			{
				switch(i)//如若发生恢复，就将这个位置置1！
				{
					case 0://
						mark_status=mark_status|0x0001;						
					break;
					case 1://
						mark_status=mark_status|0x0002;			
					break;
					case 2://
						mark_status=mark_status|0x0004;						
					break;	
					case 3://
						mark_status=mark_status|0x0008;						
					break;
					case 4://
						mark_status=mark_status|0x0010;						
					break;	
					case 5://
						mark_status=mark_status|0x0020;						
					break;
					case 6://
						mark_status=mark_status|0x0040;										
					break;	
					case 7://
						mark_status=mark_status|0x0080;				
					break;		
                    case 8://
						mark_status=mark_status|0x0100;						
					break;
					case 9://
						mark_status=mark_status|0x0200;			
					break;
					case 10://
						mark_status=mark_status|0x0400;						
					break;	
					case 11://
						mark_status=mark_status|0x0800;						
					break;
					case 12://
						mark_status=mark_status|0x1000;						
					break;	
					case 13://
						mark_status=mark_status|0x2000;
					break;
					case 14://
						mark_status=mark_status|0x4000;
					break;	
					case 15://
						mark_status=mark_status|0x8000;				
					break;		
					default:
					break;
				}					
			}
		}
		if(mark_status!=mark_status_last)//比较数据。方便打包。如若位置数据发生变化，则认为离位或者恢复了。
		{
			for(i=0;i<Bluetooh_amount;i++)
			{
				switch(i)
				{		
					case 0://
						mark_flag=mark_status&0x0001;			
					break;
					case 1://
						mark_flag=mark_status&0x0002;				
					break;
					case 2://
						mark_flag=mark_status&0x0004;					
					break;	
					case 3://
						mark_flag=mark_status&0x0008;						
					break;
					case 4://
						mark_flag=mark_status&0x0010;		
					break;	
					case 5://
						mark_flag=mark_status&0x0020;							
					break;
					case 6://
						mark_flag=mark_status&0x0040;												
					break;	
					case 7://
						mark_flag=mark_status&0x0080;					
					break;
                    case 8://
						mark_flag=mark_status&0x0100;			
					break;
					case 9://
						mark_flag=mark_status&0x0200;				
					break;
					case 10://
						mark_flag=mark_status&0x0400;					
					break;	
					case 11://
						mark_flag=mark_status&0x0800;						
					break;
					case 12://
						mark_flag=mark_status&0x1000;		
					break;	
					case 13://
						mark_flag=mark_status&0x2000;							
					break;
					case 14://
						mark_flag=mark_status&0x4000;												
					break;	
					case 15://
						mark_flag=mark_status&0x8000;					
					break;	
					default:
					break;
				}
				switch(i)
				{
					case 0://
						mark_flag_last=mark_status_last&0x0001;			
					break;
					case 1://
						mark_flag_last=mark_status_last&0x0002;				
					break;
					case 2://
						mark_flag_last=mark_status_last&0x0004;					
					break;	
					case 3://
						mark_flag_last=mark_status_last&0x0008;						
					break;
					case 4://
						mark_flag_last=mark_status_last&0x0010;		
					break;	
					case 5://
						mark_flag_last=mark_status_last&0x0020;							
					break;
					case 6://
						mark_flag_last=mark_status_last&0x0040;												
					break;	
					case 7://
						mark_flag_last=mark_status_last&0x0080;					
					break;
                    case 8://
						mark_flag_last=mark_status_last&0x0100;			
					break;
					case 9://
						mark_flag_last=mark_status_last&0x0200;				
					break;
					case 10://
						mark_flag_last=mark_status_last&0x0400;					
					break;	
					case 11://
						mark_flag_last=mark_status_last&0x0800;						
					break;
					case 12://
						mark_flag_last=mark_status_last&0x1000;		
					break;	
					case 13://
						mark_flag_last=mark_status_last&0x2000;							
					break;
					case 14://
						mark_flag_last=mark_status_last&0x4000;												
					break;	
					case 15://
						mark_flag_last=mark_status_last&0x8000;					
					break;	
					default:
					break;
				}
				if(mark_flag==mark_flag_last)//状态没有发生变化
				{
					
				}
				else if(mark_flag>mark_flag_last)//发生了离位恢复
				{
					Package_DATA[j]=0x05;//离位恢复
					j=j+1;
					STMFLASH_Read ( (DEVICE_ADDRESS+i*10),(u16*)Bluetooh_ID,5 );//读出恢复的蓝牙标签的ID。
					for(k=0;k<6;k++)//将离位标签的6位ID压入到比较数组中。
					{
						Package_DATA[j]=Bluetooh_ID[k];
						j=j+1;								
					}							
				}
				else if(mark_flag<mark_flag_last)//发生了离位
				{
					Package_DATA[j]=0x02;//离位报警
					j=j+1;
					STMFLASH_Read ( (DEVICE_ADDRESS+i*10),(u16*)Bluetooh_ID,5 );//读出失联的蓝牙标签的ID。
					for(k=0;k<6;k++)//将离位标签的6位ID压入到比较数组中。
					{
						Package_DATA[j]=Bluetooh_ID[k];
						j=j+1;								
					}						
				}
			}	
//			Package_DATA[0]=(j-1)/7;//计算出有多少组数据。
            if(j<=57)
            {
                Package_DATA[0]=j-1;//计算出数组中有多少个元素。			
	//			send_flag=0;
	//			USART3_SEND(Package_DATA_last,Package_DATA_last[0]);//调试用
//			USART1_SEND(Package_DATA,Package_DATA[0]+1);//调试用
			package_send_data(Package_DATA);//Package_DATA[0]代表数组Package_DATA中元素的个数+1.		
            }
            else
            {
                Package_DATA[0]=56;//计算出数组中有多少个元素。			
                Package_DATA2[0]=j-57;//计算出数组中有多少个元素。                
                memcpy(Package_DATA2+1,Package_DATA+57,j-57);
                memset(Package_DATA+57,0,sizeof(Package_DATA)-57);
                package_send_data(Package_DATA);//Package_DATA[0]代表数组Package_DATA中元素的个数+1.		
                package_send_data(Package_DATA2);//Package_DATA[0]代表数组Package_DATA中元素的个数+1.
            }
			
			mark_status_last=mark_status;
// 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 
//1C 05 FB 00 00 00 02 15 02 FB 00 00 00 01 11 02 FB 00 00 00 00 45 05 FB 00 00 00 00 30 	
//离位：			
//FB 00 00 00 02 15			
//FB 00 00 00 00 30			
//恢复：
//FB 00 00 00 01 11 
//FB 00 00 00 00 45		
		}		
	}
/***********************************************************************************************************/		
//将离位标签的ID进行打包。打包完毕后再跟上次的数据进行比对。如果不一致说明有新的离位标签离位了或者恢复了，则需要立刻上发。	
}

void get_all_status(void)
{
    u32 i=0,j=1,k=0;
    u16 mark_flag;
    u8 Package_DATA[120]={0x00};
    u8 Bluetooh_ID[20]={0x00};
    unsigned char Package_DATA2[100]={0x00};
    for(i=0;i<Bluetooh_amount;i++)
    {
        mark_flag=mark_status&(0x0001<<i);
        if(mark_flag)//发生了离位恢复
        {
            Package_DATA[j]=0x05;//离位恢复
            j=j+1;
            STMFLASH_Read ( (DEVICE_ADDRESS+i*10),(u16*)Bluetooh_ID,5 );//读出恢复的蓝牙标签的ID。
            for(k=0;k<6;k++)//将离位标签的6位ID压入到比较数组中。
            {
                Package_DATA[j]=Bluetooh_ID[k];
                j=j+1;								
            }							
        }
        else//发生了离位
        {
            Package_DATA[j]=0x02;//离位报警
            j=j+1;
            STMFLASH_Read ( (DEVICE_ADDRESS+i*10),(u16*)Bluetooh_ID,5 );//读出失联的蓝牙标签的ID。
            for(k=0;k<6;k++)//将离位标签的6位ID压入到比较数组中。
            {
                Package_DATA[j]=Bluetooh_ID[k];
                j=j+1;								
            }						
        }
    }
    if(j<=57)
    {
        Package_DATA[0]=j-1;//计算出数组中有多少个元素。			
        package_send_data(Package_DATA);//Package_DATA[0]代表数组Package_DATA中元素的个数+1.		
    }
    else
    {
        Package_DATA[0]=56;//计算出数组中有多少个元素。			
        Package_DATA2[0]=j-57;//计算出数组中有多少个元素。                
        memcpy(Package_DATA2+1,Package_DATA+57,j-57);
        memset(Package_DATA+57,0,sizeof(Package_DATA)-57);
        package_send_data(Package_DATA);//Package_DATA[0]代表数组Package_DATA中元素的个数+1.		
        package_send_data(Package_DATA2);//Package_DATA[0]代表数组Package_DATA中元素的个数+1.
    }
}

void dislocation_test(void)//短按键后，发送一帧离位报警。包括所有的节点信息。
{
//举例
//41 01 02 B2 17 11 11 01 00 00 00 AB 06 04 FB 00 00 00 00 14 FB 00 00 00 00 15 FB 00 00 00 00 16 FB 00 00 00 00 17 00 00 E6 24
// 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 	
	unsigned char i=0;
	unsigned char j=1;	
	unsigned char k=0;	
	unsigned char Bluetooh_ID[20]={0x00};			
	unsigned char Package_DATA[120]={0x00};	//离位标签数据堆栈用。
	printf("测试发送数据\r\n");
	for(i=0;i<Bluetooh_amount;i++)//  Bluetooh_amount 是指蓝牙基站保存了有多少个蓝牙离位标签。检测离位标签倒计时是否到了。比较次数。
	{
		Bluetooh_time[i]=0;//将所有离位标签的计数器都清空，制造一种都离位的假象。方便测试的时候对标签状态进行测试。发送完全部离位后，会陆陆续续的发送回复报警。
		mark_status=0x0000;//离位标签的标志位。按位来算。当为0的时候，表示离位。例如 mark_tatus 当前的值为：0100 0011  则说明8，6，5，4，3号标签发生了离位。
		mark_status_last=0xFFFF;//上次离位标签的标志位
	}
}

