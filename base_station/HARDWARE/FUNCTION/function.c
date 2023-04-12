#include "function.h"
#include "usart.h"
#include "stmflash.h"
#include "delay.h"
#include "string.h"
unsigned short judge_data_u16[10]={0x0000}; 
unsigned char All_ID[2048]={0xFF};
//////////////////////////////////////////////////////////////////////////////////	 
//�����ܡ�
//����У��͡�
//data�����飬star���������ʼλ�ã�end������Ľ���λ�á�
//���������� data��0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37��
//sumcheck(data,1,5);
//���Ǽ��� 0x31,0x32,0x33,0x34,0x35�ĺ͡�
//����ֵΪ 0xFF;
////////////////////////////////////////////////////////////////////////////////// 	
unsigned char sumcheck(unsigned char *data,unsigned char star,unsigned char end)//����У���.
{
	unsigned char sum_check=0;
	unsigned char i=0;	
	end=end-star+1;
	for(i=0;i<end;i++)
	{
		sum_check=sum_check+data[i+star];
	}		
	return sum_check;//����У���
}

//////////////////////////////////////////////////////////////////////////////////	 
//�����ܡ�
//��һ������ѹ����һ�롣
//ǰ������������е�Ԫ��ֻ���ǣ�0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46��
//��Ӧ��Ϊ��1��2��3��4��5��6��7��8��9,A,B,C,D,E,F��
//�����������30 31 42 32 31 37 41 31 31 31 43 46 30 30 30 30 30 30 30 31  ��
//�����꣬��������Ϊ��01 B2 17 A1 11 CF 00 00 00 01��
////////////////////////////////////////////////////////////////////////////////// 	
void DATA_change(unsigned char *data,unsigned int size,unsigned char *target_data)  
{//data��׼��ת�������顣size��ת��������Ԫ�ص�������������ż��,target_data������ת�����ŵ����顣
	unsigned char i=0;
	unsigned char j=0;	
	unsigned char data_middle[200]={0x00};	
	for(i=0;i<size;i++)//��ȡ����ID
	{
		switch(data[i])//
		{
		 case 0x30:		
				data_middle[i]=0x00;
				break;	 			
		 case 0x31:		
				data_middle[i]=0x01;		 
				break;	
		 case 0x32:		
				data_middle[i]=0x02;				 
				break;	 			
		 case 0x33:		
				data_middle[i]=0x03;				 
				break;	
		 case 0x34:		
				data_middle[i]=0x04;				 
				break;	
		 case 0x35:		
				data_middle[i]=0x05;				 
				break;	 			
		 case 0x36:		
				data_middle[i]=0x06;				 
				break;	
		 case 0x37:		
				data_middle[i]=0x07;				 
				break;	 			
		 case 0x38:		
				data_middle[i]=0x08;				 
				break;	
		 case 0x39:		
				data_middle[i]=0x09;				 
				break;		 
		 case 0x41:		
				data_middle[i]=0x0A;				 
				break;	 			
		 case 0x42:		
				data_middle[i]=0x0B;				 
				break;	
		 case 0x43:		
				data_middle[i]=0x0C;				 
				break;	 			
		 case 0x44:		
				data_middle[i]=0x0D;				 
				break;	
		 case 0x45:		
				data_middle[i]=0x0E;				 
				break;		 
		 case 0x46:		
				data_middle[i]=0x0F;				 
				break;	 
		 default:
				data_middle[i]=0x0F;			 
				break;
		}
	}
	for(i=0;i<size;i++)//��ȡ����ID
	{
		j=i%2;
		if(j==0)//����
		{
			data_middle[i]=data_middle[i]<<4;
		}
	}
	for(i=0;i<size/2;i++)//��ȡ����ID
	{
		target_data[i]=data_middle[i*2]+data_middle[(i*2)+1];
	}
}
void handle_arrays_u8(unsigned char *data,unsigned char size,unsigned char *printf_data)//�������飬DATA��Ҫ��������飬SIZE������ĳ��ȡ�//���罫0x25�ֿ�����0x32��0x35.��0xAB�ֿ�����0x40,0x41. 
{//DATA����������飬SIZE��������Ԫ�ص�������Printf_data������������ڲ��Դ�ӡ�����顣
	unsigned char i=0;
//	unsigned char j=0;	
	unsigned char DATA_Printf_H_L_middle=0;
	unsigned char DATA_Printf_H=0;
	unsigned char DATA_Printf_L=0;
	for(i=0;i<size;i++)
	{
		DATA_Printf_H_L_middle=data[i];
		DATA_Printf_H_L_middle=DATA_Printf_H_L_middle&0xF0;		
		DATA_Printf_H=(DATA_Printf_H_L_middle>>4);
		switch(DATA_Printf_H)//�ж�Buzzer_flag���ж�Ӧ���������
		{
		 case 0x00:		
				DATA_Printf_H=0x30;
				break;	 			
		 case 0x01:		
				DATA_Printf_H=0x31;		 
				break;	
		 case 0x02:		
				DATA_Printf_H=0x32;				 
				break;	 			
		 case 0x03:		
				DATA_Printf_H=0x33;				 
				break;	
		 case 0x04:		
				DATA_Printf_H=0x34;				 
				break;	
		 case 0x05:		
				DATA_Printf_H=0x35;				 
				break;	 			
		 case 0x06:		
				DATA_Printf_H=0x36;				 
				break;	
		 case 0x07:		
				DATA_Printf_H=0x37;				 
				break;	 			
		 case 0x08:		
				DATA_Printf_H=0x38;				 
				break;	
		 case 0x09:		
				DATA_Printf_H=0x39;				 
				break;		 
		 case 0x0A:		
				DATA_Printf_H=0x41;				 
				break;	 			
		 case 0x0B:		
				DATA_Printf_H=0x42;				 
				break;	
		 case 0x0C:		
				DATA_Printf_H=0x43;				 
				break;	 			
		 case 0x0D:		
				DATA_Printf_H=0x44;				 
				break;	
		 case 0x0E:		
				DATA_Printf_H=0x45;				 
				break;		 
		 case 0x0F:		
				DATA_Printf_H=0x46;				 
				break;	 
		 default:
				DATA_Printf_H=0x46;			 
				break;
		}			
		DATA_Printf_H_L_middle=data[i];
		DATA_Printf_H_L_middle=DATA_Printf_H_L_middle&0x0F;
		DATA_Printf_L=DATA_Printf_H_L_middle;
		switch(DATA_Printf_L)//�ж�Buzzer_flag���ж�Ӧ���������
		{
		 case 0x00:		
				DATA_Printf_L=0x30;
				break;	 			
		 case 0x01:		
				DATA_Printf_L=0x31;		 
				break;	
		 case 0x02:		
				DATA_Printf_L=0x32;				 
				break;	 			
		 case 0x03:		
				DATA_Printf_L=0x33;				 
				break;	
		 case 0x04:		
				DATA_Printf_L=0x34;				 
				break;	
		 case 0x05:		
				DATA_Printf_L=0x35;				 
				break;	 			
		 case 0x06:		
				DATA_Printf_L=0x36;				 
				break;	
		 case 0x07:		
				DATA_Printf_L=0x37;				 
				break;	 			
		 case 0x08:		
				DATA_Printf_L=0x38;				 
				break;	
		 case 0x09:		
				DATA_Printf_L=0x39;				 
				break;		 
		 case 0x0A:		
				DATA_Printf_L=0x41;				 
				break;	 			
		 case 0x0B:		
				DATA_Printf_L=0x42;				 
				break;	
		 case 0x0C:		
				DATA_Printf_L=0x43;				 
				break;	 			
		 case 0x0D:		
				DATA_Printf_L=0x44;				 
				break;	
		 case 0x0E:		
				DATA_Printf_L=0x45;				 
				break;		 
		 case 0x0F:		
				DATA_Printf_L=0x46;				 
				break;	 
		 default:
				DATA_Printf_L=0x46;			 
				break;
		}		
		printf_data[(i*2)+0]=DATA_Printf_H;
		printf_data[(i*2)+1]=DATA_Printf_L;	
	}	
}
void DATA_change_u8_to_u16(unsigned char *data_u8,unsigned int size,unsigned short *data_u16) //������8λ����ת����һ��16λ���ݡ����罫��0x25,0x5F���0x255F
{//data_u8�������8λ���飬size��8λ������Ԫ�ص�������������ż������data_u16�Ƿ��ص�16λ���顣
	unsigned char i=0;
	unsigned short  DATA_middle_u16=0;	
	for(i=0;i<size/2;i++)//������ID��д�����ص�eeprom��
	{
		DATA_middle_u16=data_u8[(i*2)+0];
		DATA_middle_u16=DATA_middle_u16<<8;
		data_u16[i]=DATA_middle_u16+data_u8[(i*2)+1];
	}
}

void DATA_change_u16_to_u8(unsigned short *data_u16,unsigned int size,unsigned char *data_u8) //��һ��16λ����ת��������8λ���ݡ� 
{//data_u16�������16λ���飬size��16λ������Ԫ�ص�������data_u8�Ƿ��ص�8λ���顣
	unsigned char i=0;
	unsigned short  DATA_middle_u16=0;	
	for(i=0;i<size;i++)//������ID��д�����ص�eeprom��
	{
		DATA_middle_u16=data_u16[i];
		data_u8[(i*2)+0]=DATA_middle_u16>>8;
		data_u8[(i*2)+1]=data_u16[i];
	}
}

void Printf_device_type(unsigned char type)//���Դ�ӡ�豸���͡����ݹ��ꡣ
{
	switch(type)
	{
		case 0x17://�ֶ�������ť
			printf("\r\n�ֶ�������ť  ");
		break;
		case 0x1E://���»���̽����
			printf("\r\n���»���̽����");					
		break;
		case 0x28://���̻���̽����
			printf("\r\n���̻���̽����");					
		break;		
		case 0xB1://���ⱨ��װ��
			printf("\r\n���ⱨ��װ��");		
		break;	
		case 0xB2://������λ��վ
			printf("\r\n������λ��վ");					
		break;														
		case 0xB3://�Ŵ�̽����
			printf("\r\n�Ŵ�̽����");					
		break;										
		default:
			printf("\r\nδ֪�豸");	
		break;
	}		
}
void Judge_ID(unsigned short *In_Put_Compare_ID)//�����жϺϷ��ԡ�Ȼ�����ݴ�С�������С�
{
//	unsigned char flash_memory_u8[20]={0x00};//�洢����Ϣ��
//	unsigned short memory_index[30]={0x00};		//��ȡ���������������洢�˶��ٸ�ID��
//	unsigned short memory_address[30]={0x00};	//��ַ�š�
	unsigned char break_flag=0;//while������־λ����Ϊ0��ʱ��������	
	unsigned short  front=0;//���ַ��Ƚ����õ�����ʱ����,�Ͻ硣
	unsigned short  mid=0;  //���ַ��Ƚ����õ�����ʱ����,�м䡣	
	unsigned short  end=0;  //���ַ��Ƚ����õ�����ʱ����,�½�	��
//	unsigned char flash_page=0;//�ڴ���ָ���ID����ҳ���λ�á�
	unsigned short  position_point=0;//ָ��ĵ�ַ��
//	unsigned int  memory_ID_amount=0;	//�����б����ID��������
	unsigned short Memory_Compare_ID[20]={0x00};//�ڴ���ָ���ID��		
	unsigned short Memory_Compare_ID1[20]={0x00};//�ڴ���ָ���ID��			
	unsigned char Memory_Compare_ID_middle[20]={0x00};
//	unsigned char Memory_Compare_ID_middle1[20]={0x00};	
//	unsigned char NODE_ID_u8[20]={0x00};	
//	unsigned char printf_data[50]={0x00};//���Դ�ӡ�õ�����
//----------�豸ID�ĺϷ�����֤������Ѿ���eeprom���б��棬�򷵻�0xFFFF����֮�򷵻ؾ����ַ��-------------//
//	STMFLASH_Read(NUMBER_ADDRESS,(u16*)flash_memory_u8,5); // ��ȡ���Ѿ�ע����豸��ID��������
////����������ܹ�ע���˶��ٸ��豸��
//	memory_ID_amount=flash_memory_u8[6];//��λ
//	memory_ID_amount=(memory_ID_amount<<8)+flash_memory_u8[7];//��λ�ӵ�λ
/********************************/
//	memory_ID_amount=1;//�Ƚ��������ܵ�Ԫ��������
	if(Bluetooh_amount==0)//��δ�����κ�ID
	{
		break_flag=1;//���ؽ�������ıȽϻ��ڣ�ֱ�ӷ���û��ע����豸��
		position_point=0;//��Ϊ��û�б���ID�����Խ������ID����0��λ�á�
	}
	else
	{
//���ַ�ʹ��ʱ�ĳ�ʼֵ
		break_flag=0;
		front=0;								//���ַ��Ƚ����õ�����ʱ����,�Ͻ硣
		end=Bluetooh_amount-1; //���ַ��Ƚ����õ�����ʱ����,�½硣�����һ��Ԫ�ص��±꣩	�����á���ʱ��100��Ԫ�ص�������в��ԡ�
	}
/********************************/		
/*************������**************/	
//	STMFLASH_Read((DEVICE_ADDRESS+(mid/200)*2048+(mid%200)*10),(u16*)Memory_Compare_ID_middle,3);//���������λ�õ�ҳ�롣����һ��3λ��16λID���롣ÿ�ζ�10���ֽڡ�����4λ�Ǳ��á�
//	handle_arrays_u8(Memory_Compare_ID_middle,6,printf_data);//���罫0x25�ֿ�����0x32��0x35.��0xAB�ֿ�����0x40,0x41.								
//	USART1_SEND(printf_data,12);								
//	printf("\r\n");			
/********************************/	

//	STMFLASH_Read((DEVICE_ADDRESS+(mid/200)*2048+(mid%200)*10),(u16*)Memory_Compare_ID_middle,3);//���������λ�õ�ҳ�롣����һ��3λ��16λID���롣ÿ�ζ�10���ֽڡ�����4λ�Ǳ��á�
//	handle_arrays_u8(Memory_Compare_ID_middle,6,printf_data);//���罫0x25�ֿ�����0x32��0x35.��0xAB�ֿ�����0x40,0x41.								
//	USART1_SEND(printf_data,12);								
//	printf("\r\n");			
//	DATA_change_u8_to_u16(Memory_Compare_ID_middle,6,Memory_Compare_ID);
//	printf("%d\r\n",Memory_Compare_ID[0]);
//	printf("%d\r\n",Memory_Compare_ID[1]);	
//	printf("%d\r\n",Memory_Compare_ID[2]);		
/************������************/			
//	printf("����ID\r\n");	
//	printf("%d\r\n",In_Put_Compare_ID[0]);
//	printf("%d\r\n",In_Put_Compare_ID[1]);	
//	printf("%d\r\n",In_Put_Compare_ID[2]);	
//	printf("\r\n");	
/************������************/				
	while(break_flag==0)//һֱ�Ƚϣ�ֱ���ȽϽ�����������
	{
		mid=(front+end)/2;  		//���ַ��Ƚ����õ�����ʱ����,�м䡣
//		printf("%d\r\n",mid);//������			
		if(end-front<=1)
		{//����������ݺ�������������ǰ��Ƚϡ�
			if( (end-front)==0 )//ʣ��һλ����
			{
//6λID��4λ���á�����10λ��
//ÿҳ���洢204����Լ�����ÿҳ�洢200����			
/****************************************************��������Ҫ�Ƚ϶���ڲ��洢�ռ��ID*****************************************************/
//*****************����ַ***********ҳ��ַ********������ַ
				STMFLASH_Read((DEVICE_ADDRESS+(front/200)*2048+(front%200)*10),(u16*)Memory_Compare_ID_middle,3);//���������λ�õ�ҳ���λ�á�����һ��6��Ԫ�ص�8λID���롣
/*************������**************/	
//				handle_arrays_u8(Memory_Compare_ID_middle,6,printf_data);//���罫0x25�ֿ�����0x32��0x35.��0xAB�ֿ�����0x40,0x41.								
//				USART1_SEND(printf_data,12);
//				printf("\r\n");			
				DATA_change_u8_to_u16(Memory_Compare_ID_middle,6,Memory_Compare_ID);//��8λ����ת����16λ���顣
//����	Memory_Compare_ID[0],Memory_Compare_ID[1],Memory_Compare_ID[2];			
				if((In_Put_Compare_ID[0]<Memory_Compare_ID[0])||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]<Memory_Compare_ID[1]))||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID[1])&&(In_Put_Compare_ID[2]<Memory_Compare_ID[2])))//���ָ���IDС�������ID
				{//����ֵС���ڴ��ж�ȡ������ֵ��
					break_flag=1;//������־λ��
					position_point=front;				
				}
				else if((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID[1])&&(In_Put_Compare_ID[2]==Memory_Compare_ID[2]))//����鵽��һģһ����ID��˵��ע�����
				{//����ֵ�����ڴ��ж�ȡ������ֵ��
					break_flag=2;//������־λ��
					position_point=front;								
				}
				else if((In_Put_Compare_ID[0]>Memory_Compare_ID[0])||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]>Memory_Compare_ID[1]))||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID[1])&&(In_Put_Compare_ID[2]>Memory_Compare_ID[2])))//���ָ���ID���������ID
				{//����ֵ�����ڴ��ж�ȡ������ֵ��
					break_flag=1;//������־λ��
					position_point=front+1;						
				}
			}
			else//ʣ����λ����
			{
				STMFLASH_Read((DEVICE_ADDRESS+(front/200)*2048+(front%200)*10),(u16*)Memory_Compare_ID_middle,3);//���������λ�õ�ҳ���λ�á�����һ��6��Ԫ�ص�8λID���롣
/*************������**************/	
//				handle_arrays_u8(Memory_Compare_ID_middle,6,printf_data);//���罫0x25�ֿ�����0x32��0x35.��0xAB�ֿ�����0x40,0x41.								
//				USART1_SEND(printf_data,12);								
//				printf("\r\n");			
				DATA_change_u8_to_u16(Memory_Compare_ID_middle,6,Memory_Compare_ID);//��8λ����ת����16λ���顣
//����	Memory_Compare_ID[0],Memory_Compare_ID[1],Memory_Compare_ID[2];		
				STMFLASH_Read((DEVICE_ADDRESS+((front+1)/200)*2048+((front%200)+1)*10),(u16*)Memory_Compare_ID_middle,3);//���������λ�õ�ҳ���λ�á�����һ��6��Ԫ�ص�8λID���롣
/*************������**************/	
//				handle_arrays_u8(Memory_Compare_ID_middle,6,printf_data);//���罫0x25�ֿ�����0x32��0x35.��0xAB�ֿ�����0x40,0x41.								
//				USART1_SEND(printf_data,12);								
//				printf("\r\n");			
				DATA_change_u8_to_u16(Memory_Compare_ID_middle,6,Memory_Compare_ID1);//��8λ����ת����16λ���顣
//����	Memory_Compare_ID[0],Memory_Compare_ID[1],Memory_Compare_ID[2];						
// front   end					
//   A      B  			
				if((In_Put_Compare_ID[0]<Memory_Compare_ID[0])||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]<Memory_Compare_ID[1]))||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID[1])&&(In_Put_Compare_ID[2]<Memory_Compare_ID[2])))//���ָ���IDС�������ID
				{//����ֵС���ڴ��ж�ȡ������Aֵ��
					break_flag=1;//������־λ��
					position_point=front;				
				}			
				else if((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID[1])&&(In_Put_Compare_ID[2]==Memory_Compare_ID[2]))//����鵽��һģһ����ID��˵��ע�����
				{//����ֵ�����ڴ��ж�ȡ������Aֵ��
					break_flag=2;//������־λ��
					position_point=front;
				}
				else if( ((In_Put_Compare_ID[0]>Memory_Compare_ID[0])||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]>Memory_Compare_ID[1]))||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID[1])&&(In_Put_Compare_ID[2]>Memory_Compare_ID[2])))&&((In_Put_Compare_ID[0]<Memory_Compare_ID1[0])||((In_Put_Compare_ID[0]==Memory_Compare_ID1[0])&&(In_Put_Compare_ID[1]<Memory_Compare_ID1[1]))||((In_Put_Compare_ID[0]==Memory_Compare_ID1[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID1[1])&&(In_Put_Compare_ID[2]<Memory_Compare_ID1[2]))) )//���ָ���ID���������ID
				{//����ֵ�����ڴ��ж�ȡ������Aֵ,С�ڶ�ȡ������Bֵ��				
					break_flag=1;//������־λ��
					position_point=front+1;				
				}
				else if((In_Put_Compare_ID[0]==Memory_Compare_ID1[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID1[1])&&(In_Put_Compare_ID[2]==Memory_Compare_ID1[2]))//����鵽��һģһ����ID��˵��ע�����
				{//����ֵ�����ڴ��ж�ȡ������Bֵ��
					break_flag=2;//������־λ��
					position_point=front+1;							
				}
				else if((In_Put_Compare_ID[0]>Memory_Compare_ID1[0])||((In_Put_Compare_ID[0]==Memory_Compare_ID1[0])&&(In_Put_Compare_ID[1]>Memory_Compare_ID1[1]))||((In_Put_Compare_ID[0]==Memory_Compare_ID1[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID1[1])&&(In_Put_Compare_ID[2]>Memory_Compare_ID1[2])))
				{//����ֵ�����ڴ��ж�ȡ������Bֵ��
					break_flag=1;//������־λ��
					position_point=front+2;							
				}			
			}
		}
		else
		{
			STMFLASH_Read((DEVICE_ADDRESS+(mid/200)*2048+(mid%200)*10),(u16*)Memory_Compare_ID_middle,3);//���������λ�õ�ҳ���λ�á�����һ��6��Ԫ�ص�8λID���롣
/*************������**************/	
//				handle_arrays_u8(Memory_Compare_ID_middle,6,printf_data);//���罫0x25�ֿ�����0x32��0x35.��0xAB�ֿ�����0x40,0x41.								
//				USART1_SEND(printf_data,12);								
//				printf("\r\n");			
			DATA_change_u8_to_u16(Memory_Compare_ID_middle,6,Memory_Compare_ID);//��8λ����ת����16λ���顣
//����	Memory_Compare_ID[0],Memory_Compare_ID[1],Memory_Compare_ID[2];		
			
			if((In_Put_Compare_ID[0]<Memory_Compare_ID[0])||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]<Memory_Compare_ID[1]))||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID[1])&&(In_Put_Compare_ID[2]<Memory_Compare_ID[2])))//���ָ���IDС�������ID
			{//������ǰ���ȥ���ҡ�		
				end=mid-1;  			//���ַ��Ƚ����õ�����ʱ����,�½�	��	
			}		
			else if((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID[1])&&(In_Put_Compare_ID[2]==Memory_Compare_ID[2]))//����鵽��һģһ����ID��˵��ע�����
			{//��ȡ�
				break_flag=2;//������־λ��
				position_point=mid;
			}
			else if((In_Put_Compare_ID[0]>Memory_Compare_ID[0])||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]>Memory_Compare_ID[1]))||((In_Put_Compare_ID[0]==Memory_Compare_ID[0])&&(In_Put_Compare_ID[1]==Memory_Compare_ID[1])&&(In_Put_Compare_ID[2]>Memory_Compare_ID[2])))
			{//����������ȥ���ҡ�
				front=mid+1;			//���ַ��Ƚ����õ�����ʱ����,�Ͻ硣			
			}
		}
/************������************/					
//	printf("�ڴ��е�ID\r\n");	
//	printf("%d\r\n",Memory_Compare_ID[0]);
//	printf("%d\r\n",Memory_Compare_ID[1]);	
//	printf("%d\r\n",Memory_Compare_ID[2]);
//	printf("%d\r\n",Memory_Compare_ID1[0]);
//	printf("%d\r\n",Memory_Compare_ID1[1]);	
//	printf("%d\r\n",Memory_Compare_ID1[2]);		
//	printf("\r\n");				
/************������************/					
	}
//����ֵ����3����ϢԪ�ء�һ����ID�Ƿ�ע�����һ����ҳ�롣һ����ID���ڵ�λ�ã����ע�������������ID���ڵ�λ�á����ûע�������������ID֮���λ�ã���
	if(break_flag==2)//˵��ע���
	{
		In_Put_Compare_ID[3]=position_point;	//ע������ID�ĵ�ַ	
		In_Put_Compare_ID[4]=0x0001;						//ע������ID
//		printf("\r\n�������ID��IDλ���±�Ϊ:%d\r\n",position_point);	
//		DATA_change_u16_to_u8(In_Put_Compare_ID,3,NODE_ID_u8);		
//		handle_arrays_u8(NODE_ID_u8,6,printf_data);
//		USART1_SEND(printf_data,12);
//		printf("\r\n");
	}
	else
	{
		In_Put_Compare_ID[3]=position_point;	//���������ID�ĵ�ַ	
		In_Put_Compare_ID[4]=0x0000;						//û��ע������ID
//		printf("\r\n���������ID����Ҫ��ID���뵽�±�Ϊ:%d\r\n",position_point);
//		DATA_change_u16_to_u8(In_Put_Compare_ID,3,NODE_ID_u8);		
//		handle_arrays_u8(NODE_ID_u8,6,printf_data);
//		USART1_SEND(printf_data,12);
//		printf("\r\n");		
	}
//	return position_point;//���ص�ַ�š�	�����ַ��ָ����ÿҳ�е�0~199.����ָ��ĵ�ַΪ320.��ʵ�ʵ�Ƭ���еĵ�ַΪDEVICE_ADDRESS+(320/200)*2048+(320%200)*10
//	printf("ָ���λ�ã�%d\r\n",position_point);//�����á�	
}
unsigned int Save_ROUTER_ID(unsigned char *ROUTER_ID)//��������ID.�������ɹ�������1���������ʧ�ܣ��򷵻�0��
{
	unsigned int i=0;
	unsigned char flash_memory_u8[20]={0};//��ȡ�����������������ص�ID����ʹ洢�˶��ٸ�ID��
	STMFLASH_Read(NUMBER_ADDRESS,(u16*)flash_memory_u8,4); //��ȡ������Ϣ��ǰ6λ�����ص�ID������λ��������ע����豸������			
	for(i=0;i<6;i++)//���µ�����ID�����ȥ
	{
		flash_memory_u8[i]=ROUTER_ID[i];
	}						
	STMFLASH_Write(NUMBER_ADDRESS,(u16*)flash_memory_u8,5);//���䶯���������������д��eeprom
	STMFLASH_Read(NUMBER_ADDRESS,(u16*)flash_memory_u8,4);
	if( (flash_memory_u8[0]==ROUTER_ID[0])&&(flash_memory_u8[1]==ROUTER_ID[1])&&(flash_memory_u8[2]==ROUTER_ID[2])&&(flash_memory_u8[3]==ROUTER_ID[3])&&(flash_memory_u8[4]==ROUTER_ID[4])&&(flash_memory_u8[5]==ROUTER_ID[5]) )
	{//�����ȡ���������ݸ����ȥ������һ�£�����Ϊд��ɹ���
		i=1;
	}
	else
	{
		i=0;
	}
	return i;
}
unsigned int Save_NODE_ID(unsigned char *In_put_ID)//�����豸ID.�������ɹ������ر���ĵ�ַ�š��������ʧ�ܣ��򷵻�0xFFFF��
{
//	unsigned char printf_data[50]={0};//���Դ�ӡ�õ�����	
	unsigned char flash_memory_u8[20]={0x00};//�洢����Ϣ��
	unsigned int i=0;	
//	unsigned int j=0;
	unsigned char test[20]={0x00};
	unsigned int back_flag=0;//���ر�־λ
	unsigned char Page_All=0;//����ҳ
	unsigned char Page_Now=0;//���������ҳ
	unsigned char Address_Now=0;//��һҳ�е�λ�á�
//	unsigned char Address_Now_middle=0;
	unsigned int  position_point=0;//ָ��ĵ�ַ��
//	unsigned char Last_ID[10]={0};//ÿҳ�����һ��ID������������У�Ȼ�������һҳ�ĵ�һ��ID�ϡ�
//	unsigned int  flash_memory_u16[30]={0};//��ȡ���������������洢�˶��ٸ�ID�����ص�ID���롣
//	unsigned int  memory_ID_amount=0;//�����б����ID�������� 	
//	STMFLASH_Read(NUMBER_ADDRESS,(u16*)flash_memory_u8,4); // ��ȡ���Ѿ�ע����豸��ID��������
////����������ܹ�ע���˶��ٸ��豸��
//	memory_ID_amount=flash_memory_u8[6];//��λ
//	memory_ID_amount=(memory_ID_amount<<8)+flash_memory_u8[7];//��λ�ӵ�λ
//	printf("��������ע���豸ID����Ϊ��%d\r\n",memory_ID_amount);//������
	Page_All=Bluetooh_amount/200;//������ܹ����ڴ��˶���ҳ��
//������������ĵ�ַ�����ڴ���������е�6~7����λ�á���	
	position_point=In_put_ID[6];//��λ
	position_point=(position_point<<8)+In_put_ID[7];//��λ�ӵ�λ
	In_put_ID[6]=0x00;
	In_put_ID[7]=0x00;
//	printf("ָ���λ�ã�%d\r\n",position_point);//�����á�	
	Page_Now=position_point/200;//��������Ԫ��Ӧ�ô��ڵڼ�ҳ��
	Address_Now=position_point%200;//��������Ԫ��Ӧ������һҳ�е��ĸ�λ�á�0��ʼ��
//	handle_arrays_u8(In_put_ID,10,printf_data);	
//	USART1_SEND(printf_data,20);
	STMFLASH_Read((DEVICE_ADDRESS+Page_Now*2048),(u16*)All_ID,1000);//������Ҫ�޸�ҳ����ҳ��ID��
//	test=Address_Now-position_point;
//	printf("%d\r\n",test);//�����á�
//	for(i=0;i<(1000-Address_Now)*10;i++)//Ҫ����Ԫ�صĸ�����
	for(i=0;i<2000-(Address_Now*10);i++)//Ҫ����Ԫ�صĸ�����	
	{
		All_ID[2009-i]=All_ID[2009-i-10];//����������10λ��
	}
//	printf("%d\r\n",Address_Now);//�����á�	
	for(i=0;i<10;i++)//��Ҫ�����10λID��䵽����λ�á�
	{
		All_ID[(Address_Now*10)+i]=In_put_ID[i];
	}
	STMFLASH_Write( (DEVICE_ADDRESS+Page_Now*2048),(u16*)All_ID,1005);//����ҳ�䶯�������д��
//�Ƿ���Ҫ���ơ��粻���ƣ���ᵼ�±�����д�ڸ��ϴ���д��������д��Ĳ�һ�µ�ʱ�򣩡�
//	STMFLASH_Read((DEVICE_ADDRESS+position_point*10),(u16*)All_ID,5);//�����ո�д���ID��	
	STMFLASH_Read((DEVICE_ADDRESS+(position_point/200)*2048+(position_point%200)*10),(u16*)All_ID,5);//�����ո�д���ID��		
///******************������***********************/
//	handle_arrays_u8(All_ID,10,printf_data);//������	
//	USART1_SEND(printf_data,20);//��ӡ�����ID���롣		
//	printf("\r\n");
///***********************************************/
	if( (All_ID[0]==In_put_ID[0])&&(All_ID[1]==In_put_ID[1])&&(All_ID[2]==In_put_ID[2])&&(All_ID[3]==In_put_ID[3])&&(All_ID[4]==In_put_ID[4])&&(All_ID[5]==In_put_ID[5]) )
	{
		back_flag=position_point;//����ֵ��ָ��Ĵ洢ID��λ�á�
		if(Bluetooh_amount>=Bluetooh_Max_NO)//�ж�������ID�����Ƿ���Ҫ��1.
		{
			
		}	
		else
		{
			Bluetooh_amount=Bluetooh_amount+1;
		}	
	STMFLASH_Read(NUMBER_ADDRESS,(u16*)flash_memory_u8,4); // ��ȡ���Ѿ�ע����豸��ID��������
//����������ܹ�ע���˶��ٸ��豸��		
		flash_memory_u8[6]=Bluetooh_amount>>8;//��λ			
		flash_memory_u8[7]=Bluetooh_amount;//��λ
		STMFLASH_Write(NUMBER_ADDRESS,(u16*)flash_memory_u8,4);//���䶯���������Ϣ����д�뵽�����С�
		if(Page_All==Page_Now)//д��ĵ�ַ�����һҳ���档
		{
		}
		else
		{
			for(i=0;i<Page_All-Page_Now;i++)//�鿴��Ҫ���ж��ٴ�ҳ�Ĳ������������λ����ҳ��
			{		
				STMFLASH_Read((DEVICE_ADDRESS+(Page_Now+i)*2048+2000),(u16*)test,5);//����ĩ��д���ID��			
				STMFLASH_Read((DEVICE_ADDRESS+(Page_Now+i+1)*2048),(u16*)All_ID,1000);//����ĩ��д���ID��	
				for(i=0;i<2000;i++)//Ҫ����Ԫ�صĸ�����
				{
					All_ID[2009-i]=All_ID[2009-i-10];//����������10λ��
				}				
				for(i=0;i<10;i++)//��Ҫ�����10λID��䵽����λ�á�
				{
					All_ID[i]=test[i];
				}
				STMFLASH_Write( (DEVICE_ADDRESS+(Page_Now+i+1)*2048),(u16*)All_ID,1005);//����ҳ�䶯�������д��		
			}			
		}		
		if(Bluetooh_amount%200==1)//���д����һҳ���������һҳ��
		{
				STMFLASH_Read((DEVICE_ADDRESS+Page_All*2048+2000),(u16*)test,5);//����ĩ��д���ID		
				for(i=0;i<10;i++)//��Ҫ�����10λID��䵽����λ�á�
				{
					All_ID[i]=test[i];
				}
				STMFLASH_Write( (DEVICE_ADDRESS+(Page_All+1)*2048),(u16*)All_ID,5);//����ҳ�䶯�������д��								
		}		
	}
	else
	{
//		printf("�����豸IDʧ��");
		back_flag=0xFFFF;
	}
	return back_flag;
}


void Delete_NODE_ID(unsigned char *In_put_ID)//ɾ���豸ID.���ɾ���ɹ������ر���ĵ�ַ�š����ɾ��ʧ�ܣ��򷵻�0xFFFF��
{
//	unsigned char printf_data[50]={0};//���Դ�ӡ�õ�����	
	unsigned char flash_memory_u8[20]={0x00};//�洢����Ϣ��
	unsigned int i=0;	
	unsigned int j=0;
	unsigned char test[20]={0x00};
//	unsigned int back_flag=0;//���ر�־λ
	unsigned char Page_All=0;//����ҳ
	unsigned char Page_Now=0;//���������ҳ
	unsigned char Address_Now=0;//��һҳ�е�λ�á�
//	unsigned char Address_Now_middle=0;
	unsigned int  position_point=0;//ָ��ĵ�ַ��
//	unsigned char Last_ID[10]={0};//ÿҳ�����һ��ID������������У�Ȼ�������һҳ�ĵ�һ��ID�ϡ�
//	unsigned int  flash_memory_u16[30]={0};//��ȡ���������������洢�˶��ٸ�ID�����ص�ID���롣
//	unsigned int  memory_ID_amount=0;//�����б����ID�������� 	
	STMFLASH_Read(NUMBER_ADDRESS,(u16*)flash_memory_u8,4); // ��ȡ���Ѿ�ע����豸��ID��������
//����������ܹ�ע���˶��ٸ��豸��
	Bluetooh_amount=flash_memory_u8[6];//��λ
	Bluetooh_amount=(Bluetooh_amount<<8)+flash_memory_u8[7];//��λ�ӵ�λ
//	printf("��������ע���豸ID����Ϊ��%d\r\n",memory_ID_amount);//������
	Page_All=Bluetooh_amount/200;//������ܹ����ڴ��˶���ҳ��
//������������ĵ�ַ��	
	position_point=In_put_ID[6];//��λ
	position_point=(position_point<<8)+In_put_ID[7];//��λ�ӵ�λ
//	printf("ָ���λ�ã�%d\r\n",position_point);//�����á�	
	Page_Now=position_point/200;//��������Ԫ��Ӧ�ô��ڵڼ�ҳ��ȡ����
	Address_Now=position_point%200;//��������Ԫ��Ӧ������һҳ�е��ĸ�λ�á�0��ʼ��ȡ�ࡣ
//	handle_arrays_u8(In_put_ID,10,printf_data);
//	USART1_SEND(printf_data,20);
	STMFLASH_Read((DEVICE_ADDRESS+Page_Now*2048),(u16*)All_ID,1005);//������Ҫ�޸�ҳ����ҳ��ID��
//	test=Address_Now-position_point;
//	printf("%d\r\n",test);//�����á�
//	for(i=0;i<(1000-Address_Now)*10;i++)//Ҫ����Ԫ�صĸ�����
	for(i=0;i<2010-(Address_Now*10);i++)//Ҫ����Ԫ�صĸ�����
	{
		All_ID[(Address_Now*10)+i]=All_ID[(Address_Now*10)+i+10];//����������10λ��
	}
//	printf("%d\r\n",Address_Now);//�����á�	
	STMFLASH_Read((DEVICE_ADDRESS+(Page_Now+1)*2048),(u16*)test,5);//������Ҫ�޸�ҳ�ĵ�һ��ID��	
	for(i=0;i<10;i++)//����һҳ��ǰ���ID���浽��ҳ��β����
	{
		All_ID[2000+i]=test[i];
	}
	STMFLASH_Write( (DEVICE_ADDRESS+Page_Now*2048),(u16*)All_ID,1005);//����ҳ�䶯�������д��
//�Ƿ���Ҫ���ơ��粻���ƣ���ᵼ�±�����д�ڸ��ϴ���д��������д��Ĳ�һ�µ�ʱ�򣩡�
//	STMFLASH_Read((DEVICE_ADDRESS+position_point*10),(u16*)All_ID,5);//�����ո�д���ID��	
//	STMFLASH_Read((DEVICE_ADDRESS+(position_point/200)*2048+(position_point%200)*10),(u16*)All_ID,5);//�����ո�д���ID��		
///******************������***********************/
//	handle_arrays_u8(All_ID,10,printf_data);//������	
//	USART1_SEND(printf_data,20);//��ӡ�����ID���롣		
//	printf("\r\n");
///***********************************************/
		Bluetooh_amount=Bluetooh_amount-1;//������ID����������1.
		flash_memory_u8[7]=Bluetooh_amount;//��λ
		flash_memory_u8[6]=Bluetooh_amount>>8;//��λ	
		STMFLASH_Write(NUMBER_ADDRESS,(u16*)flash_memory_u8,4);//���䶯���������Ϣ����д�뵽�����С�
		if(Page_All==Page_Now)//д��ĵ�ַ�����һҳ���档
		{
		}
		else
		{
			for(i=1;i<(Page_All-Page_Now)+1;i++)//�鿴��Ҫ���ж��ٴ�ҳ�Ĳ������������λ����ҳ��
			{
				STMFLASH_Read((DEVICE_ADDRESS+i*2048),(u16*)All_ID,1005);//������Ҫ�޸�ҳ����ҳ��ID��
				for(j=0;j<2010;j++)//Ҫ����Ԫ�صĸ�����
				{
					All_ID[j]=All_ID[j+10];//����������10λ��
				}
				STMFLASH_Read((DEVICE_ADDRESS+(i+1)*2048),(u16*)test,5);//������Ҫ�޸�ҳ����ҳ��ID��
				for(j=0;j<10;j++)//����һҳ��ǰ���ID���浽��ҳ��β����
				{
					All_ID[2000+j]=test[j];
				}	
				STMFLASH_Write( (DEVICE_ADDRESS+i*2048),(u16*)All_ID,1005);//����ҳ�䶯�������д��
			}
		}		
}

void Delete_ALL_NODE_ID(void)//ɾ������ע����豸ID
{
	unsigned int i=0;
	unsigned char memory_index[20]={0x00};
	for(i=0;i<2048;i++)
	{
		All_ID[i]=0xFF;
	}
	STMFLASH_Read(NUMBER_ADDRESS,(u16*)memory_index,5); // ��ȡ��ע���豸����		
	memory_index[6]=0x00;//��ձ����ID����	 
	memory_index[7]=0x00;		
	Bluetooh_amount=0;//���������վ�б����ID����
	STMFLASH_Write(NUMBER_ADDRESS,(u16*)memory_index,5);
	STMFLASH_Write(DEVICE_ADDRESS,(u16*)All_ID,1024);
	printf("������\r\n");
	
}


/*****************************************************************************
* ��������: unsigned char  Sting_Compare(unsigned char  *Sting1,unsigned char  *Sting2,unsigned char Len)
* ��������: �ַ����Ƚ�
* ��    ��:Sting1���ַ���1;Sting2���ַ���2;Len:�ַ���2�ĳ���
* ����  ֵ:0���ַ�������ͬ��1���ַ�����ͬ
*****************************************************************************/
unsigned char  Sting_Compare(unsigned char  *Sting1,unsigned char  *Sting2,unsigned char Len)//�ַ����Ƚ�
{
	unsigned char i = 0; 
	unsigned char j = 0; 	
	unsigned char Flag = 1;
	j=strlen(Sting1);
	if(j==Len)
	{
		for(i=0;i<Len;i++)
		{
			if(Sting1[i]!=Sting2[i])
			{
							Flag = 0;
			}
		}		
	}
	else
	{
		Flag=0;
	}
	return Flag;
}

unsigned int str_int(unsigned char *str)//������ת����int�����ݡ���������strΪ��0x31,0x32,0x33,0x34,0x35,0x36��ת����ɺ�ͱ�ɣ�123456��
{
	unsigned int i=0,tmp=0;
	unsigned char len=0;
	len=strlen((char *)str);
//	for(i=0;(i<len)&&(i<6);i++)
	for(i=0;i<len;i++)	
	{
		if(str[i]>='0'&&str[i]<='9')
		{
			tmp=tmp*10+(str[i]-'0');			
		}
		else 
		{
			break;
		}
	}
	return tmp;
}













