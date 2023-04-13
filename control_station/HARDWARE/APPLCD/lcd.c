#include "lcd.h"
#include "string.h"
#include "stm32f10x.h"
#include "uart5.h"
#include "usart2.h"
#include "includes.h"
#include "ServerAction.h"
#include "rtc.h"
#include "w25q16.h"
#include "net.h"
#include "SDcontrol.h"
#include "ProtocolBoard.h"
#include "gprs.h"
#include "os.h"
#include "includes.h"
//#include "CAN1ProtocolParsing.h"

//#define LCDBright10S 70
//#define RS485_232_Mode 1
//#define CAN_Mode 2

//#define LCD_POWEROFF() 		GPIO_ResetBits(GPIOC,GPIO_Pin_2)
//#define LCD_POWERON() 		GPIO_SetBits(GPIOC,GPIO_Pin_2)
//#define LCD_POWERREAD()		GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_2)

//#define FUN1CURSOR_HOME_PAGE 0
//#define FUN1CURSOR_PRE_PAGE 1
//#define FUN1CURSOR_NEXT_PAGE 2
//#define FUN1CURSOR_LAST_PAGE 3
//#define FUN1CURSOR_PAGE_NUM 4
//#define FUN1CURSOR_LOAD2SD 5
//#define FUN1CURSOR_EXIT 6
////unsigned char Loop_Part_flag=0;//�����û�������ź�������·�����ŵı�־λ��1��ʱ��Ϊ�û��������룬0��ʱ��Ϊ��·�����š�
//unsigned char cancel_flag=0;
//unsigned char register_card_flag=0;//ע��忨��־λ��0��ʱ�򲻽���ע��忨ҳ�棬1��ʱ����롣
unsigned char correction_time_way=1;//У��ʱ�䷽ʽ����Ϊ�ֶ����Զ���0:�ֶ���1���Զ���

unsigned char Local_SoftVer[2];
//unsigned char Local_Addres[6]={0x10,0x07,0x07,0x00,0x09,0x01};//�����ã�
//unsigned char Local_Addres[6]={0x11,0x07,0x07,0x00,0x09,0x87};//�����ã�
//unsigned char Local_Addres[6]={0x12,0x07,0x07,0x00,0x09,0x01};//�����ã�
//unsigned char Local_Addres[6]={0x12,0x07,0x07,0x00,0x09,0x02};//�����ã�

unsigned char Local_Addres[6]={0x00,0x00,0x00,0x00,0x00,0x00};//�����ã�

 
//static unsigned int Lcd_Flash_Cnt=20;
////�ֱ����𾯡����ϴ�����¼
//static unsigned int Manual_alarm_Cnt=0;
//static unsigned int Host_alarm_Cnt=0;
//static unsigned int Host_Flult_Cnt=0;

//static unsigned int func_index=0;		//Ŀǰ��ʾ�����
//static unsigned char fun0_Cursor=0;		//0������λ��
//static unsigned char signal_flag=0;		//�Ϸ��ź�ָʾ

//static unsigned char fun1_Cursor=0;		//1������λ��
//static unsigned int fun1_page=1;
//static unsigned int fun1_page_fixed=1;

////static unsigned char fun2_Cursor=0;		//2������λ��
//static unsigned char input_password_no=0;
//static unsigned char password_three_ok=0;
//static unsigned char input_password[4]={0x20,0x20,0x20,0x20};
////static unsigned char input_password_two[4]={0x20,0x20,0x20,0x20};

//static unsigned char fun3_Cursor=0;		//3������λ��

//static unsigned char fun6_Cursor=0;		//6������λ��
//static unsigned char net_fixed=1;		//�������ӡ�����������ʾ


//static unsigned char fun9_Cursor=0;		//9������λ��
//static unsigned char host_fixed=1;

////static unsigned char Board_card_flag=0;//������ĸ��忨��
//unsigned short register_flag=0x0000;//��Ҫ��ʾ�İ忨��Ϣ  //ע��ɹ���־λ��1��ʱ��ע��ɹ���0��ʱ��ע��ʧ�ܡ�
//static unsigned char fun12_Cursor=0;	//12������λ��
//static unsigned char fun12_Cursor_fixed=4;
//static unsigned char rs232baund=0;
//static unsigned char rs232parity=0;
//static unsigned char rs232stopbit=0;
//static unsigned char rs232baund_fixed=0;
//static unsigned char rs232parity_fixed=0;
//static unsigned char rs232stopbit_fixed=0;

//static unsigned char fun15_Cursor=0;	//15������λ��
//static unsigned char fun15_Cursor_fixed=2;
//static unsigned char CANbaund=0;
//static unsigned char CANbaund_fixed=0;

//static unsigned char fun18_Cursor=0;	//18������λ��
//static unsigned char fun18_Cursor_fixed=3;
//static unsigned char Backlight_time=0;
//static unsigned char Backlight_brightness=0;
//static unsigned char Backlight_time_fixed=0;
//static unsigned char Backlight_brightness_fixed=0;
//static unsigned int Backlight__keep_time=LCDBright10S;//���Ᵽ��ʱ��

////static unsigned char fun21_Cursor=0;	//21������λ��
//static unsigned char input_password_times=0;

//static unsigned char fun22_Cursor=0;	//22������λ��
//static unsigned char sound=0;
//static unsigned char sound_fixed=0;

//static unsigned char fun25_Cursor=0;	//25������λ��
//static unsigned char fun25_Cursor_fixed=0;	//25������λ��
//static unsigned char year=0;
//static unsigned char month=0;
//static unsigned char date=0;
//static unsigned char hours=0;
//static unsigned char minute=0;
//static unsigned char sec=0;

//static unsigned char KeyTypeFlag=0;		//28���棬��Ҫ��������İ������࣬��λ���Լ졢���

//static unsigned char password_two[4]={'0','1','1','9'};		//��������
//const unsigned char password_three[4]={'1','8','0','6'};	//��������

////������������״̬
//const char ExtensionStatus[74][16]=
//{
//"����","��������","��","����","���ϻָ�",\
//"��λ","����","���ν��","����","������",\
//"ȷ��","���","��ܳ���","����","�Զ�ֹͣ",\
//"�ֶ�����","�ֶ�ֹͣ","�ֶ�����","Ѳ��","�Լ�",\
//"���Ӧ��","����","��������","����ȱʧ","��ʱ",\
//"��ʩ��λ","��ʩ��λ","�����Ŵ�","�����Źر�","ѹ������",\
//"��ѹ����","��ѹ����","ѹ���ָ�����","��ʩ��λ����","��ʩ�ָ���λ",\
//"���շ����Ŵ�","���������Źر�","����ͨ������","����ͨ������","�����쳣",\
//"��������","��ǩ����","��·���·","��·���·","�ش�",\
//"�ش���","��Դ����","�������","����ָ�","��Դ���ϻָ�",\
//"��ӡ������","��ӡ�����ϻָ�","����","�ػ�","��������",\
//"�����ش�","�����ش�����","����","�������ֱ���","���շ����Źر�",\
//"���������Ŵ�","ˮλ����","ˮλ����","ˮλ�ָ�����","ˮ�¹���",\
//"ˮ�¹���","ˮ�»ָ�����","©�籨��","ײ��","��б",\
//"����","����","����˨�Ǵ�","δ֪"
//};
////����������ʩϵͳ״̬
//const char HostStatus[14][15]={"��������","������","��������","��������","�������",\
//							"��������","��������","������ʱ״̬","�����������","�����������",\
//							"�������߹���","�����ֶ�״̬","�������øı�","������λ"};
////����������ʩ��������״̬
//const char HostPartsStatus[9][9]={"��������","��","����","����","���","����","����","��ʱ״̬","��Դ����"};
////����������ʩ������Ϣ
//const char HostOperation[8][15]={"�޲���","������λ","��������","�����ֶ�����","������������","�����Լ�","����ȷ��","��������"};
////�û���Ϣ����װ�ò�����Ϣ
//const char LocalOperation[8][15]={"������λ","��������","�����ֶ�����","������������","�����Լ�","�������Ӧ��","��������","������������"};
////���ϴ���
////const char FaultCode[16][13]={"�޹���","���ϴ���:01","���ϴ���:02","���ϴ���:03","���ϴ���:04","���ϴ���:05","���ϴ���:06","���ϴ���:07","���ϴ���:08"\
////								,"���ϴ���:09","���ϴ���:10","���ϴ���:11","���ϴ���:12","���ϴ���:13","���ϴ���:14","���ϴ���:15"};
//const char FaultCode[16][20]={"�޹���","����","����","����+����","��·","��·+����","��·+����","��·+����+����","��·" 
//								,"��·+����","��·+����","��·+����+����","��·+��·","��·+��·+����","��·+��·+����","��·+��·+����+����"};
////RS232����������
//const char SetRS232Baund[17][12]={"1200 bps","2400 bps","4800 bps","9600 bps","14400 bps","19200 bps","38400 bps","43000 bps"\
//								,"57600 bps","76800 bps","115200 bps","128000 bps","230440 bps","256000 bps","460800 bps"\
//								,"921600 bps","1382400 bps"};
////RS232У��λ����
//const char SetRS232Parity[3][7]={"��У��","��У��","żУ��"};
////RS232ֹͣλ����
//const char SetRS232StopBit[3][4]={"1","1.5","2"};
////can����������
//const char SetCANBaund[15][12]={"3Kbps","5Kbps","10Kbps","20Kbps","40Kbps","50Kbps","80Kbps","100Kbps"\
//								,"125Kbps","200Kbps","250Kbps","400Kbps","500Kbps","800Kbps","1000Kbps"};
////��������
//const char SetNetMode[4][9]={"        ","��������","��������","���ź�"};
////ע��Э�����ɱ�־λ��
//unsigned char RegisteProComplete=0;

//OS_TMR LcdPowertmr;
////��������
//void fun_cancel(void);
//static void delay_ms(unsigned int ms)
//{
//	unsigned int j,i;
//	for(i=0;i<ms;i++)
//		for(j=0;j<10350;j++);
//}
//unsigned char SendPacketToLcd(unsigned char *Data,unsigned int Lenth)
//{
//	unsigned char flag=0;
//	//20180507,�ٽ�����������������жϣ��ܿ��ܵ��£���ʾʧ�ܡ�
//	CPU_SR_ALLOC();
//	OS_CRITICAL_ENTER();//�����ٽ���
//	//////////
//	flag=UART5_Send_Data(Data,Lenth);
//	delay_ms(1);
//	//////////
//	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
//	return flag;
//}
//unsigned char DataCharge(unsigned char Data)
//{
//	unsigned char num=0;
//	switch(Data)
//	{
//		case 0x00: 
//			num=0;	
//			break;
//		case 0x01: 
//			num=1;	
//			break;
//		case 0x02: 
//			num=2;	
//			break;		
//		case 0x04: 
//			num=3;	
//			break;			
//		case 0x08: 
//			num=4;	
//			break;		
//		case 0x10: 
//			num=5;	
//			break;				
//		case 0x20: 
//			num=6;	
//			break;		
//		case 0x40: 
//			num=7;	
//			break;			
//		case 0x80: 
//			num=0;	
//			break;				
//		default:	
//      return 0;			
//			break;
//	}	
//	return num;
//}



//unsigned char input_extensionStatus(unsigned char *input_data,unsigned char lenth_now,unsigned int input_id,unsigned char *output_data)//����Ҫ��ʾ�Ľ�����������״̬
//{
////	unsigned char input_data[40]={0};//��������顣	
////	unsigned char lenth_now=0;//��ǰ��������ĳ���	
////	unsigned int input_id=0;//�����Ҫ����λ�á�
////	unsigned char output_data[40]={0};//�������Ϻ�����顣
//	unsigned char num=0;//�����ʵ��������
////	unsigned char lenth=0;//�����ʵ�����鳤��	
//	unsigned char i=0;//for ��	
//	unsigned char lenth=0;
//	switch(input_id)
//	{
//		case 0: 
//			num=0;	
//			break;
//		case 1: 
//			num=1;	
//			break;
//		case 2: 
//			num=2;	
//			break;
//		case 4: 
//			num=3;	
//			break;
//		case 5: 
//			num=4;	
//			break;
//		case 6: 
//			num=5;	
//			break;
//		case 8: 
//			num=6;	
//			break;
//		case 9: 
//			num=7;	
//			break;		
//		case 10: 
//			num=8;	
//			break;		
//		case 11: 
//			num=9;	
//			break;		
//		case 12: 
//			num=10;	
//			break;
//		case 16: 
//			num=11;	
//			break;		
//		case 17: 
//			num=12;	
//			break;		
//		case 32: 
//			num=13;	
//			break;				
//		case 33: 
//			num=14;	
//			break;
//		case 34: 
//			num=15;	
//			break;		
//		case 35: 
//			num=16;	
//			break;		
//		case 36: 
//			num=17;	
//			break;		
//		case 37: 
//			num=18;	
//			break;
//		case 38: 
//			num=19;	
//			break;		
//		case 39: 
//			num=20;	
//			break;		
//		case 64: 
//			num=21;	
//			break;					
//		case 65: 
//			num=22;	
//			break;
//		case 66: 
//			num=23;	
//			break;		
//		case 128: 
//			num=24;	
//			break;		
//		case 200: 
//			num=25;	
//			break;		
//		case 201: 
//			num=26;	
//			break;
//		case 202: 
//			num=27;	
//			break;		
//		case 203: 
//			num=28;	
//			break;		
//		case 204: 
//			num=29;	
//			break;
//		case 205: 
//			num=30;	
//			break;
//		case 206: 
//			num=31;	
//			break;		
//		case 207: 
//			num=32;	
//			break;		
//		case 208: 
//			num=33;	
//			break;		
//		case 209: 
//			num=34;	
//			break;
//		case 210: 
//			num=35;	
//			break;		
//		case 211: 
//			num=36;	
//			break;		
//		case 212: 
//			num=37;	
//			break;				
//		case 213: 
//			num=38;	
//			break;
//		case 214: 
//			num=39;	
//			break;		
//		case 215: 
//			num=40;	
//			break;		
//		case 216: 
//			num=41;	
//			break;		
//		case 217: 
//			num=42;	
//			break;
//		case 218: 
//			num=43;	
//			break;		
//		case 219: 
//			num=44;	
//			break;		
//		case 220: 
//			num=45;	
//			break;					
//		case 256: 
//			num=46;	
//			break;
//		case 257: 
//			num=47;	
//			break;		
//		case 258: 
//			num=48;	
//			break;		
//		case 260: 
//			num=49;	
//			break;		
//		case 261: 
//			num=50;	
//			break;
//		case 262: 
//			num=51;	
//			break;		
//		case 263: 
//			num=52;	
//			break;		
//		case 264: 
//			num=53;	
//			break;
//		case 300: 
//			num=54;	
//			break;
//		case 301: 
//			num=55;	
//			break;		
//		case 302: 
//			num=56;	
//			break;		
//		case 303: 
//			num=57;	
//			break;		
//		case 304: 
//			num=58;	
//			break;
//		case 310: 
//			num=59;	
//			break;		
//		case 311: 
//			num=60;	
//			break;		
//		case 312: 
//			num=61;	
//			break;				
//		case 313: 
//			num=62;	
//			break;
//		case 314: 
//			num=63;	
//			break;
//		case 315: 
//			num=64;	
//			break;
//		case 316: 
//			num=65;	
//			break;		
//		case 317: 
//			num=66;	
//			break;		
//		case 401: 
//			num=67;	
//			break;		
//		case 402: 
//			num=68;	
//			break;		
//		case 403: 
//			num=69;	
//			break;
//		case 404: 
//			num=70;	
//			break;		
//		case 405: 
//			num=71;	
//			break;		
//		case 406: 
//			num=72;	
//			break;					
//		case 10000: 
//			num=73;	
//			break;
//		default:
//			num=73;				
////      return 0;			
//			break;
//	}
//	lenth=strlen((char *)&ExtensionStatus[num]);
//	memcpy((char *)&input_data[lenth_now],(char *)&ExtensionStatus[num],lenth);		
//	for(i=0;i<40;i++)
//	{
//		output_data[i]=input_data[i];
//	}
//	lenth_now+=lenth;		
//	return lenth;
//}


////���жϺ����е��ã��Զ��洢
//void Uart5_RxISR(unsigned char buf)
//{
//	static unsigned char UART5_RX_BUF[20];					//��ȡ���յ��ĵ�����Ļ����������
//	static unsigned char UART5_RX_NO=0;						//��������������ݵ��ۼӸ���
////	static unsigned char DWIN_RX_RIGHT=0;					//����յ���Ļ��ȷ��֡�������ֵ��һ��
//	
//	UART5_RX_BUF[UART5_RX_NO]=buf;
//	UART5_RX_NO++;											//ÿ�鴮�������յ������ݸ����ۼ�
//	if(UART5_RX_NO==20)
//	{
//		UART5_RX_NO=0;
//	}
//	if((UART5_RX_BUF[0]==0x5A)&&(UART5_RX_BUF[1]==0xA5))	//�յ���Ļ��������ȷ��֡��0x5A,0xA5Ϊ��ͷ��
//	{
////		DWIN_RX_RIGHT=1;
//	}
//}
//void LcdUart_init(unsigned int bound,unsigned char parity)
//{
//	Uart5_init(bound,parity);
//	pUsart5Rx_Isr=Uart5_RxISR;
//}



//void LcdPowertmr_callback(void *p_tmr,void *p_arg)
//{
//	OS_ERR err;
//	unsigned char i=10;
//	//ִ���˳�
//	while((func_index!=0)&&(i!=0))
//	{
//		i--;
//		input_password_no=0;
//		fun_cancel();
//	}
////	LCD_POWEROFF();

//  LCD_backlight_low();//�����ȵ��ڵ���͡�	

//	OSTmrDel((OS_TMR		*)&LcdPowertmr,(OS_ERR*	)&err);
//}
//void LCD_SET_OFF(void)
//{
////	OS_ERR err;
////	OS_STATE state;
////	state=OSTmrStateGet((OS_TMR		*)&LcdPowertmr,(OS_ERR*	)&err);
////	if(state!=OS_TMR_STATE_UNUSED)
////	{
////		OSTmrDel((OS_TMR		*)&LcdPowertmr,(OS_ERR*	)&err);
////	}
//		OS_ERR err;
//	OS_STATE state;
//	state=OSTmrStateGet((OS_TMR		*)&LcdPowertmr,(OS_ERR*	)&err);
//	if(state!=OS_TMR_STATE_UNUSED)
//	{
//		OSTmrDel((OS_TMR		*)&LcdPowertmr,(OS_ERR*	)&err);
//	}
//	OSTmrCreate((OS_TMR		*)&LcdPowertmr,
//				(CPU_CHAR	*)"LcdPowertmr",
//				(OS_TICK	)100,
//				(OS_TICK	)0,
//				(OS_OPT		)OS_OPT_TMR_ONE_SHOT,
//				(OS_TMR_CALLBACK_PTR)LcdPowertmr_callback,
//				(void*	)0,
//				(OS_ERR*	)&err);
//	OSTmrStart((OS_TMR		*)&LcdPowertmr,(OS_ERR*	)&err);
//}
//void LCD_SET_ON(void)
//{
//	OS_ERR err;
//	OS_STATE state;
//	state=OSTmrStateGet((OS_TMR		*)&LcdPowertmr,(OS_ERR*	)&err);
//	if(state!=OS_TMR_STATE_UNUSED)
//	{
//		OSTmrDel((OS_TMR		*)&LcdPowertmr,(OS_ERR*	)&err);
//	}
//	OSTmrCreate((OS_TMR		*)&LcdPowertmr,
//				(CPU_CHAR	*)"LcdPowertmr",
//				(OS_TICK	)Backlight__keep_time*20,
//				(OS_TICK	)0,
//				(OS_OPT		)OS_OPT_TMR_ONE_SHOT,
//				(OS_TMR_CALLBACK_PTR)LcdPowertmr_callback,
//				(void*	)0,
//				(OS_ERR*	)&err);
//	OSTmrStart((OS_TMR		*)&LcdPowertmr,(OS_ERR*	)&err);
//}

//void LCD_SET_LONG_ON(void)
//{
//	OS_ERR err;
//	OS_STATE state;
//	state=OSTmrStateGet((OS_TMR		*)&LcdPowertmr,(OS_ERR*	)&err);
//	if(state!=OS_TMR_STATE_UNUSED)
//	{
//		OSTmrDel((OS_TMR		*)&LcdPowertmr,(OS_ERR*	)&err);
//	}
//	OSTmrCreate((OS_TMR		*)&LcdPowertmr,
//				(CPU_CHAR	*)"LcdPowertmr",
//				(OS_TICK	)Backlight__keep_time*60,//20
//				(OS_TICK	)0,
//				(OS_OPT		)OS_OPT_TMR_ONE_SHOT,
//				(OS_TMR_CALLBACK_PTR)LcdPowertmr_callback,
//				(void*	)0,
//				(OS_ERR*	)&err);
//	OSTmrStart((OS_TMR		*)&LcdPowertmr,(OS_ERR*	)&err);
//}

//unsigned char LcdOn(void)
//{
//	OS_ERR err;
//	OS_STATE state;
//	unsigned char flag=0;
//	if(LCD_POWERREAD()==0)
//	{
//		LCD_POWERON();
//		flag=1;
//	}
//	state=OSTmrStateGet((OS_TMR		*)&LcdPowertmr,(OS_ERR*	)&err);
//	if(state!=OS_TMR_STATE_UNUSED)
//	{
//		OSTmrDel((OS_TMR		*)&LcdPowertmr,(OS_ERR*	)&err);
//	}
//	OSTmrCreate((OS_TMR		*)&LcdPowertmr,
//				(CPU_CHAR	*)"LcdPowertmr",
//				(OS_TICK	)Backlight__keep_time*20,
//				(OS_TICK	)0,
//				(OS_OPT		)OS_OPT_TMR_ONE_SHOT,
//				(OS_TMR_CALLBACK_PTR)LcdPowertmr_callback,
//				(void*	)0,
//				(OS_ERR*	)&err);
//	OSTmrStart((OS_TMR		*)&LcdPowertmr,(OS_ERR*	)&err);
//	return flag;
//}

////С��99���ڵ�����ת��Ϊ�ַ���
////����99������ʾ99+��
//static void num2str(unsigned int num,unsigned char *data)
//{
//	if(num>99)
//	{
//		memcpy(data,"+99",3);
//	}
//	else
//	{
//		*data++=0x20;
//		*data++=num/10+0x30;
//		*data++=num%10+0x30;
//	}
//}
//static unsigned int crc16(unsigned char *str,unsigned int num) 
//{
//	unsigned int ddd,eee,bbb,crc;
//	crc=0xffff;
//	for (ddd=0;ddd<num;ddd++)
//	{
//		bbb=str[ddd]&0x00ff;
//		crc^=bbb;
//		for (eee=0;eee<8;eee++)
//		{
//			if (crc&0x0001)
//			{
//				crc>>=1;
//				crc^=0xa001;
//			}
//			else
//			crc>>=1;
//		}
//	}
//	return(crc);
//}
////��������ͼƬ
//void change_back_picture(unsigned int add)
//{
//	unsigned char SEND_BUFER[10];
//	unsigned int crc1=0;
//	
//	SEND_BUFER[0]=0x5A;
//	SEND_BUFER[1]=0xA5;
//	SEND_BUFER[2]=0x06;
//	
//	SEND_BUFER[3]=0x80;
//	SEND_BUFER[4]=0x03;
//	SEND_BUFER[5]=(unsigned char)(add>>8);	
//	SEND_BUFER[6]=(unsigned char)add;
//	
//	crc1=crc16((unsigned char*)&SEND_BUFER[3],4);
//	SEND_BUFER[7]=crc1&0x00ff;
//	SEND_BUFER[8]=crc1>>8;
//	SendPacketToLcd(SEND_BUFER,9);
//}
////��ȡ����ͼ��
//void input_small_picture(unsigned int add,unsigned int pic)
//{
//	unsigned char SEND_BUFER[10];//���崮�ڷ�������
//	unsigned crc1=0;
//	
//	SEND_BUFER[0]=0x5A;
//	SEND_BUFER[1]=0xA5;	
//	SEND_BUFER[2]=0x07;	
//	SEND_BUFER[3]=0x82;
//	SEND_BUFER[4]=(unsigned char)(add>>8);
//	SEND_BUFER[5]=(unsigned char)add;	
//	SEND_BUFER[6]=(unsigned char)(pic>>8);
//	SEND_BUFER[7]=(unsigned char)pic;
//	
//	crc1=crc16((unsigned char*)&SEND_BUFER[3],5);
//	SEND_BUFER[8]=crc1&0x00ff;
//	SEND_BUFER[9]=crc1>>8;
//	
//	SendPacketToLcd(SEND_BUFER,10);
//	
//}
////���뺺���ı�
//void input_text(unsigned int add,unsigned char data_long,unsigned char *text)
//{
//	unsigned char SEND_BUFER[100];//���崮�ڷ�������
//	unsigned int crc1=0;
//	
//	if(data_long+8<100)
//	{
//		memset(SEND_BUFER,0,100);
//		SEND_BUFER[0]=0x5A;
//		SEND_BUFER[1]=0xA5;	
//		SEND_BUFER[2]=data_long+5;//���ݳ���	
//		SEND_BUFER[3]=0x82;
//		SEND_BUFER[4]=(unsigned char)(add>>8);//��ַ��λ
//		SEND_BUFER[5]=(unsigned char)add;//��ַ��λ	
//		memcpy(&SEND_BUFER[6],text,data_long);
//		
//		crc1=crc16((unsigned char*)&SEND_BUFER[3],data_long+3);
//		SEND_BUFER[data_long+6]=crc1&0x00ff;
//		SEND_BUFER[data_long+7]=crc1>>8;
//		
//		data_long=data_long+8;
//		SendPacketToLcd(SEND_BUFER,data_long);
//	}
//}
//void back_light_change(unsigned char Backlight_brightness)//��������
//{
//	unsigned char SEND_BUFER[50];//���崮�ڷ�������
//	unsigned int crc1;
//	
//	SEND_BUFER[0]=0x5A;
//	SEND_BUFER[1]=0xA5;
//	SEND_BUFER[2]=0x05;
//	SEND_BUFER[3]=0x80;
//	SEND_BUFER[4]=0x01;
//	
//	switch(Backlight_brightness)
//	{
//		case 0: SEND_BUFER[5]=0x04;  break;
//		case 1: SEND_BUFER[5]=0x0A;	 break;
//		case 2: SEND_BUFER[5]=0x10;	 break;
//		case 3: SEND_BUFER[5]=0x16;	 break;
//		case 4: SEND_BUFER[5]=0x1C;	 break;
//		case 5: SEND_BUFER[5]=0x22;	 break;
//		case 6: SEND_BUFER[5]=0x28;	 break;
//		case 7: SEND_BUFER[5]=0x2E;	 break;
//		case 8: SEND_BUFER[5]=0x34;  break;
//		case 9: SEND_BUFER[5]=0x3A;	 break;
//		case 10: SEND_BUFER[5]=0x00; break;
//		default:		break;
//	}
//	
//	crc1=crc16((unsigned char*)&SEND_BUFER[3],3);
//	SEND_BUFER[6]=crc1&0x00ff;
//	SEND_BUFER[7]=crc1>>8;
//	
//	SendPacketToLcd(SEND_BUFER,8);
//}
////charת��ΪBCD��
//static unsigned char B_BCD(unsigned char Byte)
//{
//    unsigned char BCDnum=0;
//    BCDnum=((Byte%100)/10)<<4;
//    BCDnum|=Byte%10;
//    return BCDnum;
//}

////data���˳��Ϊ������ʱ��������飬ռ��6��byte
//void dwin_time_set(const char *data)//����ʱ����ĵ������ڲ�RTCʱ��
//{
//	unsigned char SEND_BUFER[50];//���崮�ڷ�������
//	unsigned int crc1;

//	SEND_BUFER[0]=0x5A;
//	SEND_BUFER[1]=0xA5;
//	SEND_BUFER[2]=0x0C;//���ݳ���,���ݵĳ��Ȱ���У��λ��2λ��	
//	SEND_BUFER[3]=0x80;
//	SEND_BUFER[4]=0x1F;
//	SEND_BUFER[5]=0x5A;
//	SEND_BUFER[6]=B_BCD(*data++);//��
//	SEND_BUFER[7]=B_BCD(*data++);//��
//	SEND_BUFER[8]=B_BCD(*data++);//��
//	SEND_BUFER[9]=0x00;//
//	SEND_BUFER[10]=B_BCD(*data++);//ʱ
//	SEND_BUFER[11]=B_BCD(*data++);//��
//	SEND_BUFER[12]=B_BCD(*data);//��

//	crc1=crc16((unsigned char*)&SEND_BUFER[3],10);
//	SEND_BUFER[13]=crc1&0x00ff;//У��͵ĵ�λ��
//	SEND_BUFER[14]=crc1>>8;		 //У��͵ĸ�λ��
//	
//	SendPacketToLcd(SEND_BUFER,15);
//}
//unsigned char dwin_time_read(char *time_data)//��ȡ�������ڲ�RTCʱ��
//{
//	unsigned char test[50];	
//	unsigned char SEND_BUFER[50];//���崮�ڷ�������
//	unsigned int crc1;
//	unsigned char i=0;	
//	OS_ERR err;
//	for(i=0;i<50;i++)
//	{
//		uart5_receive_data[i]=0;
//	}
//	uart5_receive_data_inc=0;

//	SEND_BUFER[0]=0x5A;
//	SEND_BUFER[1]=0xA5;
//	SEND_BUFER[2]=0x05;//���ݳ���,���ݵĳ��Ȱ���У��λ��2λ��	
//	SEND_BUFER[3]=0x81;
//	SEND_BUFER[4]=0x20;
//	SEND_BUFER[5]=0x07;
//	crc1=crc16((unsigned char*)&SEND_BUFER[3],3);//У�����������3��4��5������Ԫ�ء�
//	SEND_BUFER[6]=crc1&0x00ff;//У��͵ĵ�λ��
//	SEND_BUFER[7]=crc1>>8;		 //У��͵ĸ�λ��
//	for(i=0;i<3;i++)//��ֹһ�ζ�ȡ���ɹ���
//	{
//		i++;
//		SendPacketToLcd(SEND_BUFER,8);
//		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
//		if( (uart5_receive_data[19]==uart5_receive_data[18])&&(uart5_receive_data[18]==uart5_receive_data[17])&&(uart5_receive_data[19]==0) )
//		{
//			
//		}
//		else
//		{
//			i=4;//����forѭ����
//		}
//	}
////	*data++=calendar.sec;
////	*data++=calendar.min;
////	*data++=calendar.hour;
////	*data++=calendar.w_date;
////	*data++=calendar.w_month;
////	*data=calendar.w_year%100;
////	time_data[0]=uart5_receive_data[13];//��
////	time_data[1]=uart5_receive_data[14];//��
////	time_data[2]=uart5_receive_data[15];//��
////	time_data[3]=uart5_receive_data[17];//ʱ	
////	time_data[4]=uart5_receive_data[18];//��	
////	time_data[5]=uart5_receive_data[19];//��
//	time_data[0]=uart5_receive_data[19];//��
//	time_data[1]=uart5_receive_data[18];//��	
//	time_data[2]=uart5_receive_data[17];//ʱ	
//	time_data[3]=uart5_receive_data[15];//��
//	time_data[4]=uart5_receive_data[14];//��
//	time_data[5]=uart5_receive_data[13];//��
//	
//// 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19	20 21
////5A A5 02 81 FF 21 A0 5A A5 0C 81 20 07 11 11 12 06 05 28 22 CA DE	

////	uart5_receive_data[uart5_receive_data_inc]=data;
////	uart5_receive_data_inc=uart5_receive_data_inc+1;
////	USART1_Send_Data(time_data,6);	
////	USART1_Send_Data(uart5_receive_data,50);	
//  return 0;
//}
//extern const unsigned char LocalID[6];
//void Read_LCDSet_HostInterface(void)
//{
//	unsigned char buf[0x20];
//	unsigned char buf1[0x20];
//	W25q16_Lock();
//	memset(buf,0,0x20);
//	Readflash_Config(buf,HOST_CONFIG_CMD);
//	host_fixed=buf[0];
//	
//	memset(buf,0,0x20);
//	Readflash_Config(buf,CAN_CONFIG_CMD);
//	CANbaund_fixed=buf[0x0];
//	CANbaund=CANbaund_fixed;

//	memset(buf,0,0x20);
//	Readflash_Config(buf,RS485_CONFIG_CMD);
//	rs232baund_fixed=buf[0x0];
//	rs232parity_fixed=buf[0x4];
//	rs232stopbit_fixed=buf[0x5];
//	rs232baund=rs232baund_fixed;
//	rs232parity=rs232parity_fixed;
//	rs232stopbit=rs232stopbit_fixed;

//	memset(buf,0,0x20);
//	Readflash_Config(buf,LCD_CONFIG_CMD);
//	Backlight_brightness_fixed=buf[0x0];
//	Backlight_time_fixed=buf[0x01];
//	Backlight_brightness=Backlight_brightness_fixed;
//	Backlight_time=Backlight_time_fixed;

//	sound_fixed=buf[0x02];
//	sound=sound_fixed;
//	memcpy(password_two,&buf[4],4);
//	
//	memset(buf,0,0x20);
//	Readflash_Config(buf,SERVER_INTERFACE_CONFIG_CMD);
//	net_fixed=buf[0];

//	memset(buf,0,0x20);
//	Readflash_Config(buf,ID_CONFIG_CMD);
//	memcpy(Local_Addres,buf,6);

//	memset(buf,0,0x20);
//	Readflash_Config(buf,VER_CONFIG_CMD);
//	memcpy(Local_SoftVer,&buf[8],2);
//	
//	memset(buf,0,0x20);
//	Readflash_Config(buf,TIMING_CMD);
//	correction_time_way=buf[0];
//	W25q16_UnLock();
//	
//	
////	RTC_Get((char *)buf);	
////	sec=buf[0];
////	minute=buf[1];
////	hours=buf[2];
////	date=buf[3];
////	month=buf[4];
////	year=buf[5];
//}
//void LCD_Init(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
//	LcdUart_init(115200,USART_Parity_No);
//	Read_LCDSet_HostInterface();
//}
//void LCDTime_Init(void)
//{
//	unsigned char buf[0x07];
//	unsigned char buf1[0x07];
//	RTC_Get((char *)buf);	
//	buf1[0]=buf[5];
//	buf1[1]=buf[4];
//	buf1[2]=buf[3];
//	buf1[3]=buf[2];
//	buf1[4]=buf[1];
//	buf1[5]=buf[0];
////	printf("������\r\n");
////	dwin_time_set(buf1);
//	RTC_Set(buf1);
//}
//unsigned char BitToNum(unsigned int bit)
//{
//	int a=0;
//	if(bit==0) return 0xff;
//	else
//	{
//		while((bit%2==0)&&(bit!=0))
//		{
//			bit=bit/2;
//			a++;
//		}
//	}
//	return a;
//}

//void handle_arrays_u8(unsigned char *data,unsigned char size,unsigned char *printf_data)//�������飬DATA��Ҫ��������飬SIZE������ĳ��ȡ�//���罫0x25�ֿ�����0x32��0x35.��0xAB�ֿ�����0x40,0x41. 
//{//DATA����������飬SIZE��������Ԫ�ص�������Printf_data������������ڲ��Դ�ӡ�����顣
//	unsigned char i=0;
//	unsigned char j=0;	
//	unsigned char DATA_Printf_H_L_middle=0;
//	unsigned char DATA_Printf_H=0;
//	unsigned char DATA_Printf_L=0;
//	for(i=0;i<size;i++)
//	{
//		DATA_Printf_H_L_middle=data[i];
//		DATA_Printf_H_L_middle=DATA_Printf_H_L_middle&0xF0;		
//		DATA_Printf_H=(DATA_Printf_H_L_middle>>4);
//		switch(DATA_Printf_H)//�ж�Buzzer_flag���ж�Ӧ���������
//		{
//		 case 0x00:		
//				DATA_Printf_H=0x30;
//				break;	 			
//		 case 0x01:		
//				DATA_Printf_H=0x31;		 
//				break;	
//		 case 0x02:		
//				DATA_Printf_H=0x32;				 
//				break;	 			
//		 case 0x03:		
//				DATA_Printf_H=0x33;				 
//				break;	
//		 case 0x04:		
//				DATA_Printf_H=0x34;				 
//				break;	
//		 case 0x05:		
//				DATA_Printf_H=0x35;				 
//				break;	 			
//		 case 0x06:		
//				DATA_Printf_H=0x36;				 
//				break;	
//		 case 0x07:		
//				DATA_Printf_H=0x37;				 
//				break;	 			
//		 case 0x08:		
//				DATA_Printf_H=0x38;				 
//				break;	
//		 case 0x09:		
//				DATA_Printf_H=0x39;				 
//				break;		 
//		 case 0x0A:		
//				DATA_Printf_H=0x41;				 
//				break;	 			
//		 case 0x0B:		
//				DATA_Printf_H=0x42;				 
//				break;	
//		 case 0x0C:		
//				DATA_Printf_H=0x43;				 
//				break;	 			
//		 case 0x0D:		
//				DATA_Printf_H=0x44;				 
//				break;	
//		 case 0x0E:		
//				DATA_Printf_H=0x45;				 
//				break;		 
//		 case 0x0F:		
//				DATA_Printf_H=0x46;				 
//				break;	 
//		 default:
//				DATA_Printf_H=0x46;			 
//				break;
//		}			
//		DATA_Printf_H_L_middle=data[i];
//		DATA_Printf_H_L_middle=DATA_Printf_H_L_middle&0x0F;
//		DATA_Printf_L=DATA_Printf_H_L_middle;
//		switch(DATA_Printf_L)//�ж�Buzzer_flag���ж�Ӧ���������
//		{
//		 case 0x00:		
//				DATA_Printf_L=0x30;
//				break;	 			
//		 case 0x01:		
//				DATA_Printf_L=0x31;		 
//				break;	
//		 case 0x02:		
//				DATA_Printf_L=0x32;				 
//				break;	 			
//		 case 0x03:		
//				DATA_Printf_L=0x33;				 
//				break;	
//		 case 0x04:		
//				DATA_Printf_L=0x34;				 
//				break;	
//		 case 0x05:		
//				DATA_Printf_L=0x35;				 
//				break;	 			
//		 case 0x06:		
//				DATA_Printf_L=0x36;				 
//				break;	
//		 case 0x07:		
//				DATA_Printf_L=0x37;				 
//				break;	 			
//		 case 0x08:		
//				DATA_Printf_L=0x38;				 
//				break;	
//		 case 0x09:		
//				DATA_Printf_L=0x39;				 
//				break;		 
//		 case 0x0A:		
//				DATA_Printf_L=0x41;				 
//				break;	 			
//		 case 0x0B:		
//				DATA_Printf_L=0x42;				 
//				break;	
//		 case 0x0C:		
//				DATA_Printf_L=0x43;				 
//				break;	 			
//		 case 0x0D:		
//				DATA_Printf_L=0x44;				 
//				break;	
//		 case 0x0E:		
//				DATA_Printf_L=0x45;				 
//				break;		 
//		 case 0x0F:		
//				DATA_Printf_L=0x46;				 
//				break;	 
//		 default:
//				DATA_Printf_L=0x46;			 
//				break;
//		}		
//		printf_data[(i*2)+0]=DATA_Printf_H;
//		printf_data[(i*2)+1]=DATA_Printf_L;	
//	}	
//}
//void display_text(unsigned int add,unsigned char *data,unsigned char lenth)//add����ʾ��λ�á�data�����ݡ�lenth����data������Ԫ�ص�������
//{
//	unsigned char i=0;
//	unsigned int crc1=0;
//	unsigned char SEND_BUFER[100]={0x20};			//���崮�ڷ�������
////1090	10AA	10C4
////10E0	10FA	1114
////1130	114A	1164
//		SEND_BUFER[0]=0x5A;
//		SEND_BUFER[1]=0xA5;
//		SEND_BUFER[2]=lenth+5;//���ݳ��ȡ�			
//		SEND_BUFER[3]=0x82;//������������
//		SEND_BUFER[4]=(unsigned char)(add>>8);	//��ַ��λ
//		SEND_BUFER[5]=(unsigned char)add;		//��ַ��λ
//		for(i=0;i<lenth;i++)
//		{
//			if(data[i]==0)
//			{
//				SEND_BUFER[6+i]=0x20;
//			}
//			else
//			{
//				SEND_BUFER[6+i]=data[i];
//			}
//		}	
//		crc1=crc16((unsigned char*)&SEND_BUFER[3],lenth+3);
//		SEND_BUFER[lenth+6]=crc1&0x00ff;
//		SEND_BUFER[lenth+7]=crc1>>8;
//		SendPacketToLcd(SEND_BUFER,lenth+8);		
//}




//void input_alarm(unsigned int add,unsigned int sn,unsigned char *data,unsigned char Host_NO)//add����ʾ��λ�á�sn����ʾ�ڼ�ҳ��data�����ݡ�
//{
//	unsigned char breakdown_flag=0;//���ϱ�־λ��
//	unsigned char i=0;
//	unsigned int num=0;
//	unsigned char t=0;
//	unsigned char data_long=0;
//	unsigned int crc1=0;
//	unsigned char SEND_BUFER[100]={0x20};			//���崮�ڷ�������
////	unsigned char HostNO;
//	unsigned int  LoopNO,PartNO;
//	unsigned char data_lenth=0;
//	unsigned char test1[40]={0};	
//	unsigned char test2[40]={0};
//	
//	unsigned char time_data[40]={0x20};//ʱ������
//	unsigned char time_data_lenth=0;	

//	unsigned char type_data[40]={0x20};//��������
//	unsigned char type_data_lenth=0;		
//	
//	unsigned char message_data[40]={0x20};//��Ϣ����
//	unsigned char message_data_lenth=0;		

//	unsigned char index_data[40]={0x20};//�������
//	unsigned char index_data_lenth=0;
//	
//	unsigned int  Loop_Part=0;//�û���������
//	
//	if( (data[0x19]==0)&&(data[0x18]==0)&&(data[0x17]==0)&&(data[0x16]==0) )
//	{

//	}
//	else
//	{
///******************��ջ�������*****************/
//	memset(SEND_BUFER,0x20,100);//��100��0x20�Ž�SEND_BUFER�С����д���������顣
///******************��ȡ��������*****************/
//	memset(index_data,0x20,40);//��100��0x20�Ž�SEND_BUFER�С�
//		if(sn==0)//���ҳ��Ϊ0��˵������ʾ����ҳ��������Ϣ�� ��ҳ��������Ϣ������ʾҳ�롣
//		{

//		}
//		else//��ʾ�������ݵ�������
//		{
//			index_data_lenth=40;
//			t=3;
//			index_data[t++]=sn/1000+0x30;
//			index_data[t++]=(sn%1000)/100+0x30;
//			index_data[t++]=((sn%1000)%100)/10+0x30;
//			index_data[t++]=((sn%1000)%100)%10+0x30;
//			index_data[t++]=0x20;
//			index_data[t++]=0x20;
//			index_data[t++]=0x20;
//			index_data[t++]=0x20;
//			index_data[t++]=0x20;
//			index_data[t++]=0x20;
//			index_data[t++]=0x20;
//			index_data[t++]=0x20;
//		}
///******************��ȡʱ��*****************/
//		memset(time_data,0x20,40);//��100��0x20�Ž�SEND_BUFER�С�	
//		if(sn==0)//ҳ�롣 
//		{
//			time_data_lenth=50;
//			t=5;
//		}
//		else
//		{
//			time_data_lenth=40;
//			t=2;
//		}
//		
//		if(data[0]!=0x20)//ҳ�롣 
//		{
//		time_data[t++]=data[0x1b]%100/10+0x30;
//		time_data[t++]=data[0x1b]%10+0x30;
//		time_data[t++]=0xC4;		//��
//		time_data[t++]=0xEA;
//		time_data[t++]=data[0x1a]%100/10+0x30;
//		time_data[t++]=data[0x1a]%10+0x30;
//		time_data[t++]=0xD4;		//��
//		time_data[t++]=0xC2;
//		time_data[t++]=data[0x19]%100/10+0x30;
//		time_data[t++]=data[0x19]%10+0x30;
//		time_data[t++]=0xC8;		//��
//		time_data[t++]=0xD5;
//		time_data[t++]=0x20;		//�ո�
//		time_data[t++]=data[0x18]%100/10+0x30;
//		time_data[t++]=data[0x18]%10+0x30;
//		time_data[t++]=0x3A;		//��
//		time_data[t++]=data[0x17]%100/10+0x30;
//		time_data[t++]=data[0x17]%10+0x30;
//		time_data[t++]=0x3A;
//		time_data[t++]=data[0x16]%100/10+0x30;
//		time_data[t++]=data[0x16]%10+0x30;
//	}
////		time_data_lenth=t;
///******************��ȡ����*****************/
//		memset(type_data,0x20,40);//��100��0x20�Ž�SEND_BUFER�С�
//		switch(data[0])//�ж��ֽڳ��ȡ�
//		{
//			case 1:						//�ϴ�����������ʩϵͳ״̬
//				num=BitToNum(data[4]+data[5]*256);
////			  num=data[4]+data[5]*256;
//				if(num<14)
//				{
//					data_lenth=strlen((char *)&HostStatus[num]);//����״̬��
//					data_lenth=data_lenth/2;
//				}
//				break;
//			case 2:						//�ϴ�����������ʩ��������״̬
////				num=BitToNum(data[9]+data[10]*256);
////				if(num<9)
////				{
////					data_lenth=strlen((char *)&HostPartsStatus[num]);
////					data_lenth=data_lenth/2;
////				}		

//			
////				num=BitToNum(data[2]);
////			  num=BitToNum(data[4]+data[5]*256);
//				num=data[9]+data[10]*256;			
////				if(num<8)
////				{
//					data_lenth=input_extensionStatus(test1,0,num,test2);//����Ҫ��ʾ�Ľ�����������״̬
//					data_lenth=data_lenth/2;
////				}				
//			break;
//			case 4:						//�ϴ�����������ʩ������Ϣ
////				num=BitToNum(data[4]);
////			num=data[4];
//			num=DataCharge(data[4]);
//				if(num<7)
//				{
//					data_lenth=strlen((char *)&HostOperation[num]);
//					data_lenth=data_lenth/2;
//				}
//			break;
//			case 24:						//�ϴ��û���Ϣ����װ�ò�����Ϣ
//			num=BitToNum(data[2]);
////				  num=data[2];
//				if(num<8)
//				{
//					data_lenth=strlen((char *)&LocalOperation[num]);
//					data_lenth=data_lenth/2;
//				}
//			break;			
//			case 21://��������
//					data_lenth=2;
//			break;
//			default:
////					data_lenth=2;
//			break;
//		}
////�ж���ӿո�ĸ�����
//		if(data_lenth==2)//�жϣ����������2�����֡�
//		{
//			t=12;				
//		}
//		else if(data_lenth==4)//�жϣ����������4�����֡�
//		{
//			t=10;
//		}
//		else if(data_lenth==5)//�жϣ����������5�����֡�
//		{
//			t=9;
//		}		
//		else if(data_lenth==6)//�жϣ����������6�����֡�
//		{
//			t=8;
//		}
//		else if(data_lenth==7)//�жϣ����������7�����֡�
//		{
//			t=7;
//		}		
//		else
//		{
//			t=6;		
//		}
//		if(sn==0)//ҳ�롣 
//		{
//			type_data_lenth=50;
//		}
//		else
//		{
//			type_data_lenth=40;
//			t=t-6;
//		}
//		switch(data[0])
//		{
//			case 1:						//�ϴ�����������ʩϵͳ״̬			
//			  Host_NO=data[3]>>4;	//Э�鿨�ţ�
//			  num=BitToNum(data[4]+data[5]*256);
////			  num=data[4]+data[5]*256;
//				if(num<14)
//				{
//					i=strlen((char *)&HostStatus[num]);
//					memcpy((char *)&type_data[t],(char *)&HostStatus[num],i);
//					t+=i;
//				}
//				breakdown_flag=1;//���ϱ�־λ��
//				break;
//			case 2:						//�ϴ�����������ʩ��������״̬
////				num=BitToNum(data[9]+data[10]*256);
////				if(num<9)
////				{
////					i=strlen((char *)&HostPartsStatus[num]);
////					memcpy((char *)&type_data[t],(char *)&HostPartsStatus[num],i);
////					t+=i;
////				}
////				HostNO=data[3];
////				LoopNO=data[6];
////				LoopNO=LoopNO<<8;
////				LoopNO=LoopNO+data[5];
////				PartNO=data[8];
////				PartNO=PartNO<<8;
////				PartNO=PartNO+data[7];				
////				breakdown_flag=1;//���ϱ�־λ��


//			
////				num=BitToNum(data[2]);
////					num=BitToNum(data[9]+data[10]*256);	
//					num=data[9]+data[10]*256;				
////				if(num<9)
////				{
//					data_lenth=input_extensionStatus(type_data,t,num,test1);//����Ҫ��ʾ�Ľ�����������״̬
//					for(i=0;i<(t+data_lenth);i++)
//					{
//						type_data[i]=test1[i];
//					}
//					
////				}
//				Host_NO=data[3]>>4;	//Э�鿨�ţ�
//				LoopNO=data[6];
//				LoopNO=LoopNO<<8;
//				LoopNO=LoopNO+data[5];
//				PartNO=data[8];
//				PartNO=PartNO<<8;
//				PartNO=PartNO+data[7];				
//				breakdown_flag=1;//���ϱ�־λ��							
//			break;	
//			case 4:						//�ϴ�����������ʩ������Ϣ
////				num=BitToNum(data[4]);
////			  num=data[4];		
//			Host_NO=data[3]>>4;	//Э�鿨�ţ�			
//			num=DataCharge(data[4]);			
//				if(num<7)
//				{
//					i=strlen((char *)&HostOperation[num]);
//					memcpy((char *)&type_data[t],(char *)&HostOperation[num],i);
//					t+=i;
//				}
//				breakdown_flag=1;
//			break;	
//			case 24:						//�ϴ��û���Ϣ����װ�ò�����Ϣ
//				num=BitToNum(data[2]);
////			  num=data[2];
//				if(num<8)
//				{
//					i=strlen((char *)&LocalOperation[num]);
//					memcpy((char *)&type_data[t],(char *)&LocalOperation[num],i);
//					t+=i;
//				}
//			break;
////			case ?:						//������������״̬���������ͬ����
////				num=BitToNum(data[2]);
////				if(num<8)
////				{				
////					input_extensionStatus(type_data,t,num,test1,data_lenth);//����Ҫ��ʾ�Ľ�����������״̬
////					for(i=0;i<(t+data_lenth);i++)
////					{
////						type_data[i]=data_lenth[i];
////					}
////				}
////			break;						
//				
//////			case ?:						//������������״̬���������ͬ����
//////				num=BitToNum(data[2]);
//////				if(num<8)
//////				{
//////					i=strlen((char *)&ExtensionStatus[num]);
//////					memcpy((char *)&type_data[t],(char *)&ExtensionStatus[num],i);
//////					t+=i;				
//////				}
//////			break;				
//			case 21://��������
////					memcpy(&SEND_BUFER[t],"��������",4);
////					t+=8;

//					memcpy(&type_data[t],"����",4);
//					t+=4;
//			break;
//			default:
//			break;
//		}
////		type_data_lenth=t+1;
///******************��ȡ��Ϣ*****************/
//		memset(message_data,0x20,40);//��100��0x20�Ž�SEND_BUFER�С�
//		if(breakdown_flag==1)//��ʾ�������������˹��ϣ���ʾ�������͡�
//		{
//			breakdown_flag=0;
//			if(sn==0)//ҳ�롣 
//			{//�������ҳ����ʾ��3����Ϣ
//				if(Host_NO<=9)
//				{
////				message_data[4]=0x31;//�������	��
//					message_data[6]=Host_NO+'0';//�������	��					
//				}
//				else
//				{
//					switch(Host_NO)
//					{
//					 case 0x0A:		
//						message_data[5]=0x31;
//						message_data[6]=0x30;					 
//							break;	 			
//					 case 0x0B:		
//						message_data[5]=0x31;
//						message_data[6]=0x31;					 	 
//							break;	
//					 case 0x0C:		
//						message_data[5]=0x31;
//						message_data[6]=0x32;					 		 
//							break;	 			
//					 case 0x0D:		
//						message_data[5]=0x31;
//						message_data[6]=0x33;					 		 
//							break;	
//					 case 0x0E:		
//						message_data[5]=0x31;
//						message_data[6]=0x34;					 			 
//							break;	
//					 case 0x0F:		
//						message_data[5]=0x31;
//						message_data[6]=0x35;					 		 
//							break;	 
//					 case 0x10:		
//						message_data[5]=0x31;
//						message_data[6]=0x36;					 		 
//							break;	 					 
//					 default:
//						message_data[5]=0x30;
//						message_data[6]=0x30;					 	 
//							break;
//					}		
//				}
//				message_data[7]='#';					
//			}
//			else
//			{//�������Ϣ��ѯ������ʾ��6����Ϣ
//////				message_data[1]=0x31;//�������	��
////				message_data[3]=HostNO+'0';//�������	��	
//////				message_data[1]=host_number;//�������롣				
////				message_data[4]='#';		

//				if(Host_NO<=9)
//				{
////				message_data[4]=0x31;//�������	��
//					message_data[3]=Host_NO+'0';//�������	��					
//				}
//				else
//				{
//					switch(Host_NO)
//					{
//					 case 0x0A:		
//						message_data[2]=0x31;
//						message_data[3]=0x30;					 
//							break;	 			
//					 case 0x0B:		
//						message_data[2]=0x31;
//						message_data[3]=0x31;					 	 
//							break;	
//					 case 0x0C:		
//						message_data[2]=0x31;
//						message_data[3]=0x32;					 		 
//							break;	 			
//					 case 0x0D:		
//						message_data[2]=0x31;
//						message_data[3]=0x33;					 		 
//							break;	
//					 case 0x0E:		
//						message_data[2]=0x31;
//						message_data[3]=0x34;					 			 
//							break;	
//					 case 0x0F:		
//						message_data[2]=0x31;
//						message_data[3]=0x35;					 		 
//							break;	
//					 case 0x10:		
//						message_data[5]=0x31;
//						message_data[6]=0x36;					 		 
//							break;					 
//					 default:
//						message_data[2]=0x30;
//						message_data[3]=0x30;					 	 
//							break;
//					}		
//				}
//				message_data[4]='#';	

//				
//			}
//		}
//		if(data[0]==21)
//		{//�������Ϣ�����м������֡�
//			num=data[2]>>3;
//			if(num<15)
//			{
//				data_lenth=strlen((char *)&FaultCode[num]);
//				data_lenth=data_lenth/2;//���㺺�ֵ�����������2�����֣�Ҳ��1�����ֵĳ��ȡ�һ����������2��asc����ɵġ�
//			}
//			if(data_lenth==2)//�жϣ����������2�����֡�
//			{
//				t=12;
//			}
//			else if(data_lenth==3)//�жϣ����������3�����֡�
//			{
//				t=11;
//			}
//			else if(data_lenth==4)//�жϣ����������4�����֡�
//			{
//				t=10;
//			}	
//			else if(data_lenth==7)//�жϣ����������7�����֡�
//			{
//				t=7;
//			}
//			else if(data_lenth==9)//�жϣ����������9�����֡�
//			{
//				t=5;
//			}
//		}
//		else
//		{
//			t=7;
//		}
//		if(sn==0)//ҳ�롣 
//		{
//			message_data_lenth=50;
//		}
//		else
//		{
//			message_data_lenth=40;
//			t=t-3;
//		}
//		if(data[0]==21)
//		{//�û���Ϣ����װ�ñ�����Ϣ��
//			num=data[2]>>3;
//			if(num<15)
//			{
////				message_data[t++]=0x31;	 //  1
////				message_data[t++]=0x23;	 //  #
////				message_data[t++]=0x20;	 //   	
//				i=strlen((char *)&FaultCode[num]);
//				memcpy((char *)&message_data[t],(char *)&FaultCode[num],i);
//				t+=i;
//			}
//		}
//		else if(data[0]==2)
//		{
////			message_data[t++]=0x31;	 //  1
////			message_data[t++]=0x23;	 //  #
////			message_data[t++]=0x20;	 //   
//			t++;			
//			t++;	

//			if(data[11]==0x11)//�Ǳ������Ϣ��int�����ݣ���ʾ�û��������롣
//			{
//				Loop_Part=data[8];
//				Loop_Part=Loop_Part<<8;
//				Loop_Part=Loop_Part+data[7];
//				Loop_Part=Loop_Part<<8;
//				Loop_Part=Loop_Part+data[6];
//				Loop_Part=Loop_Part<<8;
//				Loop_Part=Loop_Part+data[5];					
//				//   /ȡ��
//				//   %ȡ��
//				message_data[t++]=Loop_Part/100000000+0x30;
//				message_data[t++]=(Loop_Part%100000000)/10000000+0x30;				
//				message_data[t++]=(Loop_Part%10000000)/1000000+0x30;		
//				message_data[t++]=(Loop_Part%1000000)/100000+0x30;					
//				message_data[t++]=(Loop_Part%100000)/10000+0x30;				
//				message_data[t++]=(Loop_Part%10000)/1000+0x30;				
//				message_data[t++]=(Loop_Part%1000)/100+0x30;
//				message_data[t++]=(Loop_Part%100)/10+0x30;					
//				message_data[t++]= Loop_Part%10+0x30;												
//			}
//			else//char�����ݣ���ʾ�����Ļ�·�������롣
//			{			
//				message_data[t++]=LoopNO/100+0x30;
//				message_data[t++]=(LoopNO%100)/10+0x30;
//				message_data[t++]=LoopNO%10+0x30;
//				message_data[t++]=0xBB;//��·
//				message_data[t++]=0xD8;
//				message_data[t++]=0xC2;
//				message_data[t++]=0xB7;
//				message_data[t++]=PartNO/100+0x30;
//				message_data[t++]=(PartNO%100)/10+0x30;
//				message_data[t++]=PartNO%10+0x30;
//				message_data[t++]=0xBA;//��
//				message_data[t++]=0xC5;
//			}
//		}
////		message_data_lenth=t;
///******************************************/	
////ע�⣡�ٸ���Ļ�������ݵ�ʱ��һ��Ҫ������֡���ݣ�û���ݵĵط����Ϳո񡣱���������롣
//		switch(add)//�ж���ʾ��ַ��
//		{
////0x1090	0x10AA	0x10C4
////0x10E0	0x10FA	0x1114
////0x1130	0x114A	0x1164				
//			case 0x1090://��ҳ�ĵ�һ����Ϣ
//			display_text(0x1090,time_data,time_data_lenth);//��ʾʱ��
//			display_text(0x10AA,type_data,type_data_lenth);//��ʾ����		
//			display_text(0x10C4,message_data,message_data_lenth);//��ʾ��Ϣ
//			break;
//			case 0x10E0://��ҳ�ĵڶ�����Ϣ
//			display_text(0x10E0,time_data,time_data_lenth);			
//			display_text(0x10FA,type_data,type_data_lenth);			
//			display_text(0x1114,message_data,message_data_lenth);				
//			break;
//			case 0x1130://��ҳ�ĵ�������Ϣ
//			display_text(0x1130,time_data,time_data_lenth);
//			display_text(0x114A,type_data,type_data_lenth);
//			display_text(0x1164,message_data,message_data_lenth);		
//			break;	
////1240	1254	1268	127C
////1290	12A4	12B8	12CC
////12E0	12F4	1308	131C
////1330	1344	1358	136C
////1390	13A4	13B8	13CC
////13E0	13F4	1408	141C
//			case 0x1240://��Ϣҳ�ĵ�һ����Ϣ
//			display_text(0x1240,index_data,index_data_lenth);
//			display_text(0x1254,time_data,time_data_lenth);			
//			display_text(0x1268,type_data,type_data_lenth);
//			display_text(0x127C,message_data,message_data_lenth);				
//			break;				
//			case 0x1290://��Ϣҳ�ĵڶ�����Ϣ
//			display_text(0x1290,index_data,index_data_lenth);
//			display_text(0x12A4,time_data,time_data_lenth);			
//			display_text(0x12B8,type_data,type_data_lenth);
//			display_text(0x12CC,message_data,message_data_lenth);				
//			break;			
//			case 0x12E0://��Ϣҳ�ĵ�������Ϣ
//			display_text(0x12E0,index_data,index_data_lenth);
//			display_text(0x12F4,time_data,time_data_lenth);			
//			display_text(0x1308,type_data,type_data_lenth);
//			display_text(0x131C,message_data,message_data_lenth);				
//			break;				
//			case 0x1330://��Ϣҳ�ĵ�������Ϣ
//			display_text(0x1330,index_data,index_data_lenth);
//			display_text(0x1344,time_data,time_data_lenth);			
//			display_text(0x1358,type_data,type_data_lenth);
//			display_text(0x136C,message_data,message_data_lenth);				
//			break;								
//			case 0x1390://��Ϣҳ�ĵ�������Ϣ
//			display_text(0x1390,index_data,index_data_lenth);
//			display_text(0x13A4,time_data,time_data_lenth);			
//			display_text(0x13B8,type_data,type_data_lenth);
//			display_text(0x13CC,message_data,message_data_lenth);				
//			break;					
//			case 0x13E0://��Ϣҳ�ĵ�������Ϣ
//			display_text(0x13E0,index_data,index_data_lenth);
//			display_text(0x13F4,time_data,time_data_lenth);			
//			display_text(0x1408,type_data,type_data_lenth);
//			display_text(0x141C,message_data,message_data_lenth);				
//			break;		
//			default:
//			break;			
//		}
//	}
//}
//void fun0(void)//��ҳ��ʾ���б�����Ϣ��
//{
//	u16 add0=0x1001;//��
//	u16 add1=0x1003;//��
//	u16 add2=0x1005;//���ݽ���ͼ��
//	u16 add3=0x1006;//������ʽ
//	u16 add4=0x1015;//�ֶ���������
//	u16 add5=0x101A;//�Զ���������
//	u16 add6=0x1020;//���ϱ�������
//	u16 add7=0x1030;//�ֶ�����
//	u16 add8=0x1040;//�Զ�����
//	u16 add9=0x1050;//���ϱ���
//	u16 add10=0x1060;//ʱ��
//	u16 add11=0x1070;//����
//	u16 add12=0x1080;//��Ϣ
//	u16 add13=0x1090;//��һ������
//	u16 add14=0x10E0;//�ڶ�������
//	u16 add15=0x1130;//����������
//	u16 add16=0x1182;//����
//	u16 add17=0x1192;//��Ϣ
//	unsigned char SEND_TEXT[100];//���崮��2��������
//	unsigned char len=0;
//	unsigned char Host_NO=0;//�������	
//	change_back_picture(0x0001);//��ȡ��ҳ����ͼƬ
//	input_text(add0,2,"��");//��ʾ����
//	input_text(add1,2,"��");//��ʾ����
//	input_text(add7,8,"�ֶ�����");
//	input_text(add8,8,"�Զ�����");
//	input_text(add9,8,"���ϱ���");
//	input_text(add10,4,"ʱ��");
//	input_text(add11,4,"����");
//	input_text(add12,4,"��Ϣ");
//	input_text(add16,4,"����");
//	input_text(add17,4,"��Ϣ");

////	LCDTime_Init();
////	LCD_SET_OFF();//������Ļ

//	//�Ƿ������ϴ������������ݡ����ݽ�����0��ʱ��û�����ݣ�1��ʱ�������ݡ�
//	switch(signal_flag)
//	{
//		case 0:	input_small_picture(add2,0000);	break;
//		case 1:	input_small_picture(add2,0005);	break;
//		default:	break;
//	}
//	
//	W25q16_Lock();
//	Read_Record(SEND_TEXT);
//	W25q16_UnLock();
//	Lcd_Flash_Cnt=SEND_TEXT[8]+(SEND_TEXT[9]<<8)+(SEND_TEXT[10]<<16)+(SEND_TEXT[11]<<24);
//	Manual_alarm_Cnt=SEND_TEXT[0x13]*256+SEND_TEXT[0x12];
//	Host_alarm_Cnt=SEND_TEXT[0x11]*256+SEND_TEXT[0x10];
//	Host_Flult_Cnt=SEND_TEXT[0x15]*256+SEND_TEXT[0x14];
//	num2str(Manual_alarm_Cnt,SEND_TEXT);
//	input_text(add4,3,SEND_TEXT);
//	num2str(Host_alarm_Cnt,SEND_TEXT);
//	input_text(add5,3,SEND_TEXT);
//	num2str(Host_Flult_Cnt,SEND_TEXT);
//	input_text(add6,3,SEND_TEXT);
//	//0��ʱ�������źţ�1��ʱ�����������ӣ�2��ʱ�����������ӡ�
//	len=strlen((char *)&SetNetMode[net_fixed]);
//	input_text(add3,len,(unsigned char *)&SetNetMode[net_fixed]);
//	if(fun0_Cursor==0)					//ѡ������
//	{
//		input_small_picture(0x1181,0001);
//		input_small_picture(0x1900,0001);
//		input_small_picture(0x1190,0000);
//		input_small_picture(0x1910,0000);
//		input_small_picture(0x1181,0001);
//		input_small_picture(0x1900,0001);
//		input_small_picture(0x1190,0000);
//		input_small_picture(0x1910,0000);		
//	}
//	else								//ѡ����Ϣ��ѯ
//	{
//		input_small_picture(0x1181,0000);
//		input_small_picture(0x1900,0000);
//		input_small_picture(0x1190,0001);
//		input_small_picture(0x1910,0001);
//		input_small_picture(0x1181,0000);
//		input_small_picture(0x1900,0000);
//		input_small_picture(0x1190,0001);
//		input_small_picture(0x1910,0001);		
//	}
//	W25q16_Lock();
//	memset(SEND_TEXT,0,0x20);//��bufferȫ���ÿո�
//	if(Lcd_Flash_Cnt>0)
//	{
//		Readflash_Infor(SEND_TEXT, 1);//��ȡflash�д洢����Ϣ��
//	}
//	input_alarm(add13,0,SEND_TEXT,Host_NO);//��ҳ��ʾ��һ����Ϣ����ʾ��ַΪadd13����0ҳ����������ΪSEND_TEXT�е����ݡ�
//	memset(SEND_TEXT,0,0x20);//��bufferȫ���ÿո�
//	if(Lcd_Flash_Cnt>1)
//	{
//		Readflash_Infor(SEND_TEXT, 2);
//	}
//	input_alarm(add14,0,SEND_TEXT,Host_NO);//��ҳ��ʾ�ڶ�����Ϣ
//	memset(SEND_TEXT,0,0x20);//��bufferȫ���ÿո�
//	if(Lcd_Flash_Cnt>2)
//	{
//		Readflash_Infor(SEND_TEXT, 3);
//	}
//	input_alarm(add15,0,SEND_TEXT,Host_NO);//��ҳ��ʾ��������Ϣ
//	W25q16_UnLock();
//}
///*****************************************************************************
//* ��������: unsigned char  Sting_Compare(unsigned char  *Sting1,unsigned char  *Sting2,unsigned char Len)
//* ��������: �ַ����Ƚ�
//* ��    ��:Sting1���ַ���1;Sting2���ַ���2;Len:�ַ���2�ĳ���
//* ����  ֵ:0���ַ�������ͬ��1���ַ�����ͬ
//*****************************************************************************/
//unsigned char  Sting_Compare(unsigned char  *Sting1,unsigned char  *Sting2,unsigned char Len)//�ַ����Ƚ�
//{
//	unsigned char i = 0; 
//	unsigned char j = 0; 	
//	unsigned char Flag = 1;
//	j=strlen(Sting1);
//	if(j==Len)
//	{
//		for(i=0;i<Len;i++)
//		{
//			if(Sting1[i]!=Sting2[i])
//			{
//							Flag = 0;
//			}
//		}		
//	}
//	else
//	{
//		Flag=0;
//	}
//	return Flag;
//}

//void fun1(void)//��ʾ������Ϣ��
//{
////	unsigned char time_data[6]={0};
//	unsigned char SEND_TEXT[100];
//	unsigned char Host_NO=0;//�������	
//	unsigned char Sting2[8]={0xB1,0xBE,0xBB,0xFA,0xB8,0xB4,0xCE,0xBB};//������λ
//	unsigned RS_flag=0;
////	RTC_Get(time_data);
//	if(fun1_page_fixed<=1)
//	{
//		fun1_page_fixed=1;
//	}
//	change_back_picture(0x0003);	//�л��ײ�ͼƬ����Ϣ��ѯ����
//	
//	memset(SEND_TEXT,0x20,100);
//	input_text(0x1480,5,SEND_TEXT);
//	//����ȫ��������ѡ�С�
//	input_small_picture(0x14A5,0x0007);//����������
//	input_small_picture(0x14A6,0x0007);
//	input_small_picture(0x14A7,0x0007);
//	input_small_picture(0x14A8,0x0007);
//	input_small_picture(0x14A9,0x0007);
//	input_small_picture(0x14AA,0x0009);
//	input_small_picture(0x14AB,0x0007);
//	if(fun1_Cursor!=4)fun1_page=fun1_page_fixed;
//	if(fun1_page>=100)//��λ��
//	{
//		SEND_TEXT[0]=fun1_page/100+0x30;
//		SEND_TEXT[1]=(fun1_page%100)/10+0x30;
//		SEND_TEXT[2]=fun1_page%10+0x30;
//		SEND_TEXT[3]=0xD2;
//		SEND_TEXT[4]=0xB3;
//		input_text(0x1480,5,SEND_TEXT);//��ʾҳ��
//	}
//	else if(fun1_page>=10)
//	{
//		SEND_TEXT[0]=(fun1_page%100)/10+0x30;
//		SEND_TEXT[1]=fun1_page%10+0x30;
//		SEND_TEXT[2]=0xD2;
//		SEND_TEXT[3]=0xB3;
//		input_text(0x1480,4,SEND_TEXT);
//	}
//	else
//	{
//		SEND_TEXT[0]=fun1_page+0x30;
//		SEND_TEXT[1]=0xD2;
//		SEND_TEXT[2]=0xB3;
//		input_text(0x1480,3,SEND_TEXT);
//	}

//	W25q16_Lock();
//	Read_Record(SEND_TEXT);
//	Lcd_Flash_Cnt=SEND_TEXT[8]+(SEND_TEXT[9]<<8)+(SEND_TEXT[10]<<16)+(SEND_TEXT[11]<<24);

//	memset(SEND_TEXT,0x20,100);
//	if(Lcd_Flash_Cnt>(fun1_page_fixed-1)*6)
//	{
//		Readflash_Infor(SEND_TEXT,(fun1_page_fixed-1)*6+1);
//	}
//	//������������λ����������ʾҳ�������ݣ�
//	if(
//		((SEND_TEXT[0]==0x18)&&(SEND_TEXT[2]==0x01))||(RS_flag==1)
//	  )
//	{
//		RS_flag=1;	
//	}
//	else
//	{
//		RS_flag=0;	
//	}
//	///
//	input_alarm(0x1240,(fun1_page_fixed-1)*6+1,SEND_TEXT,Host_NO);	
//	
// 	memset(SEND_TEXT,0x20,100);
//	if(Lcd_Flash_Cnt>(fun1_page_fixed-1)*6+1)
//	{
//		if(RS_flag==0)
//		{
//		Readflash_Infor(SEND_TEXT,(fun1_page_fixed-1)*6+2);
//		}
//	}
//		//������������λ����������ʾҳ�������ݣ�
//	if(
//		((SEND_TEXT[0]==0x18)&&(SEND_TEXT[2]==0x01))||(RS_flag==1)
//	  )
//	{
//		RS_flag=1;
//	}
//	else
//	{
//		RS_flag=0;		
//	}
//	////
// 	input_alarm(0x1290,(fun1_page_fixed-1)*6+2,SEND_TEXT,Host_NO);	
//	
//	memset(SEND_TEXT,0x20,100);		
//	if(Lcd_Flash_Cnt>(fun1_page_fixed-1)*6+2)
//	{
//		if(RS_flag==0)
//		{
//		Readflash_Infor(SEND_TEXT,(fun1_page_fixed-1)*6+3);
//		}
//	}
//	//������������λ����������ʾҳ�������ݣ�
//	if(
//		((SEND_TEXT[0]==0x18)&&(SEND_TEXT[2]==0x01))||(RS_flag==1)
//	  )
//	{
//		RS_flag=1;	
//	}
//	else
//	{
//		RS_flag=0;	
//	}
//	////
//	input_alarm(0x12E0,(fun1_page_fixed-1)*6+3,SEND_TEXT,Host_NO);
//	
//	memset(SEND_TEXT,0x20,100);
//	if(Lcd_Flash_Cnt>(fun1_page_fixed-1)*6+3)
//	{
//		if(RS_flag==0)
//		{
//		Readflash_Infor(SEND_TEXT,(fun1_page_fixed-1)*6+4);
//		}
//	}
//	//������������λ����������ʾҳ�������ݣ�
//	if(
//		((SEND_TEXT[0]==0x18)&&(SEND_TEXT[2]==0x01))||(RS_flag==1)
//	  )
//	{
//		RS_flag=1;
//	}
//	else
//	{
//		RS_flag=0;		
//	}
//	////
//	input_alarm(0x1330,(fun1_page_fixed-1)*6+4,SEND_TEXT,Host_NO);		
//	
//	memset(SEND_TEXT,0x20,100);
//	if(Lcd_Flash_Cnt>(fun1_page_fixed-1)*6+4)
//	{
//		if(RS_flag==0)
//		{
//		Readflash_Infor(SEND_TEXT,(fun1_page_fixed-1)*6+5);
//		}
//	}
//			//������������λ����������ʾҳ�������ݣ�
//	if(
//		((SEND_TEXT[0]==0x18)&&(SEND_TEXT[2]==0x01))||(RS_flag==1)
//	  )
//	{
//		RS_flag=1;			
//	}
//	else
//	{
//		RS_flag=0;	
//	}
//	////
//	input_alarm(0x1390,(fun1_page_fixed-1)*6+5,SEND_TEXT,Host_NO);		
//	
//	memset(SEND_TEXT,0x20,100);
//	if(Lcd_Flash_Cnt>(fun1_page_fixed-1)*6+5)
//	{
//		if(RS_flag==0)
//		{
//		Readflash_Infor(SEND_TEXT,(fun1_page_fixed-1)*6+6);
//		}
//	}
//	//������������λ����������ʾҳ�������ݣ�
//	if(
//		((SEND_TEXT[0]==0x18)&&(SEND_TEXT[2]==0x01))||(RS_flag==1)
//	  )
//	{
//		RS_flag=1;
//	}
//	else
//	{
//		RS_flag=0;		
//	}
//	////
//	input_alarm(0x13E0,(fun1_page_fixed-1)*6+6,SEND_TEXT,Host_NO);
//	
//	W25q16_UnLock();
//	if(fun1_Cursor<7) input_small_picture(0x14A5+fun1_Cursor,0x0006);
//	else input_small_picture(0x14A5,0x0006);
//}
//void fun2(void)
//{
//	unsigned char SEND_TEXT[40];	//���崮��2��������
//	unsigned int add0=0x14D0;		//��������ĵ�0�ŵ�ַ
//	unsigned int add1=0x14D2;		//��������ĵ�1�ŵ�ַ
//	unsigned int add2=0x14D4;		//��������ĵ�2�ŵ�ַ
//	unsigned int add3=0x14D6;		//��������ĵ�3�ŵ�ַ
//	unsigned int add4=0x14C0;		//����������ʾ����
//	unsigned int add5=0x1901;		//��Ϣ��ʾ��
//	
//	memset(SEND_TEXT,0x20,40);
//	change_back_picture(0x0004);	//�л��ײ�ͼƬ�����������������
//	input_text(add4,28,SEND_TEXT);	//��ʾ����յ�ǰ������ʾ��λ�����֡�
//	input_text(add5,28,SEND_TEXT);	//��ʾ����յ�ǰ������ʾ��λ�����֡�
//	
//	switch(password_three_ok)
//	{
//		case 0:
//			input_text(add4,10,"����������");//��ʾ������������
//		break;
//		case 2:
//			input_text(add4,14,"��������������");//�������
//		break;
//		default:			break;
//	}
//	//��ʾ������
//	SEND_TEXT[0]=0x20;
//	SEND_TEXT[1]=input_password[0];
//	input_text(add0,2,SEND_TEXT);
//	SEND_TEXT[0]=0x20;
//	SEND_TEXT[1]=input_password[1];
//	input_text(add1,2,SEND_TEXT);
//	SEND_TEXT[0]=0x20;
//	SEND_TEXT[1]=input_password[2];
//	input_text(add2,2,SEND_TEXT);
//	SEND_TEXT[0]=0x20;
//	SEND_TEXT[1]=input_password[3];
//	input_text(add3,2,SEND_TEXT);
//	
////	memset(SEND_TEXT,0x20,40);
////	input_text(0x1870,24,SEND_TEXT);//��ʾ����յ�ǰ������ʾ��λ�����֡�
//}
//void fun3(void)
//{
//	unsigned char i=0;
//	change_back_picture(0x0005);
//	for(i=0;i<6;i++)
//	{
//		input_small_picture(0x1503+i,0x0000);
//	}
//	if(fun3_Cursor<6) input_small_picture(0x1503+fun3_Cursor,0x0002);
//}
//void fun4(void){}
//void fun5(void){}
//void fun6(void)
//{
//	input_small_picture(0x1570,0x0000);
//	input_small_picture(0x1571,0x0000);
//	input_text(0x1001,2,"��");
//	input_text(0x1003,2,"��");
//	switch(fun6_Cursor)
//	{
//		case 0:			//ѡ������  		
//			change_back_picture(0x0006);
//		break;		
//		case 1:			//ѡ������	
//			change_back_picture(0x0007);
//		break;
////		case 2:			//ѡ�з�����
////			change_back_picture(0x0008);
//////			input_text(0x15B0,22,"����:www.fubangyun.com");
//////			input_text(0x15E0,12,"�˿ں�:10086");
////		break;
//		case 2:			//ѡ�б���		
//			input_small_picture(0x1570,0x0001);//����ȥ�����ѡ�С�
//			input_small_picture(0x1571,0x0000);//ȡ��ȥ����겻ѡ�С�			
//			change_back_picture(0x0008);
//		break;	
//		case 3:			//ѡ���˳�
//			input_small_picture(0x1570,0x0000);//����ȥ�����ѡ�С�
//			input_small_picture(0x1571,0x0001);//ȡ��ȥ����겻ѡ�С�			
//			change_back_picture(0x0008);
//		break;	
//		default:	break;
//	}
//	switch(net_fixed)
//	{
//		case 1:					//ѡ������	
//			input_small_picture(0x1630,0x000A);//С���ѡ�С�
//			input_small_picture(0x1631,0x0000);//С���ѡ�С�
//			input_small_picture(0x1633,0x0000);//С���ѡ�С�		
//		break;
//		case 2:					//ѡ������
//			input_small_picture(0x1630,0x0000);//С���ѡ�С�		
//			input_small_picture(0x1631,0x000A);//С���ѡ�С�
//			input_small_picture(0x1633,0x0000);//С���ѡ�С�		
//		break;
////		case 3:					//ѡ�з�����
////			input_small_picture(0x1630,0x0000);//С���ѡ�С�		
////			input_small_picture(0x1631,0x0000);//С���ѡ�С�		
////			input_small_picture(0x1633,0x000A);//С���ѡ�С�		
////		break;		
//		default:	break;
//	}
//}
//void fun7(void){}
//void fun8(void){}
//void fun9(void)//����ͨ�����á�Э��忨��Ϣ��
//{
//	unsigned int i=0;	
//	unsigned char Config[256]={0};//��ȡflash�д洢���û���Ϣ����װ����Ϣ��
//	unsigned char data_local[100]={0x20};
//	unsigned char test[20]={0};	
//	unsigned char printf_data[40]={0};		
//	unsigned int 	port=0;
////	unsigned char step=0;//��경��λ�á�		
////	unsigned char step=0;//��경��λ�á�
//	unsigned int 	fun_add0=0x2B00;	
//	unsigned int 	fun_add1=0x2B10;	
//	unsigned int 	fun_add2=0x2B20;	
//	unsigned int 	fun_add3=0x2B30;	
//	unsigned int 	fun_add4=0x2B40;	
//	unsigned int 	fun_add5=0x2B50;	
//	unsigned int 	fun_add6=0x2B60;	
//	unsigned int 	fun_add7=0x2B70;	
//	unsigned int 	fun_add8=0x2B80;	
//	unsigned int 	fun_add9=0x2B90;	
//	unsigned int 	fun_addA=0x2BA0;	
//	unsigned int 	fun_addB=0x2BB0;	
//	unsigned int 	fun_addC=0x2BC0;	
//	unsigned int 	fun_addD=0x2BD0;	
//	unsigned int 	fun_addE=0x2BE0;	
//	unsigned int 	fun_addF=0x2BF0;		
//	
//	unsigned int 	fun_add00=0x2D00;	
//	unsigned int 	fun_add01=0x2D01;	
//	unsigned int 	fun_add02=0x2D02;	
//	unsigned int 	fun_add03=0x2D03;	
//	unsigned int 	fun_add04=0x2D04;	
//	unsigned int 	fun_add05=0x2D05;	
//	unsigned int 	fun_add06=0x2D06;	
//	unsigned int 	fun_add07=0x2D07;	
//	unsigned int 	fun_add08=0x2D08;	
//	unsigned int 	fun_add09=0x2D09;	
//	unsigned int 	fun_add0A=0x2D0A;	
//	unsigned int 	fun_add0B=0x2D0B;	
//	unsigned int 	fun_add0C=0x2D0C;	
//	unsigned int 	fun_add0D=0x2D0D;	
//	unsigned int 	fun_add0E=0x2D0E;	
//	unsigned int 	fun_add0F=0x2D0F;	
//	unsigned int 	fun_add0G=0x2D10;	
//	unsigned int 	fun_add0H=0x2D11;	
//	unsigned int 	fun_add0I=0x2D12;		

//	change_back_picture(0x0032);//��ȡ�û���Ϣ����װ�û�����Ϣ����ͼƬ��

////�������ҳ���������ı����е����֡�
////ͨ��SPI��ȡЭ�鿨����Ϣ��
//	
////�����ӿ���ʾ��Ϣ��
//	
////�忨����		
////��ʾЭ�����ͣ������ͺţ���
////�汾�ţ�Э���������汾�ţ�
////�ӿ����ͷ�ʽ��CAN/RS485�������ʡ�У��λ������λ��ֹͣλ��


////const unsigned char BKXH_msg[20]="XSBD-FB801Z-N";
////const unsigned char XFZJ_msg[20]="JB_QBL_bdqn11s";
////const unsigned char SOFT_msg[40]="FB801Z_BJBDQN11S_V1.0_20180131";
////const unsigned char ITFS_msg[20]="RS232 2400"

//////�忨�ͺţ������ͺţ�����汾���ӿ�˵����
////const unsigned char BKXH_msg[20]="XSBD-FB801Z-N";
////const unsigned char XFZJ_msg[20]="JB_QBL_bdqn11s";
////const unsigned char SOFT_msg[40]="FB801Z_BJBDQN11S_V1.0_20180131";
////const unsigned char ITFS_msg[20]="RS232 2400";	

//////��ʾ�忨�ͺţ������ͺţ���
////	memset(data_local,0x20,100);//��100��0x20�Ž�SEND_BUFER�С�		
////	memcpy(data_local," �忨���ͣ� ",12);
////	for(i=0;i<20;i++)//���buffer
////	{
////		data_local[12+i]=BK_Message[i];
////	}		
////	input_text(0x2B00,60,data_local);
//////��ʾ�����ͺţ������ͺţ���
////	memcpy(data_local," �����ͺţ� ",12);
////	for(i=0;i<20;i++)//���buffer
////	{
////		data_local[12+i]=BK_Message[i+20];
////	}			
////	input_text(0x2B50,60,data_local);
//////����汾��Э���������汾��
////	memcpy(data_local," �汾�ţ� ",10);
////	for(i=0;i<40;i++)//���buffer
////	{
////		data_local[10+i]=BK_Message[i+40];
////	}			
////	input_text(0x2C00,60,data_local);
//////�ӿ�˵����CAN/RS485�������ʡ�У��λ������λ��ֹͣλ��	
////	memcpy(data_local," �ӿ�˵���� ",12);
////	for(i=0;i<20;i++)//���buffer
////	{
////		data_local[12+i]=BK_Message[i+80];
////	}			
////	input_text(0x2C50,60,data_local);	
//	
//	
//	
//	
//	
////	input_small_picture(0x1631,0x0000);	//������
////	input_small_picture(0x1632,0x0000);
////	//��ת�����ý����Ƭ
////	switch(fun9_Cursor)
////	{
////		case 0:				//ѡ��RS232/RS485
////			change_back_picture(0x000A);
////		break;
////		case 1:				//ѡ��CAN
////			change_back_picture(0x000B);
////		break;
////		case 2:				//ѡ�б���
////			change_back_picture(0x000C);
////			input_small_picture(0x1631,0x0001);
////			input_small_picture(0x1632,0x0000);
////		break;
////		case 3:				//ѡ���˳�
////			change_back_picture(0x000C);
////			input_small_picture(0x1631,0x0000);
////			input_small_picture(0x1632,0x0001);
////		break;		
////		default:		break;
////	}
////	switch(host_fixed)
////	{
////		case 1:				//ѡ��485
////			input_small_picture(0x1640,0x000A);
////			input_small_picture(0x1641,0x0000);
////		break;
////		case 2:				//ѡ��CAN
////			input_small_picture(0x1640,0x0000);
////			input_small_picture(0x1641,0x000A);
////		break;
////		default:		break;
////	}


////С���ѡ�С�
//		input_small_picture(fun_add00,0x0000);
//		input_small_picture(fun_add01,0x0000);
//		input_small_picture(fun_add02,0x0000);
//		input_small_picture(fun_add03,0x0000);
//		input_small_picture(fun_add04,0x0000);
//		input_small_picture(fun_add05,0x0000);
//		input_small_picture(fun_add06,0x0000);
//		input_small_picture(fun_add07,0x0000);
//		input_small_picture(fun_add08,0x0000);
//		input_small_picture(fun_add09,0x0000);
//		input_small_picture(fun_add0A,0x0000);
//		input_small_picture(fun_add0B,0x0000);
//		input_small_picture(fun_add0C,0x0000);
//		input_small_picture(fun_add0D,0x0000);
//		input_small_picture(fun_add0E,0x0000);
//		input_small_picture(fun_add0F,0x0000);
//		input_small_picture(fun_add0G,0x0000);
//		input_small_picture(fun_add0H,0x0000);
//		input_small_picture(fun_add0I,0x0000);		
////�鿴ѡ�����ĸ����		
//		switch(fun9_Cursor)
//		{
//			case 0:		//���ѡ��0�Ű忨
//				input_small_picture(fun_add00,0x000A);
//			break;
//			case 1:		//���ѡ��1�Ű忨
//				input_small_picture(fun_add01,0x000A);
//			break;
//			case 2:		//���ѡ��2�Ű忨
//				input_small_picture(fun_add02,0x000A);
//			break;
//			case 3:		//���ѡ��3�Ű忨
//				input_small_picture(fun_add03,0x000A);
//			break;
//			case 4:		//���ѡ��4�Ű忨
//				input_small_picture(fun_add04,0x000A);
//			break;
//			case 5:		//���ѡ��5�Ű忨
//				input_small_picture(fun_add05,0x000A);
//			break;
//			case 6:		//���ѡ��6�Ű忨
//				input_small_picture(fun_add06,0x000A);
//			break;
//			case 7:		//���ѡ��7�Ű忨
//				input_small_picture(fun_add07,0x000A);
//			break;
//			case 8:		//���ѡ��8�Ű忨
//				input_small_picture(fun_add08,0x000A);
//			break;
//			case 9:		//���ѡ��9�Ű忨
//				input_small_picture(fun_add09,0x000A);
//			break;
//			case 10:		//���ѡ��10�Ű忨
//				input_small_picture(fun_add0A,0x000A);
//			break;
//			case 11:		//���ѡ��11�Ű忨
//				input_small_picture(fun_add0B,0x000A);
//			break;
//			case 12:		//���ѡ��12�Ű忨
//				input_small_picture(fun_add0C,0x000A);
//			break;
//			case 13:		//���ѡ��13�Ű忨
//				input_small_picture(fun_add0D,0x000A);
//			break;
//			case 14:		//���ѡ��14�Ű忨
//				input_small_picture(fun_add0E,0x000A);
//			break;
//			case 15:		//���ѡ��15�Ű忨
//				input_small_picture(fun_add0F,0x000A);
//			break;
//			case 16:		//���ѡ��ע��忨
//				input_small_picture(fun_add0G,0x000A);
//			break;
//			case 17:		//���ѡ�б������
//				input_small_picture(fun_add0H,0x000A);
//			break;
//			case 18:		//���ѡ���˳�
//				input_small_picture(fun_add0I,0x000A);
//			break;				
//			default:		
//				break;
//		}
//		if((register_flag&0x0001)!=0x0001)
//		{
//			input_text(fun_add0,10,"00�Ű忨 ");
//		}
//		else
//		{
//			input_text(fun_add0,16,"00�Ű忨��ע�� ");			
//		}			
//		if((register_flag&0x0002)!=0x0002)
//		{
//			input_text(fun_add1,10,"01�Ű忨 ");			
//		}
//		else
//		{
//			input_text(fun_add1,16,"01�Ű忨��ע�� ");			
//		}
//		if((register_flag&0x0004)!=0x0004)	
//		{
//			input_text(fun_add2,10,"02�Ű忨 ");
//		}
//		else
//		{
//			input_text(fun_add2,16,"02�Ű忨��ע�� ");			
//		}
//		if((register_flag&0x0008)!=0x0008)
//		{
//			input_text(fun_add3,10,"03�Ű忨 ");
//		}
//		else
//		{
//			input_text(fun_add3,16,"03�Ű忨��ע�� ");			
//		}					
//		if((register_flag&0x0010)!=0x0010)
//		{
//			input_text(fun_add4,10,"04�Ű忨 ");
//		}
//		else
//		{
//			input_text(fun_add4,16,"04�Ű忨��ע�� ");			
//		}					
//		if((register_flag&0x0020)!=0x0020)	
//		{
//			input_text(fun_add5,10,"05�Ű忨 ");
//		}
//		else
//		{
//			input_text(fun_add5,16,"05�Ű忨��ע�� ");			
//		}					
//		if((register_flag&0x0040)!=0x0040)
//		{
//			input_text(fun_add6,10,"06�Ű忨 ");
//		}
//		else
//		{
//			input_text(fun_add6,16,"06�Ű忨��ע�� ");			
//		}					
//		if((register_flag&0x0080)!=0x0080)
//		{
//			input_text(fun_add7,10,"07�Ű忨 ");
//		}
//		else
//		{
//			input_text(fun_add7,16,"07�Ű忨��ע�� ");			
//		}					
//		if((register_flag&0x0100)!=0x0100)	
//		{
//			input_text(fun_add8,10,"08�Ű忨 ");
//		}
//		else
//		{
//			input_text(fun_add8,16,"08�Ű忨��ע�� ");			
//		}					
//		if((register_flag&0x0200)!=0x0200)
//		{
//			input_text(fun_add9,10,"09�Ű忨 ");
//		}
//		else
//		{
//			input_text(fun_add9,16,"09�Ű忨��ע�� ");			
//		}					
//		if((register_flag&0x0400)!=0x0400)
//		{
//			input_text(fun_addA,10,"10�Ű忨 ");
//		}
//		else
//		{
//			input_text(fun_addA,16,"10�Ű忨��ע�� ");			
//		}					
//		if((register_flag&0x0800)!=0x0800)	
//		{
//			input_text(fun_addB,10,"11�Ű忨 ");
//		}
//		else
//		{
//			input_text(fun_addB,16,"11�Ű忨��ע��");			
//		}					
//		if((register_flag&0x1000)!=0x1000)	
//		{
//			input_text(fun_addC,10,"12�Ű忨 ");
//		}
//		else
//		{
//			input_text(fun_addC,16,"12�Ű忨��ע�� ");			
//		}					
//		if((register_flag&0x2000)!=0x2000)
//		{
//			input_text(fun_addD,10,"13�Ű忨 ");
//		}
//		else
//		{
//			input_text(fun_addD,16,"13�Ű忨��ע�� ");			
//		}
//		if((register_flag&0x4000)!=0x4000)
//		{
//			input_text(fun_addE,10,"14�Ű忨 ");
//		}
//		else
//		{
//			input_text(fun_addE,16,"14�Ű忨��ע�� ");			
//		}					
//		if((register_flag&0x8000)!=0x8000)	
//		{
//			input_text(fun_addF,10,"15�Ű忨 ");
//		}
//		else
//		{
//			input_text(fun_addF,16,"15�Ű忨��ע�� ");			
//		}		
////		if((register_flag&0x0001)!=0x0001)
////		{
////			input_text(fun_add0,16,"00�Ű忨δע�� ");
////		}
////		else
////		{
////			input_text(fun_add0,16,"00�Ű忨��ע�� ");			
////		}			
////		if((register_flag&0x0002)!=0x0002)
////		{
////			input_text(fun_add1,16,"01�Ű忨δע�� ");			
////		}
////		else
////		{
////			input_text(fun_add1,16,"01�Ű忨��ע�� ");			
////		}
////		if((register_flag&0x0004)!=0x0004)	
////		{
////			input_text(fun_add2,16,"02�Ű忨δע�� ");
////		}
////		else
////		{
////			input_text(fun_add2,16,"02�Ű忨��ע�� ");			
////		}
////		if((register_flag&0x0008)!=0x0008)
////		{
////			input_text(fun_add3,16,"03�Ű忨δע�� ");
////		}
////		else
////		{
////			input_text(fun_add3,16,"03�Ű忨��ע�� ");			
////		}					
////		if((register_flag&0x0010)!=0x0010)
////		{
////			input_text(fun_add4,16,"04�Ű忨δע�� ");
////		}
////		else
////		{
////			input_text(fun_add4,16,"04�Ű忨��ע�� ");			
////		}					
////		if((register_flag&0x0020)!=0x0020)	
////		{
////			input_text(fun_add5,16,"05�Ű忨δע�� ");
////		}
////		else
////		{
////			input_text(fun_add5,16,"05�Ű忨��ע�� ");			
////		}					
////		if((register_flag&0x0040)!=0x0040)
////		{
////			input_text(fun_add6,16,"06�Ű忨δע�� ");
////		}
////		else
////		{
////			input_text(fun_add6,16,"06�Ű忨��ע�� ");			
////		}					
////		if((register_flag&0x0080)!=0x0080)
////		{
////			input_text(fun_add7,16,"07�Ű忨δע�� ");
////		}
////		else
////		{
////			input_text(fun_add7,16,"07�Ű忨��ע�� ");			
////		}					
////		if((register_flag&0x0100)!=0x0100)	
////		{
////			input_text(fun_add8,16,"08�Ű忨δע�� ");
////		}
////		else
////		{
////			input_text(fun_add8,16,"08�Ű忨��ע�� ");			
////		}					
////		if((register_flag&0x0200)!=0x0200)
////		{
////			input_text(fun_add9,16,"09�Ű忨δע�� ");
////		}
////		else
////		{
////			input_text(fun_add9,16,"09�Ű忨��ע�� ");			
////		}					
////		if((register_flag&0x0400)!=0x0400)
////		{
////			input_text(fun_addA,16,"10�Ű忨δע�� ");
////		}
////		else
////		{
////			input_text(fun_addA,16,"10�Ű忨��ע�� ");			
////		}					
////		if((register_flag&0x0800)!=0x0800)	
////		{
////			input_text(fun_addB,16,"11�Ű忨δע�� ");
////		}
////		else
////		{
////			input_text(fun_addB,16,"11�Ű忨��ע��");			
////		}					
////		if((register_flag&0x1000)!=0x1000)	
////		{
////			input_text(fun_addC,16,"12�Ű忨δע�� ");
////		}
////		else
////		{
////			input_text(fun_addC,16,"12�Ű忨��ע�� ");			
////		}					
////		if((register_flag&0x2000)!=0x2000)
////		{
////			input_text(fun_addD,16,"13�Ű忨δע�� ");
////		}
////		else
////		{
////			input_text(fun_addD,16,"13�Ű忨��ע�� ");			
////		}
////		if((register_flag&0x4000)!=0x4000)
////		{
////			input_text(fun_addE,16,"14�Ű忨δע�� ");
////		}
////		else
////		{
////			input_text(fun_addE,16,"14�Ű忨��ע�� ");			
////		}					
////		if((register_flag&0x8000)!=0x8000)	
////		{
////			input_text(fun_addF,16,"15�Ű忨δע�� ");
////		}
////		else
////		{
////			input_text(fun_addF,16,"15�Ű忨��ע�� ");			
////		}
//}
//void fun10(void){}
//void fun11(void){}
//void fun12(void)//��ʾÿ���忨������Ϣ���忨�ͺţ������ͺţ�����汾���ӿ�˵����
//{
//	unsigned char i=0;	
//	unsigned char data_local[100]={0x20};	
//	unsigned char len=0;
//	u16 fun_add0=0x2D50;	
//	u16 fun_add1=0x2E00;
//	u16 fun_add2=0x2E50;
//	u16 fun_add3=0x2F00;
//	u16 fun_add4=0x2F50;	
//	
////	input_small_picture(0x1680,0x0000);//����ȥ�����ѡ�С�
////	input_small_picture(0x1681,0x0000);//ȡ��ȥ�����ѡ�С�
////	input_text(add1,16,"                ");			
////	input_text(add2,16,"                ");	
////	input_text(add3,16,"                ");		
////	switch(fun12_Cursor)
////	{
////		case 0:		//����������
////			change_back_picture(0x000D);
////		break;
////		case 1:		//��żУ��λ
////			change_back_picture(0x000E);
////		break;
////		case 2:		//ֹͣλ
////			change_back_picture(0x000F);
////		break;
////		case 3:		//����
////			change_back_picture(0x0010);
////			input_small_picture(0x1680,0x0001);//����ȥ�����ѡ�С�
////			input_small_picture(0x1681,0x0000);//ȡ��ȥ�����ѡ�С�
////		break;
////		case 4:		//�˳�
////			change_back_picture(0x0010);
////			input_small_picture(0x1680,0x0000);//����ȥ�����ѡ�С�
////			input_small_picture(0x1681,0x0001);//ȡ��ȥ�����ѡ�С�
////		break;
////		default:		break;
////	}
////	len=strlen((char *)&SetRS232Baund[rs232baund]);
////	input_text(add1,len,(unsigned char *)&SetRS232Baund[rs232baund]);
////	
////	len=strlen((char *)&SetRS232Parity[rs232parity]);
////	input_text(add2,len,(unsigned char *)&SetRS232Parity[rs232parity]);
////	
////	len=strlen((char *)&SetRS232StopBit[rs232stopbit]);
////	input_text(add3,len,(unsigned char *)&SetRS232StopBit[rs232stopbit]);

//	change_back_picture(0x0033);//��ȡ�û���Ϣ����װ�û�����Ϣ����ͼƬ��
////	if( (BK_Message[fun9_Cursor][0]==0)&&(BK_Message[fun9_Cursor][1]==0)&&(BK_Message[fun9_Cursor][2]==0) )//������ְ忨û�б�ע�ᡣ
////	{
////		switch(fun9_Cursor)
////		{
////			case 0:		//0�Ű忨
////				input_text(fun_add0,14,"00�Ű忨δע��  ");
////			break;
////			case 1:		//1�Ű忨
////				input_text(fun_add0,14,"01�Ű忨δע��  ");
////			break;
////			case 2:		//2�Ű忨
////				input_text(fun_add0,14,"02�Ű忨δע��  ");
////			break;
////			case 3:		//3�Ű忨
////				input_text(fun_add0,14,"03�Ű忨δע��  ");
////			break;
////			case 4:		//4�Ű忨
////				input_text(fun_add0,14,"04�Ű忨δע��  ");
////			break;
////			case 5:		//5�Ű忨
////				input_text(fun_add0,14,"05�Ű忨δע��  ");
////			break;
////			case 6:		//6�Ű忨
////				input_text(fun_add0,14,"06�Ű忨δע��  ");
////			break;
////			case 7:		//7�Ű忨
////				input_text(fun_add0,14,"07�Ű忨δע��  ");
////			break;
////			case 8:		//8�Ű忨
////				input_text(fun_add0,14,"08�Ű忨δע��  ");
////			break;
////			case 9:		//9�Ű忨
////				input_text(fun_add0,14,"09�Ű忨δע��  ");
////			break;
////			case 10:	//10�Ű忨
////				input_text(fun_add0,14,"10�Ű忨δע��  ");
////			break;
////			case 11:	//11�Ű忨
////				input_text(fun_add0,14,"11�Ű忨δע��  ");
////			break;
////			case 12:	//12�Ű忨
////				input_text(fun_add0,14,"12�Ű忨δע��  ");
////			break;
////			case 13:	//13�Ű忨
////				input_text(fun_add0,14,"13�Ű忨δע��  ");
////			break;
////			case 14:	//14�Ű忨
////				input_text(fun_add0,14,"14�Ű忨δע��  ");
////			break;
////			case 15:	//15�Ű忨
////				input_text(fun_add0,14,"15�Ű忨δע��  ");
////			break;		
////			default:		
////				break;
////		}				
////	}
////	else
////	{
//		switch(fun9_Cursor)
//		{
//			case 0:		//���ѡ��0�Ű忨
//				input_text(fun_add0,12,"00�Ű忨��Ϣ  ");		
//			break;
//			case 1:		//���ѡ��1�Ű忨
//				input_text(fun_add0,12,"01�Ű忨��Ϣ  ");
//			break;
//			case 2:		//���ѡ��2�Ű忨
//				input_text(fun_add0,12,"02�Ű忨��Ϣ  ");
//			break;
//			case 3:		//���ѡ��3�Ű忨
//				input_text(fun_add0,12,"03�Ű忨��Ϣ  ");
//			break;
//			case 4:		//���ѡ��4�Ű忨
//				input_text(fun_add0,12,"04�Ű忨��Ϣ  ");
//			break;
//			case 5:		//���ѡ��5�Ű忨
//				input_text(fun_add0,12,"05�Ű忨��Ϣ  ");
//			break;
//			case 6:		//���ѡ��6�Ű忨
//				input_text(fun_add0,12,"06�Ű忨��Ϣ  ");
//			break;
//			case 7:		//���ѡ��7�Ű忨
//				input_text(fun_add0,12,"07�Ű忨��Ϣ  ");
//			break;
//			case 8:		//���ѡ��8�Ű忨
//				input_text(fun_add0,12,"08�Ű忨��Ϣ  ");
//			break;
//			case 9:		//���ѡ��9�Ű忨
//				input_text(fun_add0,12,"09�Ű忨��Ϣ  ");
//			break;
//			case 10:		//���ѡ��10�Ű忨
//				input_text(fun_add0,12,"10�Ű忨��Ϣ  ");
//			break;
//			case 11:		//���ѡ��11�Ű忨
//				input_text(fun_add0,12,"11�Ű忨��Ϣ  ");
//			break;
//			case 12:		//���ѡ��12�Ű忨
//				input_text(fun_add0,12,"12�Ű忨��Ϣ  ");
//			break;
//			case 13:		//���ѡ��13�Ű忨
//				input_text(fun_add0,12,"13�Ű忨��Ϣ  ");
//			break;
//			case 14:		//���ѡ��14�Ű忨
//				input_text(fun_add0,12,"14�Ű忨��Ϣ  ");
//			break;
//			case 15:		//���ѡ��15�Ű忨
//				input_text(fun_add0,12,"15�Ű忨��Ϣ  ");
//			break;
//			default:
//				break;
//		}
////	}



////�������ҳ���������ı����е����֡�
////ͨ��SPI��ȡЭ�鿨����Ϣ��
//	
////�����ӿ���ʾ��Ϣ��
//	
////�忨����		
////��ʾЭ�����ͣ������ͺţ���
////�汾�ţ�Э���������汾�ţ�
////�ӿ����ͷ�ʽ��CAN/RS485�������ʡ�У��λ������λ��ֹͣλ��


////const unsigned char BKXH_msg[20]="XSBD-FB801Z-N";
////const unsigned char XFZJ_msg[20]="JB_QBL_bdqn11s";
////const unsigned char SOFT_msg[40]="FB801Z_BJBDQN11S_V1.0_20180131";
////const unsigned char ITFS_msg[20]="RS232 2400"

//////�忨�ͺţ������ͺţ�����汾���ӿ�˵����
////const unsigned char BKXH_msg[20]="XSBD-FB801Z-N";
////const unsigned char XFZJ_msg[20]="JB_QBL_bdqn11s";
////const unsigned char SOFT_msg[40]="FB801Z_BJBDQN11S_V1.0_20180131";
////const unsigned char ITFS_msg[20]="RS232 2400";	


//////��ʾ�忨�ͺţ������ͺţ���
////	memset(data_local,0x20,100);//��100��0x20�Ž�SEND_BUFER�С�		
////	memcpy(data_local," �忨���ͣ� ",12);
////	for(i=0;i<20;i++)//���buffer
////	{
////		data_local[12+i]=BK_Message[i];
////	}		
////	input_text(0x2E00,60,data_local);
//////��ʾ�����ͺţ������ͺţ���
////	memcpy(data_local," �����ͺţ� ",12);
////	for(i=0;i<20;i++)//���buffer
////	{
////		data_local[12+i]=BK_Message[i+20];
////	}			
////	input_text(0x2E50,60,data_local);
//////����汾��Э���������汾��
////	memcpy(data_local," �汾�ţ� ",10);
////	for(i=0;i<40;i++)//���buffer
////	{
////		data_local[10+i]=BK_Message[i+40];
////	}
////	input_text(0x2F00,60,data_local);
//////�ӿ�˵����CAN/RS485�������ʡ�У��λ������λ��ֹͣλ��	
////	memcpy(data_local," �ӿ�˵���� ",12);
////	for(i=0;i<20;i++)//���buffer
////	{
////		data_local[12+i]=BK_Message[i+80];
////	}
////	input_text(0x2F50,60,data_local);	
//	
////..\HARDWARE\APPLCD\lcd.c(1980): error:  #142: expression must have pointer-to-object type data_local[12+i]=BK_Message[fun9_Cursor][i];
////��ʾ�忨�ͺţ������ͺţ���
//	memset(data_local,0x20,100);//��100��0x20�Ž�SEND_BUFER�С�		
//	memcpy(data_local,"�忨���ͣ�",10);
//	for(i=0;i<20;i++)//���buffer
//	{
//		data_local[10+i]=BK_Message[fun9_Cursor][i];
//	}
//	input_text(fun_add1,60,data_local);
////��ʾ�����ͺţ������ͺţ���
//	memset(data_local,0x20,100);//��100��0x20�Ž�SEND_BUFER�С�	
//	memcpy(data_local,"�����ͺţ�",10);
//	for(i=0;i<20;i++)//���buffer
//	{
//		data_local[10+i]=BK_Message[fun9_Cursor][i+20];
//	}			
//	input_text(fun_add2,60,data_local);
////����汾��Э���������汾��
//	memset(data_local,0x20,100);//��100��0x20�Ž�SEND_BUFER�С�
//	memcpy(data_local,"�汾�ţ�",8);
//	for(i=0;i<40;i++)//���buffer
//	{
//		data_local[8+i]=BK_Message[fun9_Cursor][i+40];
//	}
//	input_text(fun_add3,60,data_local);
////�ӿ�˵����CAN/RS485�������ʡ�У��λ������λ��ֹͣλ��	
//	memset(data_local,0x20,100);//��100��0x20�Ž�SEND_BUFER�С�
//	memcpy(data_local,"�ӿ�˵����",10);
//	for(i=0;i<20;i++)//���buffer
//	{
//		data_local[10+i]=BK_Message[fun9_Cursor][i+80];
//	}
//	input_text(fun_add4,60,data_local);		
//}
//void fun13(void){}
//void fun14(void){}
//void fun15(void)//ע��忨�ͱ���������
//{
//	unsigned char data_local[100]={0x20};
//	unsigned int schedule=0;//����
//	unsigned int i=0;
//	u16 add1=0x1701;
//	unsigned char len=0;
//	OS_ERR err;
////	LCDTime_Init();
////	OSTimeDlyHMSM(0,0,20,10,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ10ms
////	LCD_SET_OFF();//������Ļ
//	change_back_picture(0x0034);//��ȡ�û���Ϣ����װ�õ���������ͼƬ��
//	//������Ϣ�������˰�ȡ��������������ʹ�ܡ�
////	SDDataInFlagt=1;
////	SD_DATAIN(); //ִ�б�����뺯����
////	i=400;
//	cancel_flag=0;
//	if(register_card_flag==1)//�����ע��忨���档
//	{
//		Registered_Start=1;
//		register_card_flag=0;//��ձ�־λ��

//		memset(data_local,0x20,100);//
//		input_text(0x2F70,30,data_local);		
//		
//		memset(data_local,0x20,100);//
//		input_text(0x2F80,30,data_local);		

//		memset(data_local,0x20,100);//
//		input_text(0x2FA0,30,data_local);		
//		
//		memset(data_local,0x20,100);//
//		input_text(0x2FC0,30,data_local);		
//		
//		memset(data_local,0x20,100);//
//		input_text(0x2FE0,30,data_local);			
//		
//		memset(data_local,0x20,100);//
//		memcpy(data_local,"�忨ע��",8);
//		input_text(0x2F70,40,data_local);		
//		
//		memset(data_local,0x20,100);//
//		memcpy(data_local,"�忨����ע���У����Ժ򡣡���",28);	
//		input_text(0x2F80,30,data_local);		

//		for(i=60;i>0;i--)//1����ˢ��һ�ν��ȡ����ܴ����ҵ��ڡ���Ȼ����ָ�����Ȼ����ʾ���롣
//		{						
//         LCD_SET_ON();//������Ļ
//			  back_light_change(Backlight_brightness);//������Ļ����	
//				OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s								
//			  if( (i<2)&&(RegisteProComplete==0) )//���ʱ����0�ˡ�˵��ע��忨ʧ�ܡ����˳����档
//				{
//					memset(data_local,0x20,100);//
//					memcpy(data_local,"ע��忨ʧ��",12);
//					input_text(0x2FE0,13,data_local);				
//					OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ3s	
//					func_index=3;		
//				}
//				else
//				{
//					if(RegisteProComplete==1)//���ע����ϣ���������������ر���Ļ��
//					{
//						i=1;						
//						memset(data_local,0x20,100);//
//						memcpy(data_local,"ע��忨���",12);
//						input_text(0x2FE0,13,data_local);							
// 						OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ2s						
//						func_index=3;				
//						RegisteProComplete=0;//��ձ�־λ	
//						refresh_lcd();
//					}				
//				}			
//		}
////		LCD_SET_OFF();//������Ļ
////    refresh_lcd();		
//	}
//	else//������û����������档������û������ĵ��������ʾ��
//	{ 
//		SDDataInFlagt=1;
//		memset(data_local,0x20,100);//
//		memcpy(data_local,"�û��������",14);
//		input_text(0x2F70,16,data_local);		
//		for(i=10;i>0;i--)//1����ˢ��һ�ν��ȡ����ܴ����ҵ��ڡ���Ȼ����ָ�����Ȼ����ʾ���롣
//		{
//			LCD_SET_ON();//������Ļ
//			schedule=FBWBReadStatus;
//			memset(data_local,0x20,100);//
//			memcpy(data_local,"������ȣ�",10);
//			data_local[11]=schedule/100+0x30;// ȡ���ȵİ�λ������
//			data_local[12]=(schedule%100)/10+0x30;// ȡ���ȵ�ʮλ������
//			data_local[13]=((schedule%100)%10)%10+0x30;// ȡ���ȵĸ�λ������
//			data_local[14]='%';
//			input_text(0x2F80,30,data_local);
//			memset(data_local,0x20,100);//
//			memcpy(data_local,"��ǰ��ȡ��������",16);
//			data_local[17]=FBWBReadNbr/10000+0x30;// ȡ���ȵ���λ������
//			data_local[18]=(FBWBReadNbr%10000)/1000+0x30;// ȡ���ȵ�ǧλ������
//			data_local[19]=((FBWBReadNbr%10000)%1000)/100+0x30;// ȡ���ȵİ�λ������
//			data_local[20]=(((FBWBReadNbr%10000)%1000)%100)/10+0x30;// ȡ���ȵ�ʮλ������
//			data_local[21]=(((FBWBReadNbr%10000)%1000)%100)%10+0x30;// ȡ���ȵĸ�λ������
//			input_text(0x2FA0,30,data_local);

//			memset(data_local,0x20,100);//
//			memcpy(data_local,"��ǰ��ȡʧ�ܵ�������",20);
//			data_local[21]=FBWBReadFail/10000+0x30;// ȡ���ȵ���λ������		
//			data_local[22]=(FBWBReadFail%10000)/1000+0x30;// ȡ���ȵ�ǧλ������		
//			data_local[23]=((FBWBReadFail%10000)%1000)/100+0x30;// ȡ���ȵİ�λ������			
//			data_local[24]=(((FBWBReadFail%10000)%1000)%100)/10+0x30;// ȡ���ȵ�ʮλ������				
//			data_local[25]=(((FBWBReadFail%10000)%1000)%100)%10+0x30;// ȡ���ȵĸ�λ������			
//			input_text(0x2FC0,30,data_local);		
//	//if(cancel_flag==1)//��������У��˳���
//	//{
//	//	cancel_flag=0;
//	//	func_index=3;				
//	//	refresh_lcd();	
//	//}	
//			if(FBWBReadStatus>=100)
//			{
//				i=0;//����ѭ��
//				memset(data_local,0x20,100);//		
//				memcpy(data_local,"������������4���������",27);
//				input_text(0x2FE0,27,data_local);		
//				OSTimeDlyHMSM(0,0,4,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ4s	
//			  SoftReset();//���������
//				func_index=12;
//				break;
//			}
//			else
//			{
//				memset(data_local,0x20,100);//				
//				input_text(0x2FE0,30,data_local);			
//				if(i<2)//���ʱ����0�ˡ�˵��SD���𻵻���û�в忨�����˳����档
//				{
//					memset(data_local,0x20,100);//
//					memcpy(data_local,"δ����SD����SD������",22);
//					input_text(0x2FE0,23,data_local);				
//					OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s	
//	//				func_index=12;
//					func_index=3;				
//					refresh_lcd();
//				}
//				else
//				{
//					if(schedule>=2)
//					{
//						i=10;
//						memset(data_local,0x20,100);//	
//						input_text(0x2FE0,30,data_local);	
//					}	
//					else
//					{
//						memset(data_local,0x20,100);//		
//						memcpy(data_local,"����ʱ�䣺",10);
//						data_local[11]=i/100+0x30;// ȡ���ȵİ�λ������
//						data_local[12]=(i%100)/10+0x30;// ȡ���ȵ�ʮλ������
//						data_local[13]=((i%100)%10)%10+0x30;// ȡ���ȵĸ�λ������
//						data_local[14]=0xC3;//��
//						data_local[15]=0xEB;//
//						input_text(0x2FE0,16,data_local);						
//					}
//				}
//			}
//			OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s					
//		}		
//	}
//						

////		LCD_SET_ON();//�ر���Ļ
//}
//void fun16(void){}
//void fun17(void){}
//void fun18(void)//������ڡ�
//{
//	u16 add1=0x1733;
//	u16 add2=0x1734;
//	input_small_picture(add1,0x0000);
//	input_small_picture(add2,0x0000);
//	switch(fun18_Cursor)//
//	{
//		case 0:					//���ñ�������
//			back_light_change(Backlight_brightness);//��������
//			if(Backlight_brightness<10)
//			{
//				change_back_picture(0x0013+Backlight_brightness);
//			}
//			else
//			{
//				Backlight_brightness=0;
//				change_back_picture(0x0013);
//			}
//		break;
//		case 1:					//���ñ���ʱ��
//			if(Backlight_time<3)
//			{
//				change_back_picture(0x001D+Backlight_time);
//				Backlight__keep_time=LCDBright10S*(Backlight_time+1);
//			}
//			else
//			{
//				Backlight_time=0;
//				change_back_picture(0x001D);
//				Backlight__keep_time=LCDBright10S;
//			}
//		break;	
//		case 2:					//����
//			change_back_picture(0x0020);//�������Ⱥ�ʱ����ڶ�û��ѡ��
//			input_small_picture(add1,0x0001);//����ȥ�����ѡ�С�
//			input_small_picture(add2,0x0000);//ȡ��ȥ����겻ѡ�С�		
//		break;	
//		case 3:					//�˳�
//			change_back_picture(0x0020);//�������Ⱥ�ʱ����ڶ�û��ѡ��
//			input_small_picture(add1,0x0000);//����ȥ����겻ѡ�С�
//			input_small_picture(add2,0x0001);//ȡ��ȥ�����ѡ�С�		
//		break;		
//		default:
//		break;
//	}
//}
//void fun19(void){}
//void fun20(void){}
//void fun21(void)
//{
//	u8 SEND_TEXT[40];//���崮��2��������
//	u16 add0=0x1820;//��������ĵ�0�ŵ�ַ
//	u16 add1=0x1822;//��������ĵ�1�ŵ�ַ
//	u16 add2=0x1824;//��������ĵ�2�ŵ�ַ
//	u16 add3=0x1826;//��������ĵ�3�ŵ�ַ
//	u16 add4=0x1801;//������ʾ�ַ���λ��
//	change_back_picture(0x0021);//��������ͼƬ	

//	memset(SEND_TEXT,0x20,40);
//	input_text(add4,28,SEND_TEXT);//��ʾ����յ�ǰ������ʾ��λ�����֡�
//	//��ʾ��ǰ���������ֵ
//	if(input_password_times==0)
//	{	//��ʾ����ĵ�һ�����롣
//		input_text(add4,10,"����������");
//		SEND_TEXT[0]=0x20;
//		SEND_TEXT[1]=input_password[0];
//		input_text(add0,2,SEND_TEXT);
//		SEND_TEXT[0]=0x20;
//		SEND_TEXT[1]=input_password[1];
//		input_text(add1,2,SEND_TEXT);
//		SEND_TEXT[0]=0x20;
//		SEND_TEXT[1]=input_password[2];
//		input_text(add2,2,SEND_TEXT);
//		SEND_TEXT[0]=0x20;
//		SEND_TEXT[1]=input_password[3];
//		input_text(add3,2,SEND_TEXT);
//	}
//	else if(input_password_times==1)
//	{	//��ʾ����ĵڶ������롣
//		input_text(add4,14,"���ٴ���������");
//		SEND_TEXT[0]=0x20;
//		SEND_TEXT[1]=input_password[0];
//		input_text(add0,2,SEND_TEXT);
//		SEND_TEXT[0]=0x20;
//		SEND_TEXT[1]=input_password[1];
//		input_text(add1,2,SEND_TEXT);
//		SEND_TEXT[0]=0x20;
//		SEND_TEXT[1]=input_password[2];
//		input_text(add2,2,SEND_TEXT);
//		SEND_TEXT[0]=0x20;
//		SEND_TEXT[1]=input_password[3];
//		input_text(add3,2,SEND_TEXT);
//	}
//}
//void fun22(void)//�������á���ʾ�û���Ϣ����װ�û�����Ϣ��
//{ //������ ������ٽ�������20180503
//	unsigned int i=0;	
//	unsigned int add=0x50000;
////	unsigned char data_local[250]={0};	
//	unsigned char message_data[256]={0};//��ȡflash�д洢���û���Ϣ����װ����Ϣ��
//	unsigned char data_local[100]={0x2D};
//	unsigned char test[30]={0};
//	unsigned char data[40]={0};	
//	unsigned char printf_data[40]={0};		
//	unsigned int 	port=0;
////	CPU_SR_ALLOC();
////	OS_CRITICAL_ENTER();//�����ٽ���
//	
////	unsigned char data_local[50]={"��������ַ��"};

////	unsigned char data_local[10][100]={"����汾�ţ�","������ַ��","��������ַ��","�������˿ںţ�","��������K2��̬IP��",	
////																		"����汾�ţ�","������ַ��","��������ַ��","��������ַ��","��������K2��̬IP��"};																		

////2.�����µĽ�����ʾ������Ϣ����Ļ�ϣ������������ݣ�
////������ַ����FLASH�ж�ȡ����
////��������ַ����FLASH�ж�ȡ����
////K2��Ϣ����̬��̬���á�����IP��Ŀ���ַ�˿ںš�����״̬����K2�ж�ȡ����
////GM3��Ϣ��IMEI��SN��Ŀ���ַ�˿ںš�����״̬�������ź�ǿ�ȣ���GM3�ж�ȡ����
////�������汾�ţ���FLASH�ж�ȡ��	
//	change_back_picture(0x0031);//��ȡ�û���Ϣ����װ�û�����Ϣ��	
//	//�������ҳ���������ı����е����֡�

////����汾�ţ�
////������ַ��
////��������ַ��
////�������˿ںţ�
////��������K2��̬IP��
////01 00 00 00 00 00 00 00 00 00 
////	SPI_FLASH_BufferRead(Config,add,256);
//  Config_LcdCopy(message_data);
////	for(i=0;i<200;i++)
////	{
////		Config[i]=0x31;
////	}
//	
////	USART1_Send_Data(Config,256);

////	 // 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15	
////0  //01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
////1  //77 77 77 2E 66 75 62 61 6E 67 79 75 6E 2E 63 6F 
////2  //6D 2E 64 74 75 00 00 00 00 00 00 00 00 00 0A 1A 
////3  //01 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
////4  //C0 A8 01 3B FF FF FF 00 C0 A8 01 01 72 72 72 72 
////5  //00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
////6  //00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
////7  //00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
////8  //01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
////9  //02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
////10 //0A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
////11  //09 01 01 01 30 31 31 39 00 00 00 00 00 00 00 00 
////12  //01 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 
////13  //00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
////14  //00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
////15  //11 07 07 00 00 21 00 00 00 00 00 00 00 00 00 00 
////	���У�16~45 �Ƿ�������ַ,����30����

//	if(net_fixed==2)//���õ����������ӷ�ʽ
//	{
//		for(i=0;i<100;i++)//���buffer
//		{
//			data_local[i]=0x20;
//		}
//		memcpy(data_local,"��ѯ�У����Ժ�",16);
//		input_text(0x2520,20,data_local);		
//	}
//	else
//	{
//		for(i=0;i<100;i++)//���buffer
//		{
//			data_local[i]=0x20;
//		}		
//		memcpy(data_local,"��������",8);
//		input_text(0x2520,20,data_local);		
//	}

//	for(i=0;i<100;i++)//���buffer
//	{
//		data_local[i]=0x20;
//	}	
//	memcpy(data_local,"��������ַ��",12);
//	for(i=0;i<message_data[0x2D];i++)
//	{
//		data_local[13+i]=message_data[16+i];
//	}
//	input_text(0x2000,43,data_local);
//	for(i=0;i<100;i++)//���buffer
//	{
//		data_local[i]=0x20;
//	}	
////	���У�46~47 �Ƕ˿ںš�����2����
//	memcpy(data_local,"�������˿ںţ�",14);
//	port=message_data[46];//����������˿ں�
//	port=port<<8;
//	port=port+message_data[47];	
//	test[0]=(port/10000)+0x30;// ȡ��������
//	port=port%10000;	 // ȡ����
//	test[1]=(port/1000)+0x30;
//	port=port%1000;	
//	test[2]=(port/100)+0x30;
//	port=port%100;		
//	test[3]=(port/10)+0x30;		
//	port=port%10;		
//	test[4]=port+0x30;	
//	for(i=0;i<5;i++)
//	{
//		data_local[15+i]=test[i];
//	}
//	input_text(0x2050,21,data_local);	
//	for(i=0;i<100;i++)//���buffer
//	{
//		data_local[i]=0x20;
//	}		
////	���У�48�Ƿ������ӿڷ�ʽ��Ϊ1ʱ-���ߣ�Ϊ0ʱ-���ߣ�Ĭ�����ߣ���
////	switch(Config[48])
//	switch(net_fixed)		
//	{
//		case 1:
//			memcpy(data_local,"�������ӿڷ�ʽ�� ��������",26);
//			input_text(0x2100,26,data_local);			
//		break;
//		case 2:
//			memcpy(data_local,"�������ӿڷ�ʽ�� ��������",26);
//			input_text(0x2100,26,data_local);
//		break;
//		case 3:
//			memcpy(data_local,"�������ӿڷ�ʽ�� �����л�",26);
//			input_text(0x2100,26,data_local);
//		break;	
//		default:			
//			memcpy(data_local,"�������ӿڷ�ʽ�� �����л�",26);
//			input_text(0x2100,26,data_local);	
//		break;
//	}
////	if(Config[48]==0)//�������ӷ�ʽ
////	{
////		memcpy(data_local,"�������ӿڷ�ʽ�� ��������",26);
////		input_text(0x2100,26,data_local);			
////	}
////	else if(Config[48]==1)//�������ӷ�ʽ
////	{
////		memcpy(data_local,"�������ӿڷ�ʽ�� ��������",26);
////		input_text(0x2100,26,data_local);
////	}
////	else
////	{
////		memcpy(data_local,"�������ӿڷ�ʽ�� �����л�",26);
////		input_text(0x2100,26,data_local);
////	}	
//	for(i=0;i<100;i++)//���buffer
//	{
//		data_local[i]=0x20;
//	}
////0x31������ip���÷�ʽ��Ϊ1ʱDHCP��Ϊ0ʱ��̬ip��Ĭ��DHCP��
//	if(message_data[49]==0)//��̬ip���ӷ�ʽ
//	{
//		memcpy(data_local,"����IP���÷�ʽ�� ��̬IP",24);
//		input_text(0x2150,24,data_local);			
//	}
//	else//DHCP���ӷ�ʽ
//	{
//		memcpy(data_local,"����IP���÷�ʽ�� DHCP",22);
//		input_text(0x2150,22,data_local);			
//	}
//	for(i=0;i<100;i++)//���buffer
//	{
//		data_local[i]=0x20;
//	}
////0x40-0x4f��ip���������룬���أ�dns��	
////0x50	GM3��SN��.15λ
//	memcpy(data_local,"GM3����ģ��SN�룺",17);
//	for(i=0;i<15;i++)
//	{
//		data_local[18+i]=message_data[80+i];
////		data_local[18+i]=Config[80+i]+0x30;		
//	}
//	input_text(0x2200,34,data_local);	
//	for(i=0;i<100;i++)//���buffer
//	{
//		data_local[i]=0x20;
//	}		
////0x60	GM3��IMEI��.15λ
//	memcpy(data_local,"GM3����ģ��IMEI�룺",19);
//	for(i=0;i<15;i++)
//	{
//		data_local[20+i]=message_data[96+i];
////		data_local[20+i]=Config[96+i]+0x30;		
//	}
//	input_text(0x2250,36,data_local);	
//	for(i=0;i<100;i++)//���buffer
//	{
//		data_local[i]=0x20;
//	}		
//////0x70	GM3��PHONE��.13λ
////	memcpy(data_local,"GM3����ģ��PHONE�룺",20);
////	for(i=0;i<13;i++)
////	{
////		data_local[21+i]=Config[112+i];
//////		data_local[21+i]=Config[112+i]+0x30;		
////	}
////	input_text(0x2300,35,data_local);	
////0xf0-0xf5��������ַ����

//	memcpy(data_local,"����װ�ñ�����ַ��",18);
////	for(i=0;i<6;i++)
////	{
////		data_local[27+i]=Config[240+i];
////	}		
////handle_arrays_u8(test,6,printf_data)		
////	input_text(0x2300,33,data_local);			
////			for(i=0;i<12;i++)
////		{//������ID�Ÿ��µ�25Q16�С�
////			flash_memory[0xF0+i]=Config[0xB0+i];//
////	for(i=0;i<12;i++)
////	{
////		test[i]=Config[0xF0+i];
////	}
////	handle_arrays_u8(test,6,printf_data);	

//	for(i=0;i<6;i++)
//	{
//		test[i]=message_data[0xF0+i];
//	}		
//	handle_arrays_u8(test,6,printf_data);	
//	for(i=0;i<12;i++)
//	{
//		data_local[19+i]=printf_data[i];
//	}		
//	input_text(0x2300,32,data_local);		

////	for(i=0;i<12;i++)
////	{
////		data_local[19+i]=Config[0xF0+i];
////	}		
////	input_text(0x2300,32,data_local);	
//	
//	for(i=0;i<100;i++)//���buffer
//	{
//		data_local[i]=0x20;
//	}
////0xb4-0xb7���������룬Ĭ��Ϊ0119��
//	memcpy(data_local,"�������룺",10);
//	for(i=0;i<4;i++)
//	{
//		data_local[11+i]=message_data[0xB4+i];
//	}
//	input_text(0x2350,15,data_local);	

////	for(i=0;i<4;i++)
////	{
////		data[i]=Config[180+i];
////	}
////	handle_arrays_u8(data,4,printf_data);//�������飬DATA��Ҫ��������飬SIZE������ĳ��ȡ�//���罫0x25�ֿ�����0x32��0x35.��0xAB�ֿ�����0x40,0x41. 
////	for(i=0;i<8;i++)
////	{
////		data_local[11+i]=printf_data[i];
////	}
////	input_text(0x2350,20,data_local);

//	for(i=0;i<100;i++)//���buffer
//	{
//		data_local[i]=0x20;
//	}
////0xc0-0xc1��Ӳ���汾�����ֽ�Ϊ���汾�����ֽ�Ϊ�ΰ汾��Ĭ��1.0��
//	memcpy(data_local,"Ӳ���汾��",10);
//	data_local[11]=message_data[0xC0];	
//	data_local[13]=0x2E;//С����	
//	data_local[15]=message_data[0xC1];	
//	input_text(0x2400,16,data_local);			

//	for(i=0;i<100;i++)//���buffer
//	{
//		data_local[i]=0x20;
//	}
////0xD0	K2��MAC��ַ.6λ
//	memcpy(data_local,"K2��MAC��ַ��",16);
//	input_text(0x2500,16,data_local);		
//	for(i=0;i<100;i++)//���buffer
//	{
//		data_local[i]=0x20;
//	}
//	for(i=0;i<12;i++)
//	{
//		data_local[i]=message_data[0xD0+i];
//	}	
//	input_text(0x2510,12,data_local);		

//	for(i=0;i<100;i++)//���buffer
//	{
//		data_local[i]=0x20;
//	}		

//////0xD0	K2��MAC��ַ.6λ
////	for(i=0;i<100;i++)//���buffer
////	{
////		data_local[i]=0x20;
////	}			
////	memcpy(data_local,"��ǰGM3�ź�ǿ�ȣ�25",25);
////	input_text(0x2550,25,data_local);				
//	
////	for(i=0;i<100;i++)//���buffer
////	{
////		data_local[i]=0x20;
////	}			
////	memcpy(data_local,"25",2);	
//////	for(i=0;i<12;i++)
//////	{
//////		data_local[i]=Config[0xD0+i];
//////	}	
////	input_text(0x2520,2,data_local);		

//	
////	for(i=0;i<2;i++)
////	{
////		data_local[10+i]=Config[192+i];
////	}
//	
////	for(i=0;i<2;i++)
////	{
////		data[i]=Config[192+i];
////	}	
////	handle_arrays_u8(data,2,printf_data);//�������飬DATA��Ҫ��������飬SIZE������ĳ��ȡ�//���罫0x25�ֿ�����0x32��0x35.��0xAB�ֿ�����0x40,0x41. 
////	for(i=0;i<4;i++)
////	{
////		data_local[11+i]=printf_data[i];
////	}
////	input_text(0x2400,15,data_local);	
//	for(i=0;i<100;i++)//���buffer
//	{
//		data_local[i]=0x20;
//	}
////0xc8-0xc9������汾�����ֽ�Ϊ���汾�����ֽ�Ϊ�ΰ汾��Ĭ��1.0��
//	memcpy(data_local,"����汾��",10);
//	data_local[11]=message_data[0xC8];
//	data_local[13]=0x2E;//С����
//	data_local[15]=message_data[0xC9];	
//	input_text(0x2450,16,data_local);		

//	if(net_fixed==2)//���õ����������ӷ�ʽ
//	{
//		sendata_flag=1;
//		gm3_send_cmd("+++","a",800);//���� 	
//	//	printf("%s\n",usart2_receive_data);
//	//	USART1_Send_Data(usart2_receive_data,20);	
//		sendata_flag=1;
//		gm3_send_cmd("a","+ok",800);//
//	//	USART1_Send_Data(usart2_receive_data,20);
//	//	printf("%s\n",usart2_receive_data);
//		gm3_send_cmd("AT+CSQ","",800);//��ȡGM3��SN����
////	USART1_Send_Data(usart2_receive_data,20);
//		
////		printf("%s\n",usart2_receive_data);
//		for(i=0;i<100;i++)//���buffer
//		{
//			data_local[i]=0x20;
//		}//7,8
////		data_local[0]=usart2_receive_data[6];//��ʾ�ź�ǿ��
////		data_local[1]=usart2_receive_data[7];
////		data_local[2]=usart2_receive_data[8];
////		data_local[3]=usart2_receive_data[9];
//		if(usart2_receive_data[9]==0x2C)//��������˶��ţ�˵���ź�ǿ��Ϊ0~9������ʾ00~09
//		{
//			data_local[0]=0x30;
//			data_local[1]=usart2_receive_data[8];
//		}
//		else
//		{
//			data_local[0]=usart2_receive_data[8];//��ʾ�ź�ǿ��
//			data_local[1]=usart2_receive_data[9];			
//		}
//		//20180514������Χֵ�����㡣
//		if((data_local[0]+data_local[1])>0x6C)//���������Χ�������ź�ǿ��Ϊ 00 ��
//		{
//			data_local[0]=0x30;
//			data_local[1]=0x30;
//		}
//		
//		input_text(0x2520,20,data_local);

//		gm3_send_cmd("AT+ENTM","",800);//�ָ�͸��
//	//	USART1_Send_Data(usart2_receive_data,20);
//	//	printf("%s\n",usart2_receive_data);//����				
//	}
//	else
//	{
////		for(i=0;i<100;i++)//���buffer
////		{
////			data_local[i]=0x20;
////		}		
////		memcpy(data_local,"��������",8);
////		input_text(0x2520,20,data_local);		
//	}




////1.2 AT+CSQ
////������ͣ���������ź�ǿ��
////�����ʽ��AT+CSQ
////����أ�+CSQ: **,##
////����**Ӧ�� 10 �� 31 ֮�䣬��ֵԽ������ź�����Խ�ã�##Ϊ����
////��,ֵ�� 0 �� 99 ֮�䡣
////����Ӧ������߻� SIM ���Ƿ���ȷ��װ
////���Խ����
////AT+CSQ
////+CSQ: 20,5













////	for(i=0;i<2;i++)
////	{
////		data[i]=Config[200+i];
////	}	
////	handle_arrays_u8(data,2,printf_data);//�������飬DATA��Ҫ��������飬SIZE������ĳ��ȡ�//���罫0x25�ֿ�����0x32��0x35.��0xAB�ֿ�����0x40,0x41. 
////	for(i=0;i<4;i++)
////	{
////		data_local[11+i]=printf_data[i];
////	}
////	input_text(0x2450,15,data_local);		
////	for(i=0;i<100;i++)//���buffer
////	{
////		data_local[i]=0x20;
////	}
//////0xf0-0xf5��������ַ����
////	memcpy(data_local,"�û���Ϣ����װ�ñ�����ַ��",26);
//////	for(i=0;i<6;i++)
//////	{
//////		data_local[26+i]=Config[240+i]+0x30;
//////	}
////	for(i=0;i<6;i++)
////	{
////		data_local[27+i]=Config[240+i];
////	}	
////	input_text(0x2500,33,data_local);		
//	
////	for(i=0;i<6;i++)
////	{
////		data[i]=Config[240+i];
////	}	
////	handle_arrays_u8(data,6,printf_data);//�������飬DATA��Ҫ��������飬SIZE������ĳ��ȡ�//���罫0x25�ֿ�����0x32��0x35.��0xAB�ֿ�����0x40,0x41. 
////	for(i=0;i<12;i++)
////	{
////		data_local[27+i]=printf_data[i];
////	}
////	input_text(0x2500,40,data_local);	
////	for(i=0;i<100;i++)//���buffer
////	{
////		data_local[i]=0x20;
////	}

////	u16 add0=0x1834;
////	u16 add1=0x1835;
////	u16 add2=0x184E;
////	u16 add3=0x184F;
////	input_small_picture(add0,0x0000);//����ȥ�����ѡ�С�
////	input_small_picture(add1,0x0000);//ȡ��ȥ�����ѡ�С�
////	if(fun22_Cursor<2)
////	{
////		change_back_picture(0x0022+fun22_Cursor);
////	}
////	else if(fun22_Cursor==2)
////	{
////		input_small_picture(add0,0x0001);//����ȥ�����ѡ�С�
////		input_small_picture(add1,0x0000);//ȡ��ȥ����겻ѡ�С�
////		change_back_picture(0x0024);
////	}else if(fun22_Cursor==3)
////	{
////		input_small_picture(add0,0x0000);//����ȥ�����ѡ�С�
////		input_small_picture(add1,0x0001);//ȡ��ȥ����겻ѡ�С�
////		change_back_picture(0x0024);
////	}
////	
////	if(sound_fixed==0)
////	{
////		input_small_picture(add2,0x000A);//С���ѡ�С�
////		input_small_picture(add3,0x0000);//С���ѡ�С�
////	}else if(sound_fixed==1)
////	{
////		input_small_picture(add2,0x0000);//С���ѡ�С�
////		input_small_picture(add3,0x000A);//С���ѡ�С�
////	}
////	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
//}
//void fun23(void){}
//void fun24(void){}
//void fun25(void)//ʱ������
//{
//	unsigned char time_data[6]={0};
//	u8 SEND_TEXT[40];//���崮��2��������
//	unsigned char buf[0x06];
//	u16 add0=0x1860;//���� ��� ��ַ
//	u16 add1=0x1870;//���� �·� ��ַ
//	u16 add2=0x1880;//���� ���� ��ַ
//	u16 add3=0x1890;//���� Сʱ ��ַ
//	u16 add4=0x18A0;//���� ���� ��ַ
//	u16 add5=0x18B0;//���� ��   ��ַ
//	u16 add6=0x1858;//����
//	u16 add7=0x1859;//�˳�
//	u16 add8=0x19E0;// �Զ�Уʱ/�ֶ�Уʱ ����λ��	
//	u16 add9=0x19F0;// �Զ�Уʱ/�ֶ�Уʱ ����ַ 		
//	input_small_picture(add6,0x0000);//����ȥ����겻ѡ�С�
//	input_small_picture(add7,0x0000);//ȡ��ȥ����겻ѡ�С�
//	input_small_picture(add9,0x0000);//�Զ�Уʱ/�ֶ�Уʱ ȡ��ѡ��
//	
////	RTC_Get(time_data);//��ȡʱ�䡣��֤����ʱ�����õ�ʱ���ܹ���ȷ����ʾ��ǰ��ʱ�䡣
////	sec=time_data[0];
////	minute=time_data[1];
////	hours=time_data[2];
////	date=time_data[3];
////	month=time_data[4];
////	year=time_data[5];	
//	
//	if(correction_time_way==1)
//	{
//		input_text(add8,8,"�Զ�Уʱ");
//	}
//	else
//	{
//		input_text(add8,8,"�ֶ�Уʱ");
//	}	
//	
//	if(fun25_Cursor<6)//�л�����ͼƬ��0~6��7�š�
//	{
//		input_small_picture(add9,0x0000);//�Զ�Уʱ/�ֶ�Уʱ ȡ��ѡ��		
//		change_back_picture(0x0025+fun25_Cursor);
//	}else if(fun25_Cursor==6)
//	{
//		change_back_picture(0x002B);
//		input_small_picture(add9,0x0000);//�Զ�Уʱ/�ֶ�Уʱ ȡ��ѡ��
//		input_small_picture(add6,0x0001);//����ȥ�����ѡ�С�
//		input_small_picture(add7,0x0000);//ȡ��ȥ����겻ѡ�С�
//	}else if(fun25_Cursor==7)
//	{
//		change_back_picture(0x002B);
//		input_small_picture(add9,0x0000);//�Զ�Уʱ/�ֶ�Уʱ ȡ��ѡ��
//		input_small_picture(add6,0x0000);//����ȥ����겻ѡ�С�
//		input_small_picture(add7,0x0001);//ȡ��ȥ�����ѡ�С�
//		RTC_Get((char *)buf);		
//		sec=buf[0];
//		minute=buf[1];
//		hours=buf[2];
//		date=buf[3];
//		month=buf[4];
//		year=buf[5];
//		
////		printf("������\r\n");
//		
//	}else if(fun25_Cursor==8)//ѡ�� �Զ�Уʱ/�ֶ�Уʱ ��
//	{
//		change_back_picture(0x002B);		
//		input_small_picture(add9,0x0001);//�Զ�Уʱ/�ֶ�Уʱ ѡ��
//		input_small_picture(add6,0x0000);//����ȥ�����ѡ�С�
//		input_small_picture(add7,0x0000);//ȡ��ȥ����겻ѡ�С�		
//	}

////	SEND_TEXT[0]=0x32;//��
////	SEND_TEXT[1]=0x30;		
////	SEND_TEXT[2]=year/16+0x30;
////	SEND_TEXT[3]=year%16+0x30;
////	input_text(add0,4,SEND_TEXT);

////	SEND_TEXT[0]=month/16+0x30;//��
////	SEND_TEXT[1]=month%16+0x30;
////	input_text(add1,2,SEND_TEXT);
////	
////	SEND_TEXT[0]=date/16+0x30;//��
////	SEND_TEXT[1]=date%16+0x30;
////	input_text(add2,2,SEND_TEXT);
////	
////	SEND_TEXT[0]=hours/16+0x30;//ʱ
////	SEND_TEXT[1]=hours%16+0x30;
////	input_text(add3,2,SEND_TEXT);
////	
////	SEND_TEXT[0]=minute/16+0x30;//��
////	SEND_TEXT[1]=minute%16+0x30;
////	input_text(add4,2,SEND_TEXT);
////	
////	SEND_TEXT[0]=sec/16+0x30;//��
////	SEND_TEXT[1]=sec%16+0x30;
////	input_text(add5,2,SEND_TEXT);
//	
//	SEND_TEXT[0]=0x32;//��
//	SEND_TEXT[1]=0x30;		
//	SEND_TEXT[2]=year/10+0x30;
//	SEND_TEXT[3]=year%10+0x30;
//	input_text(add0,4,SEND_TEXT);

//	SEND_TEXT[0]=month/10+0x30;//��
//	SEND_TEXT[1]=month%10+0x30;
//	input_text(add1,2,SEND_TEXT);
//	
//	SEND_TEXT[0]=date/10+0x30;//��
//	SEND_TEXT[1]=date%10+0x30;
//	input_text(add2,2,SEND_TEXT);
//	
//	SEND_TEXT[0]=hours/10+0x30;//ʱ
//	SEND_TEXT[1]=hours%10+0x30;
//	input_text(add3,2,SEND_TEXT);
//	
//	SEND_TEXT[0]=minute/10+0x30;//��
//	SEND_TEXT[1]=minute%10+0x30;
//	input_text(add4,2,SEND_TEXT);
//	
//	SEND_TEXT[0]=sec/10+0x30;//��
//	SEND_TEXT[1]=sec%10+0x30;
//	input_text(add5,2,SEND_TEXT);	
//}
//void fun26(void){}
//void fun27(void){}
//void fun28(void)
//{
//	u8 SEND_TEXT[40];
//	u16 add0=0x14D0;
//	u16 add1=0x14D2;
//	u16 add2=0x14D4;
//	u16 add3=0x14D6;
//	u16 add4=0x14C0;//����������ʾ����
//	u16 add5=0x1901;//��Ϣ��ʾ��
//	change_back_picture(0x0004);//�л��ײ�ͼƬ�����������������
//	memset(SEND_TEXT,0x20,28);
//	input_text(add4,28,SEND_TEXT);//��ʾ����յ�ǰ������ʾ��λ�����֡�
//	input_text(add5,28,SEND_TEXT);//��ʾ����յ�ǰ������ʾ��λ�����֡�
//	
//	switch(password_three_ok)
//	{
//		case 0:
//			input_text(add4,10,"����������");//��ʾ������������
//		break;
//		case 2:
//			input_text(add4,14,"��������������");//�������
//		break;
//		default:			break;
//	}
//	//��ʾ������
//	SEND_TEXT[0]=0x20;
//	SEND_TEXT[1]=input_password[0];
//	input_text(add0,2,SEND_TEXT);
//	SEND_TEXT[0]=0x20;
//	SEND_TEXT[1]=input_password[1];
//	input_text(add1,2,SEND_TEXT);
//	SEND_TEXT[0]=0x20;
//	SEND_TEXT[1]=input_password[2];
//	input_text(add2,2,SEND_TEXT);
//	SEND_TEXT[0]=0x20;
//	SEND_TEXT[1]=input_password[3];
//	input_text(add3,2,SEND_TEXT);
//}
//void (*current_operation_index)();//��ˢ�²˵��йء�
//typedef struct
//{
//	unsigned char current;//�˵�����
//	unsigned char key_note; 	//��¼��
//	unsigned char key_set; 		//���ü�
//	unsigned char key_up;     //�ϼ�
//	unsigned char key_down;   //�¼�		
//	unsigned char key_cancel; //ȡ����
//	unsigned char key_enter;  //ȷ�ϼ�
//	unsigned char key_release_alarm; 			//������
//	unsigned char key_release_breakdown;  //��������
//	unsigned char key_self_check; 				//�����Լ�
//	unsigned char key_noise_reduction ;  	//����
//	unsigned char key_reset; 							//��λ
//	unsigned char key_Manual_alarm ;  		//�ֶ�����
//	unsigned char key_response; 					//���Ӧ��
//	unsigned char key_zero; //0
//	unsigned char key_one;  //1
//	unsigned char key_two;  //2
//	unsigned char key_three;//3
//	unsigned char key_four; //4
//	unsigned char key_five; //5
//	unsigned char key_six;  //6
//	unsigned char key_seven;//7
//	unsigned char key_eight;//8
//	unsigned char key_nine; //9
//	void (*current_operation)();
//} key_table;
////������ĺ���ָ��
//key_table const table[30]=
//{
///*   
//(0) ����: ��¼��ѯ
//(1) ����: ����
//(2) ����: ��
//(3) ����: ��
//(4) ����: ȡ��
//(5) ����: ȷ��
//(6) ����: ������
//(7) ����: ��������
//(8) ����: �����Լ�
//(9) ����: ����
//(10)����: ��λ
//(11)����: �ֶ�����
//(12)����: ���Ӧ��
//(13)����: 0
//(14)����: 1
//(15)����: 2
//(16)����: 3
//(17)����: 4
//(18)����: 5
//(19)����: 6
//(20)����: 7
//(12)����: 8
//(22)����: 9
//*/
///*    0.  1.  2.  3.  4.  5.  6.  7.  8.  9.  10. 11. 12. 13. 14. 15. 16. 17. 18. 19. 20. 21. 22.    */
// {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,(*fun0)},//��ʾhome����	
// {1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,(*fun1)},//��ʾ��Ϣ��ѯ����	 
// {2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,(*fun2)},//��ʾ�����������
//	 
// {3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,(*fun3)},//��ʾ���ý���	 
// {4,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,(*fun4)},//���ý������ƶ���	
// {5,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,(*fun5)},//���ý������ƶ���	
//	 
// {6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,(*fun6)},//��ʾ����������ý���
// {7,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,(*fun7)},//���ý������ƶ���	
// {8,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,(*fun8)},//���ý������ƶ���	

// {9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,(*fun9)},//��ʾ����ͨ�����ý���
// {10, 9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,(*fun10)},//���ý������ƶ���	
// {11, 9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,(*fun11)},//���ý������ƶ���	
//	 
// {12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,(*fun12)},//����RS232/RS485������		 
// {13, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,(*fun13)},//���ý������ƶ��� 
// {14, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,(*fun14)},//���ý������ƶ���

// {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,(*fun15)},//����CAN������		
// {16, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,(*fun16)},//��ʾ�������ý���
// {17, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,(*fun17)},//���ý������ƶ���	
//	 
// {18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,(*fun18)},//��ʾ������ڽ���
// {19, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,(*fun19)},//���ý������ƶ���		 
// {20, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,(*fun20)},//���ý������ƶ���		 
//	 
// {21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,(*fun21)},//��ʾ�������ý���

// {22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,(*fun22)},//��ʾ�������ý���
// {23, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,(*fun23)},//���ý������ƶ���	
// {24, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,(*fun24)},//���ý������ƶ���	

// {25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,(*fun25)},//��ʾʱ�����ý���
// {26, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,(*fun26)},//���ý������ƶ���	
// {27, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,(*fun27)},//���ý������ƶ���	
//	 
// {28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,(*fun28)},//������������
//};
////�����ְ���ת��Ϊchar
//unsigned char key2num(unsigned int key)
//{
//	unsigned char buf=0;
//	switch(key)
//	{
//		case (1<<21):	buf=0;break;
//		case (1<<0):	buf=1;break;
//		case (1<<5):	buf=2;break;
//		case (1<<10):	buf=3;break;
//		case (1<<15):	buf=4;break;
//		case (1<<20):	buf=5;break;
//		case (1<<1):	buf=6;break;
//		case (1<<6):	buf=7;break;
//		case (1<<11):	buf=8;break;
//		case (1<<16):	buf=9;break;
//		default:		buf=0xff;break;
//	}
//	return buf;
//}
//void fun0_up(void)
//{
//	fun0_Cursor=!fun0_Cursor;
//}
//void fun0_down(void)
//{
//	fun0_Cursor=!fun0_Cursor;
//}
//void fun0_enter(void)
//{
//	if(fun0_Cursor==0)	//�������õ���������������
//	{
//		func_index=2;
//	}
//	else				//������Ϣ��ѯ����
//	{
//		func_index=1;
//	}
//}
//void fun1_up(void)
//{
//	if(fun1_Cursor<1)fun1_Cursor=FUN1CURSOR_EXIT;
//	else fun1_Cursor--;
//}
//void fun1_down(void)
//{
//	if(fun1_Cursor>=FUN1CURSOR_EXIT)fun1_Cursor=0;
//	else fun1_Cursor++;
//}
//void fun1_cancel(void)
//{
//	if((input_password_no!=0)&&(fun1_Cursor==4))
//	{
//		if(input_password_no>=1)input_password[input_password_no--]=0x20;
//		if(input_password_no==0)fun1_page=1;
//		else if(input_password_no==1)fun1_page=(input_password[0]-'0');
//		else if(input_password_no==2)fun1_page=(input_password[0]-'0')*10+(input_password[1]-'0');
//		input_password[input_password_no]=0x20;
//	}
//	else
//	{
//		input_password_no=0;
//		memset(input_password,0x20,4);
//		fun1_Cursor=0;
//		fun1_page_fixed=1;
//		func_index=0;
//	}
//}
//void fun1_enter(void)
//{
//	unsigned int page_MAXnum=0;
//	unsigned char tmp[0x20];
//	unsigned char FileFlag=1;
//	OS_ERR err;
//	
//	W25q16_Lock();
//	Read_Record(tmp);
//	W25q16_UnLock();
//	Lcd_Flash_Cnt=tmp[8]+(tmp[9]<<8)+(tmp[10]<<16)+(tmp[11]<<24);
//	if(Lcd_Flash_Cnt<=6)page_MAXnum=1;
//	else
//	{
//		if(Lcd_Flash_Cnt%6==0) page_MAXnum=Lcd_Flash_Cnt/6;
//		else page_MAXnum=Lcd_Flash_Cnt/6+1;
//	}
//	switch(fun1_Cursor)
//	{
//		case FUN1CURSOR_HOME_PAGE:
//			fun1_page_fixed=1;
//		break;
//		case FUN1CURSOR_PRE_PAGE:
//			if(fun1_page_fixed>1)//�жϣ���ֹ���硣
//			{
//				fun1_page_fixed--;			
//			}
//		break;
//		case FUN1CURSOR_NEXT_PAGE:
//			if(fun1_page_fixed<page_MAXnum)
//			{
//				fun1_page_fixed++;			
//			}
//		break;
//		case FUN1CURSOR_LAST_PAGE:
//			fun1_page_fixed=page_MAXnum;
//		break;
//		case FUN1CURSOR_PAGE_NUM:
//			if(fun1_page>page_MAXnum)//�жϣ���ֹ���硣
//			{
//				fun1_page=page_MAXnum;			
//			}
//			fun1_page_fixed=fun1_page;
//			input_password_no=0;
//			memset(input_password,0x20,4);
//		break;
//		case FUN1CURSOR_LOAD2SD:
//			fun1_page_fixed=1;
//			/////////////////
////			LcdOn();
////		  LCD_SET_ON();//������Ļ
//		  LCD_SET_LONG_ON();//������Ļ
//			back_light_change(Backlight_brightness);//������Ļ����	
//		  master_exportmessage_to_SD(0);
//			FileFlag=filesysint();
//		  if(FileFlag==0)
//		  { printf("�ļ�ϵͳ������%d\n\r,FileFlag");
//				filesysInfRed(); 
//			  master_exportmessage_to_SD(1);
//			}
//			else
//			{ printf("�ļ�ϵͳ�쳣��%d\n\r,FileFlag");
//		    master_exportmessage_to_SD(3);
//			}
//			//////////////////
//		break;
//		case FUN1CURSOR_EXIT:
//			input_password_no=0;
//			memset(input_password,0x20,4);
//			fun1_Cursor=0;
//			fun1_page_fixed=1;
//			func_index=0;
//		break;
//		default:
//		break;
//	}
//}
//void fun2_cancel(void)
//{
//	if(input_password_no!=0)
//	{
//		input_password_no--;
//		input_password[input_password_no]=0x20;
//	}
//	else
//	{
//		password_three_ok=0;
//		input_password_no=0;
//		memset(input_password,0x20,4);
//		func_index=0;
//	}
//}
//void fun2_enter(void)
//{
//	if((input_password_no==4)&&(password_three[0]==input_password[0])&&(password_three[1]==input_password[1])\
//					&&(password_three[2]==input_password[2])&&(password_three[3]==input_password[3]))
//	{
//		password_three_ok=0;
//		input_password_no=0;
//		memset(input_password,0x20,4);
//		func_index=3;
//	}
//	else
//	{
//		input_password_no=0;
//		memset(input_password,0x20,4);
//		password_three_ok=2;//�������
//	}
//}
//void fun3_up(void)
//{
//	if(fun3_Cursor<1)fun3_Cursor=5;
//	else fun3_Cursor--;
//}
//void fun3_down(void)
//{
//	if(fun3_Cursor>=5)fun3_Cursor=0;
//	else fun3_Cursor++;
//}
//void fun3_cancel(void)
//{
//	fun3_Cursor=0;
//	func_index=0;
//}
//void fun3_enter(void)
//{
//	switch(fun3_Cursor)
//	{
//		case 0:	func_index=6;	break;//��������������ý��档
//		case 1:	func_index=9;	break;//��������ͨ�����ý��档
//		case 2:	func_index=18;	break;//���뱳����ڽ��档
//		case 3:	func_index=21;	break;//�����������ý��档
//		case 4:	func_index=22;	break;//�����������ý��档
//		case 5:	func_index=25;	break;//����ʱ�����ý��档
//		default:				break;
//	}
//}
//void fun6_up(void)
//{
//	if(fun6_Cursor<1)fun6_Cursor=3;
//	else fun6_Cursor--;
//}
//void fun6_down(void)
//{
//	if(fun6_Cursor>=3)fun6_Cursor=0;
//	else fun6_Cursor++;
//}
//void fun6_cancel(void)
//{
//	fun6_Cursor=0;
//	func_index=0;
//}
//void fun6_enter(void)
//{
//	unsigned char buf[0x10];
////	fun6_Cursor=0;//����˵������б�־λ�����㡣
////	func_index=3;	
//	switch(fun6_Cursor)//�鿴ѡ�����ĸ���ꡣ
//	{
//		case 0:	
//			net_fixed=1;
//			func_index=6;
//		break;//ѡ������
//		case 1:	
//			net_fixed=2;
//			func_index=6;
//		break;//ѡ������
////		case 2:	
////			net_fixed=3;
////			func_index=6;
////		break;//ѡ�з�����
//		case 2:			//ѡ�б���
//			//ASL...//��������ͨ�ŷ�ʽnet_fixed//ASL...
//			buf[0]=net_fixed;
//			W25q16_Lock();
//			Writeflash_Config(buf,SERVER_INTERFACE_CONFIG_CMD);
//			Net_Init();
//			W25q16_UnLock();
//			fun6_Cursor=0;//����˵������б�־λ�����㡣
//			func_index=3;
//		break;
//		case 3:			//ѡ���˳�
//			fun6_Cursor=0;//����˵������б�־λ�����㡣
//			func_index=3;
//		break;
//		default:			break;
//	}
//}
//void fun9_up(void)
//{
//	if(fun9_Cursor<1)fun9_Cursor=18;
//	else fun9_Cursor--;
//}
//void fun9_down(void)
//{
//	if(fun9_Cursor>=18)fun9_Cursor=0;
//	else fun9_Cursor++;
//}
//void fun9_cancel(void)
//{
//	fun9_Cursor=0;
//	func_index=3;
//}
//void fun9_enter(void)
//{
//	unsigned char buf[0x10];
//	switch(fun9_Cursor)
//	{
//		case 16://ѡ��ע��忨
//			func_index=15;		
//			register_card_flag=1;//ע��忨��־λ��1.

//		break;
//		case 17://ѡ�б������
//			func_index=15;			
//			register_card_flag=0;//��������־λ��1.
//		break;
//		case 18://ѡ���˳�
//			fun9_Cursor=0;
//			func_index=3;
//		break;		
//		default://ѡ�в鿴�忨��Ϣ		0~15
//			func_index=12;//ѡ�а忨��Ϣ��ʾ					
//			break;
//	}
//}
//void fun12_up(void)
//{
////	if(fun12_Cursor_fixed>=3)
////	{
////		if(fun12_Cursor<1)fun12_Cursor=4;
////		else fun12_Cursor--;
////	}
////	else
////	{
////		if(fun12_Cursor_fixed==0)
////		{
////			if(rs232baund<1)rs232baund=16;
////			else rs232baund--;
////		}else if(fun12_Cursor_fixed==1)
////		{
////			if(rs232parity<1)rs232parity=2;
////			else rs232parity--;
////		}else if(fun12_Cursor_fixed==2)
////		{
////			if(rs232stopbit<1)rs232stopbit=2;
////			else rs232stopbit--;
////		}
////	}
//}
//void fun12_down(void)
//{
////	if(fun12_Cursor_fixed>=3)
////	{
////		if(fun12_Cursor>=4)fun12_Cursor=0;
////		else fun12_Cursor++;
////	}
////	else
////	{
////		if(fun12_Cursor_fixed==0)
////		{
////			if(rs232baund>=16)rs232baund=0;
////			else rs232baund++;
////		}else if(fun12_Cursor_fixed==1)
////		{
////			if(rs232parity>=2)rs232parity=0;
////			else rs232parity++;
////		}else if(fun12_Cursor_fixed==2)
////		{
////			if(rs232stopbit>=2)rs232stopbit=0;
////			else rs232stopbit++;
////		}
////	}
//}
//void fun12_cancel(void)
//{
////	fun12_Cursor=4;
////	rs232baund=rs232baund_fixed;
////	rs232parity=rs232parity_fixed;
////	rs232stopbit=rs232stopbit_fixed;
//	func_index=9;
//}
//void fun12_enter(void)
//{
//	unsigned char buf[0x10];
////	if(fun12_Cursor_fixed>=3)
////	{
////		fun12_Cursor_fixed=fun12_Cursor;
////		if(fun12_Cursor==3)
////		{
////			rs232baund_fixed=rs232baund;
////			rs232parity_fixed=rs232parity;
////			rs232stopbit_fixed=rs232stopbit;
////			buf[0x0]=rs232baund_fixed;
////			buf[0x4]=rs232parity_fixed;
////			buf[0x5]=rs232stopbit_fixed;
////			W25q16_Lock();
////			Writeflash_Config(buf,RS485_CONFIG_CMD);
////			W25q16_UnLock();
////			func_index=9;
////		}else if(fun12_Cursor==4)
////		{
////			rs232baund=rs232baund_fixed;
////			rs232parity=rs232parity_fixed;
////			rs232stopbit=rs232stopbit_fixed;
////			func_index=9;
////		}
////	}
////	else
////	{
////		fun12_Cursor_fixed=4;
////	}
//}
//void fun15_up(void)
//{
////	if(fun15_Cursor_fixed>=1)
////	{
////		if(fun15_Cursor<1)fun15_Cursor=2;
////		else fun15_Cursor--;
////	}
////	else if(fun15_Cursor_fixed==0)
////	{
////		if(CANbaund<1)CANbaund=14;
////		else CANbaund--;
////	}
//}
//void fun15_down(void)
//{
////	if(fun15_Cursor_fixed>=1)
////	{
////		if(fun15_Cursor>=2)fun15_Cursor=0;
////		else fun15_Cursor++;
////	}
////	else if(fun15_Cursor_fixed==0)
////	{
////		if(CANbaund>=14)CANbaund=0;
////		else CANbaund++;
////	}
//}
//void fun15_cancel(void)
//{
////	fun15_Cursor=2;
////	CANbaund=CANbaund_fixed;
//	func_index=9;
//}
//void fun15_enter(void)
//{
//	unsigned char buf[0x10];
////	if(fun15_Cursor_fixed>=1)
////	{
////		fun15_Cursor_fixed=fun15_Cursor;
////		if(fun15_Cursor==1)
////		{
////			CANbaund_fixed=CANbaund;
////			buf[0]=CANbaund_fixed;
////			W25q16_Lock();
////			Writeflash_Config(buf,CAN_CONFIG_CMD);
////			W25q16_UnLock();
////			func_index=9;
////		}else if(fun15_Cursor==2)
////		{
////			CANbaund=CANbaund_fixed;
////			func_index=9;
////		}
////	}
////	else
////	{
////		fun15_Cursor_fixed=2;
////	}
//}
//void fun18_up(void)
//{
//	if(fun18_Cursor_fixed>=2)
//	{
//		if(fun18_Cursor<1)fun18_Cursor=3;
//		else fun18_Cursor--;
//	}
//	else
//	{
//		if(fun18_Cursor_fixed==0)
//		{
//			if(Backlight_brightness<1)Backlight_brightness=9;
//			else Backlight_brightness--;
//		}
//		else if(fun18_Cursor_fixed==1)
//		{
//			if(Backlight_time<1)Backlight_time=2;
//			else Backlight_time--;
//		}
//	}
//}
//void fun18_down(void)
//{
//	if(fun18_Cursor_fixed>=2)
//	{
//		if(fun18_Cursor>=3)fun18_Cursor=0;
//		else fun18_Cursor++;
//	}
//	else
//	{
//		if(fun18_Cursor_fixed==0)
//		{
//			if(Backlight_brightness>=9)Backlight_brightness=0;
//			else Backlight_brightness++;
//		}
//		else if(fun18_Cursor_fixed==1)
//		{
//			if(Backlight_time>=2)Backlight_time=0;
//			else Backlight_time++;
//		}
//	}
//}
//void fun18_cancel(void)
//{
//	fun18_Cursor_fixed=3;
//	fun18_Cursor=3;
//	Backlight_brightness=Backlight_brightness_fixed;
//	Backlight_time=Backlight_time_fixed;
//	func_index=3;
//}
//void fun18_enter(void)
//{
////	fun18_Cursor_fixed=3;
////	fun18_Cursor=3;
////	Backlight_brightness=Backlight_brightness_fixed;
////	Backlight_time=Backlight_time_fixed;
////	func_index=3;
//	unsigned char buf[0x10];
//	if(fun18_Cursor_fixed>=2)
//	{
//		if(fun18_Cursor==2)
//		{
//			Backlight_brightness_fixed=Backlight_brightness;
//			Backlight_time_fixed=Backlight_time;
//			buf[0x0]=Backlight_brightness_fixed;
//			buf[0x01]=Backlight_time_fixed;
//			W25q16_Lock();
//			Writeflash_Config(buf,LCD_CONFIG_CMD);
//			W25q16_UnLock();
//			func_index=3;
//			fun18_Cursor_fixed=3;
//			fun18_Cursor=3;
//		}else if(fun18_Cursor==3)
//		{
//			Backlight_brightness=Backlight_brightness_fixed;
//			Backlight_time=Backlight_time_fixed;
//			func_index=3;
//			fun18_Cursor_fixed=3;
//			fun18_Cursor=3;
//		}else
//		{
//			fun18_Cursor_fixed=fun18_Cursor;
//		}
//	}
//	else
//	{
//		fun18_Cursor_fixed=3;
//	}
//}
//void fun21_cancel(void)
//{
//	if(input_password_no!=0)
//	{
//		input_password_no--;
//		input_password[input_password_no]=0x20;
//	}
//	else
//	{
//		input_password_times=0;
//		input_password_no=0;
//		memset(input_password,0x20,4);
//		func_index=3;
//	}
//}
//void fun21_enter(void)
//{
//	static unsigned char first_passedword[4]; 
//	if(input_password_no==4)
//	{
//		if(input_password_times==0)
//		{
//			input_password_times++;
//			memcpy(first_passedword,input_password,4);
//			input_password_no=0;
//			memset(input_password,0x20,4);
//		}else if(input_password_times==1)
//		{
//			if((first_passedword[0]==input_password[0])&&(first_passedword[1]==input_password[1])&&\
//				(first_passedword[2]==input_password[2])&&(first_passedword[3]==input_password[3]))
//			{
//				input_password_times=0;
//				input_password_no=0;
//				memset(input_password,0x20,4);
//				memcpy(password_two,first_passedword,4);
//				W25q16_Lock();
//				Writeflash_Config(password_two,LCD_MIMA_CMD);
//				W25q16_UnLock();
//				memset(first_passedword,0x20,4);
//				func_index=3;
//			}
//			else
//			{
//				input_password_times=0;
//				input_password_no=0;
//				memset(input_password,0x20,4);
//				memset(first_passedword,0x20,4);
//			}
//		}
//	}
//	else
//	{
//		input_password_no=0;
//		memset(input_password,0x20,4);
//		input_password_times=0;//�������
//	}
//}
//void fun22_up(void)
//{
//	if(fun22_Cursor<1)fun22_Cursor=3;
//	else fun22_Cursor--;
//}
//void fun22_down(void)
//{
//	if(fun22_Cursor>=3)fun22_Cursor=0;
//	else fun22_Cursor++;
//}
//void fun22_cancel(void)
//{
//	fun22_Cursor=3;
//	sound=sound_fixed;
//	func_index=3;
//}
//void fun22_enter(void)
//{
//	unsigned char buf[0x10];
//	if(fun22_Cursor<2)
//	{
//		sound=fun22_Cursor;
//	}
//	else if(fun22_Cursor==2)
//	{
//		sound_fixed=sound;
//		buf[0]=sound_fixed;
//		W25q16_Lock();
//		Writeflash_Config(buf,LCD_VOICE_CMD);
//		W25q16_UnLock();
//		func_index=3;
//	}else if(fun22_Cursor==3)
//	{
//		sound=sound_fixed;
//		func_index=3;
//	}
//}
//void fun25_up(void)
//{
//	if(correction_time_way==1)//�Զ���ʱ��������������ʱ��������á�
//	{
//		if(fun25_Cursor_fixed>=6)
//		{		
//			if(fun25_Cursor<=6)fun25_Cursor=8;
//			else fun25_Cursor--;
//		}		
//	}
//	else
//	{
//		if(fun25_Cursor_fixed>=6)
//		{
//			if(fun25_Cursor<1)fun25_Cursor=8;
//			else fun25_Cursor--;
//		}
//		else
//		{
//				if(fun25_Cursor_fixed==0)
//				{
//					if(year>=99)year=0;
//					else year++;					
//				}
//				else if(fun25_Cursor_fixed==1)
//				{
//					if(month>=12)month=1;
//					else month++;					
//				}
//				else if(fun25_Cursor_fixed==2)
//				{
//					if((month==1)||(month==3)||(month==5)||(month==7)||(month==8)||(month==10)||(month==12))
//					{
//						if(date>=31)date=1;
//						else date++;						
//					}
//					else if((month==4)||(month==6)||(month==9)||(month==11))
//					{
//							if(date>=30)date=1;
//							else date++;					
//					}
//					else if(month==2)
//					{
//							if((year+2000)/4>=1)
//							{
//								if(date>=28)date=1;//ƽ�꣬��28�졣
//								else date++;					
//							}
//							else
//							{
//								if(date>=29)date=1;//����29��
//								else date++;					
//							}
//					}
//				}
//				else if(fun25_Cursor_fixed==3)
//				{
//				if(hours>=23)hours=0;
//				else hours++;				
//				}
//				else if(fun25_Cursor_fixed==4)
//				{
//				if(minute>=59)minute=0;
//				else minute++;				
//				}
//				else if(fun25_Cursor_fixed==5)
//				{
//				if(sec>=59)sec=0;
//				else sec++;
//				}
//		}
//	}
//}
// void fun25_down(void)
//{
//	if(correction_time_way==1)//�Զ���ʱ��������������ʱ��������á�
//	{
//		if(fun25_Cursor_fixed>=6)
//		{
//			if(fun25_Cursor>=8)fun25_Cursor=6;
//			else fun25_Cursor++;
//		}		
//	}	
//	else
//	{
//		if(fun25_Cursor_fixed>=6)
//		{
//			if(fun25_Cursor>=8)fun25_Cursor=0;
//			else fun25_Cursor++;
//		}
//		else
//		{
//			if(fun25_Cursor_fixed==0)
//			{
//			 if(year<1)year=99;
//	      else year--;
//			}
//			else if(fun25_Cursor_fixed==1)
//			{
//				if(month<2)month=12;
//				else month--;				
//			}
//			else if(fun25_Cursor_fixed==2)
//			{
//				if((month==1)||(month==3)||(month==5)||(month==7)||(month==8)||(month==10)||(month==12))
//				{
//				  if(date<2)date=31;
//					else date--;					
//				}
//				else if((month==4)||(month==6)||(month==9)||(month==11))
//				{
//					if(date<2)date=30;
//					else date--;					
//				}
//				else if(month==2)
//				{
//			      if((year+2000)/4>=1)
//						{
//							if(date<2)date=28;//ƽ�꣬��28�졣
//							else date--;						
//						}
//						else
//						{
//							if(date<2)date=29;//����29��
//							else date--;						
//						}
//				}
//			}
//			else if(fun25_Cursor_fixed==3)
//			{
//					if(hours<1)hours=23;
//					else hours--; 		
//			}
//			else if(fun25_Cursor_fixed==4)
//			{
//					if(minute<1)minute=59;
//					else minute--;	
//			}
//			else if(fun25_Cursor_fixed==5)
//			{
//					if(sec<1)sec=59;
//					else sec--;	
//			}			
//		}		
//	}
//}
//void fun25_cancel(void)
//{
//	fun25_Cursor=7;
//	fun25_Cursor_fixed=7;
//	func_index=3;
//}
//void fun25_enter(void)
//{
//	unsigned char buf[7];
//	if(fun25_Cursor_fixed>=6)
//	{
//		fun25_Cursor_fixed=fun25_Cursor;
//		if(fun25_Cursor==6)
//		{
//			buf[0]=year;
//			buf[1]=month;
//			buf[2]=date;
//			buf[3]=hours;
//			buf[4]=minute;
//			buf[5]=sec;
//			RTC_Set((char *)buf);
////			dwin_time_set(buf);
//			func_index=3;
//			fun25_Cursor=7;
//		}else if(fun25_Cursor==7)
//		{
//			func_index=3;
//		}else if(fun25_Cursor==8)		
//		{
//			if(correction_time_way==0)//��תУ��ʱ��ķ�ʽ��
//			{
//				correction_time_way=1;//�Զ�Уʱ
//			}
//			else
//			{
//				correction_time_way=0;//�ֶ�Уʱ
//			}
//			W25q16_Lock();
//			Writeflash_Config(&correction_time_way,TIMING_CMD);
//			W25q16_UnLock();
//			func_index=25;
//		}
//	}
//	else
//	{
//		fun25_Cursor_fixed=7;
//	}
//}
//void fun28_cancel(void)
//{
//	if(input_password_no!=0)
//	{
//		input_password_no--;
//		input_password[input_password_no]=0x20;
//	}
//	else
//	{
//		password_three_ok=0;
//		input_password_no=0;
//		memset(input_password,0x20,4);
//		func_index=0;
//	}
//}
//void fun28_enter(void)
//{
//	if(((input_password_no==4)&&(password_three[0]==input_password[0])&&(password_three[1]==input_password[1])\
//					&&(password_three[2]==input_password[2])&&(password_three[3]==input_password[3]))||
//	((input_password_no==4)&&(password_two[0]==input_password[0])&&(password_two[1]==input_password[1])\
//					&&(password_two[2]==input_password[2])&&(password_two[3]==input_password[3])))
//	{
//		password_three_ok=0;
//		input_password_no=0;
//		memset(input_password,0x20,4);
//		switch(KeyTypeFlag)
//		{
//			case 1:		
//								back_light_change(10);//�������ȣ��رձ��⡣	
//								Reste_Key();	
//								break;
//			case 2:	Duty_Key();		func_index=0;break;
//			case 3:	SelfCheck_Key();break;
//			default:	break;
//		}
//	}
//	else
//	{
//		input_password_no=0;
//		memset(input_password,0x20,4);
//		password_three_ok=2;//�������
//	}
//}
//void fun_cancel(void)
//{
//	switch(func_index)
//	{
//		case 1:	fun1_cancel();	break;
//		case 2:	fun2_cancel();	break;
//		case 3:	fun3_cancel();	break;
//		case 6:	fun6_cancel();	break;
//		case 9:	fun9_cancel();	break;
//		case 12:fun12_cancel();	break;
//		case 15:fun15_cancel();	break;
//		case 18:fun18_cancel();	break;
//		case 21:fun21_cancel();	break;
//		case 22:fun22_cancel();	break;
//		case 25:fun25_cancel();	break;
//		case 28:fun28_cancel();	break;
//		default:		break;
//	}
//}
//void fun_enter(void)
//{
//	switch(func_index)
//	{
//		case 0:	fun0_enter();	break;
//		case 1:	fun1_enter();	break;
//		case 2:	fun2_enter();	break;
//		case 3:	fun3_enter();	break;
//		case 6:	fun6_enter();	break;
//		case 9:	fun9_enter();	break;
//		case 12:fun12_enter();	break;
//		case 15:fun15_enter();	break;
//		case 18:fun18_enter();	break;
//		case 21:fun21_enter();	break;
//		case 22:fun22_enter();	break;
//		case 25:fun25_enter();	break;
//		case 28:fun28_enter();	break;
//		default:		break;
//	}
//}
//void fun_up(void)
//{
//	switch(func_index)
//	{
//		case 0:	fun0_up();	break;
//		case 1:	fun1_up();	break;
//		case 3:	fun3_up();	break;
//		case 6:	fun6_up();	break;
//		case 9:	fun9_up();	break;
//		case 12:fun12_up();	break;
//		case 15:fun15_up();	break;
//		case 18:fun18_up();	break;
//		case 22:fun22_up();	break;
//		case 25:fun25_up();	break;
//		default:		break;
//	}
//}
//void fun_down(void)
//{
//	switch(func_index)
//	{
//		case 0:	fun0_down();	break;
//		case 1:	fun1_down();	break;
//		case 3:	fun3_down();	break;
//		case 6:	fun6_down();	break;
//		case 9:	fun9_down();	break;
//		case 12:fun12_down();	break;
//		case 15:fun15_down();	break;
//		case 18:fun18_down();	break;
//		case 22:fun22_down();	break;
//		case 25:fun25_down();	break;
//		default:		break;
//	}
//}

unsigned char	System_Mod=1;//1=������0=���ԣ�
extern void RefreshRunLedState(void);


void key_Explain(unsigned int key)
{
	unsigned char FLAG=1;

#ifdef FB1010
	//Ȩ������ȡ
	FLAG=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9);//ͨ����͵�ƽ������II��III��Ȩ�ޡ��ϣ�����I��Ȩ�ޣ�û�й��ܡ�
#endif

#ifdef FB1030
	FLAG=0;
#endif	
	
	
	if(FLAG==1)//�� I  �ֱ�������
	{
				switch(key)
				{
						case 0x00000400:
						FaultMute_Key();//������
						VoiceActionClear();//���Ƶƺ�����
						break;//����=��������Ȼ��������
						
						case 0x00000100:
						ManAlarm_Key();//�����ֱ����ݣ�
						break;//�ֱ���
						
				    default:break;
				}	
  }		
	else if(FLAG==0)//ͨ II\ III  ���й���
	{
				switch(key)
				{
					case 0x00000010:
						if(System_Mod==0)
						{System_Mod=1;}
						else
						{System_Mod=0;}
						SystemMod_Key();
						RefreshRunLedState();
						break;//��Ϊ�������빤���л�����
						
					case 0x00000400:
						FaultMute_Key();//������
						VoiceActionClear();//���Ƶƺ�����
						break;//����=��������Ȼ��������
					
					case 0x00000080:
						VoiceActionClear();//���Ƶƺ�����
						SelfCheckStep1();//������ʾ��
						Reste_Key();//�������ݣ���ʱ����λ��
						break;//��λ
					
					case 0x00000100:
						ManAlarm_Key();//�����ֱ����ݣ�
						break;//�ֱ���
					
					case 0x00000001:
						JingQingXiaoChu_Key();//��������
						break;//����������
					
					case 0x00000040:
						Duty_Key(); //���Ӧ��
					break;//���Ӧ��
					
					case 0x00000800:
						SelfCheck_Key();
					 break;//�Լ�
					
					default:break;
				}
  }
	else
	{}
}



//void refresh_lcd(void)//ˢ����Ļ
//{
//	OS_ERR err;
//	unsigned char flag=0;
//	flag=LcdOn();				//����LCD����ʱ800ms�����ٷ��Ͳ�����ʾ����,���Ѿ�������������ʱ
//	back_light_change(Backlight_brightness);//������Ļ����
//	if(flag==1)
//	{
//		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
////		back_light_change(Backlight_brightness);//������Ļ����
////		LCDTime_Init();
//	}
//	current_operation_index=table[func_index].current_operation;
//	(*current_operation_index)();//ִ�е�ǰ��������
//}
OS_TMR selfchecktmr;
unsigned char g_self_check_step=0;
extern OS_TCB	lcdTaskTCB;
extern unsigned char g_RunState;
void selfcheck_stop(void)
{
	OS_STATE state;
	OS_ERR err;
	
	if(g_self_check_step!=0)//������Լ���̣���ֹͣ�Լ�
	{
	g_self_check_step=0;
	
	state=OSTmrStateGet((OS_TMR		*)&selfchecktmr,(OS_ERR*	)&err);
	if(state!=OS_TMR_STATE_UNUSED)
	{
		OSTmrDel((OS_TMR		*)&selfchecktmr,(OS_ERR*	)&err);
	}
	
//  RefreshRunLedState();
    RunState_Action(g_RunState);
	}
}
void selfchecktmr_callback(void *p_tmr,void *p_arg)
{
	CPU_TS ts;
	OS_ERR err;
	//if(g_self_check_step<8)g_self_check_step+=1;
	if(g_self_check_step<4)g_self_check_step+=1;
	else g_self_check_step=0;
//	LcdOn();
	OS_TaskQPost((OS_TCB	*)&lcdTaskTCB,
				(void		*)&g_self_check_step,
				(OS_MSG_SIZE)sizeof(g_self_check_step),
				(OS_OPT		)OS_OPT_POST_FIFO,
				(CPU_TS		)ts,
				(OS_ERR		*)&err
				);
}
void selfchecktimer_on(unsigned int n100sec)
{
	OS_ERR err;
	OS_STATE state;
	state=OSTmrStateGet((OS_TMR		*)&selfchecktmr,(OS_ERR*	)&err);
	if(state!=OS_TMR_STATE_UNUSED)
	{
		OSTmrDel((OS_TMR		*)&selfchecktmr,(OS_ERR*	)&err);
	}
	if(n100sec!=0)
	{
		OSTmrCreate((OS_TMR		*)&selfchecktmr,
				(CPU_CHAR	*)"selfchecktmr",
				(OS_TICK	)n100sec*20,
				(OS_TICK	)0,
				(OS_OPT		)OS_OPT_TMR_ONE_SHOT,
				(OS_TMR_CALLBACK_PTR)selfchecktmr_callback,
				(void*		)0,
				(OS_ERR*	)&err);
		OSTmrStart((OS_TMR		*)&selfchecktmr,(OS_ERR*	)&err);
	}
}		

void check_self_fun(unsigned char state)//�Լ�
{

	switch(g_self_check_step)
	{
		case 1:
				SelfCheckStep1();
				selfchecktimer_on(20);
			break;
		case 2:
				SelfCheckStep2();
				selfchecktimer_on(20);
			break;
		case 3:
//				if((state&0x60)==0x00)
//				{
//				}
//				else if((state&0x60)==0x20)
//				{
//				}
//				else if((state&0x60)==0x40)
//				{
//				}
//				else if((state&0x60)==0x60)
//				{
//				}
		    SelfCheckStep3();
		    NetCHECK();
				selfchecktimer_on(5);
			break;
		case 4:
				SelfCheckStep4();
				selfchecktimer_on(5);
  		  RefreshRunLedState();
			break;
		default:
				g_self_check_step=0;
			break;
	}
}
//void LcdSelfCheck(unsigned char RunState)
//{
//	check_self_fun(RunState);
//}
void lcd_SelfCheck(unsigned char RunState)
{
	OS_ERR err;
	check_self_fun(RunState);
	OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
}
//void fun30(void)
//{
//	u16 add0=0x1001;//��
//	u16 add1=0x1003;//��
//	u16 add2=0x1970;//�û���������С�����
//	u16 add3=0x1990;//�û��������ɹ���	
//	u16 add4=0x19B0;//��ϵ磬ȡ���ڴ濨��Ȼ�������ϵ硣	
//	unsigned char SEND_TEXT[100];//���崮��2��������
//	unsigned char len=0;	
//	unsigned char step=1;//��չ���Ĳ��衣
//	change_back_picture(0x002F);//��ȡ����ɫ����ͼƬ
//	input_text(add0,2,"��");//��ʾ����
//	input_text(add1,2,"��");//��ʾ����
//	switch(step)
//	{
//		case 0:	//�û���������С�����
//			input_text(add2,22,"�û���������С�����");
//			input_text(add3,20,"                    ");	
//			input_text(add4,34,"                                  ");			
//		break;
//		case 1:	
//			input_text(add2,22,"�û���������С�����");
//			input_text(add3,20,"�û��������ɹ���");	
//			input_text(add4,34,"��ϵ磬ȡ���ڴ濨��Ȼ�������ϵ硣");		
//		break;	
//		default:	
//			break;
//	}	
//}

///*�������*/
//void SysSoftReset1(void)
//{
//	printf("�������������\r\n");		
//	__set_FAULTMASK(1); 
//	NVIC_SystemReset(); 
//}

//void master_exportmessage_to_SD(unsigned char step)//��ʾ������Ϣ������SD�����档  step�����չ���Ĳ��衣
//{
//	u16 add0=0x1001;//��
//	u16 add1=0x1003;//��
//	u16 add2=0x1900;//��ʷ�������ݵ����С�����
//	u16 add3=0x1920;//��ʷ�������ݵ����ɹ���	
//	u16 add4=0x1940;//��ϵ磬ȡ���ڴ濨��Ȼ�������ϵ硣	
//	unsigned char SEND_TEXT[100];//���崮��2��������
//	unsigned char len=0;	
//	u32 count=0;//��ʷ�������ݵ����ɹ���		
//	
//	
//	change_back_picture(0x002F);//��ȡ�������ͼƬ
//	input_text(add0,2,"��");//��ʾ����
//	input_text(add1,2,"��");//��ʾ����
//	switch(step)
//	{
//		case 0:	//��ʷ�������ݵ����С�����
//			input_text(add2,24,"��ʷ�������ݵ����С�����");
//			input_text(add3,22,"                    ");	
//			input_text(add4,34,"                                  ");			
//		break;
//		
//		case 1:	
//			input_text(add2,24,"��ʷ�������ݵ����С�����");
//			input_text(add3,22,"��ʷ�������ݵ����ɹ���");	
//			input_text(add4,34,"��ϵ磬ȡ���ڴ濨��Ȼ�������ϵ硣");		
//		  while(1)
//			{
//				count++;
//			 if(count>10000000)
//			 {SysSoftReset1();}
//			}
//		break;

//			default:	
//			input_text(add2,24,"��ʷ�������ݵ����С�����");
//			input_text(add3,22,"��ʷ�������ݵ���ʧ�ܡ�");	
//			input_text(add4,34,"��ϵ磬�����ڴ濨��Ȼ�������ϵ硣");		
//		  while(1)
//			{
//				count++;
//			 if(count>10000000)
//			 {SysSoftReset1();}			
//			;}
//		break;
//	}	
//}
//void SD_input_message_to_master(unsigned char step)//��ʾ�û���Ϣ�����ͨ��SD�����뵽�û���Ϣ����װ����  step�����չ���Ĳ��衣
//{
//	u16 add0=0x1001;//��
//	u16 add1=0x1003;//��
//	u16 add2=0x1970;//�û���������С�����
//	u16 add3=0x1990;//�û��������ɹ���	
//	u16 add4=0x19B0;//��ϵ磬ȡ���ڴ濨��Ȼ�������ϵ硣	
//	unsigned char SEND_TEXT[100];//���崮��2��������
//	unsigned char len=0;
//	change_back_picture(0x0030);//��ȡ����ɫ����ͼƬ
//	input_text(add0,2,"��");//��ʾ����
//	input_text(add1,2,"��");//��ʾ����
//	switch(step)
//	{
//		case 0:	//�û���������С�����
//			input_text(add2,22,"�û���������С�����");
//			input_text(add3,20,"                    ");	
//			input_text(add4,34,"                                  ");			
//		break;
//		case 1:	
//			input_text(add2,22,"�û���������С�����");
//			input_text(add3,20,"�û��������ɹ���");	
//			input_text(add4,34,"��ϵ磬ȡ���ڴ濨��Ȼ�������ϵ硣");		
//			while(1)
//			{;}
//		break;	
//		case 2:	
//			input_text(add2,22,"�û���������С�����");
//			input_text(add3,20,"�û��������ʧ�ܡ�");	
//			input_text(add4,34,"��ϵ磬����ڴ濨��Ȼ�������ϵ硣");		
//			while(1)
//			{;}
//		break;			
//		default:	
//			break;
//	}
//}

//void display_SD_schedule()//��ʾSD�������û��������ȡ�
//{
//	unsigned char data_local[100]={0x20};
//	unsigned int schedule=0;//����	
//	unsigned int i=0;
//	unsigned char len=0;
//		LCD_SET_ON();//������Ļ
//		schedule=FBWBReadStatus;
//		memset(data_local,0x20,100);//		
//		memcpy(data_local,"������ȣ�",10);
//		data_local[11]=schedule/100+0x30;// ȡ���ȵİ�λ������		
//		data_local[12]=(schedule%100)/10+0x30;// ȡ���ȵ�ʮλ������
//		data_local[13]=((schedule%100)%10)%10+0x30;// ȡ���ȵĸ�λ������
//		data_local[14]='%';		
//		input_text(0x2F80,15,data_local);

//		memset(data_local,0x20,100);//		
//		memcpy(data_local,"��ǰ��ȡ��������",16);
//		data_local[17]=FBWBReadNbr/10000+0x30;// ȡ���ȵ���λ������		
//		data_local[18]=(FBWBReadNbr%10000)/1000+0x30;// ȡ���ȵ�ǧλ������		
//		data_local[19]=((FBWBReadNbr%10000)%1000)/100+0x30;// ȡ���ȵİ�λ������			
//		data_local[20]=(((FBWBReadNbr%10000)%1000)%100)/10+0x30;// ȡ���ȵ�ʮλ������				
//		data_local[21]=(((FBWBReadNbr%10000)%1000)%100)%10+0x30;// ȡ���ȵĸ�λ������			
//		input_text(0x2FA0,22,data_local);

//		memset(data_local,0x20,100);//
//		memcpy(data_local,"��ǰ��ȡʧ�ܵ�������",20);
//		data_local[21]=FBWBReadFail/10000+0x30;// ȡ���ȵ���λ������
//		data_local[22]=(FBWBReadFail%10000)/1000+0x30;// ȡ���ȵ�ǧλ������
//		data_local[23]=((FBWBReadFail%10000)%1000)/100+0x30;// ȡ���ȵİ�λ������
//		data_local[24]=(((FBWBReadFail%10000)%1000)%100)/10+0x30;// ȡ���ȵ�ʮλ������
//		data_local[25]=(((FBWBReadFail%10000)%1000)%100)%10+0x30;// ȡ���ȵĸ�λ������
//		input_text(0x2FC0,26,data_local);

//		memset(data_local,0x20,100);//
//		memcpy(data_local,"ʣ��ʱ��Ϊ��",12);
//		data_local[13]=i/100+0x30;// ȡ���ȵİ�λ������
//		data_local[14]=(i%100)/10+0x30;// ȡ���ȵ�ʮλ������
//		data_local[15]=((i%100)%10)%10+0x30;// ȡ���ȵĸ�λ������
//		data_local[16]=0xC3;//��	
//		data_local[17]=0xEB;//			
//		input_text(0x2FE0,18,data_local);
//}


//void LCD_backlight_low(void)
//{
//	unsigned char test[50];	
//	unsigned char SEND_BUFER[50];//���崮�ڷ�������
//	unsigned int crc1;
//	unsigned char i=0;	
//	OS_ERR err;
//	for(i=0;i<50;i++)
//	{
//		uart5_receive_data[i]=0;
//	}
//	uart5_receive_data_inc=0;

//	SEND_BUFER[0]=0x5A;//������Ļ����
//	SEND_BUFER[1]=0xA5;
//	SEND_BUFER[2]=0x05;//���ݳ���,���ݵĳ��Ȱ���У��λ��2λ��	
//	SEND_BUFER[3]=0x80;
//	SEND_BUFER[4]=0x01;
//	SEND_BUFER[5]=0x00;//��Ļ���ȵ�ֵ��00~40  00��Ϣ����
//	crc1=crc16((unsigned char*)&SEND_BUFER[3],3);//У�����������3��4��5������Ԫ�ء�
//	SEND_BUFER[6]=crc1&0x00ff;//У��͵ĵ�λ��
//	SEND_BUFER[7]=crc1>>8;		 //У��͵ĸ�λ��
////	USART1_Send_Data(SEND_BUFER,8);//����		
////5A A5 05 80 01 20 70 60

//	for(i=0;i<3;i++)//��ֹһ�ζ�ȡ���ɹ���
//	{
//		i++;
//		SendPacketToLcd1(SEND_BUFER,8);
//		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
//		if( (uart5_receive_data[19]==uart5_receive_data[18])&&(uart5_receive_data[18]==uart5_receive_data[17])&&(uart5_receive_data[19]==0) )
//		{
//			
//		}
//		else
//		{
//			i=4;//����forѭ����
//		}
//	}
////	USART1_Send_Data(uart5_receive_data,50);//����	
//}



//��������
//2.�����µĽ�����ʾ������Ϣ����Ļ�ϣ������������ݣ�
//������ַ����FLASH�ж�ȡ����
//��������ַ����FLASH�ж�ȡ����
//K2��Ϣ����̬��̬���á�����IP��Ŀ���ַ�˿ںš�����״̬����K2�ж�ȡ����
//GM3��Ϣ��IMEI��SN��Ŀ���ַ�˿ںš�����״̬�������ź�ǿ�ȣ���GM3�ж�ȡ����
//�������汾�ţ���FLASH�ж�ȡ��

//����ͨ������
//�����ӿ���ʾ��Ϣ����ʾЭ�����ͣ������ͺţ����汾�ţ�Э���������汾�ţ����ӿ����ͷ�ʽ��CAN/RS485�������ʡ�У��λ������λ��ֹͣλ�������ܣ���4�죻

//����������ߵ��������豸�����𾯵�ʱ�������Ų���ʾ�����ܣ�1��

//����װ���Գ�����FBAT����ID������GM3���á�����K2���õĳ���ϲ�Ϊͬһ����������󷢸��ֳ�������ʹ�ã����ܣ�
//22��ǰ��ɣ�

//���ϴ���ָʾ����ȷ���û��޷�ֱ����⣻
//��ť����ɫ���ģ�

void ID_Print(void)
{
	unsigned char i=0;
	
	printf("1.�豸ID=");
	for(i=0;i<6;i++)
	{
	printf("%x_",Local_Addres[i]);
	}
	printf("\r\n");
}
