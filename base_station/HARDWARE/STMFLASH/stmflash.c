#include "stmflash.h"
#include "delay.h"
#include "usart.h"
#include "usart2.h"
#include "usart3.h"
#include "uart4.h"
#include "key.h" 
#include "buzzer.h" 
#include "led.h" 
#include "timer.h" 
#include "function.h"
//////////////////////////////////////////////////////////////////////////////////. 
//unsigned int  Bluetooh_Max_NO=8;//蓝牙基站中最富哦存储的标签数量。
unsigned char Gateway_ID[6]={0x00};//网关ID
unsigned char Bluetooh_amount=0;//蓝牙标签总数量
//读取指定地址的半字(16位数据)
//faddr:读地址(此地址必须为2的倍数!!)
//返回值:对应数据.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}
#if STM32_FLASH_WREN	//如果使能了写   
//不检查的写入
//WriteAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数   
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//地址增加2.
	}  
} 
//从指定地址开始写入指定长度的数据
//WriteAddr:起始地址(此地址必须为2的倍数!!)
//pBuffer:数据指针
//NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节
#else 
#define STM_SECTOR_SIZE	2048
#endif		 
u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//最多是2K字节
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //扇区地址
	u16 secoff;	   //扇区内偏移地址(16位字计算)
	u16 secremain; //扇区内剩余地址(16位字计算)	   
 	u16 i;    
	u32 offaddr;   //去掉0X08000000后的地址
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//非法地址
	FLASH_Unlock();						//解锁
	offaddr=WriteAddr-STM32_FLASH_BASE;		//实际偏移地址.
	secpos=offaddr/STM_SECTOR_SIZE;			//扇区地址  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//在扇区内的偏移(2个字节为基本单位.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//扇区剩余空间大小   
	if(NumToWrite<=secremain)secremain=NumToWrite;//不大于该扇区范围
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//擦除这个扇区
			for(i=0;i<secremain;i++)//复制
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//写入整个扇区  
		}else STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;				//扇区地址增1
			secoff=0;				//偏移位置为0 	 
		   	pBuffer+=secremain;  	//指针偏移
			WriteAddr+=secremain;	//写地址偏移	   
		   	NumToWrite-=secremain;	//字节(16位)数递减
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//下一个扇区还是写不完
			else secremain=NumToWrite;//下一个扇区可以写完了
		}	 
	};	
	FLASH_Lock();//上锁
}
#endif

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//WriteAddr:起始地址
//WriteData:要写入的数据
void Test_Write(u32 WriteAddr,u16 WriteData)   	
{
	STMFLASH_Write(WriteAddr,&WriteData,1);//写入一个字 
}
void Star_Read_ID()//读取并打印设备ID
{
	unsigned char i=0;	
	unsigned char printf_data[50]={0x00};
	unsigned char middle_data[50]={0x00};	
	unsigned char memory_index[100]={0x00};//读取数据索引。0~5设备ID,6~7回路号，8~9部件号，10~11存储的节点数量。	
	unsigned char SERVER_ID[50]={0x00};//服务器IP
//	unsigned char PORT_ID[20]={0x00};	//服务器端口号	
	unsigned int  data_long=0;	
	unsigned char test[10]={0x00};		
	unsigned char test1[10]={0x00};			
	unsigned char data[30]={0x00};		
	STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,50); // 读取已注册设备数据
	for(i=0;i<6;i++)//提取蓝牙基站的ID
	{
		Gateway_ID[i]=memory_index[i];//将蓝牙基站ID提取出来，更新全局变量。
	}
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n");
	printf("蓝牙基站ID： ");
	handle_arrays_u8(Gateway_ID,6,printf_data);
	USART1_SEND(printf_data,12);
	printf("\r\n");
//	middle_data[0]=memory_index[8];//提取蓝牙基站的回路号
//	printf("蓝牙基站回路号： ");
//	handle_arrays_u8(middle_data,1,printf_data);
//	USART1_SEND(printf_data,2);
//	printf("\r\n");
//	middle_data[0]=memory_index[9];//提取蓝牙基站的部件号
//	printf("蓝牙基站部件号： ");
//	handle_arrays_u8(middle_data,1,printf_data);
//	USART1_SEND(printf_data,2);
//	printf("\r\n");


	if(memory_index[10]==0xAA)//服务器ID已经设置过
	{
		for(i=0;i<memory_index[11];i++)//提取服务器IP
		{
			SERVER_ID[i]=memory_index[i+12];//将蓝牙基站服务器IP提取出来，更新全局变量。服务器IP的第一个字节代表长度。
		}
		printf("蓝牙基站服务器IP： ");
		USART1_SEND(SERVER_ID,memory_index[11]);
		printf("\r\n");	
	}
	else
	{
		printf("蓝牙基站服务器IP还未设置过\r\n");
	}
	if(memory_index[30]==0xAA)//服务器端口号已经设置过
	{
	/******************K2服务器测试端口号******************/	
		data[0]=memory_index[31];//将数据取出。
		data[1]=memory_index[32];
		data_long=data[0]*256+data[1];
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
			test1[0]=test[0];			
			test1[1]=test[1];
			test1[2]=test[2];			
			test1[3]=test[3];			
			test1[4]=test[4];	
			test1[5]=0x00;			
		}		
		else if( (test[0]==0x30)&&(test[1]!=0x30) )
		{//端口号是4位数。例如：01234.
			test1[0]=test[1];			
			test1[1]=test[2];
			test1[2]=test[3];			
			test1[3]=test[4];		
			test1[4]=0x00;			
		}
		else if( (test[0]==0x30)&&(test[1]==0x30)&&(test[2]!=0x30) )
		{
			test1[0]=test[2];			
			test1[1]=test[3];
			test1[2]=test[4];
			test1[3]=0x00;			
		}
		printf("K2服务器测试端口号:%s\r\n",test1);					
	}
	else
	{
		printf("蓝牙基站服务器端口号还未设置过\r\n");	
	}

//	for(i=0;i<memory_index[31];i++)//提取服务器端口号	
//	{
//		PORT_ID[i]=memory_index[i+32];//将蓝牙基站服务器端口号	提取出来，更新全局变量。服务器端口号的第一个字节代表长度。
//	}
//	printf("蓝牙基站服务器端口号： ");	
//	USART1_SEND(PORT_ID,memory_index[30]);
//	printf("\r\n");	
	printf("蓝牙基站心跳时间间隔： %d 分钟\r\n",memory_index[40]+memory_index[41]*256+memory_index[42]*256*256+memory_index[43]*256*256*256);			
	printf("判定蓝牙离位标签离位时间间隔： %d 分钟\r\n",memory_index[50]+memory_index[51]*256+memory_index[52]*256*256+memory_index[53]*256*256*256);	
	printf("蓝牙离位基站搜索标签的时间间隔： %d 秒\r\n",memory_index[60]);		

//计算出网关总共注册了多少个设备。	
	Bluetooh_amount=memory_index[6];//高位
	Bluetooh_amount=(Bluetooh_amount<<8)+memory_index[7];//高位加低位			
	
	if(Bluetooh_amount>Bluetooh_Max_NO)//最多注8个蓝牙标签。
	{//针对首次使用单片机或者ID已经满了（255）的情况。
		Bluetooh_amount=0;
		memory_index[6]=0x00;		
		memory_index[7]=0x00;
		printf("网关中还未录入设备ID");
		STMFLASH_Write(NUMBER_ADDRESS,(u16*)memory_index,5);//将变动后的数据索引重新写入eeprom		
	}
	else
	{
		printf("蓝牙基站中已注册蓝牙标签个数为：%d\r\n\r\n",Bluetooh_amount);
		for(i=0;i<Bluetooh_amount;i++)
		{
			STMFLASH_Read( (DEVICE_ADDRESS+i*10),(u16*)middle_data,5);
			handle_arrays_u8(middle_data,6,printf_data);
			USART1_SEND(printf_data,12);	
			printf("\r\n");			
		}		
	}
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n");
}
void Judge_Erase_all_ID()//判断网关是否需要擦除所有ID。	
{
//	unsigned char memory_index[20]={0};//读取数据索引。0~5设备ID,6~7回路号，8~9部件号，10~11存储的节点数量。
	if(KEY_flag==1)
	{
		KEY_flag=0;
		Buzzer_flag=1;			
		dislocation_test();
		delay_ms(100);		
		printf("发送测试帧\r\n");		
	}
//	if(KEY_flag==2)//如果长按按键，就发送一帧数据包。
//	{
//		KEY_flag=0;
//		Buzzer_flag=1;	
//		printf("清空保存的所有ID\r\n");
//		delay_ms(100);
//		Delete_ALL_NODE_ID();//删除所有注册的设备ID
//	}		
}


void Refurbish_Star_Read_ID()//刷新全局的标志位。
{
	unsigned char i=0;	
	unsigned char printf_data[50]={0x00};
	unsigned char middle_data[50]={0x00};	
	unsigned char memory_index[100]={0x00};//读取数据索引。0~5设备ID,6~7回路号，8~9部件号，10~11存储的节点数量。	
	unsigned char SERVER_ID[50]={0x00};//服务器IP
//	unsigned char PORT_ID[20]={0x00};	//服务器端口号	
	unsigned int  data_long=0;	
	unsigned char test[10]={0x00};		
	unsigned char test1[10]={0x00};			
	unsigned char data[30]={0x00};		
	STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,50); // 读取已注册设备数据
	for(i=0;i<6;i++)//提取蓝牙基站的ID
	{
		Gateway_ID[i]=memory_index[i];//将蓝牙基站ID提取出来，更新全局变量。
	}
	if(memory_index[10]==0xAA)//服务器ID已经设置过
	{
		for(i=0;i<memory_index[11];i++)//提取服务器IP
		{
			SERVER_ID[i]=memory_index[i+12];//将蓝牙基站服务器IP提取出来，更新全局变量。服务器IP的第一个字节代表长度。
		}
	}
	else
	{
	}
	if(memory_index[30]==0xAA)//服务器端口号已经设置过
	{
	/******************K2服务器测试端口号******************/	
		data[0]=memory_index[31];//将数据取出。
		data[1]=memory_index[32];
		data_long=data[0]*256+data[1];
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
			test1[0]=test[0];			
			test1[1]=test[1];
			test1[2]=test[2];			
			test1[3]=test[3];			
			test1[4]=test[4];	
			test1[5]=0x00;			
		}		
		else if( (test[0]==0x30)&&(test[1]!=0x30) )
		{//端口号是4位数。例如：01234.
			test1[0]=test[1];			
			test1[1]=test[2];
			test1[2]=test[3];			
			test1[3]=test[4];		
			test1[4]=0x00;			
		}
		else if( (test[0]==0x30)&&(test[1]==0x30)&&(test[2]!=0x30) )
		{
			test1[0]=test[2];			
			test1[1]=test[3];
			test1[2]=test[4];
			test1[3]=0x00;			
		}
	}
	else
	{
	}
//计算出网关总共注册了多少个设备。	
	Bluetooh_amount=memory_index[6];//高位
	Bluetooh_amount=(Bluetooh_amount<<8)+memory_index[7];//高位加低位			
}


