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


/*Include files*/
#include "HRSTDDEF.h"
#include "UsartMsgProcessCfgExt.h"
/*declaration range definition*/

/***********************************************************************************************
* ALL definition and declaration can be used ,but can not revised outside this driver          *
************************************************************************************************/
/* macro  definition */

/* data type definition */
typedef struct
{
 HRINT32U USARTx_BaudRate;
 HRINT16U USARTx_WordLength;
 HRINT16U USARTx_StopBits;
 HRINT16U USARTx_Parity;
 HRINT16U USARTx_HardwareFlowControl;
 HRINT16U USARTx_Mode;
 HRINT8U UARTx_IRQn;
 HRINT8U NVIC_Priority;
 HRINT8U NVIC_SubPriority;
}UARTParameterConfiguration;


/* variable definition */

/* function declaration */
/***********************************************************************************************
USARTx:USART1 USART2  USART3 USART4 USART5  
USARTx_BaudRate:9600
                38400
                115200
USARTx_WordLength:USART_WordLength_8b
                  USART_WordLength_9b
USARTx_StopBits:USART_StopBits_1
                USART_StopBits_0_5
								USART_StopBits_2
								USART_StopBits_1_5
USARTx_Parity:USART_Parity_No
              USART_Parity_Even
							USART_Parity_Odd
USARTx_HardwareFlowControl:USART_HardwareFlowControl_None
                           USART_HardwareFlowControl_RTS
													 USART_HardwareFlowControl_CTS
													 USART_HardwareFlowControl_RTS_CTS
USARTx_Mode��USART_Mode_Rx
             USART_Mode_Tx
						 USART_Mode_Rx | USART_Mode_Tx
************************************************************************************************/
extern UARTParameterConfiguration gUARTSystemParameter;

extern void USARTx_GPIO_Configuration(USART_TypeDef* USARTx);
extern void USARTx_Configuration(USART_TypeDef* USARTx,UARTParameterConfiguration UARTParameter);
extern void USARTx_Mode_Init(USART_TypeDef* USARTx,UARTParameterConfiguration UARTParameter);
//UARTx_IRQn:UART1_IRQn、UART2_IRQn、UART3_IRQn、UART4_IRQn、UART5_IRQn
extern void NVIC_Usartx_Configuration(USART_TypeDef* USARTx,uint8_t UARTx_IRQn,uint8_t NVIC_Priority,uint8_t NVIC_SubPriority ) ;
extern unsigned char UARTx_Send_Data(USART_TypeDef* USARTx,unsigned char *Data,unsigned int Lenth);




