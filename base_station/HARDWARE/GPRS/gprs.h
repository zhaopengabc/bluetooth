#ifndef GPRS_H
#define GPRS_H
#include "stdio.h"	
#include "sys.h"  

//#define GM3_LINKA 							GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)//Socket A ״̬�� �ߵ�ƽΪ�����ӣ� �͵�ƽΪδ����
#define READ_GPRS_POWER_STATUS 	GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)//GM3��WORK���š�ģ������������ ��ƽһ��仯һ�Σ� �ߵ͵�ƽ����任�� ָʾ�Ƴ���˸״̬��
#define GPRS_MOS_POWER_ENABLE   GPIO_SetBits(GPIOB,GPIO_Pin_12)		//GPRSģ��MOS�ܿ��Ƶ�Դ��ͨ
#define GPRS_MOS_POWER_DISABLE  GPIO_ResetBits(GPIOB,GPIO_Pin_12) //GPRSģ��MOS�ܿ��Ƶ�Դ�Ͽ�

#define GPRS_SOFT_POWER_ENABLE  GPIO_SetBits(GPIOC,GPIO_Pin_6)		//GPRSģ��������Ƶ�Դ��ͨ
#define GPRS_SOFT_POWER_DISABLE GPIO_ResetBits(GPIOC,GPIO_Pin_6) //GPRSģ��������Ƶ�Դ�Ͽ�

#define GPRS_REST_ENABLE     GPIO_SetBits(GPIOB,GPIO_Pin_15)	  //GPRS��λ���͵�ƽ��λ��//��Ϊ��ͨѶ���������˵�ѹ��������������Ǹߵ�ƽ�ˡ�
#define GPRS_REST_DISABLE    GPIO_ResetBits(GPIOB,GPIO_Pin_15)  //GPRS����λ���ߵ�ƽ����λ��

extern unsigned int GM3_LINKA_TIME_LOSE;//GPRS����ʱ�䡣����������ʱ�䣬����Ҫ����GPRSģ���ˡ�Ӳ�����LINK���ţ���Ϊ��Ҫ�޸Ĳ��ԣ���������ȫ�ֱ������൱�ں궨�塣


void GPRS_Init(void);
unsigned char gm3_send_cmd(unsigned char *cmd,unsigned char *ack,u16 waittime);
unsigned char *gm3_check_cmd(unsigned char *str,unsigned char *receive_data);//�ж� receive_data ����������Ƿ��� str ����ַ�����
extern unsigned char sendata_flag;
extern unsigned int GM3_LINKA_TIME;//GPRS����ʱ�䡣����������ʱ�䣬����Ҫ����GPRSģ���ˡ�
void GPRS_RESTAR(void);//GPRS������
#endif

