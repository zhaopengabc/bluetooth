#include "wdg.h"
#include "includes.h"
void IWDG_Init(unsigned char prer, unsigned short int rlr)	//���Ƶ��Ϊ64,����ֵΪ625,���ʱ��Ϊ1s
{
		printf("���Ź�������");
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);			//ʹ�ܶԼĴ���IWDG_PR��IWDG_RLR��д����
    IWDG_SetPrescaler(prer);								//����IWDGԤ��Ƶֵ:����IWDGԤ��ƵֵΪ64
    IWDG_SetReload(rlr);									//����IWDG��װ��ֵ
    IWDG_ReloadCounter();									//����IWDG��װ�ؼĴ�����ֵ��װ��IWDG������
    IWDG_Enable();											//ʹ��IWDG
}
//ι�������Ź�
void IWDG_Feed(void)
{
    IWDG_ReloadCounter();	//���ؼ���ֵ
}


