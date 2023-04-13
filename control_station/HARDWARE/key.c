#include "stm32f10x_exti.h"
#include "includes.h"
#include "key.h"
//声明函数
extern OS_TCB keyTaskTCB;
//extern unsigned char Leds_Action(unsigned int ledn,unsigned char ledaction,unsigned int n100msec);
#ifdef FB1010
//外部中断初始化
void EXTIX_Init(void)
{
 	  EXTI_InitTypeDef EXTI_InitStructure;
 	  NVIC_InitTypeDef NVIC_InitStructure;

	//PC1=KEY1 PC0=KEY2 PC3=KEY3 PC2=KEY4
	//PB4=KEY5 PB3=KEY6
	//KEY2 KEY4 KEY6 = ROW  Reset
	//KEY1 KEY3 KEY5 = Column Set
	
	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource1);//PC1=KEY1
  	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);

	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource3);//PC3=KEY3
  	EXTI_InitStructure.EXTI_Line=EXTI_Line3;
  	EXTI_Init(&EXTI_InitStructure);

	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource4);//PB4=KEY5
  	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  	EXTI_Init(&EXTI_InitStructure);

 		
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure); 
		
		
		NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure); 

		NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure); 
		
		
}
//volatile unsigned char EXTI0_FLAG=0;
//volatile unsigned char EXTI2_FLAG=0;
//volatile unsigned char EXTI3_FLAG=0;
//外部中断服务程序
void EXTI1_IRQHandler(void)
{
	OS_ERR err;
 	OSTaskResume(&keyTaskTCB,&err);
	EXTI_ClearITPendingBit(EXTI_Line1);
}

//void EXTI1_IRQHandler(void)
//{
//	OS_ERR err;
//	OSTaskResume(&keyTaskTCB,&err);
//	EXTI_ClearITPendingBit(EXTI_Line1);
//}

void EXTI3_IRQHandler(void)
{
	OS_ERR err;
//	EXTI2_FLAG=1;
 	OSTaskResume(&keyTaskTCB,&err);
	EXTI_ClearITPendingBit(EXTI_Line3);
}

void EXTI4_IRQHandler(void)
{
	OS_ERR err;
//	EXTI3_FLAG=1;
 	OSTaskResume(&keyTaskTCB,&err);
	EXTI_ClearITPendingBit(EXTI_Line4);
}

//void EXTI4_IRQHandler(void)
//{
//	OS_ERR err;
//	OSTaskResume(&keyTaskTCB,&err);
//	EXTI_ClearITPendingBit(EXTI_Line4);
//}

//void EXTI9_5_IRQHandler(void)
//{
//	OS_ERR err;
//	OSTaskResume(&keyTaskTCB,&err);
//	EXTI_ClearITPendingBit(EXTI_Line5|EXTI_Line6|EXTI_Line7|EXTI_Line8);
//}
//按键初始化函数

void KEY_Init(void)
{
	//1010A
	GPIO_InitTypeDef GPIO_InitStructure;
	
		/* 配置拨码开关1引脚 */	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); // 改变指定管脚的映射,JTAG-DP禁用,SW-DP使能
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		
	//PC1 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	//PC0  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	//PC3 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	//PC2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	//PB4 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//PB3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//PC1=KEY1 PC0=KEY2 PC3=KEY3 PC2=KEY4
	//PB4=KEY5 PB3=KEY6
	//KEY2 KEY4 KEY6 = ROW  Reset  OUT
	//KEY1 KEY3 KEY5 = Column Set  IN
	GPIO_ResetBits(GPIOC,GPIO_Pin_0|GPIO_Pin_2);//ROW
	GPIO_ResetBits(GPIOB,GPIO_Pin_3);
	
	GPIO_SetBits(GPIOC,GPIO_Pin_1|GPIO_Pin_3);//Column
	GPIO_SetBits(GPIOB,GPIO_Pin_4);
	
	
//	GPIO_SetBits(GPIOC,GPIO_Pin_1|GPIO_Pin_3);
//	GPIO_SetBits(GPIOB,GPIO_Pin_4);
//	
//	GPIO_ResetBits(GPIOC,GPIO_Pin_0|GPIO_Pin_2);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_3);
	EXTIX_Init();
	
	//PA15 权限锁--PB9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
#endif

#ifdef FB1010

unsigned int KEY_READ(void)
{
	unsigned int a=0,b=0,c=0;
//	a=GPIO_ReadInputData(GPIOC)&0x0005;
//	b=GPIO_ReadInputData(GPIOB)&0x0008;
	
 	a=GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1);
	a=a|(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)<<2);
	b=GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4)<<3;
	c=a|b;
	return c;
}
 
//按键扫描10ms执行一次；
//若按键按下，返回读取的键值（但是无效）
//或者返回有效键值|0x80000000
unsigned int KEY_SCAN(void)
{
	unsigned char i=0;
	unsigned int key_read=0;			//读取键值
	static unsigned long int key;		//短按键值
	static unsigned long int lastkey=0;	//长按键值
	static unsigned char counter=0;		//去抖延时计数
//  KEY_Init();
	
	KEY_ALL_HI();
	KEY_A0_LO();						    //扫描K0~K4
//	for(i=0;i<20;i++) {;}
	for(i=0;i<220;i++) {;}
	key_read=KEY_READ();				//键值存在低5位中 key_read=1101
// 	printf("K1=%x\r\n",key_read);
	for(i=0;i<20;i++) {;}
	
	KEY_ALL_HI();
	KEY_A1_LO();
//	for(i=0;i<20;i++) {;}
	for(i=0;i<220;i++) {;}
	key_read+=(KEY_READ()<<4);
// 	printf("K2=%x\r\n",key_read);
	for(i=0;i<20;i++) {;}

	KEY_ALL_HI();
	KEY_A2_LO();
// 	for(i=0;i<20;i++) {;}
	for(i=0;i<220;i++) {;}
	key_read+=(KEY_READ()<<8);
// 	printf("K3=%x\r\n",key_read);
	for(i=0;i<20;i++)  {;}

//	KEY_ALL_HI();
//	KEY_A3_LO();
//	for(i=0;i<20;i++) {;}
//	key_read+=(KEY_READ()<<15);
//	for(i=0;i<20;i++) {;}

//	KEY_ALL_HI();
//	KEY_A4_LO();
//	for(i=0;i<20;i++) {;}
//	key_read+=(KEY_READ()<<20);
//	for(i=0;i<20;i++) {;}
	
	//扫描完毕，IO口拉低，当有新按键时再次触发中断
	KEY_ALL_LO();
	
//	key_read^=0x01FFFFFF;
//	key_read&=0x01FFFFFF;		//低25位取值
		
	key_read^=0x00000DDD;
	key_read&=0x00000DDD;		//低25位取值
	//下段代码滤波用，暂时不加
	if(key_read==0x0)			//没有按键
	{
		counter=0;
		key=0;
	}
	else
	{
		counter++;
		if(counter<4)
		{
			key=lastkey;
		}
		else
		{
			counter=4;
			key=key_read;
		}
	}
 
	key_read=key;
	key=key&(lastkey^key);
	lastkey=key_read;
	
	if(key!=0) 
	{return (key|0x80000000);}
	else 
	{return key_read;}

	
//	key 状态 0x80000000
//  S3 S2 X S1--S6 S5 X S4--S2 S1 X S7
 
}
#endif


















#ifdef FB1030
//外部中断初始化
static unsigned char EXTIX_Flag=0;

void EXTIX_Init(void)
{
 	  EXTI_InitTypeDef EXTI_InitStructure;
 	  NVIC_InitTypeDef NVIC_InitStructure;

	//PC1=KEY1 PC0=KEY2 PC3=KEY3 PC2=KEY4
	//PB4=KEY5 PB3=KEY6
	//KEY2 KEY4 KEY6 = ROW  Reset
	//KEY1 KEY3 KEY5 = Column Set
	
	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource1);//PC1=KEY1
  	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);

		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource0);//PC0=KEY1
  	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  	EXTI_Init(&EXTI_InitStructure);
	
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource2);//PC2=KEY2
  	EXTI_InitStructure.EXTI_Line=EXTI_Line2;
  	EXTI_Init(&EXTI_InitStructure);
		
	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource3);//PC3=KEY3
  	EXTI_InitStructure.EXTI_Line=EXTI_Line3;
  	EXTI_Init(&EXTI_InitStructure);

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure); 
 		
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure); 
		
		
		NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure); 

		NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure); 	
}
//外部中断服务程序
void EXTI1_IRQHandler(void)
{
	OS_ERR err;
	if(EXTIX_Flag==0)
 	{ OSTaskResume(&keyTaskTCB,&err); }
	EXTI_ClearITPendingBit(EXTI_Line1);
	EXTIX_Flag=2;
}

void EXTI0_IRQHandler(void)
{
	OS_ERR err;
	if(EXTIX_Flag==0)
 	{OSTaskResume(&keyTaskTCB,&err);}
	EXTI_ClearITPendingBit(EXTI_Line0);
	EXTIX_Flag=1;
}

void EXTI2_IRQHandler(void)
{
	OS_ERR err;
	if(EXTIX_Flag==0)
 	{OSTaskResume(&keyTaskTCB,&err);}
	EXTI_ClearITPendingBit(EXTI_Line2);
	EXTIX_Flag=3;
}

void EXTI3_IRQHandler(void)
{
	OS_ERR err;
	if(EXTIX_Flag==0)
	{	OSTaskResume(&keyTaskTCB,&err);}
	EXTI_ClearITPendingBit(EXTI_Line3);
	EXTIX_Flag=4;
}



void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
		/* 配置拨码开关1引脚 */	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); // 改变指定管脚的映射,JTAG-DP禁用,SW-DP使能
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		
	//PC1 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	//PC0  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	//PC3 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	//PC2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	//PC1=KEY1 PC0=KEY2 PC3=KEY3 PC2=KEY4
	//PB4=KEY5 PB3=KEY6
	//KEY2 KEY4 KEY6 = ROW  Reset  OUT
	//KEY1 KEY3 KEY5 = Column Set  IN
	GPIO_ResetBits(GPIOC,GPIO_Pin_0|GPIO_Pin_2);//ROW
	GPIO_ResetBits(GPIOB,GPIO_Pin_3);
	
	GPIO_SetBits(GPIOC,GPIO_Pin_1|GPIO_Pin_3);//Column
	GPIO_SetBits(GPIOB,GPIO_Pin_4);
	
	EXTIX_Init();
	
 
}
#endif

#ifdef FB1030
unsigned int KEY_SCAN(void)
{
	unsigned int d1=1,d2=1;
	unsigned int key_read=0;
	OS_ERR err;
	
	if(EXTIX_Flag!=0)
	{
    switch(EXTIX_Flag)
		{	
			case  1 :
			{
				d1=GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0);
				OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err); //延时100ms
				d2=GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0);
				if((d1==0)&&(d2==0))
				{ key_read|=0x00000800;//自检 
					key_read|=0x80000000;
				}
				break;
			}	
			case  2 :
			{
				d1=GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1); 
				OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err); //延时100ms
				d2=GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1); 
				if((d1==0)&&(d2==0))
				{ key_read|=0x00000001;//警情消除；
					key_read|=0x80000000;
				}
				break;
			} 
			case  3 :
			{
				d1=GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2);
				OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err); //延时100ms
				d2=GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2);
				if((d1==0)&&(d2==0))
				{ key_read|=0x00000040;//查岗
					key_read|=0x80000000;
				}
				break;
			} 
		  case  4 :
			{
				d1=GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3);
				OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err); //延时100ms
				d2=GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3);
				if((d1==0)&&(d2==0))
				{ key_read|=0x00000100;//手报 
					key_read|=0x80000000;
				}
				break;
			}	
		  default		:
			   break;
			
		}
		EXTIX_Flag=0;		
	}
	
	
//	a=GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0);//复位
//	if(a==0)
//	{ key_read|=0x00000800;//自检 
//	  key_read|=0x80000000;
//	}
//		
//  b=GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1); 
//	if(b==0)
//	{ key_read|=0x00000001;//警情消除；
//		key_read|=0x80000000;
//	}
//	c=GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2);
//	if(c==0)
//	{ key_read|=0x00000040;//查岗
//		key_read|=0x80000000;
//	}
//	
//	d=GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3);
//	if(d==0)
//	{ key_read|=0x00000100;//手报 
//		key_read|=0x80000000;
//	}
	
	return key_read;
}
#endif
void Key_Test(void)
{
	OS_ERR err;
	static unsigned int Key0_count=0;
	unsigned int key=0;
	while(1)
	{
		key=KEY_SCAN();
		if(key!=0)			//按键按下
		{
			Key0_count=0;
			if((key&0x80000000)==0x80000000)		//为有效按键？
			{
				if(key&0x7FFFFFFF)
				{
//					Leds_Action(key&0x00ffffff,LED_ON,50);
				}
			}
		}
		else
		{
			Key0_count++;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //延时10ms
		if(Key0_count>50)
		{
			Key0_count=0;
 			OSTaskSuspend(&keyTaskTCB,&err);
		}
	}
}




