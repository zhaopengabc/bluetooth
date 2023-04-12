//#include "key.h"
//#include "timer.h"
//#include "buzzer.h"
//#include "led.h"
//#include "delay.h"
////按键初始化函数 
////PC5 设置成输入
//unsigned char KEY_flag=0;//当 KEY_flag=1的时候，表示短按。 KEY_flag=2的时候，表示长按。
//unsigned char KEY0_DN_L=0;//定义全局变量。按键被长按了
//unsigned char KEY0_DN_S=0;//定义全局变量。按键被短按了
//unsigned int  key_dn_long=5000;//表示长按
//unsigned int  key_dn_short=1000;//表示短按
//unsigned int  key_dn_middle=10000;
//unsigned int  KEY0_Dn_TimeCnt=0; //按键0去抖动延时计数器,可以为负值
//void KEY_Init(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使PORTA时钟	
//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;//PA1
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //设置成输入
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA1
//}
//void KEY_SCAN(void)//按键扫描
//{//识别短按和长按。要求：短按，只要不松手，就不算短按。但是要有蜂鸣器声音提醒，松开手后，直接进入定时器查看短按计时器是否在范围内，如果在，则认定短按有效，置标志位。
////长按和短按是互斥的，也就是说两者在同一时间只能存在一个。
//	/****************************************************************************/	
////static unsigned char KEY0_DN_L=0;//定义全局变量。按键被长按了
////static unsigned char KEY0_DN_S=0;//定义全局变量。按键被短按了
////static unsigned int  KEY0_Dn_TimeCnt=0; //按键0去抖动延时计数器,可以为负值
////	//按键0。判断按键是短按还是长按。
////	if(KEY0==Bit_RESET)//IO是高电平，说明按键没有被按下，这时要及时清零一些标志位
////	{
////		if(KEY0_Dn_TimeCnt==0)
////		{
////			KEY0_Dn_TimeCnt=0;//按键去抖动延时计数器清零。
////		}
////		else
////		{
////			KEY0_Dn_TimeCnt--;
////		}
////		if((KEY0_Dn_TimeCnt>=key_dn_short)&&(KEY0_Dn_TimeCnt<=key_dn_long))
////		{
////			KEY0_Dn_TimeCnt=0;
////			KEY0_DN_S=0;
////			KEY0_DN_L=0;
////			KEY_flag=1;//按键被短按
////			Buzzer_flag=4;	
////		}
////		if(KEY0_Dn_TimeCnt>=key_dn_long)
////		{
////			KEY0_Dn_TimeCnt=0;
////			KEY0_DN_S=0;
////			KEY0_DN_L=0;
////			KEY_flag=2;//按键被长按	
////			Buzzer_flag=4;				
////		}		
////	}
////	else//有按键按下，且是第一次被按下
////	{
////		if(KEY0_Dn_TimeCnt<60000)
////		{
////			KEY0_Dn_TimeCnt++;  //延时计数器	
////		}
////	  if((KEY0_Dn_TimeCnt>=key_dn_short)&&(KEY0_DN_S==0))//表示短按
////	  {
////		 KEY0_DN_S=1;
////		 Buzzer_flag=2;	
////	  }
////	  if((KEY0_Dn_TimeCnt>=key_dn_long)&&(KEY0_DN_L==0))//如果计数器大于长按计数器，则表示长按。
////	  {
////		 KEY0_DN_L=1;//按键被长按了。
////		 Buzzer_flag=4;		
////	  }
////	}
///****************************************************************************/
////  if(RE11_SET==0)//IO是高电平，说明按键没有被按下，这时要及时清零一些标志位
////  {
////   RE11_SET_TimeCnt=0;//按键去抖动延时计数器清零，此行非常巧妙，是我实战中摸索出来的。
////	 RE11_SET_L=0;
////	 RE11_SET_S=0;
////  }
////  else//有按键按下，且是第一次被按下
////  {
////     RE11_SET_TimeCnt++;  //延时计数器 
////     if( (RE11_SET_TimeCnt>=RE11_dn_long)&&(RE11_SET_L==0) )//表示长按
////     {	
////			RE11_SET_L=1;//长按翻转状态	
////KEY_flag=2;//按键被长按了。			 
////			Buzzer_flag=3;//按键长按声音			 
////     }
////		 else if(  (RE11_SET_TimeCnt>=RE11_dn_short)&&(RE11_SET_S==0) )//表示短按
////		 {
////			RE11_SET_S=1;//短按翻转状态	
////KEY_flag=1;
////			Buzzer_flag=3;//按键短按声音			 

////		 }
////  }		
//	
///****************************************************************************/	
//	if(KEY0==0)//IO是高电平，说明按键没有被按下，这时要及时清零一些标志位
//	{
//		if(KEY0_Dn_TimeCnt==0)
//		{
//			KEY0_Dn_TimeCnt=0;//按键去抖动延时计数器清零。
//		}
//		else
//		{
//			KEY0_Dn_TimeCnt--;
//		}
//	}
//	else//有按键按下，且是第一次被按下
//	{
//	  KEY0_Dn_TimeCnt++;  //延时计数器	
//		
//	  if((KEY0_Dn_TimeCnt>=key_dn_long)&&(KEY0_Dn_TimeCnt<key_dn_middle)&&(KEY_flag==0))//表示短按&&(KEY0_DN_S==0)
//	  {	
//		 KEY0_DN_S=1;//按键被短按了。
//		 KEY0_DN_L=0;
//			KEY_flag=2;			
//		 Buzzer_flag=4;	
//	  }
//	  if( (KEY0_Dn_TimeCnt>=key_dn_short)&&(KEY_flag==0) )//如果计数器大于长按计数器，则表示长按。
//	  {				
//		 KEY0_DN_S=0;//按键被短按状态清零。长按和短按只能存在一个，取长按。
//		 KEY0_DN_L=1;//按键被长按了。
//			KEY_flag=1;
//		 Buzzer_flag=1;		
//	  }
//	}	
//}



#include "key.h"
#include "timer.h"
#include "buzzer.h"
#include "led.h"
#include "delay.h"
//按键初始化函数 
//PC5 设置成输入
unsigned char KEY_flag=0;//当 KEY_flag=1的时候，表示短按。 KEY_flag=2的时候，表示长按。
unsigned int  key_dn_long=1000;//表示长按
unsigned int  key_dn_short=100;//表示短按

void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使PORTA时钟	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;//PA1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA1
}
void KEY_SCAN(void)//按键扫描
{//识别短按和长按。要求：短按，只要不松手，就不算短按。但是要有蜂鸣器声音提醒，松开手后，直接进入定时器查看短按计时器是否在范围内，如果在，则认定短按有效，置标志位。
//长按和短按是互斥的，也就是说两者在同一时间只能存在一个。
	/****************************************************************************/	
static unsigned char KEY0_DN_L=0;//定义全局变量。按键被长按了
static unsigned char KEY0_DN_S=0;//定义全局变量。按键被短按了
static unsigned int  KEY0_Dn_TimeCnt=0; //按键0去抖动延时计数器,可以为负值
	//按键0。判断按键是短按还是长按。
	if(KEY0==1)//IO是高电平，说明按键没有被按下，这时要及时清零一些标志位
	{
		if(KEY0_Dn_TimeCnt==0)
		{
			KEY0_Dn_TimeCnt=0;//按键去抖动延时计数器清零。
		}
		else
		{
			KEY0_Dn_TimeCnt--;
		}
		if((KEY0_Dn_TimeCnt>=key_dn_short)&&(KEY0_Dn_TimeCnt<=key_dn_long))
		{
			KEY0_Dn_TimeCnt=0;
			KEY0_DN_S=0;
			KEY0_DN_L=0;
			KEY_flag=1;//按键被短按
//			Buzzer_flag=4;	
		}
		if(KEY0_Dn_TimeCnt>=key_dn_long)
		{
			KEY0_Dn_TimeCnt=0;
			KEY0_DN_S=0;
			KEY0_DN_L=0;
			KEY_flag=2;//按键被长按	
//			Buzzer_flag=4;				
		}		
	}
	else//有按键按下，且是第一次被按下
	{
		if(KEY0_Dn_TimeCnt<60000)
		{
			KEY0_Dn_TimeCnt++;  //延时计数器	
		}
	  if((KEY0_Dn_TimeCnt>=key_dn_short)&&(KEY0_DN_S==0))//表示短按
	  {
		 KEY0_DN_S=1;
		 Buzzer_flag=2;	
	  }
	  if((KEY0_Dn_TimeCnt>=key_dn_long)&&(KEY0_DN_L==0))//如果计数器大于长按计数器，则表示长按。
	  {
		 KEY0_DN_L=1;//按键被长按了。
		 Buzzer_flag=4;		
	  }
	}
/****************************************************************************/
}



