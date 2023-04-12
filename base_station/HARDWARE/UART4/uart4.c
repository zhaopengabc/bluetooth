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
#include "delay.h"
#include "function.h"
#include "timer.h"
#include "rtc.h"
unsigned char receive_once=0;//������λ��վһ���������������豸������
u16 mark_status=0xFFFF;//��λ��ǩ�ı�־λ����λ���㡣��Ϊ0��ʱ�򣬱�ʾ��λ������ mark_tatus ��ǰ��ֵΪ��0100 0011  ��˵��8��6��5��4��3�ű�ǩ��������λ��
u16 mark_status_last=0xFFFF;//�ϴ���λ��ǩ�ı�־λ��	
//////////////////////////////////////////////////////////////////////////////////
unsigned int departure_time; 			//�궨����λʱ�䡣
unsigned int bluetooh_scan_heart_time; //������վѲ��ʱ��
unsigned int judge_fruit_time;//��������е����ݡ��ж��Ƿ�����λ����ֹÿ�ζ��������Աȡ��˷ѵ�Ƭ����Դ
//���ڽ��ջ����� 	
/***************************���ڽ�϶�ʱ����������***************************/
unsigned char UART4_RX_FLAG=0;         //�ñ�־λ�����߶�ʱ����ʼ������,Ҳ˵���������������ݣ���Ҫ������������ϡ�
unsigned char UART4_RX_TIME=0;       	//�����ַ���ʱ��ʱ����
unsigned int 	UART4_RX_NO=0;          	//�����ַ����ַ���������
unsigned char UART4_GROUP=0;   				//�����ջ����������	
unsigned char UART4_RX_BUF[10][100]={0x00}; 	//���ջ����ά���顣
unsigned char UART4_RX_BUF_middle[100]={0x00}; 	//���ջ����ά���顣
unsigned char UART4_finger_point=0;    //������Ϣѭ����ָ��ָ���λ�á�
/***************************************************************************/

//��ʼ��IO ����3
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void UART4_Init(u32 bound)
{ 	
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	// GPIODʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE); //����4ʱ��ʹ��
 	USART_DeInit(UART4);  //��λ����4
		 //UART4_TX   PC.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ��PC.10
    //UART4_RX	  PC.11GPIO_Mode_IPU
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������   GPIO_Mode_IPU
  GPIO_Init(GPIOC, &GPIO_InitStructure);  //��ʼ��PC.11
	
	USART_InitStructure.USART_BaudRate = bound;//������һ������Ϊ115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  
	USART_Init(UART4, &USART_InitStructure); //��ʼ������	4  
	USART_Cmd(UART4, ENABLE);                    //ʹ�ܴ��� 	
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//�����ж�
	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
}
void test_URAT4(void)
{
	char send_data[128]="+++";
	char recv_data[128];
	int i = 0;
	int j = 0;
	for(i =0;i<3;i++)
	{
		USART_SendData(UART4,send_data[i]);
	}
	for(j =0;j<3;j++)
	{
		recv_data[j]=USART_ReceiveData(UART4);
	}
}

//����4,printf ����
//ȷ��һ�η������ݲ�����USART4_MAX_SEND_LEN�ֽ�
void u4_printf(char* fmt,...)  
{
	u16 i,j; 
	u8 UART4_TX_BUF[200]={0x00}; 			//���ͻ���,���UART4_MAX_SEND_LEN�ֽ�
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)UART4_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)UART4_TX_BUF);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
	  while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
		USART_SendData(UART4,UART4_TX_BUF[j]); 
	} 
}
//����4,�������
void UART4_SEND(unsigned char* UART4_TX_BUF,unsigned int size) // UART4_TX_BUF����Ҫ��������顣size�����������ĳ��ȡ� 
{  
	u16 i; 
	for(i=0;i<size;i++)							//ѭ����������
	{
		USART_SendData(UART4,UART4_TX_BUF[i]); 
	} 
}
//ͨ���жϽ�������2���ַ�֮���ʱ������20ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//USART_RX_BUF_middle[0]����������Ԫ�ص����������磺0x05 0x01 0x02 0x03 0x04 0x05
void UART4_IRQHandler(void)
{
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
/******************************************************************************************************************/	
			UART4_RX_FLAG=1;//�ñ�־λ�����߶�ʱ�����Կ�ʼΪ�ҹ����ˡ�ʱ�����ж�һ�������Ƿ����ꡣ
			UART4_RX_TIME=5;//����װ��ֵ����ʼ����ʱ�������ж��Ƿ��������һ�����ݡ�5�ν��붨ʱ���жϡ�
			UART4_RX_BUF_middle[UART4_RX_NO+1] = USART_ReceiveData(UART4);//��ȡ���յ�������				
			if(UART4_RX_NO<98)
			{
				UART4_RX_NO++;//ÿ�鴮�������յ������ݸ����ۼ�.���ó������鶨���������		
			}
			else
			{//�����������
				UART4_RX_NO=98;
			}
			UART4_RX_BUF_middle[0]=UART4_RX_NO;
/******************************************************************************************************************/			
    }
}
void UART4_RX_SCAN(void)//��ʱ����ʱɨ�贮�ڱ�־λ�ı仯����ʱ�Ľ��д���
{
	unsigned char i=0;
	if(UART4_RX_FLAG==1)//�ж��Ƿ����ڽ��մ�������
	{
		UART4_RX_TIME--;//������ʱ�Ĵ����ڵ���ֵ��һ��
		if(UART4_RX_TIME<=3)//˵��һ�������������ݵļ���������趨��ʱ���ˣ��ж����ݽ�����ϡ�
		{//�����յ��ֽ�֮���ʱ�䳬������3~5���ֽڵ�ʱ���ʱ������Ϊ���ν�����ɣ�������ɱ�־��1.
			UART4_RX_FLAG=0;//�����ڿ�ʼ��ʱ��־λ��������	
			UART4_RX_TIME=5;//��USART_RX_TIME��ֵ��������ֹ�ڼ�1��ʱ����ָ�ֵ��
			UART4_RX_NO=0;//���ڽ������ݳ�������			
/*******************ѹջ���ݰ�����*********************/	
			//������λ�õ�Ԫ��������Ϊ0������ǰ��һ��		
			for(i=0;i<UART4_RX_BUF_middle[0]+1;i++)//ѹջ����
			{
				UART4_RX_BUF[UART4_GROUP][i]=UART4_RX_BUF_middle[i];
			}
			if(UART4_GROUP>=9)//Ⱥ��������ó������������ĳ��ȡ�ѭ����䡣0~19~0~19
			{
				UART4_GROUP=0;//Ⱥ������
			}
			else
			{//�����������
				UART4_GROUP++;//Ⱥ��ĸ�����1
			}
			if(UART4_RX_BUF[UART4_GROUP][0]!=0x00)//û���ü�����������Ѿ��γ�һ�����ˣ���Ҫ���ʼ��һ֡��������ˡ�
			{
				UART4_finger_point=UART4_GROUP;//��ָ��ָ���λ���� UART4_finger_point һ��һ���������ܴ�ʱfinger_point��λ�ã���
			}	            
		}
	}		
}

void UART4_COMAND_SCAN(void)//ˢ�¶�ȡ����3���ݡ��鿴�Ƿ������������Ҫ���� 
{
	unsigned int i=0;
	unsigned int lenth=0;//��������һ�����ݵĳ��ȡ�
	unsigned char UART4_RXD[101]={0x00};//���巢������				
	if( ( (UART4_RX_BUF[UART4_finger_point][0]!=0x00)&&(UART4_RX_FLAG==0) )==1 )//������ִ��������������������ˣ����Ҵ����������������á�
	{
		//ÿ�ζ��ǽ���һ���������ȡ��������Ϊ�Ƚ��ȳ�ԭ��FIFO��
		for(i=0;i<UART4_RX_BUF[UART4_finger_point][0];i++)//UART4_RX_BUF[UART4_GROUP][0]��UART4_RX_BUF[j][i]�����ݵĳ��ȡ�ȡ����ʹ�á������7��������UART4_RX_BUF[USART1_GROUP][0]=7.
		{
			UART4_RXD[i]=UART4_RX_BUF[UART4_finger_point][i+1];//	������Ҫ���������ȡ������	
			UART4_RXD[i+1]=0x00;//��ӽ�����
		}
		lenth=UART4_RX_BUF[UART4_finger_point][0];//���������Ԫ�صĸ�����
		UART4_RX_BUF[UART4_finger_point][0]=0x00;//������������Ԫ�صĸ�����
		if(UART4_finger_point>=9)//Ⱥ��������ó������������ĳ��ȡ�ѭ����䡣0~9~0~9
		{
			UART4_finger_point=0;//Ⱥ���ָ��䵽ͷλ�á�
		}
		else
		{
			UART4_finger_point++;//Ⱥ���ָ���1��
		}//
//		printf("%s\r\n",UART4_RXD);
				USART1_SEND(UART4_RXD,lenth);
//				printf("\r\n");
	UART4_Data_analyze(UART4_RXD,lenth);//����4�������ݽ���.UART4_RXD���������飬lenth�����ݳ��ȡ�	
	}
/************************************************************************************/			
}
void UART4_Data_analyze(unsigned char *UART4_RXD,unsigned char lenth)//����4�������ݽ���.UART4_RXD���������飬lenth�����ݳ��ȡ�
{
	unsigned int i=0;
//	unsigned int lenth=0;//��������һ�����ݵĳ��ȡ�
//	unsigned char sum_check=0;//У���
	unsigned char NODE_ID_u8[20]={0x00};//��ǩ6λID	
	unsigned char NODE_ID_u8_middle[20]={0x00};//��ǩ6λID		
//	unsigned short  NODE_ID_u16[20]={0x00};//��ǩ6λID		
//	unsigned char middle_data[20]={0x00};//�м�����		
	unsigned char printf_data[20]={0x00};	
//��Ƭ������  AT+StartScan  �����������������豸����������ģ������յ��˵�Ƭ�����͵���������ȷ���  AT+ok			
//AT+ok\r\n
//41 54 2B 6F 6B 0D 0A
//�����������������ǩ������������ģ�鷢�͸���Ƭ����������ݡ�����  A4C1389653AA  ��������ǩ��MAC��ַ��FB0000000005  ���豸����
//AT+Scan=A4C1389653AA,FB0000000005\r\n
//A   T  +  S  c  a  n  =  A  4  C  1  3  8  9  6  5  3  A  A  ,  F  B  0  0  0  0  0  0  0  0  0  5 \r \n			
//41 54 2B 53 63 61 6E 3D 41 34 43 31 33 38 39 36 35 33 41 41 2C 46 42 30 30 30 30 30 30 30 30 30 35 0D 0A			
// 0 	1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34
	
//AT+ADV_TIMEOUT=300   �����õķ�������Ϊ300.
//AT+ADVINT=50  �����õķ������ݵ����ڡ������ģ���յ���ģ�鷢�͵�AT+StartScan�󣬻�ÿ50ms��ģ��ͷ���һ�α���ID�����AT+ADV_TIMEOUT=300����timeout/50=6�ط�������
//AT+ADV_CYCLE=10  ���ǻ���������10��	

//��ģ������յ�����ģ�鷢�͵�AT+StartScan������Ǵ�ģ�鷢�͵ı���ID���ܻ���Ϊ�������أ���ɷ��͵������޷�����ģ����յ����������Ӵ�ģ��ķ��ʹ�����
//	
//����һ�������������ģ���յ�16���豸��ʱ�򣬾�ֹͣ�����ˡ�
//����Ƶ��Ӧ�ø�ʱ���ϵ������Ϊ�ǰ������������豸����������ֹͣ���Ǽ����ġ�
//�ڳ���������һ�£������������豸���ڵ���15��ʱ�򣬾Ϳ�ʼ�µ�һ��������
	
	
			if(  (UART4_RXD[0]==0x41)&&(UART4_RXD[3]==0x53)&&(UART4_RXD[7]==0x3D)&&(UART4_RXD[20]==0x2C)&&(UART4_RXD[21]==0x46)&&(UART4_RXD[22]==0x42)&&(UART4_RXD[34]==0x0A)  )//�����ͷ��β��ȷ,�ض�λ�õ�,��ȷ��
			{
				receive_once=receive_once+1;//��������һ�������豸��
//				ID_lenth=lenth-23;//����Ϊ�ܵĳ��ȼ�ȥ0x0D,0x0A�ĳ���2����ȥ����֮ǰ�ĳ���21.
				//��ȡ������ǩ��ID��ÿ����ǩ��6λID����Ϊ��ʾԭ����12λ�ġ�
				for(i=0;i<12;i++)
				{
					NODE_ID_u8_middle[i]=UART4_RXD[i+21];
				}
				DATA_change(NODE_ID_u8_middle,12,NODE_ID_u8);
				DATA_change_u8_to_u16(NODE_ID_u8,12,judge_data_u16);//��6��8λ��Ԫ������ת����3��16λԪ�ص����顣				
				Judge_ID(judge_data_u16);//�жϺϷ��ԡ��Ƿ��Ѿ���EEPROM���д洢ע��,������������е�NODE_ID[6]��NODE_ID[7]���Ƿ�0x00��ֵ�������Ѿ�ע���豸���ڵ�λ�á��������NODE_ID[6]��NODE_ID[7]��Ϊ0x00������δע���豸��	
				if(judge_data_u16[4]==0x0000)//����ڵ�û�б�ע��
				{
					
				}
				else//�豸�Ѿ���ע�����0xFF��һ�������ܵ�ֵ���������������˵���豸û����λ�����������豸�����б�־λ��������һ�ֵ��������ڡ�
				{//judge_data_u16[3]  �Ƿ��صĵ�ǰ�ҵ���������ǩID�ĵ�ַ��
					if(judge_data_u16[3]<16)
					{
//										switch(NODE_ID_u16[3])
//											{
//												case 0://
//													mark_tatus=mark_tatus&0xFE;						
//												break;
//												case 1://
//													mark_tatus=mark_tatus&0xFD;			
//												break;
//												case 2://
//													mark_tatus=mark_tatus&0xFB;						
//												break;	
//												case 3://
//													mark_tatus=mark_tatus&0xF7;						
//												break;
//												case 4://
//													mark_tatus=mark_tatus&0xEF;						
//												break;	
//												case 5://
//													mark_tatus=mark_tatus&0xDF;						
//												break;
//												case 6://
//													mark_tatus=mark_tatus&0xBF;										
//												break;	
//												case 7://
//													mark_tatus=mark_tatus&0x7F;				
//												break;		
//												default:
//												break;
//											}	
						Bluetooh_time[judge_data_u16[3]]=departure_time;//���¸�ֵ�����ʱ�䳬ʱ�������¸�ֵ��������20��Ѳ��һ�Σ��������Ѳ�죬��240���ڶ����ֲ���������ǩ������Ϊ������ǩ��λ�ˡ�
						handle_arrays_u8(NODE_ID_u8,6,printf_data);//�����á���ӡ����ǰ��������������ǩID��
						printf("��������");	
						USART1_SEND(printf_data,12);		//�����á�
						printf("\r\n");							//�����á�		
					}
					else
					{
						printf("����Խ�磬��Ϊ��Ч���ݡ�\r\n");							//�����á�	
					}	
				}
			}
}

void Bluetooh_SCAN(void)//ˢ��Ѳ�������������ݡ� 
{
//	static unsigned char Package_DATA_last[100]={0};//��һ�ε����ݴ��	��

	static u16	mark_flag=0;
	static u16	mark_flag_last=0;	
	
	unsigned char Package_DATA[120]={0x00};	//��λ��ǩ���ݶ�ջ�á�
    unsigned char Package_DATA2[100]={0x00};	//��λ��ǩ���ݶ�ջ�á�
	unsigned char Bluetooh_ID[20]={0x00};	
//	unsigned char printf_data[100]={0x00};
	unsigned char data_send[20]={0x00};
	unsigned char i=0;
	unsigned char j=1;
	unsigned char k=0;	
	if( (Bluetooh_scan_time==0)||(receive_once>=15) )//��Ƭ��������оƬ���ڷ������ݵ���ʱ������Ѳ�����AT+StartScan
	{
		receive_once=0;//������������
		Bluetooh_scan_time=bluetooh_scan_heart_time;//��Ƭ������������ǩʱ����װ��ֵ��10������һ�Ρ�		
//		Buzzer_flag=3;
		data_send[0]=0x41;
		data_send[1]=0x54;
		data_send[2]=0x2B;
		data_send[3]=0x53;		
		data_send[4]=0x74;
		data_send[5]=0x61;	
		data_send[6]=0x72;
		data_send[7]=0x74;
		data_send[8]=0x53;
		data_send[9]=0x63;		
		data_send[10]=0x61;
		data_send[11]=0x6E;	
		data_send[12]=0x0D;
		data_send[13]=0x0A;	
		UART4_SEND(data_send,14);//������ģ�鷢���������  AT+StartScan\r\n	
		printf("/******************/");
		printf("\r\n");
	}
	if(Bluetooh_Package_compare==0)//���˼��������ǩ��λ״̬��ʱ���ˡ�n����һ�Σ���ʱ���ڽ����Լ���
	{
		Bluetooh_Package_compare=judge_fruit_time;//��װ��ֵ����ֹÿ�ζ��������Աȡ��˷ѵ�Ƭ����Դ
//		Package_DATA[0]=0;
		for(i=0;i<Bluetooh_amount;i++)//  Bluetooh_amount ��ָ������վ�������ж��ٸ�������λ��ǩ�������λ��ǩ����ʱ�Ƿ��ˡ��Ƚϴ�����
		{
			if(Bluetooh_time[i]==0)//ÿ��������ǩ����������ʱ�������ʱû�б����֣�����Ϊ������ǩ��λ�ˡ�Ȼ��ͨ��GM3����433����LoRa���ͳ�ȥ����վ���߷�������
			{
				switch(i)//����������λ���ͽ����λ�����㣡
				{		
					case 0://
						mark_status=mark_status&0xFFFE;						
					break;
					case 1://
						mark_status=mark_status&0xFFFD;			
					break;
					case 2://
						mark_status=mark_status&0xFFFB;						
					break;	
					case 3://
						mark_status=mark_status&0xFFF7;						
					break;
					case 4://
						mark_status=mark_status&0xFFEF;						
					break;	
					case 5://
						mark_status=mark_status&0xFFDF;						
					break;
					case 6://
						mark_status=mark_status&0xFFBF;										
					break;	
					case 7://
						mark_status=mark_status&0xFF7F;
					break;
                    case 8://
						mark_status=mark_status&0xFEFF;						
					break;
					case 9://
						mark_status=mark_status&0xFDFF;			
					break;
					case 10://
						mark_status=mark_status&0xFBFF;						
					break;	
					case 11://
						mark_status=mark_status&0xF7FF;						
					break;
					case 12://
						mark_status=mark_status&0xEFFF;						
					break;	
					case 13://
						mark_status=mark_status&0xDFFF;						
					break;
					case 14://
						mark_status=mark_status&0xBFFF;										
					break;	
					case 15://
						mark_status=mark_status&0x7FFF;
					break;
					default:
					break;
				}				
			}
			else
			{
				switch(i)//���������ָ����ͽ����λ����1��
				{
					case 0://
						mark_status=mark_status|0x0001;						
					break;
					case 1://
						mark_status=mark_status|0x0002;			
					break;
					case 2://
						mark_status=mark_status|0x0004;						
					break;	
					case 3://
						mark_status=mark_status|0x0008;						
					break;
					case 4://
						mark_status=mark_status|0x0010;						
					break;	
					case 5://
						mark_status=mark_status|0x0020;						
					break;
					case 6://
						mark_status=mark_status|0x0040;										
					break;	
					case 7://
						mark_status=mark_status|0x0080;				
					break;		
                    case 8://
						mark_status=mark_status|0x0100;						
					break;
					case 9://
						mark_status=mark_status|0x0200;			
					break;
					case 10://
						mark_status=mark_status|0x0400;						
					break;	
					case 11://
						mark_status=mark_status|0x0800;						
					break;
					case 12://
						mark_status=mark_status|0x1000;						
					break;	
					case 13://
						mark_status=mark_status|0x2000;
					break;
					case 14://
						mark_status=mark_status|0x4000;
					break;	
					case 15://
						mark_status=mark_status|0x8000;				
					break;		
					default:
					break;
				}					
			}
		}
		if(mark_status!=mark_status_last)//�Ƚ����ݡ�������������λ�����ݷ����仯������Ϊ��λ���߻ָ��ˡ�
		{
			for(i=0;i<Bluetooh_amount;i++)
			{
				switch(i)
				{		
					case 0://
						mark_flag=mark_status&0x0001;			
					break;
					case 1://
						mark_flag=mark_status&0x0002;				
					break;
					case 2://
						mark_flag=mark_status&0x0004;					
					break;	
					case 3://
						mark_flag=mark_status&0x0008;						
					break;
					case 4://
						mark_flag=mark_status&0x0010;		
					break;	
					case 5://
						mark_flag=mark_status&0x0020;							
					break;
					case 6://
						mark_flag=mark_status&0x0040;												
					break;	
					case 7://
						mark_flag=mark_status&0x0080;					
					break;
                    case 8://
						mark_flag=mark_status&0x0100;			
					break;
					case 9://
						mark_flag=mark_status&0x0200;				
					break;
					case 10://
						mark_flag=mark_status&0x0400;					
					break;	
					case 11://
						mark_flag=mark_status&0x0800;						
					break;
					case 12://
						mark_flag=mark_status&0x1000;		
					break;	
					case 13://
						mark_flag=mark_status&0x2000;							
					break;
					case 14://
						mark_flag=mark_status&0x4000;												
					break;	
					case 15://
						mark_flag=mark_status&0x8000;					
					break;	
					default:
					break;
				}
				switch(i)
				{
					case 0://
						mark_flag_last=mark_status_last&0x0001;			
					break;
					case 1://
						mark_flag_last=mark_status_last&0x0002;				
					break;
					case 2://
						mark_flag_last=mark_status_last&0x0004;					
					break;	
					case 3://
						mark_flag_last=mark_status_last&0x0008;						
					break;
					case 4://
						mark_flag_last=mark_status_last&0x0010;		
					break;	
					case 5://
						mark_flag_last=mark_status_last&0x0020;							
					break;
					case 6://
						mark_flag_last=mark_status_last&0x0040;												
					break;	
					case 7://
						mark_flag_last=mark_status_last&0x0080;					
					break;
                    case 8://
						mark_flag_last=mark_status_last&0x0100;			
					break;
					case 9://
						mark_flag_last=mark_status_last&0x0200;				
					break;
					case 10://
						mark_flag_last=mark_status_last&0x0400;					
					break;	
					case 11://
						mark_flag_last=mark_status_last&0x0800;						
					break;
					case 12://
						mark_flag_last=mark_status_last&0x1000;		
					break;	
					case 13://
						mark_flag_last=mark_status_last&0x2000;							
					break;
					case 14://
						mark_flag_last=mark_status_last&0x4000;												
					break;	
					case 15://
						mark_flag_last=mark_status_last&0x8000;					
					break;	
					default:
					break;
				}
				if(mark_flag==mark_flag_last)//״̬û�з����仯
				{
					
				}
				else if(mark_flag>mark_flag_last)//��������λ�ָ�
				{
					Package_DATA[j]=0x05;//��λ�ָ�
					j=j+1;
					STMFLASH_Read ( (DEVICE_ADDRESS+i*10),(u16*)Bluetooh_ID,5 );//�����ָ���������ǩ��ID��
					for(k=0;k<6;k++)//����λ��ǩ��6λIDѹ�뵽�Ƚ������С�
					{
						Package_DATA[j]=Bluetooh_ID[k];
						j=j+1;								
					}							
				}
				else if(mark_flag<mark_flag_last)//��������λ
				{
					Package_DATA[j]=0x02;//��λ����
					j=j+1;
					STMFLASH_Read ( (DEVICE_ADDRESS+i*10),(u16*)Bluetooh_ID,5 );//����ʧ����������ǩ��ID��
					for(k=0;k<6;k++)//����λ��ǩ��6λIDѹ�뵽�Ƚ������С�
					{
						Package_DATA[j]=Bluetooh_ID[k];
						j=j+1;								
					}						
				}
			}	
//			Package_DATA[0]=(j-1)/7;//������ж��������ݡ�
            if(j<=57)
            {
                Package_DATA[0]=j-1;//������������ж��ٸ�Ԫ�ء�			
	//			send_flag=0;
	//			USART3_SEND(Package_DATA_last,Package_DATA_last[0]);//������
//			USART1_SEND(Package_DATA,Package_DATA[0]+1);//������
			package_send_data(Package_DATA);//Package_DATA[0]��������Package_DATA��Ԫ�صĸ���+1.		
            }
            else
            {
                Package_DATA[0]=56;//������������ж��ٸ�Ԫ�ء�			
                Package_DATA2[0]=j-57;//������������ж��ٸ�Ԫ�ء�                
                memcpy(Package_DATA2+1,Package_DATA+57,j-57);
                memset(Package_DATA+57,0,sizeof(Package_DATA)-57);
                package_send_data(Package_DATA);//Package_DATA[0]��������Package_DATA��Ԫ�صĸ���+1.		
                package_send_data(Package_DATA2);//Package_DATA[0]��������Package_DATA��Ԫ�صĸ���+1.
            }
			
			mark_status_last=mark_status;
// 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 
//1C 05 FB 00 00 00 02 15 02 FB 00 00 00 01 11 02 FB 00 00 00 00 45 05 FB 00 00 00 00 30 	
//��λ��			
//FB 00 00 00 02 15			
//FB 00 00 00 00 30			
//�ָ���
//FB 00 00 00 01 11 
//FB 00 00 00 00 45		
		}		
	}
/***********************************************************************************************************/		
//����λ��ǩ��ID���д���������Ϻ��ٸ��ϴε����ݽ��бȶԡ������һ��˵�����µ���λ��ǩ��λ�˻��߻ָ��ˣ�����Ҫ�����Ϸ���	
}

void get_all_status(void)
{
    u32 i=0,j=1,k=0;
    u16 mark_flag;
    u8 Package_DATA[120]={0x00};
    u8 Bluetooh_ID[20]={0x00};
    unsigned char Package_DATA2[100]={0x00};
    for(i=0;i<Bluetooh_amount;i++)
    {
        mark_flag=mark_status&(0x0001<<i);
        if(mark_flag)//��������λ�ָ�
        {
            Package_DATA[j]=0x05;//��λ�ָ�
            j=j+1;
            STMFLASH_Read ( (DEVICE_ADDRESS+i*10),(u16*)Bluetooh_ID,5 );//�����ָ���������ǩ��ID��
            for(k=0;k<6;k++)//����λ��ǩ��6λIDѹ�뵽�Ƚ������С�
            {
                Package_DATA[j]=Bluetooh_ID[k];
                j=j+1;								
            }							
        }
        else//��������λ
        {
            Package_DATA[j]=0x02;//��λ����
            j=j+1;
            STMFLASH_Read ( (DEVICE_ADDRESS+i*10),(u16*)Bluetooh_ID,5 );//����ʧ����������ǩ��ID��
            for(k=0;k<6;k++)//����λ��ǩ��6λIDѹ�뵽�Ƚ������С�
            {
                Package_DATA[j]=Bluetooh_ID[k];
                j=j+1;								
            }						
        }
    }
    if(j<=57)
    {
        Package_DATA[0]=j-1;//������������ж��ٸ�Ԫ�ء�			
        package_send_data(Package_DATA);//Package_DATA[0]��������Package_DATA��Ԫ�صĸ���+1.		
    }
    else
    {
        Package_DATA[0]=56;//������������ж��ٸ�Ԫ�ء�			
        Package_DATA2[0]=j-57;//������������ж��ٸ�Ԫ�ء�                
        memcpy(Package_DATA2+1,Package_DATA+57,j-57);
        memset(Package_DATA+57,0,sizeof(Package_DATA)-57);
        package_send_data(Package_DATA);//Package_DATA[0]��������Package_DATA��Ԫ�صĸ���+1.		
        package_send_data(Package_DATA2);//Package_DATA[0]��������Package_DATA��Ԫ�صĸ���+1.
    }
}

void dislocation_test(void)//�̰����󣬷���һ֡��λ�������������еĽڵ���Ϣ��
{
//����
//41 01 02 B2 17 11 11 01 00 00 00 AB 06 04 FB 00 00 00 00 14 FB 00 00 00 00 15 FB 00 00 00 00 16 FB 00 00 00 00 17 00 00 E6 24
// 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 	
	unsigned char i=0;
	unsigned char j=1;	
	unsigned char k=0;	
	unsigned char Bluetooh_ID[20]={0x00};			
	unsigned char Package_DATA[120]={0x00};	//��λ��ǩ���ݶ�ջ�á�
	printf("���Է�������\r\n");
	for(i=0;i<Bluetooh_amount;i++)//  Bluetooh_amount ��ָ������վ�������ж��ٸ�������λ��ǩ�������λ��ǩ����ʱ�Ƿ��ˡ��Ƚϴ�����
	{
		Bluetooh_time[i]=0;//��������λ��ǩ�ļ���������գ�����һ�ֶ���λ�ļ��󡣷�����Ե�ʱ��Ա�ǩ״̬���в��ԡ�������ȫ����λ�󣬻�½½�����ķ��ͻظ�������
		mark_status=0x0000;//��λ��ǩ�ı�־λ����λ���㡣��Ϊ0��ʱ�򣬱�ʾ��λ������ mark_tatus ��ǰ��ֵΪ��0100 0011  ��˵��8��6��5��4��3�ű�ǩ��������λ��
		mark_status_last=0xFFFF;//�ϴ���λ��ǩ�ı�־λ
	}
}

