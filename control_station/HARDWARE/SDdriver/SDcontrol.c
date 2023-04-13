//更新记录
//100~200的文件编号可以导入到Flash中。


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
#define readDataCot 3172 //3k文件读取；
int FileNBR=1;

FATFS fs;         /* Work area (file system object) for logical drive */
FIL fsrc, fdst;      /* file objects */
FRESULT res;
UINT br,bw;
char path0[50]="0:";
//char buffer[100];   /* file copy buffer */
//uint8_t textFileBuffer[] = "中英文测试字符串 \r\nChinese and English test strings \r\n";

uint8_t textFileBuffer[2050] = {0};
//uint8_t textFileBuffer[1] = {0};
uint8_t FBWB[50];
uint8_t FBWB1[] = "0:/FBWB_0.txt";
uint8_t FBWB2[] = "0:/FBWB_00.txt";
uint8_t FBWB3[] = "0:/FBWB_000.txt";

float FBWBReadStatus=0; //界面获取进度；
int FBWBReadNbr=0; //界面获取当前读取的条数；
int FBWBReadFail=0; //界面获取当前读取的条数；

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
  * @函数名称	Delay
  * @函数说明   实现延时功能 
  * @输入参数   nCount: 延时时间参数，数值越大延时时间越长 
  * @输出参数   无
  * @返回参数   无
  * @注意事项	无
  *****************************************************************************/
void  Delay (uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}
/*******************************************************************************
  * @函数名称	文件名字对比；
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
	
	if(i==5)//完全匹配导入名称；
	{return 1;}
}

/*******************************************************************************
  * @函数名称	scan_files
  * @函数说明   搜索文件目录下所有文件 
  * @输入参数   path: 根目录 
  * @输出参数   无
  * @返回参数   FRESULT
  * @注意事项	无
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
                sprintf(&path[i], "/%s", fn);//20180628 禁止屏蔽，否则出错
 				        printf("scan file - %s\n\r",path);//20180628 禁止屏蔽，否则出错
                res = scan_files(path);
                if (res != FR_OK) break;
                path[i] = 0;
            }else{                                       /* It is a file. */
//                printf("scan file - %s/%s\n\r", path, fn);//获取文件数量。
 							  FBWBflag=FileNameCpr(fn);//与0：/FBWB_匹配；
							  FileNBR++;//文件数量。
            }
        }
    }else{
//		printf("scan files error : %d\n\r",res);
	}

    return res;
}
/*******************************************************************************
  * @函数名称	SD_TotalSize
  * @函数说明   文件空间占用情况 
  * @输入参数   无 
  * @输出参数   无
  * @返回参数   1: 成功 
  				0: 失败
  * @注意事项	无
  *****************************************************************************/
int SD_TotalSize(char *path)
{
    FATFS *fs;
    DWORD fre_clust;        

    res = f_getfree(path, &fre_clust, &fs);  /* 必须是根目录，选择磁盘0 */
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


////////////SD卡用///////////////
//将一条信息写入Flash中，若写正确，则返回1，写错误，则返回0；报警提示Flash错误；
//记录写入的地址Write_Flash_Addr,
//写入完成自动加20((0x20))，等待下次写入;
//((0x16-0x1B写入时间标签，顺序为秒分时日月年 ))
//0x1C-0x1F写入序列号Flash_sn，顺次加1；
//若本虚拟块错误，加20，写入下一虚拟块；重复3次若都不成功，去下一块写入；（先擦除再写入）
//写到地址0x20000，自动变为0,这样总共可以写入4096组数据；
//检测写入地址为0x1000的整数倍时，先执行一次擦除块命令，再写入；
//记录错误地址信息暂不考虑，错误信息直接跳过；
//记录总共的信息条数，int型数据累加；记录在数据尾部；
//入口：写入的字符串，长度

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
			printf("条编号=%d,",SD_Flash_sn); 
			FBWBReadNbr=SD_Flash_sn;
			
			SPI_FLASH_BufferWrite(pBuffer,SD_Write_Flash_Addr,NumByteToWrite);
			memset(buf,0,0x20);
			SPI_FLASH_BufferRead(buf,SD_Write_Flash_Addr,NumByteToWrite);//回读;
			for(i=0;i<NumByteToWrite;i++)
			{
				if(buf[i]!=*(pBuffer+i))break;
			}
			if(i==NumByteToWrite)//		写入数据成功；
			{
			//写入正确，则写入序列号,后面需要精确处理存储位置时再添加；
			//在读信息时通过检查此号码，判定是否正确；防止坏块导致读出错误值；
				buf[0x1C]=(unsigned char)SD_Flash_sn;
				buf[0x1D]=(unsigned char)(SD_Flash_sn>>8);
				buf[0x1E]=(unsigned char)(SD_Flash_sn>>16);
				buf[0x1F]=(unsigned char)(SD_Flash_sn>>24);
				SPI_FLASH_BufferWrite(&buf[0x1C],SD_Write_Flash_Addr+0x1C,4);//写序号；
				
				SD_Flash_sn++;//下次写入；
				SD_Write_Flash_Addr+=0x20;//下次写入；
				if(SD_Write_Flash_Addr>=0x1A0000)
					SD_Write_Flash_Addr=0x19FFE0;//最后一个宽区域；
				if(SD_Write_Flash_Addr%0x1000==0)
					SPI_FLASH_SectorErase(SD_Write_Flash_Addr);//下一帧块擦出；
//				Infor_Record_Num(buf,10);
//				Infor_Parameter_Save();
				return 1;
			}
		}
	//虚拟块有问题或写入错误		
		SD_Write_Flash_Addr+=0x20;
	if(SD_Write_Flash_Addr>=0x1A0000)
	 SD_Write_Flash_Addr=0x19FFE0;
	if(SD_Write_Flash_Addr%0x1000==0)//擦出下一块；
	 SPI_FLASH_SectorErase(SD_Write_Flash_Addr);
	}
	return 0;
}

//读取第sn条信息存放到pBuffer中
//第一条为Write_Fassh_Addr-0x20
//第n条为Write_Fassh_Addr-0x20*n
//第n条的序列号为Flash_sn-sn；读出后查看序列号是否正确，
//若正确，返回1，不正确，判断Flash_sn-sn与tmp_sn的差；
//若差小于256则补齐标号差，再次查询，此标号差产生的原因为
//坏块写不成功，故序号+1再次查找；
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
	//配置SPI1接口
	MSD0_SPI_Configuration();
	//挂载文件系统
 	res = f_mount(0,&fs); 
 	if(res != FR_OK){
		  
  		printf("文件挂起失败: %d\n\r",res);
 	}
	else
	{   
 		  printf("文件挂起成功 : %d\n\r",res);
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
//历史报警信息导出；		
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
		IWDG_Feed();//喂狗
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
	  YHXX[15]=NBR/10;//创建新文件包；
	  m=YHXX[15]*10;
	  YHXX[15]+=0x30;
	  YHXX[16]=NBR-m+0x30;//创建新文件包；
//	  printf("n: %d\n\r",NBR);
	  printf("FILE: %s\n\r",YHXX);
			
	//挂载文件系统
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
//每2K数组存储；
u16 DataFlashSave(uint8_t *path,u16 nbr) 
{	
	u16 i,b=0;
  u8 Rlt=0;
	
  for(i=0;i<nbr;i++)	
	{ 
		if(*(path+i)!=0x0A)//不是包未；
		{ 
	    FLASHsave[FLASHsaveNBR++]=*(path+i);
		}
		else if(*(path+i)==0x0A )//是包未；
    {
		  Rlt=SD_Writeflash_Infor(FLASHsave, FLASHsaveNBR);//存储1条；
			if(Rlt==1){printf("写入成功\n\r");}  
			else{printf("写入失败\n\r");}
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

//文件导入；
u16 FilesysRedIn(void)
{
	unsigned int time_int_SD=0;	
	OS_ERR err;//	(ucos用)
	u8 m=0;
	int i;
	u16 NBR=0,B=0,Back=0;
	SD_Flash_sn=1;
  FBWBReadStatus=0; //界面获取进度；
  FBWBReadNbr=0; //界面获取当前读取的条数；
  FBWBReadFail=0; //界面获取当前读取的条数；
  SD_Write_Flash_Addr=0x100000;//复位
  SD_Read_Flash_Addr=0x100000;//复位
	
	//挂载文件系统；
 	res = f_mount(0,&fs);
 	if(res != FR_OK){
 		printf("挂起文件失败 : %d\n\r",res);
 	}
	else
	{
		printf("挂起文件成功 : %d\n\r",res);
	}
	//读取文件；
	scan_files(path0);
	printf("文件数量: %d\n\r",FileNBR);
	if(FileNBR==0)
	{
		return 1;
	}
	//界面显示及看门狗；
	for(i=1;i<FileNBR+1;i++)
	{ 
		IWDG_Feed();//每读取个文件，复位看门狗；
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时2ms
		time_int_SD=time_int_SD+1;
		if(time_int_SD>=100)//1秒一次
		{
			time_int_SD=0;
//			display_SD_schedule();//显示SD卡导入用户编码表进度
		}
	//确定文件名；		
		memset(&FBWB,0,50);
		if((i>0)&&(i<10))//读取FBWB_1~FBWB_9的文件；
		{
			FBWB1[8]=0x30;
			FBWB1[8]+=i;

			FBWBReadStatus=(float)i/FileNBR*100;
			printf("文件号: %d,进度=%f%%，",i,FBWBReadStatus);
			printf("文件路径: %s\n\r",FBWB1);
			memcpy(&FBWB,&FBWB1,sizeof(FBWB1));
		}
		else if((i>9)&&(i<100))//读取FBWB_10~FBWB_99的文件；
		{
			NBR=i;
			m=0;		
			FBWB2[8]=NBR/10;//创建新文件包；
			m=FBWB2[8]*10;
			FBWB2[8]+=0x30;
			FBWB2[9]=NBR-m+0x30;//创建新文件包；
			
			FBWBReadStatus=(float)i/FileNBR*100;
			printf("文件号: %d,进度=%f%%，",i,FBWBReadStatus);
			printf("文件路径: %s\n\r",FBWB2);
			
			memcpy(&FBWB,&FBWB2,sizeof(FBWB1));
		}
		else if((i>99)&&(i<200))//读取FBWB_100~FBWB_200的文件；
		{
			NBR=i;
			m=0;		
			FBWB3[8]=NBR/100;//创建新文件包；
			FBWB3[9]=(NBR-(FBWB3[8]*100))/10;
			FBWB3[10]=NBR-(FBWB3[8]*100)-(FBWB3[9]*10);

			FBWB3[8]+=0x30;
			FBWB3[9]+=0x30;
			FBWB3[10]+=0x30;		
			
			FBWBReadStatus=(float)i/FileNBR*100;
			printf("文件号: %d,进度=%f%%，",i,FBWBReadStatus);
			printf("文件路径: %s\n\r",FBWB3);
			
			memcpy(&FBWB,&FBWB3,sizeof(FBWB3));
		}
		else
		{
			return 2;
		}
		//读文件内容读取；
		res = f_open(&fsrc, (const TCHAR *)FBWB, FA_OPEN_EXISTING | FA_READ);
					if(res != FR_OK)
					{ printf("open file error : %d\n\r",res); }
					else
					{ 
						res = f_read(&fsrc, textFileBuffer, readDataCot, &br);     /* Read a chunk of src file */ //读2k+200；
						if(res==FR_OK)
							{
 								printf("读取文件字节数及内容 : %d字节\n\r",br);
								printf("%s\n\r",textFileBuffer);
							}
						else
							{
			          printf("read file error : %d\n\r",res);
		          }
						/*close file */
						f_close(&fsrc);
					}	
	  //文件数据存储到flash中
		B=DataFlashSave(textFileBuffer,br);	
		Back+=B;			
   	printf("导入条数=%d,失败条数=%d\n\r",Back,FBWBReadFail);   					
	}		
	  Writeflash_Config((unsigned char *)&Back,ITEM_CONFIG_CMD);//存储编码表条数到FLASH中；UCOS系统用；
//  	Write_Config((unsigned char *)&Back,ITEM_CONFIG_CMD);//存储编码表条数到FLASH中；非UCOS系统用；	
	FileNBR=0;
  return Back;
}
//编码表导入；
u8 SDDataInFlagt=0;
u32 ItemNbr=0;
void SDdata_Init(void)
{
	unsigned char nbr[4];
	memset(nbr,0,4);
	ItemNbr=0;
	if(ItemNbr==0)
  {
		Readflash_Config((unsigned char *)&ItemNbr,ITEM_CONFIG_CMD);//UCOS系统用；
//		Read_Config((unsigned char *)nbr,ITEM_CONFIG_CMD);//非UCOS系统用；	
//		ItemNbr=nbr[3];
//		ItemNbr=ItemNbr<<8;
//		ItemNbr=ItemNbr+nbr[2];
//		ItemNbr=ItemNbr<<8;
//		ItemNbr=ItemNbr+nbr[1];
//		ItemNbr=ItemNbr<<8;
//		ItemNbr=ItemNbr+nbr[0];
	}
	if(ItemNbr>19000)//实际可以达到19000条。
	{
		ItemNbr=19000;
	}
	else
	{}
}
void SD_DATAIN(void)
{
	printf("编码表功能未开启！\n\r");//数据空间3k取消中
//	FBWBReadStatus=0;
//	printf("编码表读取开始！\n\r");
//	if((filesysint()==0)&&(FBWBflag==1)) //。
//	{			
//		FilesysRedIn();		
//	}
//	printf("编码表执行结束！\n\r");	
}
//解析用户编码表；
typedef struct
{
	unsigned char length;
  unsigned char DATA[10];
}YHData_Typedef;
 
//存储用户编码条数；

//读取用户编码条数；
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
	printf("编码表对比开始。。%d\n\r",data);	
	for(i=0;i<ItemNbr;i++)//最大，19000内搜索；
   {
		 if(i%0x100==0)
     {IWDG_Feed();} //每100条的读取，喂狗；
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
 
  	 if(data==SaveData)//找到用户表；
		 {
			  MID=0;
				for(j=0;j<YHData[0].length;j++)//回路
        {
				   MID=MID+(YHData[0].DATA[j]-0x30); 
           LOP=MID;
           MID=LOP*10;				
				}
				MID=0;
		    for(j=0;j<YHData[1].length;j++)//回路
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
      printf("编码表对比结束！i=%d\n\r",i);					
		  return back;
		 }
// 		 printf("i=%d,BM=%d\n\r",i,SaveData);			
	 }
   printf("编码表未找到！\n\r");		 
	 return 0;
}
 

/*软件重启*/
void SoftReset(void)
{
	printf("执行软件重启！\n\r");		
__set_FAULTMASK(1); 
NVIC_SystemReset(); 
}

void ItemAnalyse(unsigned char *data)
{
	int BMB=0;
	int LopAdd=0;	
	if( (data[0]==2) //部件运行状态；
		  &&(data[5]==0)&&(data[6]==0)&&(data[7]==0)&&(data[8]==0)//回路地址为0；
		  &&(data[11]==0xB8)&&(data[13]==1)&&(data[14]==4))//并且有编码表；
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

