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
#include "stm32f10x.h"
/*declaration range definition*/

/***********************************************************************************************
* ALL definition and declaration can be used ,but can not revised outside this driver          *
************************************************************************************************/
/* macro  definition */
typedef struct
{
  SPI_TypeDef*    SPIx;
	uint32_t        SPI_GPIO_RCC;
	uint32_t        SPI_RCC;
	uint16_t        SPI_GPIO;
	GPIO_TypeDef*   SPI_GPIO_GROUP;
	uint16_t        SPI_BaudRatePrescaler_Set;
	uint16_t        SPI_Mode;
}SPI_CONFIGURATION;

typedef struct
{
	SPI_TypeDef*    SPIx;
	FunctionalState      SPI_DMAStatus;
	DMA_Channel_TypeDef* SPI_DMA_CHANNEL_TX;//
	DMA_Channel_TypeDef* SPI_DMA_CHANNEL_RX;//
	uint8_t  SPI_DMA_ChannelRX_IRQn;
	uint8_t  SPI_DMA_ChannelTX_IRQn;
	uint32_t SPI_cpar;
	uint32_t SPI_cRXmar;
	uint32_t SPI_cTXmar;
	uint16_t SPI_RXcndtr;
	uint16_t SPI_TXcndtr;	
}SPI_DMA_CONFIGURATION;


/* data type definition */
//SPI1 定义//
#define SPI1_GPIO_RCC                          RCC_APB2Periph_GPIOA
#define SPI1_RCC                               RCC_APB2Periph_SPI1 
#define SPI1_GPIO                              GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6|GPIO_Pin_7
#define SPI1_GPIO_GROUP                        GPIOA
#define SPI1_BaudRatePrescaler_Set             SPI_BaudRatePrescaler_32
#define SPI1_Mode                              SPI_Mode_Slave//

#define SPI1_DMA_Status_ENABLE                 ENABLE
#define SPI1_DMA_CHANNEL_TX                    DMA1_Channel3
#define SPI1_DMA_CHANNEL_RX                    DMA1_Channel2
#define SPI1_DMA_ChannelRX_IRQn                DMA1_Channel2_IRQn
#define SPI1_DMA_ChannelTX_IRQn                DMA1_Channel3_IRQn
#define SPI1_cpar                              (uint32_t)&SPI1->DR
//SPI2 定义//
#define SPI2_GPIO_RCC                          RCC_APB2Periph_GPIOB 
#define SPI2_RCC                               RCC_APB1Periph_SPI2 
#define SPI2_GPIO                              GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 
#define SPI2_GPIO_GROUP                        GPIOB
#define SPI2_BaudRatePrescaler_Set             SPI_BaudRatePrescaler_256
#define SPI2_Mode                              SPI_Mode_Master  //SPI_Mode_Master

#define SPI2_DMA_Status_ENABLE                 ENABLE
#define SPI2_DMA_CHANNEL_TX                    DMA1_Channel5
#define SPI2_DMA_CHANNEL_RX                    DMA1_Channel4
#define SPI2_DMA_ChannelRX_IRQn                DMA1_Channel4_IRQn
#define SPI2_DMA_ChannelTX_IRQn                DMA1_Channel5_IRQn
#define SPI2_cpar                              (uint32_t)&SPI2->DR
/* variable definition */
extern int znR;
extern int znT;
#define SPIxTxen 1
#define SPIxRxen 0
/* function declaration */
void SPI_ConfigarationInit(unsigned char SPIxState);
void TIM5_Init(u16 arr, u16 psc);
/* end of header file */
