#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "stmflash.h"
extern unsigned char TIM2_IRQ_flag;
extern unsigned int Send_Overtime;//���ػ��߷������������ݳ�ʱʱ����
extern unsigned int Bluetooh_Package_compare;//�������ݰ����бȽϵ���ʱ�ݶ�5��Ƚ�һ�Ρ�
extern unsigned int Bluetooh_scan_time;//������վ��ʱɨ��ʱ�䣬��0��ʱ�����ɨ�裬ɨ��������¸�ֵ��
extern unsigned int Bluetooh_heartbeat_time;//������վ�ϴ�����վ����������5�����Ϸ�һ�Ρ�
extern unsigned int Bluetooh_time[Bluetooh_Max_NO];//���������8��������ǩ.���е���ʱ��ʱ���������鶼���е���ʱ��	
extern unsigned int send_time;//���в�ѯ�Ƿ���Ҫ����������������
extern unsigned int GM3_LINKA_TIME;//GPRSʧ��ʱ�䵹��ʱ�����ʧ��ʱ�䳬��һ��ʱ�䣬��������
extern unsigned int KEY0_DN_S_effective_time;//�̰�������Чʱ�䡣
extern unsigned int KEY0_DN_L_effective_time;//����������Чʱ�䡣
void TIM2_Int_Init(u16 arr,u16 psc);
void timer_flag_int(void);//ϵͳ�ڱ�־λ�����Լ�������
#endif
