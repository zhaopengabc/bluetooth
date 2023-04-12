#ifndef __USART2_H
#define __USART2_H	
#include "stdio.h"	
#include "sys.h"  
//extern unsigned int send_lenth;//������Ԫ�صĸ�����
extern unsigned char send_message[100];//�����������顣
extern unsigned char SEND_once;//�ж��ط�������
//�����������ڴ�С
extern unsigned int heartbeat_time;//����ʱ�䶨��1��������һ�Ρ�
extern unsigned int timeout;//��ʱ���ж���5ms����һ�Ρ�����2000�Σ���Ϊ10���ӡ�������������һ֡���ݵĳ�ʱʱ�䡣����������ʱ�䣬����Ϊ����ʱ��̫����
//////////////////////////////////////////////////////////////////////////////////
extern unsigned int SEND_DATA_to_server_time;//���������������ݵ���ʱ������������ʱ��û�з��ͳɹ��������·��͡�
extern unsigned char SEND_DATA_to_server_GROUP;//������Ҫ�����������������������
extern unsigned char SEND_DATA_to_server_ok;//���ͳɹ�Ϊ0������ʧ��Ϊ1.

/***************************���ڽ�϶�ʱ����������***************************/
extern unsigned char USART2_RX_FLAG;       	//�ñ�־λ�����߶�ʱ����ʼ�����ˡ�
extern unsigned char USART2_RX_TIME;       	//�����ַ���ʱ��ʱ����
extern unsigned int  USART2_RX_NO;         	//�����ַ����ַ���������
extern unsigned char USART2_GROUP;   				//�����ջ����������	
extern unsigned char USART2_RX_BUF[10][100];//���ջ����ά���顣
extern unsigned char USART2_RX_BUF_middle[100]; 	//���ջ����ά���顣
extern unsigned char USART2_finger_point;   //������Ϣѭ����ָ��ָ���λ�á�
/***************************************************************************/
extern unsigned char finger_point;//������Ϣѭ����ָ��ָ���λ�á�
extern unsigned char send_data[20][80];//����20�����ݣ������ط�ʱ��ջ�á������������λ��վ�ϴ����8����ǩ����һ������ֽ�Ϊ400�ֽڡ�����Ԥ������410���ֽڡ�
extern unsigned short  last_serial_number;//��һ�ε�ҵ����ˮ��
extern unsigned short  serial_number_counter;//ҵ����ˮ�š�	


//extern u8 dispose_flag;//���ڴ��������ڹ�����
//����봮���жϽ��գ��벻Ҫע�����º궨��
void USART2_Init(u32 bound);
void u2_printf(char* fmt,...);//����2��ӡ���� 
//����2,����ַ�������
void USART2_SEND(unsigned char* USART2_TX_BUF,unsigned int size);
void USART2_RX_SCAN(void);//��ʱ����ʱɨ�贮�ڱ�־λ�ı仯����ʱ�Ľ��д���
void USART2_COMAND_SCAN(void);//ˢ�¶�ȡ����1���ݡ��鿴�Ƿ������������Ҫ���� 
void Judge_GM3_send_data(void);//�ж������Ƿ���Ҫͨ��GM3�ϴ������������ݡ�
void Router_send_data_to_server(unsigned char *message);//���ط������ݸ�������
void package_send_data(unsigned char *data);//	data���������顣�������ж��������ݵ��㷨����������Ԫ�صĸ�����1Ȼ�����6������ĵ�һ��Ԫ����������Ԫ�ظ�����size����������Ԫ�صĸ���
void USART2_Data_analyze(unsigned char *USART2_RXD,unsigned char lenth);//����2�������ݽ���.USART2_RXD���������飬lenth�����ݳ��ȡ�
#endif













