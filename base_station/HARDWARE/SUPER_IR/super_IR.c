#include "sys.h"
#include "super_IR.h"
#include <stdbool.h>
/**************ȫ�ֱ���****************************************************/
unsigned char hh_w,ll_w;           //��,�͵�ƽ���
unsigned char ma_x;                //���յ��ڼ�λ������
unsigned char old_rc5;             //������һ�β�ѯ���ĵ�ƽ״̬
unsigned char tb_ok;               //���յ�ͬ������ʱ��1
unsigned long int tb_data=0;       //���յ�������
bool tb_data_i=0;                  //���ڴ��ÿλ
unsigned int tb_time=0;            //���ڼ�ʱ�������һ��ʱ����û�м�⵽�ߵ�ƽ������
unsigned long int RF_address=0;    //ȫ�ֱ��������ڴ洢
unsigned char RF_data=0;           //ȫ�ֱ��������ڴ������
unsigned char Rf_ok_flag=0;        //ȫ�ֱ��������յ�һ��������ң���������1,֪ͨ���������Խ�����

/**********************************************************************
* ��������: 433M IO�ĳ�ʼ������
* �������: ��
* �� �� ֵ: ��
* ˵    ��: ��
***********************************************************************/
void RF_GPIO_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PC�˿�ʱ��

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				 	//SUPER_IR_IN-->PC.0 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 	//����ģʽ
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 		//�����趨������ʼ��GPIOC.0
 GPIO_SetBits(GPIOC,GPIO_Pin_10);						 				//PA.0 �����	
	
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 	//SUPER_IR_OUT-->PC.8 �˿�����
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//�������
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO���ٶ�Ϊ50MHz
// GPIO_Init(GPIOC, &GPIO_InitStructure);					 		//�����趨������ʼ��GPIOC.2
// GPIO_SetBits(GPIOC,GPIO_Pin_2);						 				//PC.2 �����	
}

/**********************************************************************
* ��������: 433M ��ѯɨ�躯��
* �������: ��
* �� �� ֵ: ��
* ˵    ��: �����𵴵�����ֵ�빩���ѹ�����ж�,
* 					����ʱ��ֱ������ֵ����10��������
*						������Ϊ1.2ms������ʱ�������Ϊ120us��
***********************************************************************/
void DecodingEv1527(void)
{
    unsigned char RC5; 
    tb_time++;
    RC5=  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10);	
    if(!RC5)  // ��⵽�͵�ƽ �͵�ƽʱ���1,��¼���ε�ƽ״̬
    {
        ll_w++;
        old_rc5=0; 
    }          
    else                                      // ��⵽�ߵ�ƽ
    { 
        tb_time=0;
        hh_w++;
        if (!old_rc5)                          // ��⵽�ӵ͵��ߵ�����,�Ѽ�⵽һ������(��-��)��ƽ����
        { 
            if ((!tb_ok)&&((hh_w>=2)&&(hh_w<=3))&&((ll_w>=62)&&(ll_w<=93)))          //��ͬ����
            { 
                ma_x=0; tb_ok=1;hh_w=0; ll_w=0; tb_data=0;
            }
            else if ((tb_ok)&&((hh_w>=6)&&(hh_w<=9))&&((ll_w>=2)&&(ll_w<=3))) //�Ѿ����յ�ͬ����,��1
            {
                hh_w=0; ll_w=0;
                tb_data_i=1;
                tb_data|=(unsigned long)(tb_data_i)<<(23-ma_x);
                ma_x++;
            }    
            else if ((tb_ok)&&((hh_w>=2)&&(hh_w<=3))&&(((ll_w>=6)&&(ll_w<=9))))   //����ѽ��ܵ�ͬ���룬��0
            { 
                hh_w=0; ll_w=0;
                tb_data_i=0;
                tb_data|=(unsigned long)(tb_data_i)<<(23-ma_x);
                ma_x++;
            }
						/*
						else if ((tb_ok)&&((hh_w>=2)&&(hh_w<=3))&&(((ll_w>=2)&&(ll_w<=3))))   // �����β��
            { 
                hh_w=0; ll_w=0;
                tb_data_i=0;
                tb_data|=(unsigned long)(tb_data_i)<<(23-ma_x);
                ma_x++;
            }
						*/
            else //���յ������ϵĸ�-�͵�ƽ����
            {
                ma_x=0; tb_ok=0; hh_w=0; ll_w=0;
            }
        }
        if(ma_x == 24)
        {                           
            Rf_ok_flag=1; ma_x=0; tb_ok=0; hh_w=0; ll_w=0;tb_ok=0;
            RF_address=tb_data>>4;
            RF_data=tb_data&0x0F;
            tb_data=0;
        }
        old_rc5=1;      // ��¼���ε�ƽ״̬
    } 
    if(tb_time>MAX_TB_TIME)
    {
        ma_x=0; tb_ok=0; hh_w=0; ll_w=0;
    }
}
#if 0
//��ʱһ��10us
void delay_us(unsigned int time)
{
#if 0
    unsigned int i,b;
    for(i=0;i<time;i++)
    {
        for(b=0;b<18;b++)
        {
            asm("nop");
        }           
    } 
#endif
    TIM3_ARRPreloadConfig(ENABLE);  //ʹ�ܶ�ʱ��3�Զ����ع���    
    TIM3_Cmd(ENABLE);               //������ʱ��3��ʼ����
    while(time--)
    {
        while( TIM3_GetFlagStatus(TIM3_FLAG_Update) == RESET); //�ȴ������Ƿ�ﵽ10us
        TIM3_ClearFlag(TIM3_FLAG_Update);  //�������10us�������Ӧ�ı�־
    }
    TIM3_Cmd(DISABLE);                   //��ʱȫ���������رն�ʱ��3
}

/**
*
*���������͵�ƽ�ߡ�����   �ߣ���=3:1
*/
void SendEv1527_High(unsigned int time)
{
    GPIO_SetBits(Ev_SEND_GPIO_PORT,Ev_SEND_GPIO_PINS);
    delay_us(3*time);
    GPIO_ResetBits(Ev_SEND_GPIO_PORT,Ev_SEND_GPIO_PINS);
    delay_us(time);
}

/**
*
*���������͵�ƽ�͡�����  �ߣ���=1:3
*/
void SendEv1527_Low(unsigned int time)
{
    GPIO_SetBits(Ev_SEND_GPIO_PORT,Ev_SEND_GPIO_PINS);
    delay_us(time);
    GPIO_ResetBits(Ev_SEND_GPIO_PORT,Ev_SEND_GPIO_PINS);
    delay_us(3*time);
}
/**
*
*����ͬ���롣����  �ߣ���=1:3
*/
void SendEv1527_Head(unsigned int time)
{
    GPIO_SetBits(Ev_SEND_GPIO_PORT,Ev_SEND_GPIO_PINS);
    delay_us(time);
    GPIO_ResetBits(Ev_SEND_GPIO_PORT,Ev_SEND_GPIO_PINS);
    delay_us(31*time);
}

/**
*
*����ǰ���뷢��10���ߵ�����
*/
void SendEv1527_Lead(unsigned int time)
{
    unsigned char i=0;
    for(i=0;i<5;i++)
    {
        GPIO_SetBits(Ev_SEND_GPIO_PORT,Ev_SEND_GPIO_PINS);
        delay_us(time);
        GPIO_ResetBits(Ev_SEND_GPIO_PORT,Ev_SEND_GPIO_PINS);
        delay_us(time);    
    }
}

/**
*
*���ͽ�����
*/
void SendEv1527_End(unsigned int time)
{
    GPIO_SetBits(Ev_SEND_GPIO_PORT,Ev_SEND_GPIO_PINS);
    delay_us(time);
    GPIO_ResetBits(Ev_SEND_GPIO_PORT,Ev_SEND_GPIO_PINS);
    delay_us(time);
}

/***
*
*������ģ��ev1527�������ݣ��ȷ���λ���ٷ���λ
*/
unsigned char SendEv1527(unsigned long int address,unsigned char data,unsigned int time)
{
    unsigned char i=0;
//   SendEv1527_Lead(time);
#if 1
   SendEv1527_Head(time);
    //���͵�ַ��  
    for(i=20;i--;i>0)
    {
        if((address>>i)&0x01)
            SendEv1527_High(time);
        else
            SendEv1527_Low(time);
    }
    //��������λ
    LEDG_ON;
    for(i=4;i--;i>0)
    {
        if((data>>i)&0x01)
            SendEv1527_High(time);
        else
            SendEv1527_Low(time);
    }
    LEDG_OFF;
//        SendEv1527_End(time);
#endif
}

#endif