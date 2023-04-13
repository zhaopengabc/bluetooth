#include "stm32f10x.h"
#include "usart2.h"
#include "string.h"
#include "UsartMsgProcessAppExt.h"

unsigned char usart2_receive_data[50]={0};
unsigned char usart2_receive_data_inc=0;
void (*pUsart2Rx_Isr)(unsigned char);
void Usart2_init(unsigned int bound,unsigned char parity)
{
	UARTParameterConfiguration UARTParameter;
	USARTx_GPIO_Configuration(USART2);
	
	UARTParameter.USARTx_BaudRate=115200;
	UARTParameter.USARTx_WordLength=USART_WordLength_8b;
	UARTParameter.USARTx_StopBits=USART_StopBits_1;
	UARTParameter.USARTx_Parity=USART_Parity_No;
	UARTParameter.USARTx_HardwareFlowControl=USART_HardwareFlowControl_None;
	UARTParameter.USARTx_Mode=USART_Mode_Rx | USART_Mode_Tx;
	
	USARTx_Configuration(USART2,UARTParameter);
	NVIC_Usartx_Configuration(USART2,USART2_IRQn,0,3);
	pUsart2Rx_Isr=NULL;
}
unsigned char USART2_Send_Data(unsigned char *Data,unsigned int Lenth)
{
	UARTx_Send_Data(USART2,Data,Lenth);
	//FORTEST 
//	UARTx_Send_Data(USART1,Data,Lenth);
	return 1;
}

//串口2,printf 函数
//确保一次发送数据不超过USART2_MAX_SEND_LEN字节
void GM3_Send_Data(unsigned char* data)  
{  
	u16 i,j; 
	i=strlen((const char*)data);		//此次发送数据的长度
	UARTx_Send_Data(USART2,data,i);
}

//GM3
//void USART2_IRQHandler(void)
//{
//    unsigned char data;
//	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
//    {
//		data=USART2->DR;
//		
//		usart2_receive_data[usart2_receive_data_inc]=data;
//		usart2_receive_data[usart2_receive_data_inc+1]=0;			
//		usart2_receive_data_inc=usart2_receive_data_inc+1;
//			if(usart2_receive_data_inc>=49)
//			{
//				usart2_receive_data_inc=0;
//			}
//		(*pUsart2Rx_Isr)(data);
////#if		USART_USART2_TRANS
//			//ZHAONING...................
// 		USART1->DR =data;
////#endif
//    }
//}

//测试程序；
//void Usart2_Process(void)
//{
//    if(Usart2RxBuf[Usart2_Rd_Addr].Flag==1)
//    {
//        USART2_Send_Data(Usart2RxBuf[Usart2_Rd_Addr].data,Usart2RxBuf[Usart2_Rd_Addr].Lenth);
//        Usart2RxBuf[Usart2_Rd_Addr].Flag=0;
//        Usart2RxBuf[Usart2_Rd_Addr].Lenth=0;
//        memset(Usart2RxBuf[Usart2_Rd_Addr].data,0,USART2_RX_DATANUM);
//        Usart2_Rd_Addr++;
//        if(Usart2_Rd_Addr>=USART2_RX_GROUPNUM)Usart2_Rd_Addr=0;
//    }
//}
//void Usart2_Test(void)
//{
//    while(Usart2_Wr_Addr!=Usart2_Rd_Addr)
//    {
//        Usart2_Process();
//    }
//}
