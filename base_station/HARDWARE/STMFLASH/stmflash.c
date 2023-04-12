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
//unsigned int  Bluetooh_Max_NO=8;//������վ���Ŷ�洢�ı�ǩ������
unsigned char Gateway_ID[6]={0x00};//����ID
unsigned char Bluetooh_amount=0;//������ǩ������
//��ȡָ����ַ�İ���(16λ����)
//faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
//����ֵ:��Ӧ����.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}
#if STM32_FLASH_WREN	//���ʹ����д   
//������д��
//WriteAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��   
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//��ַ����2.
	}  
} 
//��ָ����ַ��ʼд��ָ�����ȵ�����
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
//pBuffer:����ָ��
//NumToWrite:����(16λ)��(����Ҫд���16λ���ݵĸ���.)
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //�ֽ�
#else 
#define STM_SECTOR_SIZE	2048
#endif		 
u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//�����2K�ֽ�
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //������ַ
	u16 secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
	u16 secremain; //������ʣ���ַ(16λ�ּ���)	   
 	u16 i;    
	u32 offaddr;   //ȥ��0X08000000��ĵ�ַ
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//�Ƿ���ַ
	FLASH_Unlock();						//����
	offaddr=WriteAddr-STM32_FLASH_BASE;		//ʵ��ƫ�Ƶ�ַ.
	secpos=offaddr/STM_SECTOR_SIZE;			//������ַ  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//����ʣ��ռ��С   
	if(NumToWrite<=secremain)secremain=NumToWrite;//�����ڸ�������Χ
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//�����������
			for(i=0;i<secremain;i++)//����
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//д����������  
		}else STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;				//������ַ��1
			secoff=0;				//ƫ��λ��Ϊ0 	 
		   	pBuffer+=secremain;  	//ָ��ƫ��
			WriteAddr+=secremain;	//д��ַƫ��	   
		   	NumToWrite-=secremain;	//�ֽ�(16λ)���ݼ�
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//��һ����������д����
			else secremain=NumToWrite;//��һ����������д����
		}	 
	};	
	FLASH_Lock();//����
}
#endif

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
		ReadAddr+=2;//ƫ��2���ֽ�.	
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//WriteAddr:��ʼ��ַ
//WriteData:Ҫд�������
void Test_Write(u32 WriteAddr,u16 WriteData)   	
{
	STMFLASH_Write(WriteAddr,&WriteData,1);//д��һ���� 
}
void Star_Read_ID()//��ȡ����ӡ�豸ID
{
	unsigned char i=0;	
	unsigned char printf_data[50]={0x00};
	unsigned char middle_data[50]={0x00};	
	unsigned char memory_index[100]={0x00};//��ȡ����������0~5�豸ID,6~7��·�ţ�8~9�����ţ�10~11�洢�Ľڵ�������	
	unsigned char SERVER_ID[50]={0x00};//������IP
//	unsigned char PORT_ID[20]={0x00};	//�������˿ں�	
	unsigned int  data_long=0;	
	unsigned char test[10]={0x00};		
	unsigned char test1[10]={0x00};			
	unsigned char data[30]={0x00};		
	STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,50); // ��ȡ��ע���豸����
	for(i=0;i<6;i++)//��ȡ������վ��ID
	{
		Gateway_ID[i]=memory_index[i];//��������վID��ȡ����������ȫ�ֱ�����
	}
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n");
	printf("������վID�� ");
	handle_arrays_u8(Gateway_ID,6,printf_data);
	USART1_SEND(printf_data,12);
	printf("\r\n");
//	middle_data[0]=memory_index[8];//��ȡ������վ�Ļ�·��
//	printf("������վ��·�ţ� ");
//	handle_arrays_u8(middle_data,1,printf_data);
//	USART1_SEND(printf_data,2);
//	printf("\r\n");
//	middle_data[0]=memory_index[9];//��ȡ������վ�Ĳ�����
//	printf("������վ�����ţ� ");
//	handle_arrays_u8(middle_data,1,printf_data);
//	USART1_SEND(printf_data,2);
//	printf("\r\n");


	if(memory_index[10]==0xAA)//������ID�Ѿ����ù�
	{
		for(i=0;i<memory_index[11];i++)//��ȡ������IP
		{
			SERVER_ID[i]=memory_index[i+12];//��������վ������IP��ȡ����������ȫ�ֱ�����������IP�ĵ�һ���ֽڴ����ȡ�
		}
		printf("������վ������IP�� ");
		USART1_SEND(SERVER_ID,memory_index[11]);
		printf("\r\n");	
	}
	else
	{
		printf("������վ������IP��δ���ù�\r\n");
	}
	if(memory_index[30]==0xAA)//�������˿ں��Ѿ����ù�
	{
	/******************K2���������Զ˿ں�******************/	
		data[0]=memory_index[31];//������ȡ����
		data[1]=memory_index[32];
		data_long=data[0]*256+data[1];
		test[0]=(data_long/10000)+0x30;// ȡ��������
		data_long=data_long%10000;	 // ȡ����
		test[1]=(data_long/1000)+0x30;
		data_long=data_long%1000;	
		test[2]=(data_long/100)+0x30;
		data_long=data_long%100;		
		test[3]=(data_long/10)+0x30;		
		data_long=data_long%10;		
		test[4]=data_long+0x30;	
		if( test[0]!=0x30 )
		{//�˿ں���5λ�������磺10086.
			test1[0]=test[0];			
			test1[1]=test[1];
			test1[2]=test[2];			
			test1[3]=test[3];			
			test1[4]=test[4];	
			test1[5]=0x00;			
		}		
		else if( (test[0]==0x30)&&(test[1]!=0x30) )
		{//�˿ں���4λ�������磺01234.
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
		printf("K2���������Զ˿ں�:%s\r\n",test1);					
	}
	else
	{
		printf("������վ�������˿ںŻ�δ���ù�\r\n");	
	}

//	for(i=0;i<memory_index[31];i++)//��ȡ�������˿ں�	
//	{
//		PORT_ID[i]=memory_index[i+32];//��������վ�������˿ں�	��ȡ����������ȫ�ֱ������������˿ںŵĵ�һ���ֽڴ����ȡ�
//	}
//	printf("������վ�������˿ںţ� ");	
//	USART1_SEND(PORT_ID,memory_index[30]);
//	printf("\r\n");	
	printf("������վ����ʱ������ %d ����\r\n",memory_index[40]+memory_index[41]*256+memory_index[42]*256*256+memory_index[43]*256*256*256);			
	printf("�ж�������λ��ǩ��λʱ������ %d ����\r\n",memory_index[50]+memory_index[51]*256+memory_index[52]*256*256+memory_index[53]*256*256*256);	
	printf("������λ��վ������ǩ��ʱ������ %d ��\r\n",memory_index[60]);		

//����������ܹ�ע���˶��ٸ��豸��	
	Bluetooh_amount=memory_index[6];//��λ
	Bluetooh_amount=(Bluetooh_amount<<8)+memory_index[7];//��λ�ӵ�λ			
	
	if(Bluetooh_amount>Bluetooh_Max_NO)//���ע8��������ǩ��
	{//����״�ʹ�õ�Ƭ������ID�Ѿ����ˣ�255���������
		Bluetooh_amount=0;
		memory_index[6]=0x00;		
		memory_index[7]=0x00;
		printf("�����л�δ¼���豸ID");
		STMFLASH_Write(NUMBER_ADDRESS,(u16*)memory_index,5);//���䶯���������������д��eeprom		
	}
	else
	{
		printf("������վ����ע��������ǩ����Ϊ��%d\r\n\r\n",Bluetooh_amount);
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
void Judge_Erase_all_ID()//�ж������Ƿ���Ҫ��������ID��	
{
//	unsigned char memory_index[20]={0};//��ȡ����������0~5�豸ID,6~7��·�ţ�8~9�����ţ�10~11�洢�Ľڵ�������
	if(KEY_flag==1)
	{
		KEY_flag=0;
		Buzzer_flag=1;			
		dislocation_test();
		delay_ms(100);		
		printf("���Ͳ���֡\r\n");		
	}
//	if(KEY_flag==2)//��������������ͷ���һ֡���ݰ���
//	{
//		KEY_flag=0;
//		Buzzer_flag=1;	
//		printf("��ձ��������ID\r\n");
//		delay_ms(100);
//		Delete_ALL_NODE_ID();//ɾ������ע����豸ID
//	}		
}


void Refurbish_Star_Read_ID()//ˢ��ȫ�ֵı�־λ��
{
	unsigned char i=0;	
	unsigned char printf_data[50]={0x00};
	unsigned char middle_data[50]={0x00};	
	unsigned char memory_index[100]={0x00};//��ȡ����������0~5�豸ID,6~7��·�ţ�8~9�����ţ�10~11�洢�Ľڵ�������	
	unsigned char SERVER_ID[50]={0x00};//������IP
//	unsigned char PORT_ID[20]={0x00};	//�������˿ں�	
	unsigned int  data_long=0;	
	unsigned char test[10]={0x00};		
	unsigned char test1[10]={0x00};			
	unsigned char data[30]={0x00};		
	STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,50); // ��ȡ��ע���豸����
	for(i=0;i<6;i++)//��ȡ������վ��ID
	{
		Gateway_ID[i]=memory_index[i];//��������վID��ȡ����������ȫ�ֱ�����
	}
	if(memory_index[10]==0xAA)//������ID�Ѿ����ù�
	{
		for(i=0;i<memory_index[11];i++)//��ȡ������IP
		{
			SERVER_ID[i]=memory_index[i+12];//��������վ������IP��ȡ����������ȫ�ֱ�����������IP�ĵ�һ���ֽڴ����ȡ�
		}
	}
	else
	{
	}
	if(memory_index[30]==0xAA)//�������˿ں��Ѿ����ù�
	{
	/******************K2���������Զ˿ں�******************/	
		data[0]=memory_index[31];//������ȡ����
		data[1]=memory_index[32];
		data_long=data[0]*256+data[1];
		test[0]=(data_long/10000)+0x30;// ȡ��������
		data_long=data_long%10000;	 // ȡ����
		test[1]=(data_long/1000)+0x30;
		data_long=data_long%1000;	
		test[2]=(data_long/100)+0x30;
		data_long=data_long%100;		
		test[3]=(data_long/10)+0x30;		
		data_long=data_long%10;		
		test[4]=data_long+0x30;	
		if( test[0]!=0x30 )
		{//�˿ں���5λ�������磺10086.
			test1[0]=test[0];			
			test1[1]=test[1];
			test1[2]=test[2];			
			test1[3]=test[3];			
			test1[4]=test[4];	
			test1[5]=0x00;			
		}		
		else if( (test[0]==0x30)&&(test[1]!=0x30) )
		{//�˿ں���4λ�������磺01234.
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
//����������ܹ�ע���˶��ٸ��豸��	
	Bluetooh_amount=memory_index[6];//��λ
	Bluetooh_amount=(Bluetooh_amount<<8)+memory_index[7];//��λ�ӵ�λ			
}


