#include "bc95.h"

/*******************************************************************************
*                       BC35ģ������
********************************************************************************
*��˾������Ƽ�
*ʱ�䣺2018-05-20
*******************************************************************************/
//Ӳ��˵����
// GPRSģ���Դ
// GPRS_CRTL-----PB0    1 �ߵ�ƽʹ�ܣ���Դ����  0 �͵�ƽ��ֹ����Դ�ر�
// ��Դ����
// GPRS_PWR------PB4   ���ػ����ߵ�ƽʹ�ܿ������͵�ƽ��ֹ
// ��λ
// IO_1---GPRS_RESET----PD5   ģ�鸴λ���ߵ�ƽ��Ч���͵�ƽ��ֹ
// ģ�����ͼ��
// CS------------        NC���գ�δʹ�ã�
//--------------------------------------------------------------------------------//
//�޸ļ�¼
//1����COAP_EN�궨��ɾ��
//2������ͨ��Э��ΪGB26875.3
//3��

/***********************ͷ�ļ�***************************************************/
//#include "stm8l15x.h"
//#include "gprs.h"
//#include "usart2.h"
#include "usart.h"
#include <string.h>
#include "rtc.h"
//#include "common.h"
#include "stm32f10x_gpio.h"
#include "delay.h"
#include "os.h"

#include "includes.h"
#include "UsartMsgProcessAppExt.h"
/*********************��������*******************************************************/
const char ATOK[]="OK\r\n";
const char AT_ERROR[]="ERROR\r\n";
const char ATCCLK[]="+CCLK:";
const char ATCSQ[]="+CSQ:";
const char ATCGATT[]="+CGATT:";
const char ATCPSMS[]="+CPSMS";
const char ATCPSMS0[]="+CPSMS:0";
const char ATCGSN[]="+CGSN:";
const char ATCEREG[]="+CEREG:";
const char ATNBAND[]="+NBAND:";
const char ATNSMI[]="+NSMI";
const char ATNNMI[]="+NNMI";
const char ATCGPADDR[]="+CGPADDR:";
const char AT_CLOSED[]="CLOSE";
const char AT_CGATT1[]="+CGATT:1";
const char AT_CEREG1[]="+CEREG:1,1";
const char AT_CEREG5[]="+CEREG:1,5";

const char AT_CEREG01[]="+CEREG:0,1";
const char AT_CEREG05[]="+CEREG:0,5";

AT_link_ENUM NB_Link_step=NBLINK;
u8 NB_BAND=0;
usart2_buf uart_gprs;
unsigned int NBbuflenth=0;

volatile unsigned char gprs_shutdown_flag=1;		//�ػ���־��Ϊ0����״̬��Ϊ1�ػ�״̬��Ϊ2��λ״̬
volatile unsigned char SendSuccessedFlag=0;				//���ݷ��ͳɹ���־��Ϊ0����ʧ�ܣ�Ϊ1���ͳɹ�
unsigned char GprsStopSendFlag;

volatile unsigned char Signal_Value=99;   //�洢�ź�ǿ��
static unsigned char IMEI[15];          //�洢IMEI
static unsigned char IMSI[15];          //�洢IMSI
//extern usart2_buf uart_gprs;
unsigned char Data_Frame[512];
unsigned char temp_Buffer[RX_BUF_LEN];

u8 MNC=0;
//Ƶ��������־�����ź�����������������������Ƶ�㣻netlockflag��1��
//�豸������־���㣻
static unsigned char NetLockFlag = 0;
//�豸�ϵ�������Ƶ�㴦���־;�豸�ϵ�������ʱ��־λ��λ���жϱ�־λ����λ�����Ƶ�㴦��
static unsigned char NetPowerFlag = 0;
//�豸Ƶ��洢�������ϵ��ȡƵ�㣬���洢��֮������Ҫ���������μ�һ����
static unsigned char NetNum=0;//0:�����ϵ磬���ж�ȡ1��B3��:2:B5,3:B8


usart1_buf uart_debug;
usart2_buf uart_gprs;
usart3_buf uart_rs485;


volatile unsigned char	NB_RegistFlag=1;//NBע���־λ  1ע��ʧ�ܣ�0ע��ɹ���
volatile unsigned char	W5_RegistFlag=0;//NBע���־λ  1ע��ʧ�ܣ�0ע��ɹ���
extern unsigned char MIM_flag;
/*********************��������******************************************************/
void USART4_sendbuf(u8 *str,u32 length)
{
    u32 xi=0;
    for(xi=0;xi<length;xi++)
    {
        USART_SendData(UART4,*(str+xi));
						  if(MIM_flag==1)
							{
					 		USART_SendData(USART1,*(str+xi));	//test
							}
		while(USART_GetFlagStatus(UART4, USART_FLAG_TXE)==RESET);
        #ifdef  IWDG_FUNCTION_EN
	    IWDG_ReloadCounter();
        #endif
    }
    while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET);
}

/*****************************************************************************
* ��������: void UartRxISR(stUart *st,char c)
* ��������: �����ַ�ָ����д���
* ��    ��:
* ����  ֵ:
*****************************************************************************/
static	void UartRxISR(stUart *st,char c)
{
    OS_ERR err;
    if(st->pRxWr<=st->RxBuf) st->pRxWr = st->RxBuf;
    else if(st->pRxWr - st->RxBuf >= sizeof(st->RxBuf)-1) st->pRxWr = st->RxBuf;
  	*(st->pRxWr) = c;
        st->pRxWr++;                                                           
	
		if(st->pRxWr - st->RxBuf >= sizeof(st->RxBuf)-1)
				st->pRxWr = st->RxBuf;
        st->last_receive=OSTimeGet(&err);
}

usart2_buf uart_gprs;
void UART4_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		/*Res =USART_ReceiveData(USART2);	//��ȡ���յ�������
		receive2[j2]=Res;
		j2++;*/
        Res =	USART_ReceiveData(UART4);
	      UartRxISR(&uart_gprs, Res);
			  if(MIM_flag==1)
 	 		  {
				USART_SendData(USART1, Res);
				}//test
    }
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 
/*****************************************************************************
* ��������: void GPRS_IO_Init()
* ��������: GPRS����IO��ʼ��
* ��    ��:
* ����  ֵ:
*****************************************************************************/
void GPRS_IO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //GPIO_Init(PORT_GPRS_RESET , PIN_GPRS_RESET , GPIO_Mode_Out_PP_Low_Slow);
    //GPIO_Init(PORT_GPRS_MP_EN , PIN_GPRS_MP_EN , GPIO_Mode_Out_PP_Low_Slow);
    //GPIO_Init(PORT_GPRS_PWR , PIN_GPRS_PWR , GPIO_Mode_Out_PP_Low_Slow);
    
    GPIO_InitStructure.GPIO_Pin = PIN_GPRS_RESET; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//�����������
    GPIO_Init(PORT_GPRS_RESET, &GPIO_InitStructure);//��ʼ��GPIOA.9
    
//    GPIO_InitStructure.GPIO_Pin = PIN_GPRS_MP_EN; //PA.9
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//�����������
//    GPIO_Init(PORT_GPRS_MP_EN, &GPIO_InitStructure);//��ʼ��GPIOA.9
//    
//    GPIO_InitStructure.GPIO_Pin = PIN_GPRS_PWR; //PA.9
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//�����������
//    GPIO_Init(PORT_GPRS_PWR, &GPIO_InitStructure);//��ʼ��GPIOA.9
    
    
	//VGPRS_MP_EN_LOW;
	//GPRS_PWR_LOW;
	GPRS_RESET_LOW;
}
//Usart���Ŀ���
void NB_UsartMode(unsigned char state)
{
    if(state == 0)
    {
        //����NB_Usart
//        USART_DeInit(USART1);
     /*   USART_Cmd(USART1 , DISABLE);
        CLK_PeripheralClockConfig(USART1_CLK , DISABLE); 
        GPIO_Init(PORT_USART1_RX , PIN_USART1_RX , GPIO_Mode_Out_PP_Low_Fast);
        GPIO_Init(PORT_USART1_TX , PIN_USART1_TX , GPIO_Mode_Out_PP_Low_Fast);
        GPIO_ResetBits(PORT_USART1_RX , PIN_USART1_RX );
        GPIO_ResetBits(PORT_USART1_TX , PIN_USART1_TX );
    }
    else
    {
        //nbģ�鳤���磬IO�����ߣ��͹���ģʽ
        USART_Cmd(USART1 , DISABLE);
        CLK_PeripheralClockConfig(USART1_CLK , ENABLE); 
        GPIO_Init(PORT_USART1_RX , PIN_USART1_RX , GPIO_Mode_Out_PP_Low_Fast);
        GPIO_Init(PORT_USART1_TX , PIN_USART1_TX , GPIO_Mode_Out_PP_Low_Fast);
        GPIO_SetBits(PORT_USART1_RX , PIN_USART1_RX );
        GPIO_SetBits(PORT_USART1_TX , PIN_USART1_TX );*/
    }
}
//���ڳ�ʼ��
void NB_UsartInit(unsigned long baudrate)
{
    //USART2_Init(9600);//Uart1_Init(baudrate);
}
void NB_UsartSendstr(unsigned char *data , unsigned int strlen)
{
   // USART2_SEND(data,strlen);
	 USART4_sendbuf(data,strlen);
}

void MP1482_POWERON(void)
{
// VGPRS_MP_EN_HIGH;
#if	NB_PRINTF
  USART1_sendstr("MP1482_On!!!\r\n");
#endif
}
void MP1482_POWEROFF(void)
{
//	VGPRS_MP_EN_LOW;
	delay_ms(100);
#if	NB_PRINTF
	USART1_sendstr("MP1482_Off!!!\r\n");
#endif
}
/*****************************************************************************
* ��������: void VGPRS_EN(unsigned char i)
* ��������: GPRSģ���Դ����
* ��    ��:
* ����  ֵ:
*****************************************************************************/
void VGPRS_EN(unsigned char i)
{
	if (i == 1)
	{
        MP1482_POWERON();
		delay_ms(1);
		//GPRS_PWR_HIGH;
		delay_ms(100);
		ResetUartBuf(&uart_gprs);
		GPRS_RESET_LOW;
		gprs_shutdown_flag=0;
		SendSuccessedFlag=0;
	}
	else if (i == 0)
	{
        MP1482_POWEROFF();
		delay_ms(100);
		//GPRS_PWR_LOW;
		delay_ms(100);
		if(NB_Link_step>=NB_CIMI)
		{
			NB_Link_step=NB_CIMI;
		}
		else
		{
			NB_Link_step = NBLINK;
		}
		gprs_shutdown_flag=1;
	}
}
/*****************************************************************************
* ��������: unsigned char OpenGPRS(void)
* ��������: GPRS������⺯��
* ��    ��: 1��׼��  0��δ׼����
* ����  ֵ:
*****************************************************************************/
u8 OpenGPRS_check(void)//unsigned char OpenGPRS(void)
{
	VGPRS_EN(1);						// GM3�ϵ�
	NB_UsartInit(9600);
    delay_ms(5000);
	return 1;   						// �����ɹ�
}
/*****************************************************************************
* ��������: void reset_gprs(void)
* ��������: ��ѯGM3���ò���SN IMEI Phone����Flash��
* ��    ��: 
* ����  ֵ:
*****************************************************************************/
void reset_gprs(void)
{
    static unsigned int reset_cnt=0;
    static unsigned int Net_cnt = 0;
	if((NetLockFlag == 0)&&(NB_Link_step >= NB_CSQ))
	{
		Net_cnt++;
		if(Net_cnt >= 3)
		{
			Net_cnt=0;
			NetPowerFlag = 1;
			reset_cnt = 6;
		}
	}
	else
	{
		Net_cnt=0;
	}
	reset_cnt++;
	if(reset_cnt>5)
	{
		reset_cnt=0;
		VGPRS_EN(0);
		delay_ms(200);
		VGPRS_EN(1);
		delay_ms(10);
#if	NB_PRINTF
		USART1_sendstr("BC95 Restart!!!");
#endif
	}
	else
	{
		delay_ms(10);
		GPRS_RESET_HIGH;
		delay_ms(200);
		GPRS_RESET_LOW;
		delay_ms(10);
        gprs_shutdown_flag=2;
#if	NB_PRINTF
		USART1_sendstr("BC95 Reset!!!");
#endif
	}
	if((NB_Link_step>=NB_CIMI)&&(reset_cnt!=0))
	{
		NB_Link_step=NB_CGSN;//NB_CIMI;//NB_NBAND;//NB_CIMI;
	}
	else
	{
		NB_Link_step = NBLINK;
	}
}


/**
*@brief	 	�����������ݵ�λ��
*@param		
*@return	n:λ��
*/
unsigned char judgeLen(int  a)
{
    int n=0;  
    while (a)
    {
        a = a / 10;  
        n++;  
    }
    return n;  
}
/**
*@brief	 	16����ת���ɴ�д�ַ��� ���磺0xA1��ת����str[0] = ��A��,str[0] = ��1��,
*@param		n:16���ƣ�str���ַ���
*@return	
*/
void BC95_chartoasc(char aa,unsigned char *buffer)
{
    unsigned char i=0;
    buffer[0]=aa/16;
    buffer[1]=aa%16;
    for(i=0;i<2;i++)
    {
        if(buffer[i]<10)buffer[i]+='0';
        else buffer[i]=buffer[i]-10+'A';
    }
}
unsigned char BC95_hextochar(char *buffer,char *aa)
{
    unsigned char i=0;
    for(i=0;i<2;i++)
    {
        if((buffer[i]>='A')&&(buffer[i]<='F'))
        {
            buffer[i]=buffer[i]-'A'+10;
        }
        else if((buffer[i]>='0')&&(buffer[i]<='9'))
        {
            buffer[i]=buffer[i]-'0';
        }
        else
        {
            return 0;
        }
    }
    *aa=(buffer[0]<<4)+buffer[1];
    return 1;
}
unsigned char BC95_HexStrtoCharStr(char *buffer,unsigned int buflen,char *aa)
{
    unsigned int i=0;
    unsigned char flag=0;
    if((buflen%2==1)||(buflen==0))return 0;
    for(i=0;i<buflen/2;i++)
    {
        flag=BC95_hextochar((char *)&buffer[i*2],(char *)&aa[i]);
        if(flag==0)return 0;
    }
    return 1;
}
/**
*@brief	 	������ת��Ϊ�ַ�������
*@param		n:Ҫת�������� str[5]:���ת������ַ���  len������������
*@return	��
*/
//void itoa(unsigned int n,unsigned char str[5], unsigned char len)
//{
//	unsigned char i=len-1;
//	memset(str,0x20,len);
//	do{
//		str[i--]=n%10+'0';
//	}while((n/=10)>0);
//	return;
//}

unsigned char int16toString(unsigned int cnt,unsigned char *buf)
{
	unsigned char tmp[5];
	unsigned char i=0;
	if(cnt>=10000)return 0;
	else //if(cnt>0x0fff)
	{
		tmp[0]=cnt/1000;
		tmp[1]=(cnt%1000)/100;
		tmp[2]=(cnt%100)/10;
		tmp[3]=cnt%10;
		for(i=0;i<4;i++)
		{
			if(tmp[i]<=9)
			{
				tmp[i]+='0';
			}
			else
			{
				return 0;
			}
		}
		memcpy(buf,tmp,4);
	}
	return i;
}
unsigned char Str_Is_Equal(unsigned char *dest,unsigned char *source,unsigned int length)
{
	unsigned int i=0;
	unsigned int flag=1;
	for(i=0;i<length;i++)
	{
		if(*dest++!=*source++)
		{
			flag=0;
			break;
		}
	}
	return flag;
}
//�ж�buffer�ַ����Ƿ����scr�ַ���
//�������scrΪ�ַ�����������\0��
//��buffer�ַ�������scr�򷵻�scr�ַ�����������һ���ַ�λ�ã����򷵻�0
unsigned int IsStrInclude(unsigned char *buffer,unsigned char *scr,unsigned int buflenth)
{
	unsigned int i=0;
	unsigned char flag=0;
	if(buflenth<strlen((char const*)scr))return 0;
	for(i=0;i<buflenth+1-strlen((char const*)scr);i++)
	{
		flag=Str_Is_Equal((unsigned char *)&buffer[i],(unsigned char *)scr,strlen((char const*)scr));
		if(flag==1)
		{
			i+=strlen((char const*)scr);
			return i;
		}
	}
	if(i==buflenth+1-strlen((char const*)scr))i=0;
	return i;
}
/*****************************************************************************
* ��������: unsigned char sum_check(unsigned char* data, unsigned char len)
* ��������: �ۼӺ�У�麯��
* ��    ��: 
* ����  ֵ:
*****************************************************************************/
unsigned char sum_check(unsigned char *data, unsigned char len)
{
	unsigned char sum = 0;
	unsigned char i = 0;
	for (i = 0; i < len; i++)
	{
		sum += data[i];
	}
	return sum;
}

//const char AT_AT[]="AT\r\n";
//const char AT_CLOSE_FUN[]="AT+CFUN=0\r\n";
//const char AT_AUTOCONNECT[]="AT+NCONFIG=AUTOCONNECT,TRUE\r\n";
//const char AT_QNBAND[]="AT+NBAND?\r\n";


/*****************************************************************************
* ��������: void Send_AT_Command(void)
* ��������: ��������
* ��    ��:
* ����  ֵ:
*****************************************************************************/
void Send_AT_Command(unsigned char step)
{
	unsigned char Data_Frame[100];
	unsigned char Count=0;
	memset(Data_Frame,0,100);
	switch(step)
	{
		case  NBLINK:
			memcpy(Data_Frame,"AT\r\n",4);
			Count = 4;
		break;
		case  NB_OPEN_FUN:
			memcpy(Data_Frame,"AT+CFUN=0\r\n",11);//�ر���Ƶ����
			Count = 11;
		break;
		case  NB_AUTOCONNECT:
			memcpy(Data_Frame,"AT+NCONFIG=AUTOCONNECT,TRUE\r\n",29);//��������ģʽΪ�Զ�����
			Count = 29;								
		break;
		case  NB_NBAND:
            if((MNC==0)||(MNC==2)||(MNC==4)||(MNC==7))//�й��ƶ�
            {
                
            }
            else if((MNC==1)||(MNC==6)||(MNC==9))//�й���ͨ
            {
                if(NetPowerFlag == 1)
                {
                    NetPowerFlag = 0;
                    if(NetLockFlag == 0)
                    {
                        NetNum = 1-NetNum;
                    }
                }
                if(NetNum==0) memcpy(Data_Frame,"AT+NBAND=3\r\n",11);//��ѯƵ��
                else memcpy(Data_Frame,"AT+NBAND=8\r\n",11);//��ѯƵ��
            }
            else//3.5.11�й�����
            {
                memcpy(Data_Frame,"AT+NBAND=5\r\n",11);//��ѯƵ��
            }
			Count = 11;
		break;
        case  NB_QNBAND:
			memcpy(Data_Frame,"AT+NBAND?\r\n",11);//��ѯƵ��
			Count = 11;
		break;
		case  NB_NSMI1:
			memcpy(Data_Frame,"AT+NSMI=0\r\n",11);//������Ϣ��־��������Ϣ����л���
			Count = 11;
		break;
		case  NB_NNMI1:
			memcpy(Data_Frame,"AT+NNMI=1\r\n",11);//������Ϣ��־�����ӵ�����Ϣ���л���
			Count = 11;
		break;
        case NB_CGMR:
            memcpy(Data_Frame,"AT+CGMR\r\n",9);//�ر���Ƶ����
			Count = 9;
		break;
        case NB_NCSEARFCN:
            memcpy(Data_Frame,"AT+NCSEARFCN\r\n",14);//�ر���Ƶ����
			Count = 14;
        break;
		case  NB_NCDP:
            if((MNC==0)||(MNC==2)||(MNC==4)||(MNC==7))//�й��ƶ�
            {
                
            }
            else if((MNC==1)||(MNC==6)||(MNC==9))//�й���ͨ
            {
                Count = sizeof(SEVER_ADDRESS_CMCC)-1;//����ͨѶID�Ͷ˿�
			    memcpy(Data_Frame,SEVER_ADDRESS_CMCC,Count); 
            }
            else//3.5.11�й�����
            {
                Count = sizeof(SEVER_ADDRESS_CTCC)-1;//����ͨѶID�Ͷ˿�
			    memcpy(Data_Frame,SEVER_ADDRESS_CTCC,Count);
            }
		break;
		
		case  NB_NRB:
			memcpy(Data_Frame,"AT+NRB\r\n",8);//������
			Count = 8;
		break;
		case  NB_CIMI:
			memcpy(Data_Frame,"AT+CIMI\r\n",9);//��ѯSIM����Ϣ
			Count = 9;
		break;
		case  NB_CGSN:
			memcpy(Data_Frame,"AT+CGSN=1\r\n",11);//��ѯIMEI��
			Count = 11;
		break;
        case NB_CPSMS_0:
            memcpy(Data_Frame,"AT+CPSMS=0\r\n",12);//��ѯ�ź�ǿ��
			Count = 12;
        break;
        case NB_CPSMS:
            memcpy(Data_Frame,"AT+CPSMS?\r\n",11);//��ѯ�ź�ǿ��
			Count = 11;
        break;
		case  NB_CSQ:
			memcpy(Data_Frame,"AT+CSQ\r\n",8);//��ѯ�ź�ǿ��
			Count = 8;
		break;
		case  NB_CGATT_1:
			memcpy(Data_Frame,"AT+CGATT=1\r\n",12);//�Զ�����
			Count = 12;
		break;
		case  NB_CEREG_1:
			memcpy(Data_Frame,"AT+CEREG=1\r\n",12);//����������
			Count = 12;
		break;
		case  NB_CGATT:
			memcpy(Data_Frame,"AT+CGATT?\r\n",11);//�Զ�����
			Count = 11;
		break;
		case  NB_CEREG:
			memcpy(Data_Frame,"AT+CEREG?\r\n",11);
			Count = 11;
		break;
		case  NB_CGPADDR:
			memcpy(Data_Frame,"AT+CGPADDR\r\n",12);//��ѯ�����������ip��ַ
			Count = 12;
            NetLockFlag = 1;
		break;
		default:
		break;				
	}
#if	NB_PRINTF
	USART1_sendstr("NB_TX:");
	USART1_sendbuf(Data_Frame,Count);
	USART1_sendstr("\r\n");
#endif
	NB_UsartSendstr(Data_Frame,Count);
}
//��ȡ������Ϣ������1��
unsigned char ATGetMsg(void)
{
	unsigned char Flag=1;
	unsigned char Data_Frame[20];
	
	memset(Data_Frame,0,20);
	memcpy(Data_Frame,"AT+NMGR\r\n",9);
#if	NB_PRINTF
	USART1_sendstr("NB_TX:");
	USART1_sendbuf(Data_Frame,9);
	USART1_sendstr("\r\n");
#endif
	NB_UsartSendstr(Data_Frame,9);
	return Flag;
}
//��ȡ�ź�ǿ�ȣ�����ʱ�����2.5s�����ʱ��0.5s
//�쳣����99�����������ź�ǿ��0-31��
unsigned char Signal_Check(void)
{
	unsigned int lenth=0;
	unsigned char i=0;

	unsigned char j=0;
	unsigned int buflenth=0;
    //�ٴλ��Ѻ���Ҫ��ȡ2�β��ܶ������ݣ�����ԭ��Ӧ��һ��������
    //ʱ����Ӧ�����������ݷ���ԭ��or���ݲ�ȫ����
    //ASL........20180612
	Send_AT_Command(NB_CSQ);
	delay_ms(500);
	Send_AT_Command(NB_CSQ);
	delay_ms(500);
	// ��ȡGSM�ź�ǿ��
	lenth = UartGetRxLen(&uart_gprs);
	if(lenth>0)
	{
		for(j=0;((buflenth!=lenth)&&(j<200));j++)			//��ֹ���ݶ�ȡ�б����
		{
			buflenth=lenth;
			delay_ms(10);
			lenth = UartGetRxLen(&uart_gprs);
		}
		lenth=UartRead(&uart_gprs, temp_Buffer, RX_BUF_LEN);
		if((lenth>=sizeof(ATCSQ))&&(strstr((const char *)temp_Buffer,ATOK ))&&(strstr((const char *)temp_Buffer,ATCSQ )))
		{
			//��ȡ�ź�ǿ�ȣ�����¼�ж��Ƿ��ź��㹻���㹻��ִ����һ��������ʱ��ѯ��
			//���ź�ǿ��<40�����ж�Ϊ������������
			i=IsStrInclude(temp_Buffer,(unsigned char *)ATCSQ,lenth);
			if((temp_Buffer[i+1]==',')&&(temp_Buffer[i] > '0')&&(temp_Buffer[i] <= '9'))
			{
				if(NB_Link_step<NB_CGATT_1)NB_Link_step=NB_CGATT_1;
				return  temp_Buffer[i]-0x30;
			}
			else if(((temp_Buffer[i] >= '0')&&(temp_Buffer[i]<='3')&&(temp_Buffer[i+1] >= '0')&&(temp_Buffer[i+1] <= '9')&&(temp_Buffer[i+2]==',')))
			{
				if(NB_Link_step<NB_CGATT_1)NB_Link_step=NB_CGATT_1;
				return  (temp_Buffer[i]-0x30)*10+temp_Buffer[i+1]-0x30;
			}
		}
	}
	return 99;
}
unsigned char RecieveATCMDhandle(void)
{
	volatile unsigned char ErrorFlag=0;
	unsigned int lenth=0;
	unsigned int buflenth=0;
//	unsigned char temp_Buffer[RX_BUF_LEN];
	unsigned int j=0;
	unsigned int i=0;
	
	lenth = UartGetRxLen(&uart_gprs);
	if(lenth>0)
	{
		for(j=0;((buflenth!=lenth)&&(j<200));j++)			//��ֹ���ݶ�ȡ�б����
		{
			buflenth=lenth;
			delay_ms(10);
			lenth = UartGetRxLen(&uart_gprs);
		}
		memset(temp_Buffer,0,RX_BUF_LEN);
		lenth=UartRead(&uart_gprs, temp_Buffer, RX_BUF_LEN);//�������ݳɹ�
        
		switch(NB_Link_step)
		{
			case  NB_NRB:
				if((lenth>=sizeof(ATOK))&&(strstr((char const*)temp_Buffer, ATOK)))
				{
					NB_Link_step++;
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
					
				}
			break;
			
			case  NBLINK:
			case  NB_OPEN_FUN:
			case  NB_AUTOCONNECT:
			case  NB_NSMI1:
			case  NB_NNMI1:
            case  NB_NCSEARFCN:
            case  NB_CPSMS_0:
			case  NB_NCDP:
			case  NB_CGATT_1:
			case  NB_CEREG_1:
            case  NB_NBAND:
				if((lenth<10)&&(lenth>=sizeof(ATOK))&&(strstr((char const*)temp_Buffer, ATOK)))
				{
					NB_Link_step++;
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
				}
			break;
            case  NB_CGMR:
				if((lenth<200)&&(lenth>=sizeof(ATOK))&&(strstr((char const*)temp_Buffer, ATOK)))
				{
					NB_Link_step++;
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
				}
			break;
			case  NB_QNBAND:
				if((lenth>=sizeof(ATNBAND))&&(strstr((const char *)temp_Buffer,ATNBAND ))&&(strstr((char const*)temp_Buffer, ATOK)))
				{
					//ģ�鹤��Ƶ���Ƿ���ȷ
					i=IsStrInclude(temp_Buffer,(unsigned char *)ATNBAND,lenth);
					if((temp_Buffer[i] == '5')||(temp_Buffer[i] == '8'))
					{
                        NB_BAND=temp_Buffer[i];
						NB_Link_step++;
					}
					else
					{
						return 1;
					}
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
				}
			break;
            case  NB_CPSMS:
				if((lenth>=sizeof(ATCPSMS))&&(strstr((const char *)temp_Buffer,ATCPSMS ))&&(strstr((char const*)temp_Buffer, ATOK)))
				{
					if(strstr((const char *)temp_Buffer,ATCPSMS0))
					{
						NB_Link_step++;
					}
					else
					{
						return 1;
					}
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
				}
			break;
			case  NB_CIMI:
				if((lenth==0x19)&&(strstr((char const*)temp_Buffer, ATOK)))
				{
					//ASL.................ȱ�ٸ�ʽ�ж���
						for(j=0;j<15;j++)
						{
							if((temp_Buffer[2+j]>='0')&&(temp_Buffer[2+j]<='9'))
							{
								
							}
							else
							{
								break;
							}
						}
						//���ݸ�ʽ��ȷ
						if(j==15)
						{
							//�洢��IMEI���ȡ��IMEI����ȣ�
							if(Str_Is_Equal((unsigned char*)&temp_Buffer[2],(unsigned char*)IMSI,15)==0)
							{
//								FLASH_Unlock(EEPROM);
//								FLASH_WriteNBytes(IMEI_ADDRESS, 15, (unsigned char*)&temp_Buffer[i]);
//								FLASH_Lock(EEPROM);
//								for(j=0;j<15;j++)
//								{
//									IMEI[j]=*(unsigned char*)((unsigned char*)IMEI_ADDRESS+j);
//								}
                                memcpy(IMSI,(unsigned char*)&temp_Buffer[2],15);
							}else
							{
								
							}
                            MNC=(IMSI[3]-'0')*10+IMSI[4]-'0';
							NB_Link_step++;
						}
					//NB_Link_step++;
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
				}
			break;
			case  NB_CGSN:
				if((lenth>=sizeof(ATCGSN))&&(strstr((const char *)temp_Buffer,ATCGSN))&&(strstr((const char *)temp_Buffer,ATOK )))
				{
					i=IsStrInclude(temp_Buffer,(unsigned char *)ATCGSN,lenth);
					//memcpy()����Ӧ��SIM���ż�¼Rxdata[2]--Rxdata[16]��15��λ
					if(i>sizeof(ATCGSN)&&(i+15<=lenth))
					{
						for(j=0;j<15;j++)
						{
							if((temp_Buffer[i+j]>='0')&&(temp_Buffer[i+j]<='9'))
							{
								
							}
							else
							{
								break;
							}
						}
						//���ݸ�ʽ��ȷ
						if(j==15)
						{
							//�洢��IMEI���ȡ��IMEI����ȣ�
							if(Str_Is_Equal((unsigned char*)&temp_Buffer[i],(unsigned char*)IMEI,15)==0)
							{
//								FLASH_Unlock(EEPROM);
//								FLASH_WriteNBytes(IMEI_ADDRESS, 15, (unsigned char*)&temp_Buffer[i]);
//								FLASH_Lock(EEPROM);
//								for(j=0;j<15;j++)
//								{
//									IMEI[j]=*(unsigned char*)((unsigned char*)IMEI_ADDRESS+j);
//								}
                                memcpy(IMEI,(unsigned char*)&temp_Buffer[i],15);
							}else
							{
								
							}
							NB_Link_step++;
						}
					}
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
				}
			break;
			case  NB_CSQ:
				if((lenth>=sizeof(ATCSQ))&&(strstr((const char *)temp_Buffer,ATCSQ ))&&(strstr((const char *)temp_Buffer,ATOK )))
				{
					//��ȡ�ź�ǿ�ȣ�����¼�ж��Ƿ��ź��㹻���㹻��ִ����һ��������ʱ��ѯ��
					//���ź�ǿ��<40�����ж�Ϊ������������
					i=IsStrInclude(temp_Buffer,(unsigned char *)ATCSQ,lenth);
					if((temp_Buffer[i+1]==',')&&(temp_Buffer[i] > '0')&&(temp_Buffer[i] <= '9'))
					{
                        Signal_Value = temp_Buffer[i]-'0';
						NB_Link_step++;
					}
					else if(((temp_Buffer[i] >= '0')&&(temp_Buffer[i]<='3')&&(temp_Buffer[i+1] >= '0')&&(temp_Buffer[i+1] <= '9')&&(temp_Buffer[i+2]==',')))
					{
                        Signal_Value = (temp_Buffer[i]-'0') * 10 +( temp_Buffer[i+1]-'0');
						NB_Link_step++;
					}
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
				}
			break;
			case  NB_CGATT:
				if((lenth>=sizeof(ATCGATT))&&(strstr((const char *)temp_Buffer,ATCGATT))&&(strstr((char const*)temp_Buffer,ATOK)))
				{
					i=IsStrInclude(temp_Buffer,(unsigned char *)ATCGATT,lenth);
					if(temp_Buffer[i]=='1')
					{
						//��ȡ����״̬����Ϊ1�����ųɹ���
						NB_Link_step++;
					}
					else if(temp_Buffer[i]=='0')
					{
                        //����ʧ�ܣ��ȴ����ųɹ�
					}
				}else if((lenth>=sizeof(ATCEREG))&&(strstr((char const*)temp_Buffer,ATCEREG)))
				{
					i=IsStrInclude(temp_Buffer,(unsigned char *)ATCEREG,lenth);
					if(temp_Buffer[i]=='1')
					{             
						NB_Link_step=NB_CGPADDR;
					}
					else
					{
					}
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
				}
			break;
			case  NB_CEREG:
				if((lenth>=sizeof(ATCEREG))&&(strstr((char const*)temp_Buffer,ATCEREG))&&(strstr((char const*)temp_Buffer,ATOK)))
				{
					i=IsStrInclude(temp_Buffer,(unsigned char *)ATCEREG,lenth);
					if(temp_Buffer[i+2]=='1')
					{
						NB_Link_step++;
					}
					else if(temp_Buffer[i+2]=='0')
					{
					}
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
				}
			break;
			case  NB_CGPADDR:
				if((lenth>=sizeof(ATCGPADDR))&&(strstr((char const*)temp_Buffer,ATOK))&&(strstr((char const*)temp_Buffer,ATCGPADDR)))
				{
					//ASL.................ȱ�ٸ�ʽ�ж���
					//ע������ɹ���
					NB_Link_step=NB_CONNECT;
				}else if((lenth>=sizeof(AT_ERROR))&&(strstr((char const*)temp_Buffer, AT_ERROR)))
				{
					ErrorFlag=1;
				}
				else
				{
				}
			break;
			case  NB_CONNECT:
				
			break;
			default :
			break;
		}
	}

	return 0;
}
unsigned char RetSendControl(void)
{
	static AT_link_ENUM NB_Status=NBLINK;		//��һ��NBģ��ִ�к���ʱ��״̬
	static unsigned int NB_Cnt=0;			//��ͬһ״̬�µ�ʱ�������100ms�ļ�����
	static unsigned int NB_ReSendTimes=0;
	static unsigned char ReSendFlag=0;		//�ط���־�����յ��������ݣ����߳�ʱʱ��λ��ʹ���ϱ���
    //�ط��������ƣ�
	NB_Cnt++;
	if(NB_Status!=NB_Link_step) //״̬���£������־��
	{
		NB_Status=NB_Link_step;
		NB_Cnt=0;
		NB_ReSendTimes=0;
		ReSendFlag=1;
	}
	else
	{
		switch(NB_Link_step)
		{
			case  NB_OPEN_FUN:
			case  NB_AUTOCONNECT:
            case  NB_QNBAND:
			case  NB_NBAND:
			case  NB_NSMI1:
			case  NB_NNMI1:
            case  NB_CGMR:
            case  NB_NCSEARFCN:
            case  NB_CPSMS_0:
            case NB_CPSMS:
			case  NB_NCDP:
			case  NB_CGATT_1:
			case  NB_CEREG_1:
				if(NB_Cnt>=20)
				{
					NB_Cnt=0;
					NB_ReSendTimes++;
					if(NB_ReSendTimes>=3)
					{
						NB_ReSendTimes=0;
						return 1;
					}
					else
					{
						ReSendFlag=1;
					}
				}
			break;
			case  NBLINK:
				if(NB_Cnt>=60)
				{
					NB_Cnt=0;
					NB_ReSendTimes++;
					if(NB_ReSendTimes>=5)
					{
						NB_ReSendTimes=0;
						return 1;
					}
					else
					{
						ReSendFlag=1;
					}
				}
			break;
			case  NB_NRB:
			case  NB_CIMI:
			case  NB_CGSN:

			case  NB_CSQ:
			case  NB_CGATT:
            //case  NB_CPSMS:
			case  NB_CEREG:
				if(NB_Cnt>=40)
				{
					NB_Cnt=0;
					NB_ReSendTimes++;
					if(NB_ReSendTimes>=8)
					{
						NB_ReSendTimes=0;
						return 1;
					}
					else
					{
						ReSendFlag=1;
					}
				}
			break;
			default:
			break;				
		}
	}
    //�ط���������
	if(ReSendFlag!=0)
	{
		ResetUartBuf(&uart_gprs);
		if(NB_Link_step<NB_CONNECT)
		{
			ReSendFlag=0;
			Send_AT_Command(NB_Link_step);
		}
	}
    return 0;
}
//ģ��ע��������
//�ط����ʱ����ƣ����ջظ����ݣ���
//��������ÿ100msִ��1�Σ�
//��ȡIMEI�����У��洢��IMEI�����У�
//��ȡCSQ�����У����ź�ǿ�ȴ洢��Signal_Value�У�
//����ֵ��Ϊ1ģ�����Ͳ���ȷ���ǵ��ŵģ������쳣�ط��������ࣻ
//ֱ���˳���ֹ�����ȴ���
unsigned char RegisterToServer(void)
{
    unsigned char flag=0;
    flag=RecieveATCMDhandle();
    if(flag == 1) return 1;
    flag=RetSendControl();
    return flag;
}
/*****************************************************************************
* ��������: unsigned char gprs_send_start(void)
* ��������: GM3ģ�鷢��׼������
* ��    ��: 
* ����  ֵ:1:ע������ʧ��	0��ע������ɹ�
//������ִ��ʱ�����60s

*****************************************************************************/
unsigned char gprs_send_start(void)
{
	unsigned int t = 0;
	unsigned char flag=0;
	OS_ERR err;
	//�����60s��û��ע��������,ģ������
	for (t = 0; t < 600; t++)								// 60S
	{
		flag=RegisterToServer();
		if(flag==1)
		{
			//������ģ���쳣���쳣�������Ӧ�����˳��ȴ���
			//������������ֹ�����ȴ����Ĺ���
			t = 600;
			break;
		}
		else
		{
			
		}
		if (NB_Link_step==NB_CONNECT)						// ��������ɹ�
		{
			Signal_Value=Signal_Check();					// ����ź�ǿ�� 
#if	NB_PRINTF
			USART1_sendstr("Register OK !\r\n");
			USART1_sendstr("BC95+CSQ:");
			USART1_sendchar((Signal_Value%100)/10+'0');
			USART1_sendchar(Signal_Value%10+'0');
#endif
			break;
		}
		else
		{
//			delay_ms(100);									// ��ʱ100ms
      OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ100ms
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ10ms
	}
	if (t >= 600)											// �������糬ʱ
	{
		reset_gprs();									// ��λģ��
//		delay_ms(5000);									// ��ʱ5000ms
		OSTimeDlyHMSM(0,0,5,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ5s
		return 1;
	}
	return 0;
}
//u32 Wait_Signal(u32 max_time)
//{
//    OS_ERR err;
//    u32 starttamp,timetamp,i;
//    ResetUartBuf(&uart_gprs);
////    starttamp=OSTimeGet(&err);
////   while(1)
//		printf("t=%d��\r\n",max_time);
//	  for(i=0;i<max_time;i++)
//    {
////         timetamp=OSTimeGet(&err);
////        if(starttamp<uart_gprs.last_receive)
////        { 
////            if(timetamp-uart_gprs.last_receive>10) break;
////        }
////        if(timetamp-starttamp>max_time) break;
//				if(timetamp-uart_gprs.last_receive>10) break;
//        delay_ms(10);
//    }
//		printf("i=%d��\r\n",i);
//    return uart_gprs.last_receive-starttamp;
//}

u32 Wait_Signal(u32 max_time)
{
    OS_ERR err;
    u32 starttamp,timetamp;
    ResetUartBuf(&uart_gprs);
//	  uart_gprs.last_receive=0;
    starttamp=OSTimeGet(&err);
    while(1)
    {
         timetamp=OSTimeGet(&err);
        if(starttamp<uart_gprs.last_receive)
        { 
            if(timetamp-uart_gprs.last_receive>10) break;
        }
        if(timetamp-starttamp>max_time) break;
//        delay_ms(10);
			  OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ10ms
    }
    return uart_gprs.last_receive-starttamp;
}

/*****************************************************************************
* ��������: void	BC95_Tx_Frame(u8 *Data,u16 Len)
* ��������: ��������
* ��    ��:	Data�����ݣ� DataLen�����ݳ���
* ����  ֵ:
ASL......�����������,BC95һ���������512����
//��Data���ֽ�����ʽ�Ϸ�
*****************************************************************************/
u32 GPRS_senddata(u8 *Data,u32 DataLen)//unsigned int BC95_Tx_Frame(unsigned char *Data,unsigned int DataLen)
{
	unsigned int i;
    u32 xi;
	unsigned char ReadData[20];
	unsigned int Count = 0;
	unsigned char Len = 0;
	if((NB_Link_step!=NB_CONNECT)||(DataLen>512))
	{
		//��ʱ���˳���ʱ�䵽�����豸
		return 0;
	}
    for(xi=0;xi<1;xi++)
    {
    ResetUartBuf(&uart_gprs);
	memset(ReadData, 0, sizeof(ReadData));
	memset(Data_Frame, 0, sizeof(Data_Frame));
	i = 0;
	memcpy(Data_Frame,"AT+NMGS=",8);
	Count = 8;
	Len =	judgeLen(DataLen+2);					//����λ��
	itoa((DataLen+2), ReadData, Len);				//ת�����ַ���
	for(i = 0; i< Len;i++)
	{
		Data_Frame[Count++] =  ReadData[i];
	}
	Data_Frame[Count++] = ',';
    
	BC95_chartoasc(DataLen/256, ReadData);
	Data_Frame[Count++] = ReadData[0];
	Data_Frame[Count++] = ReadData[1];
	
	BC95_chartoasc(DataLen%256, ReadData);
	Data_Frame[Count++] = ReadData[0];
	Data_Frame[Count++] = ReadData[1];
#if	NB_PRINTF
	USART1_sendstr("NB_TX:");
	USART1_sendbuf(Data_Frame,Count);
#endif
	NB_UsartSendstr(Data_Frame,Count);
        
	for( i = 0;i < DataLen; i++)
	{
		BC95_chartoasc(Data[i], Data_Frame);
		NB_UsartSendstr(Data_Frame,2);
#if	NB_PRINTF
		USART1_sendbuf(Data_Frame,2);
#endif
	}
	Data_Frame[0] = '\r';
	Data_Frame[1] = '\n';
	NB_UsartSendstr(Data_Frame,2);
#if	NB_PRINTF
    USART1_sendbuf(Data_Frame,2);
#endif
    Wait_Signal(10000);
    memset(&temp_Buffer, 0, RX_BUF_LEN);
    Len = UartRead(&uart_gprs, temp_Buffer, RX_BUF_LEN);
    if((Len>=sizeof(ATOK))&&(strstr((char const*)temp_Buffer, ATOK))) return 0;
    delay_ms(1000);
    }
	return 1;//return Count;
}
//u32 GPRS_buffer_send(u8 *str,u32 DataLen)
//{
//    u32 err,length;
//    err=GPRS_senddata(str,DataLen);
//    if(err) return err;
//    err=gprs_receive_check();
//    if(!err) 
//    {
//        length=Get_GPRS_Receive(USART2_RX_BUF_middle+1,sizeof(USART2_RX_BUF_middle)-1);
//        USART2_RX_BUF_middle[0]=(u8)length;
//        USART2_RX_FLAG=1;
//        ResetUartBuf(&uart_gprs);
//    }
//    return err;
//}
u32 GPRS_buffer_send(u8 *str,u32 DataLen)
{
    u32 err;
    err=GPRS_senddata(str,DataLen);
    return err;
}
unsigned int NB_Rx_Frame(unsigned char *Rxdata,unsigned int lenth,unsigned char *NBRxdata)
{
	unsigned int j,i=0;
	unsigned int NBRxlen=0;
	
	i=IsStrInclude(Rxdata,(unsigned char *)"\r\n",lenth);
	
	for(j=i;j<lenth;j++)
	{
		if((Rxdata[j]>='0')&&(Rxdata[j]<='9'))
		{
			NBRxlen=NBRxlen*10;
			NBRxlen+=Rxdata[j]-'0';
		}
		else break;
	}
	NBRxlen*=2;
	memcpy(NBRxdata,(unsigned char *)&Rxdata[j+1],NBRxlen);
	return NBRxlen;
}

u32 gprs_receive_check(void)
{
    u32 lenth;
    //ResetUartBuf(&uart_gprs);
    //ATGetMsg();
    Wait_Signal(25000);
    memset(&temp_Buffer, 0, RX_BUF_LEN);
    lenth = UartRead(&uart_gprs, temp_Buffer, RX_BUF_LEN);
    if((lenth>=sizeof(ATNNMI)+2)&&(strstr((char const*)temp_Buffer, ATNNMI))) return 0;
    else return 1;
}
u32 gprs_check_connect(void)
{
    volatile u32 length;
	  u32  Back=1;
	
    u8 command[11]="AT+CEREG?\r\n";
    //u8 command[11]="AT+CGATT?\r\n";
    USART4_sendbuf(command,11);// USART2_SEND(command,11);//USART2_SEND(command,11);
    Wait_Signal(10000); //ZHAONING
//		delay_ms(1000);									// ��ʱ5000ms
    memset(&temp_Buffer, 0, RX_BUF_LEN);
    length = UartRead(&uart_gprs, temp_Buffer, RX_BUF_LEN);
    if(strstr((char const*)temp_Buffer, AT_CEREG1)) {Back=0;}
    if(strstr((char const*)temp_Buffer, AT_CEREG5)) {Back=0;}
	  //20190121
	  if(strstr((char const*)temp_Buffer, AT_CEREG01)) {Back=0;}
    if(strstr((char const*)temp_Buffer, AT_CEREG05)) {Back=0;}
    
		ResetUartBuf(&uart_gprs);
		
		return Back;
}
u8 data_transform(u8 data)
{
    if((data>='0')&&(data<='9')) return data-'0';
    if((data>='a')&&(data<='z')) return data-'a'+10;
    if((data>='A')&&(data<='Z')) return data-'A'+10;
    else return 0xff;
}
u32 Get_GPRS_Receive(u8 *str,u32 max_length)
//u32 Get_GPRS_Rec(u8 *str,u32 max_length)
{
    u32 i,j,length,add;
    length=uart_gprs.pRxWr-uart_gprs.RxBuf;
    for(i=0;i<length;i++)
    {
        if(uart_gprs.RxBuf[i]==',') break;
    }
    if(uart_gprs.RxBuf[i]!=',') return 0;
    length=0;
    for(j=1,add=1;j<=3;j++)
    {
        if(data_transform(uart_gprs.RxBuf[i-j])==0xff) break;
        length+=data_transform(uart_gprs.RxBuf[i-j])*add;
        add*=10;
    }
    length-=2;
    if(length>max_length) length=max_length;
    for(j=0;j<length*2;j++)
    {
        if(data_transform(uart_gprs.RxBuf[i+5+j])==0xff) break;
        if(j%2) *(str+j/2)+=data_transform(uart_gprs.RxBuf[i+5+j]);
        else *(str+j/2)=data_transform(uart_gprs.RxBuf[i+5+j])*16;
    }
		
		ResetUartBuf(&uart_gprs);
		
    //delay_ms(50);
    if(j==length*2) return length;
    else return 0;
    
}

u32 Get_GPRS_Receive_Length()
{
	  u32 length=0;
    length=uart_gprs.pRxWr-uart_gprs.RxBuf;
	  return length;
}
//-----------------------------����-------------------------------------------------------------//
/*****************************************************************************
* ��������: void ResetUartBuf(stUart *st)
* ��������: ��λ���ڻ�����
* ��    ��:
* ����  ֵ:
*****************************************************************************/
void ResetUartBuf(stUart *st)
{
	memset((void *)(st->RxBuf), 0, RX_BUF_LEN);
	memset((void *)(st->TxBuf), 0, TX_BUF_LEN);
	st->pRxWr = st->RxBuf;
	st->pRxRd = st->RxBuf;
	st->pTxWr = st->TxBuf;
	st->pTxRd = st->TxBuf;
}


unsigned int UartGetRxLen(stUart *st)
{
	unsigned int rxcnt;
	if(st->pRxWr >= st->pRxRd)
		rxcnt = st->pRxWr - st->pRxRd;
	else
		rxcnt = st->pRxWr + sizeof(st->RxBuf) - st->pRxRd;
	return rxcnt;
}


unsigned int UartRead(stUart *st, unsigned char *buf, unsigned int len)
{
	unsigned int rxcnt,rdlen,ret;
	unsigned char *p = buf;
	if(len == 0)
		return 0;
	rxcnt = UartGetRxLen(st);
	rdlen = (rxcnt < len) ? rxcnt : len;
	ret = rdlen;
	while(rdlen)
	{
		rdlen--;
		*p = *(st->pRxRd);
		p++;
		st->pRxRd++;
		if(st->pRxRd - st->RxBuf >= sizeof(st->RxBuf))
		st->pRxRd = st->RxBuf;
	}
	return ret;
}

/*****************************************************************************
* ��������: void    gprs_link(void)
* ��������: GPRS�����ж�
* ��    ��:
* ����  ֵ:
*****************************************************************************/
void    gprs_link(void)
{
  u8 open_flag;
        /*unsigned char t  = 0;
        unsigned char i  = 0;
        do
        {
                t = gprs_send_start();
                i++;
                if(i > 4)
                {
                        i = 0;
                        GPRS_Reset();
                        delay_nms(5000);
//                        t = 0;
                }
        }
        while(t != 0 );*/
  do
  {
    OpenGPRS_check();
    open_flag = gprs_send_start();
  }while(open_flag);
    
}
/*
void UartRxISR(stUart *st,char c)
{
    OS_ERR err;
    if(st->pRxWr<=st->RxBuf) st->pRxWr = st->RxBuf;
    else if(st->pRxWr - st->RxBuf >= sizeof(st->RxBuf)-1) st->pRxWr = st->RxBuf;
  	*(st->pRxWr) = c;
        st->pRxWr++;                                                           
	
		if(st->pRxWr - st->RxBuf >= sizeof(st->RxBuf)-1)
				st->pRxWr = st->RxBuf;
        st->last_receive=OSTimeGet(&err);
}
*/
/*
u32 GPRS_Detect_Handle(u32 timestamp)
{
    u32 length;
    if(timestamp-uart_gprs.last_receive>10)
    {
        if(uart_gprs.pRxWr-uart_gprs.RxBuf<MIN_GPRS_DATALENGTH)
        {
            ResetUartBuf(&uart_gprs);
            return 0;
        }
        else
        {
            length=Get_GPRS_Receive(USART2_RX_BUF_middle+1,sizeof(USART2_RX_BUF_middle)-1);
            if(length)
            {
                USART2_RX_BUF_middle[0]=(u8)length;
                USART2_RX_FLAG=1;
            }
            
            ResetUartBuf(&uart_gprs);
            return length;
        }
    }
    //else ResetUartBuf(&uart_gprs);
    return 0;
}
*/
void UART4_Init(u32 bound)
{ 	
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	// GPIODʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE); //����4ʱ��ʹ��
 	USART_DeInit(UART4);  //��λ����4
		 //UART4_TX   PC.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ��PC.10
    //UART4_RX	  PC.11GPIO_Mode_IPU
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������   GPIO_Mode_IPU
  GPIO_Init(GPIOC, &GPIO_InitStructure);  //��ʼ��PC.11
	
	USART_InitStructure.USART_BaudRate = bound;//������һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  
	USART_Init(UART4, &USART_InitStructure); //��ʼ������	4  
	USART_Cmd(UART4, ENABLE);                    //ʹ�ܴ��� 	
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//�����ж�
	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
}

void NB_Init(void)//NB��ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;
	UARTParameterConfiguration gUARTSystemParameter;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 
	//��λ�ܽ�
	GPIO_InitStructure.GPIO_Pin = PIN_GPRS_RESET;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(PORT_GPRS_RESET, &GPIO_InitStructure);					  
 					 
	GPRS_RESET_LOW;
	
	UART4_Init(9600);

}