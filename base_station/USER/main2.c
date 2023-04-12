/***********************************************************************/
//         中继网关MINI版 V1.0
// 公司：富邦科技
// 时间：2017-5-30
/***********************************************************************/
// 网关地址：0x11111（超再生地址）

/*****************头文件*************************************************/
#include "include.h"
#include "string.h"

/*****************全局变量***********************************************/
//配置信息
unsigned long int address=0xE011B;
unsigned char data=0x05;

unsigned char key1=0;
unsigned char key2=0;
unsigned char key3=0;

unsigned char number=0;
unsigned char send_ok=0;
unsigned long int old_address=0;
unsigned long temp_address=0;

// 按键定义
unsigned int  key1_shake=0;
unsigned int  key2_shake=0;
unsigned int  key3_shake=0;
unsigned char key1_value=0;
unsigned char key2_value=0;
unsigned char key3_value=0;

// GPRS 通信
unsigned char Link_Flag=0;      // 连接标志 1：连接 0：未连接
unsigned int  Hart_Time=0;      // 心跳发送时间计数（30S）
unsigned char Hart_Send_Flag=0; // 心跳发送标志 1: send
unsigned char Hart_Data[7]={0x33,0x01,0x01,0x01,0x01,0x01};
unsigned char IMEI_Number1[16]={'3','5','6','5','6','6','0','7','0','7','1','8','8','3','8'};  // IMEI号[1]
unsigned char IMEI_Number2[16]={'3','5','6','5','6','6','0','7','0','7','1','5','0','6','7'};
unsigned char SN_Number1[16]={'0','1','1','2','0','1','6','0','6','0','3','6','6','5','3'};    // SN码
unsigned char PHONE_Number1[16]={'1','0','6','4','8','3','2','1','7','2','9','3','4'};         // TEL电话号码
unsigned short FrameCount = 0;// 帧计数器

// 模式设置
unsigned char Set_Mode_Flag = 0;  // 设置模式 1
unsigned char Normal_Mode_Flag=1; // 正常工作模式 1

// 设备注册
unsigned char Device_Count=0;    // 已注册设备个数

// EEPROM存储
unsigned char Flash_Query=0;     // 报警设备信息所在地址

// 报警帧数据部分[DTU协议]
unsigned char Alarm_Data[10];    // 报警帧数据部分

// 时间等待
unsigned int iwwd_time=0;

// 配置注册命令
unsigned char delete_low_address = 0; // 删除设备地址 低位
unsigned char delete_mid_address = 0; // 中位
unsigned char delete_high_address = 0;// 高位

unsigned char temp_low_address = 0;  // 读取低位地址
unsigned char temp_mid_address = 0;  // 读取中位地址
unsigned char temp_high_address = 0; // 读取高位地址
unsigned char temp_type = 0;         // 读取设备类型
unsigned char temp_pass_nnmber = 0;  // 读取回路号
unsigned char temp_part_number = 0;  // 读取固件号

unsigned char last_low_address = 0;  // 读取低位地址
unsigned char last_mid_address = 0;  // 读取中位地址
unsigned char last_high_address = 0; // 读取高位地址
unsigned char last_type = 0;         // 读取设备类型
unsigned char last_pass_nnmber = 0;  // 读取回路号
unsigned char last_part_number = 0;  // 读取固件号

unsigned char delete_ok_flag=0;      // 删除成功标志

// RF无线接收数据处理
unsigned char RF_First_Data[5];

/*******************数据处理*************************************************/
unsigned char Wen1_Enable_Flag=1;    // 2号温感允许发送标志
unsigned char Yan1_Enable_Flag=1;    // 1号烟感允许发送标志 
unsigned char Shou1_Enable_Flag=1;   // 3号手报允许发送标志

unsigned char Wen1_Start_Flag=0;     // 2号温感开始计时标志
unsigned char Yan1_Start_Flag=0;     // 1号烟感开始计时标志
unsigned char Shou1_Start_Flag=0;    // 3号手报开始计时标志r

unsigned int  Wen1_Timer_Count=0;    // 2号温感再次发送计时
unsigned int  Yan1_Timer_Count=0;    // 1号烟感再次发送计时
unsigned int  Shou1_Timer_Count=0;   // 3号手报再次发送计时


/*****************宏定义****************************************************/
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

// 清除按键状态，确保只有一种按键状态
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


/*************函数声明****************************************************/
void send_data(void); 
void uart_sendbyte(unsigned char src);
void DTU_send_data(unsigned char Device_Type,unsigned char Channel);
void FeedTheIWDog();

/*************函数原型****************************************************/
void delay_ms(unsigned int time)
{
    unsigned int i=0;
    for(i=0;i<time;i++)
    {
        delay_us(100);
       //WWDG_Enable(80);
	   FeedTheIWDog();// 喂狗
    }
}

void delay(unsigned int nCount)
{
    for (; nCount != 0; nCount--);
}

// 返回Flash设置注册数据
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

// 按键初始化
void KEY_GPIO_Init(void)
{
    GPIO_Init(KEY1_GPIO_PORT,KEY1_GPIO_PINS,GPIO_Mode_In_PU_No_IT); // KEY1 不带中断的上拉输入模式
    //  EXTI_SetPortSensitivity(EXTI_Port_B ,  EXTI_Trigger_Rising);  //PortB端口为上升沿出发中断
    //  EXTI_SetHalfPortSelection(EXTI_HalfPort_B_LSB , ENABLE);       //设置PB[3:0]作为外部通用中断
    GPIO_Init(KEY2_GPIO_PORT,KEY2_GPIO_PINS,GPIO_Mode_In_PU_No_IT); // KEY2 不带中断的上拉输入模式
	GPIO_Init(KEY3_GPIO_PORT,KEY3_GPIO_PINS,GPIO_Mode_In_PU_No_IT); // KEY3 不带中断的上拉输入模式
}
 
//关闭不需要的IO口
void HALT_Off_GPIO(void)  
{  
    //IO配置为输出  
    GPIO_Init(GPIOA, GPIO_Pin_0|GPIO_Pin_2 | GPIO_Pin_3, GPIO_Mode_Out_PP_High_Fast);   
    GPIO_Init(GPIOB, GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7, GPIO_Mode_Out_PP_High_Fast);   
    GPIO_Init(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast);   
    GPIO_Init(GPIOD, GPIO_Pin_0 , GPIO_Mode_Out_PP_High_Fast);   
    
    //IO配置为输出低（注意低电平使能的得设置为输出高）  
    GPIO_ResetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_2 | GPIO_Pin_3);
    GPIO_ResetBits(GPIOB,GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
    GPIO_ResetBits(GPIOC,GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_4);
    GPIO_ResetBits(GPIOD,GPIO_Pin_0);
    
    //外部晶振接口设置为输入模式
    GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_In_PU_No_IT);   
    
    //NRST引脚
    //  GPIO_Init(GPIOA, GPIO_Pin_1 | GPIO_Pin_6, GPIO_Mode_In_PU_No_IT);   
    
} 

// GPIO初始化
void GPIO_Set(void)
{
    GPIO_Init(LEDR_GPIO_PORT,LEDR_GPIO_PINS,GPIO_Mode_Out_PP_Low_Fast);
    GPIO_Init(LEDG_GPIO_PORT,LEDG_GPIO_PINS,GPIO_Mode_Out_PP_Low_Fast);
	GPIO_Init(LEDS_GPIO_PORT,LEDS_GPIO_PINS,GPIO_Mode_Out_PP_Low_Fast);
	GPIO_Init(LEDL_GPIO_PORT,LEDL_GPIO_PINS,GPIO_Mode_Out_PP_Low_Fast);
	
	//---------GPRS通信---------------------------------------------------//
    GPIO_Init(ST_GPIO_PORT,ST_GPIO_PINS,GPIO_Mode_In_PU_No_IT);       // ST    不带中断的上拉输入模式
	GPIO_Init(LINKA_GPIO_PORT,LINKA_GPIO_PINS,GPIO_Mode_In_PU_No_IT); // LINKA 不带中断的上拉输入模式
}

//关闭时钟外设
void HALT_Off_Clkout(void)  
{  
    //时钟失能  
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, DISABLE);  
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, DISABLE);  
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, DISABLE);  
    CLK_PeripheralClockConfig(CLK_Peripheral_USART1, DISABLE);  
} 

// 时钟设置
void HALT_Clk(void)  
{  
    //寄存器重置   
    CLK_DeInit();                
    //时钟8分频，2MHz  
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_8); 
}  

// 1、串口配置功能
// 2、远程控制功能
// 3、服务器应答
void receive_data(void)
{
    unsigned char i = 0;
	unsigned char num=0;
    unsigned int k1=0;
	
	//--------------配置模式注册数据录入-------------------------------------------//
    //串口配置
	if ((uart_buffer[0] == 0x07) && (uart_buffer[1] == 0xf0)&&(Set_Mode_Flag==1))
    { 
		// 查询所有设备信息命令
        if (uart_buffer[2] == 0x06)
        {
			// 这里可以写个函数
			number=FLASH_ReadByte(NUMBER_ADDRESS);    
		    for(i=0;i<number;i++)
			{
				temp_low_address = FLASH_ReadByte((DEVICE_ADDRESS)+6*i);  // 读取低位地址
				temp_mid_address = FLASH_ReadByte((DEVICE_ADDRESS)+6*i+1);  // 读取中位地址
				temp_high_address = FLASH_ReadByte((DEVICE_ADDRESS)+6*i+2); // 读取高位地址
				temp_type = FLASH_ReadByte((DEVICE_ADDRESS)+6*i+3);         // 读取设备类型
				temp_pass_nnmber = FLASH_ReadByte((DEVICE_ADDRESS)+6*i+4);  // 读取回路号
				temp_part_number = FLASH_ReadByte((DEVICE_ADDRESS)+6*i+5);  // 读取固件号
				
				uart_sendbyte(temp_low_address); // 串口输出所有注册设备
				uart_sendbyte(temp_mid_address);
				uart_sendbyte(temp_high_address);
				uart_sendbyte(temp_type);
				uart_sendbyte(temp_pass_nnmber);
				uart_sendbyte(temp_part_number);
				delay_ms(1000); // 延时1S等待帧换行
			}
            //return_flash();
        }
        // 注册设备配置命令
        else if (uart_buffer[2] == 0x05)
        {
            num=FLASH_ReadByte(NUMBER_ADDRESS);             // 读取已注册设备数据
			num++;
			FLASH_SetProgrammingTime(FLASH_ProgramTime_TProg); //设定编程时间为标准编程时间
			FLASH_Unlock(FLASH_MemType_Data);
			FLASH_ProgramByte(NUMBER_ADDRESS,num);             // 增加新注册设备个数
			while(FLASH_GetFlagStatus(FLASH_FLAG_WR_PG_DIS) == 1);    // 等待编程结束
			while(FLASH_GetFlagStatus(FLASH_FLAG_EOP) == 1);          // 等待编程结束
			//-----------------------------------------------------------------------------------//
			if(num<=42)// MCU最大256B字节 6字节记录一个设备
			{
				//写入注册设备信息数据
				FLASH_SetProgrammingTime(FLASH_ProgramTime_TProg); //设定编程时间为标准编程时间
				FLASH_Unlock(FLASH_MemType_Data);
				FLASH_ProgramByte(DEVICE_ADDRESS+(num-1)*6,uart_buffer[4]);   // 地址低位
				FLASH_ProgramByte(DEVICE_ADDRESS+(num-1)*6+1,uart_buffer[5]); // 地址中位
				FLASH_ProgramByte(DEVICE_ADDRESS+(num-1)*6+2,uart_buffer[6]); // 地址高位
				FLASH_ProgramByte(DEVICE_ADDRESS+(num-1)*6+3,uart_buffer[3]); // 设备类型
				FLASH_ProgramByte(DEVICE_ADDRESS+(num-1)*6+4,uart_buffer[7]); // 回路号
				FLASH_ProgramByte(DEVICE_ADDRESS+(num-1)*6+5,uart_buffer[8]); // 部件号                                                     
				while(FLASH_GetFlagStatus(FLASH_FLAG_WR_PG_DIS) == 1);    // 等待编程结束
				while(FLASH_GetFlagStatus(FLASH_FLAG_EOP) == 1);          // 等待编程结束
				FLASH_Lock(FLASH_MemType_Data);       
				//return_flash(); // 返回EEPROM值
				uart_sendbyte(0x0A); // 注册成功，响应
				uart_sendbyte(0x0B);
			}
			else // 设备已满
			{
			}
        }
        // 删除所有设备命令
        else if(uart_buffer[2]==0x0F)
        {              
            FLASH_SetProgrammingTime(FLASH_ProgramTime_TProg); //设定编程时间为标准编程时间
            FLASH_Unlock(FLASH_MemType_Data);
            for(k1=0;k1<256;k1++)
            {
                FLASH_EraseByte(DEVICE_ADDRESS+k1);
            }           
            while(FLASH_GetFlagStatus(FLASH_FLAG_WR_PG_DIS) == 1);      //等待编程结束
            while(FLASH_GetFlagStatus(FLASH_FLAG_EOP) == 1);            //等待编程结束
            FLASH_Lock(FLASH_MemType_Data);      
            //return_flash(); // 返回EEPROM值
			      uart_sendbyte(0x0A); // 成功，响应
			      uart_sendbyte(0x0B);
        }
		// 删除单个注册设备命令
		//-----------------------------------------------------------------------------------//
		else if (uart_buffer[2] == 0x0D)
	    {
		  //---------------------------------------------------------------------------------//
                //向flash中写入地址，地址为3个字节，另外还有一个计数器
                number=FLASH_ReadByte(NUMBER_ADDRESS);                                   
		        delete_low_address = uart_buffer[4]; // 删除设备地址 低位
				delete_mid_address = uart_buffer[5]; // 中位
				delete_high_address = uart_buffer[6];// 高位
				
                for(i=0;i<number;i++)
                {
					temp_low_address = FLASH_ReadByte((DEVICE_ADDRESS)+6*i);  // 读取低位地址
					temp_mid_address = FLASH_ReadByte((DEVICE_ADDRESS)+6*i+1);  // 读取中位地址
					temp_high_address = FLASH_ReadByte((DEVICE_ADDRESS)+6*i+2); // 读取高位地址
					temp_type = FLASH_ReadByte((DEVICE_ADDRESS)+6*i+3);         // 读取设备类型
					temp_pass_nnmber = FLASH_ReadByte((DEVICE_ADDRESS)+6*i+4);  // 读取回路号
					temp_part_number = FLASH_ReadByte((DEVICE_ADDRESS)+6*i+5);  // 读取固件号
					
                    if((temp_low_address==delete_low_address)&&(temp_mid_address==delete_mid_address)&&(temp_high_address==temp_high_address))// 查询到删除设备
                    {
						if(number==1)// 只有一个设备
						{
						  //-----------首地址数据清零----------------------------------------------//
							FLASH_Unlock(FLASH_MemType_Data);                    
							FLASH_ProgramByte((DEVICE_ADDRESS),0x00);// 第一个数据清零
							FLASH_ProgramByte((DEVICE_ADDRESS)+1,0x00);
							FLASH_ProgramByte((DEVICE_ADDRESS)+2,0x00);
							FLASH_ProgramByte((DEVICE_ADDRESS)+3,0x00);
							FLASH_ProgramByte((DEVICE_ADDRESS)+4,0x00);
							FLASH_ProgramByte((DEVICE_ADDRESS)+5,0x00);
                        
							number=0; // 设备个数为0
							FLASH_ProgramByte(NUMBER_ADDRESS,number);// 更新设备个数
							while(FLASH_GetFlagStatus(FLASH_FLAG_WR_PG_DIS) == 1);      //等待编程结束
							while(FLASH_GetFlagStatus(FLASH_FLAG_EOP) == 1);            //等待编程结束
							FLASH_Lock(FLASH_MemType_Data); 
						
						}
						else // 大于1个设备
						{
							//-----------读取末尾设备---------------------------------------------------------//
							last_low_address = FLASH_ReadByte((DEVICE_ADDRESS)+6*(number-1));  // 读取低位地址
							last_mid_address = FLASH_ReadByte((DEVICE_ADDRESS)+6*(number-1)+1);  // 读取中位地址
							last_high_address = FLASH_ReadByte((DEVICE_ADDRESS)+6*(number-1)+2); // 读取高位地址
							last_type = FLASH_ReadByte((DEVICE_ADDRESS)+6*(number-1)+3);         // 读取设备类型
							last_pass_nnmber = FLASH_ReadByte((DEVICE_ADDRESS)+6*(number-1)+4);  // 读取回路号
							last_part_number = FLASH_ReadByte((DEVICE_ADDRESS)+6*(number-1)+5);  // 读取固件号
						
							//-----------写入末尾设备到删除设备位置----------------------------------------------//
							FLASH_Unlock(FLASH_MemType_Data); // 写入被删除设备位置
						
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*i,last_low_address);// 清零末尾数据
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*i+1,last_mid_address);
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*i+2,last_high_address);
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*i+3,last_type);
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*i+4,last_pass_nnmber);
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*i+5,last_part_number);
						                       
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*(number-1),0x00);// 清零末尾数据
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*(number-1)+1,0x00);
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*(number-1)+2,0x00);
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*(number-1)+3,0x00);
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*(number-1)+4,0x00);
							FLASH_ProgramByte((DEVICE_ADDRESS)+6*(number-1)+5,0x00);
                        
							number=number-1; // 设备个数减1
							FLASH_ProgramByte(NUMBER_ADDRESS,number);// 更新设备个数
							while(FLASH_GetFlagStatus(FLASH_FLAG_WR_PG_DIS) == 1);      //等待编程结束
							while(FLASH_GetFlagStatus(FLASH_FLAG_EOP) == 1);            //等待编程结束
							FLASH_Lock(FLASH_MemType_Data); 
						}
                        //返回删除成功
                        delete_ok_flag=1;
                        old_address=0;             
                        break;
                    }
                }
                LEDR_OFF;
                LEDG_OFF;
                if(!delete_ok_flag)
                {
					uart_sendbyte(0x0F); // 成功，响应
					uart_sendbyte(0x0F);
                    //提示删除失败
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
                    //提示删除成功
					uart_sendbyte(0x0A); // 成功，响应
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
				LEDG_ON;  // 配置模式D6亮
            }
	   //-------------------------------------------------------------------------------------//	
    }
	//------------------远程控制功能------------------------------------------------------------//
	if ((uart_buffer[0] == 0x40) && (uart_buffer[1] == 0x40)&&(Normal_Mode_Flag==1))
		{
		   //----------声光报警开启命令----------------------------------------------------------//
		   if((uart_buffer[6]==0x91)&&(uart_buffer[7]==0x11))
			{
				for(i=0;i<10;i++) // RF发送声光报警开启命令
				{ 
				   SendEv1527(0x11111,0x06,29); // 声光报警开启
				}				
										        // 应答服务器
			}
		   //----------声光报警关闭命令----------------------------------------------------------//
		   if((uart_buffer[6]==0x92)&&(uart_buffer[7]==0x22))
		   { 
			   for(i=0;i<10;i++) // RF发送声光报警关闭命令
			   { 
				   SendEv1527(0x11111,0x09,29); // 声光报警关闭
			   }			                                    // 应答服务器 
		   }
		}
	
	
	//------------------服务器应答帧------------------------------------------------------------//
    memset(uart_buffer,0,uart_len);
    uart_len=0;
    uart_rec_flag=0;
}

// RTC设置
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
* 单击配对，再次按键取消配对，配对成功取消配对，双击删除已配对信息
* 单击发送数据的时候都发送什么数据（不发送数据）
*
*/
void key_scanf(void)
{
	//---------------------配置键------------------------------//
    if((KEY1_VAL==0)&&(READ_KEY1_STATE==0))
    {
        //延时去抖
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
    //------------------------退出配置键-----------------------------//
    if((KEY2_VAL==0)&&(READ_KEY2_STATE==0))
    {
        //延时去抖
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
	//---------------------发送键---------------------------------------//
	if((KEY3_VAL==0)&&(READ_KEY3_STATE==0))
    {
        //延时去抖
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
**函数名称：void IWDG_Init()
**功能描述：初始化独立看门狗
**入口参数：无
**输出：无
*******************************************************************************/
void IWDG_Init()
{
  IWDG_SetReload(0xff);      //重载寄存器写入255
  //先写键值 ，先写0XCC , 后写0X55
  IWDG_Enable();
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  IWDG_SetPrescaler(IWDG_Prescaler_256);   //64KHZ / 256 = 1.024S
}

/*******************************************************************************
**函数名称：void FeedTheIWDog(void)
**功能描述：向独立看门狗喂狗，防止系统复位
**入口参数：无
**输出：无
*******************************************************************************/
void FeedTheIWDog()
{
  IWDG_ReloadCounter();
}

// 串口发送单个字节
void uart_sendbyte(unsigned char src)
{
    while(USART_GetFlagStatus(USART1 , USART_FLAG_TXE) == 0);       
    USART_SendData8(USART1,src);
    
}
// 擦除EEPROM
void error_errom(void)
{
    unsigned char i=0;
    FLASH_Unlock(FLASH_MemType_Data);
    for(i=0;i<256;i++)
    {
         FLASH_ProgramByte((DEVICE_ADDRESS)+i,0x00);
    }
    while(FLASH_GetFlagStatus(FLASH_FLAG_WR_PG_DIS) == 1);      //等待编程结束
    while(FLASH_GetFlagStatus(FLASH_FLAG_EOP) == 1);            //等待编程结束
}

// 主函数
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
    //开机闪灯
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
    IWDG_Init();        // 独立看门gou(约1.024秒后则会产生复位)
    //uart_sendbyte(0xAA); // 开机提示符
   //Device_Count;
   
   //------------初始化变量------------------------------------------------//
   Wen1_Enable_Flag=1;    // 2号温感允许发送标志
   Yan1_Enable_Flag=1;    // 1号烟感允许发送标志
   Shou1_Enable_Flag=1;   // 3号手报允许发送标志
   
   Wen1_Start_Flag=0;     // 1号温感开始计时标志
   Yan1_Start_Flag=0;     // 1号烟感开始计时标志
   Shou1_Start_Flag=0;    // 3号手报允许发送标志
   
   Wen1_Timer_Count=0;    // 2号温感再次发送计时
   Yan1_Timer_Count=0;    // 1号烟感再次发送计时
   Shou1_Timer_Count=0;   // 3号手报再次发送计时
   
    while(1)
    {
        //--------------串口接收数据处理-------------------------------------//
        if(uart_rec_flag==1) 
        {
            receive_data(); // 串口接收数据处理
            uart_rec_flag=0;// 串口接收标志清零
                       
        }
		
		//--------------按键功能--------------------------------------------//
		if(READ_KEY1_SHORT)// 进入配置键
		{
			CLEAR_KEY1_SHORT;   // 清零配置键标志
			Set_Mode_Flag = 1;  // 设置模式 1
			Normal_Mode_Flag=0; // 正常工作模式 0 
			LEDG_ON;            // D6 亮 D5 熄灭
			LEDR_OFF;
			memset(uart_buffer,0,uart_len);// 清空接收缓冲区
			uart_len=0;
			uart_rec_flag=0;
		}
        
		if(READ_KEY2_SHORT)// 退出配置键
	    {
			CLEAR_KEY2_SHORT;   // 清除退出配置键标志
			Set_Mode_Flag = 0;  // 设置模式 0
			Normal_Mode_Flag=1; // 正常工作模式 1 
			LEDG_OFF;           // D6 熄灭 D5 亮
			LEDR_ON;
			memset(uart_buffer,0,uart_len);// 清空接收缓冲区
			uart_len=0;
			uart_rec_flag=0;
		}
		if(READ_KEY3_SHORT)// 发送键 按键1次发送1次，灯亮一次
	    {
           for(i=0;i<10;i++)
            { 
               SendEv1527(0x11111,0x0C,29);// 发送主机注册码
            }
			CLEAR_KEY3_SHORT;   // 清除发送按键标志
			if(Set_Mode_Flag==1)
			{
				LEDG_ON;
			}
		}
	    //----------------RF接收报警帧处理----------------------------------------------------//
        if(Rf_ok_flag==1)// 接收到报警帧
        {   
			for(i=0;i<4;i++)              // 接收闪烁提示（可使用定时器延时）
			{
				LEDS_TO;      
				delay_ms(50);       
			}
			LEDS_OFF; // 接收数据闪烁
           //------------------配置模式-------------------------------------------------------//
		   if(Set_Mode_Flag==1) // 对码注册直接串口输出设备地址
		   {
			   uart_sendbyte(0x02);
			   uart_sendbyte(0x03);
			   uart_sendbyte(0x04);
			   uart_sendbyte(RF_address);    // 低位
			   uart_sendbyte(RF_address>>8); // 中位
			   uart_sendbyte(RF_address>>16);// 高位
			   uart_sendbyte(0xFF);
			   uart_sendbyte(RF_data);       // 数据位
			   uart_sendbyte(0x0d);
			   uart_sendbyte(0x0a);
			   Rf_ok_flag=0;                 // 清零接收标志
		   }
		   //------------------正常工作模式----------------------------------------------------//
		   if(Normal_Mode_Flag==1)
		   {
			number=FLASH_ReadByte(NUMBER_ADDRESS);
            for(i=0;i<number;i++)
            {
                temp_address=FLASH_ReadByte((DEVICE_ADDRESS)+6*i); // 低位
                temp_address|=(unsigned long int)FLASH_ReadByte((DEVICE_ADDRESS)+6*i+1)<<8; // 中位
                temp_address|=(unsigned long int)FLASH_ReadByte((DEVICE_ADDRESS)+6*i+2)<<16;// 高位
                if(RF_address==temp_address)
                {
                    address=RF_address; // 保存RF接收地址
                    data=RF_data;       // 保存RF接收数据
					Flash_Query = i;    // 保存报警设备在Flash中的位置
                    send_ok=1;          // 设置接收到合法标志                 
                    break;
                }
            }
			Rf_ok_flag=0;               // 非法设备
		   }
        }
		//-----------------GPRS报警数据帧发送----------------------------------------------------//
        if(send_ok==1) // 接收到合法报警帧处理任务
        {
		   //-------------串口输出报警设备地址---------------------------------------//
		   /*
            uart_sendbyte(0x02);
            uart_sendbyte(0x03);
            uart_sendbyte(0x04);
            uart_sendbyte(address);    // 低位
			uart_sendbyte(address>>8); // 中位
			uart_sendbyte(address>>16);// 高位
            uart_sendbyte(0xFF);
            uart_sendbyte(data);
            uart_sendbyte(0x0d);
            uart_sendbyte(0x0a);
			*/
		   //------------封装数据部分[DTU协议]---------------------------------------//
		    Alarm_Data[0]=0x02; // 版本号【0x02超再生版】
			Alarm_Data[2]=FLASH_ReadByte((DEVICE_ADDRESS)+Flash_Query*6+4);// 回路号 					  
			Alarm_Data[3]=FLASH_ReadByte((DEVICE_ADDRESS)+Flash_Query*6+5);// 部件号	
			k=FLASH_ReadByte((DEVICE_ADDRESS)+Flash_Query*6+3);            // 设备类型
			switch(k)  // 设备类型  常闭型门磁=0x00  常开型门磁=0x01 离位中继=02
			{  
				case 0x44:  // 温感报警器=0x03
				    if(data==0x09)          // 1001 报警帧  
					{
						//---发送温感报警数据包[信息]-----------------------//
						if((Link_Flag==1)&&(Wen1_Enable_Flag==1)) // 已连接发送报警数据
						{ 
							DTU_send_data(0x1E,0x02);// 报警数据帧发送
							Wen1_Enable_Flag=0;      // 限制发送数据
							Wen1_Start_Flag=1;       // 启动5S再次发送定时
							Wen1_Timer_Count=0;      // 清零计时变量
						}
						else            // 未连接，保存报警帧MAX=10
						{
						}
					}
					else if(data==0x06)     // 0110 巡检帧
					{
						                    // 数据类型---巡检帧	
					}
					else                    // 不合理数据，默认巡检帧
					{
					}
				    break;
				case 0x55:  // 烟感报警器=0x04 
				    if(data==0x09)          // 1001 报警帧  
					{
					  //---发送温感报警数据包[信息]-----------------------//
						if((Link_Flag==1)&&(Yan1_Enable_Flag==1)) // 已连接发送报警数据
						{
							DTU_send_data(0x28,0x01);  // 报警数据帧发送
							Yan1_Enable_Flag=0;        // 限制发送数据
							Yan1_Start_Flag=1;         // 启动5S再次发送定时
							Yan1_Timer_Count=0;        // 清零计时变量
						}
						else            // 未连接，保存报警帧MAX=10
						{
						}
					}
					else if(data==0x0F)     // 0110 巡检帧
					{	
					}
					else                    // 不合理数据，默认巡检帧
					{
					}
					break;
				case 0x66:  // 手动报警器=0x66
				    if(data==0x09)          // 1001 报警帧  
					{
						//---手动报警数据包[信息]-----------------------//
						if((Link_Flag==1)&&(Shou1_Enable_Flag==1)) // 已连接发送报警数据
						{       
							DTU_send_data(0x17,0x03);// 报警数据帧发送
							Shou1_Enable_Flag=0;      // 限制发送数据
							Shou1_Start_Flag=1;       // 启动5S再次发送定时
							Shou1_Timer_Count=0;      // 清零计时变量
                            delay_ms(2000);           // 延时2S
							//-------启动声光报警----------------------//
							for(i=0;i<100;i++) // RF发送声光报警开启命令
							{ 
								SendEv1527(0x11111,0x06,29); // 声光报警开启
                                FeedTheIWDog();              // 喂狗
							}				
						}
						else            // 未连接，保存报警帧MAX=10
						{
						}
					}
					else if(data==0x06)     // 0110 手动报警恢复功能
					{
                        delay_ms(2000);           // 延时2S
						// 手动报警恢复停止报警--------------------------//
					    for(i=0;i<100;i++) // RF发送声光报警关闭命令
						{ 
							SendEv1527(0x11111,0x09,29); // 声光报警关闭
							FeedTheIWDog();// 喂狗
						}                    	
					}
					else                    // 不合理数据
					{
					}
				    break;
			    case 0x77:// 声光报警器
				    if(data==0x09)          // 1001 注册帧  
					{
					}
					else if(data==0x06)     // 0110 巡检帧
					{	
					}
					else                    // 不合理数据，默认巡检帧
					{
					}
					break;
				default:                                                                                                                              
					break;
			}
		   //------------报警数据发送-----------------------------------------------//
            delay_ms(200);
            Rf_ok_flag=0;  // 清零RF接收标志 
            send_ok=0;     // 清零发送标志
        }
	//----------------心跳包发送------------------------------------//
	if((Hart_Send_Flag==1)&&(Link_Flag==1))// 已连接,发送心跳包
	{
		Hart_Send_Flag=0;
		for(i=0;i<6;i++)
		{
		  uart_sendbyte(Hart_Data[i]);
		}
	}
	//----------------喂狗-----------------------------------------//
    FeedTheIWDog();            //调用独立看门狗函数进行喂狗,防止系统复位
    }
}

// 报警数据帧发送
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
	//Type  上行0x01
	uart_sendbyte(0x01);
	//encrypt 未加密
	uart_sendbyte(0x00);
	//Length
	uart_sendbyte(0x06);
	uart_sendbyte(0x00);
	//Data 报警数据
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

// 报警帧发送---[遵循用户信息传输装置协议]
// 无线中继网关
// 设备一：1回路1号烟感报警器
// 设备二：1回路2号温感报警器
// Device_Type=0x1E:温感报警器
// Device_Type=0x28:烟感报警器
void DTU_send_data(unsigned char Device_Type,unsigned char Channel)
{
	unsigned char ii = 0;
	// 启动符[2字节]
	delay_ms(1);
	uart_sendbyte(0x40);
	uart_sendbyte(0x40);
	/***控制单元*****************/ 
	// 业务流水号[2字节]
	uart_sendbyte(FrameCount & 0xFF);
	uart_sendbyte((FrameCount >> 8) & 0xFF);
	// 协议版本号[2字节]
	uart_sendbyte(0x01);
	uart_sendbyte(0x0a);
	// 时间标签[6字节]
	uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
	// 源地址[6字节] 11 ed e3 9d ee 1e
	uart_sendbyte(0x33); // 中继无线网关 0x33
    uart_sendbyte(0x01);
    uart_sendbyte(0x01);
    uart_sendbyte(0x01);
    uart_sendbyte(0x01);
    uart_sendbyte(0x01);
	// 目的地址[6字节] 00 00 00 00 00 00
	for (ii = 0; ii < 6; ii++)
		uart_sendbyte(0x00);
	// 应用数据单元长度[2字节] 30 00
	uart_sendbyte(0x30);
	uart_sendbyte(0x00);
	// 命令字节[1字节]
	uart_sendbyte(0x02);
	/*******应用数据单元***********/
	uart_sendbyte(0x02);// 类型标识 1字节 上传建筑消防设施部件运行状态 上行
	uart_sendbyte(0x01);// 信息对象数目 01
	//------信息体----------------//
	uart_sendbyte(0x01);// 系统类型标识 1字节 火灾报警系统 01 
	uart_sendbyte(0x01);// 系统地址 1字节 系统地址=01
	uart_sendbyte(Device_Type);// 部件类型 1字节 0x1E 温感报警器 0x28 烟感报警器
	uart_sendbyte(0x01);// 部件地址 00 01 01 00  1回路1号
	uart_sendbyte(0x00);
	uart_sendbyte(Channel);// 部件号
	uart_sendbyte(0x00);
	uart_sendbyte(0x02);// 火警
	uart_sendbyte(0x00);
	//------部件说明[31字节]-------//
	for(ii=0;ii<31;ii++)	
	{
	  uart_sendbyte(0x00);
	}
	//------时间标签[6字节]--------//
	uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
	/********校验和***************/
	// 校验和
	uart_sendbyte(0x00);
	/********结束符***************/
	// 结束符
	uart_sendbyte(0x23);
	uart_sendbyte(0x23);
	FrameCount++;
}

// 报警帧发送---[遵循用户信息传输装置协议]
// 无线中继网关
// 设备一：1回路1号烟感报警器
// 设备二：1回路1号温感报警器
// Device_Type=0x1E:温感报警器
// Device_Type=0x28:烟感报警器
// 加入校验和
void DTU_send_data1(unsigned char Device_Type)
{
	unsigned char ii = 0;
	// 启动符[2字节]
	delay_ms(1);
	uart_sendbyte(0x40);
	uart_sendbyte(0x40);
	/***控制单元*****************/ 
	// 业务流水号[2字节]
	uart_sendbyte(FrameCount & 0xFF);
	uart_sendbyte((FrameCount >> 8) & 0xFF);
	// 协议版本号[2字节]
	uart_sendbyte(0x01);
	uart_sendbyte(0x0a);
	// 时间标签[6字节]
	uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
	// 源地址[6字节] 11 ed e3 9d ee 1e
	uart_sendbyte(0x33); // 中继无线网关 0x33
    uart_sendbyte(0xed);
    uart_sendbyte(0xe3);
    uart_sendbyte(0x9d);
    uart_sendbyte(0xee);
    uart_sendbyte(0x1e);
	// 目的地址[6字节] 00 00 00 00 00 00
	for (ii = 0; ii < 6; ii++)
		uart_sendbyte(0x00);
	// 应用数据单元长度[2字节] 30 00
	uart_sendbyte(0x30);
	uart_sendbyte(0x00);
	// 命令字节[1字节]
	uart_sendbyte(0x02);
	/*******应用数据单元***********/
	uart_sendbyte(0x02);// 类型标识 1字节 上传建筑消防设施部件运行状态 上行
	uart_sendbyte(0x01);// 信息对象数目 01
	//------信息体----------------//
	uart_sendbyte(0x01);// 系统类型标识 1字节 火灾报警系统 01 
	uart_sendbyte(0x01);// 系统地址 1字节 系统地址=01
	uart_sendbyte(Device_Type);// 部件类型 1字节 0x1E 温感报警器 0x28 烟感报警器
	uart_sendbyte(0x00);// 部件地址 00 01 01 00  1回路1号
	uart_sendbyte(0x01);
	uart_sendbyte(0x01);
	uart_sendbyte(0x00);
	uart_sendbyte(0x02);// 火警
	uart_sendbyte(0x00);
	//------部件说明[31字节]-------//
	for(ii=0;ii<31;ii++)	
	{
	  uart_sendbyte(0x00);
	}
	//------时间标签[6字节]--------//
	uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
    uart_sendbyte(0x00);
	/********校验和***************/
	// 校验和
	uart_sendbyte(0x00);
	/********结束符***************/
	// 结束符
	uart_sendbyte(0x23);
	uart_sendbyte(0x23);
	FrameCount++;
}

/*****************************************************************************
* 函数名称: void SendData(void)
* 功能描述: USART1发送正常数据帧
* 参    数: 
* 返回  值:
*****************************************************************************/
/*
void SendData2(void)
{
	unsigned int ii;
	unsigned char Data_Frame[500];
	unsigned int  Data_Count=0;
	unsigned char Check_Sum=0;
	
	#if ENCRYPT // 加密
    #else       // 未加密
	// Header
	Uart1SendByte(0x40);
	Uart1SendByte(0x40);
	// DeviceType
	Uart1SendByte(0x80); // 水位仪修改为0x80
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
//注册功能
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
			FeedTheIWDog();            //调用独立看门狗函数进行喂狗,防止系统复位
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
		//向flash中写入地址，地址为3个字节，另外还有一个计数器
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
				while(FLASH_GetFlagStatus(FLASH_FLAG_WR_PG_DIS) == 1);      //等待编程结束
				while(FLASH_GetFlagStatus(FLASH_FLAG_EOP) == 1);            //等待编程结束
				FLASH_Lock(FLASH_MemType_Data);       
				//返回注册成功
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
			//提示注册失败
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
			//提示注册成功
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
//删除已注册信息
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
			FeedTheIWDog();            //调用独立看门狗函数进行喂狗,防止系统复位
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
		//向flash中写入地址，地址为3个字节，另外还有一个计数器
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
				while(FLASH_GetFlagStatus(FLASH_FLAG_WR_PG_DIS) == 1);      //等待编程结束
				while(FLASH_GetFlagStatus(FLASH_FLAG_EOP) == 1);            //等待编程结束
				FLASH_Lock(FLASH_MemType_Data);       
				//返回删除成功
				delete_ok_flag=1;
				old_address=0;             
				break;
			}
		}
		LEDR_OFF;
		LEDG_OFF;
		if(!delete_ok_flag)
		{
			//提示删除失败
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
			//提示删除成功
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
    if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);   //清除中断标志位
        Res = USART_ReceiveData8(USART1);  //(USART1->DR);//读取接收到的数据
		//---------配置模式------------------------------------------------//
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
		//-------正常工作模式------------------------------------------------//
		if(Normal_Mode_Flag==1)
	    {
			if(uart_buffer[0]==0x40) // 接收到帧头
			{
			   uart_buffer[uart_len++]=Res;// 继续接收其它字节
			}
			else  // 未检测到帧头
			{
				if(Res==0x40)// 若为帧头保存帧头
				{
				  uart_buffer[0]=Res;
				  uart_len++;
				}
			}
			if ((uart_buffer[uart_len - 2] == 0x23) && (uart_buffer[uart_len - 1] == 0x23)&&(uart_len>2))
			{
				uart_rec_flag=1;  // 接收到一帧
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















