#include "sys.h"
#include "usart.h"
#include "usart3.h"
#include "uart4.h"
#include "stmflash.h"
#include "key.h"
#include "led.h"
#include "stmflash.h"
#include "buzzer.h"
#include "function.h"
#include <math.h>
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 
/***************************���ڽ�϶�ʱ����������***************************/
unsigned char USART1_RX_FLAG=0;         //�ñ�־λ�����߶�ʱ����ʼ������,Ҳ˵���������������ݣ���Ҫ������������ϡ�
unsigned char USART1_RX_TIME=0;       	//�����ַ���ʱ��ʱ����
unsigned int 	USART1_RX_NO=0;          	//�����ַ����ַ���������
unsigned char USART1_GROUP=0;   				//�����ջ����������	
unsigned char USART1_RX_BUF[10][100]={0x00}; 	//���ջ����ά���顣
unsigned char USART1_RX_BUF_middle[100]={0x00}; 	//���ջ����ά���顣
unsigned char USART1_finger_point=0;    //������Ϣѭ����ָ��ָ���λ�á�
/***************************************************************************/
void USART1_Init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������GPIO_Mode_IN_FLOATING
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10   
	
   //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 \\
	
  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
}
//����1,�������
void USART1_SEND(unsigned char* USART1_TX_BUF,unsigned int size) // USART1_TX_BUF����Ҫ��������顣size�����������ĳ��ȡ� 
{
	unsigned int i; 
	for(i=0;i<size;i++)							//ѭ����������
	{
//	  while(USART_GetFlagStatus(UART4,UART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
		USART_SendData(USART1,USART1_TX_BUF[i]); 
	} 
}
//ͨ���жϽ�������2���ַ�֮���ʱ������20ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//USART_RX_BUF_middle[0]����������Ԫ�ص����������磺0x05 0x01 0x02 0x03 0x04 0x05
void USART1_IRQHandler(void)
{
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
/******************************************************************************************************************/	
			USART1_RX_FLAG=1;//�ñ�־λ�����߶�ʱ�����Կ�ʼΪ�ҹ����ˡ�ʱ�����ж�һ�������Ƿ����ꡣ
			USART1_RX_TIME=5;//����װ��ֵ����ʼ����ʱ�������ж��Ƿ��������һ�����ݡ�5�ν��붨ʱ���жϡ�
			USART1_RX_BUF_middle[USART1_RX_NO+1] = USART_ReceiveData(USART1);//��ȡ���յ�������		
			if(USART1_RX_NO<98)
			{
				USART1_RX_NO++;//ÿ�鴮�������յ������ݸ����ۼ�.���ó������鶨���������		
			}
			else
			{//�����������
				USART1_RX_NO=98;
			}
			USART1_RX_BUF_middle[0]=USART1_RX_NO;
/******************************************************************************************************************/			
    }
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif        
}
void USART1_RX_SCAN(void)//��ʱ����ʱɨ�贮�ڱ�־λ�ı仯����ʱ�Ľ��д���
{
	unsigned char i=0;
	if(USART1_RX_FLAG==1)//�ж��Ƿ����ڽ��մ�������
	{
		USART1_RX_TIME--;//������ʱ�Ĵ����ڵ���ֵ��һ��
		if(USART1_RX_TIME<=1)//˵��һ�������������ݵļ���������趨��ʱ���ˣ��ж����ݽ�����ϡ�
		{//�����յ��ֽ�֮���ʱ�䳬������3~5���ֽڵ�ʱ���ʱ������Ϊ���ν�����ɣ�������ɱ�־��1.
			USART1_RX_FLAG=0;//�����ڿ�ʼ��ʱ��־λ��������	
			USART1_RX_TIME=5;//��USART_RX_TIME��ֵ��������ֹ�ڼ�1��ʱ����ָ�ֵ��
			USART1_RX_NO=0;//���ڽ������ݳ�������			
/*******************ѹջ���ݰ�����*********************/	
			//������λ�õ�Ԫ��������Ϊ0������ǰ��һ��
			for(i=0;i<USART1_RX_BUF_middle[0]+1;i++)//ѹջ����
			{
				USART1_RX_BUF[USART1_GROUP][i]=USART1_RX_BUF_middle[i];
			}
			if(USART1_GROUP>=9)//Ⱥ��������ó������������ĳ��ȡ�ѭ����䡣0~19~0~19
			{
				USART1_GROUP=0;//Ⱥ������
			}
			else
			{//�����������
				USART1_GROUP++;//Ⱥ��ĸ�����1
			}
            if(USART1_RX_BUF[USART1_GROUP][0]!=0x00)//û���ü�����������Ѿ��γ�һ�����ˣ���Ҫ���ʼ��һ֡��������ˡ�
			{
				USART1_finger_point=USART1_GROUP;//��ָ��ָ���λ���� USART1_finger_point һ��һ���������ܴ�ʱfinger_point��λ�ã���
			}
		}
	}		
}
void USART1_COMAND_SCAN(void)//ˢ�¶�ȡ����1���ݡ��鿴�Ƿ������������Ҫ���� 
{
	unsigned int i=0;
	unsigned int lenth=0;//��������һ�����ݵĳ��ȡ�
	unsigned char USART1_RXD[100]={0x00};//���巢������			
	if( ( (USART1_RX_BUF[USART1_finger_point][0]!=0x00)&&(USART1_RX_FLAG==0) )==1 )//������ִ��������������������ˣ����Ҵ��������á�
	{
		//ÿ�ζ��ǽ���һ���������ȡ��������Ϊ�Ƚ��ȳ�ԭ��FIFO��
		for(i=0;i<USART1_RX_BUF[USART1_finger_point][0];i++)//USART1_RX_BUF[USART1_GROUP][0]��USART1_RX_BUF[j][i]�����ݵĳ��ȡ�ȡ����ʹ�á������7��������USART1_RX_BUF[USART1_GROUP][0]=7.
		{
			USART1_RXD[i]=USART1_RX_BUF[USART1_finger_point][i+1];//	������Ҫ���������ȡ������	
			USART1_RXD[i+1]=0x00;//��ӽ�����
		}
		lenth=USART1_RX_BUF[USART1_finger_point][0];//���������Ԫ�صĸ�����
		USART1_RX_BUF[USART1_finger_point][0]=0x00;//������������Ԫ�صĸ�����
		if(USART1_finger_point>=9)//Ⱥ��������ó������������ĳ��ȡ�ѭ����䡣0~9~0~9
		{
			USART1_finger_point=0;//Ⱥ���ָ��䵽ͷλ�á�
		}
		else
		{
			USART1_finger_point++;//Ⱥ���ָ���1��
		}
//		printf("%s\r\n",USART1_RXD);//�����ã����ղ��յ������ݴ�ӡ������
//		USART1_SEND(USART1_RXD,lenth);//�����ã����ղ��յ������ݴ�ӡ������
/************************************************************************************/	
		if((USART1_RXD[0]==0x42)&&(USART1_RXD[1]==0x54)&&(USART1_RXD[2]==0x2B)&&(USART1_RXD[lenth-2]==0x0D)&&(USART1_RXD[lenth-1]==0x0A))//BT+��ͷ��0x0D��0x0A��β��
		{
			Buzzer_flag=2;
			USART1_Data_analyze(USART1_RXD,lenth);//����1�������ݽ���.USART1_RXD���������飬lenth�����ݳ��ȡ�	
		}	
	}					
}
unsigned char memory_index[200]={0x00};//��ȡ����������0~5�豸ID,6~7��·�ţ�8~9�����ţ�10~11�洢�Ľڵ�������
void USART1_Data_analyze(unsigned char *USART1_RXD,unsigned char lenth)//����1�������ݽ���.USART1_RXD���������飬lenth�����ݳ��ȡ�
{
//�������Եĵ�ַ 124.192.224.226 �˿�5005	
	unsigned int i=0;
//	unsigned char data_lenth=0;	
	unsigned char NODE_ID_u8[30]={0x00};
//	unsigned char NODE_ID_u8_middle[30]={0x00};
//	unsigned short NODE_ID_u16[30]={0x00};		
	unsigned char printf_data[100]={0x00};//���Դ�ӡ�õ�����	
//	unsigned char SERVER_ID[50]={0};//������IP
//	unsigned char PORT_ID[20]={0};	//�������˿ں�
	u32 time_save=0;
	unsigned char time_search=0;	
	unsigned char command_lenth=0;//�����ֽڳ��ȡ�	
	unsigned char command[30]={0x00};//�����ֽڡ�	
	unsigned char data_group[30]={0x00};//�������顣	
	unsigned char data_group_lenth=0;		
	unsigned int data_group_int=0;//�����int�������ݡ�	
//����������ά�����е�һ���������������ڶ�����������Ԫ�ص�������
	unsigned char OrderCode[10][20]=
	{
	  "ROUTER_ID",			//������վID��
		"MARK_ID",				//��ӱ�ǩID��
		"ERASURE_ID",			//ɾ������ID��
		"ERASURE_ALL",		//ɾ������������ǩID
		"SERVER_ID",			//���÷�����IP
		"PORT_ID",				//���÷������˿ں�
		"HEART_TIME",			//����������վ����ʱ����	memory_index[40]
		"MARK_LEAVE_TIME",//������λ��ǩ��λʱ��			memory_index[50]
		"ROUTER_SEARCH_TIME",//��վѲ��ʱ������//1~99�롣
		"READ_CONFIGURE",	//��ȡ��ǰ����		
	};
//�������������ڴ��������з��͵�ʱ����빴ѡ�س����С�
//BT+ROUTER_ID=B22396604001    ����������վID��
//BT+MARK_ID=FB0000000140    ��ӱ�ǩID��
//BT+ERASURE_ID=FB0000000141   ɾ������ID��
//BT+ERASURE_ALL=1      ɾ������������ǩID��
//BT+SERVER_ID=192.168.1.1  ���÷�����IP
//BT+PORT_ID=10086		���÷������˿ں�
//BT+HEART_TIME=02  ����������վ����ʱ����
//BT+MARK_LEAVE_TIME=02	 ������λ��ǩ��λʱ��	
//BT+ROUTER_SEARCH_TIME=10 ����Ѳ��ʱ����Ϊ10��	
//BT+READ_CONFIGURE=1	 ��ȡ��ǰ����	
	
//��ȡ��������	
//	������
//	BT+LOCAL_ID=123456
//	 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17
//	42 54 2B 4C 4F 43 41 4C 5F 49 44 3D 31 32 33 34 35 36 
//	 B  T  +  L  O  C  A  L  _  I  D  =  1  2  3  4  5  6
		for(i=0;i<30;i++)//��ȡ�����ʽ��
		{//�����ʽ�Ϊ30���ֽڡ�
			if(USART1_RXD[i+3]==0x3D)//����������ںţ�˵�������ֽ��Ѿ�����β�ˡ�����forѭ����
			{
				i=40;//����for��
			}
			else
			{
				command[i]=USART1_RXD[i+3];//��BT+���˵���
				command_lenth=i+1;
			}
		}
//USART1_SEND(command,command_lenth);
//printf("\r\n");			

//����������վID��
//��ӱ�ǩID��
//ɾ������ID��
//ɾ������������ǩID
//���÷�����IP
//���÷������˿ں�
//����������վ����ʱ����	memory_index[40]
//������λ��ǩ��λʱ��			memory_index[50]
//��ȡ��ǰ����				
			
		//��ȡ�����еȺź�������ݡ�
		for(i=0;i<30;i++)//�����Ϊ30���ֽڡ�
		{
			if( (USART1_RXD[i+command_lenth+4]==0x0D)&&(USART1_RXD[i+command_lenth+5]==0x0A) )//��������Ȼس����У�˵�������Ѿ�����β�ˡ�����forѭ����
			{
				i=40;//����for��
			}
			else
			{
				data_group[i]=USART1_RXD[i+command_lenth+4];
				data_group_lenth=i+1;
			}
		}			
//USART1_SEND(data_group,data_group_lenth);		
//printf("\r\n");
		if(Sting_Compare(OrderCode[0],command,command_lenth))//����������վID��
		{
			if(data_group_lenth==12)
			{
				DATA_change(data_group,12,Gateway_ID); //data��׼��ת�������顣size��ת��������Ԫ�ص���������size����Ϊż������ target_data������ת�����ŵ����顣
				STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,5); // ��ȡ��ע���豸����
				for(i=0;i<6;i++)//������ID��д�����ص�eeprom��
				{
					memory_index[i]=Gateway_ID[i];
				}
				STMFLASH_Write(NUMBER_ADDRESS,(u16*)memory_index,5);//���䶯���������������д��eeprom		
	/**************������**************/
				handle_arrays_u8(memory_index,6,printf_data);
				printf("\r\n������վIDд��ɹ���ID�� ");	
				USART1_SEND(printf_data,12);
				printf("\r\n");					
			}
			else
			{
				printf("������վID���ݳ����������������롣\r\n");	
			}
		}
		else if(Sting_Compare(OrderCode[1],command,command_lenth))//��ӱ�ǩID��
		{
			if(data_group_lenth==12)
			{
				DATA_change(data_group,12,NODE_ID_u8);//ת�����ݡ�
				DATA_change_u8_to_u16(NODE_ID_u8,12,judge_data_u16);//��6��8λ��Ԫ������ת����3��16λԪ�ص����顣				
				Judge_ID(judge_data_u16);//�жϺϷ��ԡ��Ƿ��Ѿ���EEPROM���д洢ע��,������������е�NODE_ID[6]��NODE_ID[7]���Ƿ�0x00��ֵ�������Ѿ�ע���豸���ڵ�λ�á��������NODE_ID[6]��NODE_ID[7]��Ϊ0x00������δע���豸��	
				if(judge_data_u16[4]==0x0000)//����ڵ�û�б�ע��
				{
	//				Buzzer_flag=1;
					if(Bluetooh_amount>=Bluetooh_Max_NO)
					{
						printf("��վ��������ǩ�Ѿ�����%d����\r\n",Bluetooh_amount);
					}
					else
					{
						//��������ǩID���д洢��
						DATA_change_u16_to_u8(judge_data_u16,6,NODE_ID_u8);
						i=Save_NODE_ID(NODE_ID_u8);//NODE_ID_u8�ĳ���Ϊ10λ����0~5��λΪ�豸ID����6~7��Ϊ��Ҫ�����λ�ã���8~9�����á�					
						if(i==0xFFFF)//�����豸IDʧ��
						{
							printf("����������ǩIDʧ��\r\n");
						}
						else//�����豸ID�ɹ���
						{//��ӡ�豸ע��ɹ�
	//						Buzzer_flag=4;//����������һ�¡�
							printf("���β��뵽�洢�����е�λ��Ϊ�� %d\r\n",i);		
							printf("������վ��ע���ID����Ϊ�� %d\r\n",Bluetooh_amount);							
							printf("������ǩע��ɹ���ID��");
							handle_arrays_u8(NODE_ID_u8,6,printf_data);
							USART1_SEND(printf_data,12);	
							printf("\r\n");												
						}					
					}
				}
				else//�豸�Ѿ���ע�����0x0000��һ�������ܵ�ֵ���������������˵���豸û����λ�����������豸�����б�־λ��������һ�ֵ��������ڡ�
				{
						printf("������ǩ�ѱ�ע�ᡣID��");
						handle_arrays_u8(NODE_ID_u8,6,printf_data);
						USART1_SEND(printf_data,12);	
						printf("\r\n");			
				}				
			}
			else
			{
				printf("������ǩID���ݳ����������������롣\r\n");	
			}			
		}
		else if(Sting_Compare(OrderCode[2],command,command_lenth))//ɾ������ID��
		{
			if(data_group_lenth==12)
			{
				DATA_change(data_group,12,NODE_ID_u8);//ת�����ݡ�
				DATA_change_u8_to_u16(NODE_ID_u8,12,judge_data_u16);//��6��8λ��Ԫ������ת����3��16λԪ�ص����顣				
				Judge_ID(judge_data_u16);//�жϺϷ��ԡ��Ƿ��Ѿ���EEPROM���д洢ע��,������������е�NODE_ID[6]��NODE_ID[7]���Ƿ�0x00��ֵ�������Ѿ�ע���豸���ڵ�λ�á��������NODE_ID[6]��NODE_ID[7]��Ϊ0x00������δע���豸��	
				if(judge_data_u16[4]==0x0000)//����ڵ�û�б�ע��
				{
					printf("������λ��վ���޴�ID������Ҫɾ����\r\n");					
				}
				else//�豸�Ѿ���ע�ᡣ��鿴��Ѳ����Ϣ���Ǳ�����Ϣ��
				{	
	///************ɾID��************/		
					DATA_change_u16_to_u8(judge_data_u16,4,NODE_ID_u8); //��һ��16λ����ת��������8λ���ݡ�
					Delete_NODE_ID(NODE_ID_u8);//ɾ��ID��
					handle_arrays_u8(NODE_ID_u8,6,printf_data);//�����á�
					printf("�ɹ�ɾ��������λ��վ���豸ID: ");	
					USART1_SEND(printf_data,12);
	//				printf(" ɾ���ɹ���\r\n");					
					printf("\r\n");					
				}				
			}
			else
			{
				printf("������ǩID���ݳ����������������롣\r\n");	
			}			
		}	
		else if(Sting_Compare(OrderCode[3],command,command_lenth))//ɾ������������ǩID
		{
			if( (data_group_lenth==1)&&(data_group[0]==0x31) )
//			if( (data_group_lenth==1) )				
			{
	//	/*-------����---------*/
	//	42 54 2B 45 52 41 53 55 52 45 5F 49 44 3D			��ͷ  BT+ERASURE_ID=  			 14���ֽ�
	//	41 4C 4C		ɾ�������ԣ�������ȫ��ɾ����ALL	 12���ֽ�						
				STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,100); // ��ȡ��ע���豸����		
				Bluetooh_amount=0;	
				memory_index[6]=0x00;		
				memory_index[7]=0x00;				
				STMFLASH_Write(NUMBER_ADDRESS,(u16*)memory_index,100);//���䶯���������������д��eeprom		
				printf("����ID�Ѿ���ɾ��\r\n");					
			}
			else
			{
				printf("ɾ�������������������롣\r\n");					
			}				
		}	
		else if(Sting_Compare(OrderCode[4],command,command_lenth))//���÷�����IP
		{
			printf("\r\n������IP���óɹ���������IP�� ");
			USART1_SEND(data_group,data_group_lenth);						
			printf("\r\n");	
			STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,100); // ��ȡ��ע���豸����
			for(i=0;i<data_group_lenth;i++)//������ID��д�����ص�eeprom��
			{
				data_group[data_group_lenth-i]=data_group[data_group_lenth-i-1];
			}	
			data_group[0]=data_group_lenth;//������ID�ĳ���
			for(i=0;i<data_group[0]+1;i++)//������ID��д�����ص�eeprom��
			{
				memory_index[11+i]=data_group[i];
				memory_index[10]=0xAA;//��־λ��1����ʾ�Ѿ����ù�������ID�����û���ù������������GPRSдID��
			}
			STMFLASH_Write(NUMBER_ADDRESS,(u16*)memory_index,100);//���䶯���������������д��eeprom							
		}			
		else if(Sting_Compare(OrderCode[5],command,command_lenth))//���÷������˿ں�
		{//31,32
			if(data_group_lenth<=5)
			{
				printf("\r\n�������˿����óɹ����������˿ڣ� ");	
				USART1_SEND(data_group,data_group_lenth);						
				printf("\r\n");				
				STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,100); // ��ȡ��ע���豸����
				memory_index[30]=0xAA;//��־λ��λ��			
				data_group_int=str_int(data_group);
				memory_index[31]=data_group_int/256;
				memory_index[32]=data_group_int%256;
				STMFLASH_Write(NUMBER_ADDRESS,(u16*)memory_index,100);//д��䶯������ݡ�						
			}
			else
			{
				printf("�������˿ڳ������������������롣\r\n");							
			}
		}
		else if( (Sting_Compare(OrderCode[6],command,command_lenth))&&(data_group[data_group_lenth-1]!=0x20) )//����������վ����ʱ����.�����һλ���õ��ڿո�
		{
            if((data_group_lenth>0)&&(data_group_lenth<6))
            {
                time_save=0;
                for(i=0;i<data_group_lenth;i++)
                {
                    time_save+=(data_group[i]-0x30)*pow(10,data_group_lenth-i-1);
                }
            }
            
//			if(data_group_lenth==1)		 //���ݳ���Ϊһλ
//			{
//				time_save=data_group[0]-0x30;
//			}
//			else if(data_group_lenth==2)//���ݳ���Ϊ��λ
//			{
//				time_save=(data_group[0]-0x30)*10+(data_group[1]-0x30);
//			}
//			else if(data_group_lenth==3)//���ݳ���Ϊ��λ
//			{
//				time_save=(data_group[0]-0x30)*100+(data_group[1]-0x30)*10+(data_group[2]-0x30);				
//			}
//            else if(data_group_lenth==4)//���ݳ���Ϊ��λ
//			{
//				time_save=(data_group[0]-0x30)*1000+(data_group[1]-0x30);
//			}
//			else if(data_group_lenth==5)//���ݳ���Ϊ��λ
//			{
//				time_save=(data_group[0]-0x30)*10000+(data_group[1]-0x30)*10+(data_group[2]-0x30);				
//			}
			else
			{
//				printf("������վ����ʱ���������������������롣\r\n");
				time_save=255;
			}
			printf("������վ����ʱ������ %d ����\r\n",time_save);			
			STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,100); // ��ȡ��ע���豸����
			memory_index[40]=time_save&0x000000FF;
            memory_index[41]=(time_save&0x0000FF00)>>8;
            memory_index[42]=(time_save&0x00FF0000)>>16;
            memory_index[43]=(time_save&0xFF000000)>>24;
			STMFLASH_Write(NUMBER_ADDRESS,(u16*)memory_index,100);//���䶯���������������д��eeprom						
		}	
		else if(Sting_Compare(OrderCode[7],command,command_lenth))//������λ��ǩ��λʱ��
		{
            if((data_group_lenth>0)&&(data_group_lenth<6))
            {
                time_save=0;
                for(i=0;i<data_group_lenth;i++)
                {
                    time_save+=(data_group[i]-0x30)*pow(10,data_group_lenth-i-1);
                }
            }
//			if(data_group_lenth==1)		 //���ݳ���Ϊһλ
//			{
//				time_save=data_group[0]-0x30;
//			}
//			else if(data_group_lenth==2)//���ݳ���Ϊ��λ
//			{
//				time_save=(data_group[0]-0x30)*10+(data_group[1]-0x30);
//			}
//			else if(data_group_lenth==3)//���ݳ���Ϊ��λ
//			{
//				time_save=(data_group[0]-0x30)*100+(data_group[1]-0x30)*10+(data_group[2]-0x30);				
//			}
			else
			{
				time_save=255;
			}
			printf("�ж�������λ��ǩ��λʱ������ %d ����\r\n",time_save);			

			STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,100); // ��ȡ��ע���豸����
			memory_index[50]=time_save&0x000000FF;
            memory_index[51]=(time_save&0x0000FF00)>>8;
            memory_index[52]=(time_save&0x00FF0000)>>16;
            memory_index[53]=(time_save&0xFF000000)>>24;
			STMFLASH_Write(NUMBER_ADDRESS,(u16*)memory_index,100);//���䶯���������������д��eeprom						
/****************************/			
//			Star_Read_ID();//��ȡ����ӡ�豸ID .����ȫ�ֱ�����				
		}
		else if(Sting_Compare(OrderCode[8],command,command_lenth))//���û�վѲ��ʱ������//1~99�롣
		{	
			if(data_group_lenth==1)		 //���ݳ���Ϊһλ
			{
				time_search=data_group[0]-0x30;
			}
			else if(data_group_lenth==2)//���ݳ���Ϊ��λ
			{
				time_search=(data_group[0]-0x30)*10+(data_group[1]-0x30);
			}
			else if(data_group_lenth==3)//���ݳ���Ϊ��λ
			{
				time_search=(data_group[0]-0x30)*100+(data_group[1]-0x30)*10+(data_group[2]-0x30);				
			}
			else
			{
				time_search=255;
			}
			printf("������λ��վ������ǩ��ʱ������ %d ��\r\n",time_search);			

			STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,100); // ��ȡ��ע���豸����
			memory_index[60]=time_search;
			bluetooh_scan_heart_time=time_search;
			STMFLASH_Write(NUMBER_ADDRESS,(u16*)memory_index,100);//���䶯���������������д��eeprom						
/****************************/			
//			Star_Read_ID();//��ȡ����ӡ�豸ID .����ȫ�ֱ�����		
		}
		else if(Sting_Compare(OrderCode[9],command,command_lenth))//��ȡ����ӡ��ǰ�洢�е�����
		{
			if( (data_group_lenth==1)&&(data_group[0]==0x31) )		
			{
				Star_Read_ID();//��ȡ����ӡ�豸ID .����ȫ�ֱ�����					
			}
			else
			{
				printf("�鿴�����������������롣\r\n");					
			}
		}	
		Refurbish_Star_Read_ID();	//ˢ��ȫ�ֵı�־λ��		
}























