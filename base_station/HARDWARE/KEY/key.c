//#include "key.h"
//#include "timer.h"
//#include "buzzer.h"
//#include "led.h"
//#include "delay.h"
////������ʼ������ 
////PC5 ���ó�����
//unsigned char KEY_flag=0;//�� KEY_flag=1��ʱ�򣬱�ʾ�̰��� KEY_flag=2��ʱ�򣬱�ʾ������
//unsigned char KEY0_DN_L=0;//����ȫ�ֱ�����������������
//unsigned char KEY0_DN_S=0;//����ȫ�ֱ������������̰���
//unsigned int  key_dn_long=5000;//��ʾ����
//unsigned int  key_dn_short=1000;//��ʾ�̰�
//unsigned int  key_dn_middle=10000;
//unsigned int  KEY0_Dn_TimeCnt=0; //����0ȥ������ʱ������,����Ϊ��ֵ
//void KEY_Init(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹPORTAʱ��	
//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;//PA1
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //���ó�����
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA1
//}
//void KEY_SCAN(void)//����ɨ��
//{//ʶ��̰��ͳ�����Ҫ�󣺶̰���ֻҪ�����֣��Ͳ���̰�������Ҫ�з������������ѣ��ɿ��ֺ�ֱ�ӽ��붨ʱ���鿴�̰���ʱ���Ƿ��ڷ�Χ�ڣ�����ڣ����϶��̰���Ч���ñ�־λ��
////�����Ͷ̰��ǻ���ģ�Ҳ����˵������ͬһʱ��ֻ�ܴ���һ����
//	/****************************************************************************/	
////static unsigned char KEY0_DN_L=0;//����ȫ�ֱ�����������������
////static unsigned char KEY0_DN_S=0;//����ȫ�ֱ������������̰���
////static unsigned int  KEY0_Dn_TimeCnt=0; //����0ȥ������ʱ������,����Ϊ��ֵ
////	//����0���жϰ����Ƕ̰����ǳ�����
////	if(KEY0==Bit_RESET)//IO�Ǹߵ�ƽ��˵������û�б����£���ʱҪ��ʱ����һЩ��־λ
////	{
////		if(KEY0_Dn_TimeCnt==0)
////		{
////			KEY0_Dn_TimeCnt=0;//����ȥ������ʱ���������㡣
////		}
////		else
////		{
////			KEY0_Dn_TimeCnt--;
////		}
////		if((KEY0_Dn_TimeCnt>=key_dn_short)&&(KEY0_Dn_TimeCnt<=key_dn_long))
////		{
////			KEY0_Dn_TimeCnt=0;
////			KEY0_DN_S=0;
////			KEY0_DN_L=0;
////			KEY_flag=1;//�������̰�
////			Buzzer_flag=4;	
////		}
////		if(KEY0_Dn_TimeCnt>=key_dn_long)
////		{
////			KEY0_Dn_TimeCnt=0;
////			KEY0_DN_S=0;
////			KEY0_DN_L=0;
////			KEY_flag=2;//����������	
////			Buzzer_flag=4;				
////		}		
////	}
////	else//�а������£����ǵ�һ�α�����
////	{
////		if(KEY0_Dn_TimeCnt<60000)
////		{
////			KEY0_Dn_TimeCnt++;  //��ʱ������	
////		}
////	  if((KEY0_Dn_TimeCnt>=key_dn_short)&&(KEY0_DN_S==0))//��ʾ�̰�
////	  {
////		 KEY0_DN_S=1;
////		 Buzzer_flag=2;	
////	  }
////	  if((KEY0_Dn_TimeCnt>=key_dn_long)&&(KEY0_DN_L==0))//������������ڳ��������������ʾ������
////	  {
////		 KEY0_DN_L=1;//�����������ˡ�
////		 Buzzer_flag=4;		
////	  }
////	}
///****************************************************************************/
////  if(RE11_SET==0)//IO�Ǹߵ�ƽ��˵������û�б����£���ʱҪ��ʱ����һЩ��־λ
////  {
////   RE11_SET_TimeCnt=0;//����ȥ������ʱ���������㣬���зǳ��������ʵս�����������ġ�
////	 RE11_SET_L=0;
////	 RE11_SET_S=0;
////  }
////  else//�а������£����ǵ�һ�α�����
////  {
////     RE11_SET_TimeCnt++;  //��ʱ������ 
////     if( (RE11_SET_TimeCnt>=RE11_dn_long)&&(RE11_SET_L==0) )//��ʾ����
////     {	
////			RE11_SET_L=1;//������ת״̬	
////KEY_flag=2;//�����������ˡ�			 
////			Buzzer_flag=3;//������������			 
////     }
////		 else if(  (RE11_SET_TimeCnt>=RE11_dn_short)&&(RE11_SET_S==0) )//��ʾ�̰�
////		 {
////			RE11_SET_S=1;//�̰���ת״̬	
////KEY_flag=1;
////			Buzzer_flag=3;//�����̰�����			 

////		 }
////  }		
//	
///****************************************************************************/	
//	if(KEY0==0)//IO�Ǹߵ�ƽ��˵������û�б����£���ʱҪ��ʱ����һЩ��־λ
//	{
//		if(KEY0_Dn_TimeCnt==0)
//		{
//			KEY0_Dn_TimeCnt=0;//����ȥ������ʱ���������㡣
//		}
//		else
//		{
//			KEY0_Dn_TimeCnt--;
//		}
//	}
//	else//�а������£����ǵ�һ�α�����
//	{
//	  KEY0_Dn_TimeCnt++;  //��ʱ������	
//		
//	  if((KEY0_Dn_TimeCnt>=key_dn_long)&&(KEY0_Dn_TimeCnt<key_dn_middle)&&(KEY_flag==0))//��ʾ�̰�&&(KEY0_DN_S==0)
//	  {	
//		 KEY0_DN_S=1;//�������̰��ˡ�
//		 KEY0_DN_L=0;
//			KEY_flag=2;			
//		 Buzzer_flag=4;	
//	  }
//	  if( (KEY0_Dn_TimeCnt>=key_dn_short)&&(KEY_flag==0) )//������������ڳ��������������ʾ������
//	  {				
//		 KEY0_DN_S=0;//�������̰�״̬���㡣�����Ͷ̰�ֻ�ܴ���һ����ȡ������
//		 KEY0_DN_L=1;//�����������ˡ�
//			KEY_flag=1;
//		 Buzzer_flag=1;		
//	  }
//	}	
//}



#include "key.h"
#include "timer.h"
#include "buzzer.h"
#include "led.h"
#include "delay.h"
//������ʼ������ 
//PC5 ���ó�����
unsigned char KEY_flag=0;//�� KEY_flag=1��ʱ�򣬱�ʾ�̰��� KEY_flag=2��ʱ�򣬱�ʾ������
unsigned int  key_dn_long=1000;//��ʾ����
unsigned int  key_dn_short=100;//��ʾ�̰�

void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹPORTAʱ��	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;//PA1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó�����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA1
}
void KEY_SCAN(void)//����ɨ��
{//ʶ��̰��ͳ�����Ҫ�󣺶̰���ֻҪ�����֣��Ͳ���̰�������Ҫ�з������������ѣ��ɿ��ֺ�ֱ�ӽ��붨ʱ���鿴�̰���ʱ���Ƿ��ڷ�Χ�ڣ�����ڣ����϶��̰���Ч���ñ�־λ��
//�����Ͷ̰��ǻ���ģ�Ҳ����˵������ͬһʱ��ֻ�ܴ���һ����
	/****************************************************************************/	
static unsigned char KEY0_DN_L=0;//����ȫ�ֱ�����������������
static unsigned char KEY0_DN_S=0;//����ȫ�ֱ������������̰���
static unsigned int  KEY0_Dn_TimeCnt=0; //����0ȥ������ʱ������,����Ϊ��ֵ
	//����0���жϰ����Ƕ̰����ǳ�����
	if(KEY0==1)//IO�Ǹߵ�ƽ��˵������û�б����£���ʱҪ��ʱ����һЩ��־λ
	{
		if(KEY0_Dn_TimeCnt==0)
		{
			KEY0_Dn_TimeCnt=0;//����ȥ������ʱ���������㡣
		}
		else
		{
			KEY0_Dn_TimeCnt--;
		}
		if((KEY0_Dn_TimeCnt>=key_dn_short)&&(KEY0_Dn_TimeCnt<=key_dn_long))
		{
			KEY0_Dn_TimeCnt=0;
			KEY0_DN_S=0;
			KEY0_DN_L=0;
			KEY_flag=1;//�������̰�
//			Buzzer_flag=4;	
		}
		if(KEY0_Dn_TimeCnt>=key_dn_long)
		{
			KEY0_Dn_TimeCnt=0;
			KEY0_DN_S=0;
			KEY0_DN_L=0;
			KEY_flag=2;//����������	
//			Buzzer_flag=4;				
		}		
	}
	else//�а������£����ǵ�һ�α�����
	{
		if(KEY0_Dn_TimeCnt<60000)
		{
			KEY0_Dn_TimeCnt++;  //��ʱ������	
		}
	  if((KEY0_Dn_TimeCnt>=key_dn_short)&&(KEY0_DN_S==0))//��ʾ�̰�
	  {
		 KEY0_DN_S=1;
		 Buzzer_flag=2;	
	  }
	  if((KEY0_Dn_TimeCnt>=key_dn_long)&&(KEY0_DN_L==0))//������������ڳ��������������ʾ������
	  {
		 KEY0_DN_L=1;//�����������ˡ�
		 Buzzer_flag=4;		
	  }
	}
/****************************************************************************/
}



