#ifndef __UART4_H
#define __UART4_H	 
#include "stdio.h"	
#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����2��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/3/29
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
extern u16 mark_status;//��λ��ǩ�ı�־λ����λ���㡣��Ϊ0��ʱ�򣬱�ʾ��λ������ mark_tatus ��ǰ��ֵΪ��0100 0011  ��˵��8��6��5��4��3�ű�ǩ��������λ��
extern u16 mark_status_last;//�ϴ���λ��ǩ�ı�־λ��
extern unsigned int departure_time; 			//�궨����λʱ�䡣
extern unsigned int judge_fruit_time;//��������е����ݡ��ж��Ƿ�����λ����ֹÿ�ζ��������Աȡ��˷ѵ�Ƭ����Դ
extern unsigned int bluetooh_scan_heart_time; //������վѲ��ʱ��
/***************************���ڽ�϶�ʱ����������***************************/
extern unsigned char UART4_RX_FLAG;       	//�ñ�־λ�����߶�ʱ����ʼ�����ˡ�
extern unsigned char UART4_RX_TIME;       	//�����ַ���ʱ��ʱ����
extern unsigned int  UART4_RX_NO;         	//�����ַ����ַ���������
extern unsigned char UART4_GROUP;   				//�����ջ����������	
extern unsigned char UART4_RX_BUF[10][100];//���ջ����ά���顣
extern unsigned char UART4_RX_BUF_middle[100]; 	//���ջ����ά���顣
extern unsigned char UART4_finger_point;   //������Ϣѭ����ָ��ָ���λ�á�
/***************************************************************************/
//extern u8 dispose_flag;//���ڴ��������ڹ�����
//����봮���жϽ��գ��벻Ҫע�����º궨��
void UART4_Init(u32 bound);
void test_URAT4(void);
void u4_printf(char* fmt,...);//����2��ӡ���� 
//����2,����ַ�������
void UART4_SEND(unsigned char* UART4_TX_BUF,unsigned int size); 
void UART4_RX_SCAN(void);//��ʱ����ʱɨ�贮�ڱ�־λ�ı仯����ʱ�Ľ��д���
void UART4_COMAND_SCAN(void);//ˢ�¶�ȡ����1���ݡ��鿴�Ƿ������������Ҫ���� 
void UART4_SEND(unsigned char* UART4_TX_BUF,unsigned int size); // USART1_TX_BUF����Ҫ��������顣size�����������ĳ��ȡ�
void Bluetooh_SCAN(void);//ˢ��Ѳ�������������ݡ�
void dislocation_test(void);//�̰����󣬷���һ֡��λ�������������еĽڵ���Ϣ��
void UART4_Data_analyze(unsigned char *UART4_RXD,unsigned char lenth);//����4�������ݽ���.UART4_RXD���������飬lenth�����ݳ��ȡ�
void get_all_status(void);
#endif











