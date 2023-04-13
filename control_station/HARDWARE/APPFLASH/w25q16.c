#include "w25q16.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "includes.h"

const unsigned char URL[]="www.fubangyun.com.dtu";
const unsigned char PORT[]="16666";
const unsigned char IPv4[]={192,168,1,59};
const unsigned char SubnetMask[]={255,255,255,0};
const unsigned char GateWay[]={192,168,1,1};
const unsigned char DNS[]={114,114,114,114};
const unsigned char LocalID[6]={17,07,07,00,00,33};
//写入Flash信息存储的地址，范围为0x000000-0x20000
//1组数据（虚拟块）占0x20，前11（0-0x0a）个数据为存储信息，
//0x10-0x14为Flash存储的序列号，范围为unsigned int，存储一个自动加1
//0x1f为本数据组（虚拟块）使用信息，0x55为存储的正常数据，其他字节为错误字节；
//Flash_sn为存储信息的序列号
//Lcd_Flash_Num为LCD显示的组数（0-3000）
//Record_Addr为存储以上三个参数的地址范围为0x40000-0x42000
//1组数据占用0x20字节，
//存储结构为:低位在前，高位在后，小端模式
//0-3为Infor_Addr，4-7为Flash_sn
//8-11为Lcd_Flash_Num；12-15为存储信息正常，0x55为存储数据有效，其他为无效
static unsigned int Infor_Addr;
static unsigned int Flash_sn;
unsigned int Lcd_Flash_Num=0;
static unsigned int LCD_Manual_Num=0;		//手动报警次数统计
static unsigned int LCD_Host_Num=0;			//主机报警次数统计
static unsigned int LCD_Fault_Num=0;		//故障次数统计
static unsigned int Record_Addr;
static unsigned char Config[256];
//配置信息下一次写入的位置起始点为256的整数倍
static unsigned int Config_Addr;


//比较两个字符串对应长度是否相等
//相等为1，不等为0
static unsigned char IsStrEqual(const char *dest,const char *source,unsigned int lenth)
{
	unsigned int i=0;
	unsigned char flag=1;
	for(i=0;i<lenth;i++)
	{
		if(*dest++!=*source++)
		{
			flag=0;
			break;
		}
	}
	return flag;
}
//定时存储Flash的几个重要参数；或者断电前存储；10min或1小时执行一次或小于1小时的20次动作
//先判定是否有变化，有变化才更新
//存储位置
//unsigned int Write_Flssh_Addr;  	0x40000地址开始   前四字节存数据，第16字节填写使用情况，使用填0，未使用默认0xff，1组数据8位；
//unsigned int Flash_sn;			0x42000结束，
//unsigned int Lcd_Flash_Num;
//返回0
unsigned char Update_RecordArea(void)
{
	unsigned char i,j,flag;
	unsigned char buf[0x20];
	unsigned char tmp[0x20];
	unsigned int tmp_addr;

	memset(buf,0xff,0x20);
	buf[0]=(unsigned char)Infor_Addr;
	buf[1]=(unsigned char)(Infor_Addr>>8);
	buf[2]=(unsigned char)(Infor_Addr>>16);
	buf[3]=(unsigned char)(Infor_Addr>>24);
	
	buf[4]=(unsigned char)Flash_sn;
	buf[5]=(unsigned char)(Flash_sn>>8);
	buf[6]=(unsigned char)(Flash_sn>>16);
	buf[7]=(unsigned char)(Flash_sn>>24);
	
	buf[8]=(unsigned char)Lcd_Flash_Num;
	buf[9]=(unsigned char)(Lcd_Flash_Num>>8);
	buf[10]=(unsigned char)(Lcd_Flash_Num>>16);
	buf[11]=(unsigned char)(Lcd_Flash_Num>>24);
	
	buf[12]=0x55;
	buf[13]=0x55;
	buf[14]=0x55;
	buf[15]=0x55;
	
	buf[0x10]=LCD_Host_Num;
	buf[0x11]=(unsigned char)(LCD_Host_Num>>8);
	buf[0x12]=LCD_Manual_Num;
	buf[0x13]=(unsigned char)(LCD_Manual_Num>>8);
	buf[0x14]=LCD_Fault_Num;
	buf[0x15]=(unsigned char)(LCD_Fault_Num>>8);
	if(Record_Addr==RECORD_AREA) tmp_addr=RECORD_AREA+0x2000-0x20;
	else tmp_addr=RECORD_AREA-0x20;
	memset(tmp,0,0x20);
	SPI_FLASH_BufferRead(tmp,tmp_addr,0x20);
	flag=IsStrEqual((char *)buf,(char *)tmp,0x20);
	if(flag==1) return 2;//数据相等没有更新不需写入
	else
	{
		for(j=0;j<3;j++)
		{
			memset(tmp,0,0x20);
			SPI_FLASH_BufferRead(tmp,Record_Addr,0x20);
			for(i=0;i<0x20;i++)
			{
				if(tmp[i]!=0xff)break;
			}
			if(i==0x20)
			{
				SPI_FLASH_BufferWrite(buf,Record_Addr,0x20);
				memset(tmp,0,0x20);
				SPI_FLASH_BufferRead(tmp,Record_Addr,0x20);
				flag=IsStrEqual((char *)buf,(char *)tmp,0x20);
				if(flag==1)//写入的数据正常
				{
					Record_Addr+=0x20;
					if(Record_Addr>=RECORD_AREA+0x2000) Record_Addr=RECORD_AREA;
					if(Record_Addr%0x1000==0)SPI_FLASH_SectorErase(Record_Addr);
					return 1;
				}
			}
			//虚拟块有问题或写入错误
			Record_Addr+=0x20;
			if(Record_Addr>=RECORD_AREA+0x2000) Record_Addr=RECORD_AREA;
			if(Record_Addr%0x1000==0)SPI_FLASH_SectorErase(Record_Addr);
		}
		return 0;
	}
}
void Read_Record(unsigned char *buf)
{
	memset(buf,0xff,0x20);
	buf[4]=(unsigned char)Flash_sn;
	buf[5]=(unsigned char)(Flash_sn>>8);
	buf[6]=(unsigned char)(Flash_sn>>16);
	buf[7]=(unsigned char)(Flash_sn>>24);
	
	buf[8]=(unsigned char)Lcd_Flash_Num;
	buf[9]=(unsigned char)(Lcd_Flash_Num>>8);
	buf[10]=(unsigned char)(Lcd_Flash_Num>>16);
	buf[11]=(unsigned char)(Lcd_Flash_Num>>24);
	
	buf[12]=0x55;
	buf[13]=0x55;
	buf[14]=0x55;
	buf[15]=0x55;
	
	buf[0x10]=LCD_Host_Num;
	buf[0x11]=(unsigned char)(LCD_Host_Num>>8);
	buf[0x12]=LCD_Manual_Num;
	buf[0x13]=(unsigned char)(LCD_Manual_Num>>8);
	buf[0x14]=LCD_Fault_Num;
	buf[0x15]=(unsigned char)(LCD_Fault_Num>>8);
}
//上电复位后查找主要参数,读取记录信息
//判定标准为前一个指定字符为0x55；后一个指定字符为0xff；
//判定上电复位后执行此操作；
//每0x20为1块虚拟块，0x0c位置存放的是虚拟块使用情况，使用了为0x55，没有使用为0xff
//共256个虚拟块，占用8KB，范围0x40000-0x42000
//出口参数返回0，认定系统初始化从零开始记录
//返回其他值为Record_Addr的值
unsigned int Get_RecordArea(void)
{
	unsigned int i;
	unsigned char tmp[256];
	unsigned int j=0;
	memset(tmp,0,256);
	for(i=0x0c;i<0x2000;i+=0x20)
	{
		SPI_FLASH_BufferRead(&tmp[j++],RECORD_AREA+i,1);
	}
	for(i=0;i<256;i++)
	{
		if((tmp[i]==0x55)&&(tmp[(i+1)%0x100]==0xff))break;
	}
	if(i<256)
	{
		memset(tmp,0,256);
		SPI_FLASH_BufferRead(tmp,RECORD_AREA+i*0x20,0x20);
		Infor_Addr=tmp[0]+(tmp[1]<<8)+(tmp[2]<<16)+(tmp[3]<<24);
		Flash_sn=tmp[4]+(tmp[5]<<8)+(tmp[6]<<16)+(tmp[7]<<24);
		Lcd_Flash_Num=tmp[8]+(tmp[9]<<8)+(tmp[10]<<16)+(tmp[11]<<24);
		LCD_Host_Num=tmp[0x10]+(tmp[0x11]<<8);
		LCD_Manual_Num=tmp[0x12]+(tmp[0x13]<<8);
		LCD_Fault_Num=tmp[0x14]+(tmp[0x15]<<8);
		Record_Addr=RECORD_AREA+(i+1)*0x20;
		return Record_Addr;
	}
	//查找最后一个0x55，读取SN号与其他对比；判定正确的地址；
	//暂不处理，后续完善；
	//else {}
	Infor_Addr=0x00;
	Flash_sn=0x00;
	Lcd_Flash_Num=0x0;
	LCD_Host_Num=0;
	LCD_Manual_Num=0;
	LCD_Fault_Num=0;
	Record_Addr=RECORD_AREA;
	SPI_FLASH_SectorErase(Record_Addr);
	SPI_FLASH_SectorErase(Infor_Addr);
	Update_RecordArea();
	return 0;
}
//void Write_LCD_Num(unsigned int Host_Num,unsigned int Manual_Num,unsigned int Fault_Num)
//{
//	
//}
void Infor_Record_Num(unsigned char *data,unsigned int len)
{
	if(Lcd_Flash_Num<3000) Lcd_Flash_Num++;
	if(((data[0]==1)&&(data[4]&0x02))||((data[0]==2)&&(data[9]&0x02)))//上报火警
	{
		if(LCD_Host_Num<150)LCD_Host_Num++;
		else LCD_Host_Num=150;
	}
	if(((data[0]==1)&&((data[4]&0x04)||(data[5]&0x07)))||((data[0]==2)&&((data[9]&0x04)||(data[10]&0x01))))//上报部件故障
	{
		if(LCD_Fault_Num<150)LCD_Fault_Num++;
		else LCD_Fault_Num=150;
	}
	if((data[0]==24)&&(data[2]&0x04))
	{
		if(LCD_Manual_Num<150)LCD_Manual_Num++;
		else LCD_Manual_Num=150;
	}
//	Write_LCD_Num(LCD_Host_Num,LCD_Manual_Num,LCD_Fault_Num);
	Update_RecordArea();
}
//对屏显的所有信息清零复位，复位键按下时调用
unsigned int Clear_LCD_Num(void)
{
	LCD_Host_Num=0;
	LCD_Manual_Num=0;
	LCD_Fault_Num=0;
	Lcd_Flash_Num=1;
	Update_RecordArea();
//	Write_LCD_Num(LCD_Host_Num,LCD_Manual_Num,LCD_Fault_Num);
	return Lcd_Flash_Num;
}
//配置信息存储位置CONFIG_AREA	0x50000---0x51FFF共2个扇区，
//存储信息占用256B，可更新32次；
//20min调用1次，判定是否有配置更新，若有，则执行刷新写入下一配置块
unsigned char Updata_ConfigArea(void)
{
	unsigned char flag;
	unsigned char buf[256];
	if(Config_Addr==CONFIG_AREA)
		SPI_FLASH_BufferRead(buf,0x1F00+CONFIG_AREA,256);
	else 
		SPI_FLASH_BufferRead(buf,Config_Addr,256);
	flag=IsStrEqual((char *)buf,(char *)Config,256);
	if(flag==0)
	{
		Config[0]++;
		//因为擦除之后为0xff，避免出现0xff影响判定
		if(Config[0]==0xff) Config[0]=0x00;
		SPI_FLASH_BufferWrite(Config,Config_Addr,256);
		Config_Addr+=256;
		if(Config_Addr==CONFIG_AREA+0x2000)Config_Addr=CONFIG_AREA;
		if(Config_Addr%0x1000==0)SPI_FLASH_SectorErase(Config_Addr);
	}
	return 1;
}
unsigned int Get_ConfigArea(void)
{
	unsigned char data[0x20];
	unsigned int i=0;
	for(i=0;i<0x20;i++)
	{
		SPI_FLASH_BufferRead(&data[i],CONFIG_AREA+256*i,1);
	}
	for(i=0;i<0x20;i++)
	{
		if((data[i]!=0xff)&&(data[(i+1)%0x20])==0xff)
		{
			Config_Addr=i*256+CONFIG_AREA;
			break;
		}
	}
	//不符合条件，判定为新片子，进行初始化；
	if(i>=0x20)
	{
		//写入初始化配置，并把地址设为0
		for(i=0;i<256;i++)Config[i]=0;
		//配置更新标识，每更新一次加1，判断本地址最大且下一页本地址为FF判定为当前使用的配置
		Config[0]=1;
		//服务器域名
		memcpy(&Config[0x10],URL,sizeof(URL));
		//端口号设置6666，小端存储
		Config[0x2e]=6666%256;
		Config[0x2f]=6666/256;
		//服务器接口方式，为1-GM3模式，为0-K2模式
		Config[0x30]=1;
		//K2模式，本机地址配置选择，为1-DHCP；为0-静态IP；
		Config[0x31]=1;
		//IP、子网掩码、DNS、网关
		memcpy(&Config[0x40],IPv4,4);
		memcpy(&Config[0x44],SubnetMask,4);
		memcpy(&Config[0x48],GateWay,4);
		memcpy(&Config[0x4c],DNS,4);
		//mac
//		memcpy(&Config[0x50],MAC,4);		
		//SN--GM3_SN[]默认为0；
		//IMEI--GM3_IMEI[]
		//PHONE
		//主机接口方式，0-CAN，1-485,2-232
		Config[0x80]=1;
		//CAN_bound：单位（kbps）  2Byte范围5-500
		Config[0x90]=2;
		//485_bound：单位bps 4Byte，校验位 1Byte 停止位 1Byte
		Config[0xA0]=10;
		//校验位0-无校验，2-偶校验，1-奇校验
		Config[0xA4]=0;
		//停止位0-1位，1-1.5位，2-2位
		Config[0xA5]=0;
		//LCD设置，背光亮度1Byte(0-9)，亮屏时间1Byte(0-1min，1-1min，2-2min，3-3min)
		//手报/火警声音关闭2Byte(0-开启，1-关闭)，密码4Byte（默认为0119）
		Config[0xB0]=9;
		Config[0xB1]=1;
		Config[0xB2]=1;
		Config[0xB3]=1;
		Config[0xB4]='0';
		Config[0xB5]='1';
		Config[0xB6]='1';
		Config[0xB7]='9';
		//版本号，低8位硬件版本，高8位软件版本,前两个字节有效，高位为次版本号，低位为主版本号，
		//主版本号统一取1，次版本号自行定义；
		Config[0xC0]=1;
		Config[0xC1]=0;
		Config[0xC8]=1;
		Config[0xC9]=0;
		//本机地址，暂时没用到，先行预留6Byte
		Config[0xF0]=LocalID[0];
		Config[0xF1]=LocalID[1];
		Config[0xF2]=LocalID[2];
		Config[0xF3]=LocalID[3];
		Config[0xF4]=LocalID[4];
		Config[0xF5]=LocalID[5];
		Config_Addr=CONFIG_AREA;
		SPI_FLASH_SectorErase(Config_Addr);
		SPI_FLASH_BufferWrite(Config,Config_Addr,256);
		Config_Addr+=256;
		return Config_Addr;
	}
	else
	{
		SPI_FLASH_BufferRead(Config,Config_Addr,256);
		Config_Addr+=256;
		if(Config_Addr>=CONFIG_AREA+0x2000)Config_Addr=CONFIG_AREA;
		if(Config_Addr%0x1000==0)SPI_FLASH_SectorErase(Config_Addr);
		return Config_Addr;
	}
}
//读取Config中的一些信息；
//入口参数：cmd要读取的信息命令
//出口参数：data信息内容，长度返回，若返回0判定读取失败；
unsigned char Readflash_Config(unsigned char *data,unsigned char cmd)
{
	unsigned char len=0;
	switch(cmd)
	{
		case	SERVER_CONFIG_CMD:
			len=0x1E;
			memcpy(data,&Config[cmd],len);break;
		
		case	TIMING_CMD:
			len=0x1;
			memcpy(data,&Config[cmd],len);break;
			
		case	UPDATE_CONFIG_CMD:
		case	SERVER_INTERFACE_CONFIG_CMD:
		case	K2_IP_CONFIG_CMD:
		case	GPRS_SN_CONFIG_CMD:
		case	GPRS_IMEI_CONFIG_CMD:
		case	GPRS_PHONE_CONFIG_CMD:
		case	HOST_CONFIG_CMD:
		case	CAN_CONFIG_CMD:
		case	RS485_CONFIG_CMD:
		case	LCD_CONFIG_CMD:
		case	LCD_VOICE_CMD:
		case	LCD_MIMA_CMD:
		case	VER_CONFIG_CMD:
		case	ID_CONFIG_CMD:
			len=0x10;
			memcpy(data,&Config[cmd],len);break;
		case	ITEM_CONFIG_CMD://用户编码用；
			len=0x04;
			memcpy(data,&Config[cmd],len);break;
		case	ProID_CONFIG_CMD://用户编码用；
			len=0x04;
			memcpy(data,&Config[cmd],len);break;

		case	SERVER_PORT_CMD:
			len=0x02;
			memcpy(data,&Config[cmd],len);break;
		
		default		:
			break;
	}
	return len;
}
//更改Config中的一些信息；
//入口参数：cmd要读取的信息命令，data更改的信息内容
//出口参数：若返回0判定读取失败；
unsigned char Writeflash_Config(unsigned char *data,unsigned char cmd)
{
	unsigned char len=0;
	unsigned char buf[0x20];
	unsigned char flag=0;
	//应先读取判定，再写入
	len=Readflash_Config(buf,cmd);
	if(cmd==LCD_MIMA_CMD)len=4;
	else if(cmd==LCD_CONFIG_CMD) len=2;
	else if(cmd==LCD_VOICE_CMD) len=2;
	else if(cmd==SERVER_PORT_CMD) len=2;
	
	flag=IsStrEqual((char *)buf,(char *)data,len);//相等 则退出；
	if(flag==1) return 1;
	
	switch(cmd)
	{
		case	UPDATE_CONFIG_CMD:
			Config[0]++;
			break;
		case	SERVER_CONFIG_CMD:
			len=0x1E;
			memcpy(&Config[SERVER_CONFIG_CMD],data,len);
			break;
		case	SERVER_INTERFACE_CONFIG_CMD:
			len=0x10;
			memcpy(&Config[SERVER_INTERFACE_CONFIG_CMD],data,len);break;
		case	K2_IP_CONFIG_CMD:
			len=0x10;
			memcpy(&Config[K2_IP_CONFIG_CMD],data,len);
			break;
		case	GPRS_SN_CONFIG_CMD:
			len=0x06;
			memcpy(&Config[GPRS_SN_CONFIG_CMD],data,len);
			break;
		case	GPRS_IMEI_CONFIG_CMD:
			len=0x10;
			memcpy(&Config[GPRS_IMEI_CONFIG_CMD],data,len);
			break;
		case	GPRS_PHONE_CONFIG_CMD:
			len=0x10;
			memcpy(&Config[GPRS_PHONE_CONFIG_CMD],data,len);
			break;
		case	HOST_CONFIG_CMD:
			len=0x10;
			memcpy(&Config[HOST_CONFIG_CMD],data,len);
			break;
		case	CAN_CONFIG_CMD:
			len=0x10;
			memcpy(&Config[CAN_CONFIG_CMD],data,len);
			break;
		case	RS485_CONFIG_CMD:
			len=0x10;
			memcpy(&Config[RS485_CONFIG_CMD],data,len);
			break;
		case	LCD_CONFIG_CMD:
			len=0x2;
			memcpy(&Config[LCD_CONFIG_CMD],data,len);
			break;
		case	LCD_VOICE_CMD:
			len=0x2;
			memcpy(&Config[LCD_VOICE_CMD],data,len);
			break;
		case	LCD_MIMA_CMD:
			len=0x4;
			memcpy(&Config[LCD_MIMA_CMD],data,len);
			break;
		case	VER_CONFIG_CMD:
			len=0x10;
			memcpy(&Config[VER_CONFIG_CMD],data,len);
			break;
		case	TIMING_CMD:
			len=0x01;
			memcpy(&Config[TIMING_CMD],data,len);
			break;
		case	ITEM_CONFIG_CMD://用户编码表存储；
			len=0x04;
			memcpy(&Config[ITEM_CONFIG_CMD],data,len);
			break;
		case	ProID_CONFIG_CMD://协议卡注册标志位；
			len=0x04;
			memcpy(&Config[ProID_CONFIG_CMD],data,len);
			break;		
		case	ID_CONFIG_CMD:
			len=0x10;
			memcpy(&Config[ID_CONFIG_CMD],data,len);
			break;
		case	SERVER_PORT_CMD:
			len=0x02;
			memcpy(&Config[SERVER_PORT_CMD],data,len);
			break;	
		
		default		:
			break;
	}
	Updata_ConfigArea();
	return len;
}
///////////////////////////
//将一条信息写入Flash中，若写正确，则返回1，写错误，则返回0；报警提示Flash错误；
//记录写入的地址Infor_Addr,
//写入完成自动加0x20，等待下次写入;
//0x16-0x1B写入时间标签，顺序为秒分时日月年
//0x1C-0x1F写入序列号Flash_sn，顺次加1；
//若本虚拟块错误，加20，写入下一虚拟块；重复3次若都不成功，去下一块写入；（先擦除再写入）
//写到地址0x20000，自动变为0,这样总共可以写入4096组数据；
//检测写入地址为0x1000的整数倍时，先执行一次擦除块命令，再写入；
//记录错误地址信息暂不考虑，错误信息直接跳过；
//记录总共的信息条数，int型数据累加；记录在数据尾部；
//入口：写入的字符串，长度
unsigned char Writeflash_Infor(u8* pBuffer, u16 NumByteToWrite)
{
	unsigned char buf[0x20];
//	char data[10];
	unsigned int i,j;
	unsigned char flag;
	if(NumByteToWrite>0x1C)return 0;
    //读取前50组存储数据中是否有本组数据，如果有，不再存储
    for(i=1;(i<51)&&(i<=Lcd_Flash_Num);i++)
    {
        Readflash_Infor(buf, i);
				//ASL..........20180324更改了1min上报多条同一部件的同一状态，不再对sec进行比较，只对min以上进行比较；
				if(NumByteToWrite==0x1c)
				{
//						if((IsStrEqual((char *)buf,(char *)pBuffer,0x16)==1)&&(buf[0x17]==pBuffer[0x17])&&(buf[0x18]==pBuffer[0x18])&&	\
//							(buf[0x19]==pBuffer[0x19])&&(buf[0x1a]==pBuffer[0x1a])&&(buf[0x1b]==pBuffer[0x1b])&&(buf[0]==2))						
					
					 if(buf[0]!=0x18)//如果不是用户信息报警，则执行1min屏蔽
					 {
						if((IsStrEqual((char *)buf,(char *)pBuffer,0x16)==1)&&(buf[0x17]==pBuffer[0x17])&&(buf[0x18]==pBuffer[0x18])&&	\
							(buf[0x19]==pBuffer[0x19])&&(buf[0x1a]==pBuffer[0x1a])&&(buf[0x1b]==pBuffer[0x1b]))//&&(buf[0]==2)//复位，主备电故障。好多主机是有重发复位的。				
						
							return 2;
					 }
				}
				else
				{
					if(IsStrEqual((char *)buf,(char *)pBuffer,NumByteToWrite)==1)return 2;
				}
    }
    
	for(j=0;j<3;j++)
	{
	SPI_FLASH_BufferRead(buf,Infor_Addr,0x20);
	for(i=0;i<0x20;i++)
	{
		if(buf[i]!=0xff)break;
	}
	if(i==0x20)
	{
		SPI_FLASH_BufferWrite(pBuffer,Infor_Addr,NumByteToWrite);
		memset(buf,0,0x20);
		SPI_FLASH_BufferRead(buf,Infor_Addr,NumByteToWrite);
		flag=IsStrEqual((char *)buf,(char *)pBuffer,NumByteToWrite);
		if(flag==1)
		{
			//写入正确，则写入序列号,后面需要精确处理存储位置时再添加；
			//在读信息时通过检查此号码，判定是否正确；防止坏块导致读出错误值；
//			RTC_Get((char*)&data);
//			buf[0x16]=data[5];
//			buf[0x17]=data[4];
//			buf[0x18]=data[3];
//			buf[0x19]=data[2];
//			buf[0x1A]=data[1];
//			buf[0x1B]=data[0];
			buf[0x1C]=(unsigned char)Flash_sn;
			buf[0x1D]=(unsigned char)(Flash_sn>>8);
			buf[0x1E]=(unsigned char)(Flash_sn>>16);
			buf[0x1F]=(unsigned char)(Flash_sn>>24);
			SPI_FLASH_BufferWrite(&buf[0x1C],Infor_Addr+0x1C,4);
			Flash_sn++;
			Infor_Addr+=0x20;
			if(Infor_Addr>=0x20000)Infor_Addr=0;
			if(Infor_Addr%0x1000==0)SPI_FLASH_SectorErase(Infor_Addr);
			Infor_Record_Num(buf,10);
			return 1;
		}
	}
	//虚拟块有问题或写入错误
	Infor_Addr+=0x20;
	if(Infor_Addr>=0x20000)Infor_Addr=0;
	if(Infor_Addr%0x1000==0)SPI_FLASH_SectorErase(Infor_Addr);
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
unsigned char Readflash_Infor(u8* pBuffer, unsigned int sn)
{
	unsigned char buf[0x20];
	unsigned int tmp_sn;
	unsigned char i;
	unsigned char j=0;
	if((sn<=Lcd_Flash_Num)&&(sn!=0))
	{
		for(i=0;i<3;i++)
		{
			memset(buf,0,0x20);
			if((sn+j)*0x20<=Infor_Addr)
			{
				SPI_FLASH_BufferRead(buf,Infor_Addr-(sn+j)*0x20,0x20);
			}
			else
			{
				SPI_FLASH_BufferRead(buf,Infor_Addr+0x20000-(sn+j)*0x20,0x20);
			}
			tmp_sn=buf[0x1C]+((unsigned int)buf[0x1D]<<8)+((unsigned int)buf[0x1E]<<16)+((unsigned int)buf[0x1F]<<24);
			if(Flash_sn==sn+tmp_sn)
			{
				memcpy(pBuffer,buf,0x20);
				return 1;
			}
			else
			{
				if((tmp_sn-(Flash_sn-(sn+j)))<256) j=sn+tmp_sn-Flash_sn;
				else j++;
			}
		}
	}
	//超出LCD显示范围，无数据
	return 0;
}
//void Flash_Printf(void)
//{
//	unsigned char buf[0x30];
//	memset(buf,0,0x30);
//	Readflash_Config(buf,UPDATE_CONFIG_CMD);
//	printf("更新信息：%d\r\n",buf[0]);
//	memset(buf,0,0x30);
//	Readflash_Config(buf,SERVER_CONFIG_CMD);
//	printf("服务器信息-域名：%s\t\t端口号：%d\t",buf,buf[0x1e]+buf[0x1f]*256);
//	memset(buf,0,0x30);
//	Readflash_Config(buf,SERVER_INTERFACE_CONFIG_CMD);
//	printf("服务器接口方式：%d\t\t联网设置：%d\t",buf[0x0],buf[0x01]);
//	memset(buf,0,0x30);
//	Readflash_Config(buf,K2_IP_CONFIG_CMD);
//	printf("IP:%d.%d.%d.%d\t子网掩码:%d.%d.%d.%d\t网关:%d.%d.%d.%d\tDNS:%d.%d.%d.%d\t",\
//	buf[0x00],buf[0x01],buf[0x02],buf[0x03],	\
//	buf[0x04],buf[0x05],buf[0x06],buf[0x07],	\
//	buf[0x08],buf[0x09],buf[0x0a],buf[0x0b],	\
//	buf[0x0c],buf[0x0d],buf[0x0e],buf[0x0f]		);
//	memset(buf,0,0x30);
//	Readflash_Config(buf,GPRS_SN_CONFIG_CMD);
//	printf("GPRS_SN:%s\t",buf);
//	memset(buf,0,0x30);
//	Readflash_Config(buf,GPRS_IMEI_CONFIG_CMD);
//	printf("GPRS_IMEI:%s\t",buf);
//	memset(buf,0,0x30);
//	Readflash_Config(buf,GPRS_PHONE_CONFIG_CMD);
//	printf("GPRS_PHONE:%s\t",buf);
//	memset(buf,0,0x30);
//	Readflash_Config(buf,HOST_CONFIG_CMD);
//	printf("主机接口方式：%d\t\t",buf[0x0]);
//	memset(buf,0,0x30);
//	Readflash_Config(buf,CAN_CONFIG_CMD);
//	printf("CAN_bound：%d\t\t",buf[0]);
//	memset(buf,0,0x30);
//	Readflash_Config(buf,RS485_CONFIG_CMD);
//	printf("485_bound：%d\t\t校验：%d\t\t停止位：%d\t\t",buf[0x0],buf[0x4],buf[0x5]);
//	memset(buf,0,0x30);
//	Readflash_Config(buf,LCD_CONFIG_CMD);
//	printf("背光亮度:%d\t\t",buf[0x0]);
//	printf("关屏时间：%d\t\t",buf[0x01]);
//	printf("火警声音:%d\t\t",buf[0x02]);
//	printf("手报声音：%d\t\t",buf[0x03]);
//	printf("密码:%d\t%d\t%d\t%d\t",buf[0x04],buf[0x05],buf[0x06],buf[0x07]);
//	memset(buf,0,0x30);
//	Readflash_Config(buf,VER_CONFIG_CMD);
//	printf("硬件版本:%d.%d\t\t",buf[0x0],buf[0x1]);
//	printf("软件版本:%d.%d\t\t",buf[0x8],buf[0x9]);
//	memset(buf,0,0x30);
//	Readflash_Config(buf,ID_CONFIG_CMD);
//	printf("本机地址:%d\t%d\t%d\t%d\t%d\t%d\r\n",buf[0x0],buf[0x1],buf[0x2],buf[0x3],buf[0x4],buf[0x5]);

//	printf("0x%x\t",Infor_Addr);
//	printf("0x%x\t",Flash_sn);
//	printf("0x%x\t",Lcd_Flash_Num);
//	printf("0x%x\t",LCD_Host_Num);
//	printf("0x%x\t",LCD_Manual_Num);
//	printf("0x%x\t",LCD_Fault_Num);
//	printf("0x%x",Record_Addr);
//}
//void Flash_Test(void)
//{
//	static unsigned int k=0;
//	unsigned int i,j;
//	unsigned char buf[50];
//	unsigned char bufout[50];
//	unsigned int rand_num;
//	for(i=0;i<10;i++)
//	{
//		for(j=0;j<1000;j++);
//	}
//	memset(buf,k%256,11);
//	Writeflash_Infor(buf,11);
//	if(k%256==0)
//	{
//		Update_RecordArea();
//		Flash_Printf();
//		memset(bufout,0,11);
//		rand_num=rand();
//		if(Readflash_Infor(bufout,rand_num%Lcd_Flash_Num))
//		{
//			printf("\t\t0x%x\t:\t%x\t%x\t%x",rand_num%3000,bufout[0],bufout[6],bufout[10]);//\r\n
//		}
//	}
//	k++;
//}

//初始化系统参数
extern unsigned char  Local_Addres[6];
extern unsigned char  Server_Interface_Mode;
extern unsigned char  remote_ip[4];											        /*远端IP地址*/
extern unsigned short remote_port;																/*远端端口号*/
extern unsigned char  mac[6];
extern unsigned char domain_name[30];

extern u8 local_ip[4];											/*定义W5500默认IP地址*/
extern u8 subnet[4];												/*定义W5500默认子网掩码*/
extern u8 gateway[4];												/*定义W5500默认网关*/
extern u8 dns_server[4];									  /*定义W5500默认DNS*/

extern u8 IPMode;

extern unsigned char domain_CHK(unsigned char *name);

unsigned char NDS_FLAG=0;

extern unsigned char BT_SucessFlag;
extern unsigned char BT_SucessDATA[4];
void ConfigSystemSet(void)
{	
	memcpy(Local_Addres,&Config[ID_CONFIG_CMD],6);
	Server_Interface_Mode=Config[SERVER_INTERFACE_CONFIG_CMD];
	memcpy(domain_name,&Config[SERVER_CONFIG_CMD],30);
	memcpy(remote_ip,&Config[K2_IP_CONFIG_CMD],4);
	memcpy((unsigned char*)&remote_port,&Config[SERVER_PORT_CMD],2);
	memcpy(mac,&Config[GPRS_SN_CONFIG_CMD],6);
	
	memcpy(local_ip,&Config[GPRS_IMEI_CONFIG_CMD],4);
	memcpy(subnet,&Config[GPRS_IMEI_CONFIG_CMD+4],4);
	memcpy(gateway,&Config[GPRS_IMEI_CONFIG_CMD+8],4);
	memcpy(dns_server,&Config[GPRS_IMEI_CONFIG_CMD+12],4);
	
	IPMode=Config[GPRS_PHONE_CONFIG_CMD];
	
	NDS_FLAG=domain_CHK(domain_name);
	
	if(Config[HOST_CONFIG_CMD]==0xAB)//BT标志位读取；
	{
		memcpy(BT_SucessDATA,&Config[HOST_CONFIG_CMD],4);
	  BT_SucessFlag=4;
	}
	
}

void ConfigSystemW5500Set(void)
{

	memcpy(domain_name,&Config[SERVER_CONFIG_CMD],30);
	memcpy(remote_ip,&Config[K2_IP_CONFIG_CMD],4);
	memcpy((unsigned char*)&remote_port,&Config[SERVER_PORT_CMD],2);
	memcpy(mac,&Config[GPRS_SN_CONFIG_CMD],6);
}

OS_MUTEX W25Q16_Mutex;
void W25q16_Init(void)
{
	OS_ERR err;
	SPI_FLASH_Init();
	if(Get_RecordArea()==0)
	{
//		printf("系统存储信息初始化\r\n");
	}
	Get_ConfigArea();
	ConfigSystemSet();
	OSMutexCreate((OS_MUTEX*)&W25Q16_Mutex,
				(CPU_CHAR*	)"W25Q16_Mutex",
				(OS_ERR*	)&err);
}
void W25q16_Lock(void)
{
	OS_ERR err;
	OSMutexPend((OS_MUTEX*	)&W25Q16_Mutex,
				(OS_TICK	) 0,
				(OS_OPT		) OS_OPT_PEND_BLOCKING,
				(CPU_TS    *) 0,
				(OS_ERR*	)&err);
}
void W25q16_UnLock(void)
{
	OS_ERR err;
	OSMutexPost((OS_MUTEX*)&W25Q16_Mutex,
				(OS_OPT 	)OS_OPT_POST_NONE,
				(OS_ERR*	)&err);
}
const unsigned char ShouBao[]={0x40,0x40,0x0f,0x00,0x00,0x00,0x1d,0x09,0x13,0x13,0x09,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0a,0x00,0x02,0x18,0x01,0x04,0x02,0x1d,0x09,0x13,0x13,0x09,0x11,0x06,0x23,0x23};
unsigned char XiaoYin[]={0x40,0x40,0x10,0x00,0x00,0x00,0x1e,0x09,0x13,0x13,0x09,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
			0x0a,0x00,0x02,0x18,0x01,0x02,0x02,0x1e,0x09,0x13,0x13,0x09,0x11,0x07,0x23,0x23};
unsigned int Server_SaveInfor(unsigned char *data,unsigned int i);
void W25q16_Test(void)
{
	XiaoYin[30]++;
	Server_SaveInfor((unsigned char *)XiaoYin,0);
}


void Config_LcdCopy(unsigned char *destination)
{
	memcpy(destination,Config,256);
}

//系统参数设置
unsigned char ConfigDataSet(unsigned char TypeA,unsigned char *Indata,unsigned char datalength)
{	
	unsigned char B=0;
	
	switch(TypeA)
	{
		case 1:
		  B=Writeflash_Config(Indata,ID_CONFIG_CMD);	
		break;
		
		case 2:
			B=Writeflash_Config(Indata,SERVER_INTERFACE_CONFIG_CMD);	
		break;	
		
		case 3:
			B=Writeflash_Config(Indata,SERVER_CONFIG_CMD);				
		break;	
		
		case 4:
			B=Writeflash_Config(Indata,SERVER_PORT_CMD);				
		break;	

		case 5:
			B=Writeflash_Config(Indata,GPRS_SN_CONFIG_CMD);				
		break;	

		case 6:
			B=Writeflash_Config(Indata,GPRS_IMEI_CONFIG_CMD);				
		break;	

		case 7:
			B=Writeflash_Config(Indata,GPRS_PHONE_CONFIG_CMD);				
		break;	
		
		default:
			
		break;	
  }

  return B;	
}


//uint8 local_ip[4]={0,0,0,0};											/*定义W5500默认IP地址*/
//uint8 subnet[4]={0,0,0,0};												/*定义W5500默认子网掩码*/
//uint8 gateway[4]={0,0,0,0};												/*定义W5500默认网关*/
//uint8 dns_server[4]={0,0,0,0};									  /*定义W5500默认DNS*/
void IP_Write_Flash(void)
{
	u8 data[0x10];
	memset(data,0,0x10);
	memcpy(&data[0],local_ip,4);
	memcpy(&data[4],subnet,4);
	memcpy(&data[8],gateway,4);
	memcpy(&data[12],dns_server,4);	
	
	Writeflash_Config(data,GPRS_IMEI_CONFIG_CMD);	
}

void RemoteIP_Write_Flash(void)
{
	u8 data[0x10];
	memset(data,0,0x10);
	memcpy(&data[0],remote_ip,4);	
	Writeflash_Config(data,K2_IP_CONFIG_CMD);		
}

extern const unsigned char SOFT_msg[40];
extern const char FB1010_sv[20];

void ConfigSystemDataPrint(void)
{
	unsigned char i=0;

	printf("1.本机地址:");
	for(i=0;i<6;i++)
	{
	printf("  %x",Local_Addres[i]);
	}
	printf("\r\n");

 	printf("2.本机网络模式:  %d ",Server_Interface_Mode);
	if(Server_Interface_Mode==2)
	{
		printf("无线模式。\n\r");
	}
	else
	{
		printf("有线模式。\n\r");		
	}
	///	
	printf("3.有线MAC地址:");
	for(i=0;i<6;i++)
	{
	printf("%x  ",mac[i]);
	}
	printf("\r\n");
	///	
	printf("4.有线服务器域名:%s\r\n",domain_name);
	
	printf("5.有线服务器地址:");
	for(i=0;i<4;i++)
	{
	printf("%d.",remote_ip[i]);
	}
	printf("\r\n");	
	///	
	printf("6.有线服务器端口:%d\r\n",remote_port);
		
	printf("7.有线IP地址    :%d.%d.%d.%d\r\n", local_ip[0],local_ip[1],local_ip[2],local_ip[3]);

	printf("8.有线子网掩码  :%d.%d.%d.%d\r\n", subnet[0],subnet[1],subnet[2],subnet[3]);

	printf("9.有线默认网关  :%d.%d.%d.%d\r\n", gateway[0],gateway[1],gateway[2],gateway[3]);
	
	printf("10.有线DNS服务器:%d.%d.%d.%d\r\n", dns_server[0],dns_server[1],dns_server[2],dns_server[3]);
	
  printf("11.有线IP模式   :%d （1=手动，2=自动）\r\n", IPMode);
	
	printf("12.程序版本 ：%s\n\r",FB1010_sv);
	
	printf("13.协议版本 ：%s\n\r",SOFT_msg);
}

/*****************************************************************************
* 函数名称: void BTFlashFlagSet(void)
* 功能描述: 
* 参    数:
* 返回  值:
*****************************************************************************/
void BTFlashFlagSet(unsigned char data)
{
	  unsigned char Indata[0x10];
	  memset(Indata,data,0x10);
		Writeflash_Config(Indata,HOST_CONFIG_CMD);//并没有判断写入的数据是否成功写入了？？
}

/*****************************************************************************
* 函数名称: void BTFlashFlagSet(void)
* 功能描述: 
* 参    数:
* 返回  值:
*****************************************************************************/
void BTFlashFlagRead(unsigned char * Indata)
{
	memcpy(Indata,&Config[HOST_CONFIG_CMD],6);
}