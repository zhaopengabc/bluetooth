#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
extern unsigned char receive_once;//������λ��վһ���������������豸������
/***************************���ڽ�϶�ʱ����������***************************/
extern unsigned char USART1_RX_FLAG;       	//�ñ�־λ�����߶�ʱ����ʼ�����ˡ�
extern unsigned char USART1_RX_TIME;       	//�����ַ���ʱ��ʱ����
extern unsigned int  USART1_RX_NO;         	//�����ַ����ַ���������
extern unsigned char USART1_GROUP;   				//�����ջ����������	
extern unsigned char USART1_RX_BUF[10][100];//���ջ����ά���顣
extern unsigned char USART1_finger_point;   //������Ϣѭ����ָ��ָ���λ�á�
/***************************************************************************/
//����봮���жϽ��գ��벻Ҫע�����º궨��
void USART1_Init(u32 bound);//���ڳ�ʼ��
void USART1_RX_SCAN(void);//��ʱ����ʱɨ�贮�ڱ�־λ�ı仯����ʱ�Ľ��д���
void USART1_COMAND_SCAN(void);//ˢ�¶�ȡ����1���ݡ��鿴�Ƿ������������Ҫ���� 
void USART1_SEND(unsigned char* USART_TX_BUF,unsigned int size); // USART1_TX_BUF����Ҫ��������顣size�����������ĳ��ȡ� 
void USART1_Data_analyze(unsigned char *USART1_RXD,unsigned char lenth);//����1�������ݽ���.USART1_RXD���������飬lenth�����ݳ��ȡ�
#endif


