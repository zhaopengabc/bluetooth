///**
//  ******************************************************************************
//  * @file    main.h
//  * $Author: wdluo $
//  * $Revision: 17 $
//  * $Date:: 2012-07-06 11:16:48 +0800 #$
//  * @brief   主函数包含的头文件.
//  ******************************************************************************
//  * @attention
//  *
//  *<h3><center>&copy; Copyright 2009-2012, ViewTool</center>
//  *<center><a href="http:\\www.viewtool.com">http://www.viewtool.com</a></center>
//  *<center>All Rights Reserved</center></h3>
//  * 
//  ******************************************************************************
//  */
///* Define to prevent recursive inclusion -------------------------------------*/
//#ifndef __SDcontrol_H
//#define __SDcontrol_H
///* Includes ------------------------------------------------------------------*/
//#include "stm32f10x_conf.h"
////#include "usart.h"
//#include "ff.h"
//#include "SPI_MSD0_Driver.h"
//#include <string.h>

///* Exported Functions --------------------------------------------------------*/



//extern u8 FBWBflag;
//extern u8 filesysint(void);

//extern void filesysInfRed(void);
//extern u16 FilesysRedIn(void);
//#endif/* __MAIN_H */

///*********************************END OF FILE**********************************/







/**
  ******************************************************************************
  * @file    main.h
  * $Author: wdluo $
  * $Revision: 17 $
  * $Date:: 2012-07-06 11:16:48 +0800 #$
  * @brief   主函数包含的头文件.
  ******************************************************************************
  * @attention
  *
  *<h3><center>&copy; Copyright 2009-2012, ViewTool</center>
  *<center><a href="http:\\www.viewtool.com">http://www.viewtool.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SDcontrol_H
#define __SDcontrol_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_conf.h"
//#include "usart.h"
#include "ff.h"
#include "SPI_MSD0_Driver.h"
#include <string.h>

/* Exported Functions --------------------------------------------------------*/



extern u8 FBWBflag;
extern u8 filesysint(void);

extern void SDdata_Init(void);//初始化SD卡的读取；
extern void filesysInfRed(void);
extern u16 FilesysRedIn(void);
extern void SD_DATAIN(void);
extern void SoftReset(void);
extern void ItemAnalyse(unsigned char *data);
	
extern float FBWBReadStatus; //界面获取进度；
extern int FBWBReadNbr; //界面获取当前读取的条数；
extern int FBWBReadFail; //界面获取当前读取失败的条数；
extern u8 SDDataInFlagt;
#endif/* __MAIN_H */

/*********************************END OF FILE**********************************/

