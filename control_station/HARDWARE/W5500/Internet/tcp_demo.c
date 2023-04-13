/**
************************************************************************************************
* @file   		tcp_demo.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-02-14
* @brief   		TCP 演示函数
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

uint8 buff[2048];				                              	         /*定义一个2KB的缓存*/

/**
*@brief		TCP Server回环演示函数。
*@param		无
*@return	无
*/
void do_tcp_server(void)
{	
	uint16 len=0;  
//	CPU_SR_ALLOC();
//	OS_CRITICAL_ENTER();//进入临界区
	switch(getSn_SR(SOCK_TCPS))											            	/*获取socket的状态*/
	{
		case SOCK_CLOSED:													                  /*socket处于关闭状态*/
			socket(SOCK_TCPS ,Sn_MR_TCP,local_port,Sn_MR_ND);	        /*打开socket*/
		  break;     
    
		case SOCK_INIT:														                  /*socket已初始化状态*/
			listen(SOCK_TCPS);												                /*socket建立监听*/
		  break;
		
		case SOCK_ESTABLISHED:												              /*socket处于连接建立状态*/
		
			if(getSn_IR(SOCK_TCPS) & Sn_IR_CON)
			{
				setSn_IR(SOCK_TCPS, Sn_IR_CON);								          /*清除接收中断标志位*/
			}
			len=getSn_RX_RSR(SOCK_TCPS);									            /*定义len为已接收数据的长度*/
			if(len>0)
			{
				recv(SOCK_TCPS,buff,len);								              	/*接收来自Client的数据*/
				buff[len]=0x00; 											                  /*添加字符串结束符*/
				printf("%s\r\n",buff);
				send(SOCK_TCPS,buff,len);									              /*向Client发送数据*/
		  }
		  break;
		
		case SOCK_CLOSE_WAIT:												                /*socket处于等待关闭状态*/
			close(SOCK_TCPS);
		  break;
	}
//	OS_CRITICAL_EXIT();	//退出临界区	
}

/**
*@brief		TCP Client回环演示函数。
*@param		无
*@return	无
*/
unsigned int do_tcp_client_tx(unsigned char *TXdata,unsigned int length) 
{	
	unsigned int b=0,i;
	
//	CPU_SR_ALLOC();
//	OS_CRITICAL_ENTER();//进入临界区
		printf("发:");
		for(i=0;i<length;i++)
		{printf("%x,",TXdata[i]);}
		printf("\r\n");
				
	switch(getSn_SR(SOCK_TCPC))								  				         /*获取socket的状态*/
	{
		case SOCK_CLOSED:											        		         /*socket处于关闭状态*/
			socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND);
		  break;
		
		case SOCK_INIT:													        	         /*socket处于初始化状态*/
			connect(SOCK_TCPC,remote_ip,remote_port);                /*socket连接服务器*/ 
		  break;
		
		case SOCK_ESTABLISHED: 												             /*socket处于连接建立状态*/
			if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
			{
				setSn_IR(SOCK_TCPC, Sn_IR_CON); 							         /*清除接收中断标志位*/
			}
			send(SOCK_TCPC,TXdata,length);								     	         /*向Server发送数据*/	
      b=1;			
		  break;
			
		case SOCK_CLOSE_WAIT: 											    	         /*socket处于等待关闭状态*/
			close(SOCK_TCPC);
		  break;
	}
//	OS_CRITICAL_EXIT();	//退出临界区	
	return b;
}
//返回  0：无数据，网络正常。
//      1：有数据，网络正常。
//      2.3：网线拔掉。
//      2.3.5：服务器端口关闭。
unsigned int do_tcp_client_RxStatus(void)
{	
	unsigned char count=0;
  unsigned char nbr=0xFF;
	
//	CPU_SR_ALLOC();	
//	OS_CRITICAL_ENTER();//进入临界区

//	IINCHIP_WRITE(SOCK_TCPC,Sn_CR_SEND_KEEP);                    /*连接服务器,发送一个心跳*/ 	
	
  static unsigned char Snflag=0;
	
//	if(Snflag==0)
//	{
//	IINCHIP_WRITE(Sn_KPALVTR(SOCK_TCPC),0x01);
//	Snflag=1;
//	}
	
	switch(getSn_SR(SOCK_TCPC))								  				         /*获取socket的状态*/
	{	
		case SOCK_CLOSED:											        		         /*socket处于关闭状态*/
//			socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND);
		  nbr=2;
		  break;
		
		case SOCK_INIT:													        	         /*socket处于初始化状态*/
//			connect(SOCK_TCPC,remote_ip,remote_port);                /*socket连接服务器*/ 
			nbr=3;
		  break;
		
		case SOCK_ESTABLISHED: 												             /*socket处于连接建立状态*/
			IINCHIP_WRITE( Sn_CR(SOCK_TCPC) ,Sn_CR_SEND_KEEP); 	    // KeepAlive功能-手动模式
			nbr=0;
			count=getSn_RX_RSR(SOCK_TCPC); 								  	         /*定义len为已接收数据的长度*/
		  if(count!=0) 
		  {
				return 1;
			}
		  break;
		
		case SOCK_CLOSE_WAIT: 											    	         /*socket处于等待关闭状态*/
//			close(SOCK_TCPC);
		  nbr=4;
		  break;
				
		default:	
			nbr=5;
		  break;
	}
//	OS_CRITICAL_EXIT();	//退出临界区	
	return nbr;
}

//返回  0：无数据，网络正常。
//      1：有数据，网络正常。
//      2.3：网线拔掉。
//      2.3.5：服务器端口关闭。
unsigned int do_tcp_client_RxRes(void) 
{	
	unsigned char count=0;
  unsigned char nbr=0;
	
	switch(getSn_SR(SOCK_TCPC))								  				         /*获取socket的状态*/
	{	
		case SOCK_CLOSED:											        		         /*socket处于关闭状态*/
 			socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND);
		  nbr=2;
		  break;
		
		case SOCK_INIT:													        	         /*socket处于初始化状态*/
 			connect(SOCK_TCPC,remote_ip,remote_port);                /*socket连接服务器*/ 
			nbr=3;
		  break;
		
		case SOCK_ESTABLISHED: 												             /*socket处于连接建立状态*/
			count=getSn_RX_RSR(SOCK_TCPC); 								  	         /*定义len为已接收数据的长度*/
		  if(count!=0) 
		  {
				return 1;
			}
		  break;
		
		case SOCK_CLOSE_WAIT: 											    	         /*socket处于等待关闭状态*/
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
//	OS_CRITICAL_ENTER();//进入临界区
	
	switch(getSn_SR(SOCK_TCPC))								  				         /*获取socket的状态*/
	{
		case SOCK_CLOSED:											        		         /*socket处于关闭状态*/
			socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND);
		  break;
		
		case SOCK_INIT:													        	         /*socket处于初始化状态*/
			connect(SOCK_TCPC,remote_ip,remote_port);                /*socket连接服务器*/ 
		  break;
		
		case SOCK_ESTABLISHED: 												             /*socket处于连接建立状态*/
			if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
			{
				setSn_IR(SOCK_TCPC, Sn_IR_CON); 							         /*清除接收中断标志位*/
			}
		
			nbr=getSn_RX_RSR(SOCK_TCPC); 								  	         /*定义len为已接收数据的长度*/
			if((nbr>0)&&(nbr<500))//小于500字节后接收；
			{
				recv(SOCK_TCPC,RXdata,nbr); 							   		         /*接收来自Server的数据*/
				RXdata[nbr]=0x00;  											                 /*添加字符串结束符*/
				
				printf("收:");
				for(i=0;i<nbr;i++)
				{printf("%x_",RXdata[i]);}
				printf("\r\n");
				
				//TEST_DEBUG接收数据打印
//			  UARTx_Send_Data(USART1,RXdata,nbr);
			}	
			else
			{
				recv(SOCK_TCPC,buff,nbr); 							   		         /*接收来自Server的数据*/
				buff[nbr]=0x00;  											                 /*添加字符串结束符*/
//				printf("%s\r\n",buff);				
			}
			
		  break;
			
		case SOCK_CLOSE_WAIT: 											    	         /*socket处于等待关闭状态*/
			close(SOCK_TCPC);
		  break;

	}
//	OS_CRITICAL_EXIT();	//退出临界区	
	return nbr;
}


