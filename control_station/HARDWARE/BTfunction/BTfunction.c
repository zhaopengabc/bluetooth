
#include "BTfunction.h"
#include "includes.h"
#include "spi_flash.h"

//远程升级
#define BTVersion 0x10

BT_frame BTBuffer_frame;

#ifdef FB1010
unsigned char gDeviceType=0x71;
#endif

#ifdef FB1030
unsigned char gDeviceType=0x72;
#endif
unsigned short gBTCont=0;
//BT功能初始化；
extern unsigned char   Local_Addres[6];
static unsigned short  FlashWriteLength=256;
extern const unsigned short  VERS_msg;
unsigned char  VERS_msg_New1=0;
unsigned char  VERS_msg_New2=0;

static unsigned char  BT_Status=0;

void BTBuffer_frameInit(unsigned char typ,unsigned char * data)
{
	unsigned char che=0;
	unsigned char i=0;
	unsigned char *add;
		
	BTBuffer_frame.m_HeaderL=0x40;
	BTBuffer_frame.m_HeaderH=0x40;	
  BTBuffer_frame.m_DeviceType=gDeviceType;
  BTBuffer_frame.m_CountL=gBTCont;
	BTBuffer_frame.m_CountH=gBTCont>>8;
	
	BTBuffer_frame.m_VersionL=VERS_msg_New1;	//版本信息；
	BTBuffer_frame.m_VersionH=VERS_msg_New2;			
	
	memset(BTBuffer_frame.m_SN,0,15);
	memcpy(BTBuffer_frame.m_SN,Local_Addres,6);//存ID
	
	memset(BTBuffer_frame.m_IMEI,0,15);
	
	memset(BTBuffer_frame.m_PhoneNumber,0,11);	
	
	BTBuffer_frame.m_Type=typ;		
	BTBuffer_frame.m_Encrypt=0x00;
	BTBuffer_frame.m_LengthL=0x00;
	BTBuffer_frame.m_LengthH=0x00;
	memcpy(BTBuffer_frame.m_Data,data,4);
	
	if(typ==0xBB)//开始\结束下载上传旧版本；
	{
		BTBuffer_frame.m_Data[0]=VERS_msg;	//版本信息；
		BTBuffer_frame.m_Data[1]=VERS_msg>>8;	
	}
	else//其它新版本；
	{

  }		
	
	//校验和
	add=&BTBuffer_frame.m_CountL;
	for(i=0;i<53;i++)
	{
	    che=che+*add;
		  add++;
	}
	BTBuffer_frame.m_Check=che;
	
	BTBuffer_frame.m_EndL =0x23;
	BTBuffer_frame.m_EndH =0x23;
	
	gBTCont++;
}

//BT功能初始化；
unsigned char BT_FunctionInit(void)
{
	unsigned char Type=0;
	
  return Type;
}

//BT数据解析；
static unsigned int PackageNumber;

unsigned char BT_DataFrame(unsigned char *data,unsigned int dataLength)
{
	unsigned char Type=0;
	unsigned char chk=0;
  unsigned short i,lenth;
 
	if((data[0]==0x41)&&(data[1]==0x41))
	{			
		lenth=data[5];
		lenth=lenth<<8;
		lenth=lenth+data[4];		
	
		for(i=2;i<lenth+1;i++)
	  {
	    chk+=data[i];
		}		
		if(chk==data[lenth+1])
		{
		  Type=data[21];		
		}
		else
		{	 printf("校验错误！%x=%x\r\n",chk,data[lenth+1]);}
		
   PackageNumber =  data[3u];
	 PackageNumber =  PackageNumber<<8;
   PackageNumber =  PackageNumber + data[2u];
	
	 
	 printf("\r\n");
	 printf("数据包序列号=%d！\r\n",PackageNumber);		
	}	
	
  return Type;
}
/*****************************************************************************
* 函数名称: void BtDownloadBeginRequest(void)
* 功能描述: 开始下载响应；
* 参    数: 无
* 返回  值: 无
*****************************************************************************/
void BtDownloadBeginRequest(void)
{       
	  unsigned char dataS[4];
	  
	  memset(dataS,0,4);
	
    BTBuffer_frameInit(0xBB,dataS);
}
/*****************************************************************************
* 函数名称: void BtAbnormalRequest(void)
* 功能描述: 异常帧的响应
* 参    数: 无
* 返回  值: 无
*****************************************************************************/
void BtAbnormalRequest(unsigned int nbr)
{
		unsigned char dataS[4];
	  
	  memset(dataS,0,4);
		dataS[0]=BT_Status;		
	  if(BT_Status==0xA7)
		{
		dataS[1]=nbr;
		dataS[2]=nbr>>8;
		dataS[3]=nbr>>16;
    }
    BTBuffer_frameInit(0xEE,dataS);//上发数据；
}

/*****************************************************************************
* 函数名称: void BtFlashEraseRequest(void)
* 功能描述: Flash擦出帧响应
* 参    数: 无
* 返回  值: 无
APP程序下载存储区
大小：    0x040000（262144B=4096*64）
起始地址：0x043000
结束地址：0x083000
*****************************************************************************/
//static unsigned char tmpDATA[256];
unsigned int W25Q16FlashErase(unsigned int address)
{
	  unsigned char i,j;
	  unsigned short z,nbr=0;	
	  unsigned char tmpDATA[256];
	
	  printf("Flash擦除开始\r\n");
	  for(i=0;i<64;i++)//擦除64个扇区；
	  {
			//擦除APP下载存储区；
			SPI_FLASH_SectorErase(address);
			//回读，校验擦除是否正确;
			nbr=0;
			for(j=0;j<16;j++)//4096/256
			{ memset(tmpDATA,0x12,256);
				SPI_FLASH_BufferRead(tmpDATA,address+nbr,256);
				for(z=0;z<256;z++)//每个字节判断；
				{
					if(tmpDATA[z]!=0xFF)//如果擦除失败；
				  {
					 printf("Flash擦除校验异常_%x",address);	
					 return 10001;//故障码；
			     }
				}
				nbr=nbr+256;
			}
	
			printf("Flash擦除正常_%x ",address);			
			address+=0x1000;							 //4096字节；
	  }
		
		printf("Flash擦除成功\r\n");
		return 2;
}

unsigned char BtFlashEraseRequest(void)
{
	  unsigned char dataS[4];
	  unsigned int add=0;
	  unsigned int nbr=0;
			
	  memset(dataS,0,4);
	  
	  add=APP_DownLoadAddress;
		nbr=W25Q16FlashErase(add);
		  
    if(nbr==2)//第一次擦除正确;
		{ 
			add=APP_RecoverAddress;
		  nbr=W25Q16FlashErase(add);		
		}

		if(nbr==2)//第二次擦除正确;	
    {				
      BTBuffer_frameInit(0xBD,dataS);//上发数据；
      return 1;			
    }		
		
		BtAbnormalRequest(nbr);//上发异常包；
    
		return 0;	
}
/*****************************************************************************
* 函数名称: void BtDataTranseferRequest(void)
* 功能描述: 数据下载帧响应
* 参    数: 无
* 返回  值: 无
* 内容描述：数据下载，数据区，有线256长度；
*****************************************************************************/
unsigned char W25Q16FlashWrite(unsigned char *data,unsigned int dataLength)
{
	  unsigned int address=APP_DownLoadAddress;
	  unsigned char i,j;
	  unsigned short z,nbr=0;	
	 	unsigned char tmp[256];
	  static unsigned int addAdd=0;
    
	  //写入数据； 
	  address=address+addAdd;
		SPI_FLASH_BufferWrite(&data[22],address,FlashWriteLength);
		//读取		
		SPI_FLASH_BufferRead(tmp,address,256);
	
	  for(z=0;z<FlashWriteLength;z++)
	  {
			if(data[22+z]!=tmp[z])
			{	printf("Flash写入异常_%x ",address);	
			  return 102;}
		}
//		printf("Flash写入正常_%x ",address);	
		printf("Flash写入正常%x: %x %x %x %x \r\n",address,tmp[0],tmp[1],tmp[2],tmp[3]);	
		
	  addAdd=addAdd+FlashWriteLength;
		
		return 2;
}

void BtDataTranseferRequest(unsigned char *data,unsigned int dataLength)
{
		static unsigned int address=APP_DownLoadAddress;
	  unsigned char dataS[4];
	  
	  memset(dataS,0,4);
	
	  dataS[0]=W25Q16FlashWrite(data,dataLength);

	  BTBuffer_frameInit(0xB9,dataS);
}

/*****************************************************************************
* 函数名称: void UsartReceiveMsg_ISR(uint8_t)
* 功能描述: 
* 参    数:
* 返回  值:
*****************************************************************************/
extern void SysSoftReset(void);
extern void BTFlashFlagRead(unsigned char * Indata);
extern unsigned int Net_Send_Infor(unsigned char *data,unsigned int Lenth);
extern void BTFlashFlagSet(unsigned char data);
//BT逻辑；
static unsigned int  Period_RecCnt=0;
static unsigned char BT_EEcot=1;

unsigned char BT_Logic(unsigned char *data,unsigned int dataLength)
{
	unsigned int Package_msg=0;
	unsigned char gReceiveType=0;		
		//数据格式校验；
		gReceiveType=BT_DataFrame(data,dataLength);
		//异常包，清零；
		if(gReceiveType!=0xEE)
    {Period_RecCnt=0;}
		//功能处理；
		BT_EEcot=8;
		switch(gReceiveType)//通讯协议数据类型
		{
			 case 0xA5://开始下载命令；
			 { 
				VERS_msg_New1=data[22];	//新版本号的接收；
				VERS_msg_New2=data[23];	
				BtDownloadBeginRequest(); //握手；
				printf("1.A5开始命令,版本：%x_%X\r\n\r\n",VERS_msg_New1,VERS_msg_New2);		
				BT_Status=0xA5;			 
				break;
			 }
			case 0xA3://擦除Flash命令；
			 { 
				BtFlashEraseRequest(); //擦除外部FLASH；
				Package_msg=0;
				Package_msg=data[23];//数据传输包的记录；	
				Package_msg=Package_msg<<8;
				Package_msg=Package_msg+data[22];						
				printf("2.A3擦除命令,总包数=%d！\r\n\r\n",Package_msg);
				BT_Status=0xA3;			
				break;
			 }
			case 0xA7://数据传输命令；
			 {
				BtDataTranseferRequest(data,dataLength);   //接收数据；
				printf("3.A7传输命令！\r\n\r\n");
				BT_Status=0xA7;
				break;
			 } 	
			case 0xA1://进入BT程序指令；
			 { 
				BTFlashFlagSet(0xAA); //程序更新标志位职位；
				BT_Status=0xAB;
				printf("4.********************退出APP********************\r\n\r\n");				 
				SysSoftReset();		//跳转BT程序； 
				break;
			 }	
			default: //异常状态帧；
			{
				printf("0.XX异常返回\r\n\r\n");			
				BtAbnormalRequest(0);
				break;
			}	
		}

	//发送数据；
	Net_Send_Infor(&BTBuffer_frame.m_HeaderL,sizeof (BTBuffer_frame));
	
	return 1;
}

//BT程序 运行结果上报；
unsigned char BT_SucessFlag=0;
unsigned char BT_SucessDATA[4];
unsigned char BT_HeartFlag=0;

void BT_SucessInfor(void)
{
	 if((BT_HeartFlag!=0)&&(BT_SucessFlag>1))//与服务器有心跳后，上发成功标志位；
	 {
		 BT_SucessFlag--;
	  
		 if((BT_SucessDATA[0]==0xAB)&&(BT_SucessDATA[1]==0x00))
		 {BTBuffer_frameInit(0xB5,BT_SucessDATA);}//上发数据；
	   else
		 {BTBuffer_frameInit(0xEE,BT_SucessDATA);}//上发数据；
		 
		 Net_Send_Infor(&BTBuffer_frame.m_HeaderL,sizeof (BTBuffer_frame));
		 
		 BTFlashFlagSet(1);
	 }
}

//BT过程 周期异常上报；
void BT_PeriodTask(void)
{
	//升级中,上发异常包；
	if(BT_Status!=0)
	{	
	  Period_RecCnt++;	
		printf("Period_RecCnt=%d！\r\n",Period_RecCnt);
	  if(Period_RecCnt%3==0)
	  {
				if(BT_EEcot>1)
				{
					 BT_EEcot--;
					 BtAbnormalRequest(PackageNumber+1);
					 //发送数据；
					 Net_Send_Infor(&BTBuffer_frame.m_HeaderL,sizeof (BTBuffer_frame));
			  }
	  }
	}
	 //上报，升级成功；
 	 BT_SucessInfor(); 
}



