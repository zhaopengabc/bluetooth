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

 _calendar_obj calendar;//ʱ�ӽṹ��

u8 const table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5}; //���������ݱ�
const u8 mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; //ƽ����·����ڱ�

u8 RTC_Get_Week(u16 year, u8 month, u8 day);

//�ж��Ƿ������꺯��
//�·�   1  2  3  4  5  6  7  8  9  10 11 12
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31
//����:���
//���:������ǲ�������.1,��.0,����
u8 Is_Leap_Year(u16 year)
{
    if(year % 4 == 0) //�����ܱ�4����
    {
        if(year % 100 == 0)
        {
            if(year % 400 == 0)
			{
				return 1; //�����00��β,��Ҫ�ܱ�400����
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
//    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		//RTCȫ���ж�
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�1λ,�����ȼ�3λ
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//��ռ���ȼ�0λ,�����ȼ�4λ
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//ʹ�ܸ�ͨ���ж�
//    NVIC_Init(&NVIC_InitStructure);		//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
//}
//�õ���ǰ��ʱ��
//����ֵ:0,�ɹ�;����:�������.
u8 RTC_Get_DATA(void)
{
    static u16 daycnt = 0;
    u32 timecount = 0;
    u32 temp = 0;
    u16 temp1 = 0;
    timecount = RTC_GetCounter();
    temp = timecount / 86400;				//�õ�����(��������Ӧ��)
    if(daycnt != temp)						//����һ����
    {
        daycnt = temp;
        temp1 = 1970;						//��1970�꿪ʼ
        while(temp >= 365)
        {
            if(Is_Leap_Year(temp1))			//������
            {
                if(temp >= 366)
				{
					temp -= 366;			//�����������
				}
                else
                {
                    temp1++;
                    break;
                }
            }
            else temp -= 365;				//ƽ��
            temp1++;
        }
        calendar.w_year = temp1;			//�õ����
        temp1 = 0;
        while(temp >= 28)					//������һ����
        {
            if(Is_Leap_Year(calendar.w_year) && temp1 == 1) //�����ǲ�������/2�·�
            {
                if(temp >= 29)temp -= 29;	//�����������
                else break;
            }
            else
            {
                if(temp >= mon_table[temp1])temp -= mon_table[temp1]; //ƽ��
                else break;
            }
            temp1++;
        }
        calendar.w_month = temp1 + 1;	//�õ��·�
        calendar.w_date = temp + 1;  	//�õ�����
    }
    temp = timecount % 86400;     		//�õ�������
    calendar.hour = temp / 3600;     	//Сʱ
    calendar.min = (temp % 3600) / 60; 	//����
    calendar.sec = (temp % 3600) % 60; 	//����
    calendar.week = RTC_Get_Week(calendar.w_year, calendar.w_month, calendar.w_date); //��ȡ����
    return 0;
}

u8 RTC_Get(char *data)
{
  RTC_Get_DATA();
	
	data[0]=calendar.sec;//��
	data[1]=calendar.min;//��
	data[2]=calendar.hour;//ʱ
	data[3]=calendar.w_date;//��
	data[4]=calendar.w_month;//��
	data[5]=calendar.w_year%2000;//��	
	
}
//ʵʱʱ������
//��ʼ��RTCʱ��,ͬʱ���ʱ���Ƿ�������
//BKP->DR1���ڱ����Ƿ��һ�����õ�����
//����0:����
//����:�������
u8 RTC_Init(void)
{
    //����ǲ��ǵ�һ������ʱ��
    u16 temp = 0;
	  u8  time[6]={01, 21, 14, 10, 8, 18};;
		
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
   if (BKP_ReadBackupRegister(BKP_DR1) != 0x5050)
//		    if (1)//����ʱ��
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
			printf("rtc��ʼ��ʧ��");
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
//RTCʱ���ж�
//ÿ�봥��һ��
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

//����ʱ��
//�������ʱ��ת��Ϊ����
//��1970��1��1��Ϊ��׼
//1970~2099��Ϊ�Ϸ����
//����ֵ:0,�ɹ�;����:�������.
//�·����ݱ�
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
    for(t = 1970; t < syear; t++)			//��������ݵ��������
    {
        if(Is_Leap_Year(t))
		{
			seccount += 31622400;			//�����������
		}
        else 
		{
			seccount += 31536000;			//ƽ���������
		}
    }
    smon -= 1;
    for(t = 0; t < smon; t++)				//��ǰ���·ݵ����������
    {
        seccount += (u32)mon_table[t] * 86400; //�·����������
        if(Is_Leap_Year(syear) && t == 1)
		{
			seccount += 86400; //����2�·�����һ���������
		}
    }
    seccount += (u32)(sday - 1) * 86400;	//��ǰ�����ڵ����������
    seccount += (u32)hour * 3600;			//Сʱ������
    seccount += (u32)min * 60;				//����������
    seccount += sec;						//�������Ӽ���ȥ

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��
    PWR_BackupAccessCmd(ENABLE);			//ʹ��RTC�ͺ󱸼Ĵ�������
    RTC_SetCounter(seccount);				//����RTC��������ֵ

    RTC_WaitForLastTask();					//�ȴ����һ�ζ�RTC�Ĵ�����д�������
    return 0;
}
 
//������������ڼ�
//��������:���빫�����ڵõ�����(ֻ����1901-2099��)
//�������������������
//����ֵ�����ں�
u8 RTC_Get_Week(u16 year, u8 month, u8 day)
{
    u16 temp2;
    u8 yearH, yearL;

    yearH = year / 100;
    yearL = year % 100;
    // ���Ϊ21����,�������100
    if (yearH > 19)
		{
			yearL += 100;
		}
    // ����������ֻ��1900��֮���
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
//	SystemTime[0]=data[0];//��
//	SystemTime[1]=data[1];//��
//	SystemTime[2]=data[2];//ʱ
//	SystemTime[3]=data[3];//��
//	SystemTime[4]=data[4];//��
//	SystemTime[5]=data[5];//��	
//}

//static char TimeAddCont100mS=0;
//u8 RTC_add(void)
//{
//	TimeAddCont100mS++;
//	if(TimeAddCont100mS>10)//1S	
//	{
//		SystemTime[0]=SystemTime[0]+1;//��+
//		if(SystemTime[0]>59)
//		{
//			SystemTime[0]=0;
//			SystemTime[1]=SystemTime[1]+1;//��+
//		}
//		if(SystemTime[1]>59)
//		{
//			SystemTime[1]=0;
//			SystemTime[2]=SystemTime[1]+1;//ʱ+
//		}
//		if(SystemTime[2]>23)
//		{
//			SystemTime[2]=0;
//			SystemTime[3]=SystemTime[3]+1;//��+
//		}		
//		if(SystemTime[3]>29)
//		{
//			SystemTime[3]=0;
//			SystemTime[4]=SystemTime[4]+1;//��+
//		}			
//		if(SystemTime[4]>12)
//		{
//			SystemTime[4]=0;
//			SystemTime[5]=SystemTime[5]+1;//��+
//		}			
//	}
//}
void RTC_Test(void)
{
	RTC_Get_DATA();
	printf("Real Time:%d-%d-%d %d:%d:%d\n", calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec);
}
/***********************end of file**************************/