#include "function.h"
#include "usart.h"
#include "stmflash.h"
#include "delay.h"
#include "string.h"
unsigned short judge_data_u16[10]={0x0000}; 
unsigned char All_ID[2048]={0xFF};
//////////////////////////////////////////////////////////////////////////////////	 
//程序功能。
//计算校验和。
//data是数组，star是数组的起始位置，end是数组的结束位置。
//举例：数组 data｛0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37｝
//sumcheck(data,1,5);
//则是计算 0x31,0x32,0x33,0x34,0x35的和。
//返回值为 0xFF;
////////////////////////////////////////////////////////////////////////////////// 	
unsigned char sumcheck(unsigned char *data,unsigned char star,unsigned char end)//计算校验和.
{
	unsigned char sum_check=0;
	unsigned char i=0;	
	end=end-star+1;
	for(i=0;i<end;i++)
	{
		sum_check=sum_check+data[i+star];
	}		
	return sum_check;//返回校验和
}

//////////////////////////////////////////////////////////////////////////////////	 
//程序功能。
//将一个数组压缩成一半。
//前提是这个数组中的元素只能是｛0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46｝
//对应的为｛1，2，3，4，5，6，7，8，9,A,B,C,D,E,F｝
//举例：数组｛30 31 42 32 31 37 41 31 31 31 43 46 30 30 30 30 30 30 30 31  ｝
//整合完，返回数组为｛01 B2 17 A1 11 CF 00 00 00 01｝
////////////////////////////////////////////////////////////////////////////////// 	
void DATA_change(unsigned char *data,unsigned int size,unsigned char *target_data)  
{//data是准备转换的数组。size是转换数组中元素的数量，必须是偶数,target_data是数据转换后存放的数组。
	unsigned char i=0;
	unsigned char j=0;	
	unsigned char data_middle[200]={0x00};	
	for(i=0;i<size;i++)//提取网关ID
	{
		switch(data[i])//
		{
		 case 0x30:		
				data_middle[i]=0x00;
				break;	 			
		 case 0x31:		
				data_middle[i]=0x01;		 
				break;	
		 case 0x32:		
				data_middle[i]=0x02;				 
				break;	 			
		 case 0x33:		
				data_middle[i]=0x03;				 
				break;	
		 case 0x34:		
				data_middle[i]=0x04;				 
				break;	
		 case 0x35:		
				data_middle[i]=0x05;				 
				break;	 			
		 case 0x36:		
				data_middle[i]=0x06;				 
				break;	
		 case 0x37:		
				data_middle[i]=0x07;				 
				break;	 			
		 case 0x38:		
				data_middle[i]=0x08;				 
				break;	
		 case 0x39:		
				data_middle[i]=0x09;				 
				break;		 
		 case 0x41:		
				data_middle[i]=0x0A;				 
				break;	 			
		 case 0x42:		
				data_middle[i]=0x0B;				 
				break;	
		 case 0x43:		
				data_middle[i]=0x0C;				 
				break;	 			
		 case 0x44:		
				data_middle[i]=0x0D;				 
				break;	
		 case 0x45:		
				data_middle[i]=0x0E;				 
				break;		 
		 case 0x46:		
				data_middle[i]=0x0F;				 
				break;	 
		 default:
				data_middle[i]=0x0F;			 
				break;
		}
	}
	for(i=0;i<size;i++)//提取网关ID
	{
		j=i%2;
		if(j==0)//奇数
		{
			data_middle[i]=data_middle[i]<<4;
		}
	}
	for(i=0;i<size/2;i++)//提取网关ID
	{
		target_data[i]=data_middle[i*2]+data_middle[(i*2)+1];
	}
}
void handle_arrays_u8(unsigned char *data,unsigned char size,unsigned char *printf_data)//处理数组，DATA是要处理的数组，SIZE是数组的长度。//比如将0x25分开就是0x32和0x35.。0xAB分开就是0x40,0x41. 
{//DATA是输入的数组，SIZE是数组中元素的数量，Printf_data是整理完后用于测试打印的数组。
	unsigned char i=0;
//	unsigned char j=0;	
	unsigned char DATA_Printf_H_L_middle=0;
	unsigned char DATA_Printf_H=0;
	unsigned char DATA_Printf_L=0;
	for(i=0;i<size;i++)
	{
		DATA_Printf_H_L_middle=data[i];
		DATA_Printf_H_L_middle=DATA_Printf_H_L_middle&0xF0;		
		DATA_Printf_H=(DATA_Printf_H_L_middle>>4);
		switch(DATA_Printf_H)//判断Buzzer_flag来判断应该如何鸣叫
		{
		 case 0x00:		
				DATA_Printf_H=0x30;
				break;	 			
		 case 0x01:		
				DATA_Printf_H=0x31;		 
				break;	
		 case 0x02:		
				DATA_Printf_H=0x32;				 
				break;	 			
		 case 0x03:		
				DATA_Printf_H=0x33;				 
				break;	
		 case 0x04:		
				DATA_Printf_H=0x34;				 
				break;	
		 case 0x05:		
				DATA_Printf_H=0x35;				 
				break;	 			
		 case 0x06:		
				DATA_Printf_H=0x36;				 
				break;	
		 case 0x07:		
				DATA_Printf_H=0x37;				 
				break;	 			
		 case 0x08:		
				DATA_Printf_H=0x38;				 
				break;	
		 case 0x09:		
				DATA_Printf_H=0x39;				 
				break;		 
		 case 0x0A:		
				DATA_Printf_H=0x41;				 
				break;	 			
		 case 0x0B:		
				DATA_Printf_H=0x42;				 
				break;	
		 case 0x0C:		
				DATA_Printf_H=0x43;				 
				break;	 			
		 case 0x0D:		
				DATA_Printf_H=0x44;				 
				break;	
		 case 0x0E:		
				DATA_Printf_H=0x45;				 
				break;		 
		 case 0x0F:		
				DATA_Printf_H=0x46;				 
				break;	 
		 default:
				DATA_Printf_H=0x46;			 
				break;
		}			
		DATA_Printf_H_L_middle=data[i];
		DATA_Printf_H_L_middle=DATA_Printf_H_L_middle&0x0F;
		DATA_Printf_L=DATA_Printf_H_L_middle;
		switch(DATA_Printf_L)//判断Buzzer_flag来判断应该如何鸣叫
		{
		 case 0x00:		
				DATA_Printf_L=0x30;
				break;	 			
		 case 0x01:		
				DATA_Printf_L=0x31;		 
				break;	
		 case 0x02:		
				DATA_Printf_L=0x32;				 
				break;	 			
		 case 0x03:		
				DATA_Printf_L=0x33;				 
				break;	
		 case 0x04:		
				DATA_Printf_L=0x34;				 
				break;	
		 case 0x05:		
				DATA_Printf_L=0x35;				 
				break;	 			
		 case 0x06:		
				DATA_Printf_L=0x36;				 
				break;	
		 case 0x07:		
				DATA_Printf_L=0x37;				 
				break;	 			
		 case 0x08:		
				DATA_Printf_L=0x38;				 
				break;	
		 case 0x09:		
				DATA_Printf_L=0x39;				 
				break;		 
		 case 0x0A:		
				DATA_Printf_L=0x41;				 
				break;	 			
		 case 0x0B:		
				DATA_Printf_L=0x42;				 
				break;	
		 case 0x0C:		
				DATA_Printf_L=0x43;				 
				break;	 			
		 case 0x0D:		
				DATA_Printf_L=0x44;				 
				break;	
		 case 0x0E:		
				DATA_Printf_L=0x45;				 
				break;		 
		 case 0x0F:		
				DATA_Printf_L=0x46;				 
				break;	 
		 default:
				DATA_Printf_L=0x46;			 
				break;
		}		
		printf_data[(i*2)+0]=DATA_Printf_H;
		printf_data[(i*2)+1]=DATA_Printf_L;	
	}	
}
void DATA_change_u8_to_u16(unsigned char *data_u8,unsigned int size,unsigned short *data_u16) //将两个8位数据转换成一个16位数据。比如将：0x25,0x5F变成0x255F
{//data_u8是输入的8位数组，size是8位数组中元素的数量（必须是偶数），data_u16是返回的16位数组。
	unsigned char i=0;
	unsigned short  DATA_middle_u16=0;	
	for(i=0;i<size/2;i++)//将网关ID填写进网关的eeprom中
	{
		DATA_middle_u16=data_u8[(i*2)+0];
		DATA_middle_u16=DATA_middle_u16<<8;
		data_u16[i]=DATA_middle_u16+data_u8[(i*2)+1];
	}
}

void DATA_change_u16_to_u8(unsigned short *data_u16,unsigned int size,unsigned char *data_u8) //将一个16位数据转换成两个8位数据。 
{//data_u16是输入的16位数组，size是16位数组中元素的数量，data_u8是返回的8位数组。
	unsigned char i=0;
	unsigned short  DATA_middle_u16=0;	
	for(i=0;i<size;i++)//将网关ID填写进网关的eeprom中
	{
		DATA_middle_u16=data_u16[i];
		data_u8[(i*2)+0]=DATA_middle_u16>>8;
		data_u8[(i*2)+1]=data_u16[i];
	}
}

void Printf_device_type(unsigned char type)//测试打印设备类型。根据国标。
{
	switch(type)
	{
		case 0x17://手动报警按钮
			printf("\r\n手动报警按钮  ");
		break;
		case 0x1E://感温火灾探测器
			printf("\r\n感温火灾探测器");					
		break;
		case 0x28://感烟火灾探测器
			printf("\r\n感烟火灾探测器");					
		break;		
		case 0xB1://声光报警装置
			printf("\r\n声光报警装置");		
		break;	
		case 0xB2://蓝牙离位基站
			printf("\r\n蓝牙离位基站");					
		break;														
		case 0xB3://门磁探测器
			printf("\r\n门磁探测器");					
		break;										
		default:
			printf("\r\n未知设备");	
		break;
	}		
}
void Judge_ID(unsigned short *In_Put_Compare_ID)//首先判断合法性。然后依据大小进行排列。
{
//	unsigned char flash_memory_u8[20]={0x00};//存储的信息。
//	unsigned short memory_index[30]={0x00};		//读取数据索引。包括存储了多少个ID。
//	unsigned short memory_address[30]={0x00};	//地址号。
	unsigned char break_flag=0;//while跳出标志位。不为0的时候，跳出。	
	unsigned short  front=0;//二分法比较中用到的临时变量,上界。
	unsigned short  mid=0;  //二分法比较中用到的临时变量,中间。	
	unsigned short  end=0;  //二分法比较中用到的临时变量,下界	。
//	unsigned char flash_page=0;//内存中指向的ID所在页码的位置。
	unsigned short  position_point=0;//指向的地址。
//	unsigned int  memory_ID_amount=0;	//网关中保存的ID的总数量
	unsigned short Memory_Compare_ID[20]={0x00};//内存中指向的ID。		
	unsigned short Memory_Compare_ID1[20]={0x00};//内存中指向的ID。			
	unsigned char Memory_Compare_ID_middle[20]={0x00};
//	unsigned char Memory_Compare_ID_middle1[20]={0x00};	
//	unsigned char NODE_ID_u8[20]={0x00};	
//	unsigned char printf_data[50]={0x00};//测试打印用的数组
//----------设备ID的合法性验证。如果已经在eeprom中有保存，则返回0xFFFF，反之则返回具体地址。-------------//
//	STMFLASH_Read(NUMBER_ADDRESS,(u16*)flash_memory_u8,5); // 读取出已经注册的设备的ID的数量。
////计算出网关总共注册了多少个设备。
//	memory_ID_amount=flash_memory_u8[6];//高位
//	memory_ID_amount=(memory_ID_amount<<8)+flash_memory_u8[7];//高位加低位
/********************************/
//	memory_ID_amount=1;//比较数组中总的元素数量。
	if(Bluetooh_amount==0)//还未保存任何ID
	{
		break_flag=1;//不必进入下面的比较环节，直接返回没有注册过设备。
		position_point=0;//因为还没有保存ID，所以将输入的ID放在0个位置。
	}
	else
	{
//二分法使用时的初始值
		break_flag=0;
		front=0;								//二分法比较中用到的临时变量,上界。
		end=Bluetooh_amount-1; //二分法比较中用到的临时变量,下界。（最后一个元素的下标）	调试用。暂时用100个元素的数组进行测试。
	}
/********************************/		
/*************测试用**************/	
//	STMFLASH_Read((DEVICE_ADDRESS+(mid/200)*2048+(mid%200)*10),(u16*)Memory_Compare_ID_middle,3);//计算出所在位置的页码。读出一组3位的16位ID号码。每次读10个字节。后面4位是备用。
//	handle_arrays_u8(Memory_Compare_ID_middle,6,printf_data);//比如将0x25分开就是0x32和0x35.。0xAB分开就是0x40,0x41.								
//	USART1_SEND(printf_data,12);								
//	printf("\r\n");			
/********************************/	

//	STMFLASH_Read((DEVICE_ADDRESS+(mid/200)*2048+(mid%200)*10),(u16*)Memory_Compare_ID_middle,3);//计算出所在位置的页码。读出一组3位的16位ID号码。每次读10个字节。后面4位是备用。
//	handle_arrays_u8(Memory_Compare_ID_middle,6,printf_data);//比如将0x25分开就是0x32和0x35.。0xAB分开就是0x40,0x41.								
//	USART1_SEND(printf_data,12);								
//	printf("\r\n");			
//	DATA_change_u8_to_u16(Memory_Compare_ID_middle,6,Memory_Compare_ID);
//	printf("%d\r\n",Memory_Compare_ID[0]);
//	printf("%d\r\n",Memory_Compare_ID[1]);	
//	printf("%d\r\n",Memory_Compare_ID[2]);		
/************测试用************/			
//	printf("输入ID\r\n");	
//	printf("%d\r\n",In_Put_Compare_ID[0]);
//	printf("%d\r\n",In_Put_Compare_ID[1]);	
//	printf("%d\r\n",In_Put_Compare_ID[2]);	
//	printf("\r\n");	
/************测试用************/				
	while(break_flag==0)//一直比较，直到比较结束才跳出。
	{
		mid=(front+end)/2;  		//二分法比较中用到的临时变量,中间。
//		printf("%d\r\n",mid);//测试用			
		if(end-front<=1)
		{//将输入的数据和这两个数进行前后比较。
			if( (end-front)==0 )//剩下一位数。
			{
//6位ID，4位备用。共计10位。
//每页最多存储204个。约定最多每页存储200个。			
/****************************************************读出即将要比较多的内部存储空间的ID*****************************************************/
//*****************基地址***********页地址********部件地址
				STMFLASH_Read((DEVICE_ADDRESS+(front/200)*2048+(front%200)*10),(u16*)Memory_Compare_ID_middle,3);//计算出所在位置的页码和位置。读出一组6个元素的8位ID号码。
/*************测试用**************/	
//				handle_arrays_u8(Memory_Compare_ID_middle,6,printf_data);//比如将0x25分开就是0x32和0x35.。0xAB分开就是0x40,0x41.								
//				USART1_SEND(printf_data,12);
//				printf("\r\n");			
				DATA_change_u8_to_u16(Memory_Compare_ID_middle,6,Memory_Compare_ID);//将8位数组转换成16位数组。
//生成	Memory_Compare_ID[0],Memory_Compare_ID[1],Memory_Compare_ID[2];			
				if((In_Put_Compare_ID[0]<Memory_Compare_ID[0])||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]<Memory_Compare_ID[1]))||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID[1])&&(In_Put_Compare_ID[2]<Memory_Compare_ID[2])))//如果指向的ID小于输入的ID
				{//输入值小于内存中读取出来的值。
					break_flag=1;//跳出标志位。
					position_point=front;				
				}
				else if((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID[1])&&(In_Put_Compare_ID[2]==Memory_Compare_ID[2]))//如果查到了一模一样的ID。说明注册过。
				{//输入值等于内存中读取出来的值。
					break_flag=2;//跳出标志位。
					position_point=front;								
				}
				else if((In_Put_Compare_ID[0]>Memory_Compare_ID[0])||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]>Memory_Compare_ID[1]))||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID[1])&&(In_Put_Compare_ID[2]>Memory_Compare_ID[2])))//如果指向的ID大于输入的ID
				{//输入值大于内存中读取出来的值。
					break_flag=1;//跳出标志位。
					position_point=front+1;						
				}
			}
			else//剩下两位数。
			{
				STMFLASH_Read((DEVICE_ADDRESS+(front/200)*2048+(front%200)*10),(u16*)Memory_Compare_ID_middle,3);//计算出所在位置的页码和位置。读出一组6个元素的8位ID号码。
/*************测试用**************/	
//				handle_arrays_u8(Memory_Compare_ID_middle,6,printf_data);//比如将0x25分开就是0x32和0x35.。0xAB分开就是0x40,0x41.								
//				USART1_SEND(printf_data,12);								
//				printf("\r\n");			
				DATA_change_u8_to_u16(Memory_Compare_ID_middle,6,Memory_Compare_ID);//将8位数组转换成16位数组。
//生成	Memory_Compare_ID[0],Memory_Compare_ID[1],Memory_Compare_ID[2];		
				STMFLASH_Read((DEVICE_ADDRESS+((front+1)/200)*2048+((front%200)+1)*10),(u16*)Memory_Compare_ID_middle,3);//计算出所在位置的页码和位置。读出一组6个元素的8位ID号码。
/*************测试用**************/	
//				handle_arrays_u8(Memory_Compare_ID_middle,6,printf_data);//比如将0x25分开就是0x32和0x35.。0xAB分开就是0x40,0x41.								
//				USART1_SEND(printf_data,12);								
//				printf("\r\n");			
				DATA_change_u8_to_u16(Memory_Compare_ID_middle,6,Memory_Compare_ID1);//将8位数组转换成16位数组。
//生成	Memory_Compare_ID[0],Memory_Compare_ID[1],Memory_Compare_ID[2];						
// front   end					
//   A      B  			
				if((In_Put_Compare_ID[0]<Memory_Compare_ID[0])||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]<Memory_Compare_ID[1]))||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID[1])&&(In_Put_Compare_ID[2]<Memory_Compare_ID[2])))//如果指向的ID小于输入的ID
				{//输入值小于内存中读取出来的A值。
					break_flag=1;//跳出标志位。
					position_point=front;				
				}			
				else if((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID[1])&&(In_Put_Compare_ID[2]==Memory_Compare_ID[2]))//如果查到了一模一样的ID。说明注册过。
				{//输入值等于内存中读取出来的A值。
					break_flag=2;//跳出标志位。
					position_point=front;
				}
				else if( ((In_Put_Compare_ID[0]>Memory_Compare_ID[0])||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]>Memory_Compare_ID[1]))||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID[1])&&(In_Put_Compare_ID[2]>Memory_Compare_ID[2])))&&((In_Put_Compare_ID[0]<Memory_Compare_ID1[0])||((In_Put_Compare_ID[0]==Memory_Compare_ID1[0])&&(In_Put_Compare_ID[1]<Memory_Compare_ID1[1]))||((In_Put_Compare_ID[0]==Memory_Compare_ID1[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID1[1])&&(In_Put_Compare_ID[2]<Memory_Compare_ID1[2]))) )//如果指向的ID大于输入的ID
				{//输入值大于内存中读取出来的A值,小于读取出来的B值。				
					break_flag=1;//跳出标志位。
					position_point=front+1;				
				}
				else if((In_Put_Compare_ID[0]==Memory_Compare_ID1[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID1[1])&&(In_Put_Compare_ID[2]==Memory_Compare_ID1[2]))//如果查到了一模一样的ID。说明注册过。
				{//输入值等于内存中读取出来的B值。
					break_flag=2;//跳出标志位。
					position_point=front+1;							
				}
				else if((In_Put_Compare_ID[0]>Memory_Compare_ID1[0])||((In_Put_Compare_ID[0]==Memory_Compare_ID1[0])&&(In_Put_Compare_ID[1]>Memory_Compare_ID1[1]))||((In_Put_Compare_ID[0]==Memory_Compare_ID1[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID1[1])&&(In_Put_Compare_ID[2]>Memory_Compare_ID1[2])))
				{//输入值大于内存中读取出来的B值。
					break_flag=1;//跳出标志位。
					position_point=front+2;							
				}			
			}
		}
		else
		{
			STMFLASH_Read((DEVICE_ADDRESS+(mid/200)*2048+(mid%200)*10),(u16*)Memory_Compare_ID_middle,3);//计算出所在位置的页码和位置。读出一组6个元素的8位ID号码。
/*************测试用**************/	
//				handle_arrays_u8(Memory_Compare_ID_middle,6,printf_data);//比如将0x25分开就是0x32和0x35.。0xAB分开就是0x40,0x41.								
//				USART1_SEND(printf_data,12);								
//				printf("\r\n");			
			DATA_change_u8_to_u16(Memory_Compare_ID_middle,6,Memory_Compare_ID);//将8位数组转换成16位数组。
//生成	Memory_Compare_ID[0],Memory_Compare_ID[1],Memory_Compare_ID[2];		
			
			if((In_Put_Compare_ID[0]<Memory_Compare_ID[0])||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]<Memory_Compare_ID[1]))||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID[1])&&(In_Put_Compare_ID[2]<Memory_Compare_ID[2])))//如果指向的ID小于输入的ID
			{//继续往前半段去查找。		
				end=mid-1;  			//二分法比较中用到的临时变量,下界	。	
			}		
			else if((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID[1])&&(In_Put_Compare_ID[2]==Memory_Compare_ID[2]))//如果查到了一模一样的ID。说明注册过。
			{//相等。
				break_flag=2;//跳出标志位。
				position_point=mid;
			}
			else if((In_Put_Compare_ID[0]>Memory_Compare_ID[0])||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]>Memory_Compare_ID[1]))||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID[1])&&(In_Put_Compare_ID[2]>Memory_Compare_ID[2])))
			{//继续往后半段去查找。
				front=mid+1;			//二分法比较中用到的临时变量,上界。			
			}
		}
/************测试用************/					
//	printf("内存中的ID\r\n");	
//	printf("%d\r\n",Memory_Compare_ID[0]);
//	printf("%d\r\n",Memory_Compare_ID[1]);	
//	printf("%d\r\n",Memory_Compare_ID[2]);
//	printf("%d\r\n",Memory_Compare_ID1[0]);
//	printf("%d\r\n",Memory_Compare_ID1[1]);	
//	printf("%d\r\n",Memory_Compare_ID1[2]);		
//	printf("\r\n");				
/************测试用************/					
	}
//返回值中有3个信息元素。一个是ID是否注册过。一个是页码。一个是ID所在的位置（如果注册过，则是所存ID所在的位置。如果没注册过，则是两个ID之间的位置）。
	if(break_flag==2)//说明注册过
	{
		In_Put_Compare_ID[3]=position_point;	//注册的这个ID的地址	
		In_Put_Compare_ID[4]=0x0001;						//注册过这个ID
//		printf("\r\n存在这个ID，ID位置下标为:%d\r\n",position_point);	
//		DATA_change_u16_to_u8(In_Put_Compare_ID,3,NODE_ID_u8);		
//		handle_arrays_u8(NODE_ID_u8,6,printf_data);
//		USART1_SEND(printf_data,12);
//		printf("\r\n");
	}
	else
	{
		In_Put_Compare_ID[3]=position_point;	//即将插入的ID的地址	
		In_Put_Compare_ID[4]=0x0000;						//没有注册过这个ID
//		printf("\r\n不存在这个ID，需要将ID插入到下标为:%d\r\n",position_point);
//		DATA_change_u16_to_u8(In_Put_Compare_ID,3,NODE_ID_u8);		
//		handle_arrays_u8(NODE_ID_u8,6,printf_data);
//		USART1_SEND(printf_data,12);
//		printf("\r\n");		
	}
//	return position_point;//返回地址号。	这个地址号指的是每页中的0~199.比如指向的地址为320.则实际单片机中的地址为DEVICE_ADDRESS+(320/200)*2048+(320%200)*10
//	printf("指向的位置：%d\r\n",position_point);//调试用。	
}
unsigned int Save_ROUTER_ID(unsigned char *ROUTER_ID)//保存网关ID.如果保存成功。返回1。如果保存失败，则返回0。
{
	unsigned int i=0;
	unsigned char flash_memory_u8[20]={0};//读取数据索引。包括网关的ID号码和存储了多少个ID。
	STMFLASH_Read(NUMBER_ADDRESS,(u16*)flash_memory_u8,4); //读取网关信息。前6位是网关的ID。后两位是网关中注册的设备数量。			
	for(i=0;i<6;i++)//将新的网关ID整理进去
	{
		flash_memory_u8[i]=ROUTER_ID[i];
	}						
	STMFLASH_Write(NUMBER_ADDRESS,(u16*)flash_memory_u8,5);//将变动后的数据索引重新写入eeprom
	STMFLASH_Read(NUMBER_ADDRESS,(u16*)flash_memory_u8,4);
	if( (flash_memory_u8[0]==ROUTER_ID[0])&&(flash_memory_u8[1]==ROUTER_ID[1])&&(flash_memory_u8[2]==ROUTER_ID[2])&&(flash_memory_u8[3]==ROUTER_ID[3])&&(flash_memory_u8[4]==ROUTER_ID[4])&&(flash_memory_u8[5]==ROUTER_ID[5]) )
	{//如果读取出来的数据跟存进去的数据一致，则认为写入成功。
		i=1;
	}
	else
	{
		i=0;
	}
	return i;
}
unsigned int Save_NODE_ID(unsigned char *In_put_ID)//保存设备ID.如果保存成功。返回保存的地址号。如果保存失败，则返回0xFFFF。
{
//	unsigned char printf_data[50]={0};//测试打印用的数组	
	unsigned char flash_memory_u8[20]={0x00};//存储的信息。
	unsigned int i=0;	
//	unsigned int j=0;
	unsigned char test[20]={0x00};
	unsigned int back_flag=0;//返回标志位
	unsigned char Page_All=0;//所有页
	unsigned char Page_Now=0;//即将插入的页
	unsigned char Address_Now=0;//在一页中的位置。
//	unsigned char Address_Now_middle=0;
	unsigned int  position_point=0;//指向的地址。
//	unsigned char Last_ID[10]={0};//每页的最后一组ID放在这个缓存中，然后放在下一页的第一组ID上。
//	unsigned int  flash_memory_u16[30]={0};//读取数据索引。包括存储了多少个ID和网关的ID号码。
//	unsigned int  memory_ID_amount=0;//网关中保存的ID的总数量 	
//	STMFLASH_Read(NUMBER_ADDRESS,(u16*)flash_memory_u8,4); // 读取出已经注册的设备的ID的数量。
////计算出网关总共注册了多少个设备。
//	memory_ID_amount=flash_memory_u8[6];//高位
//	memory_ID_amount=(memory_ID_amount<<8)+flash_memory_u8[7];//高位加低位
//	printf("网关中已注册设备ID个数为：%d\r\n",memory_ID_amount);//测试用
	Page_All=Bluetooh_amount/200;//计算出总共现在存了多少页。
//部件即将保存的地址。（在代入的数组中的6~7两个位置。）	
	position_point=In_put_ID[6];//高位
	position_point=(position_point<<8)+In_put_ID[7];//高位加低位
	In_put_ID[6]=0x00;
	In_put_ID[7]=0x00;
//	printf("指向的位置：%d\r\n",position_point);//调试用。	
	Page_Now=position_point/200;//计算出这个元素应该存在第几页。
	Address_Now=position_point%200;//计算出这个元素应该在这一页中的哪个位置。0开始。
//	handle_arrays_u8(In_put_ID,10,printf_data);	
//	USART1_SEND(printf_data,20);
	STMFLASH_Read((DEVICE_ADDRESS+Page_Now*2048),(u16*)All_ID,1000);//读出需要修改页的整页的ID。
//	test=Address_Now-position_point;
//	printf("%d\r\n",test);//调试用。
//	for(i=0;i<(1000-Address_Now)*10;i++)//要动的元素的个数。
	for(i=0;i<2000-(Address_Now*10);i++)//要动的元素的个数。	
	{
		All_ID[2009-i]=All_ID[2009-i-10];//将数据右移10位。
	}
//	printf("%d\r\n",Address_Now);//调试用。	
	for(i=0;i<10;i++)//将要保存的10位ID填充到数组位置。
	{
		All_ID[(Address_Now*10)+i]=In_put_ID[i];
	}
	STMFLASH_Write( (DEVICE_ADDRESS+Page_Now*2048),(u16*)All_ID,1005);//将本页变动后的数据写入
//是否需要右移。如不右移，则会导致本次所写掩盖上次所写（读出与写入的不一致的时候）。
//	STMFLASH_Read((DEVICE_ADDRESS+position_point*10),(u16*)All_ID,5);//读出刚刚写入的ID。	
	STMFLASH_Read((DEVICE_ADDRESS+(position_point/200)*2048+(position_point%200)*10),(u16*)All_ID,5);//读出刚刚写入的ID。		
///******************测试用***********************/
//	handle_arrays_u8(All_ID,10,printf_data);//测试用	
//	USART1_SEND(printf_data,20);//打印保存的ID号码。		
//	printf("\r\n");
///***********************************************/
	if( (All_ID[0]==In_put_ID[0])&&(All_ID[1]==In_put_ID[1])&&(All_ID[2]==In_put_ID[2])&&(All_ID[3]==In_put_ID[3])&&(All_ID[4]==In_put_ID[4])&&(All_ID[5]==In_put_ID[5]) )
	{
		back_flag=position_point;//返回值是指向的存储ID的位置。
		if(Bluetooh_amount>=Bluetooh_Max_NO)//判断网关中ID数量是否需要加1.
		{
			
		}	
		else
		{
			Bluetooh_amount=Bluetooh_amount+1;
		}	
	STMFLASH_Read(NUMBER_ADDRESS,(u16*)flash_memory_u8,4); // 读取出已经注册的设备的ID的数量。
//计算出网关总共注册了多少个设备。		
		flash_memory_u8[6]=Bluetooh_amount>>8;//高位			
		flash_memory_u8[7]=Bluetooh_amount;//低位
		STMFLASH_Write(NUMBER_ADDRESS,(u16*)flash_memory_u8,4);//将变动后的网关信息重新写入到网关中。
		if(Page_All==Page_Now)//写入的地址在最后一页上面。
		{
		}
		else
		{
			for(i=0;i<Page_All-Page_Now;i++)//查看需要进行多少次页的操作。（往后错位多少页）
			{		
				STMFLASH_Read((DEVICE_ADDRESS+(Page_Now+i)*2048+2000),(u16*)test,5);//读出末端写入的ID。			
				STMFLASH_Read((DEVICE_ADDRESS+(Page_Now+i+1)*2048),(u16*)All_ID,1000);//读出末端写入的ID。	
				for(i=0;i<2000;i++)//要动的元素的个数。
				{
					All_ID[2009-i]=All_ID[2009-i-10];//将数据右移10位。
				}				
				for(i=0;i<10;i++)//将要保存的10位ID填充到数组位置。
				{
					All_ID[i]=test[i];
				}
				STMFLASH_Write( (DEVICE_ADDRESS+(Page_Now+i+1)*2048),(u16*)All_ID,1005);//将本页变动后的数据写入		
			}			
		}		
		if(Bluetooh_amount%200==1)//如果写满了一页。则往后错一页。
		{
				STMFLASH_Read((DEVICE_ADDRESS+Page_All*2048+2000),(u16*)test,5);//读出末端写入的ID		
				for(i=0;i<10;i++)//将要保存的10位ID填充到数组位置。
				{
					All_ID[i]=test[i];
				}
				STMFLASH_Write( (DEVICE_ADDRESS+(Page_All+1)*2048),(u16*)All_ID,5);//将本页变动后的数据写入								
		}		
	}
	else
	{
//		printf("保存设备ID失败");
		back_flag=0xFFFF;
	}
	return back_flag;
}


void Delete_NODE_ID(unsigned char *In_put_ID)//删除设备ID.如果删除成功。返回保存的地址号。如果删除失败，则返回0xFFFF。
{
//	unsigned char printf_data[50]={0};//测试打印用的数组	
	unsigned char flash_memory_u8[20]={0x00};//存储的信息。
	unsigned int i=0;	
	unsigned int j=0;
	unsigned char test[20]={0x00};
//	unsigned int back_flag=0;//返回标志位
	unsigned char Page_All=0;//所有页
	unsigned char Page_Now=0;//即将插入的页
	unsigned char Address_Now=0;//在一页中的位置。
//	unsigned char Address_Now_middle=0;
	unsigned int  position_point=0;//指向的地址。
//	unsigned char Last_ID[10]={0};//每页的最后一组ID放在这个缓存中，然后放在下一页的第一组ID上。
//	unsigned int  flash_memory_u16[30]={0};//读取数据索引。包括存储了多少个ID和网关的ID号码。
//	unsigned int  memory_ID_amount=0;//网关中保存的ID的总数量 	
	STMFLASH_Read(NUMBER_ADDRESS,(u16*)flash_memory_u8,4); // 读取出已经注册的设备的ID的数量。
//计算出网关总共注册了多少个设备。
	Bluetooh_amount=flash_memory_u8[6];//高位
	Bluetooh_amount=(Bluetooh_amount<<8)+flash_memory_u8[7];//高位加低位
//	printf("网关中已注册设备ID个数为：%d\r\n",memory_ID_amount);//测试用
	Page_All=Bluetooh_amount/200;//计算出总共现在存了多少页。
//部件即将保存的地址。	
	position_point=In_put_ID[6];//高位
	position_point=(position_point<<8)+In_put_ID[7];//高位加低位
//	printf("指向的位置：%d\r\n",position_point);//调试用。	
	Page_Now=position_point/200;//计算出这个元素应该存在第几页。取整。
	Address_Now=position_point%200;//计算出这个元素应该在这一页中的哪个位置。0开始。取余。
//	handle_arrays_u8(In_put_ID,10,printf_data);
//	USART1_SEND(printf_data,20);
	STMFLASH_Read((DEVICE_ADDRESS+Page_Now*2048),(u16*)All_ID,1005);//读出需要修改页的整页的ID。
//	test=Address_Now-position_point;
//	printf("%d\r\n",test);//调试用。
//	for(i=0;i<(1000-Address_Now)*10;i++)//要动的元素的个数。
	for(i=0;i<2010-(Address_Now*10);i++)//要动的元素的个数。
	{
		All_ID[(Address_Now*10)+i]=All_ID[(Address_Now*10)+i+10];//将数据左移10位。
	}
//	printf("%d\r\n",Address_Now);//调试用。	
	STMFLASH_Read((DEVICE_ADDRESS+(Page_Now+1)*2048),(u16*)test,5);//读出需要修改页的第一组ID。	
	for(i=0;i<10;i++)//将下一页中前面的ID保存到本页的尾部。
	{
		All_ID[2000+i]=test[i];
	}
	STMFLASH_Write( (DEVICE_ADDRESS+Page_Now*2048),(u16*)All_ID,1005);//将本页变动后的数据写入
//是否需要右移。如不右移，则会导致本次所写掩盖上次所写（读出与写入的不一致的时候）。
//	STMFLASH_Read((DEVICE_ADDRESS+position_point*10),(u16*)All_ID,5);//读出刚刚写入的ID。	
//	STMFLASH_Read((DEVICE_ADDRESS+(position_point/200)*2048+(position_point%200)*10),(u16*)All_ID,5);//读出刚刚写入的ID。		
///******************测试用***********************/
//	handle_arrays_u8(All_ID,10,printf_data);//测试用	
//	USART1_SEND(printf_data,20);//打印保存的ID号码。		
//	printf("\r\n");
///***********************************************/
		Bluetooh_amount=Bluetooh_amount-1;//网关中ID的总数量减1.
		flash_memory_u8[7]=Bluetooh_amount;//低位
		flash_memory_u8[6]=Bluetooh_amount>>8;//高位	
		STMFLASH_Write(NUMBER_ADDRESS,(u16*)flash_memory_u8,4);//将变动后的网关信息重新写入到网关中。
		if(Page_All==Page_Now)//写入的地址在最后一页上面。
		{
		}
		else
		{
			for(i=1;i<(Page_All-Page_Now)+1;i++)//查看需要进行多少次页的操作。（往后错位多少页）
			{
				STMFLASH_Read((DEVICE_ADDRESS+i*2048),(u16*)All_ID,1005);//读出需要修改页的整页的ID。
				for(j=0;j<2010;j++)//要动的元素的个数。
				{
					All_ID[j]=All_ID[j+10];//将数据左移10位。
				}
				STMFLASH_Read((DEVICE_ADDRESS+(i+1)*2048),(u16*)test,5);//读出需要修改页的整页的ID。
				for(j=0;j<10;j++)//将下一页中前面的ID保存到本页的尾部。
				{
					All_ID[2000+j]=test[j];
				}	
				STMFLASH_Write( (DEVICE_ADDRESS+i*2048),(u16*)All_ID,1005);//将本页变动后的数据写入
			}
		}		
}

void Delete_ALL_NODE_ID(void)//删除所有注册的设备ID
{
	unsigned int i=0;
	unsigned char memory_index[20]={0x00};
	for(i=0;i<2048;i++)
	{
		All_ID[i]=0xFF;
	}
	STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,5); // 读取已注册设备数据		
	memory_index[6]=0x00;//清空保存的ID数量	 
	memory_index[7]=0x00;		
	Bluetooh_amount=0;//清空蓝牙基站中保存的ID数量
	STMFLASH_Write(NUMBER_ADDRESS,(u16*)memory_index,5);
	STMFLASH_Write(DEVICE_ADDRESS,(u16*)All_ID,1024);
	printf("清空完毕\r\n");
	
}


/*****************************************************************************
* 函数名称: unsigned char  Sting_Compare(unsigned char  *Sting1,unsigned char  *Sting2,unsigned char Len)
* 功能描述: 字符串比较
* 参    数:Sting1：字符串1;Sting2：字符串2;Len:字符串2的长度
* 返回  值:0：字符串不相同；1：字符串相同
*****************************************************************************/
unsigned char  Sting_Compare(unsigned char  *Sting1,unsigned char  *Sting2,unsigned char Len)//字符串比较
{
	unsigned char i = 0; 
	unsigned char j = 0; 	
	unsigned char Flag = 1;
	j=strlen(Sting1);
	if(j==Len)
	{
		for(i=0;i<Len;i++)
		{
			if(Sting1[i]!=Sting2[i])
			{
							Flag = 0;
			}
		}		
	}
	else
	{
		Flag=0;
	}
	return Flag;
}

unsigned int str_int(unsigned char *str)//将数组转换成int型数据。比如数组str为：0x31,0x32,0x33,0x34,0x35,0x36。转换完成后就变成：123456了
{
	unsigned int i=0,tmp=0;
	unsigned char len=0;
	len=strlen((char *)str);
//	for(i=0;(i<len)&&(i<6);i++)
	for(i=0;i<len;i++)	
	{
		if(str[i]>='0'&&str[i]<='9')
		{
			tmp=tmp*10+(str[i]-'0');			
		}
		else 
		{
			break;
		}
	}
	return tmp;
}













