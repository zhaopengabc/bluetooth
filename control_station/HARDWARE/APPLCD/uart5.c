#include "stm32f10x.h"
#include "uart5.h"
#include "string.h"
#include "UsartMsgProcessAppExt.h"
unsigned char uart5_receive_data[50]={0};
unsigned char uart5_receive_data_inc=0;
void (*pUsart5Rx_Isr)(unsigned char);
void Uart5_init(unsigned int bound,unsigned char parity)
{
	UARTParameterConfiguration UARTParameter;
	USARTx_GPIO_Configuration(UART5);
	
	UARTParameter.USARTx_BaudRate=115200;
	UARTParameter.USARTx_WordLength=USART_WordLength_8b;
	UARTParameter.USARTx_StopBits=USART_StopBits_1;
	UARTParameter.USARTx_Parity=USART_Parity_No;
	UARTParameter.USARTx_HardwareFlowControl=USART_HardwareFlowControl_None;
	UARTParameter.USARTx_Mode=USART_Mode_Rx | USART_Mode_Tx;
	
	USARTx_Configuration(UART5,UARTParameter);
	NVIC_Usartx_Configuration(UART5,UART5_IRQn,0,3);
	pUsart5Rx_Isr=NULL;
}
unsigned char UART5_Send_Data(unsigned char *Data,unsigned int Lenth)
{
	UARTx_Send_Data(UART5,Data,Lenth);
	return 1;
}
void UART5_IRQHandler(void)
{
	unsigned char data;
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
    {
		data= UART5->DR;//读取接收到的数据
		uart5_receive_data[uart5_receive_data_inc]=data;
		uart5_receive_data[uart5_receive_data_inc+1]=0;
		uart5_receive_data_inc=uart5_receive_data_inc+1;
		if(uart5_receive_data_inc>=49)
		{
			uart5_receive_data_inc=0;
		}		
		(*pUsart5Rx_Isr)(data);
    }
}












