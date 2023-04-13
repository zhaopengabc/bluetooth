#ifndef _BC95_H
#define _BC95_H
//#include "connect.h"
#include <string.h>
//#include "os.h"
#include "stm32f10x.h"
/***********************头文件**************************************************/
//#include "stm8l15x.h"
/***********************宏定义***************************************************/
//#define SEVER_ADDRESS   "AT+NCDP=180.101.147.115,5683\r\n"
#define SEVER_ADDRESS_CTCC   "AT+NCDP=117.60.157.137,5683\r\n"
//#define SEVER_ADDRESS_CMCC   "AT+NCDP=117.60.157.137,5683\r\n"
#define SEVER_ADDRESS_CMCC   "AT+NCDP=58.240.96.46,5683\r\n"
//#define SEVER_ADDRESS_CMCC   "AT+NCDP=139.159.140.34,5683\r\n"

#define MIN_GPRS_DATALENGTH 3

#define TX_MAX_LEN       250

#define GPRS_SEND_ENABLE 0
#define GPRS_SEND_DISABLE 1
#define GPRS_SEND_LOCK 2

#define RX_BUF_LEN 255 // 定义接收缓冲区长度
#define TX_BUF_LEN 1  // 定义发送缓冲区长度
typedef struct// 接收队列数据结构
{
  volatile	unsigned char RxBuf[RX_BUF_LEN];
  volatile	unsigned char TxBuf[TX_BUF_LEN];
  volatile	unsigned char *pRxWr;
  volatile	unsigned char *pRxRd;
  volatile	unsigned char *pTxWr;
  volatile	unsigned char *pTxRd;
  volatile	u32 last_receive;
}stUart;
typedef stUart usart1_buf,usart2_buf,usart3_buf;

//extern usart1_buf uart_debug;
//extern usart2_buf uart_gprs;
//extern usart3_buf uart_rs485;
//#define NB_PRINTF 1

//	GPRS_RESET----PD5 输出;0：禁止 1：复位
//	VGPRS_MP_EN---PB0 输出；0：禁止 1：使能输出
//  GPRS_PWR------PB4 输出；0：禁止 1：使能
#ifdef FB1010 //RESET  PC13

#define PORT_GPRS_RESET         GPIOC
#define PIN_GPRS_RESET          GPIO_Pin_13

#define GPRS_RESET_HIGH				GPIO_SetBits(GPIOC,GPIO_Pin_13)
#define GPRS_RESET_LOW				GPIO_ResetBits(GPIOC,GPIO_Pin_13)

#endif



#ifdef FB1030 //RESET  PA15

#define PORT_GPRS_RESET         GPIOA
#define PIN_GPRS_RESET          GPIO_Pin_15

#define GPRS_RESET_HIGH				GPIO_SetBits(GPIOA,GPIO_Pin_15)
#define GPRS_RESET_LOW				GPIO_ResetBits(GPIOA,GPIO_Pin_15)

#endif

typedef enum
{
	NBLINK= 0,						/*AT连接*/
    NB_CIMI,						/*查询IMSI号，有就是有USIM卡*/  //应该需要
    NB_OPEN_FUN,          			/*关闭功能*/    //试试能不能去掉？ 
	NB_NCSEARFCN,
    
    NB_NBAND,						/*查询模块工作频段*/
    NB_NCDP,						/*配置Cdp服务器*/
    NB_CGMR,
    //NB_CPSMS,
    NB_AUTOCONNECT,					/*自动连接*/    //手动连接
	NB_NRB,							/*重启*/
	
    //NB_NSMI1,						/*发送消息使能*/
	//NB_NNMI1,						/*接收消息提示使能*/
    
   
    
	NB_CGSN,						/*查询IMEI号*/                  //应该需要
    NB_NSMI1,						/*发送消息使能*/           //需要吗
	NB_NNMI1,						/*接收消息提示使能*/        //需要吗
    NB_CPSMS_0,                                                
    NB_CPSMS,
    NB_CEREG_1,						/*查询注册网络状态*/       //需要吗
    NB_CGATT_1,						/*查询附着状态*/
	NB_CSQ,							/*查询信号强度*/
	
	NB_CEREG,						/*查询注册网络状态*/        //需要吗
	NB_CGATT,						/*查询附着状态*/
	
	NB_CGPADDR,
	NB_CONNECT,						/*网络连接状态*/
    NB_QNBAND,						/*查询模块工作频段*/
}AT_link_ENUM;

/*********************函数声明****************************************************/
void GPRS_IO_Init(void);
u8 OpenGPRS_check(void);//unsigned char OpenGPRS(void);
unsigned char Signal_Check(void);
unsigned char gprs_send_start(void);
u32 GPRS_senddata(u8 *str,u32 DataLen);
void reset_gprs(void);
u32 gprs_receive_check(void);
u32 Get_GPRS_Receive(u8 *str,u32 max_length);
u32 gprs_check_connect(void);
unsigned char sum_check(unsigned char *data, unsigned char len);
//unsigned int BC95_Tx_Frame(unsigned char *Data,unsigned int DataLen);
void ResetUartBuf(stUart *st);
unsigned int UartGetRxLen(stUart *st);
unsigned int UartRead(stUart *st, unsigned char *buf, unsigned int len);
void    gprs_link(void);
void UartRxISR(stUart *st,char c);
u32 GPRS_buffer_send(u8 *str,u32 DataLen);
u32 GPRS_Detect_Handle(u32 timestamp);

void NB_Init(void);//NB初始化
extern volatile unsigned char	NB_RegistFlag;
extern volatile unsigned char	W5_RegistFlag;
u32 Get_GPRS_Receive_Length();
/*********************外部变量声明*************************************************/

#endif
