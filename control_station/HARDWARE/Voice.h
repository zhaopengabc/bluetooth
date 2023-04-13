
/*****************************
//KEY.h
//����ͷ�ļ�
******************************/

#ifndef _Voice_H
#define _Voice_H	
//#include "stm32f10x.h"

/*****************************
//�궨��
*****************************/

//����оƬæ�ź�
#define NVC_BUSY_RCC				 RCC_APB2Periph_GPIOC                //����ʱ��
#define NVC_BUSY_GPIO				 GPIOC
#define NVC_BUSY_Pin         GPIO_Pin_5
#define GetState_NVC_BUSY  	 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)   //��ȡ�˿�״̬

//#define NVC_BUSY_High()        GPIO_SetBits(GPIOA,GPIO_Pin_0)     //��������ߵ�ƽ
//#define NVC_BUSY_Low()           GPIO_ResetBits(GPIOA,GPIO_Pin_0)   //��������͵�ƽ


//����оƬʱ���ź�
#define NVC_CLK_RCC					  RCC_APB2Periph_GPIOB                //����ʱ��
#define NVC_CLK_GPIO					GPIOB
#define NVC_CLK_Pin         	GPIO_Pin_0
#define NVC_CLK_High()        GPIO_SetBits(GPIOB,GPIO_Pin_0)     //��������ߵ�ƽ
#define NVC_CLK_Low()         GPIO_ResetBits(GPIOB,GPIO_Pin_0)   //��������͵�ƽ

//����оƬ�����ź�
#define NVC_SDA_RCC						RCC_APB2Periph_GPIOB                //
#define NVC_SDA_GPIO					GPIOB
#define NVC_SDA_Pin         	GPIO_Pin_1
#define NVC_SDA_High()        GPIO_SetBits(GPIOB,GPIO_Pin_1)     //��������ߵ�ƽ
#define NVC_SDA_Low()         GPIO_ResetBits(GPIOB,GPIO_Pin_1)   //��������͵�ƽ


//������Ƶ����ʹ�ܿ��ź�
#define NVC_ShutDown_RCC			RCC_APB2Periph_GPIOC                //
#define NVC_ShutDown_GPIO			GPIOC
#define NVC_ShutDown_Pin      GPIO_Pin_4
#define NVC_ShutDown_High()   GPIO_SetBits(GPIOC,GPIO_Pin_4)     //��������ߵ�ƽ
#define NVC_ShutDown_Low()    GPIO_ResetBits(GPIOC,GPIO_Pin_4)   //��������͵�ƽ
#define NVC_ShutDown_READ()   GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_4)     //��������ߵ�ƽ


#define VOICE_ALARM			0x24
#define VOICE_LOCALFAULT	0x27//0x18
#define VOICE_HOSTFAULT		0x26
#define VOICE_DUTY			0x28
#define VOICE_SELFCHECK		0x29
#define VOICE_MUTE			0xfe

#define VOICE_HALT			0
#define VOICE_FOREVER		0xffffffff


/******************************
//����/��������
*******************************/
void Voice_Init(void);
unsigned char VoiceAction(unsigned char type,unsigned int sec);
unsigned char ReadVoiceType(void);
void Voice_Test(void);
#endif




