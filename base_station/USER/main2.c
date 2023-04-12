/***********************************************************************/
//         �м�����MINI�� V1.0
// ��˾������Ƽ�
// ʱ�䣺2017-5-30
/***********************************************************************/
// ���ص�ַ��0x11111����������ַ��

/*****************ͷ�ļ�*************************************************/
#include "include.h"
#include "string.h"

/*****************ȫ�ֱ���***********************************************/
//������Ϣ
unsigned long int address=0xE011B;
unsigned char data=0x05;

unsigned char key1=0;
unsigned char key2=0;
unsigned char key3=0;

unsigned char number=0;
unsigned char send_ok=0;
unsigned long int old_address=0;
unsigned long temp_address=0;

// ��������
unsigned int  key1_shake=0;
unsigned int  key2_shake=0;
unsigned int  key3_shake=0;
unsigned char key1_value=0;
unsigned char key2_value=0;
unsigned char key3_value=0;

// GPRS ͨ��
unsigned char Link_Flag=0;      // ���ӱ�־ 1������ 0��δ����
unsigned int  Hart_Time=0;      // ��������ʱ�������30S��
unsigned char Hart_Send_Flag=0; // �������ͱ�־ 1: send
unsigned char Hart_Data[7]={0x33,0x01,0x01,0x01,0x01,0x01};
unsigned char IMEI_Number1[16]={'3','5','6','5','6','6','0','7','0','7','1','8','8','3','8'};  // IMEI��[1]
unsigned char IMEI_Number2[16]={'3','5','6','5','6','6','0','7','0','7','1','5','0','6','7'};
unsigned char SN_Number1[16]={'0','1','1','2','0','1','6','0','6','0','3','6','6','5','3'};    // SN��
unsigned char PHONE_Number1[16]={'1','0','6','4','8','3','2','1','7','2','9','3','4'};         // TEL�绰����
unsigned short FrameCount = 0;// ֡������

// ģʽ����
unsigned char Set_Mode_Flag = 0;  // ����ģʽ 1
unsigned char Normal_Mode_Flag=1; // ��������ģʽ 1

// �豸ע��
unsigned char Device_Count=0;    // ��ע���豸����

// EEPROM�洢
unsigned char Flash_Query=0;     // �����豸��Ϣ���ڵ�ַ

// ����֡���ݲ���[DTUЭ��]
unsigned char Alarm_Data[10];    // ����֡���ݲ���

// ʱ��ȴ�
unsigned int iwwd_time=0;

// ����ע������
unsigned char delete_low_address = 0; // ɾ���豸��ַ ��λ
unsigned char delete_mid_address = 0; // ��λ
unsigned char delete_high_address = 0;// ��λ

unsigned char temp_low_address = 0;  // ��ȡ��λ��ַ
unsigned char temp_mid_address = 0;  // ��ȡ��λ��ַ
unsigned char temp_high_address = 0; // ��ȡ��λ��ַ
unsigned char temp_type = 0;         // ��ȡ�豸����
unsigned char temp_pass_nnmber = 0;  // ��ȡ��·��
unsigned char temp_part_number = 0;  // ��ȡ�̼���

unsigned char last_low_address = 0;  // ��ȡ��λ��ַ
unsigned char last_mid_address = 0;  // ��ȡ��λ��ַ
unsigned char last_high_address = 0; // ��ȡ��λ��ַ
unsigned char last_type = 0;         // ��ȡ�豸����
unsigned char last_pass_nnmber = 0;  // ��ȡ��·��
unsigned char last_part_number = 0;  // ��ȡ�̼���

unsigned char delete_ok_flag=0;      // ɾ���ɹ���־

// RF���߽������ݴ���
unsigned char RF_First_Data[5];

/*******************���ݴ���*************************************************/
unsigned char Wen1_Enable_Flag=1;    // 2���¸������ͱ�־
unsigned char Yan1_Enable_Flag=1;    // 1���̸������ͱ�־ 
unsigned char Shou1_Enable_Flag=1;   // 3���ֱ������ͱ�־

unsigned char Wen1_Start_Flag=0;     // 2���¸п�ʼ��ʱ��־
unsigned char Yan1_Start_Flag=0;     // 1���̸п�ʼ��ʱ��־
unsigned char Shou1_Start_Flag=0;    // 3���ֱ���ʼ��ʱ��־r

unsigned int  Wen1_Timer_Count=0;    // 2���¸��ٴη��ͼ�ʱ
unsigned int  Yan1_Timer_Count=0;    // 1���̸��ٴη��ͼ�ʱ
unsigned int  Shou1_Timer_Count=0;   // 3���ֱ��ٴη��ͼ�ʱ


/*****************�궨��****************************************************/
#define READ_KEY1_STATE (key1_value&0x01)
#define SET_KEY1_STATE (key1_value|=0x01)
#define CLEAR_KEY1_STATE (key1_value&=~0x01)

#define READ_KEY1_SHORT (key1_value&0x02)
#define SET_KEY1_SHORT (key1_value|=0x02)
#define CLEAR_KEY1_SHORT (key1_value&=~0x02)

#define READ_KEY1_DOUBLE (key1_value&0x04)
#define SET_KEY1_DOUBLE (key1_value|=0x04)
#define CLEAR_KEY1_DOUBLE (key1_value&=~0x04)

#define READ_KEY1_LONG (key1_value&0x08)
#define SET_KEY1_LONG (key1_value|=0x08)
#define CLEAR_KEY1_LONG (key1_value&=~0x08)

// �������״̬��ȷ��ֻ��һ�ְ���״̬
#define CLEAR_KEY1 (key1_value&=~0xFE)
#define CLEAR_KEY2 (key2_value&=~0xFE)
#define CLEAR_KEY3 (key3_value&=~0xFE)

#define READ_KEY1 (key1_value&0xFE)
#define READ_KEY2 (key2_value&0xFE)
#define READ_KEY3 (key3_value&0xFE)

#define READ_KEY2_STATE (key2_value&0x01)
#define SET_KEY2_STATE (key2_value|=0x01)
#define CLEAR_KEY2_STATE (key2_value&=~0x01)

#define READ_KEY2_SHORT (key2_value&0x02)
#define SET_KEY2_SHORT (key2_value|=0x02)
#define CLEAR_KEY2_SHORT (key2_value&=~0x02)

#define READ_KEY2_DOUBLE (key2_value&0x04)
#define SET_KEY2_DOUBLE (key2_value|=0x04)
#define CLEAR_KEY2_DOUBLE (key2_value&=~0x04)

#define READ_KEY2_LONG (key2_value&0x08)
#define SET_KEY2_LONG (key2_value|=0x08)
#define CLEAR_KEY2_LONG (key2_value&=~0x08)

#define READ_KEY3_STATE (key3_value&0x01)
#define SET_KEY3_STATE (key3_value|=0x01)
#define CLEAR_KEY3_STATE (key3_value&=~0x01)

#define READ_KEY3_SHORT (key3_value&0x02)
#define SET_KEY3_SHORT (key3_value|=0x02)
#define CLEAR_KEY3_SHORT (key3_value&=~0x02)

#define READ_KEY3_DOUBLE (key3_value&0x04)
#define SET_KEY3_DOUBLE (key3_value|=0x04)
#define CLEAR_KEY3_DOUBLE (key3_value&=~0x04)

#define READ_KEY3_LONG (key3_value&0x08)
#define SET_KEY3_LONG (key3_value|=0x08)
#define CLEAR_KEY3_LONG (key3_value&=~0x08)


/*************��������****************************************************/
void send_data(void); 
void uart_sendbyte(unsigned char src);
void DTU_send_data(unsigned char Device_Type,unsigned char Channel);
void FeedTheIWDog();

/*************����ԭ��****************************************************/
void delay_ms(unsigned int time)
{
    unsigned int i=0;
    for(i=0;i<time;i++)
    {
        delay_us(100);
       //WWDG_Enable(80);
	   FeedTheIWDog();// ι��
    }
}

void delay(unsigned int nCount)
{
    for (; nCount != 0; nCount--);
}

// ����Flash����ע������
void return_flash(void)
{
    unsigned char num1;
	num1=FLASH_ReadByte(NUMBER_ADDRESS); 
    USART_SendData8(USART1 , 0x07); 
    USART_SendData8(USART1 , 0xFF); 
    USART_SendData8(USART1 , FLASH_ReadByte(DEVICE_ADDRESS+num1*6)); 
    USART_SendData8(USART1 , FLASH_ReadByte(DEVICE_ADDRESS+num1*6+1)); 
    USART_SendData8(USART1 , FLASH_ReadByte(DEVICE_ADDRESS+num1*6+2)); 
	USART_SendData8(USART1 , FLASH_ReadByte(DEVICE_ADDRESS+num1*6+3)); 
    USART_SendData8(USART1 , FLASH_ReadByte(DEVICE_ADDRESS+num1*6+4)); 
    USART_SendData8(USART1 , FLASH_ReadByte(DEVICE_ADDRESS+num1*6+5)); 
    USART_SendData8(USART1 , 0x0D); 
    USART_SendData8(USART1 , 0x0A);     
}

// ������ʼ��
void KEY_GPIO_Init(void)
{
    GPIO_Init(KEY1_GPIO_PORT,KEY1_GPIO_PINS,GPIO_Mode_In_PU_No_IT); // KEY1 �����жϵ���������ģʽ
    //  EXTI_SetPortSensitivity(EXTI_Port_B ,  EXTI_Trigger_Rising);  //PortB�˿�Ϊ�����س����ж�
    //  EXTI_SetHalfPortSelection(EXTI_HalfPort_B_LSB , ENABLE);       //����PB[3:0]��Ϊ�ⲿͨ���ж�
    GPIO_Init(KEY2_GPIO_PORT,KEY2_GPIO_PINS,GPIO_Mode_In_PU_No_IT); // KEY2 �����жϵ���������ģʽ
	GPIO_Init(KEY3_GPIO_PORT,KEY3_GPIO_PINS,GPIO_Mode_In_PU_No_IT); // KEY3 �����жϵ���������ģʽ
}
 
//�رղ���Ҫ��IO��
void HALT_Off_GPIO(void)  
{  
    //IO����Ϊ���  
    GPIO_Init(GPIOA, GPIO_Pin_0|GPIO_Pin_2 | GPIO_Pin_3, GPIO_Mode_Out_PP_High_Fast);   
    GPIO_Init(GPIOB, GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7, GPIO_Mode_Out_PP_High_Fast);   
    GPIO_Init(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast);   
    GPIO_Init(GPIOD, GPIO_Pin_0 , GPIO_Mode_Out_PP_High_Fast);   
    
    //IO����Ϊ����ͣ�ע��͵�ƽʹ�ܵĵ�����Ϊ����ߣ�  
    GPIO_ResetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_2 | GPIO_Pin_3);
    GPIO_ResetBits(GPIOB,GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
    GPIO_ResetBits(GPIOC,GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_4);
    GPIO_ResetBits(GPIOD,GPIO_Pin_0);
    
    //�ⲿ����ӿ�����Ϊ����ģʽ
    GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_In_PU_No_IT);   
    
    //NRST����
    //  GPIO_Init(GPIOA, GPIO_Pin_1 | GPIO_Pin_6, GPIO_Mode_In_PU_No_IT);   
    
} 

// GPIO��ʼ��
void GPIO_Set(void)
{
    GPIO_Init(LEDR_GPIO_PORT,LEDR_GPIO_PINS,GPIO_Mode_Out_PP_Low_Fast);
    GPIO_Init(LEDG_GPIO_PORT,LEDG_GPIO_PINS,GPIO_Mode_Out_PP_Low_Fast);
	GPIO_Init(LEDS_GPIO_PORT,LEDS_GPIO_PINS,GPIO_Mode_Out_PP_Low_Fast);
	GPIO_Init(LEDL_GPIO_PORT,LEDL_GPIO_PINS,GPIO_Mode_Out_PP_Low_Fast);
	
	//---------GPRSͨ��---------------------------------------------------//
    GPIO_Init(ST_GPIO_PORT,ST_GPIO_PINS,GPIO_Mode_In_PU_No_IT);       // ST    �����жϵ���������ģʽ
	GPIO_Init(LINKA_GPIO_PORT,LINKA_GPIO_PINS,GPIO_Mode_In_PU_No_IT); // LINKA �����жϵ���������ģʽ
}

//�ر�ʱ������
void HALT_Off_Clkout(void)  
{  
    //ʱ��ʧ��  
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, DISABLE);  
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, DISABLE);  
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, DISABLE);  
    CLK_PeripheralClockConfig(CLK_Peripheral_USART1, DISABLE);  
} 

// ʱ������
void HALT_Clk(void)  
{  
    //�Ĵ�������   
    CLK_DeInit();                
    //ʱ��8��Ƶ��2MHz  
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_8); 
}  

// 1���������ù���
// 2��Զ�̿��ƹ���
// 3��������Ӧ��
void receive_data(void)
{
    unsigned char i = 0;
	unsigned char num=0;
    unsigned int k1=0;
	
	//--------------����ģʽע������¼��-------------------------------------------//
    //��������
	if ((uart_buffer[0] == 0x07) && (uart_buffer[1] == 0xf0)&&(Set_Mode_Flag==1))
    { 
		// ��ѯ�����豸��Ϣ����
        if (uart_buffer[2] == 0x06)
        {
			// �������д������
			number=FLASH_ReadByte(NUMBER_ADDRESS);    
		    for(i=0;i<number;i++)
			{
				temp_low_address = FLASH_ReadByte((DEVICE_ADDRESS)+6*i);  // ��ȡ��λ��ַ
				temp_mid_address = FLASH_ReadByte((DEVICE_ADDRESS)+6*i+1);  // ��ȡ��λ��ַ
				temp_high_address = FLASH_ReadByte((DEVICE_ADDRESS)+6*i+2); // ��ȡ��λ��ַ
				temp_type = FLASH_ReadByte((DEVICE_ADDRESS)+6*i+3);         // ��ȡ�豸����
				temp_pass_nnmber = FLASH_ReadByte((DEVICE_ADDRESS)+6*i+4);  // ��ȡ��·��
				temp_part_number = FLASH_ReadByte((DEVICE_ADDRESS)+6*i+5);  // ��ȡ�̼���
				
				uart_sendbyte(temp_low_address); // �����������ע���豸
				uart_sendbyte(temp_mid_address);
				uart_sendbyte(temp_high_address);
				uart_sendbyte(temp_type);
				uart_sendbyte(temp_pass_nnmber);
				uart_sendbyte(temp_part_number);
				delay_ms(1000); // ��ʱ1S�ȴ�֡����
			}
            //return_flash();
        }
        // ע���豸��������
        else if (uart_buffer[2] == 0x05)
        {
            num=FLASH_ReadByte(NUMBER_ADDRESS);             // ��ȡ��ע���豸����
			num++;
			FLASH_SetProgrammingTime(FLASH_ProgramTime_TProg); //�趨���ʱ��Ϊ��׼���ʱ��
			FLASH_Unlock(FLASH_MemType_Data);
			FLASH_ProgramByte(NUMBER_ADDRESS,num);             // ������ע���豸����
			while(FLASH_GetFlagStatus(FLASH_FLAG_WR_PG_DIS) == 1);    // �ȴ���̽���
			while(FLASH_GetFlagStatus(FLASH_FLAG_EOP) == 1);          // �ȴ���̽���
			//-----------------------------------------------------------------------------------//
			if(num<=42)// MCU���256B�ֽ� 6�ֽڼ�¼һ���豸
			{
				//д��ע���豸��Ϣ����
				FLASH_SetProgrammingTime(FLASH_ProgramTime_TProg); //�趨���ʱ��Ϊ��׼���ʱ��
				FLASH_Unlock(FLASH_MemType_Data);
				FLASH_ProgramByte(DEVICE_ADDRESS+(num-1)*6,uart_buffer[4]);   // ��ַ��λ
				FLASH_ProgramByte(DEVICE_ADDRESS+(num-1)*6+1,uart_buffer[5]); // ��ַ��λ
				FLASH_ProgramByte(DEVICE_ADDRESS+(num-1)*6+2,uart_buffer[6]); // ��ַ��λ
				FLASH_ProgramByte(DEVICE_ADDRESS+(num-1)*6+3,uart_buffer[3]); // �豸����
				FLASH_ProgramByte(DEVICE_ADDRESS+(num-1)*6+4,uart_buffer[7]); // ��·��
				FLASH_ProgramByte(DEVICE_ADDRESS+(num-1)*6+5,uart_buffer[8]); // ������                                                     
				while(FLASH_GetFlagStatus(FLASH_FLAG_WR_PG_DIS) == 1);    // �ȴ���̽���
				while(FLASH_GetFlagStatus(FLASH_FLAG_EOP) == 1);          // �ȴ���̽���
				FLASH_Lock(FLASH_MemType_Data);       
				//return_flash(); // ����EEPROMֵ
				uart_sendbyte(0x0A); // ע��ɹ�����Ӧ
				uart_sendbyte(0x0B);
			}
			else // �豸����
			{
			}
        }
        // ɾ�������豸����
        else if(uart_buffer[2]==0x0F)
        {              
            FLASH_SetProgrammingTime(FLASH_ProgramTime_TProg); //�趨���ʱ��Ϊ��׼���ʱ��
            FLASH_Unlock(FLASH_MemType_Data);
            for(k1=0;k1<256;k1++)
            {
                FLASH_EraseByte(DEVICE_ADDRESS+k1);
            }           
            while(FLASH_GetFlagStatus(FLASH_FLAG_WR_PG_DIS) == 1);      //�ȴ���̽���
            while(FLASH_GetFlagStatus(FLASH_FLAG_EOP) == 1);            //�ȴ���̽���
            FLASH_Lock(FLASH_MemType_Data);      
            //return_flash(); // ����EEPROMֵ
			      uart_sendbyte(0x0A); // �ɹ�����Ӧ
			      uart_sendbyte(0x0B);
        }
		// ɾ������ע���豸����
		//-----------------------------------------------------------------------------------//
		else if (uart_buffer[2] == 0x0D)
	    {
		  //---------------------------------------------------------------------------------//
                //��flash��д���ַ����ַΪ3���ֽڣ����⻹��һ��������
                number=FLASH_ReadByte(NUMBER_ADDRESS);                                   
		        delete_low_address = uart_buffer[4]; // ɾ���豸��ַ ��λ
				delete_mid_address = uart_buffer[5]; // ��λ
				delete_high_address = uart_buffer[6];// ��λ
				
                for(i=0;i<number;i++)
                {
					temp_low_address = FLASH_ReadByte((DEVICE_ADDRESS)+6*i);  // ��ȡ��λ��ַ
					temp_mid_address = FLASH_ReadByte((DEVICE_ADDRESS)+6*i+1);  // ��ȡ��λ��ַ
					temp_high_address = FLASH_ReadByte((DEVICE_ADDRESS)+6*i+2); // ��ȡ��λ��ַ
					temp_type = FLASH_ReadByte((DEVICE_ADDRESS)+6*i+3);         // ��ȡ�豸����
					temp_pass_nnmber = FLASH_ReadByte((DEVICE_ADDRESS)+6*i+4);  // ��ȡ��·��
					temp_part_number = FLASH_ReadByte((DEVICE_ADDRESS)+6*i+5);  // ��ȡ�̼���
					
                    if((temp_low_address==delete_low_address)&&(temp_mid_address==delete_mid_address)&&(temp_high_address==temp_high_address))// ��ѯ��ɾ���豸
                    {
						if(number==1)// ֻ��һ���豸
						{
						  //-----------�׵�ַ��������----------------------------------------------//
							FLASH_Unlock(FLASH_MemType_Data);                    
							FLASH_ProgramByte((DEVICE_ADDRESS),0x00);// ��һ����������
							FLASH_ProgramByte((DEVICE_ADDRESS)+1,0x00);
							FLASH_ProgramByte((DEVICE_ADDRESS)+2,0x00);
							FLASH_ProgramByte((DEVICE_ADDRESS)+3,0x00);
							FLASH_ProgramByte((DEVICE_ADDRESS)+4,0x00);
							FLASH_ProgramByte((DEVICE_ADDRESS)+5,0x00);
                        
							number=0; // �豸����Ϊ0
							FLASH_ProgramByte(NUMBER_ADDRESS,number);// �����豸����
							while(FLASH_GetFlagStatus(FLASH_FLAG_WR_PG_DIS) == 1);      //�ȴ���̽���
							while(FLASH_GetFlagStatus(FLASH_FLAG_EOP) == 1);            //�ȴ���̽���
							FLASH_Lock(FLASH_MemType_Data); 
						
						}
						else // ����1���豸
						{
							//-----------��ȡĩβ�豸---------------------------------------------------------//
							last_low_address = FLASH_ReadByte((DEVICE_ADDRESS)+6*(number-1));  // ��ȡ��λ��ַ
							last_mid_address = FLASH_ReadByte((DEVICE_ADDRESS)+6*(number-1)+1);  // ��ȡ��λ��ַ
							last_high_address = FLASH_ReadByte((DEVICE_ADDRESS)+6*(number-1)+2); // ��ȡ��λ��ַ
							last_type = FLASH_ReadByte((DEVICE_ADDRESS)+6*(number-1)+3);         // ��ȡ�豸����
							last_pass_nnmber = FLASH_ReadByte((DEVICE_ADDRESS)+6*(number-1)+4);  // ��ȡ��·��
							last_part_number = FLASH_ReadByte((DEVICE_ADDRESS)+6*(number-1)+5);  // ��ȡ�̼���
						
							//-----------д��ĩβ�豸��ɾ���豸λ��----------------------------------------------//
							FLASH_Unlock(FLASH_MemType_Data); // д�뱻ɾ���豸λ��
						
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*i,last_low_address);// ����ĩβ����
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*i+1,last_mid_address);
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*i+2,last_high_address);
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*i+3,last_type);
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*i+4,last_pass_nnmber);
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*i+5,last_part_number);
						                       
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*(number-1),0x00);// ����ĩβ����
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*(number-1)+1,0x00);
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*(number-1)+2,0x00);
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*(number-1)+3,0x00);
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*(number-1)+4,0x00);
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*(number-1)+5,0x00);
                        
							number=number-1; // �豸������1
							FLASH_ProgramByte(NUMBER_ADDRESS,number);// �����豸����
							while(FLASH_GetFlagStatus(FLASH_FLAG_WR_PG_DIS) == 1);      //�ȴ���̽���
							while(FLASH_GetFlagStatus(FLASH_FLAG_EOP) == 1);            //�ȴ���̽���
							FLASH_Lock(FLASH_MemType_Data); 
						}
                        //����ɾ���ɹ�
                        delete_ok_flag=1;
                        old_address=0;             
                        break;
                    }
                }
                LEDR_OFF;
                LEDG_OFF;
                if(!delete_ok_flag)
                {
					uart_sendbyte(0x0F); // �ɹ�����Ӧ
					uart_sendbyte(0x0F);
                    //��ʾɾ��ʧ��
                    for(i=0;i<10;i++)
                    {
                        old_address=0;     
                        LEDG_TO;
                        LEDR_TO;
                        delay_ms(50);
                    }
                }
                else
                {
                    //��ʾɾ���ɹ�
					uart_sendbyte(0x0A); // �ɹ�����Ӧ
					uart_sendbyte(0x0B);
                    delete_ok_flag=0;                      
                    for(i=0;i<10;i++)
                    {
                        LEDG_TO;
                        delay_ms(500);
                        LEDR_TO;                               
                    }                  
                }
                Rf_ok_flag=0;
				LEDR_OFF;
				LEDG_OFF;
				LEDG_ON;  // ����ģʽD6��
            }
	   //-------------------------------------------------------------------------------------//	
    }
	//------------------Զ�̿��ƹ���------------------------------------------------------------//
	if ((uart_buffer[0] == 0x40) && (uart_buffer[1] == 0x40)&&(Normal_Mode_Flag==1))
		{
		   //----------���ⱨ����������----------------------------------------------------------//
		   if((uart_buffer[6]==0x91)&&(uart_buffer[7]==0x11))
			{
				for(i=0;i<10;i++) // RF�������ⱨ����������
				{ 
				   SendEv1527(0x11111,0x06,29); // ���ⱨ������
				}				
										        // Ӧ�������
			}
		   //----------���ⱨ���ر�����----------------------------------------------------------//
		   if((uart_buffer[6]==0x92)&&(uart_buffer[7]==0x22))
		   { 
			   for(i=0;i<10;i++) // RF�������ⱨ���ر�����
			   { 
				   SendEv1527(0x11111,0x09,29); // ���ⱨ���ر�
			   }			                                    // Ӧ������� 
		   }
		}
	
	
	//------------------������Ӧ��֡------------------------------------------------------------//
    memset(uart_buffer,0,uart_len);
    uart_len=0;
    uart_rec_flag=0;
}

// RTC����
void RTC_Config(void) 
{ 
    /* Configures the RTC */
    CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);
    CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);
    
    RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
    RTC_ITConfig(RTC_IT_WUT, ENABLE);
    
    /* Enable Interrupts*/
    enableInterrupts();
} 
/***
* ������ԣ��ٴΰ���ȡ����ԣ���Գɹ�ȡ����ԣ�˫��ɾ���������Ϣ
* �����������ݵ�ʱ�򶼷���ʲô���ݣ����������ݣ�
*
*/
void key_scanf(void)
{
	//---------------------���ü�------------------------------//
    if((KEY1_VAL==0)&&(READ_KEY1_STATE==0))
    {
        //��ʱȥ��
        key1_shake++;
        if(key1_shake>10)
        {
            key1_shake=0;
            SET_KEY1_STATE;
        }
    }
    else if(READ_KEY1_STATE)
    {
        if(KEY1_VAL==0)
        {
            if(key1_shake<2000)
                key1_shake++;
        }
        key1=KEY1_VAL;
        if(KEY1_VAL)
        {
            if(key1_shake>50&&key1_shake<=300)
            {
                SET_KEY1_SHORT;
            }
            else if(key1_shake>500)
            {
                SET_KEY1_LONG;
            }
            key1_shake=0;         
           CLEAR_KEY1_STATE;
        }
    }
    //------------------------�˳����ü�-----------------------------//
    if((KEY2_VAL==0)&&(READ_KEY2_STATE==0))
    {
        //��ʱȥ��
        key2_shake++;
        if(key2_shake>10)
        {
            key2_shake=0;
            SET_KEY2_STATE;
        }
    }
    else  if(READ_KEY2_STATE)
    {
        if(KEY2_VAL==0)
        {
            if(key2_shake<2000)
                key2_shake++;
        }
        if(KEY2_VAL)
        {
            if(key2_shake>50&&key2_shake<=300)
            {
                SET_KEY2_SHORT;
            }
            else if(key2_shake>500)
            {
                SET_KEY2_LONG;
            }
            key2_shake=0;   
            CLEAR_KEY2_STATE;
        }
    }
	//---------------------���ͼ�---------------------------------------//
	if((KEY3_VAL==0)&&(READ_KEY3_STATE==0))
    {
        //��ʱȥ��
        key3_shake++;
        if(key3_shake>10)
        {
            key3_shake=0;
            SET_KEY3_STATE;
        }
    }
    else if(READ_KEY3_STATE)
    {
        if(KEY3_VAL==0)
        {
            if(key3_shake<2000)
                key3_shake++;
        }
        key3=KEY3_VAL;
        if(KEY3_VAL)
        {
            if(key3_shake>50&&key3_shake<=300)
            {
                SET_KEY3_SHORT;
            }
            else if(key3_shake>500)
            {
                SET_KEY3_LONG;
            }
            key3_shake=0;         
			CLEAR_KEY3_STATE;
        }
    }
}

/*******************************************************************************
**�������ƣ�void IWDG_Init()
**������������ʼ���������Ź�
**��ڲ�������
**�������
*******************************************************************************/
void IWDG_Init()
{
  IWDG_SetReload(0xff);      //���ؼĴ���д��255
  //��д��ֵ ����д0XCC , ��д0X55
  IWDG_Enable();
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  IWDG_SetPrescaler(IWDG_Prescaler_256);   //64KHZ / 256 = 1.024S
}

/*******************************************************************************
**�������ƣ�void FeedTheIWDog(void)
**������������������Ź�ι������ֹϵͳ��λ
**��ڲ�������
**�������
*******************************************************************************/
void FeedTheIWDog()
{
  IWDG_ReloadCounter();
}

// ���ڷ��͵����ֽ�
void uart_sendbyte(unsigned char src)
{
    while(USART_GetFlagStatus(USART1 , USART_FLAG_TXE) == 0);       
    USART_SendData8(USART1,src);
    
}
// ����EEPROM
void error_errom(void)
{
    unsigned char i=0;
    FLASH_Unlock(FLASH_MemType_Data);
    for(i=0;i<256;i++)
    {
         FLASH_ProgramByte((DEVICE_ADDRESS)+i,0x00);
    }
    while(FLASH_GetFlagStatus(FLASH_FLAG_WR_PG_DIS) == 1);      //�ȴ���̽���
    while(FLASH_GetFlagStatus(FLASH_FLAG_EOP) == 1);            //�ȴ���̽���
}

// ������
void main(void)
{
    unsigned char register_ok_flag=0;
    unsigned char delete_ok_flag=0;
    unsigned char i=0;
    unsigned long int last_address=0;
	unsigned char k=0;
	
    KEY_GPIO_Init();
    RF_GPIO_Init();
    SYSTEM_Init();
    GPIO_Set();
    //RTC_Config();
    //��������
    for(i=0;i<5;i++)
    {
        LEDR_TO;      
        LEDG_TO;
        delay_ms(100);       
    }
	LEDG_OFF;
    LEDR_ON;
	LEDS_OFF;
	LEDL_OFF;
    /* WWDG configuration: WWDG is clocked by SYSCLK = 2MHz */
    /* WWDG timeout is equal to 251,9 ms */
    /* Watchdog Window = (COUNTER_INIT - 63) * 1 step
    = 41 * (12288 / 2Mhz)
    = 251,9 ms
    */
    /* Non Allowed Window = (COUNTER_INIT - WINDOW_VALUE) * 1 step
    = (104 - 97) * 1 step
    =  7 * 1 step 
    =  7 * (12288 / 2Mhz) 
    =  43.008 ms
    */
    address=0xE011B;
    data=0x05;
    //  WWDG_Init(80 , 0x7f);
   // error_errom();
    //
    IWDG_Init();        // ��������gou(Լ1.024�����������λ)
    //uart_sendbyte(0xAA); // ������ʾ��
   //Device_Count;
   
   //------------��ʼ������------------------------------------------------//
   Wen1_Enable_Flag=1;    // 2���¸������ͱ�־
   Yan1_Enable_Flag=1;    // 1���̸������ͱ�־
   Shou1_Enable_Flag=1;   // 3���ֱ������ͱ�־
   
   Wen1_Start_Flag=0;     // 1���¸п�ʼ��ʱ��־
   Yan1_Start_Flag=0;     // 1���̸п�ʼ��ʱ��־
   Shou1_Start_Flag=0;    // 3���ֱ������ͱ�־
   
   Wen1_Timer_Count=0;    // 2���¸��ٴη��ͼ�ʱ
   Yan1_Timer_Count=0;    // 1���̸��ٴη��ͼ�ʱ
   Shou1_Timer_Count=0;   // 3���ֱ��ٴη��ͼ�ʱ
   
    while(1)
    {
        //--------------���ڽ������ݴ���-------------------------------------//
        if(uart_rec_flag==1) 
        {
            receive_data(); // ���ڽ������ݴ���
            uart_rec_flag=0;// ���ڽ��ձ�־����
                       
        }
		
		//--------------��������--------------------------------------------//
		if(READ_KEY1_SHORT)// �������ü�
		{
			CLEAR_KEY1_SHORT;   // �������ü���־
			Set_Mode_Flag = 1;  // ����ģʽ 1
			Normal_Mode_Flag=0; // ��������ģʽ 0 
			LEDG_ON;            // D6 �� D5 Ϩ��
			LEDR_OFF;
			memset(uart_buffer,0,uart_len);// ��ս��ջ�����
			uart_len=0;
			uart_rec_flag=0;
		}
        
		if(READ_KEY2_SHORT)// �˳����ü�
	    {
			CLEAR_KEY2_SHORT;   // ����˳����ü���־
			Set_Mode_Flag = 0;  // ����ģʽ 0
			Normal_Mode_Flag=1; // ��������ģʽ 1 
			LEDG_OFF;           // D6 Ϩ�� D5 ��
			LEDR_ON;
			memset(uart_buffer,0,uart_len);// ��ս��ջ�����
			uart_len=0;
			uart_rec_flag=0;
		}
		if(READ_KEY3_SHORT)// ���ͼ� ����1�η���1�Σ�����һ��
	    {
           for(i=0;i<10;i++)
            { 
               SendEv1527(0x11111,0x0C,29);// ��������ע����
            }
			CLEAR_KEY3_SHORT;   // ������Ͱ�����־
			if(Set_Mode_Flag==1)
			{
				LEDG_ON;
			}
		}
	    //----------------RF���ձ���֡����----------------------------------------------------//
        if(Rf_ok_flag==1)// ���յ�����֡
        {   
			for(i=0;i<4;i++)              // ������˸��ʾ����ʹ�ö�ʱ����ʱ��
			{
				LEDS_TO;      
				delay_ms(50);       
			}
			LEDS_OFF; // ����������˸
           //------------------����ģʽ-------------------------------------------------------//
		   if(Set_Mode_Flag==1) // ����ע��ֱ�Ӵ�������豸��ַ
		   {
			   uart_sendbyte(0x02);
			   uart_sendbyte(0x03);
			   uart_sendbyte(0x04);
			   uart_sendbyte(RF_address);    // ��λ
			   uart_sendbyte(RF_address>>8); // ��λ
			   uart_sendbyte(RF_address>>16);// ��λ
			   uart_sendbyte(0xFF);
			   uart_sendbyte(RF_data);       // ����λ
			   uart_sendbyte(0x0d);
			   uart_sendbyte(0x0a);
			   Rf_ok_flag=0;                 // ������ձ�־
		   }
		   //------------------��������ģʽ----------------------------------------------------//
		   if(Normal_Mode_Flag==1)
		   {
			number=FLASH_ReadByte(NUMBER_ADDRESS);
            for(i=0;i<number;i++)
            {
                temp_address=FLASH_ReadByte((DEVICE_ADDRESS)+6*i); // ��λ
                temp_address|=(unsigned long int)FLASH_ReadByte((DEVICE_ADDRESS)+6*i+1)<<8; // ��λ
                temp_address|=(unsigned long int)FLASH_ReadByte((DEVICE_ADDRESS)+6*i+2)<<16;// ��λ
                if(RF_address==temp_address)
                {
                    address=RF_address; // ����RF���յ�ַ
                    data=RF_data;       // ����RF��������
					Flash_Query = i;    // ���汨���豸��Flash�е�λ��
                    send_ok=1;          // ���ý��յ��Ϸ���־                 
                    break;
                }
            }
			Rf_ok_flag=0;               // �Ƿ��豸
		   }
        }
		//-----------------GPRS��������֡����----------------------------------------------------//
        if(send_ok==1) // ���յ��Ϸ�����֡��������
        {
		   //-------------������������豸��ַ---------------------------------------//
		   /*
            uart_sendbyte(0x02);
            uart_sendbyte(0x03);
            uart_sendbyte(0x04);
            uart_sendbyte(address);    // ��λ
			uart_sendbyte(address>>8); // ��λ
			uart_sendbyte(address>>16);// ��λ
            uart_sendbyte(0xFF);
            uart_sendbyte(data);
            uart_sendbyte(0x0d);
            uart_sendbyte(0x0a);
			*/
		   //------------��װ���ݲ���[DTUЭ��]---------------------------------------//
		    Alarm_Data[0]=0x02; // �汾�š�0x02�������桿
			Alarm_Data[2]=FLASH_ReadByte((DEVICE_ADDRESS)+Flash_Query*6+4);// ��·�� 					  
			Alarm_Data[3]=FLASH_ReadByte((DEVICE_ADDRESS)+Flash_Query*6+5);// ������	
			k=FLASH_ReadByte((DEVICE_ADDRESS)+Flash_Query*6+3);            // �豸����
			switch(k)  // �豸����  �������Ŵ�=0x00  �������Ŵ�=0x01 ��λ�м�=02
			{  
				case 0x44:  // �¸б�����=0x03
				    if(data==0x09)          // 1001 ����֡  
					{
						//---�����¸б������ݰ�[��Ϣ]-----------------------//
						if((Link_Flag==1)&&(Wen1_Enable_Flag==1)) // �����ӷ��ͱ�������
						{ 
							DTU_send_data(0x1E,0x02);// ��������֡����
							Wen1_Enable_Flag=0;      // ���Ʒ�������
							Wen1_Start_Flag=1;       // ����5S�ٴη��Ͷ�ʱ
							Wen1_Timer_Count=0;      // �����ʱ����
						}
						else            // δ���ӣ����汨��֡MAX=10
						{
						}
					}
					else if(data==0x06)     // 0110 Ѳ��֡
					{
						                    // ��������---Ѳ��֡	
					}
					else                    // ���������ݣ�Ĭ��Ѳ��֡
					{
					}
				    break;
				case 0x55:  // �̸б�����=0x04 
				    if(data==0x09)          // 1001 ����֡  
					{
					  //---�����¸б������ݰ�[��Ϣ]-----------------------//
						if((Link_Flag==1)&&(Yan1_Enable_Flag==1)) // �����ӷ��ͱ�������
						{
							DTU_send_data(0x28,0x01);  // ��������֡����
							Yan1_Enable_Flag=0;        // ���Ʒ�������
							Yan1_Start_Flag=1;         // ����5S�ٴη��Ͷ�ʱ
							Yan1_Timer_Count=0;        // �����ʱ����
						}
						else            // δ���ӣ����汨��֡MAX=10
						{
						}
					}
					else if(data==0x0F)     // 0110 Ѳ��֡
					{	
					}
					else                    // ���������ݣ�Ĭ��Ѳ��֡
					{
					}
					break;
				case 0x66:  // �ֶ�������=0x66
				    if(data==0x09)          // 1001 ����֡  
					{
						//---�ֶ��������ݰ�[��Ϣ]-----------------------//
						if((Link_Flag==1)&&(Shou1_Enable_Flag==1)) // �����ӷ��ͱ�������
						{       
							DTU_send_data(0x17,0x03);// ��������֡����
							Shou1_Enable_Flag=0;      // ���Ʒ�������
							Shou1_Start_Flag=1;       // ����5S�ٴη��Ͷ�ʱ
							Shou1_Timer_Count=0;      // �����ʱ����
                            delay_ms(2000);           // ��ʱ2S
							//-------�������ⱨ��----------------------//
							for(i=0;i<100;i++) // RF�������ⱨ����������
							{ 
								SendEv1527(0x11111,0x06,29); // ���ⱨ������
                                FeedTheIWDog();              // ι��
							}				
						}
						else            // δ���ӣ����汨��֡MAX=10
						{
						}
					}
					else if(data==0x06)     // 0110 �ֶ������ָ�����
					{
                        delay_ms(2000);           // ��ʱ2S
						// �ֶ������ָ�ֹͣ����--------------------------//
					    for(i=0;i<100;i++) // RF�������ⱨ���ر�����
						{ 
							SendEv1527(0x11111,0x09,29); // ���ⱨ���ر�
							FeedTheIWDog();// ι��
						}                    	
					}
					else                    // ����������
					{
					}
				    break;
			    case 0x77:// ���ⱨ����
				    if(data==0x09)          // 1001 ע��֡  
					{
					}
					else if(data==0x06)     // 0110 Ѳ��֡
					{	
					}
					else                    // ���������ݣ�Ĭ��Ѳ��֡
					{
					}
					break;
				default:                                                                                                                              
					break;
			}
		   //------------�������ݷ���-----------------------------------------------//
            delay_ms(200);
            Rf_ok_flag=0;  // ����RF���ձ�־ 
            send_ok=0;     // ���㷢�ͱ�־
        }
	//----------------����������------------------------------------//
	if((Hart_Send_Flag==1)&&(Link_Flag==1))// ������,����������
	{
		Hart_Send_Flag=0;
		for(i=0;i<6;i++)
		{
		  uart_sendbyte(Hart_Data[i]);
		}
	}
	//----------------ι��-----------------------------------------//
    FeedTheIWDog();            //���ö������Ź���������ι��,��ֹϵͳ��λ
    }
}

// ��������֡����
void send_data(void)
{
	unsigned char ii = 0;
	//Header
	delay_ms(1);
	uart_sendbyte(0x40);
	uart_sendbyte(0x40);
	//DeviceType
	uart_sendbyte(0xFE);
	//Count
	uart_sendbyte(FrameCount & 0xFF);
	uart_sendbyte((FrameCount >> 8) & 0xFF);
	//Version
	uart_sendbyte(0x01);
	uart_sendbyte(0x10);
	//SN
	for (ii = 0; ii < 15; ii++)
		uart_sendbyte(IMEI_Number1[ii]);
	//IMEI
	for (ii = 0; ii < 15; ii++)
		uart_sendbyte(SN_Number1[ii]);
	//PhoneNum
	for (ii = 0; ii < 13; ii++)
		uart_sendbyte(PHONE_Number1[ii]);
	//Type  ����0x01
	uart_sendbyte(0x01);
	//encrypt δ����
	uart_sendbyte(0x00);
	//Length
	uart_sendbyte(0x06);
	uart_sendbyte(0x00);
	//Data ��������
	for(ii=0;ii<6;ii++)
    {
	  uart_sendbyte(Alarm_Data[ii]);
	}	
	//Reserve
	uart_sendbyte(0x00);
	uart_sendbyte(0x00);
	uart_sendbyte(0x00);
	uart_sendbyte(0x00);
	
	//checksum
	uart_sendbyte(0x00);
	//End
	uart_sendbyte(0x23);
	uart_sendbyte(0x23);
	FrameCount++;
}

// ����֡����---[��ѭ�û���Ϣ����װ��Э��]
// �����м�����
// �豸һ��1��·1���̸б�����
// �豸����1��·2���¸б�����
// Device_Type=0x1E:�¸б�����
// Device_Type=0x28:�̸б�����
void DTU_send_data(unsigned char Device_Type,unsigned char Channel)
{
	unsigned char ii = 0;
	// ������[2�ֽ�]
	delay_ms(1);
	uart_sendbyte(0x40);
	uart_sendbyte(0x40);
	/***���Ƶ�Ԫ*****************/ 
	// ҵ����ˮ��[2�ֽ�]
	uart_sendbyte(FrameCount & 0xFF);
	uart_sendbyte((FrameCount >> 8) & 0xFF);
	// Э��汾��[2�ֽ�]
	uart_sendbyte(0x01);
	uart_sendbyte(0x0a);
	// ʱ���ǩ[6�ֽ�]
	uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
	// Դ��ַ[6�ֽ�] 11 ed e3 9d ee 1e
	uart_sendbyte(0x33); // �м��������� 0x33
    uart_sendbyte(0x01);
    uart_sendbyte(0x01);
    uart_sendbyte(0x01);
    uart_sendbyte(0x01);
    uart_sendbyte(0x01);
	// Ŀ�ĵ�ַ[6�ֽ�] 00 00 00 00 00 00
	for (ii = 0; ii < 6; ii++)
		uart_sendbyte(0x00);
	// Ӧ�����ݵ�Ԫ����[2�ֽ�] 30 00
	uart_sendbyte(0x30);
	uart_sendbyte(0x00);
	// �����ֽ�[1�ֽ�]
	uart_sendbyte(0x02);
	/*******Ӧ�����ݵ�Ԫ***********/
	uart_sendbyte(0x02);// ���ͱ�ʶ 1�ֽ� �ϴ�����������ʩ��������״̬ ����
	uart_sendbyte(0x01);// ��Ϣ������Ŀ 01
	//------��Ϣ��----------------//
	uart_sendbyte(0x01);// ϵͳ���ͱ�ʶ 1�ֽ� ���ֱ���ϵͳ 01 
	uart_sendbyte(0x01);// ϵͳ��ַ 1�ֽ� ϵͳ��ַ=01
	uart_sendbyte(Device_Type);// �������� 1�ֽ� 0x1E �¸б����� 0x28 �̸б�����
	uart_sendbyte(0x01);// ������ַ 00 01 01 00  1��·1��
	uart_sendbyte(0x00);
	uart_sendbyte(Channel);// ������
	uart_sendbyte(0x00);
	uart_sendbyte(0x02);// ��
	uart_sendbyte(0x00);
	//------����˵��[31�ֽ�]-------//
	for(ii=0;ii<31;ii++)	
	{
	  uart_sendbyte(0x00);
	}
	//------ʱ���ǩ[6�ֽ�]--------//
	uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
	/********У���***************/
	// У���
	uart_sendbyte(0x00);
	/********������***************/
	// ������
	uart_sendbyte(0x23);
	uart_sendbyte(0x23);
	FrameCount++;
}

// ����֡����---[��ѭ�û���Ϣ����װ��Э��]
// �����м�����
// �豸һ��1��·1���̸б�����
// �豸����1��·1���¸б�����
// Device_Type=0x1E:�¸б�����
// Device_Type=0x28:�̸б�����
// ����У���
void DTU_send_data1(unsigned char Device_Type)
{
	unsigned char ii = 0;
	// ������[2�ֽ�]
	delay_ms(1);
	uart_sendbyte(0x40);
	uart_sendbyte(0x40);
	/***���Ƶ�Ԫ*****************/ 
	// ҵ����ˮ��[2�ֽ�]
	uart_sendbyte(FrameCount & 0xFF);
	uart_sendbyte((FrameCount >> 8) & 0xFF);
	// Э��汾��[2�ֽ�]
	uart_sendbyte(0x01);
	uart_sendbyte(0x0a);
	// ʱ���ǩ[6�ֽ�]
	uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
	// Դ��ַ[6�ֽ�] 11 ed e3 9d ee 1e
	uart_sendbyte(0x33); // �м��������� 0x33
    uart_sendbyte(0xed);
    uart_sendbyte(0xe3);
    uart_sendbyte(0x9d);
    uart_sendbyte(0xee);
    uart_sendbyte(0x1e);
	// Ŀ�ĵ�ַ[6�ֽ�] 00 00 00 00 00 00
	for (ii = 0; ii < 6; ii++)
		uart_sendbyte(0x00);
	// Ӧ�����ݵ�Ԫ����[2�ֽ�] 30 00
	uart_sendbyte(0x30);
	uart_sendbyte(0x00);
	// �����ֽ�[1�ֽ�]
	uart_sendbyte(0x02);
	/*******Ӧ�����ݵ�Ԫ***********/
	uart_sendbyte(0x02);// ���ͱ�ʶ 1�ֽ� �ϴ�����������ʩ��������״̬ ����
	uart_sendbyte(0x01);// ��Ϣ������Ŀ 01
	//------��Ϣ��----------------//
	uart_sendbyte(0x01);// ϵͳ���ͱ�ʶ 1�ֽ� ���ֱ���ϵͳ 01 
	uart_sendbyte(0x01);// ϵͳ��ַ 1�ֽ� ϵͳ��ַ=01
	uart_sendbyte(Device_Type);// �������� 1�ֽ� 0x1E �¸б����� 0x28 �̸б�����
	uart_sendbyte(0x00);// ������ַ 00 01 01 00  1��·1��
	uart_sendbyte(0x01);
	uart_sendbyte(0x01);
	uart_sendbyte(0x00);
	uart_sendbyte(0x02);// ��
	uart_sendbyte(0x00);
	//------����˵��[31�ֽ�]-------//
	for(ii=0;ii<31;ii++)	
	{
	  uart_sendbyte(0x00);
	}
	//------ʱ���ǩ[6�ֽ�]--------//
	uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
	/********У���***************/
	// У���
	uart_sendbyte(0x00);
	/********������***************/
	// ������
	uart_sendbyte(0x23);
	uart_sendbyte(0x23);
	FrameCount++;
}

/*****************************************************************************
* ��������: void SendData(void)
* ��������: USART1������������֡
* ��    ��: 
* ����  ֵ:
*****************************************************************************/
/*
void SendData2(void)
{
	unsigned int ii;
	unsigned char Data_Frame[500];
	unsigned int  Data_Count=0;
	unsigned char Check_Sum=0;
	
	#if ENCRYPT // ����
    #else       // δ����
	// Header
	Uart1SendByte(0x40);
	Uart1SendByte(0x40);
	// DeviceType
	Uart1SendByte(0x80); // ˮλ���޸�Ϊ0x80
	Data_Frame[Data_Count++]=0x80;
	// Count
	Uart1SendByte(FrameCount & 0xFF);
	Uart1SendByte((FrameCount >> 8) & 0xFF);
	Data_Frame[Data_Count++]=FrameCount & 0xFF;
    Data_Frame[Data_Count++]=(FrameCount >> 8) & 0xFF;
	// Version
	Uart1SendByte(0x01);
	Uart1SendByte(0x10);
	Data_Frame[Data_Count++]=0x01;
    Data_Frame[Data_Count++]=0x10;
	// SN
	for (ii = 0; ii < 15; ii++)
	{
		SN[ii]=*(unsigned char*)(SN_ADDRESS+ii);
		Uart1SendByte(SN[ii]);
		Data_Frame[Data_Count++] = SN[ii];
	}
	// IMEI
	for (ii = 0; ii < 15; ii++)
	{
		IMEI[ii]=*(unsigned char*)(IMEI_ADDRESS+ii);
		Uart1SendByte(IMEI[ii]);
		Data_Frame[Data_Count++] = IMEI[ii];
	}
	// PhoneNumber
	for (ii = 0; ii < 13; ii++)
	{
		PHONENUM[ii]=*(unsigned char*)(PHONE_ADDRESS+ii);
		Uart1SendByte(PHONENUM[ii]);
		Data_Frame[Data_Count++] = PHONENUM[ii];
	}
	// Type
	Uart1SendByte(1);
	Data_Frame[Data_Count++]=0x01;
	// Encrypt
	Uart1SendByte(0);
	Data_Frame[Data_Count++]=0x00;
	//--------------------------------------------------//
	// Length
	Uart1SendByte((Sending_Count*4)&0xff); // low 8 bit
	Uart1SendByte((Sending_Count*4)/256);  // high 8 bit
	Data_Frame[Data_Count++]=(Sending_Count*4)&0xff;
	Data_Frame[Data_Count++]=(Sending_Count*4)/256;
	// Data
	for (ii = 0; ii < Sending_Count; ii++)
	{
		Uart1SendByte(addata[ii*4]);
		Uart1SendByte(addata[ii*4+1]);
		Uart1SendByte(addata[ii*4+2]);
		Uart1SendByte(addata[ii*4+3]);
		Data_Frame[Data_Count++]=addata[ii*4];
		Data_Frame[Data_Count++]=addata[ii*4+1];
		Data_Frame[Data_Count++]=addata[ii*4+2];
		Data_Frame[Data_Count++]=addata[ii*4+3];
	}
	//--------------------------------------------------//
	// CheckSum
	Check_Sum=sum_check(Data_Frame, Data_Count);
	Uart1SendByte(Check_Sum);		
	// End
	Uart1SendByte(0x23);
	Uart1SendByte(0x23);
	
    #endif
}

*/










#if 0
//ע�Ṧ��
else if(READ_KEY2_SHORT)
{
	LEDR_ON;
	CLEAR_KEY2_SHORT;
	iwwd_time=0;
	while(Rf_ok_flag==0)
	{
		iwwd_time++;
		if(iwwd_time>5000)
		{
			iwwd_time=0;
			FeedTheIWDog();            //���ö������Ź���������ι��,��ֹϵͳ��λ
		}
		CLEAR_KEY2_LONG;
		if(READ_KEY2_SHORT)
		{
			CLEAR_KEY2_SHORT;
			break; 
		}
	}
	if(Rf_ok_flag==1)
	{
		//��flash��д���ַ����ַΪ3���ֽڣ����⻹��һ��������
		number=FLASH_ReadByte(NUMBER_ADDRESS);
		for(i=0;i<=number;i++)
		{
			// WWDG_Enable(80);
			temp_address=FLASH_ReadByte((DEVICE_ADDRESS)+4*i+1);
			temp_address|=(unsigned long int)FLASH_ReadByte((DEVICE_ADDRESS)+4*i+2)<<8;
			temp_address|=(unsigned long int)FLASH_ReadByte((DEVICE_ADDRESS)+4*i+3)<<16;
			if((FLASH_ReadByte((DEVICE_ADDRESS)+4*i)!=0x7F)&&(temp_address!=RF_address))
			{
				FLASH_Unlock(FLASH_MemType_Data);
				FLASH_ProgramByte((DEVICE_ADDRESS)+4*i,0x7F);
				FLASH_ProgramByte((DEVICE_ADDRESS)+4*i+1,RF_address);
				FLASH_ProgramByte((DEVICE_ADDRESS)+4*i+2,(unsigned char)(RF_address>>8));
				FLASH_ProgramByte((DEVICE_ADDRESS)+4*i+3,(unsigned char)(RF_address>>16));
				number=number+1;
				FLASH_ProgramByte(NUMBER_ADDRESS,number);
				while(FLASH_GetFlagStatus(FLASH_FLAG_WR_PG_DIS) == 1);      //�ȴ���̽���
				while(FLASH_GetFlagStatus(FLASH_FLAG_EOP) == 1);            //�ȴ���̽���
				FLASH_Lock(FLASH_MemType_Data);       
				//����ע��ɹ�
				register_ok_flag=1;             
				break;
			}
			else if((FLASH_ReadByte((DEVICE_ADDRESS)+4*i)==0x7F)&&(temp_address==RF_address))
			{
				break;
			}
		}
		LEDR_OFF;
		LEDG_OFF;
		if(!register_ok_flag)
		{
			//��ʾע��ʧ��
			for(i=0;i<10;i++)
			{
				old_address=0;     
				LEDG_TO;
				LEDR_TO;
				delay_ms(100);
			}
		}
		else
		{
			//��ʾע��ɹ�
			register_ok_flag=0;                      
			for(i=0;i<10;i++)
			{
				LEDG_TO;
				delay_ms(100);
				LEDR_TO;                               
			}                     
		}
		Rf_ok_flag=0;
	}              
	LEDR_OFF;
	LEDG_OFF;
}
//ɾ����ע����Ϣ
else if(READ_KEY2_LONG)
{
	CLEAR_KEY2_LONG;
	LEDR_ON;
	LEDG_ON;
	iwwd_time=0;
	while(Rf_ok_flag==0)
	{
		iwwd_time++;
		if(iwwd_time>5000)
		{
			iwwd_time=0;
			FeedTheIWDog();            //���ö������Ź���������ι��,��ֹϵͳ��λ
		}
		CLEAR_KEY2_SHORT;
		if(READ_KEY2_LONG)
		{
			CLEAR_KEY2_LONG;
			break;
		}
	}
	if(Rf_ok_flag==1)
	{
		//��flash��д���ַ����ַΪ3���ֽڣ����⻹��һ��������
		number=FLASH_ReadByte(NUMBER_ADDRESS);
		for(i=0;i<number;i++)
		{
			// WWDG_Enable(80);
			temp_address=FLASH_ReadByte((DEVICE_ADDRESS)+4*i+1);
			temp_address|=(unsigned long int)FLASH_ReadByte((DEVICE_ADDRESS)+4*i+2)<<8;
			temp_address|=(unsigned long int)FLASH_ReadByte((DEVICE_ADDRESS)+4*i+3)<<16;
			
			last_address=FLASH_ReadByte((DEVICE_ADDRESS)+4*(number-1)+1);
			last_address|=(unsigned long int)FLASH_ReadByte((DEVICE_ADDRESS)+4*(number-1)+2)<<8;
			last_address|=(unsigned long int)FLASH_ReadByte((DEVICE_ADDRESS)+4*(number-1)+3)<<16;
			
			if((FLASH_ReadByte((DEVICE_ADDRESS)+4*i)==0x7F)&&(temp_address==RF_address))
			{
				FLASH_Unlock(FLASH_MemType_Data);
				FLASH_ProgramByte((DEVICE_ADDRESS)+4*i,0x7F);
				FLASH_ProgramByte((DEVICE_ADDRESS)+4*i+1,last_address);
				FLASH_ProgramByte((DEVICE_ADDRESS)+4*i+2,(unsigned char)(last_address>>8));
				FLASH_ProgramByte((DEVICE_ADDRESS)+4*i+3,(unsigned char)(last_address>>16));
				
				FLASH_ProgramByte((DEVICE_ADDRESS)+4*(number-1),0x00);
				FLASH_ProgramByte((DEVICE_ADDRESS)+4*(number-1)+1,0x00);
				FLASH_ProgramByte((DEVICE_ADDRESS)+4*(number-1)+2,0x00);
				FLASH_ProgramByte((DEVICE_ADDRESS)+4*(number-1)+3,0x00);
				
				number=number-1;
				FLASH_ProgramByte(NUMBER_ADDRESS,number);
				while(FLASH_GetFlagStatus(FLASH_FLAG_WR_PG_DIS) == 1);      //�ȴ���̽���
				while(FLASH_GetFlagStatus(FLASH_FLAG_EOP) == 1);            //�ȴ���̽���
				FLASH_Lock(FLASH_MemType_Data);       
				//����ɾ���ɹ�
				delete_ok_flag=1;
				old_address=0;             
				break;
			}
		}
		LEDR_OFF;
		LEDG_OFF;
		if(!delete_ok_flag)
		{
			//��ʾɾ��ʧ��
			for(i=0;i<10;i++)
			{
				old_address=0;     
				LEDG_TO;
				LEDR_TO;
				delay_ms(100);
			}
		}
		else
		{
			//��ʾɾ���ɹ�
			delete_ok_flag=0;                      
			for(i=0;i<10;i++)
			{
				LEDG_TO;
				delay_ms(100);
				LEDR_TO;                               
			}                  
		}
		Rf_ok_flag=0;
	}
	LEDR_OFF;
	LEDG_OFF;
}
#endif

//--------------------------------------------------------------------------------//
/**
* @brief USART1 RX / Timer5 Capture/Compare Interrupt routine.
* @param  None
* @retval None
*/
INTERRUPT_HANDLER(USART1_RX_TIM5_CC_IRQHandler,28)
{
    /* In order to detect unexpected events during development,
    it is recommended to set a breakpoint on the following instruction.
    */
    uchar Res;
    if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
    {
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);   //����жϱ�־λ
        Res = USART_ReceiveData8(USART1);  //(USART1->DR);//��ȡ���յ�������
		//---------����ģʽ------------------------------------------------//
		if(Set_Mode_Flag==1)
	    {
			if(uart_len<50)
			{
				uart_buffer[uart_len++]=Res;
			}
			else
			{
				uart_len=0;
			}
			if ((uart_buffer[uart_len - 2] == 0x0d) && (uart_buffer[uart_len - 1] == 0x0a)&&(uart_len>2))
			{
				uart_rec_flag=1;  
			}
		}
		//-------��������ģʽ------------------------------------------------//
		if(Normal_Mode_Flag==1)
	    {
			if(uart_buffer[0]==0x40) // ���յ�֡ͷ
			{
			   uart_buffer[uart_len++]=Res;// �������������ֽ�
			}
			else  // δ��⵽֡ͷ
			{
				if(Res==0x40)// ��Ϊ֡ͷ����֡ͷ
				{
				  uart_buffer[0]=Res;
				  uart_len++;
				}
			}
			if ((uart_buffer[uart_len - 2] == 0x23) && (uart_buffer[uart_len - 1] == 0x23)&&(uart_len>2))
			{
				uart_rec_flag=1;  // ���յ�һ֡
                 uart_sendbyte(0x02);
			}
			if(uart_len>=50)
			{
			    uart_len=0;
			}
		}	
    } 
}

//---------------------------------------------------------------------------------//















