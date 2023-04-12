#ifndef __RTC_H
#define __RTC_H
#include "sys.h"

//ʱ��ṹ��
typedef struct
{
    vu8 hour;
    vu8 min;
    vu8 sec;
    //������������
    vu16 w_year;
    vu8  w_month;
    vu8  w_date;
    vu8  week;
} _calendar_obj;

extern _calendar_obj calendar;

u8 RTC_Init(void);
u8 RTC_Get(void);
u8 RTC_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec);
void RTC_Test(void);
#endif

/***********************end of file**************************/
