#include "led.h"
unsigned char LED_flag=0;//LED闪烁标志位。
unsigned int time_led=0;
//初始化PB1和PB9为输出口.并使能这两个口的时钟		    
//433和LoRa模块使能引脚初始化
void ENABLE_IO_Init(void)//使能引脚初始化	
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PA端口时钟

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	//端口配置, 推挽输出
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz	
 GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 GPIO_SetBits(GPIOA,GPIO_Pin_11); 						 //PA.11 输出高 	
}

//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure; 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PC端口时钟
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	//LED0-->PB.5 端口配置, 推挽输出
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz	
 GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 GPIO_SetBits(GPIOB,GPIO_Pin_5); 						 //PB.5 输出高 	
}
void LED_SCAN()//LED
{

//			if(GPRS_WORK==1)//如果连接服务器成功。
//			{
//				LED3=0;//点亮连接网络成功LED灯
//			}
//			else
//			{
//				LED3=1;
//			}			
//			if(GPRS_LINK==1)//如果连接服务器成功。
//			{
//				LED2=0;//点亮连接网络成功LED灯
//			}
//			else
//			{
//				LED2=1;
//			}

//	static unsigned int time_led=0;
  switch(LED_flag)//决定LED运行状态。
	{
	 case 0://灭灯				 
			LED0=1;
			break;	 
	 case 1://亮灯
	 		LED0=0;								
			break;	 		 		
	 case 2://500ms亮，500ms灭。
			time_led++;
			if(time_led>100)
			{
				LED0=~LED0;
				time_led=0;
			}	 											
			break;
	 case 3://200ms亮，200ms灭。
			time_led++;
			if(time_led>40)
			{
				LED0=~LED0;
				time_led=0;
			}	 	 											
			break;
	 case 4://翻转LED状态。
			LED0=~LED0;							
			break;			
	 default:
			LED0=0;	
	 		break;
	}
}












