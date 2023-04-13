#ifndef __WDG_H
#define __WDG_H
#include "stm32f10x_iwdg.h"

void IWDG_Init(unsigned char prer, unsigned short int rlr);
void IWDG_Feed(void);

#endif

/***********************end of file**************************/
