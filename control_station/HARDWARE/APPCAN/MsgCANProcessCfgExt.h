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

/***********************************************************************************************
* ALL the definition of configuration can be used and revised by user                          *
************************************************************************************************/
/* macro definition  */ 

#define     CAN1_GPIO_RCC          (RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO) 
#define     CAN1_RCC               RCC_APB1Periph_CAN1
#define     CAN1_GPIO              GPIOA
#define     CAN1_GPIO_TX           GPIO_Pin_12
#define     CAN1_GPIO_RX           GPIO_Pin_11

#define     CAN2_GPIO_RCC          (RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO) 
#define     CAN2_RCC               RCC_APB1Periph_CAN2
#define     CAN2_GPIO              GPIOB
#define     CAN2_GPIO_TX           GPIO_Pin_6
#define     CAN2_GPIO_RX           GPIO_Pin_5
  /* end of header file */
