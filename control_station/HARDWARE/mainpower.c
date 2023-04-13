#include "mainpower.h"
#include "sys.h"
#include "includes.h"
#include "stm32f10x_adc.h"
#include "voice.h"

//#define MPower_Read PDin(1)
#define MPower_Read  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
//#define SPower_Read  PAin(1)
//#define SPower_Read2 PCin(8)
#define SPower_Read2 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)

//#define SPowerSw_H() 	{GPIO_SetBits(GPIOC,GPIO_Pin_8);} 
//#define SPowerSw_L() 	{GPIO_ResetBits(GPIOC,GPIO_Pin_8);} 

//#define MPower_Read		GPIO_ReadInputDataBit(GPIOD,1)
//#define SPower_Read		GPIO_ReadInputDataBit(GPIOD,3)
#define DEBOUNCE_TIMES 70

//初始化ADC
//这里我们仅以规则通道为例
//我们默认将开启通道0~3			
//PA1
void  Adc_Init_PA0(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC2	, ENABLE );	  //使能ADC2通道时钟
 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6      72M/6=12,     ADC最大时间不能超过14M

	//PA1 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC2);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC2, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

	ADC_Cmd(ADC2, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC2);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC2));	//等待复位校准结束
	
	ADC_StartCalibration(ADC2);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC2));	 //等待校准结束
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

}

//初始化ADC
//这里我们仅以规则通道为例
//我们默认将开启通道0~3			
//PA1
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC1通道时钟
 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6      72M/6=12,     ADC最大时间不能超过14M

	//PA1 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

}

//获得ADC值
//ch:通道值 0~3
u16 Get_Adc_PA0(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC2, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC2,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC2, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC2);	//返回最近一次ADC1规则组的转换结果
}

//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

//PC10-主电状态  P11-备电状态
void PowerDetec_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
	
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
 
	  Adc_Init_PA0();
	  Adc_Init();
}
//u16 Get_Adc_Average(u8 ch,u8 times)
//{
//	u32 temp_val=0;
//	u8 t;
//	OS_ERR err;
//	
//	for(t=0;t<times;t++)
//	{
//		temp_val+=Get_Adc(ch);
////		delay_ms(5);
//		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_HMSM_STRICT,&err);
//	}
//	return temp_val/times;
//} 
u8 Get_Adc_Average_PA0(void)
{
	u16 temp_val[20];
	u16 v=0;
	u8 t,i=0,j=0,nbr;
	OS_ERR err;
	u32 AD1=0,AD2=0;
	
  float a=0;
	nbr=sizeof(temp_val);
	memset(temp_val,0,nbr);
	//数组采集
	for(t=0;t<20;t++)//20次
	{
		temp_val[t]=Get_Adc_PA0(0);
//		delay_ms(5);
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_HMSM_STRICT,&err);
	}
	//数组排序
	for(j=0;j<20;j++)
	  for(i=0;i<20-1-j;i++)
		if(temp_val[i]>temp_val[i+1]) /* 由小到大,由大到小时改为< */
		{
		v=temp_val[i];
		temp_val[i]=temp_val[i+1];
		temp_val[i+1]=v;
		}
	
	for(i=0;i<20;i++)
//	printf("%d ",temp_val[i]);
		
	a=temp_val[10];
	a=(a/4096);
	a=a*3.3;
	AD1=a*1000;	
//	printf("AD电压=%dmV!",AD1);
//	a=(a*3)/2;	
//	AD2=a*1000;			
//	printf("~PA0=%dA!",a);
//	printf("\r\n");	
//	if(temp_val[10]>3061)//大于3.7V，正常；.V/4096*3.3=;  3.7V/3*2=2.26V  2.26v/3.3v*4096=3061=阀值		
//	if(temp_val[10]>2730)//大于3.3V，正常；.V/4096*3.3=;  3.3V/3*2=2.20V  2.20v/3.3v*4096=2730=阀值
//	if(temp_val[10]>2565)//大于3.1V，正常；.V/4096*3.3=;  3.1V/3*2=2.06V  2.06v/3.3v*4096=2565=阀值		
//	if(temp_val[10]>3103)//2.5v/3.3v*4096=3103=阀值	
	if(temp_val[10]>2854)//2.3v/3.3v*4096=3103=阀值			
	{	
//		printf("~PA0=1,%dmV!",AD1);
		return 1;
	}
	else
	{	
//		printf("~PA0=0,%dmV!",AD1);
	  return 0;
	}
} 


u8 Get_Adc_Average(void)
{
	u16 temp_val[20];
	u16 v=0;
	u8 t,i=0,j=0,nbr;
	OS_ERR err;
	u32 AD1=0,AD2=0;
	
  float a=0;
	nbr=sizeof(temp_val);
	memset(temp_val,0,nbr);
	//数组采集
	for(t=0;t<20;t++)//20次
	{
		temp_val[t]=Get_Adc(1);
//		delay_ms(5);
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_HMSM_STRICT,&err);
	}
	//数组排序
	for(j=0;j<20;j++)
	  for(i=0;i<20-1-j;i++)
		if(temp_val[i]>temp_val[i+1]) /* 由小到大,由大到小时改为< */
		{
		v=temp_val[i];
		temp_val[i]=temp_val[i+1];
		temp_val[i+1]=v;
		}
	
	for(i=0;i<20;i++)
//	printf("%d ",temp_val[i]);
		
	a=temp_val[10];
	a=(a/4096);
	a=a*3.3;
	AD1=a*1000;	
//	printf("AD电压=%dmV!",AD1);
	a=(a*3)/2;	
	AD2=a*1000;			
//	printf("~BT=%dmV!",AD2);
	
	if(temp_val[10]>2730)//大于3.3V，正常；.V/4096*3.3=;  3.3V/3*2=2.20V  2.20v/3.3v*4096=2730=阀值	
	{	
		return 1;
	}
	else
	{	
	  return 0;
	}
} 

//检测主备电状态
//主电信号：因220V输入电压过低或断电时，输出低电平，主电正常工作时输出高电平；
//备电信号：备电正常输出额定电压时，输出低电平；
//主电正常工作，备电发生欠压（低于9V）、短路、断路等不能正常提供输出时，输出高电平；
//主电故障时，备电电压低于11V时，输出高电平；
//返回参数：0-主备电工作正常，1-主备电工作故障
//bit0-主电状态
//bit1-备电状态
unsigned char Power_Detection(void)
{
//gRunState
//0:正常监视状态-测试状态  1：火警
//2：故障  3：主电故障 4：备电故障
//5：与监控中心通信故障 6：监测连接线路故障
	unsigned char flag=0;
	OS_ERR err;
	unsigned char A1=0,A2=0;
	unsigned char PA0_AD=0;
	
	PA0_AD=Get_Adc_Average_PA0();
	if(PA0_AD==1)
//	if(MPower_Read==1)//正常；
	{
 		flag&= ~(0x01<<3);
	}
	else//故障
	{
		flag|=0x01<<3;
	}
	//备电采集
	//有主电：断开MOS管，采集电池电压。
	//没有主电：不断开MOS管，采集电池电压。
	//备电低于3.3V，系统关闭功耗电路。
	//SW低，mos通

	//AD采集电池电压；
  //备电采集
	A1=SPower_Read2;//备电开关正常；
	A2=Get_Adc_Average();//电池电压正常；
//	printf("A1=%d(0为闭合,1为断开)!",A1);
	if((A1==0)&&(A2==1))// 正常
	{
		flag&= ~(0x01<<4);
	}
	else//故障
	{
	flag|= 0x01<<4;
	}
	
	return flag;
}
unsigned char Power_State(void)
{
	static unsigned char count=0;
	static unsigned char flagbuf[DEBOUNCE_TIMES];
	unsigned char i=0;
	static unsigned char RunState=0;
	
	count++;
	if(count>=DEBOUNCE_TIMES)count=0;
	flagbuf[count]=Power_Detection();
	
	for(i=0;i<DEBOUNCE_TIMES;i++)
	{
		if((flagbuf[i]&(1<<3))!=(flagbuf[count]&(1<<3)))break;
	}
	if(i>=DEBOUNCE_TIMES)
	{
		RunState&=~(1<<3);
		RunState|=flagbuf[count]&(1<<3);
	}
	
	for(i=0;i<DEBOUNCE_TIMES;i++)
	{
		if((flagbuf[i]&(1<<4))!=(flagbuf[count]&(1<<4)))break;
	}
	if(i>=DEBOUNCE_TIMES)
	{
		RunState&=~(1<<4);
		RunState|=flagbuf[count]&(1<<4);
	}
	return RunState;
}
