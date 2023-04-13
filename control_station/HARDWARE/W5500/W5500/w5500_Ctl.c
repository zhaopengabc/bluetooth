#include <stdio.h> 
#include <string.h>

#include "w5500_ctl.h"
#include "w5500_conf.h"
//#include "bsp_i2c_ee.h"
//#include "bsp_spi_flash.h"
#include "utility.h"
#include "w5500.h"
#include "dhcp.h"
#include "dns.h"
//#include "bsp_TiMbase.h"

void W5500_Init(void)
{
	printf("**************W5500����**************\r\n"); 
	gpio_for_w5500_config();	         	/*��ʼ��MCU�������*/
	reset_w5500();					            /*Ӳ��λW5500*/
	set_w5500_mac();                    /*����MAC��ַ*/
  set_w5500_ip();                     /*����IP��ַ*/	
	
  socket_buf_init(txsize, rxsize);    /*��ʼ��8��Socket�ķ��ͽ��ջ����С*/

	printf("\r\n"); 	
	printf("4.������IP:%d.%d.%d.%d\r\n",remote_ip[0],remote_ip[1],remote_ip[2],remote_ip[3]);
	printf("5.�����˿�:%d \r\n",remote_port);
	printf("**************W5500����**************\r\n"); 
}

//��̬IP��ȡ����������(dns.c�ļ���������)
//���ڽ���˺���
//������Ϻ󣬶��ڻ�ȡIP����������

static unsigned char DHCP_Status=0;//IP��ȡ�ɹ�
static unsigned char DNS_Status=0;//������ȡ�ɹ�

unsigned char W5500_DHCP_DNS(void)
{
	unsigned char b=0;
	
	W5500_Init();
	//DHCP
	if(DHCP_Status==0)//���IP��ȡ�ɹ������ٻ�ȡ��̬IP
	{
     DHCP_Status = get_DHCP();
	}
	
	//DNS
	if(DNS_Status==0)//�������������ȡ�ɹ������ٻ�ȡ��������
	{
     DNS_Status = get_DNS();
	}
	
	if((DHCP_Status==1)&&(DNS_Status==1))
	{b=1;}
	return b;
}



