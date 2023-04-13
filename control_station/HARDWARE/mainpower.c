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

//��ʼ��ADC
//�������ǽ��Թ���ͨ��Ϊ��
//����Ĭ�Ͻ�����ͨ��0~3			
//PA1
void  Adc_Init_PA0(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC2	, ENABLE );	  //ʹ��ADC2ͨ��ʱ��
 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6      72M/6=12,     ADC���ʱ�䲻�ܳ���14M

	//PA1 ��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC2);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC2, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

	ADC_Cmd(ADC2, ENABLE);	//ʹ��ָ����ADC1
	
	ADC_ResetCalibration(ADC2);	//ʹ�ܸ�λУ׼  
	 
	while(ADC_GetResetCalibrationStatus(ADC2));	//�ȴ���λУ׼����
	
	ADC_StartCalibration(ADC2);	 //����ADУ׼
 
	while(ADC_GetCalibrationStatus(ADC2));	 //�ȴ�У׼����
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

}

//��ʼ��ADC
//�������ǽ��Թ���ͨ��Ϊ��
//����Ĭ�Ͻ�����ͨ��0~3			
//PA1
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6      72M/6=12,     ADC���ʱ�䲻�ܳ���14M

	//PA1 ��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	
	ADC_StartCalibration(ADC1);	 //����ADУ׼
 
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

}

//���ADCֵ
//ch:ͨ��ֵ 0~3
u16 Get_Adc_PA0(u8 ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC2, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC2,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC2, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC2);	//�������һ��ADC1�������ת�����
}

//���ADCֵ
//ch:ͨ��ֵ 0~3
u16 Get_Adc(u8 ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}

//PC10-����״̬  P11-����״̬
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
	//����ɼ�
	for(t=0;t<20;t++)//20��
	{
		temp_val[t]=Get_Adc_PA0(0);
//		delay_ms(5);
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_HMSM_STRICT,&err);
	}
	//��������
	for(j=0;j<20;j++)
	  for(i=0;i<20-1-j;i++)
		if(temp_val[i]>temp_val[i+1]) /* ��С����,�ɴ�Сʱ��Ϊ< */
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
//	printf("AD��ѹ=%dmV!",AD1);
//	a=(a*3)/2;	
//	AD2=a*1000;			
//	printf("~PA0=%dA!",a);
//	printf("\r\n");	
//	if(temp_val[10]>3061)//����3.7V��������.V/4096*3.3=;  3.7V/3*2=2.26V  2.26v/3.3v*4096=3061=��ֵ		
//	if(temp_val[10]>2730)//����3.3V��������.V/4096*3.3=;  3.3V/3*2=2.20V  2.20v/3.3v*4096=2730=��ֵ
//	if(temp_val[10]>2565)//����3.1V��������.V/4096*3.3=;  3.1V/3*2=2.06V  2.06v/3.3v*4096=2565=��ֵ		
//	if(temp_val[10]>3103)//2.5v/3.3v*4096=3103=��ֵ	
	if(temp_val[10]>2854)//2.3v/3.3v*4096=3103=��ֵ			
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
	//����ɼ�
	for(t=0;t<20;t++)//20��
	{
		temp_val[t]=Get_Adc(1);
//		delay_ms(5);
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_HMSM_STRICT,&err);
	}
	//��������
	for(j=0;j<20;j++)
	  for(i=0;i<20-1-j;i++)
		if(temp_val[i]>temp_val[i+1]) /* ��С����,�ɴ�Сʱ��Ϊ< */
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
//	printf("AD��ѹ=%dmV!",AD1);
	a=(a*3)/2;	
	AD2=a*1000;			
//	printf("~BT=%dmV!",AD2);
	
	if(temp_val[10]>2730)//����3.3V��������.V/4096*3.3=;  3.3V/3*2=2.20V  2.20v/3.3v*4096=2730=��ֵ	
	{	
		return 1;
	}
	else
	{	
	  return 0;
	}
} 

//���������״̬
//�����źţ���220V�����ѹ���ͻ�ϵ�ʱ������͵�ƽ��������������ʱ����ߵ�ƽ��
//�����źţ���������������ѹʱ������͵�ƽ��
//�����������������緢��Ƿѹ������9V������·����·�Ȳ��������ṩ���ʱ������ߵ�ƽ��
//�������ʱ�������ѹ����11Vʱ������ߵ�ƽ��
//���ز�����0-�����繤��������1-�����繤������
//bit0-����״̬
//bit1-����״̬
unsigned char Power_Detection(void)
{
//gRunState
//0:��������״̬-����״̬  1����
//2������  3��������� 4���������
//5����������ͨ�Ź��� 6�����������·����
	unsigned char flag=0;
	OS_ERR err;
	unsigned char A1=0,A2=0;
	unsigned char PA0_AD=0;
	
	PA0_AD=Get_Adc_Average_PA0();
	if(PA0_AD==1)
//	if(MPower_Read==1)//������
	{
 		flag&= ~(0x01<<3);
	}
	else//����
	{
		flag|=0x01<<3;
	}
	//����ɼ�
	//�����磺�Ͽ�MOS�ܣ��ɼ���ص�ѹ��
	//û�����磺���Ͽ�MOS�ܣ��ɼ���ص�ѹ��
	//�������3.3V��ϵͳ�رչ��ĵ�·��
	//SW�ͣ�mosͨ

	//AD�ɼ���ص�ѹ��
  //����ɼ�
	A1=SPower_Read2;//���翪��������
	A2=Get_Adc_Average();//��ص�ѹ������
//	printf("A1=%d(0Ϊ�պ�,1Ϊ�Ͽ�)!",A1);
	if((A1==0)&&(A2==1))// ����
	{
		flag&= ~(0x01<<4);
	}
	else//����
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
