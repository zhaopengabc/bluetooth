/**
************************************************************************************************
* @file   		tcp_demo.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-02-14
* @brief   		TCP ��ʾ����
* @attention  
************************************************************************************************
**/

#include <stdio.h>
#include <string.h>
#include "tcp_demo.h"
#include "W5500_conf.h"
#include "w5500.h"
#include "socket.h"
#include "UsartMsgProcessDrv.h"
#include  <os.h>

uint8 buff[2048];				                              	         /*����һ��2KB�Ļ���*/

/**
*@brief		TCP Server�ػ���ʾ������
*@param		��
*@return	��
*/
void do_tcp_server(void)
{	
	uint16 len=0;  
//	CPU_SR_ALLOC();
//	OS_CRITICAL_ENTER();//�����ٽ���
	switch(getSn_SR(SOCK_TCPS))											            	/*��ȡsocket��״̬*/
	{
		case SOCK_CLOSED:													                  /*socket���ڹر�״̬*/
			socket(SOCK_TCPS ,Sn_MR_TCP,local_port,Sn_MR_ND);	        /*��socket*/
		  break;     
    
		case SOCK_INIT:														                  /*socket�ѳ�ʼ��״̬*/
			listen(SOCK_TCPS);												                /*socket��������*/
		  break;
		
		case SOCK_ESTABLISHED:												              /*socket�������ӽ���״̬*/
		
			if(getSn_IR(SOCK_TCPS) & Sn_IR_CON)
			{
				setSn_IR(SOCK_TCPS, Sn_IR_CON);								          /*��������жϱ�־λ*/
			}
			len=getSn_RX_RSR(SOCK_TCPS);									            /*����lenΪ�ѽ������ݵĳ���*/
			if(len>0)
			{
				recv(SOCK_TCPS,buff,len);								              	/*��������Client������*/
				buff[len]=0x00; 											                  /*����ַ���������*/
				printf("%s\r\n",buff);
				send(SOCK_TCPS,buff,len);									              /*��Client��������*/
		  }
		  break;
		
		case SOCK_CLOSE_WAIT:												                /*socket���ڵȴ��ر�״̬*/
			close(SOCK_TCPS);
		  break;
	}
//	OS_CRITICAL_EXIT();	//�˳��ٽ���	
}

/**
*@brief		TCP Client�ػ���ʾ������
*@param		��
*@return	��
*/
unsigned int do_tcp_client_tx(unsigned char *TXdata,unsigned int length) 
{	
	unsigned int b=0,i;
	
//	CPU_SR_ALLOC();
//	OS_CRITICAL_ENTER();//�����ٽ���
		printf("��:");
		for(i=0;i<length;i++)
		{printf("%x,",TXdata[i]);}
		printf("\r\n");
				
	switch(getSn_SR(SOCK_TCPC))								  				         /*��ȡsocket��״̬*/
	{
		case SOCK_CLOSED:											        		         /*socket���ڹر�״̬*/
			socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND);
		  break;
		
		case SOCK_INIT:													        	         /*socket���ڳ�ʼ��״̬*/
			connect(SOCK_TCPC,remote_ip,remote_port);                /*socket���ӷ�����*/ 
		  break;
		
		case SOCK_ESTABLISHED: 												             /*socket�������ӽ���״̬*/
			if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
			{
				setSn_IR(SOCK_TCPC, Sn_IR_CON); 							         /*��������жϱ�־λ*/
			}
			send(SOCK_TCPC,TXdata,length);								     	         /*��Server��������*/	
      b=1;			
		  break;
			
		case SOCK_CLOSE_WAIT: 											    	         /*socket���ڵȴ��ر�״̬*/
			close(SOCK_TCPC);
		  break;
	}
//	OS_CRITICAL_EXIT();	//�˳��ٽ���	
	return b;
}
//����  0�������ݣ�����������
//      1�������ݣ�����������
//      2.3�����߰ε���
//      2.3.5���������˿ڹرա�
unsigned int do_tcp_client_RxStatus(void)
{	
	unsigned char count=0;
  unsigned char nbr=0xFF;
	
//	CPU_SR_ALLOC();	
//	OS_CRITICAL_ENTER();//�����ٽ���

//	IINCHIP_WRITE(SOCK_TCPC,Sn_CR_SEND_KEEP);                    /*���ӷ�����,����һ������*/ 	
	
  static unsigned char Snflag=0;
	
//	if(Snflag==0)
//	{
//	IINCHIP_WRITE(Sn_KPALVTR(SOCK_TCPC),0x01);
//	Snflag=1;
//	}
	
	switch(getSn_SR(SOCK_TCPC))								  				         /*��ȡsocket��״̬*/
	{	
		case SOCK_CLOSED:											        		         /*socket���ڹر�״̬*/
//			socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND);
		  nbr=2;
		  break;
		
		case SOCK_INIT:													        	         /*socket���ڳ�ʼ��״̬*/
//			connect(SOCK_TCPC,remote_ip,remote_port);                /*socket���ӷ�����*/ 
			nbr=3;
		  break;
		
		case SOCK_ESTABLISHED: 												             /*socket�������ӽ���״̬*/
			IINCHIP_WRITE( Sn_CR(SOCK_TCPC) ,Sn_CR_SEND_KEEP); 	    // KeepAlive����-�ֶ�ģʽ
			nbr=0;
			count=getSn_RX_RSR(SOCK_TCPC); 								  	         /*����lenΪ�ѽ������ݵĳ���*/
		  if(count!=0) 
		  {
				return 1;
			}
		  break;
		
		case SOCK_CLOSE_WAIT: 											    	         /*socket���ڵȴ��ر�״̬*/
//			close(SOCK_TCPC);
		  nbr=4;
		  break;
				
		default:	
			nbr=5;
		  break;
	}
//	OS_CRITICAL_EXIT();	//�˳��ٽ���	
	return nbr;
}

//����  0�������ݣ�����������
//      1�������ݣ�����������
//      2.3�����߰ε���
//      2.3.5���������˿ڹرա�
unsigned int do_tcp_client_RxRes(void) 
{	
	unsigned char count=0;
  unsigned char nbr=0;
	
	switch(getSn_SR(SOCK_TCPC))								  				         /*��ȡsocket��״̬*/
	{	
		case SOCK_CLOSED:											        		         /*socket���ڹر�״̬*/
 			socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND);
		  nbr=2;
		  break;
		
		case SOCK_INIT:													        	         /*socket���ڳ�ʼ��״̬*/
 			connect(SOCK_TCPC,remote_ip,remote_port);                /*socket���ӷ�����*/ 
			nbr=3;
		  break;
		
		case SOCK_ESTABLISHED: 												             /*socket�������ӽ���״̬*/
			count=getSn_RX_RSR(SOCK_TCPC); 								  	         /*����lenΪ�ѽ������ݵĳ���*/
		  if(count!=0) 
		  {
				return 1;
			}
		  break;
		
		case SOCK_CLOSE_WAIT: 											    	         /*socket���ڵȴ��ر�״̬*/
 			close(SOCK_TCPC);
		  nbr=4;
		  break;
				
		default:	
			nbr=5;
		  break;
	}
	return nbr;
}

unsigned int do_tcp_client_rx(unsigned char *RXdata) 
{	
  unsigned int nbr=0,i=0;
	
//	CPU_SR_ALLOC();
//	OS_CRITICAL_ENTER();//�����ٽ���
	
	switch(getSn_SR(SOCK_TCPC))								  				         /*��ȡsocket��״̬*/
	{
		case SOCK_CLOSED:											        		         /*socket���ڹر�״̬*/
			socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND);
		  break;
		
		case SOCK_INIT:													        	         /*socket���ڳ�ʼ��״̬*/
			connect(SOCK_TCPC,remote_ip,remote_port);                /*socket���ӷ�����*/ 
		  break;
		
		case SOCK_ESTABLISHED: 												             /*socket�������ӽ���״̬*/
			if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
			{
				setSn_IR(SOCK_TCPC, Sn_IR_CON); 							         /*��������жϱ�־λ*/
			}
		
			nbr=getSn_RX_RSR(SOCK_TCPC); 								  	         /*����lenΪ�ѽ������ݵĳ���*/
			if((nbr>0)&&(nbr<500))//С��500�ֽں���գ�
			{
				recv(SOCK_TCPC,RXdata,nbr); 							   		         /*��������Server������*/
				RXdata[nbr]=0x00;  											                 /*����ַ���������*/
				
				printf("��:");
				for(i=0;i<nbr;i++)
				{printf("%x_",RXdata[i]);}
				printf("\r\n");
				
				//TEST_DEBUG�������ݴ�ӡ
//			  UARTx_Send_Data(USART1,RXdata,nbr);
			}	
			else
			{
				recv(SOCK_TCPC,buff,nbr); 							   		         /*��������Server������*/
				buff[nbr]=0x00;  											                 /*����ַ���������*/
//				printf("%s\r\n",buff);				
			}
			
		  break;
			
		case SOCK_CLOSE_WAIT: 											    	         /*socket���ڵȴ��ر�״̬*/
			close(SOCK_TCPC);
		  break;

	}
//	OS_CRITICAL_EXIT();	//�˳��ٽ���	
	return nbr;
}


