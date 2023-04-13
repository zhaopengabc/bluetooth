#ifndef  REVEIVEDATABUFFER_H
#define  REVEIVEDATABUFFER_H

#define HOST_DATA_NUM 0x200
#define CAN_RX_DATANUM 50
#define CAN_RX_GROUPNUM 20

//服务器接口方式
typedef enum
{
	RS485_232_Mode=1,
	CAN_Mode
}HostInterMode_TypeDef;

extern unsigned char Host_Interface_Mode;

typedef struct
{
	unsigned char Flag;
	unsigned int Lenth;
	unsigned char data[HOST_DATA_NUM];
}Host_Data_Typedef;

extern Host_Data_Typedef Data_RecieveFromHost;

//extern void delay_us(unsigned int n);
//主机接串口
extern void YH_UART_IRQHandler(void);
extern void YH_UART3_IRQHandler(void);
extern void Uart4_RxISR(unsigned char buf);
extern unsigned int RS485_Read(unsigned char *Rxdata);
extern unsigned char UART4_Send_Data(unsigned char *Data,unsigned int Lenth);
extern void Reset_Uart4RxBuf(void);
//主机接CAN口
extern void YH_CAN_RX0_IRQHandler(void);
extern unsigned int CAN_Read(unsigned char *Rxdata);
extern unsigned char Can_Send_Msg(unsigned char *msg, unsigned char len);
//主机接RS485
extern void GPIO_RS485_Init(void);
extern unsigned char RS485_Send_Data(unsigned char *Data,unsigned int Lenth);
#endif
