
/*****************************
//KEY.h
//案件头文件
******************************/

#ifndef _Voice_H
#define _Voice_H	
//#include "stm32f10x.h"

/*****************************
//宏定义
*****************************/

//语音芯片忙信号
#define NVC_BUSY_RCC				 RCC_APB2Periph_GPIOC                //定义时钟
#define NVC_BUSY_GPIO				 GPIOC
#define NVC_BUSY_Pin         GPIO_Pin_5
#define GetState_NVC_BUSY  	 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)   //读取端口状态

//#define NVC_BUSY_High()        GPIO_SetBits(GPIOA,GPIO_Pin_0)     //定义输出高电平
//#define NVC_BUSY_Low()           GPIO_ResetBits(GPIOA,GPIO_Pin_0)   //定义输出低电平


//语音芯片时钟信号
#define NVC_CLK_RCC					  RCC_APB2Periph_GPIOB                //定义时钟
#define NVC_CLK_GPIO					GPIOB
#define NVC_CLK_Pin         	GPIO_Pin_0
#define NVC_CLK_High()        GPIO_SetBits(GPIOB,GPIO_Pin_0)     //定义输出高电平
#define NVC_CLK_Low()         GPIO_ResetBits(GPIOB,GPIO_Pin_0)   //定义输出低电平

//语音芯片数据信号
#define NVC_SDA_RCC						RCC_APB2Periph_GPIOB                //
#define NVC_SDA_GPIO					GPIOB
#define NVC_SDA_Pin         	GPIO_Pin_1
#define NVC_SDA_High()        GPIO_SetBits(GPIOB,GPIO_Pin_1)     //定义输出高电平
#define NVC_SDA_Low()         GPIO_ResetBits(GPIOB,GPIO_Pin_1)   //定义输出低电平


//语音音频功放使能口信号
#define NVC_ShutDown_RCC			RCC_APB2Periph_GPIOC                //
#define NVC_ShutDown_GPIO			GPIOC
#define NVC_ShutDown_Pin      GPIO_Pin_4
#define NVC_ShutDown_High()   GPIO_SetBits(GPIOC,GPIO_Pin_4)     //定义输出高电平
#define NVC_ShutDown_Low()    GPIO_ResetBits(GPIOC,GPIO_Pin_4)   //定义输出低电平
#define NVC_ShutDown_READ()   GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_4)     //定义输出高电平


#define VOICE_ALARM			0x24
#define VOICE_LOCALFAULT	0x27//0x18
#define VOICE_HOSTFAULT		0x26
#define VOICE_DUTY			0x28
#define VOICE_SELFCHECK		0x29
#define VOICE_MUTE			0xfe

#define VOICE_HALT			0
#define VOICE_FOREVER		0xffffffff


/******************************
//函数/变量声明
*******************************/
void Voice_Init(void);
unsigned char VoiceAction(unsigned char type,unsigned int sec);
unsigned char ReadVoiceType(void);
void Voice_Test(void);
#endif




