#include "sys.h"
#include "usart.h"
#include "usart3.h"
#include "uart4.h"
#include "stmflash.h"
#include "key.h"
#include "led.h"
#include "stmflash.h"
#include "buzzer.h"
#include "function.h"
#include <math.h>
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 
/***************************串口结合定时器接收数据***************************/
unsigned char USART1_RX_FLAG=0;         //置标志位，告诉定时器开始收数了,也说明串口正在收数据，不要做其他操作打断。
unsigned char USART1_RX_TIME=0;       	//接收字符串时的时间间隔
unsigned int 	USART1_RX_NO=0;          	//接收字符串字符的总数量
unsigned char USART1_GROUP=0;   				//最大接收缓存数组个数	
unsigned char USART1_RX_BUF[10][100]={0x00}; 	//接收缓冲二维数组。
unsigned char USART1_RX_BUF_middle[100]={0x00}; 	//接收缓冲二维数组。
unsigned char USART1_finger_point=0;    //数组消息循环中指针指向的位置。
/***************************************************************************/
void USART1_Init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入GPIO_Mode_IN_FLOATING
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10   
	
   //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 \\
	
  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}
//串口1,输出函数
void USART1_SEND(unsigned char* USART1_TX_BUF,unsigned int size) // USART1_TX_BUF代表要输出的数组。size代表输出数组的长度。 
{
	unsigned int i; 
	for(i=0;i<size;i++)							//循环发送数据
	{
//	  while(USART_GetFlagStatus(UART4,UART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(USART1,USART1_TX_BUF[i]); 
	} 
}
//通过判断接收连续2个字符之间的时间差不大于20ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//USART_RX_BUF_middle[0]代表数组中元素的数量。例如：0x05 0x01 0x02 0x03 0x04 0x05
void USART1_IRQHandler(void)
{
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
/******************************************************************************************************************/	
			USART1_RX_FLAG=1;//置标志位，告诉定时器可以开始为我工作了。时间间隔判断一组数据是否收完。
			USART1_RX_TIME=5;//重新装初值。开始倒计时，用来判断是否接受完了一组数据。5次进入定时器中断。
			USART1_RX_BUF_middle[USART1_RX_NO+1] = USART_ReceiveData(USART1);//读取接收到的数据		
			if(USART1_RX_NO<98)
			{
				USART1_RX_NO++;//每组串口数据收到的数据个数累加.不得超过数组定义的数量。		
			}
			else
			{//按照溢出处理
				USART1_RX_NO=98;
			}
			USART1_RX_BUF_middle[0]=USART1_RX_NO;
/******************************************************************************************************************/			
    }
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif        
}
void USART1_RX_SCAN(void)//定时器定时扫描串口标志位的变化，及时的进行处理。
{
	unsigned char i=0;
	if(USART1_RX_FLAG==1)//判断是否正在接收串口数据
	{
		USART1_RX_TIME--;//将倒计时寄存器内的数值减一。
		if(USART1_RX_TIME<=1)//说明一串数中两个数据的间隔超过了设定的时间了，判定数据接收完毕。
		{//当接收的字节之间的时间超过接收3~5个字节的时间的时候则认为本次接收完成，接收完成标志置1.
			USART1_RX_FLAG=0;//将串口开始计时标志位数据清零	
			USART1_RX_TIME=5;//将USART_RX_TIME的值填满，防止在减1的时候出现负值。
			USART1_RX_NO=0;//串口接收数据长度清理。			
/*******************压栈数据包数组*********************/	
			//如果这个位置的元素数量不为0，则向前走一格。
			for(i=0;i<USART1_RX_BUF_middle[0]+1;i++)//压栈数据
			{
				USART1_RX_BUF[USART1_GROUP][i]=USART1_RX_BUF_middle[i];
			}
			if(USART1_GROUP>=9)//群组个数不得超过定义的数组的长度。循环填充。0~19~0~19
			{
				USART1_GROUP=0;//群组数量
			}
			else
			{//按照溢出处理
				USART1_GROUP++;//群组的个数加1
			}
            if(USART1_RX_BUF[USART1_GROUP][0]!=0x00)//没来得及处理的数据已经形成一个环了，需要将最开始的一帧数据清掉了。
			{
				USART1_finger_point=USART1_GROUP;//将指向指针的位置与 USART1_finger_point 一样一样。（不管此时finger_point的位置！）
			}
		}
	}		
}
void USART1_COMAND_SCAN(void)//刷新读取串口1数据。查看是否有命令过来需要处理。 
{
	unsigned int i=0;
	unsigned int lenth=0;//串口收完一组数据的长度。
	unsigned char USART1_RXD[100]={0x00};//定义发送数组			
	if( ( (USART1_RX_BUF[USART1_finger_point][0]!=0x00)&&(USART1_RX_FLAG==0) )==1 )//如果发现串口收数数组中有数据了，并且串口正闲置。
	{
		//每次都是将第一组的数据提取出来。因为先进先出原则。FIFO。
		for(i=0;i<USART1_RX_BUF[USART1_finger_point][0];i++)//USART1_RX_BUF[USART1_GROUP][0]是USART1_RX_BUF[j][i]中数据的长度。取出，使用。如果有7个数，则USART1_RX_BUF[USART1_GROUP][0]=7.
		{
			USART1_RXD[i]=USART1_RX_BUF[USART1_finger_point][i+1];//	将即将要处理的数据取出来。	
			USART1_RXD[i+1]=0x00;//添加结束符
		}
		lenth=USART1_RX_BUF[USART1_finger_point][0];//这个数组中元素的个数。
		USART1_RX_BUF[USART1_finger_point][0]=0x00;//清空这个数组中元素的个数。
		if(USART1_finger_point>=9)//群组个数不得超过定义的数组的长度。循环填充。0~9~0~9
		{
			USART1_finger_point=0;//群组的指针变到头位置。
		}
		else
		{
			USART1_finger_point++;//群组的指针加1。
		}
//		printf("%s\r\n",USART1_RXD);//调试用，将刚才收到的数据打印出来。
//		USART1_SEND(USART1_RXD,lenth);//调试用，将刚才收到的数据打印出来。
/************************************************************************************/	
		if((USART1_RXD[0]==0x42)&&(USART1_RXD[1]==0x54)&&(USART1_RXD[2]==0x2B)&&(USART1_RXD[lenth-2]==0x0D)&&(USART1_RXD[lenth-1]==0x0A))//BT+开头，0x0D和0x0A结尾。
		{
			Buzzer_flag=2;
			USART1_Data_analyze(USART1_RXD,lenth);//串口1收数数据解析.USART1_RXD是数据数组，lenth是数据长度。	
		}	
	}					
}
unsigned char memory_index[200]={0x00};//读取数据索引。0~5设备ID,6~7回路号，8~9部件号，10~11存储的节点数量。
void USART1_Data_analyze(unsigned char *USART1_RXD,unsigned char lenth)//串口1收数数据解析.USART1_RXD是数据数组，lenth是数据长度。
{
//外网调试的地址 124.192.224.226 端口5005	
	unsigned int i=0;
//	unsigned char data_lenth=0;	
	unsigned char NODE_ID_u8[30]={0x00};
//	unsigned char NODE_ID_u8_middle[30]={0x00};
//	unsigned short NODE_ID_u16[30]={0x00};		
	unsigned char printf_data[100]={0x00};//测试打印用的数组	
//	unsigned char SERVER_ID[50]={0};//服务器IP
//	unsigned char PORT_ID[20]={0};	//服务器端口号
	u32 time_save=0;
	unsigned char time_search=0;	
	unsigned char command_lenth=0;//命令字节长度。	
	unsigned char command[30]={0x00};//命令字节。	
	unsigned char data_group[30]={0x00};//数据数组。	
	unsigned char data_group_lenth=0;		
	unsigned int data_group_int=0;//数组变int类型数据。	
//命令代码表，二维数组中第一框是数组数量，第二框是数组中元素的数量。
	unsigned char OrderCode[10][20]=
	{
	  "ROUTER_ID",			//蓝牙基站ID。
		"MARK_ID",				//添加标签ID。
		"ERASURE_ID",			//删除单个ID。
		"ERASURE_ALL",		//删除所有蓝牙标签ID
		"SERVER_ID",			//设置服务器IP
		"PORT_ID",				//设置服务器端口号
		"HEART_TIME",			//设置蓝牙基站心跳时间间隔	memory_index[40]
		"MARK_LEAVE_TIME",//设置离位标签离位时间			memory_index[50]
		"ROUTER_SEARCH_TIME",//基站巡检时间间隔。//1~99秒。
		"READ_CONFIGURE",	//读取当前配置		
	};
//以下所有命令在串口助手中发送的时候必须勾选回车换行。
//BT+ROUTER_ID=B22396604001    设置蓝牙基站ID。
//BT+MARK_ID=FB0000000140    添加标签ID。
//BT+ERASURE_ID=FB0000000141   删除单个ID。
//BT+ERASURE_ALL=1      删除所有蓝牙标签ID。
//BT+SERVER_ID=192.168.1.1  设置服务器IP
//BT+PORT_ID=10086		设置服务器端口号
//BT+HEART_TIME=02  设置蓝牙基站心跳时间间隔
//BT+MARK_LEAVE_TIME=02	 设置离位标签离位时间	
//BT+ROUTER_SEARCH_TIME=10 设置巡检时间间隔为10秒	
//BT+READ_CONFIGURE=1	 读取当前配置	
	
//提取出命令来	
//	举例：
//	BT+LOCAL_ID=123456
//	 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17
//	42 54 2B 4C 4F 43 41 4C 5F 49 44 3D 31 32 33 34 35 36 
//	 B  T  +  L  O  C  A  L  _  I  D  =  1  2  3  4  5  6
		for(i=0;i<30;i++)//提取命令格式。
		{//命令格式最长为30个字节。
			if(USART1_RXD[i+3]==0x3D)//如果遇到等于号，说明命令字节已经到达尾端。跳出for循环。
			{
				i=40;//跳出for。
			}
			else
			{
				command[i]=USART1_RXD[i+3];//将BT+过滤掉。
				command_lenth=i+1;
			}
		}
//USART1_SEND(command,command_lenth);
//printf("\r\n");			

//设置蓝牙基站ID。
//添加标签ID。
//删除单个ID。
//删除所有蓝牙标签ID
//设置服务器IP
//设置服务器端口号
//设置蓝牙基站心跳时间间隔	memory_index[40]
//设置离位标签离位时间			memory_index[50]
//读取当前配置				
			
		//提取命令中等号后面的数据。
		for(i=0;i<30;i++)//数据最长为30个字节。
		{
			if( (USART1_RXD[i+command_lenth+4]==0x0D)&&(USART1_RXD[i+command_lenth+5]==0x0A) )//如果遇到等回车换行，说明数据已经到了尾端。跳出for循环。
			{
				i=40;//跳出for。
			}
			else
			{
				data_group[i]=USART1_RXD[i+command_lenth+4];
				data_group_lenth=i+1;
			}
		}			
//USART1_SEND(data_group,data_group_lenth);		
//printf("\r\n");
		if(Sting_Compare(OrderCode[0],command,command_lenth))//设置蓝牙基站ID。
		{
			if(data_group_lenth==12)
			{
				DATA_change(data_group,12,Gateway_ID); //data是准备转换的数组。size是转换数组中元素的数量，（size必须为偶数）。 target_data是数据转换后存放的数组。
				STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,5); // 读取已注册设备数据
				for(i=0;i<6;i++)//将网关ID填写进网关的eeprom中
				{
					memory_index[i]=Gateway_ID[i];
				}
				STMFLASH_Write(NUMBER_ADDRESS,(u16*)memory_index,5);//将变动后的数据索引重新写入eeprom		
	/**************测试用**************/
				handle_arrays_u8(memory_index,6,printf_data);
				printf("\r\n蓝牙基站ID写入成功。ID： ");	
				USART1_SEND(printf_data,12);
				printf("\r\n");					
			}
			else
			{
				printf("蓝牙基站ID数据长度有误，请重新输入。\r\n");	
			}
		}
		else if(Sting_Compare(OrderCode[1],command,command_lenth))//添加标签ID。
		{
			if(data_group_lenth==12)
			{
				DATA_change(data_group,12,NODE_ID_u8);//转换数据。
				DATA_change_u8_to_u16(NODE_ID_u8,12,judge_data_u16);//将6个8位的元素数组转换成3个16位元素的数组。				
				Judge_ID(judge_data_u16);//判断合法性。是否已经在EEPROM中有存储注册,如果返回数据中的NODE_ID[6]和NODE_ID[7]都是非0x00的值，则是已经注册设备所在的位置。如果返回NODE_ID[6]和NODE_ID[7]都为0x00，则是未注册设备。	
				if(judge_data_u16[4]==0x0000)//如果节点没有被注册
				{
	//				Buzzer_flag=1;
					if(Bluetooh_amount>=Bluetooh_Max_NO)
					{
						printf("基站中蓝牙标签已经存满%d个了\r\n",Bluetooh_amount);
					}
					else
					{
						//将蓝牙标签ID进行存储。
						DATA_change_u16_to_u8(judge_data_u16,6,NODE_ID_u8);
						i=Save_NODE_ID(NODE_ID_u8);//NODE_ID_u8的长度为10位，（0~5）位为设备ID，（6~7）为需要保存的位置，（8~9）备用。					
						if(i==0xFFFF)//保存设备ID失败
						{
							printf("保存蓝牙标签ID失败\r\n");
						}
						else//保存设备ID成功。
						{//打印设备注册成功
	//						Buzzer_flag=4;//蜂鸣器长响一下。
							printf("本次插入到存储矩阵中的位置为： %d\r\n",i);		
							printf("蓝牙基站中注册的ID总数为： %d\r\n",Bluetooh_amount);							
							printf("蓝牙标签注册成功。ID：");
							handle_arrays_u8(NODE_ID_u8,6,printf_data);
							USART1_SEND(printf_data,12);	
							printf("\r\n");												
						}					
					}
				}
				else//设备已经被注册过。0x0000是一个不可能的值。如果被搜索到，说明设备没有离位，则清空这个设备的所有标志位，进入下一轮的搜索环节。
				{
						printf("蓝牙标签已被注册。ID：");
						handle_arrays_u8(NODE_ID_u8,6,printf_data);
						USART1_SEND(printf_data,12);	
						printf("\r\n");			
				}				
			}
			else
			{
				printf("蓝牙标签ID数据长度有误，请重新输入。\r\n");	
			}			
		}
		else if(Sting_Compare(OrderCode[2],command,command_lenth))//删除单个ID。
		{
			if(data_group_lenth==12)
			{
				DATA_change(data_group,12,NODE_ID_u8);//转换数据。
				DATA_change_u8_to_u16(NODE_ID_u8,12,judge_data_u16);//将6个8位的元素数组转换成3个16位元素的数组。				
				Judge_ID(judge_data_u16);//判断合法性。是否已经在EEPROM中有存储注册,如果返回数据中的NODE_ID[6]和NODE_ID[7]都是非0x00的值，则是已经注册设备所在的位置。如果返回NODE_ID[6]和NODE_ID[7]都为0x00，则是未注册设备。	
				if(judge_data_u16[4]==0x0000)//如果节点没有被注册
				{
					printf("蓝牙离位基站中无此ID，不需要删除。\r\n");					
				}
				else//设备已经被注册。则查看是巡检信息还是报警信息。
				{	
	///************删ID用************/		
					DATA_change_u16_to_u8(judge_data_u16,4,NODE_ID_u8); //将一个16位数据转换成两个8位数据。
					Delete_NODE_ID(NODE_ID_u8);//删除ID。
					handle_arrays_u8(NODE_ID_u8,6,printf_data);//测试用。
					printf("成功删除蓝牙离位基站中设备ID: ");	
					USART1_SEND(printf_data,12);
	//				printf(" 删除成功。\r\n");					
					printf("\r\n");					
				}				
			}
			else
			{
				printf("蓝牙标签ID数据长度有误，请重新输入。\r\n");	
			}			
		}	
		else if(Sting_Compare(OrderCode[3],command,command_lenth))//删除所有蓝牙标签ID
		{
			if( (data_group_lenth==1)&&(data_group[0]==0x31) )
//			if( (data_group_lenth==1) )				
			{
	//	/*-------解析---------*/
	//	42 54 2B 45 52 41 53 55 52 45 5F 49 44 3D			包头  BT+ERASURE_ID=  			 14个字节
	//	41 4C 4C		删除的属性，这里是全部删除。ALL	 12个字节						
				STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,100); // 读取已注册设备数据		
				Bluetooh_amount=0;	
				memory_index[6]=0x00;		
				memory_index[7]=0x00;				
				STMFLASH_Write(NUMBER_ADDRESS,(u16*)memory_index,100);//将变动后的数据索引重新写入eeprom		
				printf("所有ID已经被删除\r\n");					
			}
			else
			{
				printf("删除命令有误，请重新输入。\r\n");					
			}				
		}	
		else if(Sting_Compare(OrderCode[4],command,command_lenth))//设置服务器IP
		{
			printf("\r\n服务器IP设置成功，服务器IP： ");
			USART1_SEND(data_group,data_group_lenth);						
			printf("\r\n");	
			STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,100); // 读取已注册设备数据
			for(i=0;i<data_group_lenth;i++)//将网关ID填写进网关的eeprom中
			{
				data_group[data_group_lenth-i]=data_group[data_group_lenth-i-1];
			}	
			data_group[0]=data_group_lenth;//服务器ID的长度
			for(i=0;i<data_group[0]+1;i++)//将网关ID填写进网关的eeprom中
			{
				memory_index[11+i]=data_group[i];
				memory_index[10]=0xAA;//标志位置1，表示已经设置过服务器ID，如果没设置过，则不能随意给GPRS写ID。
			}
			STMFLASH_Write(NUMBER_ADDRESS,(u16*)memory_index,100);//将变动后的数据索引重新写入eeprom							
		}			
		else if(Sting_Compare(OrderCode[5],command,command_lenth))//设置服务器端口号
		{//31,32
			if(data_group_lenth<=5)
			{
				printf("\r\n服务器端口设置成功，服务器端口： ");	
				USART1_SEND(data_group,data_group_lenth);						
				printf("\r\n");				
				STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,100); // 读取已注册设备数据
				memory_index[30]=0xAA;//标志位置位。			
				data_group_int=str_int(data_group);
				memory_index[31]=data_group_int/256;
				memory_index[32]=data_group_int%256;
				STMFLASH_Write(NUMBER_ADDRESS,(u16*)memory_index,100);//写入变动后的数据。						
			}
			else
			{
				printf("服务器端口长度有误，请检查重新输入。\r\n");							
			}
		}
		else if( (Sting_Compare(OrderCode[6],command,command_lenth))&&(data_group[data_group_lenth-1]!=0x20) )//设置蓝牙基站心跳时间间隔.且最后一位不得等于空格
		{
            if((data_group_lenth>0)&&(data_group_lenth<6))
            {
                time_save=0;
                for(i=0;i<data_group_lenth;i++)
                {
                    time_save+=(data_group[i]-0x30)*pow(10,data_group_lenth-i-1);
                }
            }
            
//			if(data_group_lenth==1)		 //数据长度为一位
//			{
//				time_save=data_group[0]-0x30;
//			}
//			else if(data_group_lenth==2)//数据长度为二位
//			{
//				time_save=(data_group[0]-0x30)*10+(data_group[1]-0x30);
//			}
//			else if(data_group_lenth==3)//数据长度为三位
//			{
//				time_save=(data_group[0]-0x30)*100+(data_group[1]-0x30)*10+(data_group[2]-0x30);				
//			}
//            else if(data_group_lenth==4)//数据长度为四位
//			{
//				time_save=(data_group[0]-0x30)*1000+(data_group[1]-0x30);
//			}
//			else if(data_group_lenth==5)//数据长度为五位
//			{
//				time_save=(data_group[0]-0x30)*10000+(data_group[1]-0x30)*10+(data_group[2]-0x30);				
//			}
			else
			{
//				printf("蓝牙基站心跳时间输入错误，请检查后重新输入。\r\n");
				time_save=255;
			}
			printf("蓝牙基站心跳时间间隔： %d 分钟\r\n",time_save);			
			STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,100); // 读取已注册设备数据
			memory_index[40]=time_save&0x000000FF;
            memory_index[41]=(time_save&0x0000FF00)>>8;
            memory_index[42]=(time_save&0x00FF0000)>>16;
            memory_index[43]=(time_save&0xFF000000)>>24;
			STMFLASH_Write(NUMBER_ADDRESS,(u16*)memory_index,100);//将变动后的数据索引重新写入eeprom						
		}	
		else if(Sting_Compare(OrderCode[7],command,command_lenth))//设置离位标签离位时间
		{
            if((data_group_lenth>0)&&(data_group_lenth<6))
            {
                time_save=0;
                for(i=0;i<data_group_lenth;i++)
                {
                    time_save+=(data_group[i]-0x30)*pow(10,data_group_lenth-i-1);
                }
            }
//			if(data_group_lenth==1)		 //数据长度为一位
//			{
//				time_save=data_group[0]-0x30;
//			}
//			else if(data_group_lenth==2)//数据长度为二位
//			{
//				time_save=(data_group[0]-0x30)*10+(data_group[1]-0x30);
//			}
//			else if(data_group_lenth==3)//数据长度为三位
//			{
//				time_save=(data_group[0]-0x30)*100+(data_group[1]-0x30)*10+(data_group[2]-0x30);				
//			}
			else
			{
				time_save=255;
			}
			printf("判定蓝牙离位标签离位时间间隔： %d 分钟\r\n",time_save);			

			STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,100); // 读取已注册设备数据
			memory_index[50]=time_save&0x000000FF;
            memory_index[51]=(time_save&0x0000FF00)>>8;
            memory_index[52]=(time_save&0x00FF0000)>>16;
            memory_index[53]=(time_save&0xFF000000)>>24;
			STMFLASH_Write(NUMBER_ADDRESS,(u16*)memory_index,100);//将变动后的数据索引重新写入eeprom						
/****************************/			
//			Star_Read_ID();//读取并打印设备ID .更新全局变量。				
		}
		else if(Sting_Compare(OrderCode[8],command,command_lenth))//设置基站巡检时间间隔。//1~99秒。
		{	
			if(data_group_lenth==1)		 //数据长度为一位
			{
				time_search=data_group[0]-0x30;
			}
			else if(data_group_lenth==2)//数据长度为二位
			{
				time_search=(data_group[0]-0x30)*10+(data_group[1]-0x30);
			}
			else if(data_group_lenth==3)//数据长度为三位
			{
				time_search=(data_group[0]-0x30)*100+(data_group[1]-0x30)*10+(data_group[2]-0x30);				
			}
			else
			{
				time_search=255;
			}
			printf("蓝牙离位基站搜索标签的时间间隔： %d 秒\r\n",time_search);			

			STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,100); // 读取已注册设备数据
			memory_index[60]=time_search;
			bluetooh_scan_heart_time=time_search;
			STMFLASH_Write(NUMBER_ADDRESS,(u16*)memory_index,100);//将变动后的数据索引重新写入eeprom						
/****************************/			
//			Star_Read_ID();//读取并打印设备ID .更新全局变量。		
		}
		else if(Sting_Compare(OrderCode[9],command,command_lenth))//读取并打印当前存储中的数据
		{
			if( (data_group_lenth==1)&&(data_group[0]==0x31) )		
			{
				Star_Read_ID();//读取并打印设备ID .更新全局变量。					
			}
			else
			{
				printf("查看命令有误，请重新输入。\r\n");					
			}
		}	
		Refurbish_Star_Read_ID();	//刷新全局的标志位。		
}























