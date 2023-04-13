#include "PrinterATcmd.h"
#include "stm32f10x.h" 
#include <string.h>
#include <stdio.h> 
#include "PrinterATrev.h"

typedef struct
{
	unsigned char Flag;
	unsigned int  Lenth;
	unsigned char data[210];
}AT_Data_Typedef;
AT_Data_Typedef AT_RecieveFrame;

/*****************************************************************************
* 函数名称: unsigned char  Sting_Compare(unsigned char  *Sting1,unsigned char  *Sting2,unsigned char Len)
* 功能描述: 字符串比较
* 参    数: Sting1：字符串1;Sting2：字符串2;Len:字符串2的长度
* 返回  值: 0：字符串不相同；1：字符串相同
*****************************************************************************/
unsigned char  Sting_Compare(unsigned char  *Sting1,unsigned char  *Sting2,unsigned char Len)
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

static u8 data_transform_A(u8 data)
{
    if((data>='0')&&(data<='9')) return data-'0';
    if((data>='a')&&(data<='z')) return data-'a'+10;
    if((data>='A')&&(data<='Z')) return data-'A'+10;
    else return 0xff;
}

u8 data_transform_check(u8* str,u8* C1,u8 length)
{
	 u8 i,j=0;
	
		for(i=0;i<length;)
		{
				 if(*(C1+i)==0){break;}
				 
				  *(str+j)=data_transform(*(C1+i));
				  *(str+j)=*(str+j)<<4;
				  *(str+j)=*(str+j)+data_transform(*(C1+i+1));
				 i=i+2;	
				 j++;		

				if(j>length)
				{return 0;}
		}
		return 1;
}

u8 data_transform_checkA(u8* str,u8* C1,u8 length)
{
	  u8 i,j=0;
	  u8 Data=0;	
	
		for(i=0;i<length;i++)
		{
				
			  if((C1[i]==0x2E)||(C1[i]==0x2D)||(C1[i]==0x3E)||(C1[i]==0x00))//.-
				{
				    *str=Data;
					   Data=0;
					   str++;
					   j++;
					   if(j==16)
             {return 1;}
						 if(C1[i]==0){return 0;}
				}
				else
				{
						if((C1[i]<0x30)||(C1[i]>0x3A))//如果不是数字，返回错误
						{    return 0;    }
						else//如果是数字
						{
								 Data=Data*10;
								 Data=Data+(C1[i]-0x30);
						}
				}
		}
		return 0;
}

u8 data_transform_YM(u8* str,u8* C1,u8 length)
{
	u8 i=0,j=0;
	u16 A=0;
	
	for(i=0;i<length;i++)
	{				
     if(*(C1+i)==0)
     {break;}
		 
	   A=A*10;
		 A=A+(*(C1+i)-0x30);
	}
	
	memcpy(str,(u8*)&A,2);
	return A;
}
//////////////////////////////AT命令////////////////////////////////////////////
//<BT+10A_MIM=FB11A>	                 调试密码命令
//<BT+10A_MIM=FB11B>	                 配置密码命令
//<BT+10A_IDS=120707000911>            主机ID设置
//<BT+10A_WLS=1>                       网络设置（1=有线，2=无线） 
//<BT+10A_YIP=dtu.fubangyun.cn>        有线网络域名(支持域名、地址写入)  <BT+10A_YIP=124.192.224.226>
//<BT+10A_YDK=8010>                    有线网络端口
//<BT+10A_ZSX=1>                       有线模式选择（1=手动，2=自动） 
//<BT+10A_DZS=192.168.1.1-255.255.255.0-192.168.1.1-192.168.1.1>                   
//                                     有线IP手动写入
//////////////////////////////AT命令////////////////////////////////////////////
//<BT+10A_YMC=00A1001B7B29>            有线网络端口(生产专用)
//////////////////////////////////////////////////////////////////////////////////
void ID_Print(void);
unsigned char NetStatus_Print(void);
unsigned char ConfigDataSet(unsigned char TypeA,unsigned char *Indata,unsigned char datalength);
void ConfigSystemDataPrint(void);

unsigned char Configure_Data_Analyze(unsigned char *USART1_RXD,unsigned char lenth)//数据解析.USART1_RXD是数组数据。lenth是数组中元素的数量。
{
	  unsigned char Cmd[30]={0};//命令字节。
		unsigned char Dat[70]={0};//数据字节。
		unsigned char DatA[30]={0};//数据字节。		
    unsigned char Datlng=0;
		unsigned char BackFlag=0;
		unsigned char CmdFlag=0;
		//命令代码表		
		unsigned char OrderCode[8][10]=
		{
			"10A_MIM=","10A_IDS=","10A_WLS=","10A_YIP=","10A_YDK=","10A_YMC=","10A_DZS=","10A_ZSX="
		};
		if((USART1_RXD[0]!=0x3C)||(USART1_RXD[lenth-1]!=0x3E))
		{
			printf("命令格式错误！命令后面不应加回车！\r\n");//FLASH擦除；		 
			return 1;
		}
		memset(Cmd,0,30);
		memset(Dat,0,70);
		memset(DatA,0,30); 
		
		if( (USART1_RXD[1]==0x42)&&(USART1_RXD[2]==0x54)&&(USART1_RXD[3]==0x2B) ) 
		{
      memcpy(Cmd,&USART1_RXD[4],8);//拷贝命令；
			if(lenth>13)//有数据位
			{Datlng=lenth-13;}
			else //没有数据位
			{}
			memcpy(Dat,&USART1_RXD[12],Datlng);//拷贝数据；
		}
		else
		{	
		}
 
		if(Sting_Compare(OrderCode[0],Cmd,8))//密码命令
		{
			if((Datlng==5)&&(Dat[0]==0x46)&&(Dat[1]==0x42)&&(Dat[2]==0x31)&&(Dat[3]==0x31)&&(Dat[4]==0x41))//打印调试信息
			{  MIM_flag=1;
				 PZM_flag=0;
				 MIM_flag_Count=0;
			   printf("密码正确，调试打印！\r\n");			
			}
			else if((Datlng==5)&&(Dat[0]==0x46)&&(Dat[1]==0x42)&&(Dat[2]==0x31)&&(Dat[3]==0x31)&&(Dat[4]==0x42))//进行
			{
				 PZM_flag=1;
				 MIM_flag=0;
				 MIM_flag_Count=0;
			   printf("密码正确,配置使能！\r\n");
				 printf("********注意：配置参数后复位设备*******\r\n");
				 printf("系统参数：\r\n");
			   ConfigSystemDataPrint();                       //系统参数；
				 printf("**************************************\r\n");							
			}
			else
			{
 				 MIM_flag=0;
			   printf("密码错误！\r\n");
			}
		}
		else
		{
			if(PZM_flag==1)//密码正确
			{
				CmdFlag=0;
				if(Sting_Compare(OrderCode[1],Cmd,8))//主机ID设置(生产专用)
				{ 
					printf("无效命令！\r\n");
					CmdFlag=1;
//					if(Datlng==12)
//					{
//					data_transform_check(DatA,Dat,30);
// 		      CmdFlag=ConfigDataSet(1,DatA,Datlng); 	
//					}
//          else
//          {printf("长度错误！\r\n");
//					CmdFlag=1;}
				}
				else if(Sting_Compare(OrderCode[2],Cmd,8))//网络设置（1=有线，2=无线） 
				{
					DatA[0]=Dat[0]-0x30;
 		      CmdFlag=ConfigDataSet(2,DatA,Datlng); 
				}
		    else if(Sting_Compare(OrderCode[3],Cmd,8))//有线网络域名
				{					
 		      CmdFlag=ConfigDataSet(3,Dat,Datlng);
				}
			  else if(Sting_Compare(OrderCode[4],Cmd,8))//有线网络端口
				{
					data_transform_YM(DatA,Dat,Datlng);
 		      CmdFlag=ConfigDataSet(4,DatA,Datlng);
				}
				else if(Sting_Compare(OrderCode[5],Cmd,8))//有线网络MAC(生产专用)
				{
					printf("无效命令！\r\n");
					CmdFlag=1;					
//					if(Datlng==12)
//					{
//					data_transform_check(DatA,Dat,30);
// 		      CmdFlag=ConfigDataSet(5,DatA,Datlng);
//					}
//          else
//          {printf("长度错误！\r\n");
//					CmdFlag=1;}						
				}
				else if(Sting_Compare(OrderCode[6],Cmd,8))//有线本机IP写入
				{
					data_transform_checkA(DatA,Dat,70);
 		      CmdFlag=ConfigDataSet(6,DatA,0x10);
				}
				else if(Sting_Compare(OrderCode[7],Cmd,8))//IP自动，手动选择
				{
					DatA[0]=Dat[0]-0x30;
 		      CmdFlag=ConfigDataSet(7,DatA,Datlng);
				}				
				else
				{
					printf("无效命令:\r\n");
				}
				
				if(CmdFlag==1) 
				{
					printf("命令错误，或重复写入！\r\n");
				}
				else
				{
					printf("成功写入！\r\n");
				}				
				
			}
			else
			{
				printf("命令错误！\r\n");//FLASH擦除；		 
			}				
		}	
		return 1;		
}
//////////////////////////////////////////////////////////////////////////
 void ATcommond_Logic(void)//AT命令解析；
{	
	//读取数据
	AT_RecieveFrame.Lenth=Uart1_Rx_Read(AT_RecieveFrame.data);
	if(AT_RecieveFrame.Lenth!=0)
  {
		Configure_Data_Analyze(AT_RecieveFrame.data,AT_RecieveFrame.Lenth);
	}
}
