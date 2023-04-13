#include "rtc.h"
#include "stdio.h"
#include "string.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_pwr.h"
#include "includes.h"
#include "lcd.h"
#include "uart5.h"
#include "usart.h"
#include "delay.h"
#include "app.h"

 _calendar_obj calendar;//时钟结构体

u8 const table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5}; //月修正数据表
const u8 mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; //平年的月份日期表

u8 RTC_Get_Week(u16 year, u8 month, u8 day);

//判断是否是闰年函数
//月份   1  2  3  4  5  6  7  8  9  10 11 12
//闰年   31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31
//输入:年份
//输出:该年份是不是闰年.1,是.0,不是
u8 Is_Leap_Year(u16 year)
{
    if(year % 4 == 0) //必须能被4整除
    {
        if(year % 100 == 0)
        {
            if(year % 400 == 0)
			{
				return 1; //如果以00结尾,还要能被400整除
			}
            else 
			{
				return 0;
			}
        }
        else return 1;
    }
    else return 0;
}
//static void RTC_NVIC_Config(void)
//{
//    NVIC_InitTypeDef NVIC_InitStructure;
//    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		//RTC全局中断
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//先占优先级1位,从优先级3位
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//先占优先级0位,从优先级4位
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//使能该通道中断
//    NVIC_Init(&NVIC_InitStructure);		//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
//}
//得到当前的时间
//返回值:0,成功;其他:错误代码.
u8 RTC_Get_DATA(void)
{
    static u16 daycnt = 0;
    u32 timecount = 0;
    u32 temp = 0;
    u16 temp1 = 0;
    timecount = RTC_GetCounter();
    temp = timecount / 86400;				//得到天数(秒钟数对应的)
    if(daycnt != temp)						//超过一天了
    {
        daycnt = temp;
        temp1 = 1970;						//从1970年开始
        while(temp >= 365)
        {
            if(Is_Leap_Year(temp1))			//是闰年
            {
                if(temp >= 366)
				{
					temp -= 366;			//闰年的秒钟数
				}
                else
                {
                    temp1++;
                    break;
                }
            }
            else temp -= 365;				//平年
            temp1++;
        }
        calendar.w_year = temp1;			//得到年份
        temp1 = 0;
        while(temp >= 28)					//超过了一个月
        {
            if(Is_Leap_Year(calendar.w_year) && temp1 == 1) //当年是不是闰年/2月份
            {
                if(temp >= 29)temp -= 29;	//闰年的秒钟数
                else break;
            }
            else
            {
                if(temp >= mon_table[temp1])temp -= mon_table[temp1]; //平年
                else break;
            }
            temp1++;
        }
        calendar.w_month = temp1 + 1;	//得到月份
        calendar.w_date = temp + 1;  	//得到日期
    }
    temp = timecount % 86400;     		//得到秒钟数
    calendar.hour = temp / 3600;     	//小时
    calendar.min = (temp % 3600) / 60; 	//分钟
    calendar.sec = (temp % 3600) % 60; 	//秒钟
    calendar.week = RTC_Get_Week(calendar.w_year, calendar.w_month, calendar.w_date); //获取星期
    return 0;
}

u8 RTC_Get(char *data)
{
  RTC_Get_DATA();
	
	data[0]=calendar.sec;//秒
	data[1]=calendar.min;//分
	data[2]=calendar.hour;//时
	data[3]=calendar.w_date;//天
	data[4]=calendar.w_month;//月
	data[5]=calendar.w_year%2000;//年	
	
}
//实时时钟配置
//初始化RTC时钟,同时检测时钟是否工作正常
//BKP->DR1用于保存是否第一次配置的设置
//返回0:正常
//其他:错误代码
u8 RTC_Init(void)
{
    //检查是不是第一次配置时钟
    u16 temp = 0;
	  u8  time[6]={01, 21, 14, 10, 8, 18};;
		
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
   if (BKP_ReadBackupRegister(BKP_DR1) != 0x5050)
//		    if (1)//配置时钟
    {
        BKP_DeInit();
        RCC_LSEConfig(RCC_LSE_ON);
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET && temp < 0xf0)
        {
            temp++;
			      delay_ms(10);
        }
			
        if(temp >= 0xf0)
		{
			printf("rtc初始化失败");
			return 1;
		}
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        RCC_RTCCLKCmd(ENABLE);
        RTC_WaitForLastTask();
        RTC_WaitForSynchro();
//        RTC_ITConfig(RTC_IT_SEC, ENABLE);
        RTC_WaitForLastTask();
        RTC_EnterConfigMode();
        RTC_SetPrescaler(32767);
        RTC_WaitForLastTask();		   		
        RTC_Set((char *)&time);
        RTC_ExitConfigMode();
        BKP_WriteBackupRegister(BKP_DR1, 0X5050);
    }
    else
    {
        RTC_WaitForSynchro();
//        RTC_ITConfig(RTC_IT_SEC, ENABLE);
        RTC_WaitForLastTask();
    }
//    RTC_NVIC_Config();
    RTC_Get_DATA();
    return 0;
}
//RTC时钟中断
//每秒触发一次
void RTC_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
    {
			
        RTC_Get_DATA();
		printf("Real Time:%d-%d-%d %d:%d:%d\n", calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec);
    }
    RTC_ClearITPendingBit(RTC_IT_SEC | RTC_IT_OW);
    RTC_WaitForLastTask();
}

//设置时钟
//把输入的时钟转换为秒钟
//以1970年1月1日为基准
//1970~2099年为合法年份
//返回值:0,成功;其他:错误代码.
//月份数据表
//u8 RTC_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec)
u8 RTC_Set(char *data)
{
    u16 t;
    u32 seccount = 0;
	
	  u16 syear=0; 
	  u8 smon=0; 
	  u8 sday=0; 
	  u8 hour=0; 
	  u8 min=0;
	  u8 sec=0;

	  sec=data[0];
	  min=data[1];
	  hour=data[2];
	  sday=data[3];	
	  smon=data[4];
	  syear=data[5]+2000;	
	
	
    if(syear < 1970 || syear > 2099)
		{
			return 1;
		}
    for(t = 1970; t < syear; t++)			//把所有年份的秒钟相加
    {
        if(Is_Leap_Year(t))
		{
			seccount += 31622400;			//闰年的秒钟数
		}
        else 
		{
			seccount += 31536000;			//平年的秒钟数
		}
    }
    smon -= 1;
    for(t = 0; t < smon; t++)				//把前面月份的秒钟数相加
    {
        seccount += (u32)mon_table[t] * 86400; //月份秒钟数相加
        if(Is_Leap_Year(syear) && t == 1)
		{
			seccount += 86400; //闰年2月份增加一天的秒钟数
		}
    }
    seccount += (u32)(sday - 1) * 86400;	//把前面日期的秒钟数相加
    seccount += (u32)hour * 3600;			//小时秒钟数
    seccount += (u32)min * 60;				//分钟秒钟数
    seccount += sec;						//最后的秒钟加上去

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟
    PWR_BackupAccessCmd(ENABLE);			//使能RTC和后备寄存器访问
    RTC_SetCounter(seccount);				//设置RTC计数器的值

    RTC_WaitForLastTask();					//等待最近一次对RTC寄存器的写操作完成
    return 0;
}
 
//获得现在是星期几
//功能描述:输入公历日期得到星期(只允许1901-2099年)
//输入参数：公历年月日
//返回值：星期号
u8 RTC_Get_Week(u16 year, u8 month, u8 day)
{
    u16 temp2;
    u8 yearH, yearL;

    yearH = year / 100;
    yearL = year % 100;
    // 如果为21世纪,年份数加100
    if (yearH > 19)
		{
			yearL += 100;
		}
    // 所过闰年数只算1900年之后的
    temp2 = yearL + yearL / 4;
    temp2 = temp2 % 7;
    temp2 = temp2 + day + table_week[month - 1];
    if (yearL % 4 == 0 && month < 3)
		{
			temp2--;
		}
    return(temp2 % 7);
}
 

 

//u8 RTC_Set(char *data)
//{
//	SystemTime[0]=data[0];//秒
//	SystemTime[1]=data[1];//分
//	SystemTime[2]=data[2];//时
//	SystemTime[3]=data[3];//天
//	SystemTime[4]=data[4];//月
//	SystemTime[5]=data[5];//年	
//}

//static char TimeAddCont100mS=0;
//u8 RTC_add(void)
//{
//	TimeAddCont100mS++;
//	if(TimeAddCont100mS>10)//1S	
//	{
//		SystemTime[0]=SystemTime[0]+1;//秒+
//		if(SystemTime[0]>59)
//		{
//			SystemTime[0]=0;
//			SystemTime[1]=SystemTime[1]+1;//分+
//		}
//		if(SystemTime[1]>59)
//		{
//			SystemTime[1]=0;
//			SystemTime[2]=SystemTime[1]+1;//时+
//		}
//		if(SystemTime[2]>23)
//		{
//			SystemTime[2]=0;
//			SystemTime[3]=SystemTime[3]+1;//天+
//		}		
//		if(SystemTime[3]>29)
//		{
//			SystemTime[3]=0;
//			SystemTime[4]=SystemTime[4]+1;//月+
//		}			
//		if(SystemTime[4]>12)
//		{
//			SystemTime[4]=0;
//			SystemTime[5]=SystemTime[5]+1;//年+
//		}			
//	}
//}
void RTC_Test(void)
{
	RTC_Get_DATA();
	printf("Real Time:%d-%d-%d %d:%d:%d\n", calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec);
}
/***********************end of file**************************/