#include "k2.h" 
#include "string.h"
#include "usart3.h"
#include "includes.h"
#include "sys.h"

//unsigned char ip_address[]="172.16.10.45";	//������ IP��ַ
//unsigned char port_address[]="8010";		//������ �˿ں�
//const char OK[]="+ok";
//const char HEADER[]="+++";

//#define NETK2_RX_DATANUM 50
//#define NETK2_RX_GROUPNUM 3
//typedef struct
//{
//    unsigned char Flag;
//	unsigned int Lenth;
//	unsigned char data[NETK2_RX_DATANUM];
//}NetK2_Data_Typedef;

//NetK2_Data_Typedef NetK2RxBuf[NETK2_RX_GROUPNUM];
//unsigned char NetK2_Wr_Addr=0;
//unsigned char NetK2_Rd_Addr=0;
//OS_TMR NetK2tmr;

//extern OS_TCB RecieveFromServerTaskTCB;

//static void Reset_RxBuf(void)
//{
//	unsigned char i;
//	NetK2_Rd_Addr=0;
//	NetK2_Wr_Addr=0;
//	for(i=0;i<NETK2_RX_GROUPNUM;i++)
//	{
//		NetK2RxBuf[i].Flag=0;
//        NetK2RxBuf[i].Lenth=0;
//        memset(NetK2RxBuf[i].data,0,NETK2_RX_DATANUM);
//	}
//}
//void NetK2tmr_callback(void *p_tmr,void *p_arg)
//{
//	OS_ERR err;
//	NetK2RxBuf[NetK2_Wr_Addr].Flag=1;
//	NetK2_Wr_Addr++;
//	if(NetK2_Wr_Addr>=NETK2_RX_GROUPNUM)NetK2_Wr_Addr=0;
//	if(NetK2_Rd_Addr==NetK2_Wr_Addr)
//	{
//		NetK2_Rd_Addr++;
//		if(NetK2_Rd_Addr>=NETK2_RX_GROUPNUM)NetK2_Rd_Addr=0;
//	}
//	OSTaskSemPost ((OS_TCB	*)&RecieveFromServerTaskTCB,
//                    (OS_OPT	)OS_OPT_POST_NONE,
//                    (OS_ERR *)&err);
//	OSTmrDel((OS_TMR		*)&NetK2tmr_callback,(OS_ERR*	)&err);
//}
//���жϺ����е��ã��Զ��洢
//void NetK2_RxISR(unsigned char buf)
//{
//	OS_ERR err;
//	OS_STATE state;
//    NetK2RxBuf[NetK2_Wr_Addr].Flag=0;
//    if(NetK2RxBuf[NetK2_Wr_Addr].Lenth>=NETK2_RX_DATANUM-1) NetK2RxBuf[NetK2_Wr_Addr].Lenth=0;
//    NetK2RxBuf[NetK2_Wr_Addr].data[NetK2RxBuf[NetK2_Wr_Addr].Lenth++] = buf;
//	
//	state=OSTmrStateGet((OS_TMR		*)&NetK2tmr,(OS_ERR*	)&err);
//	if(state!=OS_TMR_STATE_UNUSED)
//	{
//		OSTmrDel((OS_TMR		*)&NetK2tmr,(OS_ERR*	)&err);
//	}
//	OSTmrCreate((OS_TMR		*)&NetK2tmr,
//				(CPU_CHAR	*)"NetK2tmr",
//				(OS_TICK	)2,
//				(OS_TICK	)0,
//				(OS_OPT		)OS_OPT_TMR_ONE_SHOT,
//				(OS_TMR_CALLBACK_PTR)NetK2tmr_callback,
//				(void*	)0,
//				(OS_ERR*	)&err);
//	OSTmrStart((OS_TMR		*)&NetK2tmr,(OS_ERR*	)&err);
//}
//unsigned int NetK2_Rx_Read(unsigned char *Rxdata)
//{
//	unsigned int tmp=0;
//	if(NetK2RxBuf[NetK2_Rd_Addr].Flag==1)
//    {
//		//��ȡ����
//		tmp=NetK2RxBuf[NetK2_Rd_Addr].Lenth;
//		memcpy(Rxdata,NetK2RxBuf[NetK2_Rd_Addr].data,NetK2RxBuf[NetK2_Rd_Addr].Lenth);
//        NetK2RxBuf[NetK2_Rd_Addr].Flag=0;
//        NetK2RxBuf[NetK2_Rd_Addr].Lenth=0;
//        memset(NetK2RxBuf[NetK2_Rd_Addr].data,0,NETK2_RX_DATANUM);
//        NetK2_Rd_Addr++;
//        if(NetK2_Rd_Addr>=NETK2_RX_GROUPNUM)NetK2_Rd_Addr=0;
//    }
//	return tmp;
//}
//K2 IO��ʼ��
//PD14-LINKA  PA1-RST
//void NetK2_GPIO_Init(void)
//{
//	//GPIOE2��ʼ������
//	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE, ENABLE);

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	K2_NET_Rst=0;
//}
//void NetK2_Init(void)
//{
////	Reset_RxBuf();
////	NetK2_GPIO_Init();
////	K2_NET_Rst=1;
////	Usart3_init(115200,USART_Parity_No,USART_StopBits_1);
////	pUsart3Rx_Isr=NetK2_RxISR;
//}
//unsigned int NetK2_Send_Infor(unsigned char *buf,unsigned int length)
//{
//	USART3_Send_Data((unsigned char *)buf,length);
//	return 1;
//}
//��ȡk2����״̬
//����ֵ1���޷������Ϸ�����������ֵ0�����ӷ���������
//unsigned char NetK2_Work(void)
//{
//	return (K2_NET_Link);
//}
//�Ƚ������ַ�����Ӧ�����Ƿ����
//���Ϊ1������Ϊ0
//static unsigned char IsStrEqual(const char *dest,const char *source,unsigned char lenth)
//{
//	unsigned char i=0;
//	unsigned char flag=1;
//	for(i=0;i<lenth;i++)
//	{
//		if(*dest++!=*source++)
//		{
//			flag=0;
//			break;
//		}
//	}
//	return flag;
//}
//unsigned char NetK2_Restart(void)
//{
//	OS_ERR err;
//	unsigned int i=0;
//	if(K2_NET_Link==1)
//	{
//		K2_NET_Rst=0;
//		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
//		K2_NET_Rst=1;
//		for(i=0;i<120;i++)
//		{
//			if(K2_NET_Link==0)return 1;
//			OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
//		}
//	}
//	return 0;
//}
//�ȽϽ��յ��������Ƿ����+OK;
//���Ϊ1������Ϊ0
//unsigned char CmpRecieveDataIsStr(char *str,unsigned char strlen)
//{
//	unsigned char RxBuf[50];
//	unsigned int lenth=0;
//	unsigned int i=0;
//	unsigned char flag=0;
//	memset(RxBuf,0x00,sizeof(RxBuf));
////	lenth=NetK2_Rx_Read(RxBuf);
//	if((lenth!=0)&&(lenth<=sizeof(RxBuf)))
//	{
//		for(i=0;i+strlen-1<lenth;i++)		//���Ǵ��ֽ�0��ʼ�Ƚϣ���ֹ���Ϳո�
//		{
//			if(IsStrEqual((char *)&RxBuf[i],str,strlen))
//			{
//				flag=1;
//			}
//		}
//	}
//	return flag;
//}
//����AT����ģʽ
//Ϊ1�ѽ�������ģʽ�ɹ���Ϊ0δ��������ģʽ��ͨѶģʽ
//unsigned char NetK2_EnterCommandMode(void)
//{
//	OS_ERR err;
//	unsigned char flag=0;
//	unsigned char m=0;
//	for(m=0;m<3;m++)
//	{
//		NetK2_Send_Infor((unsigned char *)HEADER,3);
//		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);//������50ms�㹻
//		flag=CmpRecieveDataIsStr((char*)"a",1);
//		if(flag==1)
//		{
//			NetK2_Send_Infor((unsigned char *)"a",1);
//			OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
//			flag=CmpRecieveDataIsStr((char *)"+ok",3);
//			if(flag==1) return 1;
//		}
//	}
//	return 0;
//}
//unsigned char NetK2_ExitCommandMode(void)
//{
//	OS_ERR err;
//	unsigned char flag=0;
//	unsigned char m=0;
//	for(m=0;m<3;m++)
//	{
//		NetK2_Send_Infor((unsigned char *)"AT+ENTM\r\n",8);
//		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
//		flag=CmpRecieveDataIsStr((char *)"+OK",3);
//		if(flag==1) return 1;
//	}
//	return 0;
//}
//д���������ַ��˿ں�
//Ϊ0д��ʧ�ܣ�Ϊ1д��ɹ�
//unsigned char NetK2_SetServerAddress(unsigned char *ip,unsigned char *port)
//{
//	OS_ERR err;
//	unsigned char buf[100];
//	unsigned char lenth;
//	unsigned char flag=0;
//	lenth=14+sizeof(ip)+sizeof(port);
//	if(lenth<sizeof(buf))
//	{
//		memset(buf,0,sizeof(buf));
////		lenth=sprintf((char*)buf,"AT+SOCK=TCPC,%s,%s\r\n",ip,port);
//		NetK2_Send_Infor(buf,lenth);
//		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
//		flag=CmpRecieveDataIsStr((char *)"+OK",3);
//		if(flag==1)return 1;
//	}
//	return 0;
//}
//����Ϊ��̬��ȡIP
//AT+WANN��ѯWAN��״̬��AT+WANN=IP/DHCP����WAN��״̬

//����ģ����û���������
//AT+WEBU��ȡ�û��������룻AT+WEBU=admin,admin�����û���������

//��ѯ����Э�飬������
//AT+SOCK=TCPC,192.168.0.201,8234

//��ѯ�����ñ��ض˿ں�
// AT+SOCKPORT=20108��

//��ѯLINK���ܣ�������
//AT+ SCSLINK=ON

//��ѯ���������ʧ��RESET��������
//AT+CLIENTRST =ON

//����ǰ�Ƿ���ջ���
//AT+UARTCLBUF=ON


//void NetK2_Test(void)
//{
//	OS_ERR err;
//	unsigned char flag=0;
//	unsigned char buf[]="BeiJingFuBbang";
//	flag=NetK2_EnterCommandMode();
//	if(flag!=0)
//	{
//		flag=NetK2_SetServerAddress(ip_address,port_address);
//		if(flag==1)
//		{
//		
//		}
//	}
//	flag=NetK2_ExitCommandMode();
//	while(1)
//	{
//		OSTimeDlyHMSM(0,0,5,0,OS_OPT_TIME_HMSM_STRICT,&err);
//		if(NetK2_Work()!=0)
//		{
//			NetK2_Send_Infor(buf,sizeof(buf));
//		}
//		else
//		{
//			
//		}
//	}
//}


