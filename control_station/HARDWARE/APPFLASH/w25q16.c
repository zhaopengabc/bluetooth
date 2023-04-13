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
//д��Flash��Ϣ�洢�ĵ�ַ����ΧΪ0x000000-0x20000
//1�����ݣ�����飩ռ0x20��ǰ11��0-0x0a��������Ϊ�洢��Ϣ��
//0x10-0x14ΪFlash�洢�����кţ���ΧΪunsigned int���洢һ���Զ���1
//0x1fΪ�������飨����飩ʹ����Ϣ��0x55Ϊ�洢���������ݣ������ֽ�Ϊ�����ֽڣ�
//Flash_snΪ�洢��Ϣ�����к�
//Lcd_Flash_NumΪLCD��ʾ��������0-3000��
//Record_AddrΪ�洢�������������ĵ�ַ��ΧΪ0x40000-0x42000
//1������ռ��0x20�ֽڣ�
//�洢�ṹΪ:��λ��ǰ����λ�ں�С��ģʽ
//0-3ΪInfor_Addr��4-7ΪFlash_sn
//8-11ΪLcd_Flash_Num��12-15Ϊ�洢��Ϣ������0x55Ϊ�洢������Ч������Ϊ��Ч
static unsigned int Infor_Addr;
static unsigned int Flash_sn;
unsigned int Lcd_Flash_Num=0;
static unsigned int LCD_Manual_Num=0;		//�ֶ���������ͳ��
static unsigned int LCD_Host_Num=0;			//������������ͳ��
static unsigned int LCD_Fault_Num=0;		//���ϴ���ͳ��
static unsigned int Record_Addr;
static unsigned char Config[256];
//������Ϣ��һ��д���λ����ʼ��Ϊ256��������
static unsigned int Config_Addr;


//�Ƚ������ַ�����Ӧ�����Ƿ����
//���Ϊ1������Ϊ0
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
//��ʱ�洢Flash�ļ�����Ҫ���������߶ϵ�ǰ�洢��10min��1Сʱִ��һ�λ�С��1Сʱ��20�ζ���
//���ж��Ƿ��б仯���б仯�Ÿ���
//�洢λ��
//unsigned int Write_Flssh_Addr;  	0x40000��ַ��ʼ   ǰ���ֽڴ����ݣ���16�ֽ���дʹ�������ʹ����0��δʹ��Ĭ��0xff��1������8λ��
//unsigned int Flash_sn;			0x42000������
//unsigned int Lcd_Flash_Num;
//����0
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
	if(flag==1) return 2;//�������û�и��²���д��
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
				if(flag==1)//д�����������
				{
					Record_Addr+=0x20;
					if(Record_Addr>=RECORD_AREA+0x2000) Record_Addr=RECORD_AREA;
					if(Record_Addr%0x1000==0)SPI_FLASH_SectorErase(Record_Addr);
					return 1;
				}
			}
			//������������д�����
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
//�ϵ縴λ�������Ҫ����,��ȡ��¼��Ϣ
//�ж���׼Ϊǰһ��ָ���ַ�Ϊ0x55����һ��ָ���ַ�Ϊ0xff��
//�ж��ϵ縴λ��ִ�д˲�����
//ÿ0x20Ϊ1������飬0x0cλ�ô�ŵ��������ʹ�������ʹ����Ϊ0x55��û��ʹ��Ϊ0xff
//��256������飬ռ��8KB����Χ0x40000-0x42000
//���ڲ�������0���϶�ϵͳ��ʼ�����㿪ʼ��¼
//��������ֵΪRecord_Addr��ֵ
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
	//�������һ��0x55����ȡSN���������Աȣ��ж���ȷ�ĵ�ַ��
	//�ݲ������������ƣ�
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
	if(((data[0]==1)&&(data[4]&0x02))||((data[0]==2)&&(data[9]&0x02)))//�ϱ���
	{
		if(LCD_Host_Num<150)LCD_Host_Num++;
		else LCD_Host_Num=150;
	}
	if(((data[0]==1)&&((data[4]&0x04)||(data[5]&0x07)))||((data[0]==2)&&((data[9]&0x04)||(data[10]&0x01))))//�ϱ���������
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
//�����Ե�������Ϣ���㸴λ����λ������ʱ����
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
//������Ϣ�洢λ��CONFIG_AREA	0x50000---0x51FFF��2��������
//�洢��Ϣռ��256B���ɸ���32�Σ�
//20min����1�Σ��ж��Ƿ������ø��£����У���ִ��ˢ��д����һ���ÿ�
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
		//��Ϊ����֮��Ϊ0xff���������0xffӰ���ж�
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
	//�������������ж�Ϊ��Ƭ�ӣ����г�ʼ����
	if(i>=0x20)
	{
		//д���ʼ�����ã����ѵ�ַ��Ϊ0
		for(i=0;i<256;i++)Config[i]=0;
		//���ø��±�ʶ��ÿ����һ�μ�1���жϱ���ַ�������һҳ����ַΪFF�ж�Ϊ��ǰʹ�õ�����
		Config[0]=1;
		//����������
		memcpy(&Config[0x10],URL,sizeof(URL));
		//�˿ں�����6666��С�˴洢
		Config[0x2e]=6666%256;
		Config[0x2f]=6666/256;
		//�������ӿڷ�ʽ��Ϊ1-GM3ģʽ��Ϊ0-K2ģʽ
		Config[0x30]=1;
		//K2ģʽ��������ַ����ѡ��Ϊ1-DHCP��Ϊ0-��̬IP��
		Config[0x31]=1;
		//IP���������롢DNS������
		memcpy(&Config[0x40],IPv4,4);
		memcpy(&Config[0x44],SubnetMask,4);
		memcpy(&Config[0x48],GateWay,4);
		memcpy(&Config[0x4c],DNS,4);
		//mac
//		memcpy(&Config[0x50],MAC,4);		
		//SN--GM3_SN[]Ĭ��Ϊ0��
		//IMEI--GM3_IMEI[]
		//PHONE
		//�����ӿڷ�ʽ��0-CAN��1-485,2-232
		Config[0x80]=1;
		//CAN_bound����λ��kbps��  2Byte��Χ5-500
		Config[0x90]=2;
		//485_bound����λbps 4Byte��У��λ 1Byte ֹͣλ 1Byte
		Config[0xA0]=10;
		//У��λ0-��У�飬2-żУ�飬1-��У��
		Config[0xA4]=0;
		//ֹͣλ0-1λ��1-1.5λ��2-2λ
		Config[0xA5]=0;
		//LCD���ã���������1Byte(0-9)������ʱ��1Byte(0-1min��1-1min��2-2min��3-3min)
		//�ֱ�/�������ر�2Byte(0-������1-�ر�)������4Byte��Ĭ��Ϊ0119��
		Config[0xB0]=9;
		Config[0xB1]=1;
		Config[0xB2]=1;
		Config[0xB3]=1;
		Config[0xB4]='0';
		Config[0xB5]='1';
		Config[0xB6]='1';
		Config[0xB7]='9';
		//�汾�ţ���8λӲ���汾����8λ����汾,ǰ�����ֽ���Ч����λΪ�ΰ汾�ţ���λΪ���汾�ţ�
		//���汾��ͳһȡ1���ΰ汾�����ж��壻
		Config[0xC0]=1;
		Config[0xC1]=0;
		Config[0xC8]=1;
		Config[0xC9]=0;
		//������ַ����ʱû�õ�������Ԥ��6Byte
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
//��ȡConfig�е�һЩ��Ϣ��
//��ڲ�����cmdҪ��ȡ����Ϣ����
//���ڲ�����data��Ϣ���ݣ����ȷ��أ�������0�ж���ȡʧ�ܣ�
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
		case	ITEM_CONFIG_CMD://�û������ã�
			len=0x04;
			memcpy(data,&Config[cmd],len);break;
		case	ProID_CONFIG_CMD://�û������ã�
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
//����Config�е�һЩ��Ϣ��
//��ڲ�����cmdҪ��ȡ����Ϣ���data���ĵ���Ϣ����
//���ڲ�����������0�ж���ȡʧ�ܣ�
unsigned char Writeflash_Config(unsigned char *data,unsigned char cmd)
{
	unsigned char len=0;
	unsigned char buf[0x20];
	unsigned char flag=0;
	//Ӧ�ȶ�ȡ�ж�����д��
	len=Readflash_Config(buf,cmd);
	if(cmd==LCD_MIMA_CMD)len=4;
	else if(cmd==LCD_CONFIG_CMD) len=2;
	else if(cmd==LCD_VOICE_CMD) len=2;
	else if(cmd==SERVER_PORT_CMD) len=2;
	
	flag=IsStrEqual((char *)buf,(char *)data,len);//��� ���˳���
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
		case	ITEM_CONFIG_CMD://�û������洢��
			len=0x04;
			memcpy(&Config[ITEM_CONFIG_CMD],data,len);
			break;
		case	ProID_CONFIG_CMD://Э�鿨ע���־λ��
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
//��һ����Ϣд��Flash�У���д��ȷ���򷵻�1��д�����򷵻�0��������ʾFlash����
//��¼д��ĵ�ַInfor_Addr,
//д������Զ���0x20���ȴ��´�д��;
//0x16-0x1Bд��ʱ���ǩ��˳��Ϊ���ʱ������
//0x1C-0x1Fд�����к�Flash_sn��˳�μ�1��
//�����������󣬼�20��д����һ����飻�ظ�3���������ɹ���ȥ��һ��д�룻���Ȳ�����д�룩
//д����ַ0x20000���Զ���Ϊ0,�����ܹ�����д��4096�����ݣ�
//���д���ַΪ0x1000��������ʱ����ִ��һ�β����������д�룻
//��¼�����ַ��Ϣ�ݲ����ǣ�������Ϣֱ��������
//��¼�ܹ�����Ϣ������int�������ۼӣ���¼������β����
//��ڣ�д����ַ���������
unsigned char Writeflash_Infor(u8* pBuffer, u16 NumByteToWrite)
{
	unsigned char buf[0x20];
//	char data[10];
	unsigned int i,j;
	unsigned char flag;
	if(NumByteToWrite>0x1C)return 0;
    //��ȡǰ50��洢�������Ƿ��б������ݣ�����У����ٴ洢
    for(i=1;(i<51)&&(i<=Lcd_Flash_Num);i++)
    {
        Readflash_Infor(buf, i);
				//ASL..........20180324������1min�ϱ�����ͬһ������ͬһ״̬�����ٶ�sec���бȽϣ�ֻ��min���Ͻ��бȽϣ�
				if(NumByteToWrite==0x1c)
				{
//						if((IsStrEqual((char *)buf,(char *)pBuffer,0x16)==1)&&(buf[0x17]==pBuffer[0x17])&&(buf[0x18]==pBuffer[0x18])&&	\
//							(buf[0x19]==pBuffer[0x19])&&(buf[0x1a]==pBuffer[0x1a])&&(buf[0x1b]==pBuffer[0x1b])&&(buf[0]==2))						
					
					 if(buf[0]!=0x18)//��������û���Ϣ��������ִ��1min����
					 {
						if((IsStrEqual((char *)buf,(char *)pBuffer,0x16)==1)&&(buf[0x17]==pBuffer[0x17])&&(buf[0x18]==pBuffer[0x18])&&	\
							(buf[0x19]==pBuffer[0x19])&&(buf[0x1a]==pBuffer[0x1a])&&(buf[0x1b]==pBuffer[0x1b]))//&&(buf[0]==2)//��λ����������ϡ��ö����������ط���λ�ġ�				
						
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
			//д����ȷ����д�����к�,������Ҫ��ȷ����洢λ��ʱ����ӣ�
			//�ڶ���Ϣʱͨ�����˺��룬�ж��Ƿ���ȷ����ֹ���鵼�¶�������ֵ��
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
	//������������д�����
	Infor_Addr+=0x20;
	if(Infor_Addr>=0x20000)Infor_Addr=0;
	if(Infor_Addr%0x1000==0)SPI_FLASH_SectorErase(Infor_Addr);
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
	//����LCD��ʾ��Χ��������
	return 0;
}
//void Flash_Printf(void)
//{
//	unsigned char buf[0x30];
//	memset(buf,0,0x30);
//	Readflash_Config(buf,UPDATE_CONFIG_CMD);
//	printf("������Ϣ��%d\r\n",buf[0]);
//	memset(buf,0,0x30);
//	Readflash_Config(buf,SERVER_CONFIG_CMD);
//	printf("��������Ϣ-������%s\t\t�˿ںţ�%d\t",buf,buf[0x1e]+buf[0x1f]*256);
//	memset(buf,0,0x30);
//	Readflash_Config(buf,SERVER_INTERFACE_CONFIG_CMD);
//	printf("�������ӿڷ�ʽ��%d\t\t�������ã�%d\t",buf[0x0],buf[0x01]);
//	memset(buf,0,0x30);
//	Readflash_Config(buf,K2_IP_CONFIG_CMD);
//	printf("IP:%d.%d.%d.%d\t��������:%d.%d.%d.%d\t����:%d.%d.%d.%d\tDNS:%d.%d.%d.%d\t",\
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
//	printf("�����ӿڷ�ʽ��%d\t\t",buf[0x0]);
//	memset(buf,0,0x30);
//	Readflash_Config(buf,CAN_CONFIG_CMD);
//	printf("CAN_bound��%d\t\t",buf[0]);
//	memset(buf,0,0x30);
//	Readflash_Config(buf,RS485_CONFIG_CMD);
//	printf("485_bound��%d\t\tУ�飺%d\t\tֹͣλ��%d\t\t",buf[0x0],buf[0x4],buf[0x5]);
//	memset(buf,0,0x30);
//	Readflash_Config(buf,LCD_CONFIG_CMD);
//	printf("��������:%d\t\t",buf[0x0]);
//	printf("����ʱ�䣺%d\t\t",buf[0x01]);
//	printf("������:%d\t\t",buf[0x02]);
//	printf("�ֱ�������%d\t\t",buf[0x03]);
//	printf("����:%d\t%d\t%d\t%d\t",buf[0x04],buf[0x05],buf[0x06],buf[0x07]);
//	memset(buf,0,0x30);
//	Readflash_Config(buf,VER_CONFIG_CMD);
//	printf("Ӳ���汾:%d.%d\t\t",buf[0x0],buf[0x1]);
//	printf("����汾:%d.%d\t\t",buf[0x8],buf[0x9]);
//	memset(buf,0,0x30);
//	Readflash_Config(buf,ID_CONFIG_CMD);
//	printf("������ַ:%d\t%d\t%d\t%d\t%d\t%d\r\n",buf[0x0],buf[0x1],buf[0x2],buf[0x3],buf[0x4],buf[0x5]);

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

//��ʼ��ϵͳ����
extern unsigned char  Local_Addres[6];
extern unsigned char  Server_Interface_Mode;
extern unsigned char  remote_ip[4];											        /*Զ��IP��ַ*/
extern unsigned short remote_port;																/*Զ�˶˿ں�*/
extern unsigned char  mac[6];
extern unsigned char domain_name[30];

extern u8 local_ip[4];											/*����W5500Ĭ��IP��ַ*/
extern u8 subnet[4];												/*����W5500Ĭ����������*/
extern u8 gateway[4];												/*����W5500Ĭ������*/
extern u8 dns_server[4];									  /*����W5500Ĭ��DNS*/

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
	
	if(Config[HOST_CONFIG_CMD]==0xAB)//BT��־λ��ȡ��
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
//		printf("ϵͳ�洢��Ϣ��ʼ��\r\n");
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

//ϵͳ��������
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


//uint8 local_ip[4]={0,0,0,0};											/*����W5500Ĭ��IP��ַ*/
//uint8 subnet[4]={0,0,0,0};												/*����W5500Ĭ����������*/
//uint8 gateway[4]={0,0,0,0};												/*����W5500Ĭ������*/
//uint8 dns_server[4]={0,0,0,0};									  /*����W5500Ĭ��DNS*/
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

	printf("1.������ַ:");
	for(i=0;i<6;i++)
	{
	printf("  %x",Local_Addres[i]);
	}
	printf("\r\n");

 	printf("2.��������ģʽ:  %d ",Server_Interface_Mode);
	if(Server_Interface_Mode==2)
	{
		printf("����ģʽ��\n\r");
	}
	else
	{
		printf("����ģʽ��\n\r");		
	}
	///	
	printf("3.����MAC��ַ:");
	for(i=0;i<6;i++)
	{
	printf("%x  ",mac[i]);
	}
	printf("\r\n");
	///	
	printf("4.���߷���������:%s\r\n",domain_name);
	
	printf("5.���߷�������ַ:");
	for(i=0;i<4;i++)
	{
	printf("%d.",remote_ip[i]);
	}
	printf("\r\n");	
	///	
	printf("6.���߷������˿�:%d\r\n",remote_port);
		
	printf("7.����IP��ַ    :%d.%d.%d.%d\r\n", local_ip[0],local_ip[1],local_ip[2],local_ip[3]);

	printf("8.������������  :%d.%d.%d.%d\r\n", subnet[0],subnet[1],subnet[2],subnet[3]);

	printf("9.����Ĭ������  :%d.%d.%d.%d\r\n", gateway[0],gateway[1],gateway[2],gateway[3]);
	
	printf("10.����DNS������:%d.%d.%d.%d\r\n", dns_server[0],dns_server[1],dns_server[2],dns_server[3]);
	
  printf("11.����IPģʽ   :%d ��1=�ֶ���2=�Զ���\r\n", IPMode);
	
	printf("12.����汾 ��%s\n\r",FB1010_sv);
	
	printf("13.Э��汾 ��%s\n\r",SOFT_msg);
}

/*****************************************************************************
* ��������: void BTFlashFlagSet(void)
* ��������: 
* ��    ��:
* ����  ֵ:
*****************************************************************************/
void BTFlashFlagSet(unsigned char data)
{
	  unsigned char Indata[0x10];
	  memset(Indata,data,0x10);
		Writeflash_Config(Indata,HOST_CONFIG_CMD);//��û���ж�д��������Ƿ�ɹ�д���ˣ���
}

/*****************************************************************************
* ��������: void BTFlashFlagSet(void)
* ��������: 
* ��    ��:
* ����  ֵ:
*****************************************************************************/
void BTFlashFlagRead(unsigned char * Indata)
{
	memcpy(Indata,&Config[HOST_CONFIG_CMD],6);
}