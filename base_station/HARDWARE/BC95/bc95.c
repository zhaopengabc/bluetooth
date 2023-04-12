#include "bc95.h"

/*******************************************************************************
*                       USR-GM3模块驱动
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
#include "usart2.h"
#include <string.h>
#include "rtc.h"
//#include "common.h"
#include "stm32f10x_gpio.h"
#include "delay.h"
#include "os.h"

/*********************变量定义*******************************************************/
const char ATOK[]="OK\r\n";
const char AT_ERROR[]="ERROR\r\n";
const char ATCCLK[]="+CCLK:";
const char ATCSQ[]="+CSQ:";
const char ATCGATT[]="+CGATT:";
const char ATCPSMS[]="+CPSMS";
const char ATCEDRXS[]="+CEDRXS";
const char ATCPSMS0[]="+CPSMS:0";
const char ATCEDRXS0[]="+CEDRXS:5";
const char ATCGSN[]="+CGSN:";
const char ATCEREG[]="+CEREG:";
const char ATNBAND[]="+NBAND:";
const char ATNSMI[]="+NSMI";
const char ATNNMI[]="+NNMI";
const char ATNSONMI[]="+NSONMI:";
const char ATCGPADDR[]="+CGPADDR:";
const char AT_CLOSED[]="CLOSE";
const char AT_CGATT1[]="+CGATT:1";
const char AT_CEREG1[]="+CEREG:1,1";
const char AT_CEREG5[]="+CEREG:1,5";

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
u32 socket_port=0;					//socket端口号
//static unsigned char remote_ip[4]={172,16,10,63};		/*远端IP地址*/
//u8 remote_ip_str[]="172.16.10.63";
//static unsigned char remote_ip[4]={124,192,224,226};		/*远端IP地址*/
u8 remote_ip_str[]="124.192.224.226";
static unsigned int remote_port=5015;//9223;			/*远端端口号9223;*/

//频点锁定标志；若信号正常且联网正常则锁定此频点；netlockflag置1；
//设备重启标志清零；
static unsigned char NetLockFlag = 0;
//设备上电重启，频点处理标志;设备上电重启此时标志位置位，判断标志位若置位则进行频点处理；
static unsigned char NetPowerFlag = 0;
//设备频点存储，初次上电读取频点，并存储；之后若需要处理则依次加一处理；
static unsigned char NetNum=0;//0:初次上电，进行读取1：B3，:2:B5,3:B8

/*********************函数声明******************************************************/

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
    
    GPIO_InitStructure.GPIO_Pin = PIN_GPRS_MP_EN; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//复用推挽输出
    GPIO_Init(PORT_GPRS_MP_EN, &GPIO_InitStructure);//初始化GPIOA.9
    
    GPIO_InitStructure.GPIO_Pin = PIN_GPRS_PWR; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//复用推挽输出
    GPIO_Init(PORT_GPRS_PWR, &GPIO_InitStructure);//初始化GPIOA.9
    
    
	VGPRS_MP_EN_LOW;
	GPRS_PWR_LOW;
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
    USART2_SEND(data,strlen);
}

void MP1482_POWERON(void)
{
  VGPRS_MP_EN_HIGH;
#if	NB_PRINTF
  USART1_sendstr("MP1482_On!!!\r\n");
#endif
}
void MP1482_POWEROFF(void)
{
	VGPRS_MP_EN_LOW;
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
		GPRS_PWR_HIGH;
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
		GPRS_PWR_LOW;
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
    socket_port=0;
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
void itoa(unsigned int n,unsigned char str[5], unsigned char len)
{
	unsigned char i=len-1;
	memset(str,0x20,len);
	do{
		str[i--]=n%10+'0';
	}while((n/=10)>0);
	return;
}

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
#if COAP_EN
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
                memcpy(Data_Frame,"AT+NBAND=8\r\n",12);//查询频段
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
                if(NetNum==1) memcpy(Data_Frame,"AT+NBAND=3\r\n",12);//查询频段
                else memcpy(Data_Frame,"AT+NBAND=8\r\n",12);//查询频段
            }
            else//3.5.11中国电信
            {
                memcpy(Data_Frame,"AT+NBAND=5\r\n",12);//查询频段
            }
			Count = 12;
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
//            ATGetMsg();
//        ResetUartBuf(&uart_gprs);
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
                Count = sizeof(SEVER_ADDRESS_CTCC)-1;//设置通讯ID和端口
			    memcpy(Data_Frame,SEVER_ADDRESS_CTCC,Count);
            }
            else if((MNC==1)||(MNC==6)||(MNC==9))//中国联通
            {
                Count = sizeof(SEVER_ADDRESS_CUCC)-1;//设置通讯ID和端口
			    memcpy(Data_Frame,SEVER_ADDRESS_CUCC,Count); 
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
        case NB_CEDRXS_0:
            memcpy(Data_Frame,"AT+CEDRXS=3,5\r\n",15);//查询信号强度
			Count = 15;
        break;
        case NB_CEDRXS:
            memcpy(Data_Frame,"AT+CEDRXS?\r\n",12);//查询信号强度
			Count = 12;
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
#else
void Send_AT_Command(unsigned char step)
{
    static u32 portcnt=1000;
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
                memcpy(Data_Frame,"AT+NBAND=8\r\n",12);//查询频段
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
                if(NetNum==1) memcpy(Data_Frame,"AT+NBAND=3\r\n",12);//查询频段
                else memcpy(Data_Frame,"AT+NBAND=8\r\n",12);//查询频段
            }
            else//3.5.11中国电信
            {
                memcpy(Data_Frame,"AT+NBAND=5\r\n",12);//查询频段
            }
			Count = 12;
		break;
        case  NB_QREGSWT:
            memcpy(Data_Frame,"AT+QREGSWT=1\r\n",14);
			Count = 14;												
		break;
        case  NB_SCR:
			memcpy(Data_Frame,"AT+NCONFIG=CR_0354_0338_SCRAMBLING,TRUE\r\n",41);
			Count = 41;												
		break;
        case  NB_AVOID:
			memcpy(Data_Frame,"AT+NCONFIG=CR_0859_SI_AVOID,TRUE\r\n",34);
			Count = 34;				
		break;
//		case  NB_NSMI1:
//			memcpy(Data_Frame,"AT+NSMI=0\r\n",11);//发送信息标志，发送信息后会有回馈
//			Count = 11;
//		break;
//		case  NB_NNMI1:
//			memcpy(Data_Frame,"AT+NNMI=1\r\n",11);//接收信息标志开，接到新消息会有回馈
//			Count = 11;
//		break;
        case NB_CGMR:
//            ATGetMsg();
//        ResetUartBuf(&uart_gprs);
            memcpy(Data_Frame,"AT+CGMR\r\n",9);//关闭射频功能
			Count = 9;
		break;
        case NB_NCSEARFCN:
            memcpy(Data_Frame,"AT+NCSEARFCN\r\n",14);//关闭射频功能
			Count = 14;
        break;
//		case  NB_NCDP:
//            if((MNC==0)||(MNC==2)||(MNC==4)||(MNC==7))//中国移动
//            {
//                Count = sizeof(SEVER_ADDRESS_CTCC)-1;//设置通讯ID和端口
//			    memcpy(Data_Frame,SEVER_ADDRESS_CTCC,Count);
//            }
//            else if((MNC==1)||(MNC==6)||(MNC==9))//中国联通
//            {
//                Count = sizeof(SEVER_ADDRESS_CUCC)-1;//设置通讯ID和端口
//			    memcpy(Data_Frame,SEVER_ADDRESS_CUCC,Count); 
//            }
//            else//3.5.11中国电信
//            {
//                Count = sizeof(SEVER_ADDRESS_CTCC)-1;//设置通讯ID和端口
//			    memcpy(Data_Frame,SEVER_ADDRESS_CTCC,Count);
//            }
//		break;
		
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
        case NB_CEDRXS_0:
            memcpy(Data_Frame,"AT+CEDRXS=3,5\r\n",15);//查询信号强度
			Count = 15;
        break;
        case NB_CEDRXS:
            memcpy(Data_Frame,"AT+CEDRXS?\r\n",12);//查询信号强度
			Count = 12;
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
//		case  NB_CGPADDR:
//			memcpy(Data_Frame,"AT+CGPADDR\r\n",12);//查询核心网分配的ip地址
//			Count = 12;
//            NetLockFlag = 1;
//		break;
        case  NB_CREATPORT:
			NetLockFlag = 1;
			if(socket_port != 0)
			{
				if((socket_port<'0')||(socket_port>'7'))
				{
					socket_port = 0;
					Count = 0;
				}
				else
				{
					memcpy(Data_Frame,"AT+NSOCL=",9);
					Data_Frame[9] = socket_port;
					Data_Frame[10] = '\r';
					Data_Frame[11] = '\n';
					Count = 12;
				}
			}
			else
			{
				portcnt++;
				memcpy(Data_Frame,"AT+NSOCR=DGRAM,17,4587,1",24);//字符18-字符21应随机变化或累加1
				Data_Frame[18] = (portcnt%10000)/1000+'0';
				Data_Frame[19] = (portcnt%1000)/100+'0';
				Data_Frame[20] = (portcnt%100)/10+'0';
				Data_Frame[21] = portcnt%10+'0';
				Data_Frame[24] = '\r';
				Data_Frame[25] = '\n';
				Count = 26;
			}
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

#endif

//读取接收信息，返回1；
unsigned char ATGetMsg(void)
{
	unsigned char Flag=1;
	unsigned char Data_Frame[40];
	
    ResetUartBuf(&uart_gprs);
    memset(Data_Frame,0,20);
	memcpy(Data_Frame,"AT+CGDCONT=1,\"IP\",\"CMNBIOT1\"\r\n",30);
	NB_UsartSendstr(Data_Frame,30);
    delay_ms(1000);
	memset(Data_Frame,0,20);
	memcpy(Data_Frame,"AT+CGDCONT?\r\n",13);
	NB_UsartSendstr(Data_Frame,13);
    delay_ms(1000);
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
#if COAP_EN
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
            case  NB_CEDRXS_0:
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
            case  NB_CEDRXS:
				if((lenth>=sizeof(ATCEDRXS))&&(strstr((const char *)temp_Buffer,ATCEDRXS ))&&(strstr((char const*)temp_Buffer, ATOK)))
				{
					if(strstr((const char *)temp_Buffer,ATCEDRXS0))
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
            case  NB_CEDRXS_0:
            case NB_CPSMS:
            case  NB_CEDRXS:
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

#else
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
			//case  NB_NSMI1:
			//case  NB_NNMI1:
            case  NB_NCSEARFCN:
            case  NB_CPSMS_0:
            case  NB_CEDRXS_0:
			//case  NB_NCDP:
			case  NB_CGATT_1:
			case  NB_CEREG_1:
            case  NB_AVOID:
            case  NB_QREGSWT:
			case  NB_SCR:
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
            case  NB_CEDRXS:
				if((lenth>=sizeof(ATCEDRXS))&&(strstr((const char *)temp_Buffer,ATCEDRXS ))&&(strstr((char const*)temp_Buffer, ATOK)))
				{
					if(strstr((const char *)temp_Buffer,ATCEDRXS0))
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
						NB_Link_step=NB_CREATPORT;
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
//			case  NB_CGPADDR:
//				if((lenth>=sizeof(ATCGPADDR))&&(strstr((char const*)temp_Buffer,ATOK))&&(strstr((char const*)temp_Buffer,ATCGPADDR)))
//				{
//					//ASL.................缺少格式判定；
//					//注册网络成功，
//					NB_Link_step=NB_CONNECT;
//				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
//				{
//					ErrorFlag=1;
//				}
//				else
//				{
//				}
//			break;
            case  NB_CREATPORT:
				//将读取的数据打印出来，读取socket端口号；
				if((socket_port == 0)&&(lenth==11)&&(temp_Buffer[2]>='0')&&(temp_Buffer[2]<='7')&&((strstr((char const*)temp_Buffer,ATOK))))
				{
					socket_port=temp_Buffer[2];
					NB_Link_step=NB_CONNECT;
				}else if((socket_port!=0)&&((strstr((char const*)temp_Buffer,ATOK))))
				{
//					socket_port=0;
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
//			case  NB_NSMI1:
//			case  NB_NNMI1:
            case  NB_QREGSWT:
            case  NB_SCR:
			case  NB_AVOID:
            case  NB_CGMR:
            case  NB_NCSEARFCN:
            case  NB_CPSMS_0:
            case  NB_CEDRXS_0:
            case NB_CPSMS:
            case  NB_CEDRXS:
//			case  NB_NCDP:
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
            case  NB_CREATPORT:
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
#endif
/*****************************************************************************
* 函数名称: void Read_Port(void)
* 功能描述: 读取socket端口信息命令
* 参    数:	
* 返回  值:
*****************************************************************************/
unsigned char Data1_Frame[100];
unsigned char AT_ReadPort(unsigned int lenth)
{
	unsigned char i,Flag=1;
	if(lenth > RX_BUF_LEN)return 0;
	memset(Data1_Frame,0,100);
    ResetUartBuf(&uart_gprs);
	memcpy(Data1_Frame,"AT+NSORF=",9);
	Data1_Frame[9] = socket_port;//+'0';
	Data1_Frame[10] = ',';
	i=int16toString(lenth,(unsigned char *)&Data1_Frame[11]);
	Data1_Frame[11+i] = '\r';
	Data1_Frame[12+i] = '\n';
#if	NB_PRINTF
	USART1_sendstr("NB_TX:");
	USART1_sendstr(Data1_Frame,26);
	USART1_sendstr("\r\n");
#endif
	NB_UsartSendstr(Data1_Frame,13+i);
	return Flag;
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
			delay_ms(100);									// 延时100ms
            delay_ms(1);
		}
	}
	if (t >= 600)											// 挂载网络超时
	{
		reset_gprs();									// 复位模块
		delay_ms(5000);									// 延时5000ms
		return 1;
	}
	return 0;
}
u32 Wait_Signal(u32 max_time)
{
    OS_ERR err;
    u32 starttamp,timetamp;
    ResetUartBuf(&uart_gprs);
    starttamp=OSTimeGet(&err);
    while(1)
    {
         timetamp=OSTimeGet(&err);
        if(starttamp<uart_gprs.last_receive)
        { 
            if(timetamp-uart_gprs.last_receive>10) break;
        }
        if(timetamp-starttamp>max_time) break;
        delay_ms(10);
    }
    return uart_gprs.last_receive-starttamp;
}
#if COAP_EN
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
    for(xi=0;xi<3;xi++)
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
#else
u32 GPRS_senddata(u8 *Data,u32 DataLen)
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
    for(xi=0;xi<3;xi++)
    {
        ResetUartBuf(&uart_gprs);
        memset(ReadData, 0, sizeof(ReadData));
        memset(Data_Frame, 0, sizeof(Data_Frame));
        i = 0;
        memcpy(Data_Frame,"AT+NSOST=",9);
        Count = 9;
        Data_Frame[Count++] = socket_port;	//发送socket号
        Data_Frame[Count++] = ',';
//        Data_Frame[Count++] = '0'+remote_ip[0]/100;
//        Data_Frame[Count++] = '0'+remote_ip[0]%100/10;
//        Data_Frame[Count++] = '0'+remote_ip[0]%10;
//        Data_Frame[Count++] = '.';
//        Data_Frame[Count++] = '0'+remote_ip[1]/100;
//        Data_Frame[Count++] = '0'+remote_ip[1]%100/10;
//        Data_Frame[Count++] = '0'+remote_ip[1]%10;
//        Data_Frame[Count++] = '.';
//        Data_Frame[Count++] = '0'+remote_ip[2]/100;
//        Data_Frame[Count++] = '0'+remote_ip[2]%100/10;
//        Data_Frame[Count++] = '0'+remote_ip[2]%10;
//        Data_Frame[Count++] = '.';
//        Data_Frame[Count++] = '0'+remote_ip[3]/100;
//        Data_Frame[Count++] = '0'+remote_ip[3]%100/10;
//        Data_Frame[Count++] = '0'+remote_ip[3]%10;
        memcpy(&Data_Frame[Count],remote_ip_str,sizeof(remote_ip_str));
        Count+=(sizeof(remote_ip_str)-1);
        Data_Frame[Count++] = ',';
//        Count=27;
        
        Len =judgeLen(remote_port);					//计算位数
        itoa(remote_port, ReadData, Len);			//转换成字符串
        for(i = 0; i< Len;i++)
        {
            Data_Frame[Count++] =  ReadData[i];
        }
        Data_Frame[Count++] = ',';
        Len =	judgeLen(DataLen);					//计算位数
        itoa(DataLen, ReadData, Len);				//转换成字符串
        for(i = 0; i< Len;i++)
        {
            Data_Frame[Count++] =  ReadData[i];
        }
        Data_Frame[Count++] = ',';
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
#endif
u32 GPRS_buffer_send(u8 *str,u32 DataLen)
{
    u32 err,length;
    err=GPRS_senddata(str,DataLen);
    if(err) return err;
    err=gprs_receive_check();
    if(!err) 
    {
        length=Get_GPRS_Receive(USART2_RX_BUF_middle+1,sizeof(USART2_RX_BUF_middle)-1);
        USART2_RX_BUF_middle[0]=(u8)length;
        USART2_RX_FLAG=1;
        ResetUartBuf(&uart_gprs);
    }
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
    u32 lenth,length;
    u32 i,j;
    u32 rxlen;
    //ResetUartBuf(&uart_gprs);
    //ATGetMsg();
    Wait_Signal(25000);
    memset(&temp_Buffer, 0, RX_BUF_LEN);
    lenth = UartRead(&uart_gprs, temp_Buffer, RX_BUF_LEN);
    if((lenth>=sizeof(ATNSONMI)+2)&&(strstr((char const*)temp_Buffer, ATNSONMI)))
    {
        
    }
    else return 1;
    i=IsStrInclude(temp_Buffer,(unsigned char *)ATNSONMI,lenth);
    rxlen=0;
    for(j=i+2;j<lenth;j++)
    {
        if((temp_Buffer[j]>='0')&&(temp_Buffer[j]<='9'))
        {
            rxlen=rxlen*10;
            rxlen+=temp_Buffer[j]-'0';
        }
        else break;
    }
    if((temp_Buffer[i]==socket_port)&&(rxlen!=0))
    {
        if(rxlen<RX_BUF_LEN-200)
        {
            AT_ReadPort(rxlen);
        }
        else
        {
            AT_ReadPort(RX_BUF_LEN-200);
        }
    }
    else
    {
#if	NB_PRINTF
        //打印错误信息
        USART1_sendstr("ERROR:NB_CONNECT\r\n");
        USART1_sendstr("rx错误数据：");
        USART1_sendstr(temp_Buffer);
        USART1_sendstr("\r\n");
#endif
    }
    Wait_Signal(2000);
    memset(&temp_Buffer, 0, RX_BUF_LEN);
    lenth = UartRead(&uart_gprs, temp_Buffer, RX_BUF_LEN);
    if((lenth>=sizeof(remote_ip_str)+2)&&(strstr((char const*)temp_Buffer, remote_ip_str))) return 0;
    else return 1;
}
u32 gprs_check_connect(void)
{
    volatile u32 length;
    u8 command[11]="AT+CEREG?\r\n";
    //u8 command[11]="AT+CGATT?\r\n";
    USART2_SEND(command,11);//USART2_SEND(command,11);
    Wait_Signal(10000);
    memset(&temp_Buffer, 0, RX_BUF_LEN);
    length = UartRead(&uart_gprs, temp_Buffer, RX_BUF_LEN);
    if(strstr((char const*)temp_Buffer, AT_CEREG1)) return 0;
    //if(strstr((char const*)temp_Buffer, AT_CGATT1)) return 0;
    if(strstr((char const*)temp_Buffer, AT_CEREG5)) return 0;
    else return 1;
}
u8 data_transform(u8 data)
{
    if((data>='0')&&(data<='9')) return data-'0';
    if((data>='a')&&(data<='z')) return data-'a'+10;
    if((data>='A')&&(data<='Z')) return data-'A'+10;
    else return 0xff;
}
#if COAP_EN
u32 Get_GPRS_Receive(u8 *str,u32 max_length)
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
    //delay_ms(50);
    if(j==length*2) return length;
    else return 0;
    
}
#else
u32 Get_GPRS_Receive(u8 *str,u32 max_length)
{
    u32 i,j,k,length,add;
    u32 counter=0;
    length=uart_gprs.pRxWr-uart_gprs.RxBuf;
    for(i=0;i<length;i++)
    {
        if(uart_gprs.RxBuf[i]==',') counter++;
        if(counter==4) break;
    }
    if(uart_gprs.RxBuf[i]!=',') return 0;
    length=0;
    for(k=1,add=1;k<=3;k++)
    {
        if(data_transform(uart_gprs.RxBuf[i-k])==0xff) break;
        length+=data_transform(uart_gprs.RxBuf[i-k])*add;
        add*=10;
    }
    if(length==0) return 0;
    if(length>max_length) length=max_length;
    for(j=0;j<length*2;j++)
    {
        if(data_transform(uart_gprs.RxBuf[i+j+1])==0xff) break;
        if(j%2) *(str+j/2)+=data_transform(uart_gprs.RxBuf[i+j+1]);
        else *(str+j/2)=data_transform(uart_gprs.RxBuf[i+j+1])*16;
    }
    //delay_ms(50);
    if(j==length*2) return length;
    else return 0;
    
}
#endif
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
  do
  {
    OpenGPRS_check();
    open_flag = gprs_send_start();
  }while(open_flag);
    
}

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