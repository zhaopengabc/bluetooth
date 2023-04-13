#include "stm32f10x.h"
#include "usart3.h"
#include "string.h"
#include "UsartMsgProcessAppExt.h"

void (*pUsart3Rx_Isr)(unsigned char);
void Usart3_init(unsigned int bound,unsigned int parity,unsigned int stopbits)
{
	UARTParameterConfiguration UARTParameter;
	USARTx_GPIO_Configuration(USART3);
	
	UARTParameter.USARTx_BaudRate=115200;
	UARTParameter.USARTx_WordLength=USART_WordLength_8b;
	UARTParameter.USARTx_StopBits=USART_StopBits_1;
	UARTParameter.USARTx_Parity=USART_Parity_No;
	UARTParameter.USARTx_HardwareFlowControl=USART_HardwareFlowControl_None;
	UARTParameter.USARTx_Mode=USART_Mode_Rx | USART_Mode_Tx;
	
	USARTx_Configuration(USART3,UARTParameter);
	NVIC_Usartx_Configuration(USART3,USART3_IRQn,0,3);
	pUsart3Rx_Isr=NULL;
}
unsigned char USART3_Send_Data(unsigned char *Data,unsigned int Lenth)
{
	UARTx_Send_Data(USART3,Data,Lenth);
	return 1;
}
//void USART3_IRQHandler(void)
//{
//    unsigned char data;
//	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
//    {
//		data=USART3->DR;
//		(*pUsart3Rx_Isr)(data);
//#if		USART_USART3_TRANS
//		USART1->DR =data;
//#endif
//    }
//}
