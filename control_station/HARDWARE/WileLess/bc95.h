#ifndef _BC95_H
#define _BC95_H
//#include "connect.h"
#include <string.h>
//#include "os.h"
#include "stm32f10x.h"
/***********************ͷ�ļ�**************************************************/
//#include "stm8l15x.h"
/***********************�궨��***************************************************/
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

#define RX_BUF_LEN 255 // ������ջ���������
#define TX_BUF_LEN 1  // ���巢�ͻ���������
typedef struct// ���ն������ݽṹ
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

//	GPRS_RESET----PD5 ���;0����ֹ 1����λ
//	VGPRS_MP_EN---PB0 �����0����ֹ 1��ʹ�����
//  GPRS_PWR------PB4 �����0����ֹ 1��ʹ��
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
	NBLINK= 0,						/*AT����*/
    NB_CIMI,						/*��ѯIMSI�ţ��о�����USIM��*/  //Ӧ����Ҫ
    NB_OPEN_FUN,          			/*�رչ���*/    //�����ܲ���ȥ���� 
	NB_NCSEARFCN,
    
    NB_NBAND,						/*��ѯģ�鹤��Ƶ��*/
    NB_NCDP,						/*����Cdp������*/
    NB_CGMR,
    //NB_CPSMS,
    NB_AUTOCONNECT,					/*�Զ�����*/    //�ֶ�����
	NB_NRB,							/*����*/
	
    //NB_NSMI1,						/*������Ϣʹ��*/
	//NB_NNMI1,						/*������Ϣ��ʾʹ��*/
    
   
    
	NB_CGSN,						/*��ѯIMEI��*/                  //Ӧ����Ҫ
    NB_NSMI1,						/*������Ϣʹ��*/           //��Ҫ��
	NB_NNMI1,						/*������Ϣ��ʾʹ��*/        //��Ҫ��
    NB_CPSMS_0,                                                
    NB_CPSMS,
    NB_CEREG_1,						/*��ѯע������״̬*/       //��Ҫ��
    NB_CGATT_1,						/*��ѯ����״̬*/
	NB_CSQ,							/*��ѯ�ź�ǿ��*/
	
	NB_CEREG,						/*��ѯע������״̬*/        //��Ҫ��
	NB_CGATT,						/*��ѯ����״̬*/
	
	NB_CGPADDR,
	NB_CONNECT,						/*��������״̬*/
    NB_QNBAND,						/*��ѯģ�鹤��Ƶ��*/
}AT_link_ENUM;

/*********************��������****************************************************/
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

void NB_Init(void);//NB��ʼ��
extern volatile unsigned char	NB_RegistFlag;
extern volatile unsigned char	W5_RegistFlag;
u32 Get_GPRS_Receive_Length();
/*********************�ⲿ��������*************************************************/

#endif
