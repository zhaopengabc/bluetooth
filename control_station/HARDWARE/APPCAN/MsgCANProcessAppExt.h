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
//#include "HRSTDDEF.h"
/*declaration range definition*/

/***********************************************************************************************
* ALL definition and declaration can be used ,but can not revised outside this driver          *
************************************************************************************************/
/* macro  definition */
#include "stm32f10x.h"
/* data type definition */
typedef struct
{
unsigned char   CANTsjw;
unsigned char   CANTbs2;
unsigned char   CANTbs1;
unsigned short  CANBrp;
unsigned char   CANMode;
}CANParameterConfiguration;
/* variable definition */

/* function declaration */ 
extern CANParameterConfiguration  gCANSystemParameter;
extern void CAN_Mode_Init(CAN_TypeDef* CANx,CANParameterConfiguration CANParameter);
extern unsigned char CAN_Send_One_Frame(unsigned char *msg, unsigned char len,unsigned int EID,uint32_t CAN_Id);
extern void CAN1Init(void);
extern void CAN1Init_Parameter(CANParameterConfiguration CANParameter);
extern void CAN1_Mode_Init(CAN_TypeDef* CANx,CANParameterConfiguration CANParameter) ;
extern unsigned char CAN2_Send_One_Frame(unsigned char *msg, unsigned char len,unsigned int EID,uint32_t CAN_Id);
/* end of header file */
