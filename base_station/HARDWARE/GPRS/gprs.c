#include "gprs.h"
#include "usart.h"
#include "usart2.h"
#include "delay.h"
#include "string.h" 
#include "buzzer.h"
#include "usart3.h"	  
#include "stmflash.h"
#include "iwdg.h"
#include "bc95.h"
#define GPRS_ST_Read() GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)
#define GPRS_LINKA_Read() GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6)
//unsigned char GPRS_COMMAND_MODE_flag=0;//工作模式，如果等于1，则工作在命令模式下，不进行
unsigned int GM3_LINKA_TIME_LOSE=0;//GPRS掉线时间。如果超过这个时间，则需要重启GPRS模块了。硬件检测LINK引脚，因为需要修改测试，所以做成全局变量。相当于宏定义。
unsigned char sendata_flag=0;
unsigned int GM3_LINKA_TIME=0;//GPRS掉线时间。如果超过这个时间，则需要重启GPRS模块了。
////		send_IP_port(data1,data2);	
void send_IP_port(u8 *IP,u8 *port)
{
	unsigned char i=0;
	unsigned char t=0;	
	unsigned char data[80]={0};
	for(i=0;i<80;i++)
	{
		data[i]=0;
	}
	data[0]=0x41;
	data[1]=0x54;
	data[2]=0x2B;
	data[3]=0x53;
	data[4]=0x4F;
	data[5]=0x43;
	data[6]=0x4B;
	data[7]=0x41;
	data[8]=0x3D;
	data[9]=0x22;
	data[10]=0x54;
	data[11]=0x43;
	data[12]=0x50;
	data[13]=0x22;
	data[14]=0x2C;
	data[15]=0x22;
	for(i=0;i<IP[0];i++)
	{
		data[16+i]=IP[i+1];		
	}
	t=16+i;
	data[t]=0x22;	
	t=t+1;	
	data[t]=0x2C;	
	t=t+1;		
	for(i=0;i<port[0];i++)
	{
		data[t+i]=port[i+1];
	}
	t=t+i;
	data[t]=0x0D;
	t=t+1;		
	data[t]=0x0A;	
	t=t+1;			
	GPRS_senddata(data,t);//USART2_SEND(data,t);		
//	u2_printf("%s",data);//发送命令\r\n	
//	printf("%s",data);//发送命令	
}	
void GPRS_RESTAR(void)//GPRS重启。
{
	/*unsigned short time_delay=5000;
	unsigned short time_delay_OK=0;	
	GM3_LINKA_TIME=GM3_LINKA_TIME_LOSE;//GPRS在线时间填充满。
	Buzzer_flag=4;	
//	GPRS_REST_ENABLE;		//GPRS复位
	GPRS_SOFT_POWER_DISABLE;//GM3软关机	
	Watchdog_FEED();//喂狗	
	delay_ms(500);
	Watchdog_FEED();//喂狗（独立看门狗）		
	delay_ms(500);
	Watchdog_FEED();//喂狗（独立看门狗）		
	delay_ms(500);
	Watchdog_FEED();//喂狗（独立看门狗）		
	delay_ms(500);			
	Watchdog_FEED();//喂狗（独立看门狗）			
	GPRS_MOS_POWER_DISABLE;	//GPRS电源断开	
	delay_ms(500);
	Watchdog_FEED();//喂狗（独立看门狗）	
	delay_ms(500);
	Watchdog_FEED();//喂狗（独立看门狗）		
	GPRS_REST_DISABLE;		//GPRS不复位		
	GPRS_SOFT_POWER_ENABLE;//GM3软开机		
	delay_ms(500);	
	Watchdog_FEED();//喂狗（独立看门狗）		
	GPRS_MOS_POWER_ENABLE;		//GPRS电源接通	
	time_delay=5000;		
	while(time_delay)//GM3的WORK引脚。模块正常启动后， 电平一秒变化一次， 高低电平交替变换， 指示灯呈闪烁状态。
	{
		Watchdog_FEED();//喂狗（独立看门狗）	
		time_delay=time_delay-1;
		delay_ms(1);
		if(READ_GPRS_POWER_STATUS==0)//检测WORK引脚电平状态。高低电平交替出现。
		{//检测高电平维持的时间。超过一定时间，就认为是启动成功了。
			Watchdog_FEED();//喂狗（独立看门狗）			
			time_delay_OK=time_delay_OK+1;
			if(time_delay_OK>=800)//说明模块已经正常开机。
			{
				time_delay=0;//退出等待。
			}
		}
	}*/
}


void GPRS_Init(void)
{
	unsigned char i=0;	
//	unsigned int  port=0;	
	unsigned int  data_long=0;	
	unsigned char SERVER_ID[50]={0x00};//服务器IP
	unsigned char PORT_ID[20]={0x00};	//服务器端口号	
	unsigned char PORT_ID_MIDDLE[20]={0x00};	//服务器端口号			
	unsigned char test[10]={0x00};		
	unsigned char memory_index[100]={0x00};//读取数据索引。0~5设备ID,6~7回路号，8~9部件号，10~11存储的节点数量。	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				//MOS_PWR
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				//GM3软关机
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				//READ_GPRS_POWER_STATUS
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				//Reset
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				//LINKA  高电平，连接网络成功。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//下拉
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ;//浮空输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);

//	printf("正在初始化GM3模块。。。\r\n");
//	u3_printf("正在初始化GM3模块。。。\r\n");
	GPRS_RESTAR();//GPRS重启。
	STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,50); // 读取已注册设备数据
	if( (memory_index[10]==0xAA)&&(memory_index[30]==0xAA) )//查看IP和端口是否有存储。
	{
		/*sendata_flag=1;
		//进入AT命令模式。
		gm3_send_cmd("+++","a",800);//握手 	
		delay_ms(500);
		sendata_flag=1;		
		gm3_send_cmd("a","+ok",800);//
		delay_ms(500);*/
/******************GM3服务器IP******************/		
		/*for(i=0;i<memory_index[11]+1;i++)//提取服务器IP
		{
			SERVER_ID[i]=memory_index[i+11];//将蓝牙基站服务器IP提取出来。服务器IP的第一个字节代表长度。
		}*/
/******************GM3服务器测试端口号******************/	
		/*for(i=0;i<2;i++)//提取服务器端口
		{
			PORT_ID_MIDDLE[i]=memory_index[i+31];//将蓝牙基站服务器端口提取出来，服务器端口的第一个字节代表长度。
		}		
		data_long=PORT_ID_MIDDLE[0]*256+PORT_ID_MIDDLE[1];
		test[0]=(data_long/10000)+0x30;// 取整数部分
		data_long=data_long%10000;	 // 取余数
		test[1]=(data_long/1000)+0x30;
		data_long=data_long%1000;	
		test[2]=(data_long/100)+0x30;
		data_long=data_long%100;		
		test[3]=(data_long/10)+0x30;		
		data_long=data_long%10;		
		test[4]=data_long+0x30;	
		
		if( test[0]!=0x30 )
		{//端口号是5位数。例如：10086.
			PORT_ID[0]=5;			
			PORT_ID[1]=test[0];
			PORT_ID[2]=test[1];			
			PORT_ID[3]=test[2];			
			PORT_ID[4]=test[3];			
			PORT_ID[5]=test[4];				
		}		
		else if( (test[0]==0x30)&&(test[1]!=0x30) )
		{//端口号是4位数。例如：01234.
			PORT_ID[0]=4;			
			PORT_ID[1]=test[1];
			PORT_ID[2]=test[2];			
			PORT_ID[3]=test[3];			
			PORT_ID[4]=test[4];			
		}
		else if( (test[0]==0x30)&&(test[1]==0x30)&&(test[2]!=0x30) )
		{
			PORT_ID[0]=3;			
			PORT_ID[1]=test[2];
			PORT_ID[2]=test[3];			
			PORT_ID[3]=test[4];						
		}
		send_IP_port(SERVER_ID,PORT_ID);		
		gm3_send_cmd("AT+WKMOD=\"NET\"","OK",800);//
		delay_ms(500);
		gm3_send_cmd("AT+SOCKAEN=\"on\"","OK",800);//
		delay_ms(500);
//AT+SOCKA="TCP","dtu.fubangyun.cn",9123
		
//OK
		send_IP_port(SERVER_ID,PORT_ID);
		delay_ms(500);
		gm3_send_cmd("AT+SOCKASL=\"long\"","OK",800);//		
		delay_ms(500);
		gm3_send_cmd("AT+SOCKBEN=\"off\"","OK",800);//	
		delay_ms(500);
		gm3_send_cmd("AT+HEARTEN=\"off\"","OK",800);//	
		delay_ms(500);
		gm3_send_cmd("AT+REGEN=\"off\"","OK",800);//	
		delay_ms(500);
		gm3_send_cmd("AT+UART=115200,\"NONE\",8,1,\"RS485\"","OK",800);//			
		delay_ms(500);
		gm3_send_cmd("AT+APN=\"CMNET\","",""","OK",800);//	
		delay_ms(500);
		gm3_send_cmd("AT+S","OK",500);//保存并重启	*/	
	}
	else
	{
		printf("查看IP和端口可能没有存储\r\n");
	}
	
}
//SIM800C发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 gm3_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	unsigned char USART2_RXD[200]={0x00};//定义发送数组	
	unsigned short i=0;
	unsigned short lenth=0;
	char res=0;
	unsigned char break_flag=1;//跳出循环标志位	
	if(sendata_flag==1)
	{
		sendata_flag=0;
		u2_printf("%s",cmd);//发送命令\r\n		
	}
	else
	{
		u2_printf("%s\r\n",cmd);//发送命令		
	}
	if(ack&&waittime)		//需要等待应答
	{
		while( (break_flag)&&(--waittime) )	//等待倒计时
		{
			delay_ms(1);
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
		/************************************************************************************/		
//				USART1_SEND(USART2_RXD,lenth);//串口2收数数据解析.USART2_RXD是数据数组，lenth是数据长度。		
				if(gm3_check_cmd(ack,USART2_RXD))
				{
					res=1;//得到有效数据					
				}
				break_flag=0;//用于跳出while循环。
			}									
		}
		if(waittime==0)
		{
			res=0; 
		}
	}
	return res;
} 

//SIM800C发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//其他,期待应答结果的位置(str的位置)
unsigned char *gm3_check_cmd(unsigned char *receive_data,unsigned char *str)
{
	char *strx=0;
	strx=strstr((const char*)receive_data,(const char*)str);//strstr(str1,str2) 函数用于判断字符串str2是否是str1的子串。如果是，则该函数返回str2在str1中首次出现的地址；否则，返回NULL。
	return (unsigned char*)strx;
}

