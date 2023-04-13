#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 

/*����ķ�ʽ��ͨ��ֱ�Ӳ����⺯����ʽ��ȡIO*/
////��ȡIO�ڵ�״̬
//#define KEY_READ()		((GPIO_ReadInputData(GPIOC)&0x1F00)>>8)
////����IO������ 1000p
//#define KEY_ALL_HI()	{PDout(8)=1;PDout(9)=1;PDout(10)=1;PDout(11)=1;PDout(12)=1;}
//#define KEY_ALL_LO()	{PDout(8)=0;PDout(9)=0;PDout(10)=0;PDout(11)=0;PDout(12)=0;}
//#define KEY_A0_LO() 	{PDout(8)=0;}
//#define KEY_A1_LO() 	{PDout(9)=0;}
//#define KEY_A2_LO() 	{PDout(10)=0;}
//#define KEY_A3_LO() 	{PDout(11)=0;}
//#define KEY_A4_LO() 	{PDout(12)=0;}

//��ȡIO�ڵ�״̬ 1010a   1101
//#define KEY_READ()		((GPIO_ReadInputData(GPIOC)&0x0005)||(GPIO_ReadInputData(GPIOB)&0x0008))
//����IO������ 1010a
#define KEY_ALL_HI()	{  GPIO_SetBits(GPIOC,GPIO_Pin_0);  GPIO_SetBits(GPIOC,GPIO_Pin_2);  GPIO_SetBits(GPIOB,GPIO_Pin_3);}//ROW
#define KEY_ALL_LO()	{GPIO_ResetBits(GPIOC,GPIO_Pin_0);GPIO_ResetBits(GPIOC,GPIO_Pin_2);GPIO_ResetBits(GPIOB,GPIO_Pin_3);}//ROW
#define KEY_A0_LO() 	{GPIO_ResetBits(GPIOC,GPIO_Pin_0);}//ROW
#define KEY_A1_LO() 	{GPIO_ResetBits(GPIOC,GPIO_Pin_2);}//ROW
#define KEY_A2_LO() 	{GPIO_ResetBits(GPIOB,GPIO_Pin_3);}//ROW


extern volatile unsigned char EXTI0_FLAG;
extern volatile unsigned char EXTI2_FLAG;
extern volatile unsigned char EXTI3_FLAG;


void KEY_Init(void);	//IO��ʼ��
unsigned int KEY_SCAN(void);//����ɨ�躯������

#endif
