#include "stm32f10x.h"
#include "usart2.h"
#include "includes.h"
#include "gprs.h"
#include "os.h"

//#define GPRS_Baud 115200UL

////#define GPRS_ST_Read() GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)
////#define GPRS_LINKA_Read() GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6)

////#define GPRS_RESET_LO() GPIO_ResetBits(GPIOC,GPIO_Pin_8);
////#define GPRS_RESET_HI() GPIO_SetBits(GPIOC,GPIO_Pin_8);

////#define GPRS_PWR_LO() GPIO_ResetBits(GPIOD,GPIO_Pin_13);
////#define GPRS_PWR_HI() GPIO_SetBits(GPIOD,GPIO_Pin_13);




////#define GPRS_RX_DATANUM 250
////#define GPRS_RX_DATANUM 15
////#define GPRS_RX_GROUPNUM 3

////typedef struct
////{
////    unsigned char Flag;
////	unsigned int Lenth;
////	unsigned char data[GPRS_RX_DATANUM];
////}Gprs_Data_Typedef;
////Gprs_Data_Typedef GprsRxBuf[GPRS_RX_GROUPNUM];
////static unsigned char Gprs_Rx_Cnt=TASK_GPRS_CNT;
////unsigned char Gprs_Wr_Addr=0;
////unsigned char Gprs_Rd_Addr=0;
////unsigned char sendata_flag=0;
////OS_TMR Gprstmr;
////extern OS_TCB RecieveFromServerTaskTCB;

////u8 GPRS_LINKA_Read(void)  
////{
////	 return 0;
////} 
//// 
////void GPRS_RESET_LO(void)  
////{ 
////} 

////void GPRS_RESET_HI(void)  
////{ 
////} 

////void GPRS_PWR_LO(void)  
////{ 
////}

////void GPRS_PWR_HI(void)  
////{
////}

////static void Reset_RxBuf(void)
////{
////	unsigned char i;
////	Gprs_Rd_Addr=0;
////	Gprs_Wr_Addr=0;
////	for(i=0;i<GPRS_RX_GROUPNUM;i++)
////	{
////		GprsRxBuf[i].Flag=0;
////        GprsRxBuf[i].Lenth=0;
////        memset(GprsRxBuf[i].data,0,GPRS_RX_DATANUM);
////	}
////}
////void Gprstmr_callback(void *p_tmr,void *p_arg)
////{
////	OS_ERR err;
////	GprsRxBuf[Gprs_Wr_Addr].Flag=1;
////	Gprs_Wr_Addr++;
////	if(Gprs_Wr_Addr>=GPRS_RX_GROUPNUM)Gprs_Wr_Addr=0;
////	if(Gprs_Rd_Addr==Gprs_Wr_Addr)
////	{
////		Gprs_Rd_Addr++;
////		if(Gprs_Rd_Addr>=GPRS_RX_GROUPNUM)Gprs_Rd_Addr=0;
////	}
////	OSTaskSemPost ((OS_TCB	*)&RecieveFromServerTaskTCB,
////                    (OS_OPT	)OS_OPT_POST_NONE,
////                    (OS_ERR *)&err);
////	OSTmrDel((OS_TMR		*)&Gprstmr_callback,(OS_ERR*	)&err);
////}
////void Gprs_RxISR(unsigned char buf)
////{
////	
////	OS_ERR err;
////	OS_STATE state;
////	
////    GprsRxBuf[Gprs_Wr_Addr].Flag=0;
////    if(GprsRxBuf[Gprs_Wr_Addr].Lenth>=GPRS_RX_DATANUM-1) GprsRxBuf[Gprs_Wr_Addr].Lenth=0;
////    GprsRxBuf[Gprs_Wr_Addr].data[GprsRxBuf[Gprs_Wr_Addr].Lenth++] = buf;

////	state=OSTmrStateGet((OS_TMR		*)&Gprstmr,(OS_ERR*	)&err);
////	if(state!=OS_TMR_STATE_UNUSED)
////	{
////		OSTmrDel((OS_TMR		*)&Gprstmr,(OS_ERR*	)&err);
////	}
////	OSTmrCreate((OS_TMR		*)&Gprstmr,
////				(CPU_CHAR	*)"Gprstmr",
////				(OS_TICK	)2,
////				(OS_TICK	)0,
////				(OS_OPT		)OS_OPT_TMR_ONE_SHOT,
////				(OS_TMR_CALLBACK_PTR)Gprstmr_callback,
////				(void*	)0,
////				(OS_ERR*	)&err);
////	OSTmrStart((OS_TMR		*)&Gprstmr,(OS_ERR*	)&err);
////}
////unsigned int Gprs_Rx_Read(unsigned char *Rxdata)
////{
////	unsigned int tmp=0;
////	if(GprsRxBuf[Gprs_Rd_Addr].Flag==1)
////    {
////		//��ȡ����
////		tmp=GprsRxBuf[Gprs_Rd_Addr].Lenth;
////		memcpy(Rxdata,GprsRxBuf[Gprs_Rd_Addr].data,GprsRxBuf[Gprs_Rd_Addr].Lenth);
////        GprsRxBuf[Gprs_Rd_Addr].Flag=0;
////        GprsRxBuf[Gprs_Rd_Addr].Lenth=0;
////        memset(GprsRxBuf[Gprs_Rd_Addr].data,0,GPRS_RX_DATANUM);
////        Gprs_Rd_Addr++;
////        if(Gprs_Rd_Addr>=GPRS_RX_GROUPNUM)Gprs_Rd_Addr=0;
////    }
////	return tmp;
////}

//void GPRS_GPIO_Init(void)
//{
////	GPIO_InitTypeDef GPIO_InitStructure;
////	
////	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
////	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
////	
////	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				//PWR
////	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
////	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
////	GPIO_Init(GPIOD, &GPIO_InitStructure);
////	
////	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				//Reset
////	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
////	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
////	GPIO_Init(GPIOC, &GPIO_InitStructure);
////	
////	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				//LINKB
////	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
////	GPIO_Init(GPIOD, &GPIO_InitStructure);
////	
////	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				//LINKA
////	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
////	GPIO_Init(GPIOC, &GPIO_InitStructure);

////	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				//ST
////	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
////	GPIO_Init(GPIOC, &GPIO_InitStructure);
////	
////	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				//WAKEUP
////	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
////	GPIO_Init(GPIOC, &GPIO_InitStructure);
////	
////	GPRS_PWR_LO();
////	GPRS_RESET_LO();
//}
//void GPRS_Init(void)
//{
////	unsigned char buf[0x20];
////	memset(buf,0,0x20);
////	Read_Config(buf,GPRS_SN_CONFIG_CMD);
////	memcpy(SN_ADDRESS, buf, 16);
////	memset(buf,0,0x20);
////	Read_Config(buf,GPRS_IMEI_CONFIG_CMD);
////	memcpy(IMEI_ADDRESS, buf, 16);
////	memset(buf,0,0x20);
////	Read_Config(buf,GPRS_PHONE_CONFIG_CMD);
////	memcpy(PHONE_ADDRESS, buf, 14);
//	
////	Reset_RxBuf();
////	GPRS_GPIO_Init();
////	GPRS_PWR_HI();
////	GPRS_RESET_HI();
////	Usart2_init(GPRS_Baud,USART_Parity_No);
//////	DMA_Uart_Init(DMA1_GM3);
////	pUsart2Rx_Isr=Gprs_RxISR;
//}
////�������������Ϣ
////���ͳɹ�����ҵ����ˮ�ţ�����ʧ�ܷ���0��
//unsigned int GPRS_Send_Infor(unsigned char *buf,unsigned int length)
//{
//	USART2_Send_Data((unsigned char *)buf,length);
//	return 1;
//}
////ÿ200ms���뺯��һ��
////ģ���ѹ����λ������200ms֮������ά��2min���������ɹ����˳������ɹ����ٴ�ִ��
////�����ɹ�����1��
////�������ɹ�����0��
////unsigned char GPRS_Restart(void)
////{
////	OS_ERR err;
////	unsigned int i=0;
////	if(GPRS_LINKA_Read()==0)
////	{
////		GPRS_RESET_LO();
////		GPRS_PWR_LO();
////		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
////		GPRS_PWR_HI();
////		GPRS_RESET_HI();
////		for(i=0;i<120;i++)
////		{
////			if(GPRS_LINKA_Read()==1)return 1;
////			OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
////		}
////	}
////	return 0;
////}
////GM3ģ�鹤������Լ�
////���ӷ�������������0������������1������ʱ����������ʱ����Ϊ2min
////unsigned char GPRS_Work(void)
////{
////	unsigned char flag=1;
////	if(GPRS_LINKA_Read()==1)flag=0;
////	return flag;
////}

////#define SN_HEADER_LEN 7
////#define IMEI_HEADER_LEN 7
////static unsigned char SN_ADDRESS[16];
////static unsigned char IMEI_ADDRESS[16];
////static unsigned char PHONE_ADDRESS[14];
////static unsigned char SN_Imei_Flag=0;//SN��IMei���ȡ�ɹ���־�ֽ�

////unsigned char Request_SN[]="fbiot#AT+SN\r\n";
////unsigned char Request_IMEI[]="fbiot#AT+IMEI\r\n";
////unsigned char Request_CNUM[]="fbiot#AT+CNUM?\r\n";
////const unsigned char SN_HEADER[SN_HEADER_LEN]="+SN:SN";
////const unsigned char IMEI_HEADER[IMEI_HEADER_LEN]="+IMEI:";

//////�Ƚ������ַ�����Ӧ�����Ƿ����
//////���Ϊ1������Ϊ0
////static unsigned char IsStrEqual(const char *dest,const char *source,unsigned char lenth)
////{
////	unsigned char i=0;
////	unsigned char flag=1;
////	for(i=0;i<lenth;i++)
////	{
////		if(*dest++!=*source++)
////		{
////			flag=0;
////			break;
////		}
////	}
////	return flag;
////}
//////GM3�����������ѯ
//////���ͳɹ�����1������ʧ�ܷ���0��
////unsigned char GM3_Set_data(unsigned char *data,unsigned int lenth)
////{
////	USART2_Send_Data((unsigned char *)data,lenth);
////	return 0;
////}
//////��ѯ������Ϣ
////void query_config(void)
////{
////	static unsigned int cnt=0;
////	if(!(SN_ADDRESS[15]&&IMEI_ADDRESS[15]))
////	{
////		cnt++;
////		if((IMEI_ADDRESS[15]!=1)&&(cnt%3==0))
////		{
////			GM3_Set_data(Request_IMEI,sizeof(Request_IMEI)-1);
////		}
////		else if((SN_ADDRESS[15]!=1)&&(cnt%3==1))
////		{
////			GM3_Set_data(Request_SN,sizeof(Request_SN)-1);
////		}
//////		if((PHONE_ADDRESS[13]!=1)&&(cnt%3==2)) ;
////		SN_Imei_Flag=0;
////	}
////	else
////	{
////		SN_Imei_Flag=1;
////	}
////}
//////�ж��Ƿ�ΪGM3ģ�������������ֱ�ӹ��ˣ��������������
//////��ڲ������������Rxdata������lenth���Ƿ���˵�GM3ģ�����Ϣ��Ϊ1���ˣ�Ϊ0��������
////unsigned int GPRS_Pro(unsigned char *Rxdata,unsigned int lenth,unsigned char filter_state)
////{
////	unsigned int i=0;
////	if(SN_Imei_Flag == 0)
////    {
////		for(i=0;i+13<lenth;i++)
////		{
////			if(IsStrEqual((char *)&Rxdata[i],(char *)SN_HEADER,SN_HEADER_LEN-1))
////			{
////				memcpy(SN_ADDRESS, &Rxdata[i+SN_HEADER_LEN-1], 15);
////				SN_ADDRESS[15]=1;
//////				Write_Config(SN_ADDRESS,GPRS_SN_CONFIG_CMD);
////				if(filter_state!=0)
////				{
////					memset(Rxdata,0,lenth);
////					lenth=0;
////				}
////				break;
////			}
////			else if(IsStrEqual((char *)&Rxdata[i],(char *)IMEI_HEADER,IMEI_HEADER_LEN-1))
////			{
////				memcpy(IMEI_ADDRESS, &Rxdata[i+IMEI_HEADER_LEN-1], 15);
////				IMEI_ADDRESS[15]=1;
//////				Write_Config(IMEI_ADDRESS,GPRS_IMEI_CONFIG_CMD);
////				if(filter_state!=0)
////				{
////					memset(Rxdata,0,lenth);
////					lenth=0;
////				}
////				break;
////			}
////		}
////    }
////	return lenth;
////}
/////***********************end of file**************************/



//////SIM800C��������
//////cmd:���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//////ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//////waittime:�ȴ�ʱ��(��λ:10ms)
//////����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//////       1,����ʧ��
//////unsigned char gm3_send_cmd(unsigned char *cmd,unsigned char *ack,unsigned int waittime)
//////{
//////	unsigned char res=0; 
//////	unsigned int i=0;
//////	unsigned char t=0;	 
//////	unsigned char data[20]={0};		
//////	char strx=0;		
//////	OS_ERR err;		

//////	for(i=0;i<50;i++)//���buff����
//////	{
//////		usart2_receive_data[i]=0;
//////	}
//////	usart2_receive_data_inc=0;//�������
//////	if(sendata_flag==1)
//////	{
//////		sendata_flag=0;
//////		GM3_Send_Data(cmd);//��������\r\n		
//////	}
//////	else
//////	{
//////		GM3_Send_Data(cmd);//��������		
//////		
//////		data[0]=0x0D;
//////		data[1]=0x0A;		
//////		data[2]=0;	
//////		GM3_Send_Data(cmd);
//////	}
//////	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
//////	{
//////		OSTimeDlyHMSM(0,0,0,waittime,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s	
////////		printf("%s",USART2_RXD);//�����ã����ղ��յ������ݴ�ӡ������		
//////		if(gm3_check_cmd(ack))
//////		{
//////			res=1;
//////		}
//////		else
//////		{
//////			res=0;						
//////		}
//////	}
//////	return res;
//////} 


////unsigned char gm3_send_cmd(unsigned char *cmd,unsigned char *ack,unsigned int waittime)
////{
////	unsigned char res=0; 
////	unsigned int i=0;
////	unsigned char t=0;	 
////	unsigned char data[20]={0};		
////	char strx=0;		
////	OS_ERR err;		
////	for(i=0;i<50;i++)//���buff����
////	{
////		usart2_receive_data[i]=0;
////	}
////	usart2_receive_data_inc=0;//�������
////	if(sendata_flag==1)
////	{
////		sendata_flag=0;
////		GM3_Send_Data(cmd);//��������\r\n		
////	}
////	else
////	{
////		GM3_Send_Data(cmd);//��������		
////		data[0]=0x0D;
////		data[1]=0x0A;		
////		data[2]=0;	
////		GM3_Send_Data(data);
////	}
////	OSTimeDlyHMSM(0,0,1,1,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s		
////	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
////	{
////		waittime=waittime-1;
////		OSTimeDlyHMSM(0,0,0,1,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s	
//////		printf("%s",USART2_RXD);//�����ã����ղ��յ������ݴ�ӡ������		
////		if(gm3_check_cmd(ack))
////		{
////			res=1;
////		}
////		else
////		{
////			res=0;						
////		}
////	}
////	return res;
////} 







//////SIM800C���������,�����յ���Ӧ��
//////str:�ڴ���Ӧ����
//////����ֵ:0,û�еõ��ڴ���Ӧ����
//////����,�ڴ�Ӧ������λ��(str��λ��)
////unsigned char* gm3_check_cmd(unsigned char *str)
////{
////	char *strx=0;
////	strx=strstr((const char*)usart2_receive_data,(const char*)str);
////	return (unsigned char*)strx;
////}















