#ifndef __USART3_H
#define __USART3_H	
#include "stdio.h"	
#include "sys.h"  						  
/***************************���ڽ�϶�ʱ����������***************************/   
extern unsigned char USART3_RX_FLAG;       	//�ñ�־λ�����߶�ʱ����ʼ�����ˡ�
extern unsigned char USART3_RX_TIME;       	//�����ַ���ʱ��ʱ����
extern unsigned int  USART3_RX_NO;         	//�����ַ����ַ���������
extern unsigned char USART3_GROUP;   				//�����ջ����������	
extern unsigned char USART3_RX_BUF[10][100];//���ջ����ά���顣
extern unsigned char USART3_RX_BUF_middle[100]; 	//���ջ����ά���顣
extern unsigned char USART3_finger_point;   //������Ϣѭ����ָ��ָ���λ�á�
/***************************************************************************/
extern unsigned char send_success_flag;//�����ػ��߷������������ݳɹ���־λ���ɹ�������1��ʧ�ܣ�����0��
//extern u8 dispose_flag;//���ڴ��������ڹ�����
//����봮���жϽ��գ��벻Ҫע�����º궨��
void USART3_Init(u32 bound);
void u3_printf(char* fmt,...);//����2��ӡ���� 
//����2,����ַ�������
void USART3_SEND(unsigned char* USART3_TX_BUF,unsigned int size); 
void USART3_RX_SCAN(void);//��ʱ����ʱɨ�贮�ڱ�־λ�ı仯����ʱ�Ľ��д���
void USART3_COMAND_SCAN(void);//ˢ�¶�ȡ����1���ݡ��鿴�Ƿ������������Ҫ���� 
unsigned char Wireless_Send_Data(unsigned char *data,unsigned char size);//data�Ƿ��͵����顣size�����ݳ��ȡ�
void LoRa_433_Pack_DATA(unsigned char *data);//Package_DATA[0]��������Package_DATA��Ԫ�صĸ���+1.	�������ж��������ݵ��㷨����������Ԫ�صĸ�������6��
void USART3_Data_analyze(unsigned char *USART3_RXD,unsigned char lenth);//����3�������ݽ���.USART3_RXD���������飬lenth�����ݳ��ȡ�
#endif



























