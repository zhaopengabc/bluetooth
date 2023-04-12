#include "led1.h" 

static u8 hc595_status1=0xFF;//第一个 74HC595 输入量的值
static u8 hc595_status2=0xFF;//第二个 74HC595 输入量的值
static u8 hc595_status3=0xFF;//第三个 74HC595 输入量的值

static u8 LED_DATA[24][3]; //灯有24个。每个灯有3个变量。灯闪烁时亮的时间。灯闪烁时灭的时间。灯闪烁的次数。
static u8 LED_ON_TIME[24];//灯打开的时间。
static u8 LED_OFF_TIME[24];//灯关闭的时间。
static u8 LED_status[24];//共24个灯。每个灯的状态标志位。1为点亮，0为灭。

static u16 keep_time_heartbeat=300;//灯维持时间倒计时的心跳。300*10=3s  10是进入定时器中断的时间间隔。
static u16 keep_time_heartbeat_sbu;
void LED_SCAN(void)//灯扫描	
{
	u16 i;
	static unsigned int time_step_ON=0;
	static unsigned int time_step_OFF=0;			
	if(keep_time_heartbeat_sbu==0)//2秒进去一次。
	{//灯闪烁倒计时时间。进行自减。	
		keep_time_heartbeat_sbu=keep_time_heartbeat;
		for(i=0;i<2;i++)//所有灯的倒计时次数减一。轮询一遍。
		{
				if(LED_DATA[i][2]==0)//如果倒计时到了时间。灭灯。
				{
					if( ( (LED_DATA[i][0]==0)&&(LED_DATA[i][1]==0)==1 )|| ( (LED_DATA[i][0]==1)&&(LED_DATA[i][1]==1)==1 )==1 )					
					{
						if(LED_DATA[i][2]>=254)
						{
							LED_DATA[i][2]=255;
						}
					}
					else
					{
						switch(i)//将位置1。
						{
					/*************************************************/	
					//第一组74HC595	 		
						 case 0://亮第一个灯 							     
							hc595_status1=hc595_status1|0x01;//第一个 74HC595 输入量的第0个位置一
								break;
						 case 1://
							hc595_status1=hc595_status1|0x02;
								break;		  		
						 default:
								break;
						}							
					}						
				}
				else
				{
					LED_DATA[i][2]--;		
				}		
		}					
	}
	else
	{	
		keep_time_heartbeat_sbu--;	
	}				
	for(i=0;i<2;i++)//所有灯的倒计时次数减一。轮询一遍。
	{
		if(((LED_DATA[i][0]==1)&&(LED_DATA[i][1]==1))==1)//如果都为1.则常亮。
		{
			LED_status[i]=1;
			LED_ON_TIME[i]=0;	
			LED_OFF_TIME[i]=1;				
		}
		if(((LED_DATA[i][0]==0)&&(LED_DATA[i][1]==0))==1)//如果都为0.则常灭。
		{			
			LED_status[i]=0;//亮状态
			LED_ON_TIME[i]=1;				
			LED_OFF_TIME[i]=0;			
		}		
		if(LED_DATA[i][2]>0)//如果保持时间没有到0。
		{			
			if(LED_status[i]==0)//处于灭状态
			{	
				if(LED_OFF_TIME[i]==0)//如果灭灯倒计时到了时间。就将灯点亮。
				{
					LED_OFF_TIME[i]=LED_DATA[i][1];//装初值
					LED_status[i]=1;				
					switch(i)//将位置1。
					{
				/*************************************************/	
				//第一组74HC595	 		
					 case 0://亮第一个灯 							     
						hc595_status1=hc595_status1|0x01;//第一个 74HC595 输入量的第0个位置一
							break;
					 case 1://
						hc595_status1=hc595_status1|0x02;
							break;		  		
					 default:
							break;
					}					
				}
				else
				{
						LED_OFF_TIME[i]--;			
				}					
			}
			else//处于亮状态
			{
				if(LED_ON_TIME[i]==0)//如果亮灯倒计时到了时间。就将灯灭掉。
				{
					LED_ON_TIME[i]=LED_DATA[i][0];//装初值
					LED_status[i]=0;				
					switch(i)//将位置0。
					{
				/*************************************************/	
				//第一组74HC595	 		
					 case 0://灭第一个灯 							     
						hc595_status1=hc595_status1&0xFE;//第一个 74HC595 输入量的第0个位置0
							break;
					 case 1://
						hc595_status1=hc595_status1&0xFD;
							break;		  		
					 case 2://		 
						hc595_status1=hc595_status1&0xFB;		 											
							break;
					 default:
							break;
					}														
				}
				else
				{
						LED_ON_TIME[i]--;						
				}							
			}			
		}	
		else//如果倒计时到了时间。就将灯熄灭。如果有的灯是要求默认状态为点亮，则在下面继续做判断即可。
		{
			LED_status[i]=0;
					switch(i)//将位置1。
					{
				/*************************************************/	
				//第一组74HC595	 		
					 case 0://亮第一个灯 							     
						hc595_status1=hc595_status1|0x01;//第一个 74HC595 输入量的第0个位置一
							break;
					 case 1://
						hc595_status1=hc595_status1|0x02;
							break;		  		
					 default:
							break;
					}										
		}
	}
	refurbish_hc595();		
}
void HC595_Init(void)//74HC595初始化		 
{
//GPIOE2初始化设置
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);//使能GPIOF时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;				 //LED0-->PA.8 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOE, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.8
	GPIO_SetBits(GPIOE,GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);						 //PA.8 输出高		
	
LED_TWINKLE(0,0,0,0);//常灭。
LED_TWINKLE(1,0,0,0);//常灭。
LED_TWINKLE(2,0,0,0);//常灭。
LED_TWINKLE(3,0,0,0);//常灭。
LED_TWINKLE(4,0,0,0);//常灭。
LED_TWINKLE(5,0,0,0);//常灭。
LED_TWINKLE(6,0,0,0);//常灭。
LED_TWINKLE(7,0,0,0);//常灭。	
LED_TWINKLE(8,0,0,0);//常灭。
LED_TWINKLE(9,0,0,0);//常灭。
LED_TWINKLE(10,0,0,0);//常灭。
LED_TWINKLE(11,0,0,0);//常灭。	
LED_TWINKLE(12,0,0,0);//常灭。
LED_TWINKLE(13,0,0,0);//常灭。
LED_TWINKLE(14,0,0,0);//常灭。
LED_TWINKLE(15,0,0,0);//常灭。	
LED_TWINKLE(16,0,0,0);//常灭。	
LED_TWINKLE(17,0,0,0);//常灭。	
LED_TWINKLE(18,0,0,0);//常灭。
LED_TWINKLE(19,0,0,0);//常灭。
LED_TWINKLE(20,0,0,0);//常灭。
LED_TWINKLE(21,0,0,0);//常灭。
LED_TWINKLE(22,0,0,0);//常灭。
LED_TWINKLE(23,0,0,0);//常灭。
LED_TWINKLE(24,0,0,0);//常灭。
	LED_TWINKLE(GongZuo_LED,1,1,0xFF);
}
void HC595SendData(u8 SendVal)
{  
  u8 i=0;	
//	u8 nop_flag=0;
  for(i=0;i<8;i++) 
   {
		if((SendVal<<i)&0x80) 
			DATA_595=1; //set dataline high  0X80  最高位与SendVal左移的最高位 进行逻辑运算
		else 
			DATA_595=0;				   // 如果为真 MOSIO = 1  
		delay_hc595(40);			
		RCK_595=0;
		delay_hc595(40);			
		RCK_595=1;	
   }
}
void refurbish_hc595(void)//刷新74HC595
{
//	u8 nop_flag=0;
	HC595SendData(hc595_status3);//	
	HC595SendData(hc595_status2);
	HC595SendData(hc595_status1);
	
	SCK_595=0; //set dataline low
		delay_hc595(40);		
	SCK_595=1; //set dataline high	
}
//number 是灯的编号。on_time 是灯闪烁是亮的时间。off_time 是灯闪烁时灭的时间。keep_time 是保持的时间。
//LED_TWINKLE(3,1,1,5);//常亮5秒钟。不闪烁。	
//LED_TWINKLE(3,1,1,255);//常亮。	
//LED_TWINKLE(3,0,0,0);//常灭。
void LED_TWINKLE(u8 number,u8 on_time,u8 off_time,u8 keep_time)//灯闪烁函数
{
 switch(number)
	{
/*************************************************/	
//第一组74HC595	 
	 case 0://全部熄灭。没有选中任何一个灯	 
		LED_DATA[0][0]=on_time;	//灯亮的时间
		LED_DATA[0][1]=off_time;	//灯灭的时间 
		LED_DATA[0][2]=keep_time;//灯闪烁的次数	
		LED_ON_TIME[0]=on_time;//灯打开的时间。变量。在定时器里面使用。
		LED_OFF_TIME[0]=off_time;//灯关闭的时间。变量。在定时器里面使用。		 
			break;		  		
	 case 1://		 
		LED_DATA[1][0]=on_time;	
		LED_DATA[1][1]=off_time;	
		LED_DATA[1][2]=keep_time;	
		LED_ON_TIME[1]=on_time;
		LED_OFF_TIME[1]=off_time;
			break;
	 case 2://  
		LED_DATA[2][0]=on_time;	
		LED_DATA[2][1]=off_time;	
		LED_DATA[2][2]=keep_time;	
		LED_ON_TIME[2]=on_time;
		LED_OFF_TIME[2]=off_time;	 
			break;
	 case 3://	 							 
		LED_DATA[3][0]=on_time;	
		LED_DATA[3][1]=off_time;	
		LED_DATA[3][2]=keep_time;	
		LED_ON_TIME[3]=on_time;
		LED_OFF_TIME[3]=off_time;	 
			break;	 
	 case 4://
		LED_DATA[4][0]=on_time;	
		LED_DATA[4][1]=off_time;	
		LED_DATA[4][2]=keep_time;	 
		LED_ON_TIME[4]=on_time;
		LED_OFF_TIME[4]=off_time;	 
			break;	 		 		
	 case 5://
		LED_DATA[5][0]=on_time;	
		LED_DATA[5][1]=off_time;	
		LED_DATA[5][2]=keep_time;	
		LED_ON_TIME[5]=on_time;
		LED_OFF_TIME[5]=off_time;	 
			break;
	 case 6://
		LED_DATA[6][0]=on_time;	
		LED_DATA[6][1]=off_time;	
		LED_DATA[6][2]=keep_time;	
		LED_ON_TIME[6]=on_time;
		LED_OFF_TIME[6]=off_time;	 
			break;
	 case 7://
		LED_DATA[7][0]=on_time;	
		LED_DATA[7][1]=off_time;	
		LED_DATA[7][2]=keep_time;	
		LED_ON_TIME[7]=on_time;
		LED_OFF_TIME[7]=off_time;	 
			break;
/*************************************************/	
	 case 8://
		LED_DATA[8][0]=on_time;	
		LED_DATA[8][1]=off_time;	
		LED_DATA[8][2]=keep_time;	
		LED_ON_TIME[8]=on_time;
		LED_OFF_TIME[8]=off_time;	 
			break;		  		
	 case 9://		 
		LED_DATA[9][0]=on_time;	
		LED_DATA[9][1]=off_time;	
		LED_DATA[9][2]=keep_time;	 
		LED_ON_TIME[9]=on_time;
		LED_OFF_TIME[9]=off_time;	 
			break;
	 case 10://  
		LED_DATA[10][0]=on_time;	
		LED_DATA[10][1]=off_time;	
		LED_DATA[10][2]=keep_time;
		LED_ON_TIME[10]=on_time;
		LED_OFF_TIME[10]=off_time;	 
			break;
	 case 11://	 							 
		LED_DATA[11][0]=on_time;	
		LED_DATA[11][1]=off_time;	
		LED_DATA[11][2]=keep_time;
		LED_ON_TIME[11]=on_time;
		LED_OFF_TIME[11]=off_time;	 
			break;	 
	 case 12://
		LED_DATA[12][0]=on_time;	
		LED_DATA[12][1]=off_time;	
		LED_DATA[12][2]=keep_time;	
		LED_ON_TIME[12]=on_time;
		LED_OFF_TIME[12]=off_time;	 
			break;	 		 		
	 case 13://
		LED_DATA[13][0]=on_time;	
		LED_DATA[13][1]=off_time;	
		LED_DATA[13][2]=keep_time;	
		LED_ON_TIME[13]=on_time;
		LED_OFF_TIME[13]=off_time;	 
			break;
	 case 14://
		LED_DATA[14][0]=on_time;	
		LED_DATA[14][1]=off_time;	
		LED_DATA[14][2]=keep_time;
		LED_ON_TIME[14]=on_time;
		LED_OFF_TIME[14]=off_time;	 
			break;
	 case 15://
		LED_DATA[15][0]=on_time;	
		LED_DATA[15][1]=off_time;	
		LED_DATA[15][2]=keep_time;	 
		LED_ON_TIME[15]=on_time;
		LED_OFF_TIME[15]=off_time;	 
			break;
/*************************************************/	
	 case 16:// 
		LED_DATA[16][0]=on_time;	
		LED_DATA[16][1]=off_time;	
		LED_DATA[16][2]=keep_time;	
		LED_ON_TIME[16]=on_time;
		LED_OFF_TIME[16]=off_time;	 
			break;		  		
	 case 17://	
		LED_DATA[17][0]=on_time;	
		LED_DATA[17][1]=off_time;	
		LED_DATA[17][2]=keep_time;	
		LED_ON_TIME[17]=on_time;
		LED_OFF_TIME[17]=off_time;	 
			break;
	 case 18:// 
		LED_DATA[18][0]=on_time;	
		LED_DATA[18][1]=off_time;	
		LED_DATA[18][2]=keep_time;	
		LED_ON_TIME[18]=on_time;
		LED_OFF_TIME[18]=off_time;	 
			break;
	 case 19://	
		LED_DATA[19][0]=on_time;	
		LED_DATA[19][1]=off_time;	
		LED_DATA[19][2]=keep_time;	
		LED_ON_TIME[19]=on_time;
		LED_OFF_TIME[19]=off_time;	 
			break;	 
	 case 20://
		LED_DATA[20][0]=on_time;	
		LED_DATA[20][1]=off_time;	
		LED_DATA[20][2]=keep_time;	
		LED_ON_TIME[20]=on_time;
		LED_OFF_TIME[20]=off_time;	 
			break;	 		 		
	 case 21://
		LED_DATA[21][0]=on_time;	
		LED_DATA[21][1]=off_time;	
		LED_DATA[21][2]=keep_time;	
		LED_ON_TIME[21]=on_time;
		LED_OFF_TIME[21]=off_time;	 
			break;
	 case 22://
		LED_DATA[22][0]=on_time;	
		LED_DATA[22][1]=off_time;	
		LED_DATA[22][2]=keep_time;	
		LED_ON_TIME[22]=on_time;
		LED_OFF_TIME[22]=off_time;	 
			break;
	 case 23://
		LED_DATA[23][0]=on_time;	
		LED_DATA[23][1]=off_time;	
		LED_DATA[23][2]=keep_time;
		LED_ON_TIME[23]=on_time;
		LED_OFF_TIME[23]=off_time;	 
			break; 
/*************************************************/	
	 default:
	 		break;
	}	
}
void delay_hc595(u16 once_hc595)//刷新74HC595的时候的nop
{
	u8 nop_flag=0;
	u16 i=0;
	  for(i=0;i<once_hc595;i++) 
	   {
			nop_flag=~nop_flag;			
	   }
}
//
