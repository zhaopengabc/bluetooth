//���¼�¼
//100~200���ļ���ſ��Ե��뵽Flash�С�


/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "SDcontrol.h"
#include "w25q16.h"
#include <stdio.h> 
#include <lcd.h> 
//#include "menu.h"
#include "wdg.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#define readDataCot 3172 //3k�ļ���ȡ��
int FileNBR=1;

FATFS fs;         /* Work area (file system object) for logical drive */
FIL fsrc, fdst;      /* file objects */
FRESULT res;
UINT br,bw;
char path0[50]="0:";
//char buffer[100];   /* file copy buffer */
//uint8_t textFileBuffer[] = "��Ӣ�Ĳ����ַ��� \r\nChinese and English test strings \r\n";

uint8_t textFileBuffer[2050] = {0};
//uint8_t textFileBuffer[1] = {0};
uint8_t FBWB[50];
uint8_t FBWB1[] = "0:/FBWB_0.txt";
uint8_t FBWB2[] = "0:/FBWB_00.txt";
uint8_t FBWB3[] = "0:/FBWB_000.txt";

float FBWBReadStatus=0; //�����ȡ���ȣ�
int FBWBReadNbr=0; //�����ȡ��ǰ��ȡ��������
int FBWBReadFail=0; //�����ȡ��ǰ��ȡ��������

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
  * @��������	Delay
  * @����˵��   ʵ����ʱ���� 
  * @�������   nCount: ��ʱʱ���������ֵԽ����ʱʱ��Խ�� 
  * @�������   ��
  * @���ز���   ��
  * @ע������	��
  *****************************************************************************/
void  Delay (uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}
/*******************************************************************************
  * @��������	�ļ����ֶԱȣ�
  *****************************************************************************/
u8 FileNameCpr(char* add)
{
	 u8 i;
	 for(i=0;i<5;i++)
	{
		 if(FBWB1[i+3]!=*(add+i) )
     {
         return 0;
     }		 
	}
	
	if(i==5)//��ȫƥ�䵼�����ƣ�
	{return 1;}
}

/*******************************************************************************
  * @��������	scan_files
  * @����˵��   �����ļ�Ŀ¼�������ļ� 
  * @�������   path: ��Ŀ¼ 
  * @�������   ��
  * @���ز���   FRESULT
  * @ע������	��
  *****************************************************************************/
u8 FBWBflag=0;
FRESULT scan_files (
    char* path        /* Start node to be scanned (also used as work area) */
)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
    static char lfn[_MAX_LFN + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif

		FileNBR=0;
    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        i = strlen(path);
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                sprintf(&path[i], "/%s", fn);//20180628 ��ֹ���Σ��������
 				        printf("scan file - %s\n\r",path);//20180628 ��ֹ���Σ��������
                res = scan_files(path);
                if (res != FR_OK) break;
                path[i] = 0;
            }else{                                       /* It is a file. */
//                printf("scan file - %s/%s\n\r", path, fn);//��ȡ�ļ�������
 							  FBWBflag=FileNameCpr(fn);//��0��/FBWB_ƥ�䣻
							  FileNBR++;//�ļ�������
            }
        }
    }else{
//		printf("scan files error : %d\n\r",res);
	}

    return res;
}
/*******************************************************************************
  * @��������	SD_TotalSize
  * @����˵��   �ļ��ռ�ռ����� 
  * @�������   �� 
  * @�������   ��
  * @���ز���   1: �ɹ� 
  				0: ʧ��
  * @ע������	��
  *****************************************************************************/
int SD_TotalSize(char *path)
{
    FATFS *fs;
    DWORD fre_clust;        

    res = f_getfree(path, &fre_clust, &fs);  /* �����Ǹ�Ŀ¼��ѡ�����0 */
    if ( res==FR_OK ) 
    {
	  printf("\n\rget %s drive space.\n\r",path);
	  /* Print free space in unit of MB (assuming 512 bytes/sector) */
      printf("%d MB total drive space.\r\n"
           "%d MB available.\r\n",
           ( (fs->n_fatent - 2) * fs->csize ) / 2 /1024 , (fre_clust * fs->csize) / 2 /1024 );
		
	  return 1;
	}
	else
	{ 
	  printf("\n\rGet total drive space faild!\n\r");
	  return 0;   
	}
}


////////////SD����///////////////
//��һ����Ϣд��Flash�У���д��ȷ���򷵻�1��д�����򷵻�0��������ʾFlash����
//��¼д��ĵ�ַWrite_Flash_Addr,
//д������Զ���20((0x20))���ȴ��´�д��;
//((0x16-0x1Bд��ʱ���ǩ��˳��Ϊ���ʱ������ ))
//0x1C-0x1Fд�����к�Flash_sn��˳�μ�1��
//�����������󣬼�20��д����һ����飻�ظ�3���������ɹ���ȥ��һ��д�룻���Ȳ�����д�룩
//д����ַ0x20000���Զ���Ϊ0,�����ܹ�����д��4096�����ݣ�
//���д���ַΪ0x1000��������ʱ����ִ��һ�β����������д�룻
//��¼�����ַ��Ϣ�ݲ����ǣ�������Ϣֱ��������
//��¼�ܹ�����Ϣ������int�������ۼӣ���¼������β����
//��ڣ�д����ַ���������

static unsigned int SD_Write_Flash_Addr=0x100000;
static unsigned int SD_Read_Flash_Addr=0x100000;
static unsigned int SD_Flash_sn=1;

unsigned char SD_Writeflash_Infor(u8* pBuffer, u16 NumByteToWrite)
{
	unsigned char buf[0x20];
	unsigned int i,j;

  if(SD_Flash_sn==1) 	 SPI_FLASH_SectorErase(SD_Write_Flash_Addr); 
	
	if(NumByteToWrite>0x1C)return 0;
	for(j=0;j<3;j++)
	{
		SPI_FLASH_BufferRead(buf,SD_Write_Flash_Addr,0x20);
		for(i=0;i<0x20;i++)
		{
			if(buf[i]!=0xff)break;
		}
		if(i==0x20)
		{
			printf("�����=%d,",SD_Flash_sn); 
			FBWBReadNbr=SD_Flash_sn;
			
			SPI_FLASH_BufferWrite(pBuffer,SD_Write_Flash_Addr,NumByteToWrite);
			memset(buf,0,0x20);
			SPI_FLASH_BufferRead(buf,SD_Write_Flash_Addr,NumByteToWrite);//�ض�;
			for(i=0;i<NumByteToWrite;i++)
			{
				if(buf[i]!=*(pBuffer+i))break;
			}
			if(i==NumByteToWrite)//		д�����ݳɹ���
			{
			//д����ȷ����д�����к�,������Ҫ��ȷ����洢λ��ʱ����ӣ�
			//�ڶ���Ϣʱͨ�����˺��룬�ж��Ƿ���ȷ����ֹ���鵼�¶�������ֵ��
				buf[0x1C]=(unsigned char)SD_Flash_sn;
				buf[0x1D]=(unsigned char)(SD_Flash_sn>>8);
				buf[0x1E]=(unsigned char)(SD_Flash_sn>>16);
				buf[0x1F]=(unsigned char)(SD_Flash_sn>>24);
				SPI_FLASH_BufferWrite(&buf[0x1C],SD_Write_Flash_Addr+0x1C,4);//д��ţ�
				
				SD_Flash_sn++;//�´�д�룻
				SD_Write_Flash_Addr+=0x20;//�´�д�룻
				if(SD_Write_Flash_Addr>=0x1A0000)
					SD_Write_Flash_Addr=0x19FFE0;//���һ��������
				if(SD_Write_Flash_Addr%0x1000==0)
					SPI_FLASH_SectorErase(SD_Write_Flash_Addr);//��һ֡�������
//				Infor_Record_Num(buf,10);
//				Infor_Parameter_Save();
				return 1;
			}
		}
	//������������д�����		
		SD_Write_Flash_Addr+=0x20;
	if(SD_Write_Flash_Addr>=0x1A0000)
	 SD_Write_Flash_Addr=0x19FFE0;
	if(SD_Write_Flash_Addr%0x1000==0)//������һ�飻
	 SPI_FLASH_SectorErase(SD_Write_Flash_Addr);
	}
	return 0;
}

//��ȡ��sn����Ϣ��ŵ�pBuffer��
//��һ��ΪWrite_Fassh_Addr-0x20
//��n��ΪWrite_Fassh_Addr-0x20*n
//��n�������к�ΪFlash_sn-sn��������鿴���к��Ƿ���ȷ��
//����ȷ������1������ȷ���ж�Flash_sn-sn��tmp_sn�Ĳ
//����С��256�����Ų�ٴβ�ѯ���˱�Ų������ԭ��Ϊ
//����д���ɹ��������+1�ٴβ��ң�
unsigned char SD_Readflash_Infor(u8* pBuffer, unsigned int sn)
{
	unsigned char buf[0x20];
	memset(buf,0,0x20);
	SPI_FLASH_BufferRead(buf,SD_Read_Flash_Addr+sn*0x20,0x20);
	memcpy(pBuffer,buf,0x20);	
}
/////////////////////////////
/////////////////////////////

u8 filesysint(void)
{
	u8 backflag=0;
	//����SPI1�ӿ�
	MSD0_SPI_Configuration();
	//�����ļ�ϵͳ
 	res = f_mount(0,&fs); 
 	if(res != FR_OK){
		  
  		printf("�ļ�����ʧ��: %d\n\r",res);
 	}
	else
	{   
 		  printf("�ļ�����ɹ� : %d\n\r",res);
	}

 	backflag=scan_files(path0);
  return backflag;					
}

//uint8_t YHXXa[] = "0:/Information_00.txt";
uint8_t YHXX[] = "0:/Information_00.txt";
uint8_t HexToChar(uint8_t temp)
{
    uint8_t dst;
    if (temp < 10){
        dst = temp + '0';
    }else{
        dst = temp -10 +'A';
    }
    return dst;
}
//��ʷ������Ϣ������		
void filesysInfRed(void)
{
	u16 i=0,j=0;
	u8 d[36];
//	u8 dd[98]; 
	int count=0;
	u16 NBR=0,nbr0=0;
	u8 m=0;
	d[34]=0x0D;
	d[35]=0x0A;
	
	for(NBR=0;NBR<100;NBR++)//100
	{	
		IWDG_Feed();//ι��
		nbr0=30*NBR+1;
		for(j=nbr0;j<30+nbr0;j++)//30
		{
		  d[0]=j>>8;
			d[1]=j;
  		memset(&d[2],0,32);
			Readflash_Infor(&d[2], j);

// 			printf("\n\rj=%d,",j);			
			for(i=0;i<34;i++) 
			{
				textFileBuffer[count++]=HexToChar(d[i]>>4);

				textFileBuffer[count++]=HexToChar(d[i]&0x0F);

				textFileBuffer[count++]=0x20;
			}
				textFileBuffer[count++]=0x0D;
				textFileBuffer[count++]=0x0A;
		}
		
		//NBR1  97
	  m=0;
	  YHXX[15]=NBR/10;//�������ļ�����
	  m=YHXX[15]*10;
	  YHXX[15]+=0x30;
	  YHXX[16]=NBR-m+0x30;//�������ļ�����
//	  printf("n: %d\n\r",NBR);
	  printf("FILE: %s\n\r",YHXX);
			
	//�����ļ�ϵͳ
	res = f_open(&fdst, (TCHAR *)YHXX , FA_CREATE_ALWAYS | FA_WRITE);
	if(res != FR_OK){
	//	printf("open file error : %d\n\r",res);
	}else{
	    res = f_write(&fdst, textFileBuffer, count, &bw);               /* Write it to the dst file */
		if(res == FR_OK){
//  		printf("write data ok! %d\n\r",bw);
		}else{
//			printf("write data error : %d\n\r",res);
		}
		/*close file */
		f_close(&fdst);
	}
	  count=0;
	}	
}

u8 FLASHsave[32]; 
u8 FLASHsaveNBR=0;
//ÿ2K����洢��
u16 DataFlashSave(uint8_t *path,u16 nbr) 
{	
	u16 i,b=0;
  u8 Rlt=0;
	
  for(i=0;i<nbr;i++)	
	{ 
		if(*(path+i)!=0x0A)//���ǰ�δ��
		{ 
	    FLASHsave[FLASHsaveNBR++]=*(path+i);
		}
		else if(*(path+i)==0x0A )//�ǰ�δ��
    {
		  Rlt=SD_Writeflash_Infor(FLASHsave, FLASHsaveNBR);//�洢1����
			if(Rlt==1){printf("д��ɹ�\n\r");}  
			else{printf("д��ʧ��\n\r");}
			memset(&FLASHsave,0,FLASHsaveNBR);			
			FLASHsaveNBR=0;
			if(Rlt==1)
			{ b++; }
			else
			{FBWBReadFail++;}
		}
		else  
    {
 
		}    
	}
	return b;
}

//�ļ����룻
u16 FilesysRedIn(void)
{
	unsigned int time_int_SD=0;	
	OS_ERR err;//	(ucos��)
	u8 m=0;
	int i;
	u16 NBR=0,B=0,Back=0;
	SD_Flash_sn=1;
  FBWBReadStatus=0; //�����ȡ���ȣ�
  FBWBReadNbr=0; //�����ȡ��ǰ��ȡ��������
  FBWBReadFail=0; //�����ȡ��ǰ��ȡ��������
  SD_Write_Flash_Addr=0x100000;//��λ
  SD_Read_Flash_Addr=0x100000;//��λ
	
	//�����ļ�ϵͳ��
 	res = f_mount(0,&fs);
 	if(res != FR_OK){
 		printf("�����ļ�ʧ�� : %d\n\r",res);
 	}
	else
	{
		printf("�����ļ��ɹ� : %d\n\r",res);
	}
	//��ȡ�ļ���
	scan_files(path0);
	printf("�ļ�����: %d\n\r",FileNBR);
	if(FileNBR==0)
	{
		return 1;
	}
	//������ʾ�����Ź���
	for(i=1;i<FileNBR+1;i++)
	{ 
		IWDG_Feed();//ÿ��ȡ���ļ�����λ���Ź���
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ2ms
		time_int_SD=time_int_SD+1;
		if(time_int_SD>=100)//1��һ��
		{
			time_int_SD=0;
//			display_SD_schedule();//��ʾSD�������û���������
		}
	//ȷ���ļ�����		
		memset(&FBWB,0,50);
		if((i>0)&&(i<10))//��ȡFBWB_1~FBWB_9���ļ���
		{
			FBWB1[8]=0x30;
			FBWB1[8]+=i;

			FBWBReadStatus=(float)i/FileNBR*100;
			printf("�ļ���: %d,����=%f%%��",i,FBWBReadStatus);
			printf("�ļ�·��: %s\n\r",FBWB1);
			memcpy(&FBWB,&FBWB1,sizeof(FBWB1));
		}
		else if((i>9)&&(i<100))//��ȡFBWB_10~FBWB_99���ļ���
		{
			NBR=i;
			m=0;		
			FBWB2[8]=NBR/10;//�������ļ�����
			m=FBWB2[8]*10;
			FBWB2[8]+=0x30;
			FBWB2[9]=NBR-m+0x30;//�������ļ�����
			
			FBWBReadStatus=(float)i/FileNBR*100;
			printf("�ļ���: %d,����=%f%%��",i,FBWBReadStatus);
			printf("�ļ�·��: %s\n\r",FBWB2);
			
			memcpy(&FBWB,&FBWB2,sizeof(FBWB1));
		}
		else if((i>99)&&(i<200))//��ȡFBWB_100~FBWB_200���ļ���
		{
			NBR=i;
			m=0;		
			FBWB3[8]=NBR/100;//�������ļ�����
			FBWB3[9]=(NBR-(FBWB3[8]*100))/10;
			FBWB3[10]=NBR-(FBWB3[8]*100)-(FBWB3[9]*10);

			FBWB3[8]+=0x30;
			FBWB3[9]+=0x30;
			FBWB3[10]+=0x30;		
			
			FBWBReadStatus=(float)i/FileNBR*100;
			printf("�ļ���: %d,����=%f%%��",i,FBWBReadStatus);
			printf("�ļ�·��: %s\n\r",FBWB3);
			
			memcpy(&FBWB,&FBWB3,sizeof(FBWB3));
		}
		else
		{
			return 2;
		}
		//���ļ����ݶ�ȡ��
		res = f_open(&fsrc, (const TCHAR *)FBWB, FA_OPEN_EXISTING | FA_READ);
					if(res != FR_OK)
					{ printf("open file error : %d\n\r",res); }
					else
					{ 
						res = f_read(&fsrc, textFileBuffer, readDataCot, &br);     /* Read a chunk of src file */ //��2k+200��
						if(res==FR_OK)
							{
 								printf("��ȡ�ļ��ֽ��������� : %d�ֽ�\n\r",br);
								printf("%s\n\r",textFileBuffer);
							}
						else
							{
			          printf("read file error : %d\n\r",res);
		          }
						/*close file */
						f_close(&fsrc);
					}	
	  //�ļ����ݴ洢��flash��
		B=DataFlashSave(textFileBuffer,br);	
		Back+=B;			
   	printf("��������=%d,ʧ������=%d\n\r",Back,FBWBReadFail);   					
	}		
	  Writeflash_Config((unsigned char *)&Back,ITEM_CONFIG_CMD);//�洢�����������FLASH�У�UCOSϵͳ�ã�
//  	Write_Config((unsigned char *)&Back,ITEM_CONFIG_CMD);//�洢�����������FLASH�У���UCOSϵͳ�ã�	
	FileNBR=0;
  return Back;
}
//������룻
u8 SDDataInFlagt=0;
u32 ItemNbr=0;
void SDdata_Init(void)
{
	unsigned char nbr[4];
	memset(nbr,0,4);
	ItemNbr=0;
	if(ItemNbr==0)
  {
		Readflash_Config((unsigned char *)&ItemNbr,ITEM_CONFIG_CMD);//UCOSϵͳ�ã�
//		Read_Config((unsigned char *)nbr,ITEM_CONFIG_CMD);//��UCOSϵͳ�ã�	
//		ItemNbr=nbr[3];
//		ItemNbr=ItemNbr<<8;
//		ItemNbr=ItemNbr+nbr[2];
//		ItemNbr=ItemNbr<<8;
//		ItemNbr=ItemNbr+nbr[1];
//		ItemNbr=ItemNbr<<8;
//		ItemNbr=ItemNbr+nbr[0];
	}
	if(ItemNbr>19000)//ʵ�ʿ��Դﵽ19000����
	{
		ItemNbr=19000;
	}
	else
	{}
}
void SD_DATAIN(void)
{
	printf("�������δ������\n\r");//���ݿռ�3kȡ����
//	FBWBReadStatus=0;
//	printf("������ȡ��ʼ��\n\r");
//	if((filesysint()==0)&&(FBWBflag==1)) //��
//	{			
//		FilesysRedIn();		
//	}
//	printf("�����ִ�н�����\n\r");	
}
//�����û������
typedef struct
{
	unsigned char length;
  unsigned char DATA[10];
}YHData_Typedef;
 
//�洢�û�����������

//��ȡ�û�����������
u32 LopAddSearch(u32 data)
{
	int i,back=0;
	u8 buf[32];
	u8 NBR[6];
	u8 j,z=0,h=0,g=0;
  u16 LOP=0,MID;
  u16 ADD=0;	
	YHData_Typedef YHData[3];
	u32 SaveData=0;
	u8 nbr=0;
	printf("�����Աȿ�ʼ����%d\n\r",data);	
	for(i=0;i<ItemNbr;i++)//���19000��������
   {
		 if(i%0x100==0)
     {IWDG_Feed();} //ÿ100���Ķ�ȡ��ι����
		 memset(&YHData[0].length,0,11);
		 memset(&YHData[1].length,0,11);
		 memset(&YHData[2].length,0,11);
		 SaveData=0; 
		 memset(&buf,0,32);
		 SD_Readflash_Infor(buf, i);
		 z=0;
		 h=0;	
		 for(j=0;j<32;j++)
		 {
			 if((buf[j]>0x2F)&&(buf[j]<0x3A))
			 {
			    YHData[z].DATA[h++]=buf[j];
				 	if(h>10){h=9;}		
			 }
       if( ((buf[j]<0x30)||(buf[j]>0x39))&&(h>0) )		 
		   {
				  YHData[z].length=h;
			    h=0;
				  z++;
			 }
			 if(z>2)
			 {break;}
		 }
 
		 nbr=0;
		 if(YHData[2].length<10)
		 {nbr=YHData[2].length;}
			 
		 for(g=0;g<nbr;g++)
		 {
			 SaveData=SaveData*10;
			 YHData[2].DATA[g]=YHData[2].DATA[g]-0x30;
			 SaveData=SaveData+YHData[2].DATA[g];
		 }
 
  	 if(data==SaveData)//�ҵ��û���
		 {
			  MID=0;
				for(j=0;j<YHData[0].length;j++)//��·
        {
				   MID=MID+(YHData[0].DATA[j]-0x30); 
           LOP=MID;
           MID=LOP*10;				
				}
				MID=0;
		    for(j=0;j<YHData[1].length;j++)//��·
        {
				   MID=MID+(YHData[1].DATA[j]-0x30); 
           ADD=MID;
           MID=ADD*10;					
				}
			MID=LOP>>8;
			LOP=LOP&0x00FF;	
			LOP=LOP<<8;
			back=LOP+MID;
			back=back<<16;

			MID=ADD>>8;		
      ADD=ADD&0x00FF;	
			ADD=ADD<<8;		
			ADD=ADD+MID;
				
   		back=back+ADD;	
      printf("�����ԱȽ�����i=%d\n\r",i);					
		  return back;
		 }
// 		 printf("i=%d,BM=%d\n\r",i,SaveData);			
	 }
   printf("�����δ�ҵ���\n\r");		 
	 return 0;
}
 

/*�������*/
void SoftReset(void)
{
	printf("ִ�����������\n\r");		
__set_FAULTMASK(1); 
NVIC_SystemReset(); 
}

void ItemAnalyse(unsigned char *data)
{
	int BMB=0;
	int LopAdd=0;	
	if( (data[0]==2) //��������״̬��
		  &&(data[5]==0)&&(data[6]==0)&&(data[7]==0)&&(data[8]==0)//��·��ַΪ0��
		  &&(data[11]==0xB8)&&(data[13]==1)&&(data[14]==4))//�����б����
	{
//		 BMB=data[15];
//		 BMB=BMB<<8;
//		 BMB=BMB+data[16];
//		 BMB=BMB<<8;
//		 BMB=BMB+data[17];
//		 BMB=BMB<<8;
//		 BMB=BMB+data[18];
		 BMB=data[18];
		 BMB=BMB<<8;
		 BMB=BMB+data[17];
		 BMB=BMB<<8;
		 BMB=BMB+data[16];
		 BMB=BMB<<8;
		 BMB=BMB+data[15];
		 LopAdd=LopAddSearch(BMB);
		 data[5] = LopAdd>>24;
	   data[6] = LopAdd>>16;
	   data[7] = LopAdd>>8;
	   data[8] = LopAdd;				
	}
	else
	{
	}
}

