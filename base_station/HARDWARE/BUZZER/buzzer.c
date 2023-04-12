#include "buzzer.h" 
//////////////////////////////////////////////////////////////////////////////////   	 
unsigned int Buzzer_Open_time=0;
unsigned int Buzzer_Closed_time=0;
unsigned char Buzzer_Number=0;//蜂鸣器发声变量
unsigned char Buzzer_flag=0;//蜂鸣器鸣叫类型标志位
unsigned char	Buzzer_condition=0;//蜂鸣器是在鸣叫还是静音
unsigned char	Buzzer_Doing=0;//判断蜂鸣器是否正在工作，如果没工作，就给参数赋值。

//初始化PF8为输出口		    
//BEEP IO初始化
void Buzzer_Init(void)//Buzzer初始化
{ 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 //BUZZER-->PB.6 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.6
 GPIO_SetBits(GPIOB,GPIO_Pin_6);						 //PB.6 输出高
}

void Buzzer_SCAN()//蜂鸣器
{
  switch(Buzzer_flag)//判断Buzzer_flag来判断应该如何鸣叫
	{
	 case 0://蜂鸣器静音	 							 
			Buzzer_OFF;//静音
			Buzzer_condition=0;//蜂鸣器是在响还是没响
			Buzzer_Doing=0;//蜂鸣器不在工作状态
			Buzzer_flag=0;//选择叫声标志位清零
			Buzzer_Number=0;//响次数清零
			Buzzer_Open_time=0;//响时间清零
			Buzzer_Closed_time=0;//不响时间清0
			break;	 
	 case 1://蜂鸣器响5声
	 		if(Buzzer_Doing==0)//判断蜂鸣器是否正在工作，如果没工作，就给参数赋值。
			{
			 Buzzer_Number=5;//响4声
			 Buzzer_Open_time=2;//响100毫秒
			 Buzzer_Closed_time=2;//不响100毫秒
			 Buzzer_Doing=1;//重新赋值，不再进入此循环。
			}
			else
			{					
				if( (Buzzer_Number>0)&&(Buzzer_Doing==1) )//没有达到响声次数
				{
					 if(Buzzer_condition)//如果正在响，就进入响延时状态1
					 {
					 
					  Buzzer_Open_time--;	
					  if(Buzzer_Open_time>0)
					  {
					   Buzzer_ON;//鸣叫					   
					  }
					  else
					  {
					   Buzzer_condition=!Buzzer_condition;//翻转状态为静音
					   Buzzer_Open_time=2;//计数器重新赋值
					   Buzzer_Number--;//叫声次数减1
					  }				  
					 }
					 else//如果没在响，就进入不响延时状态0
					 {
					  Buzzer_Closed_time--;	
					  if(Buzzer_Closed_time>0)
					  {
					   Buzzer_OFF;//静音					   
					  }
					  else
					  {
					   Buzzer_condition=!Buzzer_condition;//翻转状态为鸣叫
					   Buzzer_Closed_time=2;//计数器重新赋值
					  }
					 }
				}
				else //鸣叫完毕，除去标志位。
				{	
					 Buzzer_OFF;//静音
					 Buzzer_condition=0;//蜂鸣器是在响还是没响
					 Buzzer_Doing=0;//蜂鸣器不在工作状态
					 Buzzer_flag=0;//选择叫声标志位清零
					 Buzzer_Number=0;//响次数清零
 					 Buzzer_Open_time=0;//响时间清零
 					 Buzzer_Closed_time=0;//不响时间清零
				}
			}		 											
			break;
	 		 		
	 case 2://蜂鸣器响2声
	 		if(Buzzer_Doing==0)//判断蜂鸣器是否正在工作，如果没工作，就给参数赋值。
			{
			 Buzzer_Number=3;//响4声
			 Buzzer_Open_time=5;//响50毫秒
			 Buzzer_Closed_time=5;//不响50毫秒
			 Buzzer_Doing=1;//重新赋值，不再进入此循环。
			}
			else
			{					
				if( (Buzzer_Number>0)&&(Buzzer_Doing==1) )//没有达到响声次数
				{
					 if(Buzzer_condition)//如果正在响，就进入响延时状态1
					 {
					 
					  Buzzer_Open_time--;	
					  if(Buzzer_Open_time>0)
					  {
					   Buzzer_ON;//鸣叫					   
					  }
					  else
					  {
					   Buzzer_condition=!Buzzer_condition;//翻转状态为静音
					   Buzzer_Open_time=5;//计数器重新赋值
					   Buzzer_Number--;//叫声次数减1
//					   SBUF = Buzzer_Number;
					  }				  
					 }
					 else//如果没在响，就进入不响延时状态0
					 {
					  Buzzer_Closed_time--;	
					  if(Buzzer_Closed_time>0)
					  {
					   Buzzer_OFF;//静音					   
					  }
					  else
					  {
					   Buzzer_condition=!Buzzer_condition;//翻转状态为鸣叫
					   Buzzer_Closed_time=5;//计数器重新赋值
					  }
					 }
				}
				else //鸣叫完毕，除去标志位。
				{	
					 Buzzer_OFF;//静音
					 Buzzer_condition=0;//蜂鸣器是在响还是没响
					 Buzzer_Doing=0;//蜂鸣器不在工作状态
					 Buzzer_flag=0;//选择叫声标志位清零
					 Buzzer_Number=0;//响次数清零
 					 Buzzer_Open_time=0;//响时间清零
 					 Buzzer_Closed_time=0;//不响时间清零
				}
			}		 											
			break;
	 case 3://蜂鸣器长鸣一声
	 		if(Buzzer_Doing==0)//判断蜂鸣器是否正在工作，如果没工作，就给参数赋值。
			{
			 Buzzer_Number=5;//响4声
			 Buzzer_Open_time=2;//响50毫秒
			 Buzzer_Closed_time=2;//不响50毫秒
			 Buzzer_Doing=1;//重新赋值，不再进入此循环。
			}
			else
			{					
				if( (Buzzer_Number>0)&&(Buzzer_Doing==1) )//没有达到响声次数
				{
					 if(Buzzer_condition)//如果正在响，就进入响延时状态1
					 {
					 
					  Buzzer_Open_time--;	
					  if(Buzzer_Open_time>0)
					  {
					   Buzzer_ON;//鸣叫					   
					  }
					  else
					  {
					   Buzzer_condition=!Buzzer_condition;//翻转状态为静音
					   Buzzer_Open_time=1;//计数器重新赋值
					   Buzzer_Number--;//叫声次数减1
//					   SBUF = Buzzer_Number;
					  }				  
					 }
					 else//如果没在响，就进入不响延时状态0
					 {
					  Buzzer_Closed_time--;	
					  if(Buzzer_Closed_time>0)
					  {
					   Buzzer_OFF;//静音					   
					  }
					  else
					  {
					   Buzzer_condition=!Buzzer_condition;//翻转状态为鸣叫
					   Buzzer_Closed_time=1;//计数器重新赋值
					  }
					 }
				}
				else //鸣叫完毕，除去标志位。
				{	
					 Buzzer_OFF;//静音
					 Buzzer_condition=0;//蜂鸣器是在响还是没响
					 Buzzer_Doing=0;//蜂鸣器不在工作状态
					 Buzzer_flag=0;//选择叫声标志位清零
					 Buzzer_Number=0;//响次数清零
 					 Buzzer_Open_time=0;//响时间清零
 					 Buzzer_Closed_time=0;//不响时间清零
				}
			}			 											
			break;			
	 case 4://蜂鸣器响2声
	 		if(Buzzer_Doing==0)//判断蜂鸣器是否正在工作，如果没工作，就给参数赋值。
			{
			 Buzzer_Open_time=200;//响1000毫秒
			 Buzzer_ON;//鸣叫									
			 Buzzer_Doing=1;//重新赋值，不再进入此循环。
			}
			else
			{		
				Buzzer_Open_time--;				
				if(Buzzer_Open_time==0)//鸣叫完毕，除去标志位。
				{
					Buzzer_OFF;//静音
					Buzzer_condition=0;//蜂鸣器是在响还是没响	
					Buzzer_Doing=0;//蜂鸣器不在工作状态
					Buzzer_flag=0;//选择叫声标志位清零
					Buzzer_Number=0;//响次数清零
					Buzzer_Open_time=0;//响时间清零
					Buzzer_Closed_time=0;//不响时间清零					
				}
				else 
				{	
					 Buzzer_ON;//静音
				}
			}	 											
			break;			
	 case 5://蜂鸣器长鸣一声
	 		if(Buzzer_Doing==0)//判断蜂鸣器是否正在工作，如果没工作，就给参数赋值。
			{
			 Buzzer_Open_time=10;//响50毫秒
			 Buzzer_ON;//鸣叫									
			 Buzzer_Doing=1;//重新赋值，不再进入此循环。
			}
			else
			{		
				Buzzer_Open_time--;				
				if(Buzzer_Open_time==0)//鸣叫完毕，除去标志位。
				{
					Buzzer_OFF;//静音
					Buzzer_condition=0;//蜂鸣器是在响还是没响	
					Buzzer_Doing=0;//蜂鸣器不在工作状态
					Buzzer_flag=0;//选择叫声标志位清零
					Buzzer_Number=0;//响次数清零
					Buzzer_Open_time=0;//响时间清零
					Buzzer_Closed_time=0;//不响时间清零					
				}
				else 
				{	
					 Buzzer_ON;//静音
				}
			}		 											
			break;				
	 case 6://蜂鸣器长鸣一声
	 		if(Buzzer_Doing==0)//判断蜂鸣器是否正在工作，如果没工作，就给参数赋值。
			{
			 Buzzer_Open_time=5;//响50毫秒
			 Buzzer_ON;//鸣叫									
			 Buzzer_Doing=1;//重新赋值，不再进入此循环。
			}
			else
			{		
				Buzzer_Open_time--;				
				if(Buzzer_Open_time==0)//鸣叫完毕，除去标志位。
				{
					Buzzer_OFF;//静音
					Buzzer_condition=0;//蜂鸣器是在响还是没响	
					Buzzer_Doing=0;//蜂鸣器不在工作状态
					Buzzer_flag=0;//选择叫声标志位清零
					Buzzer_Number=0;//响次数清零
					Buzzer_Open_time=0;//响时间清零
					Buzzer_Closed_time=0;//不响时间清零					
				}
				else 
				{	
					 Buzzer_ON;//静音
				}
			}		 											
			break;				
	 case 7://蜂鸣器长鸣一声
	 		if(Buzzer_Doing==0)//判断蜂鸣器是否正在工作，如果没工作，就给参数赋值。
			{
			 Buzzer_Open_time=300;//响50毫秒
			 Buzzer_ON;//鸣叫									
			 Buzzer_Doing=1;//重新赋值，不再进入此循环。
			}
			else
			{		
				Buzzer_Open_time--;				
				if(Buzzer_Open_time==0)//鸣叫完毕，除去标志位。
				{
					Buzzer_OFF;//静音
					Buzzer_condition=0;//蜂鸣器是在响还是没响	
					Buzzer_Doing=0;//蜂鸣器不在工作状态
					Buzzer_flag=0;//选择叫声标志位清零
					Buzzer_Number=0;//响次数清零
					Buzzer_Open_time=0;//响时间清零
					Buzzer_Closed_time=0;//不响时间清零					
				}
				else 
				{	
					 Buzzer_ON;//静音
				}
			}		 											
			break;			
			
	 default:
	 		Buzzer_OFF;//静音
			Buzzer_flag=0;
	 		break;
	}

}












