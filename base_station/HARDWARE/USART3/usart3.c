#include "usart.h"
#include "usart2.h"
#include "usart3.h"
#include "uart4.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"
#include "led.h"
#include "key.h"
#include "stmflash.h"
#include "buzzer.h"
#include "function.h"
#include "timer.h"
#include "os.h"
/***************************���ڽ�϶�ʱ����������***************************/ 
unsigned char USART3_RX_FLAG=0;         //�ñ�־λ�����߶�ʱ����ʼ������,Ҳ˵���������������ݣ���Ҫ������������ϡ�
unsigned char USART3_RX_TIME=0;       	//�����ַ���ʱ��ʱ����
unsigned int 	USART3_RX_NO=0;          	//�����ַ����ַ���������
unsigned char USART3_GROUP=0;   				//�����ջ����������	
unsigned char USART3_RX_BUF[10][100]={0x00}; 	//���ջ����ά���顣
unsigned char USART3_RX_BUF_middle[100]={0x00}; 	//���ջ����ά���顣
unsigned char USART3_finger_point=0;    //������Ϣѭ����ָ��ָ���λ�á�
/***************************************************************************/
unsigned char send_success_flag=0;//�����ػ��߷������������ݳɹ���־λ���ɹ�������1��ʧ�ܣ�����0��
//��ʼ��IO ����3
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void USART3_Init(u32 bound)
{ 	
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);// GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

 	USART_DeInit(USART3);                                //��λ����1
	 //USART3_TX   PB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;            //PB.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	     //�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);               //��ʼ��PB.10
 
	//USART3_RX	  PB.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;            //PB.11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������  GPIO_Mode_IN_FLOATING
	GPIO_Init(GPIOB, &GPIO_InitStructure);               //��ʼ��PB.11
	
	USART_InitStructure.USART_BaudRate = bound;                                    //һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;                            //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	               //�շ�ģʽ
  
	USART_Init(USART3, &USART_InitStructure); //��ʼ������	2 
	USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�	
	
	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���		
}
 
//����2,printf ����
//ȷ��һ�η������ݲ�����USART2_MAX_SEND_LEN�ֽ�
void u3_printf(char* fmt,...)  
{  
	u16 i,j; 
	u8 USART3_TX_BUF[200]={0x00}; 			//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
//	  while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
		USART_SendData(USART3,USART3_TX_BUF[j]); 
	} 
}

//����2,�������
void USART3_SEND(unsigned char* USART3_TX_BUF,unsigned int size) // USART3_TX_BUF����Ҫ��������顣size�����������ĳ��ȡ� 
{  
	u16 i; 
	for(i=0;i<size;i++)							//ѭ����������
	{
		USART_SendData(USART3,USART3_TX_BUF[i]); 
	} 
}
//ͨ���жϽ�������2���ַ�֮���ʱ������20ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//USART_RX_BUF_middle[0]����������Ԫ�ص����������磺0x05 0x01 0x02 0x03 0x04 0x05
void USART3_IRQHandler(void)
{
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
/******************************************************************************************************************/	
			USART3_RX_FLAG=1;//�ñ�־λ�����߶�ʱ�����Կ�ʼΪ�ҹ����ˡ�ʱ�����ж�һ�������Ƿ����ꡣ
			USART3_RX_TIME=5;//����װ��ֵ����ʼ����ʱ�������ж��Ƿ��������һ�����ݡ�5�ν��붨ʱ���жϡ�
			USART3_RX_BUF_middle[USART3_RX_NO+1] = USART_ReceiveData(USART3);//��ȡ���յ�������			
			if(USART3_RX_NO<98)
			{
				USART3_RX_NO++;//ÿ�鴮�������յ������ݸ����ۼ�.���ó������鶨���������		
			}
			else
			{//�����������
				USART3_RX_NO=98;
			}
			USART3_RX_BUF_middle[0]=USART3_RX_NO;
/******************************************************************************************************************/			
    }
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
}
void USART3_RX_SCAN(void)//��ʱ����ʱɨ�贮�ڱ�־λ�ı仯����ʱ�Ľ��д���
{
	unsigned char i=0;
	if(USART3_RX_FLAG==1)//�ж��Ƿ����ڽ��մ�������
	{
		USART3_RX_TIME--;//������ʱ�Ĵ����ڵ���ֵ��һ��
		if(USART3_RX_TIME<=1)//˵��һ�������������ݵļ���������趨��ʱ���ˣ��ж����ݽ�����ϡ�
		{//�����յ��ֽ�֮���ʱ�䳬������3~5���ֽڵ�ʱ���ʱ������Ϊ���ν�����ɣ�������ɱ�־��1.
			USART3_RX_FLAG=0;//�����ڿ�ʼ��ʱ��־λ��������	
			USART3_RX_TIME=5;//��USART_RX_TIME��ֵ��������ֹ�ڼ�1��ʱ����ָ�ֵ��
			USART3_RX_NO=0;//���ڽ������ݳ�������			
/*******************ѹջ���ݰ�����*********************/	
			//������λ�õ�Ԫ��������Ϊ0������ǰ��һ��
			for(i=0;i<USART3_RX_BUF_middle[0]+1;i++)//ѹջ����
			{
				USART3_RX_BUF[USART3_GROUP][i]=USART3_RX_BUF_middle[i];
			}
			if(USART3_GROUP>=9)//Ⱥ��������ó������������ĳ��ȡ�ѭ����䡣0~19~0~19
			{
				USART3_GROUP=0;//Ⱥ������
			}
			else
			{//�����������
				USART3_GROUP++;//Ⱥ��ĸ�����1
			}
            if(USART3_RX_BUF[USART3_GROUP][0]!=0x00)//û���ü�����������Ѿ��γ�һ�����ˣ���Ҫ���ʼ��һ֡��������ˡ�
			{
				USART3_finger_point=USART3_GROUP;//��ָ��ָ���λ���� USART3_finger_point һ��һ���������ܴ�ʱfinger_point��λ�ã���
			}
		}
	}		
}
void USART3_COMAND_SCAN(void)//ˢ�¶�ȡ����3���ݡ��鿴�Ƿ������������Ҫ���� 
{
	unsigned int i=0;
	unsigned int lenth=0;//��������һ�����ݵĳ��ȡ�
	unsigned char USART3_RXD[101];//���巢������			
	if( ( (USART3_RX_BUF[USART3_finger_point][0]!=0x00)&&(USART3_RX_FLAG==0) )==1 )//������ִ��������������������ˣ����Ҵ��������á�
	{
		//ÿ�ζ��ǽ���һ���������ȡ��������ѭ�Ƚ��ȳ�ԭ��FIFO��
		for(i=0;i<USART3_RX_BUF[USART3_finger_point][0];i++)//USART3_RX_BUF[USART3_GROUP][0]��USART3_RX_BUF[j][i]�����ݵĳ��ȡ�ȡ����ʹ�á������7��������USART3_RX_BUF[USART1_GROUP][0]=7.
		{
			USART3_RXD[i]=USART3_RX_BUF[USART3_finger_point][i+1];//	������Ҫ���������ȡ������	
			USART3_RXD[i+1]=0x00;//��ӽ�����
		}
		lenth=USART3_RX_BUF[USART3_finger_point][0];//���������Ԫ�صĸ�����
		USART3_RX_BUF[USART3_finger_point][0]=0x00;//������������Ԫ�صĸ�����
		if(USART3_finger_point>=9)//Ⱥ��������ó������������ĳ��ȡ�ѭ����䡣0~9~0~9
		{
			USART3_finger_point=0;//Ⱥ���ָ��䵽ͷλ�á�
		}
		else
		{
			USART3_finger_point++;//Ⱥ���ָ���1��
		}
//USART3_SEND(USART3_RXD,lenth);
//			u3_printf("%s",USART3_RXD);//�����ã����ղ��յ������ݴ�ӡ������
		USART3_Data_analyze(USART3_RXD,lenth);//����1�������ݽ���.USART3_RXD���������飬lenth�����ݳ��ȡ�
	}
/************************************************************************************/				
}
void USART3_Data_analyze(unsigned char *USART3_RXD,unsigned char lenth)//����2�������ݽ���.USART3_RXD���������飬lenth�����ݳ��ȡ�
{
	unsigned int i=0;
	unsigned char sum_check=0;//У���
	unsigned char NODE_ID[20]={0x00};//�豸ID���롣
	unsigned char memory_index[50]={0x00};//��ȡ����������0~5�豸ID,6~7��·�ţ�8~9�����ţ�10~11�洢�Ľڵ�������
	unsigned char test[30]={0x00};					
//	������41 02 02 B2 17 11 11 01 00 00 00 AB 06 04 FB 00 00 00 00 14 FB 00 00 00 00 15 FB 00 00 00 00 16 FB 00 00 00 00 17 00 00 E7 24   
//			   0  1  2  3  4	5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41
			
//	������    00 00 E7 24 			
//	/*-------����---------*/			
//	41��0��	��ͷ
//	02��1��	��������
//	/*-------���ݳ�---------*/
//	02 B2 17 11 11 01 00 00 00 AB	��2~11���豸ID��433ͨ���豸���̸У�17��11��11�գ�01���Σ�00 00 00 01���
////0					����ͨ���豸���ͣ�433��LoRa�ȣ�
////1 				���������ͣ��̸У��¸еȣ�
////2��3��4		���������ա���17��11��12�գ���Ϊ 0x17 0x11 0x12
////5 				�����������Ρ�
////6��7��8��9	����������š�
//	06 ��12������״̬Ϊ��λ
//	04 ��13����λ����Ϊ4
//FB 00 00 00 00 14 ��14~19��
//FB 00 00 00 00 15 ��20~25��
//FB 00 00 00 00 16 ��26~31��
//FB 00 00 00 00 17	��32~37��					
//	00 00 ��38��39����������
//	/*----------------------*/
//	E7 ��40��У���  02 02 B2 17 11 11 01 00 00 00 AB 06 04 FB 00 00 00 00 14 FB 00 00 00 00 15 FB 00 00 00 00 16 FB 00 00 00 00 17 00 00   ���
//	24 ��41����β		
			if(   (USART3_RXD[0]==0x41)&&(USART3_RXD[1]==0x02)&&(USART3_RXD[lenth-1]==0x24)  )//�����ͷ,���ݷ���Ϊ���ػ��߷��������У���β��ȷ��
			{
				//����У��ͣ����Ƚ�У����Ƿ���ȷ�����ݷ���~�������ݡ�
				sum_check=sumcheck(USART3_RXD,1,lenth-3);//����У��͡��ڶ����ֽڵ��ڵ����ڶ����ֽ����
//				USART1_SEND(USART3_RXD,lenth);//������
//				printf("%x\r\n",sum_check);
				if(sum_check==USART3_RXD[lenth-2])//���У�����ȷ��
				{//��ȡ�����е�������վID��
//					USART1_SEND(USART3_RXD,lenth);//������
//					printf("��ַ��һ��");
					STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,5); //��ȡ����ע���豸����					
					for(i=0;i<6;i++)
					{
						NODE_ID[i]=USART3_RXD[2+i];
					}
//					USART1_SEND(NODE_ID,10);//������
					for(i=0;i<6;i++)//��ȡ������վ��ID
					{
						test[i]=memory_index[i];
					}
//					USART1_SEND(test,10);//������			
					for(i=0;i<6;i++)//�Ƚϴ����յ�����������ȡ��������վ��ID��������վ����洢��ID�����һ�£��򷵻�1.�����һ�£��򷵻�0.
					{
						if(NODE_ID[i]==test[i])
						{
							send_success_flag=1;//�Ƚ����ͳɹ���־λ��1.
						}
						else
						{
							send_success_flag=0;//���ID��һ�£�����0��
							i=11;//����forѭ������Լʱ�䡣							
						}
					}		
//					if(send_success_flag==1)
//					{
//					printf("��ַ��һ��");
//					}
//					else
//					{
//						printf("��ַ�Ų�һ��");
//					}
				}
				else
				{
					printf("У��ʹ���\r\n");
				}
			}			
}
unsigned char Wireless_Send_Data(unsigned char *data,unsigned char size)//data�Ƿ��͵����顣size�����ݳ��ȡ�	
{
//	unsigned char i;	
//	unsigned char test[20];	
	unsigned char Send_OK=0;//�������ʧ��,Ϊ0��������ͳɹ�������ֵΪ1������յ������ش�������ȷ��֡������ֵΪ2��
//��෢�����飬��������ɹ�������������LoRaģ�鷢��ģʽ��	
	USART3_SEND(data,size);//�����ط��ͱ�����Ϣ��
//	USART1_SEND(data,size);//�����ط��ͱ�����Ϣ��	
	Send_Overtime=250;//���ó�ʱʱ��Ϊ1�롣
	send_success_flag=0;//���ͳɹ���־λ������㡣
	while(Send_Overtime>0)
	{
		if( (Send_Overtime<=80)&&(Send_Overtime>=50)&&(send_success_flag==0) )
		{//����ǿ�ƽ����õġ���Ϊ�����е�ʱ����ʵ�Ѿ����ͳɹ��ˡ����Ƿ��ͳɹ���־λȴû�б���ǣ����Ե����޷��������ա�
		//���ڵİ취��ֱ�ӵȴ�600ms.��Ϊ433����һ֡���ݵ�ʱ��Ҳ��600ms
			Send_Overtime=0;//��ʱ������������ʧ�ܡ�
			Send_OK=0;//����ʧ��
		}
		USART3_COMAND_SCAN();//ˢ�¶�ȡ����3���ݡ��鿴�Ƿ������ݹ�����Ҫ�����鿴�Ƿ��յ���ȷ��֡��
		if(send_success_flag==1)//����յ����ػ��߷�������������ȷ��֡������Ϊ���ݷ��ͳɹ������ڵȴ���
		{
			send_success_flag=0;//���ͳɹ���־λ������㡣
			Send_Overtime=0;//���ͳɹ���׼������whileѭ����		
			Send_OK=1;//�Ѿ��յ����ص�ȷ��֡�ˡ�		
		}		
	}
	return Send_OK;		
}
void LoRa_433_Pack_DATA(unsigned char *data)//Package_DATA[0]��������Package_DATA��Ԫ�صĸ���+1.	�������ж��������ݵ��㷨����������Ԫ�صĸ�������6��
{
//������	FB 00 00 00 00 14    FB 00 00 00 00 15    FB 00 00 00 00 16    FB 00 00 00 00 17   �ĸ��豸��λ
//41 01 B2 23 96 60 40 01 06 04 FB 00 00 00 00 14 FB 00 00 00 00 15 FB 00 00 00 00 16 FB 00 00 00 00 17 00 00 59 24  ��38���ֽ�
// 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 			
//	/*-------����---------*/			
//	41��0��	��ͷ
//	01��1��	��������		
//	/*-------���ݳ�---------*/
	
//���磺��λ��վ�豸��17��12��22�գ������豸����1���Σ����Ϊ1	
//����ɶ�����	
//	B2 23 96 60 40 01	��2~7���豸ID��433ͨ���豸����λ��վ��17��11��11�գ�01���Σ�00 00 00 AB���
////�豸���	   		���	 �·�   ����   ����   ����        ���
////0000 0000| 0000 000|0 000|0 0000| 0000| 0000 00|00 0000 0000 0000 
////1011 0010| 0010 001|1 100|1 0110| 0110| 0000 01|00 0000 0000 0001 
////�����ʮ�����ƣ�B2 23 96 60 40 01	
//	06 ��8������״̬Ϊ��λ
//	04 ��9����������Ϊ4
//	FB 00 00 00 00 14 ��10~15��������ַ��
//	FB 00 00 00 00 15 ��16~21��������ַ�� 
//	FB 00 00 00 00 16 ��22~27��������ַ�� 
//	FB 00 00 00 00 17 ��28~33��������ַ��
//	00 00 ��34��35����������	
//	/*----------------------*/			
//	59 ��36��У���    01 B2 23 96 60 40 01 06 04 FB 00 00 00 00 14 FB 00 00 00 00 15 FB 00 00 00 00 16 FB 00 00 00 00 17 00 00 ���
//	24 ��37����β		
	unsigned char i=0;
	unsigned char step=0;
	unsigned char Wireless_SEND_DATA[100]={0x00};
	if(data[1]==0x01)//������
	{
		
		Wireless_SEND_DATA[0]=0x41;//���ݰ�ͷ
		Wireless_SEND_DATA[1]=0x01;//��������
		for(i=0;i<6;i++)//ID��6λ
		{
			Wireless_SEND_DATA[2+i]=Gateway_ID[i];//��������վ��IDѹ�뵽���顣
		}
	/***********����״̬***********/
		Wireless_SEND_DATA[8]=0x01;//����״̬Ϊ��������Ϊ��0x01��.
	/*************�Ѿ���λ�ı�ǩ�ĸ�����*************/
		Wireless_SEND_DATA[9]=0x01;//��������Ϊ��ѹ	 		
		Wireless_SEND_DATA[10]=0x01;//��������Ϊ1	
		Wireless_SEND_DATA[11]=0xFF;//����Ϊ��
		Wireless_SEND_DATA[12]=0xFF;//Ѳ������		
		Wireless_SEND_DATA[13]=0xFF;//
		Wireless_SEND_DATA[14]=0x00;//��������		
		Wireless_SEND_DATA[15]=0x00;//		
		Wireless_SEND_DATA[16]=sumcheck(Wireless_SEND_DATA,1,18-3);//����У��͡��ڶ����ֽڵ��ڵ����ڶ����ֽ����
		Wireless_SEND_DATA[17]=0x24;//��β
		step=18;
		/*----------------------*/
	}
	else if(data[1]==0x06)//��λ/�ָ��仯��
	{
//41 01 B2 23 96 60 40 01 06 08 02 FB 00 00 00 00 12 02 FB 00 00 00 00 13 02 FB 00 00 00 00 14 02 FB 00 00 00 00 15 02 FB 00 00 00 00 16 02 FB 00 00 00 00 17 02 FB 00 00 00 00 18 02 FB 00 00 00 00 19 01 01 FF FF FF 00 00 
//41 01 B2 23 96 60 40 01 06 08 02 FB 00 00 00 00 12 02 FB 00 00 00 00 13 02 FB 00 00 00 00 14 02 FB 00 00 00 00 15 02 FB 00 00 00 00 16 02 FB 00 00 00 00 17 02 FB 00 00 00 00 18 02 FB 00 00 00 00 19 01 01 FF FF FF 00 00 
//41 01 B2 23 96 60 40 01 06 08 02 FB 00 00 00 00 12 02 FB 00 00 00 00 13 02 FB 00 00 00 00 14 02 FB 00 00 00 00 15 02 FB 00 00 00 00 16 02 FB 00 00 00 00 17 02 FB 00 00 00 00 18 02 FB 00 00 00 00 19 01 01 FF FF FF 00 00 		
//		
//		
//41 01 B2 23 96 60 40 01 06 08 02 FB 00 00 00 00 12 02 FB 00 00 00 00 13 02 FB 00 00 00 00 14 02 FB 00 00 00 00 15 02 FB 00 00 00 00 16 02 FB 00 00 00 00 17 02 FB 00 00 00 00 18 02 FB 00 00 00 00 19 01 01 FF FF FF 00 00 
//41 01 B2 23 96 60 40 01 06 08 02 FB 00 00 00 00 12 02 FB 00 00 00 00 13 02 FB 00 00 00 00 14 02 FB 00 00 00 00 15 02 FB 00 00 00 00 16 02 FB 00 00 00 00 17 02 FB 00 00 00 00 18 02 FB 00 00 00 00 19 01 01 FF FF FF 00 00 
//41 01 B2 23 96 60 40 01 06 08 02 FB 00 00 00 00 12 02 FB 00 00 00 00 13 02 FB 00 00 00 00 14 02 FB 00 00 00 00 15 02 FB 00 00 00 00 16 02 FB 00 00 00 00 17 02 FB 00 00 00 00 18 02 FB 00 00 00 00 19 01 01 FF FF FF 00 00 		

//41 01 B2 23 96 60 40 01 06 08 02 FB 00 00 00 00 12 02 FB 00 00 00 00 13 02 FB 00 00 00 00 14 02 FB 00 00 00 00 15 02 FB 00 00 00 00 16 02 FB 00 00 00 00 17 02 FB 00 00 00 00 18 02 FB 00 00 00 00 19 01 01 FF FF FF 00 00 AE 24 
//41 01 B2 23 96 60 40 01 06 08 02 FB 00 00 00 00 12 02 FB 00 00 00 00 13 02 FB 00 00 00 00 14 02 FB 00 00 00 00 15 02 FB 00 00 00 00 16 02 FB 00 00 00 00 17 02 FB 00 00 00 00 18 02 FB 00 00 00 00 19 01 01 FF FF FF 00 00 AE 24 
//41 01 B2 23 96 60 40 01 06 08 02 FB 00 00 00 00 12 02 FB 00 00 00 00 13 02 FB 00 00 00 00 14 02 FB 00 00 00 00 15 02 FB 00 00 00 00 16 02 FB 00 00 00 00 17 02 FB 00 00 00 00 18 02 FB 00 00 00 00 19 01 01 FF FF FF 00 00 AE 24 		

		Wireless_SEND_DATA[0]=0x41;//���ݰ�ͷ
		Wireless_SEND_DATA[1]=0x01;//��������
		for(i=0;i<6;i++)//ID��6λ
		{
			Wireless_SEND_DATA[2+i]=Gateway_ID[i];//��������վ��IDѹ�뵽���顣
		}
	/***********����״̬***********/
		Wireless_SEND_DATA[8]=0x06;//����״̬Ϊ������������λ���߻ָ�������Ϊ��0x06��.
	/*************�Ѿ���λ�ı�ǩ�ĸ�����*************/
//		data[28]=(message[0]-3)/7;//��Ϣ������Ŀ		
		Wireless_SEND_DATA[9]=(data[0]-3)/7;//�������Ԫ�ظ�������ÿ��ID�ĳ���6.	
	/***********��λ��ǩ��ID***********/
		for(i=0;i<data[0]-3;i++)
		{		
			Wireless_SEND_DATA[10+i]=data[i+4];
		}
		step=10+data[0]-3;
	/***********��������λ***********/	 	
		Wireless_SEND_DATA[step]=0x01;//��������Ϊ��ѹ	
		step=step+1;		
		Wireless_SEND_DATA[step]=0x01;//��������Ϊ1
		step=step+1;
		Wireless_SEND_DATA[step]=0xFF;//����Ϊ��
		step=step+1;		
		Wireless_SEND_DATA[step]=0xFF;//Ѳ������	
		step=step+1;		
		Wireless_SEND_DATA[step]=0xFF;//
		step=step+1;		
		Wireless_SEND_DATA[step]=0x00;//��������		
		step=step+1;		
		Wireless_SEND_DATA[step]=0x00;//		
		step=step+1;		
		Wireless_SEND_DATA[step]=sumcheck(Wireless_SEND_DATA,1,step-1);//����У��͡��ڶ����ֽڵ��ڵ����ڶ����ֽ����
		step=step+1;			
		Wireless_SEND_DATA[step]=0x24;//��β
		step=step+1;			
	}
/*********************��ʼ�ط�**********************/	
//�ط���һ��
	i=Wireless_Send_Data(Wireless_SEND_DATA,step);//data�Ƿ��͵����顣size�����ݳ��ȡ�	
//�ط��ڶ���
	if(i==0)
	{
		i=Wireless_Send_Data(Wireless_SEND_DATA,step);//data�Ƿ��͵����顣size�����ݳ��ȡ�
	}
//�ط�������
	if(i==0)
	{
		i=Wireless_Send_Data(Wireless_SEND_DATA,step);//data�Ƿ��͵����顣size�����ݳ��ȡ�
	}
//	USART1_SEND(Wireless_SEND_DATA,14+data[0]);//������
}
