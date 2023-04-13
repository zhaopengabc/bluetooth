#include "bc95.h"

/*******************************************************************************
*                       BC35模块驱动
********************************************************************************
*公司：富邦科技
*时间：2018-05-20
*******************************************************************************/
//硬件说明：
// GPRS模块电源
// GPRS_CRTL-----PB0    1 高电平使能，电源开启  0 低电平禁止，电源关闭
// 电源控制
// GPRS_PWR------PB4   开关机，高电平使能开机，低电平禁止
// 复位
// IO_1---GPRS_RESET----PD5   模块复位，高电平有效，低电平禁止
// 模块类型检测
// CS------------        NC，空，未使用；
//--------------------------------------------------------------------------------//
//修改记录
//1、将COAP_EN宏定义删除
//2、更改通信协议为GB26875.3
//3、

/***********************头文件***************************************************/
//#include "stm8l15x.h"
//#include "gprs.h"
//#include "usart2.h"
#include "usart.h"
#include <string.h>
#include "rtc.h"
//#include "common.h"
#include "stm32f10x_gpio.h"
#include "delay.h"
#include "os.h"

#include "includes.h"
#include "UsartMsgProcessAppExt.h"
/*********************变量定义*******************************************************/
const char ATOK[]="OK\r\n";
const char AT_ERROR[]="ERROR\r\n";
const char ATCCLK[]="+CCLK:";
const char ATCSQ[]="+CSQ:";
const char ATCGATT[]="+CGATT:";
const char ATCPSMS[]="+CPSMS";
const char ATCPSMS0[]="+CPSMS:0";
const char ATCGSN[]="+CGSN:";
const char ATCEREG[]="+CEREG:";
const char ATNBAND[]="+NBAND:";
const char ATNSMI[]="+NSMI";
const char ATNNMI[]="+NNMI";
const char ATCGPADDR[]="+CGPADDR:";
const char AT_CLOSED[]="CLOSE";
const char AT_CGATT1[]="+CGATT:1";
const char AT_CEREG1[]="+CEREG:1,1";
const char AT_CEREG5[]="+CEREG:1,5";

const char AT_CEREG01[]="+CEREG:0,1";
const char AT_CEREG05[]="+CEREG:0,5";

AT_link_ENUM NB_Link_step=NBLINK;
u8 NB_BAND=0;
usart2_buf uart_gprs;
unsigned int NBbuflenth=0;

volatile unsigned char gprs_shutdown_flag=1;		//关机标志，为0开机状态，为1关机状态，为2复位状态
volatile unsigned char SendSuccessedFlag=0;				//数据发送成功标志，为0发送失败，为1发送成功
unsigned char GprsStopSendFlag;

volatile unsigned char Signal_Value=99;   //存储信号强度
static unsigned char IMEI[15];          //存储IMEI
static unsigned char IMSI[15];          //存储IMSI
//extern usart2_buf uart_gprs;
unsigned char Data_Frame[512];
unsigned char temp_Buffer[RX_BUF_LEN];

u8 MNC=0;
//频点锁定标志；若信号正常且联网正常则锁定此频点；netlockflag置1；
//设备重启标志清零；
static unsigned char NetLockFlag = 0;
//设备上电重启，频点处理标志;设备上电重启此时标志位置位，判断标志位若置位则进行频点处理；
static unsigned char NetPowerFlag = 0;
//设备频点存储，初次上电读取频点，并存储；之后若需要处理则依次加一处理；
static unsigned char NetNum=0;//0:初次上电，进行读取1：B3，:2:B5,3:B8


usart1_buf uart_debug;
usart2_buf uart_gprs;
usart3_buf uart_rs485;


volatile unsigned char	NB_RegistFlag=1;//NB注册标志位  1注册失败，0注册成功；
volatile unsigned char	W5_RegistFlag=0;//NB注册标志位  1注册失败，0注册成功；
extern unsigned char MIM_flag;
/*********************函数声明******************************************************/
void USART4_sendbuf(u8 *str,u32 length)
{
    u32 xi=0;
    for(xi=0;xi<length;xi++)
    {
        USART_SendData(UART4,*(str+xi));
						  if(MIM_flag==1)
							{
					 		USART_SendData(USART1,*(str+xi));	//test
							}
		while(USART_GetFlagStatus(UART4, USART_FLAG_TXE)==RESET);
        #ifdef  IWDG_FUNCTION_EN
	    IWDG_ReloadCounter();
        #endif
    }
    while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET);
}

/*****************************************************************************
* 函数名称: void UartRxISR(stUart *st,char c)
* 功能描述: 接收字符指针队列处理
* 参    数:
* 返回  值:
*****************************************************************************/
static	void UartRxISR(stUart *st,char c)
{
    OS_ERR err;
    if(st->pRxWr<=st->RxBuf) st->pRxWr = st->RxBuf;
    else if(st->pRxWr - st->RxBuf >= sizeof(st->RxBuf)-1) st->pRxWr = st->RxBuf;
  	*(st->pRxWr) = c;
        st->pRxWr++;                                                           
	
		if(st->pRxWr - st->RxBuf >= sizeof(st->RxBuf)-1)
				st->pRxWr = st->RxBuf;
        st->last_receive=OSTimeGet(&err);
}

usart2_buf uart_gprs;
void UART4_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		/*Res =USART_ReceiveData(USART2);	//读取接收到的数据
		receive2[j2]=Res;
		j2++;*/
        Res =	USART_ReceiveData(UART4);
	      UartRxISR(&uart_gprs, Res);
			  if(MIM_flag==1)
 	 		  {
				USART_SendData(USART1, Res);
				}//test
    }
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
/*****************************************************************************
* 函数名称: void GPRS_IO_Init()
* 功能描述: GPRS控制IO初始化
* 参    数:
* 返回  值:
*****************************************************************************/
void GPRS_IO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //GPIO_Init(PORT_GPRS_RESET , PIN_GPRS_RESET , GPIO_Mode_Out_PP_Low_Slow);
    //GPIO_Init(PORT_GPRS_MP_EN , PIN_GPRS_MP_EN , GPIO_Mode_Out_PP_Low_Slow);
    //GPIO_Init(PORT_GPRS_PWR , PIN_GPRS_PWR , GPIO_Mode_Out_PP_Low_Slow);
    
    GPIO_InitStructure.GPIO_Pin = PIN_GPRS_RESET; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//复用推挽输出
    GPIO_Init(PORT_GPRS_RESET, &GPIO_InitStructure);//初始化GPIOA.9
    
//    GPIO_InitStructure.GPIO_Pin = PIN_GPRS_MP_EN; //PA.9
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//复用推挽输出
//    GPIO_Init(PORT_GPRS_MP_EN, &GPIO_InitStructure);//初始化GPIOA.9
//    
//    GPIO_InitStructure.GPIO_Pin = PIN_GPRS_PWR; //PA.9
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//复用推挽输出
//    GPIO_Init(PORT_GPRS_PWR, &GPIO_InitStructure);//初始化GPIOA.9
    
    
	//VGPRS_MP_EN_LOW;
	//GPRS_PWR_LOW;
	GPRS_RESET_LOW;
}
//Usart功耗控制
void NB_UsartMode(unsigned char state)
{
    if(state == 0)
    {
        //禁能NB_Usart
//        USART_DeInit(USART1);
     /*   USART_Cmd(USART1 , DISABLE);
        CLK_PeripheralClockConfig(USART1_CLK , DISABLE); 
        GPIO_Init(PORT_USART1_RX , PIN_USART1_RX , GPIO_Mode_Out_PP_Low_Fast);
        GPIO_Init(PORT_USART1_TX , PIN_USART1_TX , GPIO_Mode_Out_PP_Low_Fast);
        GPIO_ResetBits(PORT_USART1_RX , PIN_USART1_RX );
        GPIO_ResetBits(PORT_USART1_TX , PIN_USART1_TX );
    }
    else
    {
        //nb模块长供电，IO口拉高，低功耗模式
        USART_Cmd(USART1 , DISABLE);
        CLK_PeripheralClockConfig(USART1_CLK , ENABLE); 
        GPIO_Init(PORT_USART1_RX , PIN_USART1_RX , GPIO_Mode_Out_PP_Low_Fast);
        GPIO_Init(PORT_USART1_TX , PIN_USART1_TX , GPIO_Mode_Out_PP_Low_Fast);
        GPIO_SetBits(PORT_USART1_RX , PIN_USART1_RX );
        GPIO_SetBits(PORT_USART1_TX , PIN_USART1_TX );*/
    }
}
//串口初始化
void NB_UsartInit(unsigned long baudrate)
{
    //USART2_Init(9600);//Uart1_Init(baudrate);
}
void NB_UsartSendstr(unsigned char *data , unsigned int strlen)
{
   // USART2_SEND(data,strlen);
	 USART4_sendbuf(data,strlen);
}

void MP1482_POWERON(void)
{
// VGPRS_MP_EN_HIGH;
#if	NB_PRINTF
  USART1_sendstr("MP1482_On!!!\r\n");
#endif
}
void MP1482_POWEROFF(void)
{
//	VGPRS_MP_EN_LOW;
	delay_ms(100);
#if	NB_PRINTF
	USART1_sendstr("MP1482_Off!!!\r\n");
#endif
}
/*****************************************************************************
* 函数名称: void VGPRS_EN(unsigned char i)
* 功能描述: GPRS模块电源控制
* 参    数:
* 返回  值:
*****************************************************************************/
void VGPRS_EN(unsigned char i)
{
	if (i == 1)
	{
        MP1482_POWERON();
		delay_ms(1);
		//GPRS_PWR_HIGH;
		delay_ms(100);
		ResetUartBuf(&uart_gprs);
		GPRS_RESET_LOW;
		gprs_shutdown_flag=0;
		SendSuccessedFlag=0;
	}
	else if (i == 0)
	{
        MP1482_POWEROFF();
		delay_ms(100);
		//GPRS_PWR_LOW;
		delay_ms(100);
		if(NB_Link_step>=NB_CIMI)
		{
			NB_Link_step=NB_CIMI;
		}
		else
		{
			NB_Link_step = NBLINK;
		}
		gprs_shutdown_flag=1;
	}
}
/*****************************************************************************
* 函数名称: unsigned char OpenGPRS(void)
* 功能描述: GPRS启动检测函数
* 参    数: 1：准备  0：未准备好
* 返回  值:
*****************************************************************************/
u8 OpenGPRS_check(void)//unsigned char OpenGPRS(void)
{
	VGPRS_EN(1);						// GM3上电
	NB_UsartInit(9600);
    delay_ms(5000);
	return 1;   						// 启动成功
}
/*****************************************************************************
* 函数名称: void reset_gprs(void)
* 功能描述: 查询GM3配置参数SN IMEI Phone存入Flash中
* 参    数: 
* 返回  值:
*****************************************************************************/
void reset_gprs(void)
{
    static unsigned int reset_cnt=0;
    static unsigned int Net_cnt = 0;
	if((NetLockFlag == 0)&&(NB_Link_step >= NB_CSQ))
	{
		Net_cnt++;
		if(Net_cnt >= 3)
		{
			Net_cnt=0;
			NetPowerFlag = 1;
			reset_cnt = 6;
		}
	}
	else
	{
		Net_cnt=0;
	}
	reset_cnt++;
	if(reset_cnt>5)
	{
		reset_cnt=0;
		VGPRS_EN(0);
		delay_ms(200);
		VGPRS_EN(1);
		delay_ms(10);
#if	NB_PRINTF
		USART1_sendstr("BC95 Restart!!!");
#endif
	}
	else
	{
		delay_ms(10);
		GPRS_RESET_HIGH;
		delay_ms(200);
		GPRS_RESET_LOW;
		delay_ms(10);
        gprs_shutdown_flag=2;
#if	NB_PRINTF
		USART1_sendstr("BC95 Reset!!!");
#endif
	}
	if((NB_Link_step>=NB_CIMI)&&(reset_cnt!=0))
	{
		NB_Link_step=NB_CGSN;//NB_CIMI;//NB_NBAND;//NB_CIMI;
	}
	else
	{
		NB_Link_step = NBLINK;
	}
}


/**
*@brief	 	计算整形数据的位数
*@param		
*@return	n:位数
*/
unsigned char judgeLen(int  a)
{
    int n=0;  
    while (a)
    {
        a = a / 10;  
        n++;  
    }
    return n;  
}
/**
*@brief	 	16进制转换成大写字符串 例如：0xA1，转换成str[0] = ‘A’,str[0] = ‘1’,
*@param		n:16进制，str：字符串
*@return	
*/
void BC95_chartoasc(char aa,unsigned char *buffer)
{
    unsigned char i=0;
    buffer[0]=aa/16;
    buffer[1]=aa%16;
    for(i=0;i<2;i++)
    {
        if(buffer[i]<10)buffer[i]+='0';
        else buffer[i]=buffer[i]-10+'A';
    }
}
unsigned char BC95_hextochar(char *buffer,char *aa)
{
    unsigned char i=0;
    for(i=0;i<2;i++)
    {
        if((buffer[i]>='A')&&(buffer[i]<='F'))
        {
            buffer[i]=buffer[i]-'A'+10;
        }
        else if((buffer[i]>='0')&&(buffer[i]<='9'))
        {
            buffer[i]=buffer[i]-'0';
        }
        else
        {
            return 0;
        }
    }
    *aa=(buffer[0]<<4)+buffer[1];
    return 1;
}
unsigned char BC95_HexStrtoCharStr(char *buffer,unsigned int buflen,char *aa)
{
    unsigned int i=0;
    unsigned char flag=0;
    if((buflen%2==1)||(buflen==0))return 0;
    for(i=0;i<buflen/2;i++)
    {
        flag=BC95_hextochar((char *)&buffer[i*2],(char *)&aa[i]);
        if(flag==0)return 0;
    }
    return 1;
}
/**
*@brief	 	整型数转化为字符串函数
*@param		n:要转化整数， str[5]:存放转化后的字符串  len：整型数长度
*@return	无
*/
//void itoa(unsigned int n,unsigned char str[5], unsigned char len)
//{
//	unsigned char i=len-1;
//	memset(str,0x20,len);
//	do{
//		str[i--]=n%10+'0';
//	}while((n/=10)>0);
//	return;
//}

unsigned char int16toString(unsigned int cnt,unsigned char *buf)
{
	unsigned char tmp[5];
	unsigned char i=0;
	if(cnt>=10000)return 0;
	else //if(cnt>0x0fff)
	{
		tmp[0]=cnt/1000;
		tmp[1]=(cnt%1000)/100;
		tmp[2]=(cnt%100)/10;
		tmp[3]=cnt%10;
		for(i=0;i<4;i++)
		{
			if(tmp[i]<=9)
			{
				tmp[i]+='0';
			}
			else
			{
				return 0;
			}
		}
		memcpy(buf,tmp,4);
	}
	return i;
}
unsigned char Str_Is_Equal(unsigned char *dest,unsigned char *source,unsigned int length)
{
	unsigned int i=0;
	unsigned int flag=1;
	for(i=0;i<length;i++)
	{
		if(*dest++!=*source++)
		{
			flag=0;
			break;
		}
	}
	return flag;
}
//判断buffer字符串是否包含scr字符串
//输入参数scr为字符串，包含‘\0’
//若buffer字符串包含scr则返回scr字符串结束后下一个字符位置，否则返回0
unsigned int IsStrInclude(unsigned char *buffer,unsigned char *scr,unsigned int buflenth)
{
	unsigned int i=0;
	unsigned char flag=0;
	if(buflenth<strlen((char const*)scr))return 0;
	for(i=0;i<buflenth+1-strlen((char const*)scr);i++)
	{
		flag=Str_Is_Equal((unsigned char *)&buffer[i],(unsigned char *)scr,strlen((char const*)scr));
		if(flag==1)
		{
			i+=strlen((char const*)scr);
			return i;
		}
	}
	if(i==buflenth+1-strlen((char const*)scr))i=0;
	return i;
}
/*****************************************************************************
* 函数名称: unsigned char sum_check(unsigned char* data, unsigned char len)
* 功能描述: 累加和校验函数
* 参    数: 
* 返回  值:
*****************************************************************************/
unsigned char sum_check(unsigned char *data, unsigned char len)
{
	unsigned char sum = 0;
	unsigned char i = 0;
	for (i = 0; i < len; i++)
	{
		sum += data[i];
	}
	return sum;
}

//const char AT_AT[]="AT\r\n";
//const char AT_CLOSE_FUN[]="AT+CFUN=0\r\n";
//const char AT_AUTOCONNECT[]="AT+NCONFIG=AUTOCONNECT,TRUE\r\n";
//const char AT_QNBAND[]="AT+NBAND?\r\n";


/*****************************************************************************
* 函数名称: void Send_AT_Command(void)
* 功能描述: 发送命令
* 参    数:
* 返回  值:
*****************************************************************************/
void Send_AT_Command(unsigned char step)
{
	unsigned char Data_Frame[100];
	unsigned char Count=0;
	memset(Data_Frame,0,100);
	switch(step)
	{
		case  NBLINK:
			memcpy(Data_Frame,"AT\r\n",4);
			Count = 4;
		break;
		case  NB_OPEN_FUN:
			memcpy(Data_Frame,"AT+CFUN=0\r\n",11);//关闭射频功能
			Count = 11;
		break;
		case  NB_AUTOCONNECT:
			memcpy(Data_Frame,"AT+NCONFIG=AUTOCONNECT,TRUE\r\n",29);//配置连接模式为自动连接
			Count = 29;								
		break;
		case  NB_NBAND:
            if((MNC==0)||(MNC==2)||(MNC==4)||(MNC==7))//中国移动
            {
                
            }
            else if((MNC==1)||(MNC==6)||(MNC==9))//中国联通
            {
                if(NetPowerFlag == 1)
                {
                    NetPowerFlag = 0;
                    if(NetLockFlag == 0)
                    {
                        NetNum = 1-NetNum;
                    }
                }
                if(NetNum==0) memcpy(Data_Frame,"AT+NBAND=3\r\n",11);//查询频段
                else memcpy(Data_Frame,"AT+NBAND=8\r\n",11);//查询频段
            }
            else//3.5.11中国电信
            {
                memcpy(Data_Frame,"AT+NBAND=5\r\n",11);//查询频段
            }
			Count = 11;
		break;
        case  NB_QNBAND:
			memcpy(Data_Frame,"AT+NBAND?\r\n",11);//查询频段
			Count = 11;
		break;
		case  NB_NSMI1:
			memcpy(Data_Frame,"AT+NSMI=0\r\n",11);//发送信息标志，发送信息后会有回馈
			Count = 11;
		break;
		case  NB_NNMI1:
			memcpy(Data_Frame,"AT+NNMI=1\r\n",11);//接收信息标志开，接到新消息会有回馈
			Count = 11;
		break;
        case NB_CGMR:
            memcpy(Data_Frame,"AT+CGMR\r\n",9);//关闭射频功能
			Count = 9;
		break;
        case NB_NCSEARFCN:
            memcpy(Data_Frame,"AT+NCSEARFCN\r\n",14);//关闭射频功能
			Count = 14;
        break;
		case  NB_NCDP:
            if((MNC==0)||(MNC==2)||(MNC==4)||(MNC==7))//中国移动
            {
                
            }
            else if((MNC==1)||(MNC==6)||(MNC==9))//中国联通
            {
                Count = sizeof(SEVER_ADDRESS_CMCC)-1;//设置通讯ID和端口
			    memcpy(Data_Frame,SEVER_ADDRESS_CMCC,Count); 
            }
            else//3.5.11中国电信
            {
                Count = sizeof(SEVER_ADDRESS_CTCC)-1;//设置通讯ID和端口
			    memcpy(Data_Frame,SEVER_ADDRESS_CTCC,Count);
            }
		break;
		
		case  NB_NRB:
			memcpy(Data_Frame,"AT+NRB\r\n",8);//软重启
			Count = 8;
		break;
		case  NB_CIMI:
			memcpy(Data_Frame,"AT+CIMI\r\n",9);//查询SIM卡信息
			Count = 9;
		break;
		case  NB_CGSN:
			memcpy(Data_Frame,"AT+CGSN=1\r\n",11);//查询IMEI码
			Count = 11;
		break;
        case NB_CPSMS_0:
            memcpy(Data_Frame,"AT+CPSMS=0\r\n",12);//查询信号强度
			Count = 12;
        break;
        case NB_CPSMS:
            memcpy(Data_Frame,"AT+CPSMS?\r\n",11);//查询信号强度
			Count = 11;
        break;
		case  NB_CSQ:
			memcpy(Data_Frame,"AT+CSQ\r\n",8);//查询信号强度
			Count = 8;
		break;
		case  NB_CGATT_1:
			memcpy(Data_Frame,"AT+CGATT=1\r\n",12);//自动搜网
			Count = 12;
		break;
		case  NB_CEREG_1:
			memcpy(Data_Frame,"AT+CEREG=1\r\n",12);//？？？？？
			Count = 12;
		break;
		case  NB_CGATT:
			memcpy(Data_Frame,"AT+CGATT?\r\n",11);//自动搜网
			Count = 11;
		break;
		case  NB_CEREG:
			memcpy(Data_Frame,"AT+CEREG?\r\n",11);
			Count = 11;
		break;
		case  NB_CGPADDR:
			memcpy(Data_Frame,"AT+CGPADDR\r\n",12);//查询核心网分配的ip地址
			Count = 12;
            NetLockFlag = 1;
		break;
		default:
		break;				
	}
#if	NB_PRINTF
	USART1_sendstr("NB_TX:");
	USART1_sendbuf(Data_Frame,Count);
	USART1_sendstr("\r\n");
#endif
	NB_UsartSendstr(Data_Frame,Count);
}
//读取接收信息，返回1；
unsigned char ATGetMsg(void)
{
	unsigned char Flag=1;
	unsigned char Data_Frame[20];
	
	memset(Data_Frame,0,20);
	memcpy(Data_Frame,"AT+NMGR\r\n",9);
#if	NB_PRINTF
	USART1_sendstr("NB_TX:");
	USART1_sendbuf(Data_Frame,9);
	USART1_sendstr("\r\n");
#endif
	NB_UsartSendstr(Data_Frame,9);
	return Flag;
}
//读取信号强度，阻塞时间最多2.5s，最短时间0.5s
//异常返回99，正常返回信号强度0-31；
unsigned char Signal_Check(void)
{
	unsigned int lenth=0;
	unsigned char i=0;

	unsigned char j=0;
	unsigned int buflenth=0;
    //再次唤醒后需要读取2次才能读到数据，具体原因应进一步分析，
    //时间间隔应调整，（数据发送原因or数据不全？）
    //ASL........20180612
	Send_AT_Command(NB_CSQ);
	delay_ms(500);
	Send_AT_Command(NB_CSQ);
	delay_ms(500);
	// 读取GSM信号强度
	lenth = UartGetRxLen(&uart_gprs);
	if(lenth>0)
	{
		for(j=0;((buflenth!=lenth)&&(j<200));j++)			//防止数据读取中被打断
		{
			buflenth=lenth;
			delay_ms(10);
			lenth = UartGetRxLen(&uart_gprs);
		}
		lenth=UartRead(&uart_gprs, temp_Buffer, RX_BUF_LEN);
		if((lenth>=sizeof(ATCSQ))&&(strstr((const char *)temp_Buffer,ATOK ))&&(strstr((const char *)temp_Buffer,ATCSQ )))
		{
			//读取信号强度，并记录判定是否信号足够。足够则执行下一步，否则定时查询；
			//若信号强度<40，则判定为可以正常工作
			i=IsStrInclude(temp_Buffer,(unsigned char *)ATCSQ,lenth);
			if((temp_Buffer[i+1]==',')&&(temp_Buffer[i] > '0')&&(temp_Buffer[i] <= '9'))
			{
				if(NB_Link_step<NB_CGATT_1)NB_Link_step=NB_CGATT_1;
				return  temp_Buffer[i]-0x30;
			}
			else if(((temp_Buffer[i] >= '0')&&(temp_Buffer[i]<='3')&&(temp_Buffer[i+1] >= '0')&&(temp_Buffer[i+1] <= '9')&&(temp_Buffer[i+2]==',')))
			{
				if(NB_Link_step<NB_CGATT_1)NB_Link_step=NB_CGATT_1;
				return  (temp_Buffer[i]-0x30)*10+temp_Buffer[i+1]-0x30;
			}
		}
	}
	return 99;
}
unsigned char RecieveATCMDhandle(void)
{
	volatile unsigned char ErrorFlag=0;
	unsigned int lenth=0;
	unsigned int buflenth=0;
//	unsigned char temp_Buffer[RX_BUF_LEN];
	unsigned int j=0;
	unsigned int i=0;
	
	lenth = UartGetRxLen(&uart_gprs);
	if(lenth>0)
	{
		for(j=0;((buflenth!=lenth)&&(j<200));j++)			//防止数据读取中被打断
		{
			buflenth=lenth;
			delay_ms(10);
			lenth = UartGetRxLen(&uart_gprs);
		}
		memset(temp_Buffer,0,RX_BUF_LEN);
		lenth=UartRead(&uart_gprs, temp_Buffer, RX_BUF_LEN);//接收数据成功
        
		switch(NB_Link_step)
		{
			case  NB_NRB:
				if((lenth>=sizeof(ATOK))&&(strstr((char const*)temp_Buffer, ATOK)))
				{
					NB_Link_step++;
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
					
				}
			break;
			
			case  NBLINK:
			case  NB_OPEN_FUN:
			case  NB_AUTOCONNECT:
			case  NB_NSMI1:
			case  NB_NNMI1:
            case  NB_NCSEARFCN:
            case  NB_CPSMS_0:
			case  NB_NCDP:
			case  NB_CGATT_1:
			case  NB_CEREG_1:
            case  NB_NBAND:
				if((lenth<10)&&(lenth>=sizeof(ATOK))&&(strstr((char const*)temp_Buffer, ATOK)))
				{
					NB_Link_step++;
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
				}
			break;
            case  NB_CGMR:
				if((lenth<200)&&(lenth>=sizeof(ATOK))&&(strstr((char const*)temp_Buffer, ATOK)))
				{
					NB_Link_step++;
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
				}
			break;
			case  NB_QNBAND:
				if((lenth>=sizeof(ATNBAND))&&(strstr((const char *)temp_Buffer,ATNBAND ))&&(strstr((char const*)temp_Buffer, ATOK)))
				{
					//模块工作频段是否正确
					i=IsStrInclude(temp_Buffer,(unsigned char *)ATNBAND,lenth);
					if((temp_Buffer[i] == '5')||(temp_Buffer[i] == '8'))
					{
                        NB_BAND=temp_Buffer[i];
						NB_Link_step++;
					}
					else
					{
						return 1;
					}
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
				}
			break;
            case  NB_CPSMS:
				if((lenth>=sizeof(ATCPSMS))&&(strstr((const char *)temp_Buffer,ATCPSMS ))&&(strstr((char const*)temp_Buffer, ATOK)))
				{
					if(strstr((const char *)temp_Buffer,ATCPSMS0))
					{
						NB_Link_step++;
					}
					else
					{
						return 1;
					}
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
				}
			break;
			case  NB_CIMI:
				if((lenth==0x19)&&(strstr((char const*)temp_Buffer, ATOK)))
				{
					//ASL.................缺少格式判定；
						for(j=0;j<15;j++)
						{
							if((temp_Buffer[2+j]>='0')&&(temp_Buffer[2+j]<='9'))
							{
								
							}
							else
							{
								break;
							}
						}
						//数据格式正确
						if(j==15)
						{
							//存储的IMEI与读取的IMEI不相等；
							if(Str_Is_Equal((unsigned char*)&temp_Buffer[2],(unsigned char*)IMSI,15)==0)
							{
//								FLASH_Unlock(EEPROM);
//								FLASH_WriteNBytes(IMEI_ADDRESS, 15, (unsigned char*)&temp_Buffer[i]);
//								FLASH_Lock(EEPROM);
//								for(j=0;j<15;j++)
//								{
//									IMEI[j]=*(unsigned char*)((unsigned char*)IMEI_ADDRESS+j);
//								}
                                memcpy(IMSI,(unsigned char*)&temp_Buffer[2],15);
							}else
							{
								
							}
                            MNC=(IMSI[3]-'0')*10+IMSI[4]-'0';
							NB_Link_step++;
						}
					//NB_Link_step++;
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
				}
			break;
			case  NB_CGSN:
				if((lenth>=sizeof(ATCGSN))&&(strstr((const char *)temp_Buffer,ATCGSN))&&(strstr((const char *)temp_Buffer,ATOK )))
				{
					i=IsStrInclude(temp_Buffer,(unsigned char *)ATCGSN,lenth);
					//memcpy()将对应的SIM卡号记录Rxdata[2]--Rxdata[16]共15个位
					if(i>sizeof(ATCGSN)&&(i+15<=lenth))
					{
						for(j=0;j<15;j++)
						{
							if((temp_Buffer[i+j]>='0')&&(temp_Buffer[i+j]<='9'))
							{
								
							}
							else
							{
								break;
							}
						}
						//数据格式正确
						if(j==15)
						{
							//存储的IMEI与读取的IMEI不相等；
							if(Str_Is_Equal((unsigned char*)&temp_Buffer[i],(unsigned char*)IMEI,15)==0)
							{
//								FLASH_Unlock(EEPROM);
//								FLASH_WriteNBytes(IMEI_ADDRESS, 15, (unsigned char*)&temp_Buffer[i]);
//								FLASH_Lock(EEPROM);
//								for(j=0;j<15;j++)
//								{
//									IMEI[j]=*(unsigned char*)((unsigned char*)IMEI_ADDRESS+j);
//								}
                                memcpy(IMEI,(unsigned char*)&temp_Buffer[i],15);
							}else
							{
								
							}
							NB_Link_step++;
						}
					}
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
				}
			break;
			case  NB_CSQ:
				if((lenth>=sizeof(ATCSQ))&&(strstr((const char *)temp_Buffer,ATCSQ ))&&(strstr((const char *)temp_Buffer,ATOK )))
				{
					//读取信号强度，并记录判定是否信号足够。足够则执行下一步，否则定时查询；
					//若信号强度<40，则判定为可以正常工作
					i=IsStrInclude(temp_Buffer,(unsigned char *)ATCSQ,lenth);
					if((temp_Buffer[i+1]==',')&&(temp_Buffer[i] > '0')&&(temp_Buffer[i] <= '9'))
					{
                        Signal_Value = temp_Buffer[i]-'0';
						NB_Link_step++;
					}
					else if(((temp_Buffer[i] >= '0')&&(temp_Buffer[i]<='3')&&(temp_Buffer[i+1] >= '0')&&(temp_Buffer[i+1] <= '9')&&(temp_Buffer[i+2]==',')))
					{
                        Signal_Value = (temp_Buffer[i]-'0') * 10 +( temp_Buffer[i+1]-'0');
						NB_Link_step++;
					}
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
				}
			break;
			case  NB_CGATT:
				if((lenth>=sizeof(ATCGATT))&&(strstr((const char *)temp_Buffer,ATCGATT))&&(strstr((char const*)temp_Buffer,ATOK)))
				{
					i=IsStrInclude(temp_Buffer,(unsigned char *)ATCGATT,lenth);
					if(temp_Buffer[i]=='1')
					{
						//读取附着状态，若为1，则附着成功，
						NB_Link_step++;
					}
					else if(temp_Buffer[i]=='0')
					{
                        //附着失败，等待附着成功
					}
				}else if((lenth>=sizeof(ATCEREG))&&(strstr((char const*)temp_Buffer,ATCEREG)))
				{
					i=IsStrInclude(temp_Buffer,(unsigned char *)ATCEREG,lenth);
					if(temp_Buffer[i]=='1')
					{             
						NB_Link_step=NB_CGPADDR;
					}
					else
					{
					}
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
				}
			break;
			case  NB_CEREG:
				if((lenth>=sizeof(ATCEREG))&&(strstr((char const*)temp_Buffer,ATCEREG))&&(strstr((char const*)temp_Buffer,ATOK)))
				{
					i=IsStrInclude(temp_Buffer,(unsigned char *)ATCEREG,lenth);
					if(temp_Buffer[i+2]=='1')
					{
						NB_Link_step++;
					}
					else if(temp_Buffer[i+2]=='0')
					{
					}
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
				}
			break;
			case  NB_CGPADDR:
				if((lenth>=sizeof(ATCGPADDR))&&(strstr((char const*)temp_Buffer,ATOK))&&(strstr((char const*)temp_Buffer,ATCGPADDR)))
				{
					//ASL.................缺少格式判定；
					//注册网络成功，
					NB_Link_step=NB_CONNECT;
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
				}
			break;
			case  NB_CONNECT:
				
			break;
			default :
			break;
		}
	}

	return 0;
}
unsigned char RetSendControl(void)
{
	static AT_link_ENUM NB_Status=NBLINK;		//上一次NB模块执行函数时的状态
	static unsigned int NB_Cnt=0;			//在同一状态下的时间计数（100ms的计数）
	static unsigned int NB_ReSendTimes=0;
	static unsigned char ReSendFlag=0;		//重发标志，在收到错误数据，或者超时时置位，使能上报；
    //重发次数控制；
	NB_Cnt++;
	if(NB_Status!=NB_Link_step) //状态更新，清零标志；
	{
		NB_Status=NB_Link_step;
		NB_Cnt=0;
		NB_ReSendTimes=0;
		ReSendFlag=1;
	}
	else
	{
		switch(NB_Link_step)
		{
			case  NB_OPEN_FUN:
			case  NB_AUTOCONNECT:
            case  NB_QNBAND:
			case  NB_NBAND:
			case  NB_NSMI1:
			case  NB_NNMI1:
            case  NB_CGMR:
            case  NB_NCSEARFCN:
            case  NB_CPSMS_0:
            case NB_CPSMS:
			case  NB_NCDP:
			case  NB_CGATT_1:
			case  NB_CEREG_1:
				if(NB_Cnt>=20)
				{
					NB_Cnt=0;
					NB_ReSendTimes++;
					if(NB_ReSendTimes>=3)
					{
						NB_ReSendTimes=0;
						return 1;
					}
					else
					{
						ReSendFlag=1;
					}
				}
			break;
			case  NBLINK:
				if(NB_Cnt>=60)
				{
					NB_Cnt=0;
					NB_ReSendTimes++;
					if(NB_ReSendTimes>=5)
					{
						NB_ReSendTimes=0;
						return 1;
					}
					else
					{
						ReSendFlag=1;
					}
				}
			break;
			case  NB_NRB:
			case  NB_CIMI:
			case  NB_CGSN:

			case  NB_CSQ:
			case  NB_CGATT:
            //case  NB_CPSMS:
			case  NB_CEREG:
				if(NB_Cnt>=40)
				{
					NB_Cnt=0;
					NB_ReSendTimes++;
					if(NB_ReSendTimes>=8)
					{
						NB_ReSendTimes=0;
						return 1;
					}
					else
					{
						ReSendFlag=1;
					}
				}
			break;
			default:
			break;				
		}
	}
    //重发控制命令
	if(ReSendFlag!=0)
	{
		ResetUartBuf(&uart_gprs);
		if(NB_Link_step<NB_CONNECT)
		{
			ReSendFlag=0;
			Send_AT_Command(NB_Link_step);
		}
	}
    return 0;
}
//模块注网函数；
//重发命令及时间控制，接收回复数据，；
//非阻塞，每100ms执行1次；
//读取IMEI过程中，存储于IMEI数组中；
//读取CSQ过程中，将信号强度存储于Signal_Value中；
//返回值：为1模块类型不正确不是电信的；或者异常重发次数过多；
//直接退出防止持续等待；
unsigned char RegisterToServer(void)
{
    unsigned char flag=0;
    flag=RecieveATCMDhandle();
    if(flag == 1) return 1;
    flag=RetSendControl();
    return flag;
}
/*****************************************************************************
* 函数名称: unsigned char gprs_send_start(void)
* 功能描述: GM3模块发送准备函数
* 参    数: 
* 返回  值:1:注册网络失败	0：注册网络成功
//阻塞，执行时间最多60s

*****************************************************************************/
unsigned char gprs_send_start(void)
{
	unsigned int t = 0;
	unsigned char flag=0;
	OS_ERR err;
	//如果在60s内没有注册上网络,模块重启
	for (t = 0; t < 600; t++)								// 60S
	{
		flag=RegisterToServer();
		if(flag==1)
		{
			//若出现模块异常或卡异常的情况，应尽快退出等待，
			//进行重启，防止持续等待消耗功耗
			t = 600;
			break;
		}
		else
		{
			
		}
		if (NB_Link_step==NB_CONNECT)						// 挂载网络成功
		{
			Signal_Value=Signal_Check();					// 检测信号强度 
#if	NB_PRINTF
			USART1_sendstr("Register OK !\r\n");
			USART1_sendstr("BC95+CSQ:");
			USART1_sendchar((Signal_Value%100)/10+'0');
			USART1_sendchar(Signal_Value%10+'0');
#endif
			break;
		}
		else
		{
//			delay_ms(100);									// 延时100ms
      OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err); //延时100ms
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //延时10ms
	}
	if (t >= 600)											// 挂载网络超时
	{
		reset_gprs();									// 复位模块
//		delay_ms(5000);									// 延时5000ms
		OSTimeDlyHMSM(0,0,5,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时5s
		return 1;
	}
	return 0;
}
//u32 Wait_Signal(u32 max_time)
//{
//    OS_ERR err;
//    u32 starttamp,timetamp,i;
//    ResetUartBuf(&uart_gprs);
////    starttamp=OSTimeGet(&err);
////   while(1)
//		printf("t=%d！\r\n",max_time);
//	  for(i=0;i<max_time;i++)
//    {
////         timetamp=OSTimeGet(&err);
////        if(starttamp<uart_gprs.last_receive)
////        { 
////            if(timetamp-uart_gprs.last_receive>10) break;
////        }
////        if(timetamp-starttamp>max_time) break;
//				if(timetamp-uart_gprs.last_receive>10) break;
//        delay_ms(10);
//    }
//		printf("i=%d！\r\n",i);
//    return uart_gprs.last_receive-starttamp;
//}

u32 Wait_Signal(u32 max_time)
{
    OS_ERR err;
    u32 starttamp,timetamp;
    ResetUartBuf(&uart_gprs);
//	  uart_gprs.last_receive=0;
    starttamp=OSTimeGet(&err);
    while(1)
    {
         timetamp=OSTimeGet(&err);
        if(starttamp<uart_gprs.last_receive)
        { 
            if(timetamp-uart_gprs.last_receive>10) break;
        }
        if(timetamp-starttamp>max_time) break;
//        delay_ms(10);
			  OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //延时10ms
    }
    return uart_gprs.last_receive-starttamp;
}

/*****************************************************************************
* 函数名称: void	BC95_Tx_Frame(u8 *Data,u16 Len)
* 功能描述: 发送数据
* 参    数:	Data：数据， DataLen：数据长度
* 返回  值:
ASL......添加限制条件,BC95一包数据最多512个；
//将Data以字节流格式上发
*****************************************************************************/
u32 GPRS_senddata(u8 *Data,u32 DataLen)//unsigned int BC95_Tx_Frame(unsigned char *Data,unsigned int DataLen)
{
	unsigned int i;
    u32 xi;
	unsigned char ReadData[20];
	unsigned int Count = 0;
	unsigned char Len = 0;
	if((NB_Link_step!=NB_CONNECT)||(DataLen>512))
	{
		//计时并退出，时间到重启设备
		return 0;
	}
    for(xi=0;xi<1;xi++)
    {
    ResetUartBuf(&uart_gprs);
	memset(ReadData, 0, sizeof(ReadData));
	memset(Data_Frame, 0, sizeof(Data_Frame));
	i = 0;
	memcpy(Data_Frame,"AT+NMGS=",8);
	Count = 8;
	Len =	judgeLen(DataLen+2);					//计算位数
	itoa((DataLen+2), ReadData, Len);				//转换成字符串
	for(i = 0; i< Len;i++)
	{
		Data_Frame[Count++] =  ReadData[i];
	}
	Data_Frame[Count++] = ',';
    
	BC95_chartoasc(DataLen/256, ReadData);
	Data_Frame[Count++] = ReadData[0];
	Data_Frame[Count++] = ReadData[1];
	
	BC95_chartoasc(DataLen%256, ReadData);
	Data_Frame[Count++] = ReadData[0];
	Data_Frame[Count++] = ReadData[1];
#if	NB_PRINTF
	USART1_sendstr("NB_TX:");
	USART1_sendbuf(Data_Frame,Count);
#endif
	NB_UsartSendstr(Data_Frame,Count);
        
	for( i = 0;i < DataLen; i++)
	{
		BC95_chartoasc(Data[i], Data_Frame);
		NB_UsartSendstr(Data_Frame,2);
#if	NB_PRINTF
		USART1_sendbuf(Data_Frame,2);
#endif
	}
	Data_Frame[0] = '\r';
	Data_Frame[1] = '\n';
	NB_UsartSendstr(Data_Frame,2);
#if	NB_PRINTF
    USART1_sendbuf(Data_Frame,2);
#endif
    Wait_Signal(10000);
    memset(&temp_Buffer, 0, RX_BUF_LEN);
    Len = UartRead(&uart_gprs, temp_Buffer, RX_BUF_LEN);
    if((Len>=sizeof(ATOK))&&(strstr((char const*)temp_Buffer, ATOK))) return 0;
    delay_ms(1000);
    }
	return 1;//return Count;
}
//u32 GPRS_buffer_send(u8 *str,u32 DataLen)
//{
//    u32 err,length;
//    err=GPRS_senddata(str,DataLen);
//    if(err) return err;
//    err=gprs_receive_check();
//    if(!err) 
//    {
//        length=Get_GPRS_Receive(USART2_RX_BUF_middle+1,sizeof(USART2_RX_BUF_middle)-1);
//        USART2_RX_BUF_middle[0]=(u8)length;
//        USART2_RX_FLAG=1;
//        ResetUartBuf(&uart_gprs);
//    }
//    return err;
//}
u32 GPRS_buffer_send(u8 *str,u32 DataLen)
{
    u32 err;
    err=GPRS_senddata(str,DataLen);
    return err;
}
unsigned int NB_Rx_Frame(unsigned char *Rxdata,unsigned int lenth,unsigned char *NBRxdata)
{
	unsigned int j,i=0;
	unsigned int NBRxlen=0;
	
	i=IsStrInclude(Rxdata,(unsigned char *)"\r\n",lenth);
	
	for(j=i;j<lenth;j++)
	{
		if((Rxdata[j]>='0')&&(Rxdata[j]<='9'))
		{
			NBRxlen=NBRxlen*10;
			NBRxlen+=Rxdata[j]-'0';
		}
		else break;
	}
	NBRxlen*=2;
	memcpy(NBRxdata,(unsigned char *)&Rxdata[j+1],NBRxlen);
	return NBRxlen;
}

u32 gprs_receive_check(void)
{
    u32 lenth;
    //ResetUartBuf(&uart_gprs);
    //ATGetMsg();
    Wait_Signal(25000);
    memset(&temp_Buffer, 0, RX_BUF_LEN);
    lenth = UartRead(&uart_gprs, temp_Buffer, RX_BUF_LEN);
    if((lenth>=sizeof(ATNNMI)+2)&&(strstr((char const*)temp_Buffer, ATNNMI))) return 0;
    else return 1;
}
u32 gprs_check_connect(void)
{
    volatile u32 length;
	  u32  Back=1;
	
    u8 command[11]="AT+CEREG?\r\n";
    //u8 command[11]="AT+CGATT?\r\n";
    USART4_sendbuf(command,11);// USART2_SEND(command,11);//USART2_SEND(command,11);
    Wait_Signal(10000); //ZHAONING
//		delay_ms(1000);									// 延时5000ms
    memset(&temp_Buffer, 0, RX_BUF_LEN);
    length = UartRead(&uart_gprs, temp_Buffer, RX_BUF_LEN);
    if(strstr((char const*)temp_Buffer, AT_CEREG1)) {Back=0;}
    if(strstr((char const*)temp_Buffer, AT_CEREG5)) {Back=0;}
	  //20190121
	  if(strstr((char const*)temp_Buffer, AT_CEREG01)) {Back=0;}
    if(strstr((char const*)temp_Buffer, AT_CEREG05)) {Back=0;}
    
		ResetUartBuf(&uart_gprs);
		
		return Back;
}
u8 data_transform(u8 data)
{
    if((data>='0')&&(data<='9')) return data-'0';
    if((data>='a')&&(data<='z')) return data-'a'+10;
    if((data>='A')&&(data<='Z')) return data-'A'+10;
    else return 0xff;
}
u32 Get_GPRS_Receive(u8 *str,u32 max_length)
//u32 Get_GPRS_Rec(u8 *str,u32 max_length)
{
    u32 i,j,length,add;
    length=uart_gprs.pRxWr-uart_gprs.RxBuf;
    for(i=0;i<length;i++)
    {
        if(uart_gprs.RxBuf[i]==',') break;
    }
    if(uart_gprs.RxBuf[i]!=',') return 0;
    length=0;
    for(j=1,add=1;j<=3;j++)
    {
        if(data_transform(uart_gprs.RxBuf[i-j])==0xff) break;
        length+=data_transform(uart_gprs.RxBuf[i-j])*add;
        add*=10;
    }
    length-=2;
    if(length>max_length) length=max_length;
    for(j=0;j<length*2;j++)
    {
        if(data_transform(uart_gprs.RxBuf[i+5+j])==0xff) break;
        if(j%2) *(str+j/2)+=data_transform(uart_gprs.RxBuf[i+5+j]);
        else *(str+j/2)=data_transform(uart_gprs.RxBuf[i+5+j])*16;
    }
		
		ResetUartBuf(&uart_gprs);
		
    //delay_ms(50);
    if(j==length*2) return length;
    else return 0;
    
}

u32 Get_GPRS_Receive_Length()
{
	  u32 length=0;
    length=uart_gprs.pRxWr-uart_gprs.RxBuf;
	  return length;
}
//-----------------------------结束-------------------------------------------------------------//
/*****************************************************************************
* 函数名称: void ResetUartBuf(stUart *st)
* 功能描述: 复位串口缓冲区
* 参    数:
* 返回  值:
*****************************************************************************/
void ResetUartBuf(stUart *st)
{
	memset((void *)(st->RxBuf), 0, RX_BUF_LEN);
	memset((void *)(st->TxBuf), 0, TX_BUF_LEN);
	st->pRxWr = st->RxBuf;
	st->pRxRd = st->RxBuf;
	st->pTxWr = st->TxBuf;
	st->pTxRd = st->TxBuf;
}


unsigned int UartGetRxLen(stUart *st)
{
	unsigned int rxcnt;
	if(st->pRxWr >= st->pRxRd)
		rxcnt = st->pRxWr - st->pRxRd;
	else
		rxcnt = st->pRxWr + sizeof(st->RxBuf) - st->pRxRd;
	return rxcnt;
}


unsigned int UartRead(stUart *st, unsigned char *buf, unsigned int len)
{
	unsigned int rxcnt,rdlen,ret;
	unsigned char *p = buf;
	if(len == 0)
		return 0;
	rxcnt = UartGetRxLen(st);
	rdlen = (rxcnt < len) ? rxcnt : len;
	ret = rdlen;
	while(rdlen)
	{
		rdlen--;
		*p = *(st->pRxRd);
		p++;
		st->pRxRd++;
		if(st->pRxRd - st->RxBuf >= sizeof(st->RxBuf))
		st->pRxRd = st->RxBuf;
	}
	return ret;
}

/*****************************************************************************
* 函数名称: void    gprs_link(void)
* 功能描述: GPRS连接判断
* 参    数:
* 返回  值:
*****************************************************************************/
void    gprs_link(void)
{
  u8 open_flag;
        /*unsigned char t  = 0;
        unsigned char i  = 0;
        do
        {
                t = gprs_send_start();
                i++;
                if(i > 4)
                {
                        i = 0;
                        GPRS_Reset();
                        delay_nms(5000);
//                        t = 0;
                }
        }
        while(t != 0 );*/
  do
  {
    OpenGPRS_check();
    open_flag = gprs_send_start();
  }while(open_flag);
    
}
/*
void UartRxISR(stUart *st,char c)
{
    OS_ERR err;
    if(st->pRxWr<=st->RxBuf) st->pRxWr = st->RxBuf;
    else if(st->pRxWr - st->RxBuf >= sizeof(st->RxBuf)-1) st->pRxWr = st->RxBuf;
  	*(st->pRxWr) = c;
        st->pRxWr++;                                                           
	
		if(st->pRxWr - st->RxBuf >= sizeof(st->RxBuf)-1)
				st->pRxWr = st->RxBuf;
        st->last_receive=OSTimeGet(&err);
}
*/
/*
u32 GPRS_Detect_Handle(u32 timestamp)
{
    u32 length;
    if(timestamp-uart_gprs.last_receive>10)
    {
        if(uart_gprs.pRxWr-uart_gprs.RxBuf<MIN_GPRS_DATALENGTH)
        {
            ResetUartBuf(&uart_gprs);
            return 0;
        }
        else
        {
            length=Get_GPRS_Receive(USART2_RX_BUF_middle+1,sizeof(USART2_RX_BUF_middle)-1);
            if(length)
            {
                USART2_RX_BUF_middle[0]=(u8)length;
                USART2_RX_FLAG=1;
            }
            
            ResetUartBuf(&uart_gprs);
            return length;
        }
    }
    //else ResetUartBuf(&uart_gprs);
    return 0;
}
*/
void UART4_Init(u32 bound)
{ 	
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	// GPIOD时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE); //串口4时钟使能
 	USART_DeInit(UART4);  //复位串口4
		 //UART4_TX   PC.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化PC.10
    //UART4_RX	  PC.11GPIO_Mode_IPU
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入   GPIO_Mode_IPU
  GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化PC.11
	
	USART_InitStructure.USART_BaudRate = bound;//波特率一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  
	USART_Init(UART4, &USART_InitStructure); //初始化串口	4  
	USART_Cmd(UART4, ENABLE);                    //使能串口 	
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启中断
	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}

void NB_Init(void)//NB初始化
{
	GPIO_InitTypeDef GPIO_InitStructure;
	UARTParameterConfiguration gUARTSystemParameter;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 
	//复位管脚
	GPIO_InitStructure.GPIO_Pin = PIN_GPRS_RESET;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(PORT_GPRS_RESET, &GPIO_InitStructure);					  
 					 
	GPRS_RESET_LOW;
	
	UART4_Init(9600);

}