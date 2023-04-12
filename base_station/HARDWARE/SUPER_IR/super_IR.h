#ifndef __SUPER_IR_H
#define __SUPER_IR_H
#include "sys.h"
/**********************�궨��****************************************/
#define MAX_TB_TIME 500          //����һ������ʱ�䣬����Ϊ50*���ڣ���û�������ص�ʱ�򣬴������

/**********************ȫ�ֱ���**************************************/
extern unsigned long int RF_address;     // ȫ�ֱ��������ڴ洢
extern unsigned char Rf_ok_flag;         // ȫ�ֱ��������յ�һ��������ң���������1,֪ͨ���������Խ�����
extern unsigned char RF_data;            // ȫ�ֱ��������ڴ������

/**********************��������***************************************/
extern void RF_GPIO_Init(void);  				 // 433M IO��ʼ��
extern void DecodingEv1527(void);				 // 433M ��ѯɨ��

#endif
