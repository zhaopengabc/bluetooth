#include "buzzer.h" 
//////////////////////////////////////////////////////////////////////////////////   	 
unsigned int Buzzer_Open_time=0;
unsigned int Buzzer_Closed_time=0;
unsigned char Buzzer_Number=0;//��������������
unsigned char Buzzer_flag=0;//�������������ͱ�־λ
unsigned char	Buzzer_condition=0;//�������������л��Ǿ���
unsigned char	Buzzer_Doing=0;//�жϷ������Ƿ����ڹ��������û�������͸�������ֵ��

//��ʼ��PF8Ϊ�����		    
//BEEP IO��ʼ��
void Buzzer_Init(void)//Buzzer��ʼ��
{ 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 //BUZZER-->PB.6 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.6
 GPIO_SetBits(GPIOB,GPIO_Pin_6);						 //PB.6 �����
}

void Buzzer_SCAN()//������
{
  switch(Buzzer_flag)//�ж�Buzzer_flag���ж�Ӧ���������
	{
	 case 0://����������	 							 
			Buzzer_OFF;//����
			Buzzer_condition=0;//�����������컹��û��
			Buzzer_Doing=0;//���������ڹ���״̬
			Buzzer_flag=0;//ѡ�������־λ����
			Buzzer_Number=0;//���������
			Buzzer_Open_time=0;//��ʱ������
			Buzzer_Closed_time=0;//����ʱ����0
			break;	 
	 case 1://��������5��
	 		if(Buzzer_Doing==0)//�жϷ������Ƿ����ڹ��������û�������͸�������ֵ��
			{
			 Buzzer_Number=5;//��4��
			 Buzzer_Open_time=2;//��100����
			 Buzzer_Closed_time=2;//����100����
			 Buzzer_Doing=1;//���¸�ֵ�����ٽ����ѭ����
			}
			else
			{					
				if( (Buzzer_Number>0)&&(Buzzer_Doing==1) )//û�дﵽ��������
				{
					 if(Buzzer_condition)//��������죬�ͽ�������ʱ״̬1
					 {
					 
					  Buzzer_Open_time--;	
					  if(Buzzer_Open_time>0)
					  {
					   Buzzer_ON;//����					   
					  }
					  else
					  {
					   Buzzer_condition=!Buzzer_condition;//��ת״̬Ϊ����
					   Buzzer_Open_time=2;//���������¸�ֵ
					   Buzzer_Number--;//����������1
					  }				  
					 }
					 else//���û���죬�ͽ��벻����ʱ״̬0
					 {
					  Buzzer_Closed_time--;	
					  if(Buzzer_Closed_time>0)
					  {
					   Buzzer_OFF;//����					   
					  }
					  else
					  {
					   Buzzer_condition=!Buzzer_condition;//��ת״̬Ϊ����
					   Buzzer_Closed_time=2;//���������¸�ֵ
					  }
					 }
				}
				else //������ϣ���ȥ��־λ��
				{	
					 Buzzer_OFF;//����
					 Buzzer_condition=0;//�����������컹��û��
					 Buzzer_Doing=0;//���������ڹ���״̬
					 Buzzer_flag=0;//ѡ�������־λ����
					 Buzzer_Number=0;//���������
 					 Buzzer_Open_time=0;//��ʱ������
 					 Buzzer_Closed_time=0;//����ʱ������
				}
			}		 											
			break;
	 		 		
	 case 2://��������2��
	 		if(Buzzer_Doing==0)//�жϷ������Ƿ����ڹ��������û�������͸�������ֵ��
			{
			 Buzzer_Number=3;//��4��
			 Buzzer_Open_time=5;//��50����
			 Buzzer_Closed_time=5;//����50����
			 Buzzer_Doing=1;//���¸�ֵ�����ٽ����ѭ����
			}
			else
			{					
				if( (Buzzer_Number>0)&&(Buzzer_Doing==1) )//û�дﵽ��������
				{
					 if(Buzzer_condition)//��������죬�ͽ�������ʱ״̬1
					 {
					 
					  Buzzer_Open_time--;	
					  if(Buzzer_Open_time>0)
					  {
					   Buzzer_ON;//����					   
					  }
					  else
					  {
					   Buzzer_condition=!Buzzer_condition;//��ת״̬Ϊ����
					   Buzzer_Open_time=5;//���������¸�ֵ
					   Buzzer_Number--;//����������1
//					   SBUF = Buzzer_Number;
					  }				  
					 }
					 else//���û���죬�ͽ��벻����ʱ״̬0
					 {
					  Buzzer_Closed_time--;	
					  if(Buzzer_Closed_time>0)
					  {
					   Buzzer_OFF;//����					   
					  }
					  else
					  {
					   Buzzer_condition=!Buzzer_condition;//��ת״̬Ϊ����
					   Buzzer_Closed_time=5;//���������¸�ֵ
					  }
					 }
				}
				else //������ϣ���ȥ��־λ��
				{	
					 Buzzer_OFF;//����
					 Buzzer_condition=0;//�����������컹��û��
					 Buzzer_Doing=0;//���������ڹ���״̬
					 Buzzer_flag=0;//ѡ�������־λ����
					 Buzzer_Number=0;//���������
 					 Buzzer_Open_time=0;//��ʱ������
 					 Buzzer_Closed_time=0;//����ʱ������
				}
			}		 											
			break;
	 case 3://����������һ��
	 		if(Buzzer_Doing==0)//�жϷ������Ƿ����ڹ��������û�������͸�������ֵ��
			{
			 Buzzer_Number=5;//��4��
			 Buzzer_Open_time=2;//��50����
			 Buzzer_Closed_time=2;//����50����
			 Buzzer_Doing=1;//���¸�ֵ�����ٽ����ѭ����
			}
			else
			{					
				if( (Buzzer_Number>0)&&(Buzzer_Doing==1) )//û�дﵽ��������
				{
					 if(Buzzer_condition)//��������죬�ͽ�������ʱ״̬1
					 {
					 
					  Buzzer_Open_time--;	
					  if(Buzzer_Open_time>0)
					  {
					   Buzzer_ON;//����					   
					  }
					  else
					  {
					   Buzzer_condition=!Buzzer_condition;//��ת״̬Ϊ����
					   Buzzer_Open_time=1;//���������¸�ֵ
					   Buzzer_Number--;//����������1
//					   SBUF = Buzzer_Number;
					  }				  
					 }
					 else//���û���죬�ͽ��벻����ʱ״̬0
					 {
					  Buzzer_Closed_time--;	
					  if(Buzzer_Closed_time>0)
					  {
					   Buzzer_OFF;//����					   
					  }
					  else
					  {
					   Buzzer_condition=!Buzzer_condition;//��ת״̬Ϊ����
					   Buzzer_Closed_time=1;//���������¸�ֵ
					  }
					 }
				}
				else //������ϣ���ȥ��־λ��
				{	
					 Buzzer_OFF;//����
					 Buzzer_condition=0;//�����������컹��û��
					 Buzzer_Doing=0;//���������ڹ���״̬
					 Buzzer_flag=0;//ѡ�������־λ����
					 Buzzer_Number=0;//���������
 					 Buzzer_Open_time=0;//��ʱ������
 					 Buzzer_Closed_time=0;//����ʱ������
				}
			}			 											
			break;			
	 case 4://��������2��
	 		if(Buzzer_Doing==0)//�жϷ������Ƿ����ڹ��������û�������͸�������ֵ��
			{
			 Buzzer_Open_time=200;//��1000����
			 Buzzer_ON;//����									
			 Buzzer_Doing=1;//���¸�ֵ�����ٽ����ѭ����
			}
			else
			{		
				Buzzer_Open_time--;				
				if(Buzzer_Open_time==0)//������ϣ���ȥ��־λ��
				{
					Buzzer_OFF;//����
					Buzzer_condition=0;//�����������컹��û��	
					Buzzer_Doing=0;//���������ڹ���״̬
					Buzzer_flag=0;//ѡ�������־λ����
					Buzzer_Number=0;//���������
					Buzzer_Open_time=0;//��ʱ������
					Buzzer_Closed_time=0;//����ʱ������					
				}
				else 
				{	
					 Buzzer_ON;//����
				}
			}	 											
			break;			
	 case 5://����������һ��
	 		if(Buzzer_Doing==0)//�жϷ������Ƿ����ڹ��������û�������͸�������ֵ��
			{
			 Buzzer_Open_time=10;//��50����
			 Buzzer_ON;//����									
			 Buzzer_Doing=1;//���¸�ֵ�����ٽ����ѭ����
			}
			else
			{		
				Buzzer_Open_time--;				
				if(Buzzer_Open_time==0)//������ϣ���ȥ��־λ��
				{
					Buzzer_OFF;//����
					Buzzer_condition=0;//�����������컹��û��	
					Buzzer_Doing=0;//���������ڹ���״̬
					Buzzer_flag=0;//ѡ�������־λ����
					Buzzer_Number=0;//���������
					Buzzer_Open_time=0;//��ʱ������
					Buzzer_Closed_time=0;//����ʱ������					
				}
				else 
				{	
					 Buzzer_ON;//����
				}
			}		 											
			break;				
	 case 6://����������һ��
	 		if(Buzzer_Doing==0)//�жϷ������Ƿ����ڹ��������û�������͸�������ֵ��
			{
			 Buzzer_Open_time=5;//��50����
			 Buzzer_ON;//����									
			 Buzzer_Doing=1;//���¸�ֵ�����ٽ����ѭ����
			}
			else
			{		
				Buzzer_Open_time--;				
				if(Buzzer_Open_time==0)//������ϣ���ȥ��־λ��
				{
					Buzzer_OFF;//����
					Buzzer_condition=0;//�����������컹��û��	
					Buzzer_Doing=0;//���������ڹ���״̬
					Buzzer_flag=0;//ѡ�������־λ����
					Buzzer_Number=0;//���������
					Buzzer_Open_time=0;//��ʱ������
					Buzzer_Closed_time=0;//����ʱ������					
				}
				else 
				{	
					 Buzzer_ON;//����
				}
			}		 											
			break;				
	 case 7://����������һ��
	 		if(Buzzer_Doing==0)//�жϷ������Ƿ����ڹ��������û�������͸�������ֵ��
			{
			 Buzzer_Open_time=300;//��50����
			 Buzzer_ON;//����									
			 Buzzer_Doing=1;//���¸�ֵ�����ٽ����ѭ����
			}
			else
			{		
				Buzzer_Open_time--;				
				if(Buzzer_Open_time==0)//������ϣ���ȥ��־λ��
				{
					Buzzer_OFF;//����
					Buzzer_condition=0;//�����������컹��û��	
					Buzzer_Doing=0;//���������ڹ���״̬
					Buzzer_flag=0;//ѡ�������־λ����
					Buzzer_Number=0;//���������
					Buzzer_Open_time=0;//��ʱ������
					Buzzer_Closed_time=0;//����ʱ������					
				}
				else 
				{	
					 Buzzer_ON;//����
				}
			}		 											
			break;			
			
	 default:
	 		Buzzer_OFF;//����
			Buzzer_flag=0;
	 		break;
	}

}












