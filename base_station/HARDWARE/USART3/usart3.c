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
#include "function.h"
#include "timer.h"
#include "os.h"
/***************************串口结合定时器接收数据***************************/ 
unsigned char USART3_RX_FLAG=0;         //置标志位，告诉定时器开始收数了,也说明串口正在收数据，不要做其他操作打断。
unsigned char USART3_RX_TIME=0;       	//接收字符串时的时间间隔
unsigned int 	USART3_RX_NO=0;          	//接收字符串字符的总数量
unsigned char USART3_GROUP=0;   				//最大接收缓存数组个数	
unsigned char USART3_RX_BUF[10][100]={0x00}; 	//接收缓冲二维数组。
unsigned char USART3_RX_BUF_middle[100]={0x00}; 	//接收缓冲二维数组。
unsigned char USART3_finger_point=0;    //数组消息循环中指针指向的位置。
/***************************************************************************/
unsigned char send_success_flag=0;//往网关或者服务器发送数据成功标志位。成功，返回1。失败，返回0。
//初始化IO 串口3
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void USART3_Init(u32 bound)
{ 	
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);// GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

 	USART_DeInit(USART3);                                //复位串口1
	 //USART3_TX   PB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;            //PB.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	     //复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);               //初始化PB.10
 
	//USART3_RX	  PB.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;            //PB.11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入  GPIO_Mode_IN_FLOATING
	GPIO_Init(GPIOB, &GPIO_InitStructure);               //初始化PB.11
	
	USART_InitStructure.USART_BaudRate = bound;                                    //一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;                            //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	               //收发模式
  
	USART_Init(USART3, &USART_InitStructure); //初始化串口	2 
	USART_Cmd(USART3, ENABLE);                    //使能串口 	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断	
	
	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器		
}
 
//串口2,printf 函数
//确保一次发送数据不超过USART2_MAX_SEND_LEN字节
void u3_printf(char* fmt,...)  
{  
	u16 i,j; 
	u8 USART3_TX_BUF[200]={0x00}; 			//发送缓冲,最大USART2_MAX_SEND_LEN字节
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
//	  while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(USART3,USART3_TX_BUF[j]); 
	} 
}

//串口2,输出函数
void USART3_SEND(unsigned char* USART3_TX_BUF,unsigned int size) // USART3_TX_BUF代表要输出的数组。size代表输出数组的长度。 
{  
	u16 i; 
	for(i=0;i<size;i++)							//循环发送数据
	{
		USART_SendData(USART3,USART3_TX_BUF[i]); 
	} 
}
//通过判断接收连续2个字符之间的时间差不大于20ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//USART_RX_BUF_middle[0]代表数组中元素的数量。例如：0x05 0x01 0x02 0x03 0x04 0x05
void USART3_IRQHandler(void)
{
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
/******************************************************************************************************************/	
			USART3_RX_FLAG=1;//置标志位，告诉定时器可以开始为我工作了。时间间隔判断一组数据是否收完。
			USART3_RX_TIME=5;//重新装初值。开始倒计时，用来判断是否接受完了一组数据。5次进入定时器中断。
			USART3_RX_BUF_middle[USART3_RX_NO+1] = USART_ReceiveData(USART3);//读取接收到的数据			
			if(USART3_RX_NO<98)
			{
				USART3_RX_NO++;//每组串口数据收到的数据个数累加.不得超过数组定义的数量。		
			}
			else
			{//按照溢出处理
				USART3_RX_NO=98;
			}
			USART3_RX_BUF_middle[0]=USART3_RX_NO;
/******************************************************************************************************************/			
    }
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
}
void USART3_RX_SCAN(void)//定时器定时扫描串口标志位的变化，及时的进行处理。
{
	unsigned char i=0;
	if(USART3_RX_FLAG==1)//判断是否正在接收串口数据
	{
		USART3_RX_TIME--;//将倒计时寄存器内的数值减一。
		if(USART3_RX_TIME<=1)//说明一串数中两个数据的间隔超过了设定的时间了，判定数据接收完毕。
		{//当接收的字节之间的时间超过接收3~5个字节的时间的时候则认为本次接收完成，接收完成标志置1.
			USART3_RX_FLAG=0;//将串口开始计时标志位数据清零	
			USART3_RX_TIME=5;//将USART_RX_TIME的值填满，防止在减1的时候出现负值。
			USART3_RX_NO=0;//串口接收数据长度清理。			
/*******************压栈数据包数组*********************/	
			//如果这个位置的元素数量不为0，则向前走一格。
			for(i=0;i<USART3_RX_BUF_middle[0]+1;i++)//压栈数据
			{
				USART3_RX_BUF[USART3_GROUP][i]=USART3_RX_BUF_middle[i];
			}
			if(USART3_GROUP>=9)//群组个数不得超过定义的数组的长度。循环填充。0~19~0~19
			{
				USART3_GROUP=0;//群组数量
			}
			else
			{//按照溢出处理
				USART3_GROUP++;//群组的个数加1
			}
            if(USART3_RX_BUF[USART3_GROUP][0]!=0x00)//没来得及处理的数据已经形成一个环了，需要将最开始的一帧数据清掉了。
			{
				USART3_finger_point=USART3_GROUP;//将指向指针的位置与 USART3_finger_point 一样一样。（不管此时finger_point的位置！）
			}
		}
	}		
}
void USART3_COMAND_SCAN(void)//刷新读取串口3数据。查看是否有命令过来需要处理。 
{
	unsigned int i=0;
	unsigned int lenth=0;//串口收完一组数据的长度。
	unsigned char USART3_RXD[101];//定义发送数组			
	if( ( (USART3_RX_BUF[USART3_finger_point][0]!=0x00)&&(USART3_RX_FLAG==0) )==1 )//如果发现串口收数数组中有数据了，并且串口正闲置。
	{
		//每次都是将第一组的数据提取出来。遵循先进先出原则。FIFO。
		for(i=0;i<USART3_RX_BUF[USART3_finger_point][0];i++)//USART3_RX_BUF[USART3_GROUP][0]是USART3_RX_BUF[j][i]中数据的长度。取出，使用。如果有7个数，则USART3_RX_BUF[USART1_GROUP][0]=7.
		{
			USART3_RXD[i]=USART3_RX_BUF[USART3_finger_point][i+1];//	将即将要处理的数据取出来。	
			USART3_RXD[i+1]=0x00;//添加结束符
		}
		lenth=USART3_RX_BUF[USART3_finger_point][0];//这个数组中元素的个数。
		USART3_RX_BUF[USART3_finger_point][0]=0x00;//清空这个数组中元素的个数。
		if(USART3_finger_point>=9)//群组个数不得超过定义的数组的长度。循环填充。0~9~0~9
		{
			USART3_finger_point=0;//群组的指针变到头位置。
		}
		else
		{
			USART3_finger_point++;//群组的指针加1。
		}
//USART3_SEND(USART3_RXD,lenth);
//			u3_printf("%s",USART3_RXD);//调试用，将刚才收到的数据打印出来。
		USART3_Data_analyze(USART3_RXD,lenth);//串口1收数数据解析.USART3_RXD是数据数组，lenth是数据长度。
	}
/************************************************************************************/				
}
void USART3_Data_analyze(unsigned char *USART3_RXD,unsigned char lenth)//串口2收数数据解析.USART3_RXD是数据数组，lenth是数据长度。
{
	unsigned int i=0;
	unsigned char sum_check=0;//校验和
	unsigned char NODE_ID[20]={0x00};//设备ID号码。
	unsigned char memory_index[50]={0x00};//读取数据索引。0~5设备ID,6~7回路号，8~9部件号，10~11存储的节点数量。
	unsigned char test[30]={0x00};					
//	举例：41 02 02 B2 17 11 11 01 00 00 00 AB 06 04 FB 00 00 00 00 14 FB 00 00 00 00 15 FB 00 00 00 00 16 FB 00 00 00 00 17 00 00 E7 24   
//			   0  1  2  3  4	5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41
			
//	举例：    00 00 E7 24 			
//	/*-------解析---------*/			
//	41（0）	包头
//	02（1）	下行数据
//	/*-------数据场---------*/
//	02 B2 17 11 11 01 00 00 00 AB	（2~11）设备ID。433通信设备，烟感，17年11月11日，01批次，00 00 00 01编号
////0					代表通信设备类型（433，LoRa等）
////1 				代表部件类型（烟感，温感等）
////2，3，4		代表年月日。如17年11月12日，则为 0x17 0x11 0x12
////5 				代表生产批次。
////6，7，8，9	代表生产编号。
//	06 （12）部件状态为离位
//	04 （13）离位数量为4
//FB 00 00 00 00 14 （14~19）
//FB 00 00 00 00 15 （20~25）
//FB 00 00 00 00 16 （26~31）
//FB 00 00 00 00 17	（32~37）					
//	00 00 （38，39）备用数据
//	/*----------------------*/
//	E7 （40）校验和  02 02 B2 17 11 11 01 00 00 00 AB 06 04 FB 00 00 00 00 14 FB 00 00 00 00 15 FB 00 00 00 00 16 FB 00 00 00 00 17 00 00   相加
//	24 （41）包尾		
			if(   (USART3_RXD[0]==0x41)&&(USART3_RXD[1]==0x02)&&(USART3_RXD[lenth-1]==0x24)  )//如果包头,数据方向为网关或者服务器下行，包尾正确。
			{
				//计算校验和，并比较校验和是否正确。数据方向~备用数据。
				sum_check=sumcheck(USART3_RXD,1,lenth-3);//计算校验和。第二个字节到第倒数第二个字节相加
//				USART1_SEND(USART3_RXD,lenth);//调试用
//				printf("%x\r\n",sum_check);
				if(sum_check==USART3_RXD[lenth-2])//如果校验和正确。
				{//提取数据中的蓝牙基站ID。
//					USART1_SEND(USART3_RXD,lenth);//调试用
//					printf("地址号一致");
					STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,5); //读取网关注册设备数据					
					for(i=0;i<6;i++)
					{
						NODE_ID[i]=USART3_RXD[2+i];
					}
//					USART1_SEND(NODE_ID,10);//调试用
					for(i=0;i<6;i++)//提取蓝牙基站的ID
					{
						test[i]=memory_index[i];
					}
//					USART1_SEND(test,10);//调试用			
					for(i=0;i<6;i++)//比较串口收到的数据中提取的蓝牙基站的ID与蓝牙基站本身存储的ID。如果一致，则返回1.如果不一致，则返回0.
					{
						if(NODE_ID[i]==test[i])
						{
							send_success_flag=1;//先将发送成功标志位置1.
						}
						else
						{
							send_success_flag=0;//如果ID不一致，则置0。
							i=11;//跳出for循环，节约时间。							
						}
					}		
//					if(send_success_flag==1)
//					{
//					printf("地址号一致");
//					}
//					else
//					{
//						printf("地址号不一致");
//					}
				}
				else
				{
					printf("校验和错误\r\n");
				}
			}			
}
unsigned char Wireless_Send_Data(unsigned char *data,unsigned char size)//data是发送的数组。size是数据长度。	
{
//	unsigned char i;	
//	unsigned char test[20];	
	unsigned char Send_OK=0;//如果发送失败,为0。如果发送成功，返回值为1。如果收到了网关传回来的确认帧，返回值为2。
//最多发送三遍，如果还不成功，就启动重启LoRa模块发送模式。	
	USART3_SEND(data,size);//往网关发送报警信息。
//	USART1_SEND(data,size);//往网关发送报警信息。	
	Send_Overtime=250;//设置超时时间为1秒。
	send_success_flag=0;//发送成功标志位情况置零。
	while(Send_Overtime>0)
	{
		if( (Send_Overtime<=80)&&(Send_Overtime>=50)&&(send_success_flag==0) )
		{//用于强制接收用的。因为发现有的时候其实已经发送成功了。但是发送成功标志位却没有被标记，所以导致无法正常接收。
		//现在的办法是直接等待600ms.因为433发送一帧数据的时间也就600ms
			Send_Overtime=0;//超时，跳出。发送失败。
			Send_OK=0;//发送失败
		}
		USART3_COMAND_SCAN();//刷新读取串口3数据。查看是否有数据过来需要处理。查看是否收到了确认帧。
		if(send_success_flag==1)//如果收到网关或者服务器返回来的确认帧。则认为数据发送成功。则不在等待。
		{
			send_success_flag=0;//发送成功标志位情况置零。
			Send_Overtime=0;//发送成功，准备跳出while循环。		
			Send_OK=1;//已经收到网关的确认帧了。		
		}		
	}
	return Send_OK;		
}
void LoRa_433_Pack_DATA(unsigned char *data)//Package_DATA[0]代表数组Package_DATA中元素的个数+1.	数组中有多少组数据的算法就是数组中元素的个数除以6。
{
//举例：	FB 00 00 00 00 14    FB 00 00 00 00 15    FB 00 00 00 00 16    FB 00 00 00 00 17   四个设备离位
//41 01 B2 23 96 60 40 01 06 04 FB 00 00 00 00 14 FB 00 00 00 00 15 FB 00 00 00 00 16 FB 00 00 00 00 17 00 00 59 24  共38个字节
// 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 			
//	/*-------解析---------*/			
//	41（0）	包头
//	01（1）	上行数据		
//	/*-------数据场---------*/
	
//例如：离位基站设备，17年12月22日，蓝牙设备，第1批次，编号为1	
//换算成二进制	
//	B2 23 96 60 40 01	（2~7）设备ID。433通信设备，离位基站，17年11月11日，01批次，00 00 00 AB编号
////设备编号	   		年份	 月份   日期   代号   批次        编号
////0000 0000| 0000 000|0 000|0 0000| 0000| 0000 00|00 0000 0000 0000 
////1011 0010| 0010 001|1 100|1 0110| 0110| 0000 01|00 0000 0000 0001 
////换算成十六进制：B2 23 96 60 40 01	
//	06 （8）部件状态为离位
//	04 （9）部件数量为4
//	FB 00 00 00 00 14 （10~15）部件地址号
//	FB 00 00 00 00 15 （16~21）部件地址号 
//	FB 00 00 00 00 16 （22~27）部件地址号 
//	FB 00 00 00 00 17 （28~33）部件地址号
//	00 00 （34，35）备用数据	
//	/*----------------------*/			
//	59 （36）校验和    01 B2 23 96 60 40 01 06 04 FB 00 00 00 00 14 FB 00 00 00 00 15 FB 00 00 00 00 16 FB 00 00 00 00 17 00 00 相加
//	24 （37）包尾		
	unsigned char i=0;
	unsigned char step=0;
	unsigned char Wireless_SEND_DATA[100]={0x00};
	if(data[1]==0x01)//心跳。
	{
		
		Wireless_SEND_DATA[0]=0x41;//数据包头
		Wireless_SEND_DATA[1]=0x01;//上行数据
		for(i=0;i<6;i++)//ID共6位
		{
			Wireless_SEND_DATA[2+i]=Gateway_ID[i];//将蓝牙基站的ID压入到数组。
		}
	/***********部件状态***********/
		Wireless_SEND_DATA[8]=0x01;//部件状态为心跳，则为（0x01）.
	/*************已经离位的标签的个数。*************/
		Wireless_SEND_DATA[9]=0x01;//参数类型为电压	 		
		Wireless_SEND_DATA[10]=0x01;//参数长度为1	
		Wireless_SEND_DATA[11]=0xFF;//电量为满
		Wireless_SEND_DATA[12]=0xFF;//巡检周期		
		Wireless_SEND_DATA[13]=0xFF;//
		Wireless_SEND_DATA[14]=0x00;//备用数据		
		Wireless_SEND_DATA[15]=0x00;//		
		Wireless_SEND_DATA[16]=sumcheck(Wireless_SEND_DATA,1,18-3);//计算校验和。第二个字节到第倒数第二个字节相加
		Wireless_SEND_DATA[17]=0x24;//包尾
		step=18;
		/*----------------------*/
	}
	else if(data[1]==0x06)//离位/恢复变化。
	{
//41 01 B2 23 96 60 40 01 06 08 02 FB 00 00 00 00 12 02 FB 00 00 00 00 13 02 FB 00 00 00 00 14 02 FB 00 00 00 00 15 02 FB 00 00 00 00 16 02 FB 00 00 00 00 17 02 FB 00 00 00 00 18 02 FB 00 00 00 00 19 01 01 FF FF FF 00 00 
//41 01 B2 23 96 60 40 01 06 08 02 FB 00 00 00 00 12 02 FB 00 00 00 00 13 02 FB 00 00 00 00 14 02 FB 00 00 00 00 15 02 FB 00 00 00 00 16 02 FB 00 00 00 00 17 02 FB 00 00 00 00 18 02 FB 00 00 00 00 19 01 01 FF FF FF 00 00 
//41 01 B2 23 96 60 40 01 06 08 02 FB 00 00 00 00 12 02 FB 00 00 00 00 13 02 FB 00 00 00 00 14 02 FB 00 00 00 00 15 02 FB 00 00 00 00 16 02 FB 00 00 00 00 17 02 FB 00 00 00 00 18 02 FB 00 00 00 00 19 01 01 FF FF FF 00 00 		
//		
//		
//41 01 B2 23 96 60 40 01 06 08 02 FB 00 00 00 00 12 02 FB 00 00 00 00 13 02 FB 00 00 00 00 14 02 FB 00 00 00 00 15 02 FB 00 00 00 00 16 02 FB 00 00 00 00 17 02 FB 00 00 00 00 18 02 FB 00 00 00 00 19 01 01 FF FF FF 00 00 
//41 01 B2 23 96 60 40 01 06 08 02 FB 00 00 00 00 12 02 FB 00 00 00 00 13 02 FB 00 00 00 00 14 02 FB 00 00 00 00 15 02 FB 00 00 00 00 16 02 FB 00 00 00 00 17 02 FB 00 00 00 00 18 02 FB 00 00 00 00 19 01 01 FF FF FF 00 00 
//41 01 B2 23 96 60 40 01 06 08 02 FB 00 00 00 00 12 02 FB 00 00 00 00 13 02 FB 00 00 00 00 14 02 FB 00 00 00 00 15 02 FB 00 00 00 00 16 02 FB 00 00 00 00 17 02 FB 00 00 00 00 18 02 FB 00 00 00 00 19 01 01 FF FF FF 00 00 		

//41 01 B2 23 96 60 40 01 06 08 02 FB 00 00 00 00 12 02 FB 00 00 00 00 13 02 FB 00 00 00 00 14 02 FB 00 00 00 00 15 02 FB 00 00 00 00 16 02 FB 00 00 00 00 17 02 FB 00 00 00 00 18 02 FB 00 00 00 00 19 01 01 FF FF FF 00 00 AE 24 
//41 01 B2 23 96 60 40 01 06 08 02 FB 00 00 00 00 12 02 FB 00 00 00 00 13 02 FB 00 00 00 00 14 02 FB 00 00 00 00 15 02 FB 00 00 00 00 16 02 FB 00 00 00 00 17 02 FB 00 00 00 00 18 02 FB 00 00 00 00 19 01 01 FF FF FF 00 00 AE 24 
//41 01 B2 23 96 60 40 01 06 08 02 FB 00 00 00 00 12 02 FB 00 00 00 00 13 02 FB 00 00 00 00 14 02 FB 00 00 00 00 15 02 FB 00 00 00 00 16 02 FB 00 00 00 00 17 02 FB 00 00 00 00 18 02 FB 00 00 00 00 19 01 01 FF FF FF 00 00 AE 24 		

		Wireless_SEND_DATA[0]=0x41;//数据包头
		Wireless_SEND_DATA[1]=0x01;//上行数据
		for(i=0;i<6;i++)//ID共6位
		{
			Wireless_SEND_DATA[2+i]=Gateway_ID[i];//将蓝牙基站的ID压入到数组。
		}
	/***********部件状态***********/
		Wireless_SEND_DATA[8]=0x06;//部件状态为报警（发生离位或者恢复），则为（0x06）.
	/*************已经离位的标签的个数。*************/
//		data[28]=(message[0]-3)/7;//信息对象数目		
		Wireless_SEND_DATA[9]=(data[0]-3)/7;//用数组的元素个数除以每个ID的长度6.	
	/***********离位标签的ID***********/
		for(i=0;i<data[0]-3;i++)
		{		
			Wireless_SEND_DATA[10+i]=data[i+4];
		}
		step=10+data[0]-3;
	/***********备用数据位***********/	 	
		Wireless_SEND_DATA[step]=0x01;//参数类型为电压	
		step=step+1;		
		Wireless_SEND_DATA[step]=0x01;//参数长度为1
		step=step+1;
		Wireless_SEND_DATA[step]=0xFF;//电量为满
		step=step+1;		
		Wireless_SEND_DATA[step]=0xFF;//巡检周期	
		step=step+1;		
		Wireless_SEND_DATA[step]=0xFF;//
		step=step+1;		
		Wireless_SEND_DATA[step]=0x00;//备用数据		
		step=step+1;		
		Wireless_SEND_DATA[step]=0x00;//		
		step=step+1;		
		Wireless_SEND_DATA[step]=sumcheck(Wireless_SEND_DATA,1,step-1);//计算校验和。第二个字节到第倒数第二个字节相加
		step=step+1;			
		Wireless_SEND_DATA[step]=0x24;//包尾
		step=step+1;			
	}
/*********************开始重发**********************/	
//重发第一次
	i=Wireless_Send_Data(Wireless_SEND_DATA,step);//data是发送的数组。size是数据长度。	
//重发第二次
	if(i==0)
	{
		i=Wireless_Send_Data(Wireless_SEND_DATA,step);//data是发送的数组。size是数据长度。
	}
//重发第三次
	if(i==0)
	{
		i=Wireless_Send_Data(Wireless_SEND_DATA,step);//data是发送的数组。size是数据长度。
	}
//	USART1_SEND(Wireless_SEND_DATA,14+data[0]);//调试用
}
