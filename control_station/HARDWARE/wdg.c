#include "wdg.h"
#include "includes.h"
void IWDG_Init(unsigned char prer, unsigned short int rlr)	//与分频数为64,重载值为625,溢出时间为1s
{
		printf("看门狗启动！");
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);			//使能对寄存器IWDG_PR和IWDG_RLR的写操作
    IWDG_SetPrescaler(prer);								//设置IWDG预分频值:设置IWDG预分频值为64
    IWDG_SetReload(rlr);									//设置IWDG重装载值
    IWDG_ReloadCounter();									//按照IWDG重装载寄存器的值重装载IWDG计数器
    IWDG_Enable();											//使能IWDG
}
//喂独立看门狗
void IWDG_Feed(void)
{
    IWDG_ReloadCounter();	//重载计数值
}


