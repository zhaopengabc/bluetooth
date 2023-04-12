#include "gprs.h"
#include "usart.h"
#include "usart2.h"
#include "delay.h"
#include "string.h" 
#include "buzzer.h"
#include "usart3.h"	  
#include "stmflash.h"
#include "iwdg.h"
#include "bc95.h"
#define GPRS_ST_Read() GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)
#define GPRS_LINKA_Read() GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6)
//unsigned char GPRS_COMMAND_MODE_flag=0;//����ģʽ���������1������������ģʽ�£�������
unsigned int GM3_LINKA_TIME_LOSE=0;//GPRS����ʱ�䡣����������ʱ�䣬����Ҫ����GPRSģ���ˡ�Ӳ�����LINK���ţ���Ϊ��Ҫ�޸Ĳ��ԣ���������ȫ�ֱ������൱�ں궨�塣
unsigned char sendata_flag=0;
unsigned int GM3_LINKA_TIME=0;//GPRS����ʱ�䡣����������ʱ�䣬����Ҫ����GPRSģ���ˡ�
////		send_IP_port(data1,data2);	
void send_IP_port(u8 *IP,u8 *port)
{
	unsigned char i=0;
	unsigned char t=0;	
	unsigned char data[80]={0};
	for(i=0;i<80;i++)
	{
		data[i]=0;
	}
	data[0]=0x41;
	data[1]=0x54;
	data[2]=0x2B;
	data[3]=0x53;
	data[4]=0x4F;
	data[5]=0x43;
	data[6]=0x4B;
	data[7]=0x41;
	data[8]=0x3D;
	data[9]=0x22;
	data[10]=0x54;
	data[11]=0x43;
	data[12]=0x50;
	data[13]=0x22;
	data[14]=0x2C;
	data[15]=0x22;
	for(i=0;i<IP[0];i++)
	{
		data[16+i]=IP[i+1];		
	}
	t=16+i;
	data[t]=0x22;	
	t=t+1;	
	data[t]=0x2C;	
	t=t+1;		
	for(i=0;i<port[0];i++)
	{
		data[t+i]=port[i+1];
	}
	t=t+i;
	data[t]=0x0D;
	t=t+1;		
	data[t]=0x0A;	
	t=t+1;			
	GPRS_senddata(data,t);//USART2_SEND(data,t);		
//	u2_printf("%s",data);//��������\r\n	
//	printf("%s",data);//��������	
}	
void GPRS_RESTAR(void)//GPRS������
{
	/*unsigned short time_delay=5000;
	unsigned short time_delay_OK=0;	
	GM3_LINKA_TIME=GM3_LINKA_TIME_LOSE;//GPRS����ʱ���������
	Buzzer_flag=4;	
//	GPRS_REST_ENABLE;		//GPRS��λ
	GPRS_SOFT_POWER_DISABLE;//GM3��ػ�	
	Watchdog_FEED();//ι��	
	delay_ms(500);
	Watchdog_FEED();//ι�����������Ź���		
	delay_ms(500);
	Watchdog_FEED();//ι�����������Ź���		
	delay_ms(500);
	Watchdog_FEED();//ι�����������Ź���		
	delay_ms(500);			
	Watchdog_FEED();//ι�����������Ź���			
	GPRS_MOS_POWER_DISABLE;	//GPRS��Դ�Ͽ�	
	delay_ms(500);
	Watchdog_FEED();//ι�����������Ź���	
	delay_ms(500);
	Watchdog_FEED();//ι�����������Ź���		
	GPRS_REST_DISABLE;		//GPRS����λ		
	GPRS_SOFT_POWER_ENABLE;//GM3����		
	delay_ms(500);	
	Watchdog_FEED();//ι�����������Ź���		
	GPRS_MOS_POWER_ENABLE;		//GPRS��Դ��ͨ	
	time_delay=5000;		
	while(time_delay)//GM3��WORK���š�ģ������������ ��ƽһ��仯һ�Σ� �ߵ͵�ƽ����任�� ָʾ�Ƴ���˸״̬��
	{
		Watchdog_FEED();//ι�����������Ź���	
		time_delay=time_delay-1;
		delay_ms(1);
		if(READ_GPRS_POWER_STATUS==0)//���WORK���ŵ�ƽ״̬���ߵ͵�ƽ������֡�
		{//���ߵ�ƽά�ֵ�ʱ�䡣����һ��ʱ�䣬����Ϊ�������ɹ��ˡ�
			Watchdog_FEED();//ι�����������Ź���			
			time_delay_OK=time_delay_OK+1;
			if(time_delay_OK>=800)//˵��ģ���Ѿ�����������
			{
				time_delay=0;//�˳��ȴ���
			}
		}
	}*/
}


void GPRS_Init(void)
{
	unsigned char i=0;	
//	unsigned int  port=0;	
	unsigned int  data_long=0;	
	unsigned char SERVER_ID[50]={0x00};//������IP
	unsigned char PORT_ID[20]={0x00};	//�������˿ں�	
	unsigned char PORT_ID_MIDDLE[20]={0x00};	//�������˿ں�			
	unsigned char test[10]={0x00};		
	unsigned char memory_index[100]={0x00};//��ȡ����������0~5�豸ID,6~7��·�ţ�8~9�����ţ�10~11�洢�Ľڵ�������	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				//MOS_PWR
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				//GM3��ػ�
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				//READ_GPRS_POWER_STATUS
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				//Reset
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				//LINKA  �ߵ�ƽ����������ɹ���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//����
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ;//��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);

//	printf("���ڳ�ʼ��GM3ģ�顣����\r\n");
//	u3_printf("���ڳ�ʼ��GM3ģ�顣����\r\n");
	GPRS_RESTAR();//GPRS������
	STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,50); // ��ȡ��ע���豸����
	if( (memory_index[10]==0xAA)&&(memory_index[30]==0xAA) )//�鿴IP�Ͷ˿��Ƿ��д洢��
	{
		/*sendata_flag=1;
		//����AT����ģʽ��
		gm3_send_cmd("+++","a",800);//���� 	
		delay_ms(500);
		sendata_flag=1;		
		gm3_send_cmd("a","+ok",800);//
		delay_ms(500);*/
/******************GM3������IP******************/		
		/*for(i=0;i<memory_index[11]+1;i++)//��ȡ������IP
		{
			SERVER_ID[i]=memory_index[i+11];//��������վ������IP��ȡ������������IP�ĵ�һ���ֽڴ����ȡ�
		}*/
/******************GM3���������Զ˿ں�******************/	
		/*for(i=0;i<2;i++)//��ȡ�������˿�
		{
			PORT_ID_MIDDLE[i]=memory_index[i+31];//��������վ�������˿���ȡ�������������˿ڵĵ�һ���ֽڴ����ȡ�
		}		
		data_long=PORT_ID_MIDDLE[0]*256+PORT_ID_MIDDLE[1];
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
			PORT_ID[0]=5;			
			PORT_ID[1]=test[0];
			PORT_ID[2]=test[1];			
			PORT_ID[3]=test[2];			
			PORT_ID[4]=test[3];			
			PORT_ID[5]=test[4];				
		}		
		else if( (test[0]==0x30)&&(test[1]!=0x30) )
		{//�˿ں���4λ�������磺01234.
			PORT_ID[0]=4;			
			PORT_ID[1]=test[1];
			PORT_ID[2]=test[2];			
			PORT_ID[3]=test[3];			
			PORT_ID[4]=test[4];			
		}
		else if( (test[0]==0x30)&&(test[1]==0x30)&&(test[2]!=0x30) )
		{
			PORT_ID[0]=3;			
			PORT_ID[1]=test[2];
			PORT_ID[2]=test[3];			
			PORT_ID[3]=test[4];						
		}
		send_IP_port(SERVER_ID,PORT_ID);		
		gm3_send_cmd("AT+WKMOD=\"NET\"","OK",800);//
		delay_ms(500);
		gm3_send_cmd("AT+SOCKAEN=\"on\"","OK",800);//
		delay_ms(500);
//AT+SOCKA="TCP","dtu.fubangyun.cn",9123
		
//OK
		send_IP_port(SERVER_ID,PORT_ID);
		delay_ms(500);
		gm3_send_cmd("AT+SOCKASL=\"long\"","OK",800);//		
		delay_ms(500);
		gm3_send_cmd("AT+SOCKBEN=\"off\"","OK",800);//	
		delay_ms(500);
		gm3_send_cmd("AT+HEARTEN=\"off\"","OK",800);//	
		delay_ms(500);
		gm3_send_cmd("AT+REGEN=\"off\"","OK",800);//	
		delay_ms(500);
		gm3_send_cmd("AT+UART=115200,\"NONE\",8,1,\"RS485\"","OK",800);//			
		delay_ms(500);
		gm3_send_cmd("AT+APN=\"CMNET\","",""","OK",800);//	
		delay_ms(500);
		gm3_send_cmd("AT+S","OK",500);//���沢����	*/	
	}
	else
	{
		printf("�鿴IP�Ͷ˿ڿ���û�д洢\r\n");
	}
	
}
//SIM800C��������
//cmd:���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 gm3_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	unsigned char USART2_RXD[200]={0x00};//���巢������	
	unsigned short i=0;
	unsigned short lenth=0;
	char res=0;
	unsigned char break_flag=1;//����ѭ����־λ	
	if(sendata_flag==1)
	{
		sendata_flag=0;
		u2_printf("%s",cmd);//��������\r\n		
	}
	else
	{
		u2_printf("%s\r\n",cmd);//��������		
	}
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while( (break_flag)&&(--waittime) )	//�ȴ�����ʱ
		{
			delay_ms(1);
			if( ( (USART2_RX_BUF[USART2_finger_point][0]!=0x00)&&(USART2_RX_FLAG==0) )==1 )//������ִ��������������������ˣ����Ҵ��������á�
			{
				//ÿ�ζ��ǽ���һ���������ȡ��������ѭ�Ƚ��ȳ�ԭ��FIFO��
				for(i=0;i<USART2_RX_BUF[USART2_finger_point][0];i++)//USART2_RX_BUF[USART2_GROUP][0]��USART2_RX_BUF[j][i]�����ݵĳ��ȡ�ȡ����ʹ�á������7��������USART2_RX_BUF[USART1_GROUP][0]=7.
				{
					USART2_RXD[i]=USART2_RX_BUF[USART2_finger_point][i+1];//	������Ҫ���������ȡ������	
					USART2_RXD[i+1]=0x00;//��ӽ�����
				}
				lenth=USART2_RX_BUF[USART2_finger_point][0];//���������Ԫ�صĸ�����
				USART2_RX_BUF[USART2_finger_point][0]=0x00;//������������Ԫ�صĸ�����
				if(USART2_finger_point>=9)//Ⱥ��������ó������������ĳ��ȡ�ѭ����䡣0~9~0~9
				{
					USART2_finger_point=0;//Ⱥ���ָ��䵽ͷλ�á�
				}
				else
				{
					USART2_finger_point++;//Ⱥ���ָ���1��
				}
		/************************************************************************************/		
//				USART1_SEND(USART2_RXD,lenth);//����2�������ݽ���.USART2_RXD���������飬lenth�����ݳ��ȡ�		
				if(gm3_check_cmd(ack,USART2_RXD))
				{
					res=1;//�õ���Ч����					
				}
				break_flag=0;//��������whileѭ����
			}									
		}
		if(waittime==0)
		{
			res=0; 
		}
	}
	return res;
} 

//SIM800C���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//����,�ڴ�Ӧ������λ��(str��λ��)
unsigned char *gm3_check_cmd(unsigned char *receive_data,unsigned char *str)
{
	char *strx=0;
	strx=strstr((const char*)receive_data,(const char*)str);//strstr(str1,str2) ���������ж��ַ���str2�Ƿ���str1���Ӵ�������ǣ���ú�������str2��str1���״γ��ֵĵ�ַ�����򣬷���NULL��
	return (unsigned char*)strx;
}

