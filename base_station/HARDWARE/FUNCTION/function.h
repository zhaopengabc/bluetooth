#ifndef __FUNCTION_H
#define __FUNCTION_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
extern unsigned short judge_data_u16[10]; 
extern unsigned char sumcheck(unsigned char *data,unsigned char star,unsigned char end);//����У���	
void DATA_change(unsigned char *data,unsigned int size,unsigned char *target_data);//data��׼��ת�������顣size��ת��������Ԫ�ص�������������ż��,target_data������ת�����ŵ����顣 
void handle_arrays_u8(unsigned char *data,unsigned char size,unsigned char *printf_data);//�������飬DATA��Ҫ��������飬SIZE������ĳ��ȡ�//���罫0x25�ֿ�����0x32��0x35.��0xAB�ֿ�����0x40,0x41. 
//DATA����������飬SIZE��������Ԫ�ص�������Printf_data������������ڲ��Դ�ӡ�����顣
void handle_arrays_u16(unsigned int *DATA,unsigned int SIZE);//�������飬DATA��Ҫ��������飬SIZE������ĳ��ȡ����罫0x2542�ֿ�����0x32,0x35,0x34,0x32.��0xABCD�ֿ�����0x40,0x41,0x42x0x43. 
void DATA_change_u8_to_u16(unsigned char* data_u8,unsigned int size,unsigned short* data_u16); //������8λ����ת����һ��16λ���ݡ� 
void DATA_change_u16_to_u8(unsigned short *data_u16,unsigned int size,unsigned char *data_u8); //��һ��16λ����ת��������8λ���ݡ� 
void Printf_device_type(unsigned char type);//���Դ�ӡ�豸���͡����ݹ��ꡣ
void Judge_ID(unsigned short *In_put_ID);//�����жϺϷ��ԡ�Ȼ�����ݴ�С�������С�
extern unsigned int Save_ROUTER_ID(unsigned char *ROUTER_ID);//��������ID.�������ɹ�������1���������ʧ�ܣ��򷵻�0��
void Judge_ID1(unsigned int In_put_ID);//�����жϺϷ��ԡ�Ȼ�����ݴ�С�������С�
void Delete_NODE_ID(unsigned char *In_put_ID);//ɾ���豸ID.
extern unsigned int Save_NODE_ID(unsigned char *In_put_ID);//�����豸ID.�������ɹ������ر���ĵ�ַ�š��������ʧ�ܣ��򷵻�0xFF��
void Delete_ALL_NODE_ID(void);//ɾ������ע����豸ID
extern unsigned char  Sting_Compare(unsigned char  *Sting1,unsigned char  *Sting2,unsigned char Len);//�ַ����Ƚ�
extern unsigned int str_int(unsigned char *str);//������ת����int�����ݡ���������strΪ��0x31,0x32,0x33,0x34,0x35,0x36��ת����ɺ�ͱ�ɣ�123456��
#endif
