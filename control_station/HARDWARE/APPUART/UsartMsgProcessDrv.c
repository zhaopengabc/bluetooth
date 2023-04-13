/*  BEGIN_FILE_HDR
************************************************************************************************
*   NOTICE                              
************************************************************************************************
*   File Name       :  
************************************************************************************************
*   Project/Product : 
*   Title           : 
*   Author          : 
************************************************************************************************
*   Description     : 
*
************************************************************************************************
*   Limitations     : 
*
************************************************************************************************
*
************************************************************************************************
*   Revision History:
* 
*    Version     Date           Initials       CR#           Descriptions
*    --------    ----------     -----------    ----------    ---------------
*    1.0         2017/08/03     ning.zhao     N/A           Original                    
************************************************************************************************
* END_FILE_HDR*/

/* include files */
#include "UsartMsgProcessDrv.h"

UARTParameterConfiguration  gUARTSystemParameter;

void USARTx_GPIO_Configuration(USART_TypeDef* USARTx)
{
  GPIO_InitTypeDef 		GPIO_InitStructure;
	if(USARTx==USART1) 
	  {		
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	 
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 	
			 //USART1_TX   PA.9
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;  
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	 
			GPIO_Init(GPIOA, &GPIO_InitStructure);	 
			//USART1_RX	  PA.10
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
			GPIO_Init(GPIOA, &GPIO_InitStructure);  		
			RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1,ENABLE); 
			RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1,DISABLE); 
		}			
  else if(USARTx==USART2)
  	{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE); 
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE); 
			//USART2_TX   PA.2 
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
			GPIO_Init(GPIOA, &GPIO_InitStructure);
			//USART2_RX	  PA.3   
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
			GPIO_Init(GPIOA, &GPIO_InitStructure);  
			RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,ENABLE); 
			RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,DISABLE); 
	  }
  else if(USARTx==USART3)
  	{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE); 
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); 
			//USART3_TX   PB.10 
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	 
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	 
			GPIO_Init(GPIOB, &GPIO_InitStructure);
			//USART3_RX	  PB.11   
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
			GPIO_Init(GPIOB, &GPIO_InitStructure);  
			RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3,ENABLE); 
			RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3,DISABLE); 
	  }
  else if(USARTx==UART4)
  	{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
			//UART4_TX   PC.10 
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
			GPIO_Init(GPIOC, &GPIO_InitStructure);
			//UART4_RX	 PC.11   	
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
			GPIO_Init(GPIOC, &GPIO_InitStructure);		
			RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART4,ENABLE); 
			RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART4,DISABLE); 		
	  }
  else if(USARTx==UART5)
  	{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
			//UART5_TX   PC.12 
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
			GPIO_Init(GPIOC, &GPIO_InitStructure);
			//UART5_RX	 PD.2   	
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
			GPIO_Init(GPIOD, &GPIO_InitStructure);		
			RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART5,ENABLE); 
			RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART5,DISABLE); 
		}	 
}
		

void USARTx_Configuration(USART_TypeDef* USARTx,UARTParameterConfiguration UARTParameter)
{	
    USART_InitTypeDef USART_InitStructure;
	
    USART_InitStructure.USART_BaudRate = UARTParameter.USARTx_BaudRate;
    USART_InitStructure.USART_WordLength = UARTParameter.USARTx_WordLength; //USART_WordLength_8b
    USART_InitStructure.USART_StopBits = UARTParameter.USARTx_StopBits;//USART_StopBits_1
    USART_InitStructure.USART_Parity = UARTParameter.USARTx_Parity;//USART_Parity_No
    USART_InitStructure.USART_HardwareFlowControl = UARTParameter.USARTx_HardwareFlowControl;//USART_HardwareFlowControl_RTS_CTS
    USART_InitStructure.USART_Mode = UARTParameter.USARTx_Mode;//USART_Mode_Rx | USART_Mode_Tx
    USART_Init(USARTx, &USART_InitStructure);
    USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
    /* Enable USARTX */
    USART_Cmd(USARTx, ENABLE);
}

void NVIC_Usartx_Configuration(USART_TypeDef* USARTx,uint8_t UARTx_IRQn,uint8_t NVIC_Priority,uint8_t NVIC_SubPriority ) 
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = UARTx_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_Priority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_SubPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
    USART_Cmd(USARTx, ENABLE);
}
void USARTx_Mode_Init(USART_TypeDef* USARTx,UARTParameterConfiguration UARTParameter)
{
	USARTx_GPIO_Configuration(USARTx);
	
	USARTx_Configuration(USARTx,UARTParameter);	
	
	NVIC_Usartx_Configuration(USARTx,UARTParameter.UARTx_IRQn,UARTParameter.NVIC_Priority,UARTParameter.NVIC_SubPriority); 
}
unsigned char UARTx_Send_Data(USART_TypeDef* USARTx,unsigned char *Data,unsigned int Lenth)
{
	unsigned int i=0;
	for(i=0;i<Lenth;i++)
	{
		  while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET); 
			USART_SendData(USARTx, Data[i]);
  } 
}

 

/***********************************************END*********************************************/
