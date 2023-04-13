
#include "BTfunction.h"
#include "includes.h"
#include "spi_flash.h"

//Զ������
#define BTVersion 0x10

BT_frame BTBuffer_frame;

#ifdef FB1010
unsigned char gDeviceType=0x71;
#endif

#ifdef FB1030
unsigned char gDeviceType=0x72;
#endif
unsigned short gBTCont=0;
//BT���ܳ�ʼ����
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
	
	BTBuffer_frame.m_VersionL=VERS_msg_New1;	//�汾��Ϣ��
	BTBuffer_frame.m_VersionH=VERS_msg_New2;			
	
	memset(BTBuffer_frame.m_SN,0,15);
	memcpy(BTBuffer_frame.m_SN,Local_Addres,6);//��ID
	
	memset(BTBuffer_frame.m_IMEI,0,15);
	
	memset(BTBuffer_frame.m_PhoneNumber,0,11);	
	
	BTBuffer_frame.m_Type=typ;		
	BTBuffer_frame.m_Encrypt=0x00;
	BTBuffer_frame.m_LengthL=0x00;
	BTBuffer_frame.m_LengthH=0x00;
	memcpy(BTBuffer_frame.m_Data,data,4);
	
	if(typ==0xBB)//��ʼ\���������ϴ��ɰ汾��
	{
		BTBuffer_frame.m_Data[0]=VERS_msg;	//�汾��Ϣ��
		BTBuffer_frame.m_Data[1]=VERS_msg>>8;	
	}
	else//�����°汾��
	{

  }		
	
	//У���
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

//BT���ܳ�ʼ����
unsigned char BT_FunctionInit(void)
{
	unsigned char Type=0;
	
  return Type;
}

//BT���ݽ�����
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
		{	 printf("У�����%x=%x\r\n",chk,data[lenth+1]);}
		
   PackageNumber =  data[3u];
	 PackageNumber =  PackageNumber<<8;
   PackageNumber =  PackageNumber + data[2u];
	
	 
	 printf("\r\n");
	 printf("���ݰ����к�=%d��\r\n",PackageNumber);		
	}	
	
  return Type;
}
/*****************************************************************************
* ��������: void BtDownloadBeginRequest(void)
* ��������: ��ʼ������Ӧ��
* ��    ��: ��
* ����  ֵ: ��
*****************************************************************************/
void BtDownloadBeginRequest(void)
{       
	  unsigned char dataS[4];
	  
	  memset(dataS,0,4);
	
    BTBuffer_frameInit(0xBB,dataS);
}
/*****************************************************************************
* ��������: void BtAbnormalRequest(void)
* ��������: �쳣֡����Ӧ
* ��    ��: ��
* ����  ֵ: ��
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
    BTBuffer_frameInit(0xEE,dataS);//�Ϸ����ݣ�
}

/*****************************************************************************
* ��������: void BtFlashEraseRequest(void)
* ��������: Flash����֡��Ӧ
* ��    ��: ��
* ����  ֵ: ��
APP�������ش洢��
��С��    0x040000��262144B=4096*64��
��ʼ��ַ��0x043000
������ַ��0x083000
*****************************************************************************/
//static unsigned char tmpDATA[256];
unsigned int W25Q16FlashErase(unsigned int address)
{
	  unsigned char i,j;
	  unsigned short z,nbr=0;	
	  unsigned char tmpDATA[256];
	
	  printf("Flash������ʼ\r\n");
	  for(i=0;i<64;i++)//����64��������
	  {
			//����APP���ش洢����
			SPI_FLASH_SectorErase(address);
			//�ض���У������Ƿ���ȷ;
			nbr=0;
			for(j=0;j<16;j++)//4096/256
			{ memset(tmpDATA,0x12,256);
				SPI_FLASH_BufferRead(tmpDATA,address+nbr,256);
				for(z=0;z<256;z++)//ÿ���ֽ��жϣ�
				{
					if(tmpDATA[z]!=0xFF)//�������ʧ�ܣ�
				  {
					 printf("Flash����У���쳣_%x",address);	
					 return 10001;//�����룻
			     }
				}
				nbr=nbr+256;
			}
	
			printf("Flash��������_%x ",address);			
			address+=0x1000;							 //4096�ֽڣ�
	  }
		
		printf("Flash�����ɹ�\r\n");
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
		  
    if(nbr==2)//��һ�β�����ȷ;
		{ 
			add=APP_RecoverAddress;
		  nbr=W25Q16FlashErase(add);		
		}

		if(nbr==2)//�ڶ��β�����ȷ;	
    {				
      BTBuffer_frameInit(0xBD,dataS);//�Ϸ����ݣ�
      return 1;			
    }		
		
		BtAbnormalRequest(nbr);//�Ϸ��쳣����
    
		return 0;	
}
/*****************************************************************************
* ��������: void BtDataTranseferRequest(void)
* ��������: ��������֡��Ӧ
* ��    ��: ��
* ����  ֵ: ��
* �����������������أ�������������256���ȣ�
*****************************************************************************/
unsigned char W25Q16FlashWrite(unsigned char *data,unsigned int dataLength)
{
	  unsigned int address=APP_DownLoadAddress;
	  unsigned char i,j;
	  unsigned short z,nbr=0;	
	 	unsigned char tmp[256];
	  static unsigned int addAdd=0;
    
	  //д�����ݣ� 
	  address=address+addAdd;
		SPI_FLASH_BufferWrite(&data[22],address,FlashWriteLength);
		//��ȡ		
		SPI_FLASH_BufferRead(tmp,address,256);
	
	  for(z=0;z<FlashWriteLength;z++)
	  {
			if(data[22+z]!=tmp[z])
			{	printf("Flashд���쳣_%x ",address);	
			  return 102;}
		}
//		printf("Flashд������_%x ",address);	
		printf("Flashд������%x: %x %x %x %x \r\n",address,tmp[0],tmp[1],tmp[2],tmp[3]);	
		
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
* ��������: void UsartReceiveMsg_ISR(uint8_t)
* ��������: 
* ��    ��:
* ����  ֵ:
*****************************************************************************/
extern void SysSoftReset(void);
extern void BTFlashFlagRead(unsigned char * Indata);
extern unsigned int Net_Send_Infor(unsigned char *data,unsigned int Lenth);
extern void BTFlashFlagSet(unsigned char data);
//BT�߼���
static unsigned int  Period_RecCnt=0;
static unsigned char BT_EEcot=1;

unsigned char BT_Logic(unsigned char *data,unsigned int dataLength)
{
	unsigned int Package_msg=0;
	unsigned char gReceiveType=0;		
		//���ݸ�ʽУ�飻
		gReceiveType=BT_DataFrame(data,dataLength);
		//�쳣�������㣻
		if(gReceiveType!=0xEE)
    {Period_RecCnt=0;}
		//���ܴ���
		BT_EEcot=8;
		switch(gReceiveType)//ͨѶЭ����������
		{
			 case 0xA5://��ʼ�������
			 { 
				VERS_msg_New1=data[22];	//�°汾�ŵĽ��գ�
				VERS_msg_New2=data[23];	
				BtDownloadBeginRequest(); //���֣�
				printf("1.A5��ʼ����,�汾��%x_%X\r\n\r\n",VERS_msg_New1,VERS_msg_New2);		
				BT_Status=0xA5;			 
				break;
			 }
			case 0xA3://����Flash���
			 { 
				BtFlashEraseRequest(); //�����ⲿFLASH��
				Package_msg=0;
				Package_msg=data[23];//���ݴ�����ļ�¼��	
				Package_msg=Package_msg<<8;
				Package_msg=Package_msg+data[22];						
				printf("2.A3��������,�ܰ���=%d��\r\n\r\n",Package_msg);
				BT_Status=0xA3;			
				break;
			 }
			case 0xA7://���ݴ������
			 {
				BtDataTranseferRequest(data,dataLength);   //�������ݣ�
				printf("3.A7�������\r\n\r\n");
				BT_Status=0xA7;
				break;
			 } 	
			case 0xA1://����BT����ָ�
			 { 
				BTFlashFlagSet(0xAA); //������±�־λְλ��
				BT_Status=0xAB;
				printf("4.********************�˳�APP********************\r\n\r\n");				 
				SysSoftReset();		//��תBT���� 
				break;
			 }	
			default: //�쳣״̬֡��
			{
				printf("0.XX�쳣����\r\n\r\n");			
				BtAbnormalRequest(0);
				break;
			}	
		}

	//�������ݣ�
	Net_Send_Infor(&BTBuffer_frame.m_HeaderL,sizeof (BTBuffer_frame));
	
	return 1;
}

//BT���� ���н���ϱ���
unsigned char BT_SucessFlag=0;
unsigned char BT_SucessDATA[4];
unsigned char BT_HeartFlag=0;

void BT_SucessInfor(void)
{
	 if((BT_HeartFlag!=0)&&(BT_SucessFlag>1))//����������������Ϸ��ɹ���־λ��
	 {
		 BT_SucessFlag--;
	  
		 if((BT_SucessDATA[0]==0xAB)&&(BT_SucessDATA[1]==0x00))
		 {BTBuffer_frameInit(0xB5,BT_SucessDATA);}//�Ϸ����ݣ�
	   else
		 {BTBuffer_frameInit(0xEE,BT_SucessDATA);}//�Ϸ����ݣ�
		 
		 Net_Send_Infor(&BTBuffer_frame.m_HeaderL,sizeof (BTBuffer_frame));
		 
		 BTFlashFlagSet(1);
	 }
}

//BT���� �����쳣�ϱ���
void BT_PeriodTask(void)
{
	//������,�Ϸ��쳣����
	if(BT_Status!=0)
	{	
	  Period_RecCnt++;	
		printf("Period_RecCnt=%d��\r\n",Period_RecCnt);
	  if(Period_RecCnt%3==0)
	  {
				if(BT_EEcot>1)
				{
					 BT_EEcot--;
					 BtAbnormalRequest(PackageNumber+1);
					 //�������ݣ�
					 Net_Send_Infor(&BTBuffer_frame.m_HeaderL,sizeof (BTBuffer_frame));
			  }
	  }
	}
	 //�ϱ��������ɹ���
 	 BT_SucessInfor(); 
}



