#include "sys.h"
#include "super_IR.h"
#include <stdbool.h>
/**************全局变量****************************************************/
unsigned char hh_w,ll_w;           //高,低电平宽度
unsigned char ma_x;                //接收到第几位编码了
unsigned char old_rc5;             //保存上一次查询到的电平状态
unsigned char tb_ok;               //接收到同步的马时置1
unsigned long int tb_data=0;       //接收到的数据
bool tb_data_i=0;                  //用于存放每位
unsigned int tb_time=0;            //用于计时，如果在一定时间内没有检测到高电平，清零
unsigned long int RF_address=0;    //全局变量，用于存储
unsigned char RF_data=0;           //全局变量，用于存放数据
unsigned char Rf_ok_flag=0;        //全局变量，接收到一个完整的遥控命令后置1,通知解码程序可以解码了

/**********************************************************************
* 函数功能: 433M IO的初始化函数
* 输入参数: 无
* 返 回 值: 无
* 说    明: 无
***********************************************************************/
void RF_GPIO_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PC端口时钟

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				 	//SUPER_IR_IN-->PC.0 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 	//输入模式
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO口速度为50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 		//根据设定参数初始化GPIOC.0
 GPIO_SetBits(GPIOC,GPIO_Pin_10);						 				//PA.0 输出高	
	
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 	//SUPER_IR_OUT-->PC.8 端口配置
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//推挽输出
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO口速度为50MHz
// GPIO_Init(GPIOC, &GPIO_InitStructure);					 		//根据设定参数初始化GPIOC.2
// GPIO_SetBits(GPIOC,GPIO_Pin_2);						 				//PC.2 输出高	
}

/**********************************************************************
* 函数功能: 433M 轮询扫描函数
* 输入参数: 无
* 返 回 值: 无
* 说    明: 根据震荡电阻阻值与供电电压进行判断,
* 					调用时间直接用震荡值除以10，比如震
*						荡表中为1.2ms，调用时间就设置为120us。
***********************************************************************/
void DecodingEv1527(void)
{
    unsigned char RC5; 
    tb_time++;
    RC5=  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10);	
    if(!RC5)  // 检测到低电平 低电平时间加1,记录本次电平状态
    {
        ll_w++;
        old_rc5=0; 
    }          
    else                                      // 检测到高电平
    { 
        tb_time=0;
        hh_w++;
        if (!old_rc5)                          // 检测到从低到高的跳变,已检测到一个完整(高-低)电平周期
        { 
            if ((!tb_ok)&&((hh_w>=2)&&(hh_w<=3))&&((ll_w>=62)&&(ll_w<=93)))          //判同步码
            { 
                ma_x=0; tb_ok=1;hh_w=0; ll_w=0; tb_data=0;
            }
            else if ((tb_ok)&&((hh_w>=6)&&(hh_w<=9))&&((ll_w>=2)&&(ll_w<=3))) //已经接收到同步码,判1
            {
                hh_w=0; ll_w=0;
                tb_data_i=1;
                tb_data|=(unsigned long)(tb_data_i)<<(23-ma_x);
                ma_x++;
            }    
            else if ((tb_ok)&&((hh_w>=2)&&(hh_w<=3))&&(((ll_w>=6)&&(ll_w<=9))))   //如果已接受到同步码，判0
            { 
                hh_w=0; ll_w=0;
                tb_data_i=0;
                tb_data|=(unsigned long)(tb_data_i)<<(23-ma_x);
                ma_x++;
            }
						/*
						else if ((tb_ok)&&((hh_w>=2)&&(hh_w<=3))&&(((ll_w>=2)&&(ll_w<=3))))   // 处理结尾码
            { 
                hh_w=0; ll_w=0;
                tb_data_i=0;
                tb_data|=(unsigned long)(tb_data_i)<<(23-ma_x);
                ma_x++;
            }
						*/
            else //接收到不符合的高-低电平序列
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
        old_rc5=1;      // 记录本次电平状态
    } 
    if(tb_time>MAX_TB_TIME)
    {
        ma_x=0; tb_ok=0; hh_w=0; ll_w=0;
    }
}
#if 0
//定时一个10us
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
    TIM3_ARRPreloadConfig(ENABLE);  //使能定时器3自动重载功能    
    TIM3_Cmd(ENABLE);               //启动定时器3开始计数
    while(time--)
    {
        while( TIM3_GetFlagStatus(TIM3_FLAG_Update) == RESET); //等待计数是否达到10us
        TIM3_ClearFlag(TIM3_FLAG_Update);  //计数完成10us，清除相应的标志
    }
    TIM3_Cmd(DISABLE);                   //延时全部结束，关闭定时器3
}

/**
*
*描述：发送电平高。比率   高：低=3:1
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
*描述：发送电平低。比率  高：低=1:3
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
*发送同步码。比率  高：低=1:3
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
*发送前导码发送10个高低脉冲
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
*发送结束码
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
*描述：模仿ev1527发送数据，先发高位，再发低位
*/
unsigned char SendEv1527(unsigned long int address,unsigned char data,unsigned int time)
{
    unsigned char i=0;
//   SendEv1527_Lead(time);
#if 1
   SendEv1527_Head(time);
    //发送地址码  
    for(i=20;i--;i>0)
    {
        if((address>>i)&0x01)
            SendEv1527_High(time);
        else
            SendEv1527_Low(time);
    }
    //发送数据位
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