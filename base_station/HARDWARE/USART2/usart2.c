#include "usart.h"
#include "usart2.h"
#include "usart3.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	   
#include "buzzer.h" 
#include "stmflash.h"
#include "timer.h"
#include "function.h"
#include "gprs.h"//���GPRS�������Ŷ���
#include "led.h"
#include "rtc.h"
#include "bc95.h"
#include "os.h"
#include "memory.h"
#include "uart4.h"
//#include "led.h"
//unsigned int send_lenth;//������Ԫ�صĸ�����

unsigned char GM3_LINKA=0;
extern AT_link_ENUM NB_Link_step;
extern u32 NB_LINKSTATUS;
u32 datastatus=0;

unsigned char data[500]={0x00};	
unsigned char send_message[100]={0x00};//�����������顣
//�����������ڴ�С
unsigned int heartbeat_time=0;//����ʱ�䶨��1��������һ�Ρ�
unsigned int timeout=0;//��ʱ���ж���5ms����һ�Ρ�����2000�Σ���Ϊ10���ӡ�������������һ֡���ݵĳ�ʱʱ�䡣����������ʱ�䣬����Ϊ����ʱ��̫����

unsigned int SEND_DATA_to_server_time=0;//���������������ݵ���ʱ������������ʱ��û�з��ͳɹ��������·��͡�
unsigned char SEND_DATA_to_server_ok=0;//���ͳɹ�Ϊ0������ʧ��Ϊ1.
unsigned char finger_point=0;//������Ϣѭ����ָ��ָ���λ�á�
unsigned char SEND_DATA_to_server_GROUP=0;//������Ҫ��������������������ŵ�����ʲôλ�á�
unsigned char send_data[20][80]={0x00};//����20�����ݣ������ط�ʱ��ջ�á������������λ��վ�ϴ����8����ǩ����һ������ֽ�Ϊ400�ֽڡ�����Ԥ������410���ֽڡ�
unsigned char SEND_once=0;//�ж��ط�������
unsigned short 	last_serial_number=0;//��һ�ε�ҵ����ˮ��
unsigned short  serial_number_counter=0;//ҵ����ˮ��

/***************************���ڽ�϶�ʱ����������***************************/
unsigned char USART2_RX_FLAG=0;         //�ñ�־λ�����߶�ʱ����ʼ������,Ҳ˵���������������ݣ���Ҫ������������ϡ�
unsigned char USART2_RX_TIME=0;       	//�����ַ���ʱ��ʱ����
unsigned int 	USART2_RX_NO=0;          	//�����ַ����ַ���������
unsigned char USART2_GROUP=0;   				//�����ջ����������	
unsigned char USART2_RX_BUF[10][100]={0x00}; 	//���ջ����ά���顣
unsigned char USART2_RX_BUF_middle[100]={0x00}; 	//���ջ����ά���顣
unsigned char USART2_finger_point=0;    //������Ϣѭ����ָ��ָ���λ�á�
/***************************************************************************/
//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void USART2_Init(u32 bound)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);// GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

 	USART_DeInit(USART2);                                //��λ����1
	 //USART2_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;            //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	     //�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);               //��ʼ��PA2
 
	//USART2_RX	  PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;            //PA.3   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������  GPIO_Mode_IN_FLOATING
	GPIO_Init(GPIOA, &GPIO_InitStructure);               //��ʼ��PA3
	
	USART_InitStructure.USART_BaudRate = bound;                                    //һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;                            //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	               //�շ�ģʽ
  
	USART_Init(USART2, &USART_InitStructure); //��ʼ������	2 
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�	
	
	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���		
}   
//����2,printf ����
//ȷ��һ�η������ݲ�����USART2_MAX_SEND_LEN�ֽ�
void u2_printf(char* fmt,...)  
{  
	u16 i,j; 
	u8 USART2_TX_BUF[200]={0x00}; 			//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_TX_BUF);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
		USART_SendData(USART2,USART2_TX_BUF[j]); 
	} 
}

//����2,�������
void USART2_SEND(unsigned char* USART2_TX_BUF,unsigned int size) // USART2_TX_BUF����Ҫ��������顣size�����������ĳ��ȡ� 
{  
	u16 i; 
	for(i=0;i<size;i++)							//ѭ����������
	{
//	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
		USART_SendData(USART2,USART2_TX_BUF[i]); 
	} 
}
//ͨ���жϽ�������2���ַ�֮���ʱ������20ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//USART_RX_BUF_middle[0]����������Ԫ�ص����������磺0x05 0x01 0x02 0x03 0x04 0x05	 
void USART2_IRQHandler(void)
{
    u8 Res;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
/******************************************************************************************************************/	
			//USART2_RX_FLAG=1;//�ñ�־λ�����߶�ʱ�����Կ�ʼΪ�ҹ����ˡ�ʱ�����ж�һ�������Ƿ����ꡣ
			USART2_RX_TIME=5;//����װ��ֵ����ʼ����ʱ�������ж��Ƿ��������һ�����ݡ�5�ν��붨ʱ���жϡ�
            Res =	USART_ReceiveData(USART2);
			UartRxISR(&uart_gprs, Res);
			/*USART2_RX_BUF_middle[USART2_RX_NO+1] = USART_ReceiveData(USART2);//��ȡ���յ�������
            UartRxISR(&uart_gprs, USART2_RX_BUF_middle[USART2_RX_NO+1]);            
			if(USART2_RX_NO<98)
			{
				USART2_RX_NO++;//ÿ�鴮�������յ������ݸ����ۼ�.���ó������鶨���������		
			}
			else
			{//�����������
				USART2_RX_NO=98;
			}
			USART2_RX_BUF_middle[0]=USART2_RX_NO;*/
/******************************************************************************************************************/			
    }
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
}
void USART2_RX_SCAN(void)//��ʱ����ʱɨ�贮�ڱ�־λ�ı仯����ʱ�Ľ��д���
{
	unsigned char i=0;
	if(USART2_RX_FLAG==1)//�ж��Ƿ����ڽ��մ�������
	{
//		USART2_RX_TIME--;//������ʱ�Ĵ����ڵ���ֵ��һ��
//		if(USART2_RX_TIME<=1)//˵��һ�������������ݵļ���������趨��ʱ���ˣ��ж����ݽ�����ϡ�
//		{//�����յ��ֽ�֮���ʱ�䳬������3~5���ֽڵ�ʱ���ʱ������Ϊ���ν�����ɣ�������ɱ�־��1.
			USART2_RX_FLAG=0;//�����ڿ�ʼ��ʱ��־λ��������	
			USART2_RX_TIME=5;//��USART_RX_TIME��ֵ��������ֹ�ڼ�1��ʱ����ָ�ֵ��
			//USART2_RX_NO=0;//���ڽ������ݳ�������			
/*******************ѹջ���ݰ�����*********************/	
			//������λ�õ�Ԫ��������Ϊ0������ǰ��һ��
			for(i=0;i<USART2_RX_BUF_middle[0]+1;i++)//ѹջ����
			{
				USART2_RX_BUF[USART2_GROUP][i]=USART2_RX_BUF_middle[i];
			}
			if(USART2_GROUP>=9)//Ⱥ��������ó������������ĳ��ȡ�ѭ����䡣0~19~0~19
			{
				USART2_GROUP=0;//Ⱥ������
			}
			else
			{//�����������
				USART2_GROUP++;//Ⱥ��ĸ�����1
			}
            if(USART2_RX_BUF[USART2_GROUP][0]>0)//û���ü�����������Ѿ��γ�һ�����ˣ���Ҫ���ʼ��һ֡��������ˡ�
			{
				USART2_finger_point=USART2_GROUP;//��ָ��ָ���λ���� USART2_finger_point һ��һ���������ܴ�ʱfinger_point��λ�ã���
			}
//		}
	}		
}
void USART2_COMAND_SCAN(void)//ˢ�¶�ȡ����1���ݡ��鿴�Ƿ������������Ҫ���� 
{
	unsigned int i=0;
	unsigned int lenth=0;//��������һ�����ݵĳ��ȡ�
	unsigned char USART2_RXD[101]={0x00};//���巢������			
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
//USART2_SEND(USART2_RXD,lenth);
//			u2_printf("%s",USART2_RXD);//�����ã����ղ��յ������ݴ�ӡ������
/************************************************************************************/		
		USART2_Data_analyze(USART2_RXD,lenth);//����2�������ݽ���.USART2_RXD���������飬lenth�����ݳ��ȡ�		
//	USART1_SEND(USART2_RXD,lenth);//����		
	}	
}
extern unsigned char memory_index[200];
void USART2_Data_analyze(unsigned char *USART2_RXD,unsigned char lenth)//����2�������ݽ���.USART2_RXD���������飬lenth�����ݳ��ȡ�
{
	unsigned int i=0,j=0;	
	unsigned char sum_check=0;//У��͡�	
	unsigned short 	liushui_NO=0;//����ҵ����ˮ���á�
	unsigned char NODE_ID_u8[20]={0x00};//�豸ID���롣
	unsigned int  NODE_ID_u16[20]={0x00};
	unsigned char printf_data[50]={0x00};//���Դ�ӡ�õ�����
    u32 time_save;
//�Է������·������ݽ������ݷ���������յ���ȷ��֡����SEND_DATA_to_server_ok��1.
	if(  (USART2_RXD[0]==0x40)&&(USART2_RXD[1]==0x40)&&(USART2_RXD[lenth-2]==0x23)&&(USART2_RXD[lenth-1]==0x23) )//�����ͷ��β��ȷ��
	{//�жϷ������·��������Ƿ�Ϸ���ȷ��
		//����У��ͣ����Ƚ�У����Ƿ���ȷ��������~���ء�
		sum_check=sumcheck(USART2_RXD,2,lenth-4);//����У��͡��ڶ����ֽڵ��ڵ����ڶ����ֽ����
		if(sum_check==USART2_RXD[lenth-3])//������������ص����ݰ���У��ͺ�ҵ����ˮ�Ŷ���ȷ��
		{
			RTC_Get();//�鿴�Ƿ���Ҫͬ��ʱ�䡣			
			if( (USART2_RXD[7]!=calendar.min)||(USART2_RXD[8]!=calendar.hour)||(USART2_RXD[9]!=calendar.w_date)||(USART2_RXD[10]!=calendar.w_month)||(USART2_RXD[11]!=calendar.w_year) )
			{//ֻ�鿴������ʱ�ּ��ɣ������һ�¾�У׼���ݲ���������ȽϷ�Χ��
				RTC_Set(USART2_RXD[11]+2000, USART2_RXD[10], USART2_RXD[9], USART2_RXD[8], USART2_RXD[7], USART2_RXD[6]);//����RTCʱ��Ϊ2011��11��11��11ʱ11��11��	
//				RTC_Set(2011, 11, 11, 11, 11, 11);//����RTCʱ��Ϊ2011��11��11��11ʱ11��11��	
			}
			switch(USART2_RXD[26])//�ж�������������
			{   
			 case 0x01://ʱ��ͬ��					 
		//���ظ��±���ʱ�䡣
					break;	 
			 case 0x02://��������
															
					break;	
			 case 0x03://ȷ��		
		//��ȡҵ����ˮ��			
					liushui_NO=USART2_RXD[3];//��ˮ�ŵĸ�λ
					liushui_NO=liushui_NO<<8;
					liushui_NO=liushui_NO+USART2_RXD[2];//��ˮ�ŵĵ�λ		 
					if(liushui_NO>=last_serial_number)//����ǻظ��ĸղ����������Ϸ��Ļظ�֡��
					{
						printf("�յ�������Ӧ��ҵ����ˮ�ţ�%d\r\n",liushui_NO);				
						Buzzer_flag=1;
						SEND_DATA_to_server_time=0;
						SEND_DATA_to_server_ok=0;
						SEND_once=0;//���ʹ������㡣	
		//				send_lenth=0;			
					}
					break;	 
			 case 0x04://����
                 if(USART2_RXD[27]==62)
                 {
                     printf("�յ���������ѯָ�ҵ����ˮ�ţ�%d\r\n",liushui_NO);	
                     printf("������������������б�ǩ��λ��λ״̬\r\n");	
                     get_all_status();
                     Buzzer_flag=1;
                 }
					break;	
			 case 0x05://Ӧ��
		//��ȡҵ����ˮ��
					liushui_NO=USART2_RXD[3];//��ˮ�ŵĸ�λ
					liushui_NO=liushui_NO<<8;
					liushui_NO=liushui_NO+USART2_RXD[2];//��ˮ�ŵĵ�λ
					if(liushui_NO>=last_serial_number)//����ǻظ��ĸղ����������Ϸ��Ļظ�֡��
					{
						SEND_DATA_to_server_time=0;
						SEND_DATA_to_server_ok=0;
						SEND_once=0;//���ʹ������㡣	
		//				send_lenth=0;
					}
					break;
			 case 0x06://����

					break;
			 case 0x80://����
		//		����Ӧ�����ݵ�Ԫ�е�����
					if(USART2_RXD[27]==0x93)//�������ӽڵ�
					{
						if( (USART2_RXD[28]==0)&&(USART2_RXD[29]==0)&&(USART2_RXD[30]==0)&&(USART2_RXD[31]==0)&&(USART2_RXD[32]==0)&&(USART2_RXD[33]==0) )
						{//�������Ŀ��ID����0.����Ϊ����������ID��
							//��ȡ�豸ID��
							for(i=0;i<6;i++)
							{
								NODE_ID_u8[i]=USART2_RXD[35+i];//��ȡҪ���õ����ص�ID��
							}
		//�޸�����ID
							i=Save_ROUTER_ID(NODE_ID_u8);//��������ID.�������ɹ�������1���������ʧ�ܣ��򷵻�0��
							if(i==1)//�������IDд��ɹ���
							{
								handle_arrays_u8(NODE_ID_u8,8,printf_data);//���罫0x25�ֿ�����0x32��0x35.��0xAB�ֿ�����0x40,0x41. 
								printf("����IDд��ɹ�������ID�� ");			
								USART1_SEND(printf_data,12);//��ӡ����ID
								printf("\r\n");	
//								Buzzer_flag=2;							
							}
							else
							{
								printf("����IDд��ʧ�ܣ�������д�롣\r\n");			
//								Buzzer_flag=2;										
							}	
						}
						else//������ؽڵ�ID
						{
							if(USART2_RXD[34]==1)//���һ���ڵ��ID
							{
								for(i=0;i<6;i++)
								{
									NODE_ID_u8[i]=USART2_RXD[35+i];//��ȡҪ���õ����ؽڵ��ID��
								}	
								i=Save_NODE_ID(NODE_ID_u8);//NODE_ID_u16�ĳ���Ϊ6λ�������ʱ�򱣴�10λ��������λ���á�
								if(i==0xFFFF)//�����豸IDʧ��
								{
									printf("�����豸IDʧ��");
								}
								else//�����豸ID�ɹ���
								{//��ӡ�豸ע��ɹ�
//									Buzzer_flag=4;//����������һ�¡�
									printf("��������ע���豸ID����Ϊ��%d\r\n",i);
									Printf_device_type(NODE_ID_u8[0]);//���Դ�ӡ�豸���͡����ݹ��ꡣ						
									printf("ע��ɹ����豸ID��");
									handle_arrays_u8(NODE_ID_u8,6,printf_data);//ID��ӡ��������0x3x ��ͷ�ġ�������Ҫ��ӡ20λ��							
									USART1_SEND(printf_data,12);	
									printf("\r\n");									
								}								
							}
							else//�����Ӷ���ڵ�ID
							{
								
							}
						}
					}
					else if(USART2_RXD[27]==0x94)//�����ɾ���ڵ�
					{
						if(USART2_RXD[34]==1)//ɾ��һ���ڵ��ID
						{
							for(i=0;i<6;i++)//��ȡ��Ҫɾ�����豸ID��
							{
								NODE_ID_u8[i]=USART2_RXD[35+i];//��ȡҪɾ�������ؽڵ��ID��
							}
		//					DATA_change_u8_to_u16(NODE_ID_u8,6,NODE_ID_u16);//������8λ����ת����һ��16λ���ݡ����罫��0x25,0x5F���0x255F
							if(NODE_ID_u16[4]==0x0000)//����ڵ�û�б�ע��
							{
								printf("�������޴�ID������Ҫɾ����\r\n");					
							}
							else//�豸�Ѿ���ע�ᡣ��鿴��Ѳ����Ϣ���Ǳ�����Ϣ��
							{
								printf("�������޴�ID������Ҫɾ����\r\n");		
		///************ɾID��************/							
								Delete_NODE_ID(NODE_ID_u8);//ɾ��ID��
								handle_arrays_u8(NODE_ID_u8,6,printf_data);//�����á�
								printf("�������豸ID:");	
								USART1_SEND(printf_data,12);
								printf(" ɾ���ɹ���\r\n");	
		///*****************************/								
							}
						}
						else//ɾ������ڵ��ID
						{
							
						}		
					}
                    else if(USART2_RXD[27]==0x95)//��������ò���
                    {
                        if(USART2_RXD[36]==0x89)
                        {
                            time_save=USART2_RXD[39]*256+USART2_RXD[38];
                            time_save/=10;
                            STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,100); // ��ȡ��ע���豸����
                            memory_index[50]=time_save&0x000000FF;
                            memory_index[51]=(time_save&0x0000FF00)>>8;
                            memory_index[52]=(time_save&0x00FF0000)>>16;
                            memory_index[53]=(time_save&0xFF000000)>>24;
                            STMFLASH_Write(NUMBER_ADDRESS,(u16*)memory_index,100);//���䶯���������������д��eeprom	
                            printf("�ж�������λ��ǩ��λʱ������ %d ����\r\n",time_save);
                        }
                    }
					break;
			 default:
					break;
			}
		}
	}	
}
void Judge_GM3_send_data(void)//�ж�/������վ�Ƿ���Ҫͨ��GM3�ϴ������������ݡ�	
{
	unsigned int i=0;
	unsigned char test[10]={0x00};
/*************�ڴ˴����GPRS״̬��***************/	
	/*if(TIM2_IRQ_flag==1)//GPRSδ����,�ҵ��˼��ʱ���ˡ�	
	{
		TIM2_IRQ_flag=0;//��ձ�־λ��
		if(GM3_LINKA==1)//���ģ��û���������硣
		{
			if(GM3_LINKA_TIME>1)
			{
				GM3_LINKA_TIME=GM3_LINKA_TIME-1;
			}
			else
			{
				printf("GPRSģ�鳤ʱ��δ���ӵ����磬������\r\n");
				GPRS_RESTAR();//GPRS������
			}			
		}
		else//���GPRS���ߣ���ʱ���1.
		{
			LED_flag=2;//	
			if(GM3_LINKA_TIME<GM3_LINKA_TIME_LOSE)
			{
				GM3_LINKA_TIME=GM3_LINKA_TIME+1;
			}		
		}		
	}*/

	if(Bluetooh_heartbeat_time==0)//������˷���������ʱ�䡣����������
	{
		Bluetooh_heartbeat_time=heartbeat_time;//��װ������վ������ʱ�䡣
		get_all_status();
	}
	if(NB_LINKSTATUS)//���ģ�����ӵ������硣
	{
		if(send_time==0)//2�����һ�Σ���ֹƵ�����롣
		{
			send_time=2;//��װ��ֵ
			if( (send_data[finger_point][0]!=0x00)&&(SEND_DATA_to_server_ok==0) )//��ѭ�Ƚ��ȳ�ԭ��FIFO����Ϣ���������������ݣ�����һ�������Ѿ����ͳɹ���			
			{//����������ݵ�Ԫ��������Ϊ0.˵�������������ݡ�����ʱ���Ѿ����㣨û�����鷢��ʧ�ܣ������ݷ����Ѿ��ɹ������ʹ����Ѿ�Ϊ0˵��Ŀǰ���п��С�
	//ȡ������ĵĳ��ȡ�
	//			send_lenth=lenth;
				for(i=0;i<send_data[finger_point][0]+1;i++)//ȡ�����ݡ�
				{
					send_message[i]=send_data[finger_point][i];//������Ҫ���������ȡ������
				}
				send_data[finger_point][0]=0x00;//������λ�õ����ݳ��ȡ��ں������ζ�ջ��ʱ��ͱ�����֡�����Ѿ��������ˡ����Լ�������µ������ˡ�			
				if(finger_point>=19)//���ڴ��������ָ�벻�ó��������������ܸ�����ѭ�����ν��д���0~19~0~19
				{
					finger_point=0;//Ⱥ������
				}
				else
				{//�����������
					finger_point++;//����ָ���λ�ü�1
				}
	//			USART1_SEND(send_message,send_message[0]+1);
				Router_send_data_to_server(send_message);//���ؽ����ݷ��͸���������
			}
		}
		if( (SEND_DATA_to_server_ok==1)&&(SEND_DATA_to_server_time==0) )//�����������������������,�ҷ���ʱ�䳬ʱ�ˡ�
		{//�Է��ͽ�������ж���
			SEND_once=SEND_once+1;//�ط�������1.�ط��������ó�������.	
			if(SEND_once>=2)//���ʹ���������3.��Ϊ�ط�������������һ�����ݵķ��͡�
			{
				SEND_once=0;//���ʹ������㡣
				SEND_DATA_to_server_ok=0;//ǿ�Ʊ�Ƿ��ͳɹ���־Ϊ0.��Ȼ����Զ�޷�������һ����Ϣ�ķ��͡�
				printf("GPRSģ�鳤ʱ��δ�ܷ��ͳɹ����ݣ�������\r\n");
				GPRS_RESTAR();//GPRS������
                datastatus=1;
			}
			else
			{
				Router_send_data_to_server(send_message);//���ؽ����ݷ��͸���������	
			}
		}	
	}
}
void Router_send_data_to_server(unsigned char *message)//���ط������ݸ�������
{
	unsigned char leave_no=0;//��λ��ǩ����
	unsigned int i=0;
	unsigned int j=0;	
	unsigned int t=0;		
	unsigned char printf_data[40]={0x00};
	unsigned int step=0;//����������Ԫ�صĸ�����
	unsigned int step_middle=0;	
	unsigned short 	last_serial_number_middle=0;//��һ�ε�ҵ����ˮ��	
	Bluetooh_heartbeat_time=heartbeat_time;//��װ������վ������ʱ�䡣
	SEND_DATA_to_server_ok=1;//����ʧ�ܱ�־λ��1�����ͳɹ������㡣
	SEND_DATA_to_server_time=timeout;//��ʱ���ж���5ms����һ�Ρ�����2000�Σ���Ϊ10���ӡ�
	last_serial_number_middle=message[3];//��ǰ�������ݵ���ˮ�š�0x00 0x00 ~ 0xFF 0xFF.��������λ��ǰ	
	last_serial_number_middle=last_serial_number_middle<<8;
	last_serial_number=last_serial_number_middle+message[2];
/*************ͨ�����ڷ������ݸ�������***************/
//ͨ��������������������
//40 40 01 00 02 00 00 00 00 00 00 00 01 02 03 04 05 06 00 00 00 00 00 00 03 00 02 15 01 01 34 23 23 	�����ݣ� 		
// 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32	����ţ�
//������
//40 40 ��0~1����ͷ
//01 00 ��2~3��ҵ����ˮ�š�����λ��ǰ��
//00 02 ��4~5��Э��汾�š�
//00 00 00 00 00 00 ��6~11��ʱ���ǩ
//01 02 03 04 05 06 ��12~17������ID
//00 00 00 00 00 00 ��18~23��Ŀ�ĵ�ַ
//03 00 ��24~25��Ӧ�����ݵ�Ԫ����
//02 ��26�������ֽڣ��������ݣ�
//15 ��27��//���ͱ�ʶ 1�ֽ��ϴ�����������ʩ��������״̬ ����
//01 ��28��//��Ϣ������Ŀ,1����
//01 ��29��//����״̬Ϊ������
//34 ��30��//У���
//23 23��31~32��//���ݰ�β	
//USART1_SEND(message,message[0]+1);
// 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33
//21 06 01 00 FB 00 00 00 01 36 FB 00 00 00 01 40 FB 00 00 00 01 41 FB 00 00 00 01 44 FB 00 00 00 01 45 
/*************������***************/
	if(message[1]==0x01)//����
	{
		printf("������վ����������ҵ����ˮ�ţ�%d\r\n",last_serial_number);//����	
	}
	if(message[1]==0x06)//�������λ���߻ָ�������
	{
		printf("������վ������λ/�ָ���ҵ����ˮ�ţ�%d\r\n",last_serial_number);//����	
////		��λ��ǩID����Ϊ��
		leave_no=(message[0]-3)/7;
		printf("״̬�䶯��ǩID����Ϊ��%d\r\n",leave_no);//����
		printf("/=======================================/");
		printf("\r\n");							//�����á�		
		for(i=0;i<leave_no;i++)
		{
			for(j=0;j<6;j++)
			{
				data[j]=message[5+7*i+j];
			}
			if(message[4+7*i]==0x02)
			{
				printf("��λ:");
			}
//			else if(message[4+7*i]==0x05)
			else
			{
				printf("�ָ�:");
			}
			handle_arrays_u8(data,6,printf_data);//�����á�
			USART1_SEND(printf_data,12);		//�����á�
			printf("\r\n");							//�����á�					
		}
		printf("/=======================================/");
		printf("\r\n");							//�����á�			
	}
/**********************************/
if(message[1]==0x01)//����
{
	//ͨ��������������������
	//40 40 01 00 01 20 00 00 00 00 00 00 B2 23 96 60 40 01 00 00 00 00 00 00 03 00 02 15 01 01 4A 23 23
	// 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32		
	//������
	//40 40 (0~1)��ͷ
	//01 00 (2~3)ҵ����ˮ�š�����λ��ǰ��
	//00 02 (4~5)Э��汾�š�
	//00 00 00 00 00 00 (6~11)ʱ���ǩ
	//B2 23 96 60 40 01 (12~17)����ID
	//00 00 00 00 00 00 (18~23)Ŀ�ĵ�ַ
	//03 00 (24~25)Ӧ�����ݵ�Ԫ����
	//02 (26)�����ֽڣ��������ݣ�
	//15 (27)//���ͱ�ʶ 1�ֽ��ϴ�����������ʩ��������״̬ ����
	//01 (28)//��Ϣ������Ŀ,1����
	//01 (29)//����״̬Ϊ������
	//4A (30)//У���
	//23 23 (31~32)//���ݰ�β
		// ������[2�ֽ�]
		data[0]=0x40;
		data[1]=0x40;
		/***���Ƶ�Ԫ*****************/
		// ҵ����ˮ��[2�ֽ�]
		data[2]=message[2];//��ˮ�š�0x00 0x00 ~ 0xFF 0xFF.��������λ��ǰ	
		data[3]=message[3];				
		// Э��汾��[2�ֽ�]
		data[4]=0x01;//��λ��ǰ
		data[5]=0x20;
		// ʱ���ǩ[6�ֽ�]
		for(i=0;i<6;i++)
		{
//		package_data[2]=serial_number_counter_middle;//��ˮ�š�0x00 0x00 ~ 0xFF 0xFF.��������λ��ǰ	
//		package_data[3]=serial_number_counter_middle>>8;		
//		package_data[4]=package_data[1];//��
//		package_data[5]=package_data[2];//��
//		package_data[6]=package_data[3];//ʱ
//		package_data[7]=package_data[4];//��			
//		package_data[8]=package_data[5];//��	
//		package_data[9]=package_data[6];//��					
			data[6+i]=message[4+i];
		}
		// Դ��ַ[6�ֽ�] 11 ed e3 9d ee 1e
		for (i = 0; i < 6; i++)// ������λ��վ��6λID
		{
			data[12+i]=Gateway_ID[i];
		}	
		// Ŀ�ĵ�ַ[6�ֽ�] 00 00 00 00 00 00
		for (i = 0; i < 6; i++)
		{
			data[18+i]=0x00;
		}
		// Ӧ�����ݵ�Ԫ����[2�ֽ�]��λ��ǰ���䡣
		data[24]=0x03;
		data[25]=0x00;
		// �����ֽ�[1�ֽ�]
		data[26]=0x02;//��������
		/*******Ӧ�����ݵ�Ԫ***********/		
		data[27]=0x15;//���ͱ�ʶ 1�ֽ��ϴ�����������ʩ��������״̬ ����
		data[28]=0x01;//��Ϣ������Ŀ
		//------��Ϣ��----------------//
		data[29]=0x01;//����״̬Ϊ������
		/********У���***************/
		// У���
		data[30]=sumcheck(data,2,29);//����У���.	
		/********������***************/
		// ������
		data[31]=0x23;
		data[32]=0x23;
		step=33;
	//	USART1_SEND(data,33);//�����á�		
	//	printf("\r\n");	
	//	USART2_SEND(data,33);//ͨ������2���͸�GM3��			
	//	UART4_SEND(data,33);//�����á�		
		/********************/				
	/*******************����ѹջ�����׼���Ϸ���������*********************/
}
if(message[1]==0x06)//��λ��ָ�����
{
	//������������磺 24 FB 00 00 00 00 14 FB 00 00 00 00 15 FB 00 00 00 00 16 FB 00 00 00 00 17  ��ʾ���ĸ���ǩ��λ��
	/*****************************************������վ������λ��Ϣ��������***************************************************/
	
	
//40 40 3b 00 01 20 00 00 00 00 00 00 b2 23 96 60 40 0b 00 00 00 00 00 00 30 00 02 02 01 02 01 b2 00 00 00 00 05 00 fb 00 00 00 01 55 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 b2 23 23	
// 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77  
	//������
	//40 40 (0,1)��ͷ
	//3b 00 (2,3)ҵ����ˮ�š�����λ��ǰ��
	//01 20 (4,5)Э��汾�š�
	//00 00 00 00 00 00 (6,7,8,9,10,11)ʱ���ǩ
	//b2 23 96 60 40 0b (12,13,14,15,16,17)����ID
	//00 00 00 00 00 00 (18,19,20,21,22,23)Ŀ�ĵ�ַ
	//30 00 (24,25)Ӧ�����ݵ�Ԫ����
	//02 (26)�����ֽ�
	//02(27)//���ͱ�ʶ 1�ֽ��ϴ�����������ʩ��������״̬ ����
	//01(28)//��Ϣ������Ŀ,1����
	//02(29)//ϵͳ���ͱ�ʶ	 1�ֽ� ���ֱ���ϵͳ 01 
	//01(30)//ϵͳ��ַ	1�ֽ� ϵͳ��ַ=01
	//B2(31)//�������� 
	//00 00(32,33)//��·�š���λ��ǰ�� 
	//00 00(34,35)//�����š�	��λ��ǰ��	 
	//05 00(36,37)//����״̬��	��λ��ǰ��
	//00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00��38~68��//����31���ֽ� 
	//00 00 00 00 00 00��69~74��//����6λʱ��
	//b2(75)//У��� 
	//23 23(76��77)//���ݰ�β		
	

//delay_ms(20);
		// ������[2�ֽ�]
		data[0]=0x40;
		data[1]=0x40;
		/***���Ƶ�Ԫ*****************/ 		
		// ҵ����ˮ��[2�ֽ�]
		data[2]=message[2];//��ˮ�š�0x00 0x00 ~ 0xFF 0xFF.��������λ��ǰ	
		data[3]=message[3];	
		// Э��汾��[2�ֽ�]
		data[4]=0x01;//��λ��ǰ
		data[5]=0x20;
		// ʱ���ǩ[6�ֽ�]
//		for(i=0;i<6;i++)
//		{
//			data[6+i]=0x00;
//		}

		data[6]=message[message[0]-5];//��
		data[7]=message[message[0]-4];//��		
		data[8]=message[message[0]-3];//ʱ			
		data[9]=message[message[0]-2];//��			
		data[10]=message[message[0]-1];//��	
		data[11]=message[message[0]-0];//��				

		// Դ��ַ[6�ֽ�] B2 23 96 60 40 01
		for (i = 0; i < 6; i++)// ������վ��6λID
		{
			data[12+i]=Gateway_ID[i];
		}
		// Ŀ�ĵ�ַ[6�ֽ�] 00 00 00 00 00 00
		for (i = 0; i < 6; i++)
		{
			data[18+i]=0x00;
		}
		// Ӧ�����ݵ�Ԫ����[2�ֽ�] 1C 00����28���ֽڣ������ʮ�����ƾ���0x1C����λ��ǰ���䡣
	////////////////////////////////////	data[24]=0x1C;
	////////////////////////////////////	data[25]=0x00;
		// �����ֽ�[1�ֽ�]
		data[26]=0x02;//�����ֽ�Ϊ�������ݣ��ϴ�����������ʩ��������״̬��
		/*******Ӧ�����ݵ�Ԫ***********/
	//���ݵ�Ԫ��ʶ����
		data[27]=0x02;//���ͱ�ʶ 1�ֽ��ϴ�����������ʩ��������״̬ ����
//		data[28]=5;//��Ϣ������Ŀ
		data[28]=(message[0]-9)/7;//��Ϣ������Ŀ		
		//------��Ϣ��----------------//
		step=29;
//		for(t=0;t<5;t++)
		for(t=0;t<(message[0]-9)/7;t++)//��λ��ָ���ĸ�����		
		{
			data[step]=0x02;//ϵͳ���ͱ�ʶ	 1�ֽ� ���ֱ���ϵͳ 01																//29						
			step=step+1;
			data[step]=0x01;//ϵͳ��ַ	1�ֽ� ϵͳ��ַ=01																				//30
			step=step+1;
			data[step]=0xB2;//��������Ϊ������λ��վ��0xB2.�����������ڲ���ID���棬��һ���ֽڡ���//31
			step=step+1;		
			data[step]=0x00;//��·�š���λ��ǰ��																								//32
			step=step+1;		
			data[step]=0x00;																																	//33
			step=step+1;
			data[step]=0x00;//�����š�	��λ��ǰ��																								//34
			step=step+1;		
			data[step]=0x00;																																	//35
			step=step+1;	
			data[step]=message[4+7*t];//0x02Ϊ��λ��0x05Ϊ�ָ���																//36
			step=step+1;
			data[step]=0x00;																																	//37
			step=step+1;		
		//~~~~~~~~~~����˵������31���ֽڣ�~~~~~~~~~~~~~//
			for (i = 0; i < 6; i++)//������6λID��������λ��վ��ID��.//38
			{
				data[step+i]=message[5+7*t+i];
			}
			step=step+6;	
			for (i = 0; i < 25; i++)//����31���ֽڡ�
			{
				data[step+i]=0x00;
			}
			step=step+25;		
		//----------����6λʱ��------------//		
			for(i=0;i<6;i++)
			{//����6��ʱ���ֽڡ�
				data[step]=0x00;
				step=step+1;
			}	
		}
	// Ӧ�����ݵ�Ԫ����[2�ֽ�],��λ��ǰ���䡣
step_middle=step-27;		
		data[24]=step_middle;
		data[25]=step_middle>>8;		
		
		
//		data[24]=step-27;
//		data[25]=0x00;
		/********У���***************/
		// У���
		data[step]=sumcheck(data,2,step-1);//����У���.
		step=step+1;
		/********������***************/
		// ������
		data[step]=0x23;
		step=step+1;
		data[step]=0x23;
		step=step+1;//������Ԫ���ܹ���������
	//	USART1_SEND(data,step);//�����á�		
	//	printf("\r\n");		
	//	USART2_SEND(data,step);//ͨ������2���͸�GM3��			
	//	UART4_SEND(data,step);//�����á�				
	/****************************************/	
	//	USART1_SEND(data,lenth+3);//�����ã�ͨ������1���͸�GM3.		
}
	 POST(&BT_Msg,step,data,&STORAGE_MEM);//GPRS_senddata(data,step);//USART2_SEND(data,step);//ͨ������2���͸�GM3��	
//	USART1_SEND(data,step);//����
//	printf("\r\n");//����	
}
void package_send_data(unsigned char *input_data)//	data���������顣�������ж��������ݵ��㷨����������Ԫ�صĸ�����1Ȼ�����6������ĵ�һ��Ԫ����������Ԫ�ظ�����size����������Ԫ�صĸ�����status����״̬��0x01������/Ѳ��/����.0x06����λ������
{//����ʹ��GSM/M26/NB-IOT�������ݵķ�ʽ��ѹջ���ݰ����顣
//������������磺 24 FB 00 00 00 00 14 FB 00 00 00 00 15 FB 00 00 00 00 16 FB 00 00 00 00 17  ��ʾ���ĸ���ǩ��λ��
/*****************************************������վ������λ��Ϣ��������***************************************************/
//40 40 01 00 01 20 00 00 00 00 00 00 B2 23 96 60 40 01 00 00 00 00 00 00 38 00 02 02 04 02 01 B2 00 00 00 00 06 00 B2 23 96 60 40 01 04 04 06 FB 00 00 00 00 14 04 06 FB 00 00 00 00 15 04 06 FB 00 00 00 00 16 04 06 FB 00 00 00 00 17 00 00 00 00 00 00 A3 23 23 
// 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 
//������
//40 40 (0,1)��ͷ
//01 00 (2,3)ҵ����ˮ�š�����λ��ǰ��
//01 20 (4,5)Э��汾�š�
//00 00 00 00 00 00 (6,7,8,9,10,11)ʱ���ǩ
//01 02 03 04 05 06 (12,13,14,15,16,17)����ID
//00 00 00 00 00 00 (18,19,20,21,22,23)Ŀ�ĵ�ַ
//38 00 (24,25)Ӧ�����ݵ�Ԫ����
//02 (26)�����ֽ�
//02(27)//���ͱ�ʶ 1�ֽ��ϴ�����������ʩ��������״̬ ����
//04(28)//��Ϣ������Ŀ,1����		
//02(29)//ϵͳ���ͱ�ʶ	 1�ֽ� ���ֱ���ϵͳ 01 
//01(30)//ϵͳ��ַ	1�ֽ� ϵͳ��ַ=01
//B2(31)//�������� 
//00 00(32,33)//��·�š���λ��ǰ�� 
//00 00(34,35)//�����š�	��λ��ǰ��	 
//06 00(36,37)//����״̬��	��λ��ǰ��
//28 23 96 20 00 01(38,39,40,41,42,43)//������6λID
//04(44)//�ϱ���������
//04(45)//��������(��λ��
//06(46)//�������ȣ�ÿ��ID��6���ֽڣ�
//FB 00 00 00 00 14(47~52)//��λ��ǩ��6λID 
//04(53)//��������(��λ�� 
//06(54)//�������ȣ�ÿ��ID��6���ֽڣ�  
//FB 00 00 00 00 15(55~60)//��λ��ǩ��6λID   
//04(61)//��������(��λ��   
//06(62)//�������ȣ�ÿ��ID��6���ֽڣ�  
//FB 00 00 00 00 16(63~68)//��λ��ǩ��6λID   
//04(69)//��������(��λ��   
//06(70)//�������ȣ�ÿ��ID��6���ֽڣ�  
//FB 00 00 00 00 17(71~76)//��λ��ǩ��6λID
//00 00 00 00 00 00(77~82)//ʱ�䡣
//A3(83)//У��� 
//23 23(84��85)//���ݰ�β	


//��λ��			
//FB 00 00 00 02 15			
//FB 00 00 00 00 30			
//�ָ���
//FB 00 00 00 01 11 
//FB 00 00 00 00 45		
//1C������Ǻ���Ԫ�ص������������ʮ����Ϊ28.
// 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 
//1C 05 FB 00 00 00 02 15 02 FB 00 00 00 01 11 02 FB 00 00 00 00 45 05 FB 00 00 00 00 30 	




	unsigned int i = 0;
	unsigned int j = 0;	
//	unsigned int lenth = 0;	
	unsigned int step = 0;
	unsigned char package_data[100]={0x00};//̫���ˡ�����	
	unsigned short  serial_number_counter_middle=0;//ҵ����ˮ��
/******************�������ݰ�**********************/
	if(serial_number_counter>=0xFFFE)//ҵ����ˮ�ŵ��Լӡ�
	{
		serial_number_counter=0;
	}
	else
	{
		serial_number_counter=serial_number_counter+1;
	} 		
	if(input_data[0]==0xFF)//����
	{
		// ������[2�ֽ�]
		package_data[0]=3;
		package_data[1]=0x01;//��־����Ϊ������	
		/***���Ƶ�Ԫ*****************/
		// ҵ����ˮ��[2�ֽ�]
		serial_number_counter_middle=serial_number_counter;
		package_data[2]=serial_number_counter_middle;//��ˮ�š�0x00 0x00 ~ 0xFF 0xFF.��������λ��ǰ	
		package_data[3]=serial_number_counter_middle>>8;		
		package_data[4]=package_data[1];//��
		package_data[5]=package_data[2];//��
		package_data[6]=package_data[3];//ʱ
		package_data[7]=package_data[4];//��			
		package_data[8]=package_data[5];//��	
		package_data[9]=package_data[6];//��		
		step=10;
	//	USART1_SEND(data,33);//�����á�
	}
	else
	{
		// ���ͱ�־[1�ֽ�]��
		package_data[1]=0x06;		
		// ҵ����ˮ��[2�ֽ�]
		serial_number_counter_middle=serial_number_counter;	
		package_data[2]=serial_number_counter_middle;//��ˮ�š�0x00 0x00 ~ 0xFF 0xFF.��������λ��ǰ
		package_data[3]=serial_number_counter_middle>>8;
		// ��λIDѹ�롣
		for (i = 0; i < input_data[0]; i++)//������״̬��ID��������λ��վ��ID��.
		{
			package_data[4+i]=input_data[1+i];
		}

		RTC_Get();//��ȡRTCʱ�䣬����ʱ�����	
////calendar.w_year=calendar.w_year%2000;	//�꣬ȡ��ݵĺ���λ��
////calendar.w_month;	//��
////calendar.w_date;	//��
////calendar.hour;		//Сʱ
////calendar.min;			//����
////calendar.sec;			//����	
//		package_data[1]=calendar.sec;//��
//		package_data[2]=calendar.min;//��
//		package_data[3]=calendar.hour;//ʱ
//		package_data[4]=calendar.w_date;//��			
//		package_data[5]=calendar.w_month;//��	
//		package_data[6]=calendar.w_year;//��				

		package_data[4+input_data[0]]=calendar.sec;//��
		package_data[5+input_data[0]]=calendar.min;//��
		package_data[6+input_data[0]]=calendar.hour;//ʱ
		package_data[7+input_data[0]]=calendar.w_date;//��			
		package_data[8+input_data[0]]=calendar.w_month;//��	
		package_data[9+input_data[0]]=calendar.w_year;//��			

		// ������Ԫ�صĸ���[1�ֽ�]��
//		package_data[0]=input_data[0]+3;//��Ҫ�Ǽ�������ˮ�š�
		package_data[0]=input_data[0]+9;//��Ҫ�Ǽ�������ˮ�ź�ʱ�䡣
		step=package_data[0]+1;//���ݰ���Ԫ���ܸ�����	
//	USART1_SEND(data,step);//�����ã�ͨ������1���͸�GM3.			
	}
//	LoRa_433_Pack_DATA(package_data);//package_data[0]��������Package_DATA��Ԫ�صĸ���+1.	
/*******************ѹջ���ݰ�����*********************/	
	//������λ�õ�Ԫ��������Ϊ0������ǰ��һ��
	for(i=0;i<step;i++)//ѹջ����
	{
		send_data[SEND_DATA_to_server_GROUP][i]=package_data[i];
	}
	if(SEND_DATA_to_server_GROUP>=19)//Ⱥ��������ó������������ĳ��ȡ�ѭ����䡣0~19~0~19
	{
		SEND_DATA_to_server_GROUP=0;//Ⱥ������
	}
	else
	{//�����������
		SEND_DATA_to_server_GROUP++;//Ⱥ��ĸ�����1
	}
    if(send_data[SEND_DATA_to_server_GROUP][0]>0)//û���ü�����������Ѿ��γ�һ�����ˣ���Ҫ���ʼ��һ֡��������ˡ�
	{
		finger_point=SEND_DATA_to_server_GROUP;//��ָ��ָ���λ���� SEND_DATA_to_server_GROUP һ��һ���������ܴ�ʱfinger_point��λ�ã���
	}
/****************************************/	
//	USART1_SEND(data,step);//�����ã�ͨ������1���͸�GM3.	
}



///=======================================/[2018-05-11 07:39:51.491]

//������վ����������ҵ����ˮ�ţ�209[2018-05-11 11:08:02.464]
//������վ����������ҵ����ˮ�ţ�464[2018-05-12 03:24:47.384]
//������վ����������ҵ����ˮ�ţ�654[2018-05-12 06:36:10.614]
//GPRSģ�鳤ʱ��δ�ܷ��ͳɹ����ݣ�������[2018-05-12 06:36:30.712]

//������վ����������ҵ����ˮ�ţ�654[2018-05-12 06:36:36.821]
//������վ����������ҵ����ˮ�ţ�705[2018-05-12 07:28:24.605]
//������վ����������ҵ����ˮ�ţ�719[2018-05-12 07:42:48.838]
//GPRSģ�鳤ʱ��δ�ܷ��ͳɹ����ݣ�������[2018-05-12 07:43:08.936]



//40 40 04 00 01 20 00 00 00 00 00 00 98 25 0A 30 40 08 00 00 00 00 00 00 30 00 02 02 01 02 01 B2 00 00 00 00 02 00 FB 00 00 00 01 50 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 9C 23 23 
//40 40 05 00 01 20 00 00 00 00 00 00 98 25 0A 30 40 08 00 00 00 00 00 00 30 00 02 02 01 02 01 B2 00 00 00 00 05 00 FB 00 00 00 01 50 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 A0 23 23 





