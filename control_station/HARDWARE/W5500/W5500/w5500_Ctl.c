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
	printf("**************W5500配置**************\r\n"); 
	gpio_for_w5500_config();	         	/*初始化MCU相关引脚*/
	reset_w5500();					            /*硬复位W5500*/
	set_w5500_mac();                    /*配置MAC地址*/
  set_w5500_ip();                     /*配置IP地址*/	
	
  socket_buf_init(txsize, rxsize);    /*初始化8个Socket的发送接收缓存大小*/

	printf("\r\n"); 	
	printf("4.服务器IP:%d.%d.%d.%d\r\n",remote_ip[0],remote_ip[1],remote_ip[2],remote_ip[3]);
	printf("5.监听端口:%d \r\n",remote_port);
	printf("**************W5500配置**************\r\n"); 
}

//动态IP获取，域名解析(dns.c文件配置域名)
//周期进入此函数
//网络故障后，定期获取IP和域名解析

static unsigned char DHCP_Status=0;//IP获取成功
static unsigned char DNS_Status=0;//域名获取成功

unsigned char W5500_DHCP_DNS(void)
{
	unsigned char b=0;
	
	W5500_Init();
	//DHCP
	if(DHCP_Status==0)//如果IP获取成功，则不再获取动态IP
	{
     DHCP_Status = get_DHCP();
	}
	
	//DNS
	if(DNS_Status==0)//如果域名解析获取成功，则不再获取域名解析
	{
     DNS_Status = get_DNS();
	}
	
	if((DHCP_Status==1)&&(DNS_Status==1))
	{b=1;}
	return b;
}



