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
////unsigned char Loop_Part_flag=0;//区分用户编码表编号和正常回路部件号的标志位。1的时候，为用户编码表号码，0的时候为回路部件号。
//unsigned char cancel_flag=0;
//unsigned char register_card_flag=0;//注册板卡标志位。0的时候不进入注册板卡页面，1的时候进入。
unsigned char correction_time_way=1;//校正时间方式。分为手动和自动。0:手动。1：自动。

unsigned char Local_SoftVer[2];
//unsigned char Local_Addres[6]={0x10,0x07,0x07,0x00,0x09,0x01};//测试用；
//unsigned char Local_Addres[6]={0x11,0x07,0x07,0x00,0x09,0x87};//测试用；
//unsigned char Local_Addres[6]={0x12,0x07,0x07,0x00,0x09,0x01};//测试用；
//unsigned char Local_Addres[6]={0x12,0x07,0x07,0x00,0x09,0x02};//测试用；

unsigned char Local_Addres[6]={0x00,0x00,0x00,0x00,0x00,0x00};//测试用；

 
//static unsigned int Lcd_Flash_Cnt=20;
////手报、火警、故障次数记录
//static unsigned int Manual_alarm_Cnt=0;
//static unsigned int Host_alarm_Cnt=0;
//static unsigned int Host_Flult_Cnt=0;

//static unsigned int func_index=0;		//目前显示界面号
//static unsigned char fun0_Cursor=0;		//0界面光标位置
//static unsigned char signal_flag=0;		//上发信号指示

//static unsigned char fun1_Cursor=0;		//1界面光标位置
//static unsigned int fun1_page=1;
//static unsigned int fun1_page_fixed=1;

////static unsigned char fun2_Cursor=0;		//2界面光标位置
//static unsigned char input_password_no=0;
//static unsigned char password_three_ok=0;
//static unsigned char input_password[4]={0x20,0x20,0x20,0x20};
////static unsigned char input_password_two[4]={0x20,0x20,0x20,0x20};

//static unsigned char fun3_Cursor=0;		//3界面光标位置

//static unsigned char fun6_Cursor=0;		//6界面光标位置
//static unsigned char net_fixed=1;		//有线连接、无线连接显示


//static unsigned char fun9_Cursor=0;		//9界面光标位置
//static unsigned char host_fixed=1;

////static unsigned char Board_card_flag=0;//进入的哪个板卡。
//unsigned short register_flag=0x0000;//需要显示的板卡信息  //注册成功标志位。1的时候，注册成功，0的时候注册失败。
//static unsigned char fun12_Cursor=0;	//12界面光标位置
//static unsigned char fun12_Cursor_fixed=4;
//static unsigned char rs232baund=0;
//static unsigned char rs232parity=0;
//static unsigned char rs232stopbit=0;
//static unsigned char rs232baund_fixed=0;
//static unsigned char rs232parity_fixed=0;
//static unsigned char rs232stopbit_fixed=0;

//static unsigned char fun15_Cursor=0;	//15界面光标位置
//static unsigned char fun15_Cursor_fixed=2;
//static unsigned char CANbaund=0;
//static unsigned char CANbaund_fixed=0;

//static unsigned char fun18_Cursor=0;	//18界面光标位置
//static unsigned char fun18_Cursor_fixed=3;
//static unsigned char Backlight_time=0;
//static unsigned char Backlight_brightness=0;
//static unsigned char Backlight_time_fixed=0;
//static unsigned char Backlight_brightness_fixed=0;
//static unsigned int Backlight__keep_time=LCDBright10S;//背光保持时间

////static unsigned char fun21_Cursor=0;	//21界面光标位置
//static unsigned char input_password_times=0;

//static unsigned char fun22_Cursor=0;	//22界面光标位置
//static unsigned char sound=0;
//static unsigned char sound_fixed=0;

//static unsigned char fun25_Cursor=0;	//25界面光标位置
//static unsigned char fun25_Cursor_fixed=0;	//25界面光标位置
//static unsigned char year=0;
//static unsigned char month=0;
//static unsigned char date=0;
//static unsigned char hours=0;
//static unsigned char minute=0;
//static unsigned char sec=0;

//static unsigned char KeyTypeFlag=0;		//28界面，需要输入密码的按键种类，复位、自检、查岗

//static unsigned char password_two[4]={'0','1','1','9'};		//二级密码
//const unsigned char password_three[4]={'1','8','0','6'};	//三级密码

////建筑消防主机状态
//const char ExtensionStatus[74][16]=
//{
//"测试","正常运行","火警","故障","故障恢复",\
//"复位","屏蔽","屏蔽解除","隔离","隔离解除",\
//"确认","监管","监管撤销","启动","自动停止",\
//"手动启动","手动停止","手动报警","巡检","自检",\
//"查岗应答","反馈","反馈撤销","反馈缺失","延时",\
//"设施在位","设施离位","防火门打开","防火门关闭","压力正常",\
//"低压报警","高压报警","压力恢复正常","设施离位报警","设施恢复在位",\
//"常闭防火门打开","常开防火门关闭","消防通道堵塞","消防通道正常","阀门异常",\
//"阀门正常","标签故障","回路板短路","回路板断路","回答",\
//"回答撤销","电源故障","备电故障","备电恢复","电源故障恢复",\
//"打印机故障","打印机故障恢复","开机","关机","联动请求",\
//"联动回答","联动回答消除","消音","电气火灾报警","常闭防火门关闭",\
//"常开防火门打开","水位过低","水位过高","水位恢复正常","水温过低",\
//"水温过高","水温恢复正常","漏电报警","撞击","倾斜",\
//"盗采","掩埋","消火栓盖打开","未知"
//};
////建筑消防设施系统状态
//const char HostStatus[14][15]={"主机正常","主机火警","主机故障","主机屏蔽","主机监管",\
//							"主机启动","主机反馈","主机延时状态","主机主电故障","主机备电故障",\
//							"主机总线故障","主机手动状态","主机配置改变","主机复位"};
////建筑消防设施部件运行状态
//const char HostPartsStatus[9][9]={"正常运行","火警","故障","屏蔽","监管","启动","反馈","延时状态","电源故障"};
////建筑消防设施操作信息
//const char HostOperation[8][15]={"无操作","主机复位","主机消音","主机手动报警","主机警情消除","主机自检","主机确认","主机测试"};
////用户信息传输装置操作信息
//const char LocalOperation[8][15]={"本机复位","本机消音","本机手动报警","本机警情消除","本机自检","本机查岗应答","本机测试","本机故障消除"};
////故障代码
////const char FaultCode[16][13]={"无故障","故障代码:01","故障代码:02","故障代码:03","故障代码:04","故障代码:05","故障代码:06","故障代码:07","故障代码:08"\
////								,"故障代码:09","故障代码:10","故障代码:11","故障代码:12","故障代码:13","故障代码:14","故障代码:15"};
//const char FaultCode[16][20]={"无故障","主电","备电","主电+备电","链路","链路+主电","链路+备电","链路+备电+主电","线路" 
//								,"线路+主电","线路+备电","线路+备电+主电","线路+链路","线路+链路+主电","线路+链路+备电","线路+链路+备电+主电"};
////RS232波特率设置
//const char SetRS232Baund[17][12]={"1200 bps","2400 bps","4800 bps","9600 bps","14400 bps","19200 bps","38400 bps","43000 bps"\
//								,"57600 bps","76800 bps","115200 bps","128000 bps","230440 bps","256000 bps","460800 bps"\
//								,"921600 bps","1382400 bps"};
////RS232校验位设置
//const char SetRS232Parity[3][7]={"无校验","奇校验","偶校验"};
////RS232停止位设置
//const char SetRS232StopBit[3][4]={"1","1.5","2"};
////can波特率设置
//const char SetCANBaund[15][12]={"3Kbps","5Kbps","10Kbps","20Kbps","40Kbps","50Kbps","80Kbps","100Kbps"\
//								,"125Kbps","200Kbps","250Kbps","400Kbps","500Kbps","800Kbps","1000Kbps"};
////网络设置
//const char SetNetMode[4][9]={"        ","有线连接","无线连接","无信号"};
////注册协议板完成标志位；
//unsigned char RegisteProComplete=0;

//OS_TMR LcdPowertmr;
////函数声明
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
//	//20180507,临界区保护，如果产生中断，很可能导致，显示失败。
//	CPU_SR_ALLOC();
//	OS_CRITICAL_ENTER();//进入临界区
//	//////////
//	flag=UART5_Send_Data(Data,Lenth);
//	delay_ms(1);
//	//////////
//	OS_CRITICAL_EXIT();	//退出临界区	 
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



//unsigned char input_extensionStatus(unsigned char *input_data,unsigned char lenth_now,unsigned int input_id,unsigned char *output_data)//输入要显示的建筑消防主机状态
//{
////	unsigned char input_data[40]={0};//输入的数组。	
////	unsigned char lenth_now=0;//当前输入数组的长度	
////	unsigned int input_id=0;//输入的要查表的位置。
////	unsigned char output_data[40]={0};//返回整合后的数组。
//	unsigned char num=0;//计算出实际数组标号
////	unsigned char lenth=0;//计算出实际数组长度	
//	unsigned char i=0;//for 用	
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


////在中断函数中调用，自动存储
//void Uart5_RxISR(unsigned char buf)
//{
//	static unsigned char UART5_RX_BUF[20];					//读取接收到的迪文屏幕的数据数组
//	static unsigned char UART5_RX_NO=0;						//串口数组接收数据的累加个数
////	static unsigned char DWIN_RX_RIGHT=0;					//如果收到屏幕的确认帧，则将这个值置一。
//	
//	UART5_RX_BUF[UART5_RX_NO]=buf;
//	UART5_RX_NO++;											//每组串口数据收到的数据个数累加
//	if(UART5_RX_NO==20)
//	{
//		UART5_RX_NO=0;
//	}
//	if((UART5_RX_BUF[0]==0x5A)&&(UART5_RX_BUF[1]==0xA5))	//收到屏幕发回来的确认帧。0x5A,0xA5为包头。
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
//	//执行退出
//	while((func_index!=0)&&(i!=0))
//	{
//		i--;
//		input_password_no=0;
//		fun_cancel();
//	}
////	LCD_POWEROFF();

//  LCD_backlight_low();//将亮度调节到最低。	

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

////小于99以内的数字转换为字符串
////大于99的数显示99+；
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
////更换背景图片
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
////调取变量图标
//void input_small_picture(unsigned int add,unsigned int pic)
//{
//	unsigned char SEND_BUFER[10];//定义串口发送数组
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
////输入汉字文本
//void input_text(unsigned int add,unsigned char data_long,unsigned char *text)
//{
//	unsigned char SEND_BUFER[100];//定义串口发送数组
//	unsigned int crc1=0;
//	
//	if(data_long+8<100)
//	{
//		memset(SEND_BUFER,0,100);
//		SEND_BUFER[0]=0x5A;
//		SEND_BUFER[1]=0xA5;	
//		SEND_BUFER[2]=data_long+5;//数据长度	
//		SEND_BUFER[3]=0x82;
//		SEND_BUFER[4]=(unsigned char)(add>>8);//地址高位
//		SEND_BUFER[5]=(unsigned char)add;//地址低位	
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
//void back_light_change(unsigned char Backlight_brightness)//调节亮度
//{
//	unsigned char SEND_BUFER[50];//定义串口发送数组
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
////char转换为BCD码
//static unsigned char B_BCD(unsigned char Byte)
//{
//    unsigned char BCDnum=0;
//    BCDnum=((Byte%100)/10)<<4;
//    BCDnum|=Byte%10;
//    return BCDnum;
//}

////data存放顺序为年月日时分秒的数组，占用6个byte
//void dwin_time_set(const char *data)//输入时间更改迪文屏内部RTC时间
//{
//	unsigned char SEND_BUFER[50];//定义串口发送数组
//	unsigned int crc1;

//	SEND_BUFER[0]=0x5A;
//	SEND_BUFER[1]=0xA5;
//	SEND_BUFER[2]=0x0C;//数据长度,数据的长度包含校验位的2位。	
//	SEND_BUFER[3]=0x80;
//	SEND_BUFER[4]=0x1F;
//	SEND_BUFER[5]=0x5A;
//	SEND_BUFER[6]=B_BCD(*data++);//年
//	SEND_BUFER[7]=B_BCD(*data++);//月
//	SEND_BUFER[8]=B_BCD(*data++);//日
//	SEND_BUFER[9]=0x00;//
//	SEND_BUFER[10]=B_BCD(*data++);//时
//	SEND_BUFER[11]=B_BCD(*data++);//分
//	SEND_BUFER[12]=B_BCD(*data);//秒

//	crc1=crc16((unsigned char*)&SEND_BUFER[3],10);
//	SEND_BUFER[13]=crc1&0x00ff;//校验和的低位。
//	SEND_BUFER[14]=crc1>>8;		 //校验和的高位。
//	
//	SendPacketToLcd(SEND_BUFER,15);
//}
//unsigned char dwin_time_read(char *time_data)//读取迪文屏内部RTC时间
//{
//	unsigned char test[50];	
//	unsigned char SEND_BUFER[50];//定义串口发送数组
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
//	SEND_BUFER[2]=0x05;//数据长度,数据的长度包含校验位的2位。	
//	SEND_BUFER[3]=0x81;
//	SEND_BUFER[4]=0x20;
//	SEND_BUFER[5]=0x07;
//	crc1=crc16((unsigned char*)&SEND_BUFER[3],3);//校验包括。数组3，4，5这三个元素。
//	SEND_BUFER[6]=crc1&0x00ff;//校验和的低位。
//	SEND_BUFER[7]=crc1>>8;		 //校验和的高位。
//	for(i=0;i<3;i++)//防止一次读取不成功。
//	{
//		i++;
//		SendPacketToLcd(SEND_BUFER,8);
//		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
//		if( (uart5_receive_data[19]==uart5_receive_data[18])&&(uart5_receive_data[18]==uart5_receive_data[17])&&(uart5_receive_data[19]==0) )
//		{
//			
//		}
//		else
//		{
//			i=4;//跳出for循环。
//		}
//	}
////	*data++=calendar.sec;
////	*data++=calendar.min;
////	*data++=calendar.hour;
////	*data++=calendar.w_date;
////	*data++=calendar.w_month;
////	*data=calendar.w_year%100;
////	time_data[0]=uart5_receive_data[13];//年
////	time_data[1]=uart5_receive_data[14];//月
////	time_data[2]=uart5_receive_data[15];//日
////	time_data[3]=uart5_receive_data[17];//时	
////	time_data[4]=uart5_receive_data[18];//分	
////	time_data[5]=uart5_receive_data[19];//秒
//	time_data[0]=uart5_receive_data[19];//秒
//	time_data[1]=uart5_receive_data[18];//分	
//	time_data[2]=uart5_receive_data[17];//时	
//	time_data[3]=uart5_receive_data[15];//日
//	time_data[4]=uart5_receive_data[14];//月
//	time_data[5]=uart5_receive_data[13];//年
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
////	printf("年月日\r\n");
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

//void handle_arrays_u8(unsigned char *data,unsigned char size,unsigned char *printf_data)//处理数组，DATA是要处理的数组，SIZE是数组的长度。//比如将0x25分开就是0x32和0x35.。0xAB分开就是0x40,0x41. 
//{//DATA是输入的数组，SIZE是数组中元素的数量，Printf_data是整理完后用于测试打印的数组。
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
//		switch(DATA_Printf_H)//判断Buzzer_flag来判断应该如何鸣叫
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
//		switch(DATA_Printf_L)//判断Buzzer_flag来判断应该如何鸣叫
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
//void display_text(unsigned int add,unsigned char *data,unsigned char lenth)//add是显示的位置。data是数据。lenth代表data数组内元素的数量。
//{
//	unsigned char i=0;
//	unsigned int crc1=0;
//	unsigned char SEND_BUFER[100]={0x20};			//定义串口发送数组
////1090	10AA	10C4
////10E0	10FA	1114
////1130	114A	1164
//		SEND_BUFER[0]=0x5A;
//		SEND_BUFER[1]=0xA5;
//		SEND_BUFER[2]=lenth+5;//数据长度。			
//		SEND_BUFER[3]=0x82;//数据命令类型
//		SEND_BUFER[4]=(unsigned char)(add>>8);	//地址高位
//		SEND_BUFER[5]=(unsigned char)add;		//地址低位
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




//void input_alarm(unsigned int add,unsigned int sn,unsigned char *data,unsigned char Host_NO)//add是显示的位置。sn是显示第几页。data是数据。
//{
//	unsigned char breakdown_flag=0;//故障标志位。
//	unsigned char i=0;
//	unsigned int num=0;
//	unsigned char t=0;
//	unsigned char data_long=0;
//	unsigned int crc1=0;
//	unsigned char SEND_BUFER[100]={0x20};			//定义串口发送数组
////	unsigned char HostNO;
//	unsigned int  LoopNO,PartNO;
//	unsigned char data_lenth=0;
//	unsigned char test1[40]={0};	
//	unsigned char test2[40]={0};
//	
//	unsigned char time_data[40]={0x20};//时间数组
//	unsigned char time_data_lenth=0;	

//	unsigned char type_data[40]={0x20};//类型数组
//	unsigned char type_data_lenth=0;		
//	
//	unsigned char message_data[40]={0x20};//信息数组
//	unsigned char message_data_lenth=0;		

//	unsigned char index_data[40]={0x20};//序号数组
//	unsigned char index_data_lenth=0;
//	
//	unsigned int  Loop_Part=0;//用户编码表号码
//	
//	if( (data[0x19]==0)&&(data[0x18]==0)&&(data[0x17]==0)&&(data[0x16]==0) )
//	{

//	}
//	else
//	{
///******************清空缓存内容*****************/
//	memset(SEND_BUFER,0x20,100);//将100个0x20放进SEND_BUFER中。清空写屏数据数组。
///******************提取数据索引*****************/
//	memset(index_data,0x20,40);//将100个0x20放进SEND_BUFER中。
//		if(sn==0)//如果页码为0，说明是显示的主页面三条信息。 主页面三条信息不用显示页码。
//		{

//		}
//		else//显示报警内容的条数。
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
///******************提取时间*****************/
//		memset(time_data,0x20,40);//将100个0x20放进SEND_BUFER中。	
//		if(sn==0)//页码。 
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
//		if(data[0]!=0x20)//页码。 
//		{
//		time_data[t++]=data[0x1b]%100/10+0x30;
//		time_data[t++]=data[0x1b]%10+0x30;
//		time_data[t++]=0xC4;		//年
//		time_data[t++]=0xEA;
//		time_data[t++]=data[0x1a]%100/10+0x30;
//		time_data[t++]=data[0x1a]%10+0x30;
//		time_data[t++]=0xD4;		//月
//		time_data[t++]=0xC2;
//		time_data[t++]=data[0x19]%100/10+0x30;
//		time_data[t++]=data[0x19]%10+0x30;
//		time_data[t++]=0xC8;		//日
//		time_data[t++]=0xD5;
//		time_data[t++]=0x20;		//空格
//		time_data[t++]=data[0x18]%100/10+0x30;
//		time_data[t++]=data[0x18]%10+0x30;
//		time_data[t++]=0x3A;		//：
//		time_data[t++]=data[0x17]%100/10+0x30;
//		time_data[t++]=data[0x17]%10+0x30;
//		time_data[t++]=0x3A;
//		time_data[t++]=data[0x16]%100/10+0x30;
//		time_data[t++]=data[0x16]%10+0x30;
//	}
////		time_data_lenth=t;
///******************提取类型*****************/
//		memset(type_data,0x20,40);//将100个0x20放进SEND_BUFER中。
//		switch(data[0])//判定字节长度。
//		{
//			case 1:						//上传建筑消防设施系统状态
//				num=BitToNum(data[4]+data[5]*256);
////			  num=data[4]+data[5]*256;
//				if(num<14)
//				{
//					data_lenth=strlen((char *)&HostStatus[num]);//主机状态。
//					data_lenth=data_lenth/2;
//				}
//				break;
//			case 2:						//上传建筑消防设施部件运行状态
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
//					data_lenth=input_extensionStatus(test1,0,num,test2);//输入要显示的建筑消防主机状态
//					data_lenth=data_lenth/2;
////				}				
//			break;
//			case 4:						//上传建筑消防设施操作信息
////				num=BitToNum(data[4]);
////			num=data[4];
//			num=DataCharge(data[4]);
//				if(num<7)
//				{
//					data_lenth=strlen((char *)&HostOperation[num]);
//					data_lenth=data_lenth/2;
//				}
//			break;
//			case 24:						//上传用户信息传输装置操作信息
//			num=BitToNum(data[2]);
////				  num=data[2];
//				if(num<8)
//				{
//					data_lenth=strlen((char *)&LocalOperation[num]);
//					data_lenth=data_lenth/2;
//				}
//			break;			
//			case 21://故障类型
//					data_lenth=2;
//			break;
//			default:
////					data_lenth=2;
//			break;
//		}
////判断添加空格的个数。
//		if(data_lenth==2)//判断，如果类型是2个汉字。
//		{
//			t=12;				
//		}
//		else if(data_lenth==4)//判断，如果类型是4个汉字。
//		{
//			t=10;
//		}
//		else if(data_lenth==5)//判断，如果类型是5个汉字。
//		{
//			t=9;
//		}		
//		else if(data_lenth==6)//判断，如果类型是6个汉字。
//		{
//			t=8;
//		}
//		else if(data_lenth==7)//判断，如果类型是7个汉字。
//		{
//			t=7;
//		}		
//		else
//		{
//			t=6;		
//		}
//		if(sn==0)//页码。 
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
//			case 1:						//上传建筑消防设施系统状态			
//			  Host_NO=data[3]>>4;	//协议卡号；
//			  num=BitToNum(data[4]+data[5]*256);
////			  num=data[4]+data[5]*256;
//				if(num<14)
//				{
//					i=strlen((char *)&HostStatus[num]);
//					memcpy((char *)&type_data[t],(char *)&HostStatus[num],i);
//					t+=i;
//				}
//				breakdown_flag=1;//故障标志位。
//				break;
//			case 2:						//上传建筑消防设施部件运行状态
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
////				breakdown_flag=1;//故障标志位。


//			
////				num=BitToNum(data[2]);
////					num=BitToNum(data[9]+data[10]*256);	
//					num=data[9]+data[10]*256;				
////				if(num<9)
////				{
//					data_lenth=input_extensionStatus(type_data,t,num,test1);//输入要显示的建筑消防主机状态
//					for(i=0;i<(t+data_lenth);i++)
//					{
//						type_data[i]=test1[i];
//					}
//					
////				}
//				Host_NO=data[3]>>4;	//协议卡号；
//				LoopNO=data[6];
//				LoopNO=LoopNO<<8;
//				LoopNO=LoopNO+data[5];
//				PartNO=data[8];
//				PartNO=PartNO<<8;
//				PartNO=PartNO+data[7];				
//				breakdown_flag=1;//故障标志位。							
//			break;	
//			case 4:						//上传建筑消防设施操作信息
////				num=BitToNum(data[4]);
////			  num=data[4];		
//			Host_NO=data[3]>>4;	//协议卡号；			
//			num=DataCharge(data[4]);			
//				if(num<7)
//				{
//					i=strlen((char *)&HostOperation[num]);
//					memcpy((char *)&type_data[t],(char *)&HostOperation[num],i);
//					t+=i;
//				}
//				breakdown_flag=1;
//			break;	
//			case 24:						//上传用户信息传输装置操作信息
//				num=BitToNum(data[2]);
////			  num=data[2];
//				if(num<8)
//				{
//					i=strlen((char *)&LocalOperation[num]);
//					memcpy((char *)&type_data[t],(char *)&LocalOperation[num],i);
//					t+=i;
//				}
//			break;
////			case ?:						//建筑消防主机状态（与服务器同步）
////				num=BitToNum(data[2]);
////				if(num<8)
////				{				
////					input_extensionStatus(type_data,t,num,test1,data_lenth);//输入要显示的建筑消防主机状态
////					for(i=0;i<(t+data_lenth);i++)
////					{
////						type_data[i]=data_lenth[i];
////					}
////				}
////			break;						
//				
//////			case ?:						//建筑消防主机状态（与服务器同步）
//////				num=BitToNum(data[2]);
//////				if(num<8)
//////				{
//////					i=strlen((char *)&ExtensionStatus[num]);
//////					memcpy((char *)&type_data[t],(char *)&ExtensionStatus[num],i);
//////					t+=i;				
//////				}
//////			break;				
//			case 21://故障类型
////					memcpy(&SEND_BUFER[t],"故障类型",4);
////					t+=8;

//					memcpy(&type_data[t],"故障",4);
//					t+=4;
//			break;
//			default:
//			break;
//		}
////		type_data_lenth=t+1;
///******************提取信息*****************/
//		memset(message_data,0x20,40);//将100个0x20放进SEND_BUFER中。
//		if(breakdown_flag==1)//显示几号主机出现了故障，显示故障类型。
//		{
//			breakdown_flag=0;
//			if(sn==0)//页码。 
//			{//如果是主页面显示的3条信息
//				if(Host_NO<=9)
//				{
////				message_data[4]=0x31;//主机编号	。
//					message_data[6]=Host_NO+'0';//主机编号	。					
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
//			{//如果是信息查询界面显示的6条信息
//////				message_data[1]=0x31;//主机编号	。
////				message_data[3]=HostNO+'0';//主机编号	。	
//////				message_data[1]=host_number;//主机号码。				
////				message_data[4]='#';		

//				if(Host_NO<=9)
//				{
////				message_data[4]=0x31;//主机编号	。
//					message_data[3]=Host_NO+'0';//主机编号	。					
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
//		{//计算出信息里面有几个汉字。
//			num=data[2]>>3;
//			if(num<15)
//			{
//				data_lenth=strlen((char *)&FaultCode[num]);
//				data_lenth=data_lenth/2;//计算汉字的数量。比如2个数字，也是1个汉字的长度。一个汉字是由2个asc码组成的。
//			}
//			if(data_lenth==2)//判断，如果类型是2个汉字。
//			{
//				t=12;
//			}
//			else if(data_lenth==3)//判断，如果类型是3个汉字。
//			{
//				t=11;
//			}
//			else if(data_lenth==4)//判断，如果类型是4个汉字。
//			{
//				t=10;
//			}	
//			else if(data_lenth==7)//判断，如果类型是7个汉字。
//			{
//				t=7;
//			}
//			else if(data_lenth==9)//判断，如果类型是9个汉字。
//			{
//				t=5;
//			}
//		}
//		else
//		{
//			t=7;
//		}
//		if(sn==0)//页码。 
//		{
//			message_data_lenth=50;
//		}
//		else
//		{
//			message_data_lenth=40;
//			t=t-3;
//		}
//		if(data[0]==21)
//		{//用户信息传输装置本机信息。
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

//			if(data[11]==0x11)//是编码表信息，int型数据，显示用户编码表号码。
//			{
//				Loop_Part=data[8];
//				Loop_Part=Loop_Part<<8;
//				Loop_Part=Loop_Part+data[7];
//				Loop_Part=Loop_Part<<8;
//				Loop_Part=Loop_Part+data[6];
//				Loop_Part=Loop_Part<<8;
//				Loop_Part=Loop_Part+data[5];					
//				//   /取整
//				//   %取余
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
//			else//char型数据，显示正常的回路部件号码。
//			{			
//				message_data[t++]=LoopNO/100+0x30;
//				message_data[t++]=(LoopNO%100)/10+0x30;
//				message_data[t++]=LoopNO%10+0x30;
//				message_data[t++]=0xBB;//回路
//				message_data[t++]=0xD8;
//				message_data[t++]=0xC2;
//				message_data[t++]=0xB7;
//				message_data[t++]=PartNO/100+0x30;
//				message_data[t++]=(PartNO%100)/10+0x30;
//				message_data[t++]=PartNO%10+0x30;
//				message_data[t++]=0xBA;//号
//				message_data[t++]=0xC5;
//			}
//		}
////		message_data_lenth=t;
///******************************************/	
////注意！再给屏幕发送数据的时候，一定要发送整帧数据，没数据的地方发送空格。避免出现乱码。
//		switch(add)//判断显示地址。
//		{
////0x1090	0x10AA	0x10C4
////0x10E0	0x10FA	0x1114
////0x1130	0x114A	0x1164				
//			case 0x1090://首页的第一行信息
//			display_text(0x1090,time_data,time_data_lenth);//显示时间
//			display_text(0x10AA,type_data,type_data_lenth);//显示类型		
//			display_text(0x10C4,message_data,message_data_lenth);//显示信息
//			break;
//			case 0x10E0://首页的第二行信息
//			display_text(0x10E0,time_data,time_data_lenth);			
//			display_text(0x10FA,type_data,type_data_lenth);			
//			display_text(0x1114,message_data,message_data_lenth);				
//			break;
//			case 0x1130://首页的第三行信息
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
//			case 0x1240://信息页的第一行信息
//			display_text(0x1240,index_data,index_data_lenth);
//			display_text(0x1254,time_data,time_data_lenth);			
//			display_text(0x1268,type_data,type_data_lenth);
//			display_text(0x127C,message_data,message_data_lenth);				
//			break;				
//			case 0x1290://信息页的第二行信息
//			display_text(0x1290,index_data,index_data_lenth);
//			display_text(0x12A4,time_data,time_data_lenth);			
//			display_text(0x12B8,type_data,type_data_lenth);
//			display_text(0x12CC,message_data,message_data_lenth);				
//			break;			
//			case 0x12E0://信息页的第三行信息
//			display_text(0x12E0,index_data,index_data_lenth);
//			display_text(0x12F4,time_data,time_data_lenth);			
//			display_text(0x1308,type_data,type_data_lenth);
//			display_text(0x131C,message_data,message_data_lenth);				
//			break;				
//			case 0x1330://信息页的第四行信息
//			display_text(0x1330,index_data,index_data_lenth);
//			display_text(0x1344,time_data,time_data_lenth);			
//			display_text(0x1358,type_data,type_data_lenth);
//			display_text(0x136C,message_data,message_data_lenth);				
//			break;								
//			case 0x1390://信息页的第五行信息
//			display_text(0x1390,index_data,index_data_lenth);
//			display_text(0x13A4,time_data,time_data_lenth);			
//			display_text(0x13B8,type_data,type_data_lenth);
//			display_text(0x13CC,message_data,message_data_lenth);				
//			break;					
//			case 0x13E0://信息页的第六行信息
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
//void fun0(void)//首页显示三行报警信息。
//{
//	u16 add0=0x1001;//月
//	u16 add1=0x1003;//日
//	u16 add2=0x1005;//数据交互图标
//	u16 add3=0x1006;//联网方式
//	u16 add4=0x1015;//手动报警次数
//	u16 add5=0x101A;//自动报警次数
//	u16 add6=0x1020;//故障报警次数
//	u16 add7=0x1030;//手动报警
//	u16 add8=0x1040;//自动报警
//	u16 add9=0x1050;//故障报警
//	u16 add10=0x1060;//时间
//	u16 add11=0x1070;//类型
//	u16 add12=0x1080;//信息
//	u16 add13=0x1090;//第一条数据
//	u16 add14=0x10E0;//第二条数据
//	u16 add15=0x1130;//第三条数据
//	u16 add16=0x1182;//设置
//	u16 add17=0x1192;//信息
//	unsigned char SEND_TEXT[100];//定义串口2发送数组
//	unsigned char len=0;
//	unsigned char Host_NO=0;//主机编号	
//	change_back_picture(0x0001);//调取主页背景图片
//	input_text(add0,2,"月");//显示：月
//	input_text(add1,2,"日");//显示：日
//	input_text(add7,8,"手动报警");
//	input_text(add8,8,"自动报警");
//	input_text(add9,8,"故障报警");
//	input_text(add10,4,"时间");
//	input_text(add11,4,"类型");
//	input_text(add12,4,"信息");
//	input_text(add16,4,"设置");
//	input_text(add17,4,"信息");

////	LCDTime_Init();
////	LCD_SET_OFF();//常亮屏幕

//	//是否正在上传或者下载数据。数据交互。0的时候没有数据，1的时候有数据。
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
//	//0的时候是无信号，1的时候是有线连接，2的时候是无线连接。
//	len=strlen((char *)&SetNetMode[net_fixed]);
//	input_text(add3,len,(unsigned char *)&SetNetMode[net_fixed]);
//	if(fun0_Cursor==0)					//选中设置
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
//	else								//选中信息查询
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
//	memset(SEND_TEXT,0,0x20);//将buffer全部置空格。
//	if(Lcd_Flash_Cnt>0)
//	{
//		Readflash_Infor(SEND_TEXT, 1);//读取flash中存储的信息。
//	}
//	input_alarm(add13,0,SEND_TEXT,Host_NO);//主页显示第一条信息。表示地址为add13，第0页，数据内容为SEND_TEXT中的内容。
//	memset(SEND_TEXT,0,0x20);//将buffer全部置空格。
//	if(Lcd_Flash_Cnt>1)
//	{
//		Readflash_Infor(SEND_TEXT, 2);
//	}
//	input_alarm(add14,0,SEND_TEXT,Host_NO);//主页显示第二条信息
//	memset(SEND_TEXT,0,0x20);//将buffer全部置空格。
//	if(Lcd_Flash_Cnt>2)
//	{
//		Readflash_Infor(SEND_TEXT, 3);
//	}
//	input_alarm(add15,0,SEND_TEXT,Host_NO);//主页显示第三条信息
//	W25q16_UnLock();
//}
///*****************************************************************************
//* 函数名称: unsigned char  Sting_Compare(unsigned char  *Sting1,unsigned char  *Sting2,unsigned char Len)
//* 功能描述: 字符串比较
//* 参    数:Sting1：字符串1;Sting2：字符串2;Len:字符串2的长度
//* 返回  值:0：字符串不相同；1：字符串相同
//*****************************************************************************/
//unsigned char  Sting_Compare(unsigned char  *Sting1,unsigned char  *Sting2,unsigned char Len)//字符串比较
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

//void fun1(void)//显示六条信息。
//{
////	unsigned char time_data[6]={0};
//	unsigned char SEND_TEXT[100];
//	unsigned char Host_NO=0;//主机编号	
//	unsigned char Sting2[8]={0xB1,0xBE,0xBB,0xFA,0xB8,0xB4,0xCE,0xBB};//本机复位
//	unsigned RS_flag=0;
////	RTC_Get(time_data);
//	if(fun1_page_fixed<=1)
//	{
//		fun1_page_fixed=1;
//	}
//	change_back_picture(0x0003);	//切换底部图片到信息查询界面
//	
//	memset(SEND_TEXT,0x20,100);
//	input_text(0x1480,5,SEND_TEXT);
//	//首先全部清除光标选中。
//	input_small_picture(0x14A5,0x0007);//清除光标区域
//	input_small_picture(0x14A6,0x0007);
//	input_small_picture(0x14A7,0x0007);
//	input_small_picture(0x14A8,0x0007);
//	input_small_picture(0x14A9,0x0007);
//	input_small_picture(0x14AA,0x0009);
//	input_small_picture(0x14AB,0x0007);
//	if(fun1_Cursor!=4)fun1_page=fun1_page_fixed;
//	if(fun1_page>=100)//三位数
//	{
//		SEND_TEXT[0]=fun1_page/100+0x30;
//		SEND_TEXT[1]=(fun1_page%100)/10+0x30;
//		SEND_TEXT[2]=fun1_page%10+0x30;
//		SEND_TEXT[3]=0xD2;
//		SEND_TEXT[4]=0xB3;
//		input_text(0x1480,5,SEND_TEXT);//显示页码
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
//	//搜索“本机复位”，不再显示页下面内容；
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
//		//搜索“本机复位”，不再显示页下面内容；
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
//	//搜索“本机复位”，不再显示页下面内容；
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
//	//搜索“本机复位”，不再显示页下面内容；
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
//			//搜索“本机复位”，不再显示页下面内容；
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
//	//搜索“本机复位”，不再显示页下面内容；
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
//	unsigned char SEND_TEXT[40];	//定义串口2发送数组
//	unsigned int add0=0x14D0;		//输入密码的第0号地址
//	unsigned int add1=0x14D2;		//输入密码的第1号地址
//	unsigned int add2=0x14D4;		//输入密码的第2号地址
//	unsigned int add3=0x14D6;		//输入密码的第3号地址
//	unsigned int add4=0x14C0;		//密码输入提示区间
//	unsigned int add5=0x1901;		//消息提示区
//	
//	memset(SEND_TEXT,0x20,40);
//	change_back_picture(0x0004);	//切换底部图片到输入三级密码界面
//	input_text(add4,28,SEND_TEXT);	//显示：清空当前密码提示区位置文字。
//	input_text(add5,28,SEND_TEXT);	//显示：清空当前密码提示区位置文字。
//	
//	switch(password_three_ok)
//	{
//		case 0:
//			input_text(add4,10,"请输入密码");//显示：请输入密码
//		break;
//		case 2:
//			input_text(add4,14,"请重新输入密码");//密码错误
//		break;
//		default:			break;
//	}
//	//显示密码区
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
////	input_text(0x1870,24,SEND_TEXT);//显示：清空当前密码提示区位置文字。
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
//	input_text(0x1001,2,"月");
//	input_text(0x1003,2,"日");
//	switch(fun6_Cursor)
//	{
//		case 0:			//选中有线  		
//			change_back_picture(0x0006);
//		break;		
//		case 1:			//选中无线	
//			change_back_picture(0x0007);
//		break;
////		case 2:			//选中服务器
////			change_back_picture(0x0008);
//////			input_text(0x15B0,22,"域名:www.fubangyun.com");
//////			input_text(0x15E0,12,"端口号:10086");
////		break;
//		case 2:			//选中保存		
//			input_small_picture(0x1570,0x0001);//保存去除光标选中。
//			input_small_picture(0x1571,0x0000);//取消去除光标不选中。			
//			change_back_picture(0x0008);
//		break;	
//		case 3:			//选中退出
//			input_small_picture(0x1570,0x0000);//保存去除光标选中。
//			input_small_picture(0x1571,0x0001);//取消去除光标不选中。			
//			change_back_picture(0x0008);
//		break;	
//		default:	break;
//	}
//	switch(net_fixed)
//	{
//		case 1:					//选中有线	
//			input_small_picture(0x1630,0x000A);//小光标选中。
//			input_small_picture(0x1631,0x0000);//小光标选中。
//			input_small_picture(0x1633,0x0000);//小光标选中。		
//		break;
//		case 2:					//选中无线
//			input_small_picture(0x1630,0x0000);//小光标选中。		
//			input_small_picture(0x1631,0x000A);//小光标选中。
//			input_small_picture(0x1633,0x0000);//小光标选中。		
//		break;
////		case 3:					//选中服务器
////			input_small_picture(0x1630,0x0000);//小光标选中。		
////			input_small_picture(0x1631,0x0000);//小光标选中。		
////			input_small_picture(0x1633,0x000A);//小光标选中。		
////		break;		
//		default:	break;
//	}
//}
//void fun7(void){}
//void fun8(void){}
//void fun9(void)//主机通信设置。协议板卡信息。
//{
//	unsigned int i=0;	
//	unsigned char Config[256]={0};//读取flash中存储的用户信息传输装置信息。
//	unsigned char data_local[100]={0x20};
//	unsigned char test[20]={0};	
//	unsigned char printf_data[40]={0};		
//	unsigned int 	port=0;
////	unsigned char step=0;//光标步进位置。		
////	unsigned char step=0;//光标步进位置。
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

//	change_back_picture(0x0032);//调取用户信息传输装置基本信息背景图片。

////首先清空页面中所有文本框中的文字。
////通过SPI获取协议卡的信息。
//	
////主机接口显示信息：
//	
////板卡类型		
////显示协议类型（厂家型号）、
////版本号（协议解析程序版本号）
////接口类型方式（CAN/RS485、波特率、校验位、数据位、停止位）


////const unsigned char BKXH_msg[20]="XSBD-FB801Z-N";
////const unsigned char XFZJ_msg[20]="JB_QBL_bdqn11s";
////const unsigned char SOFT_msg[40]="FB801Z_BJBDQN11S_V1.0_20180131";
////const unsigned char ITFS_msg[20]="RS232 2400"

//////板卡型号；主机型号；软件版本；接口说明；
////const unsigned char BKXH_msg[20]="XSBD-FB801Z-N";
////const unsigned char XFZJ_msg[20]="JB_QBL_bdqn11s";
////const unsigned char SOFT_msg[40]="FB801Z_BJBDQN11S_V1.0_20180131";
////const unsigned char ITFS_msg[20]="RS232 2400";	

//////显示板卡型号（厂家型号）、
////	memset(data_local,0x20,100);//将100个0x20放进SEND_BUFER中。		
////	memcpy(data_local," 板卡类型： ",12);
////	for(i=0;i<20;i++)//清空buffer
////	{
////		data_local[12+i]=BK_Message[i];
////	}		
////	input_text(0x2B00,60,data_local);
//////显示主机型号（厂家型号）、
////	memcpy(data_local," 主机型号： ",12);
////	for(i=0;i<20;i++)//清空buffer
////	{
////		data_local[12+i]=BK_Message[i+20];
////	}			
////	input_text(0x2B50,60,data_local);
//////软件版本（协议解析软件版本）
////	memcpy(data_local," 版本号： ",10);
////	for(i=0;i<40;i++)//清空buffer
////	{
////		data_local[10+i]=BK_Message[i+40];
////	}			
////	input_text(0x2C00,60,data_local);
//////接口说明（CAN/RS485、波特率、校验位、数据位、停止位）	
////	memcpy(data_local," 接口说明： ",12);
////	for(i=0;i<20;i++)//清空buffer
////	{
////		data_local[12+i]=BK_Message[i+80];
////	}			
////	input_text(0x2C50,60,data_local);	
//	
//	
//	
//	
//	
////	input_small_picture(0x1631,0x0000);	//清除光标
////	input_small_picture(0x1632,0x0000);
////	//跳转到设置界面底片
////	switch(fun9_Cursor)
////	{
////		case 0:				//选中RS232/RS485
////			change_back_picture(0x000A);
////		break;
////		case 1:				//选中CAN
////			change_back_picture(0x000B);
////		break;
////		case 2:				//选中保存
////			change_back_picture(0x000C);
////			input_small_picture(0x1631,0x0001);
////			input_small_picture(0x1632,0x0000);
////		break;
////		case 3:				//选中退出
////			change_back_picture(0x000C);
////			input_small_picture(0x1631,0x0000);
////			input_small_picture(0x1632,0x0001);
////		break;		
////		default:		break;
////	}
////	switch(host_fixed)
////	{
////		case 1:				//选中485
////			input_small_picture(0x1640,0x000A);
////			input_small_picture(0x1641,0x0000);
////		break;
////		case 2:				//选中CAN
////			input_small_picture(0x1640,0x0000);
////			input_small_picture(0x1641,0x000A);
////		break;
////		default:		break;
////	}


////小光标选中。
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
////查看选中了哪个光标		
//		switch(fun9_Cursor)
//		{
//			case 0:		//光标选中0号板卡
//				input_small_picture(fun_add00,0x000A);
//			break;
//			case 1:		//光标选中1号板卡
//				input_small_picture(fun_add01,0x000A);
//			break;
//			case 2:		//光标选中2号板卡
//				input_small_picture(fun_add02,0x000A);
//			break;
//			case 3:		//光标选中3号板卡
//				input_small_picture(fun_add03,0x000A);
//			break;
//			case 4:		//光标选中4号板卡
//				input_small_picture(fun_add04,0x000A);
//			break;
//			case 5:		//光标选中5号板卡
//				input_small_picture(fun_add05,0x000A);
//			break;
//			case 6:		//光标选中6号板卡
//				input_small_picture(fun_add06,0x000A);
//			break;
//			case 7:		//光标选中7号板卡
//				input_small_picture(fun_add07,0x000A);
//			break;
//			case 8:		//光标选中8号板卡
//				input_small_picture(fun_add08,0x000A);
//			break;
//			case 9:		//光标选中9号板卡
//				input_small_picture(fun_add09,0x000A);
//			break;
//			case 10:		//光标选中10号板卡
//				input_small_picture(fun_add0A,0x000A);
//			break;
//			case 11:		//光标选中11号板卡
//				input_small_picture(fun_add0B,0x000A);
//			break;
//			case 12:		//光标选中12号板卡
//				input_small_picture(fun_add0C,0x000A);
//			break;
//			case 13:		//光标选中13号板卡
//				input_small_picture(fun_add0D,0x000A);
//			break;
//			case 14:		//光标选中14号板卡
//				input_small_picture(fun_add0E,0x000A);
//			break;
//			case 15:		//光标选中15号板卡
//				input_small_picture(fun_add0F,0x000A);
//			break;
//			case 16:		//光标选中注册板卡
//				input_small_picture(fun_add0G,0x000A);
//			break;
//			case 17:		//光标选中编码表导入
//				input_small_picture(fun_add0H,0x000A);
//			break;
//			case 18:		//光标选中退出
//				input_small_picture(fun_add0I,0x000A);
//			break;				
//			default:		
//				break;
//		}
//		if((register_flag&0x0001)!=0x0001)
//		{
//			input_text(fun_add0,10,"00号板卡 ");
//		}
//		else
//		{
//			input_text(fun_add0,16,"00号板卡已注册 ");			
//		}			
//		if((register_flag&0x0002)!=0x0002)
//		{
//			input_text(fun_add1,10,"01号板卡 ");			
//		}
//		else
//		{
//			input_text(fun_add1,16,"01号板卡已注册 ");			
//		}
//		if((register_flag&0x0004)!=0x0004)	
//		{
//			input_text(fun_add2,10,"02号板卡 ");
//		}
//		else
//		{
//			input_text(fun_add2,16,"02号板卡已注册 ");			
//		}
//		if((register_flag&0x0008)!=0x0008)
//		{
//			input_text(fun_add3,10,"03号板卡 ");
//		}
//		else
//		{
//			input_text(fun_add3,16,"03号板卡已注册 ");			
//		}					
//		if((register_flag&0x0010)!=0x0010)
//		{
//			input_text(fun_add4,10,"04号板卡 ");
//		}
//		else
//		{
//			input_text(fun_add4,16,"04号板卡已注册 ");			
//		}					
//		if((register_flag&0x0020)!=0x0020)	
//		{
//			input_text(fun_add5,10,"05号板卡 ");
//		}
//		else
//		{
//			input_text(fun_add5,16,"05号板卡已注册 ");			
//		}					
//		if((register_flag&0x0040)!=0x0040)
//		{
//			input_text(fun_add6,10,"06号板卡 ");
//		}
//		else
//		{
//			input_text(fun_add6,16,"06号板卡已注册 ");			
//		}					
//		if((register_flag&0x0080)!=0x0080)
//		{
//			input_text(fun_add7,10,"07号板卡 ");
//		}
//		else
//		{
//			input_text(fun_add7,16,"07号板卡已注册 ");			
//		}					
//		if((register_flag&0x0100)!=0x0100)	
//		{
//			input_text(fun_add8,10,"08号板卡 ");
//		}
//		else
//		{
//			input_text(fun_add8,16,"08号板卡已注册 ");			
//		}					
//		if((register_flag&0x0200)!=0x0200)
//		{
//			input_text(fun_add9,10,"09号板卡 ");
//		}
//		else
//		{
//			input_text(fun_add9,16,"09号板卡已注册 ");			
//		}					
//		if((register_flag&0x0400)!=0x0400)
//		{
//			input_text(fun_addA,10,"10号板卡 ");
//		}
//		else
//		{
//			input_text(fun_addA,16,"10号板卡已注册 ");			
//		}					
//		if((register_flag&0x0800)!=0x0800)	
//		{
//			input_text(fun_addB,10,"11号板卡 ");
//		}
//		else
//		{
//			input_text(fun_addB,16,"11号板卡已注册");			
//		}					
//		if((register_flag&0x1000)!=0x1000)	
//		{
//			input_text(fun_addC,10,"12号板卡 ");
//		}
//		else
//		{
//			input_text(fun_addC,16,"12号板卡已注册 ");			
//		}					
//		if((register_flag&0x2000)!=0x2000)
//		{
//			input_text(fun_addD,10,"13号板卡 ");
//		}
//		else
//		{
//			input_text(fun_addD,16,"13号板卡已注册 ");			
//		}
//		if((register_flag&0x4000)!=0x4000)
//		{
//			input_text(fun_addE,10,"14号板卡 ");
//		}
//		else
//		{
//			input_text(fun_addE,16,"14号板卡已注册 ");			
//		}					
//		if((register_flag&0x8000)!=0x8000)	
//		{
//			input_text(fun_addF,10,"15号板卡 ");
//		}
//		else
//		{
//			input_text(fun_addF,16,"15号板卡已注册 ");			
//		}		
////		if((register_flag&0x0001)!=0x0001)
////		{
////			input_text(fun_add0,16,"00号板卡未注册 ");
////		}
////		else
////		{
////			input_text(fun_add0,16,"00号板卡已注册 ");			
////		}			
////		if((register_flag&0x0002)!=0x0002)
////		{
////			input_text(fun_add1,16,"01号板卡未注册 ");			
////		}
////		else
////		{
////			input_text(fun_add1,16,"01号板卡已注册 ");			
////		}
////		if((register_flag&0x0004)!=0x0004)	
////		{
////			input_text(fun_add2,16,"02号板卡未注册 ");
////		}
////		else
////		{
////			input_text(fun_add2,16,"02号板卡已注册 ");			
////		}
////		if((register_flag&0x0008)!=0x0008)
////		{
////			input_text(fun_add3,16,"03号板卡未注册 ");
////		}
////		else
////		{
////			input_text(fun_add3,16,"03号板卡已注册 ");			
////		}					
////		if((register_flag&0x0010)!=0x0010)
////		{
////			input_text(fun_add4,16,"04号板卡未注册 ");
////		}
////		else
////		{
////			input_text(fun_add4,16,"04号板卡已注册 ");			
////		}					
////		if((register_flag&0x0020)!=0x0020)	
////		{
////			input_text(fun_add5,16,"05号板卡未注册 ");
////		}
////		else
////		{
////			input_text(fun_add5,16,"05号板卡已注册 ");			
////		}					
////		if((register_flag&0x0040)!=0x0040)
////		{
////			input_text(fun_add6,16,"06号板卡未注册 ");
////		}
////		else
////		{
////			input_text(fun_add6,16,"06号板卡已注册 ");			
////		}					
////		if((register_flag&0x0080)!=0x0080)
////		{
////			input_text(fun_add7,16,"07号板卡未注册 ");
////		}
////		else
////		{
////			input_text(fun_add7,16,"07号板卡已注册 ");			
////		}					
////		if((register_flag&0x0100)!=0x0100)	
////		{
////			input_text(fun_add8,16,"08号板卡未注册 ");
////		}
////		else
////		{
////			input_text(fun_add8,16,"08号板卡已注册 ");			
////		}					
////		if((register_flag&0x0200)!=0x0200)
////		{
////			input_text(fun_add9,16,"09号板卡未注册 ");
////		}
////		else
////		{
////			input_text(fun_add9,16,"09号板卡已注册 ");			
////		}					
////		if((register_flag&0x0400)!=0x0400)
////		{
////			input_text(fun_addA,16,"10号板卡未注册 ");
////		}
////		else
////		{
////			input_text(fun_addA,16,"10号板卡已注册 ");			
////		}					
////		if((register_flag&0x0800)!=0x0800)	
////		{
////			input_text(fun_addB,16,"11号板卡未注册 ");
////		}
////		else
////		{
////			input_text(fun_addB,16,"11号板卡已注册");			
////		}					
////		if((register_flag&0x1000)!=0x1000)	
////		{
////			input_text(fun_addC,16,"12号板卡未注册 ");
////		}
////		else
////		{
////			input_text(fun_addC,16,"12号板卡已注册 ");			
////		}					
////		if((register_flag&0x2000)!=0x2000)
////		{
////			input_text(fun_addD,16,"13号板卡未注册 ");
////		}
////		else
////		{
////			input_text(fun_addD,16,"13号板卡已注册 ");			
////		}
////		if((register_flag&0x4000)!=0x4000)
////		{
////			input_text(fun_addE,16,"14号板卡未注册 ");
////		}
////		else
////		{
////			input_text(fun_addE,16,"14号板卡已注册 ");			
////		}					
////		if((register_flag&0x8000)!=0x8000)	
////		{
////			input_text(fun_addF,16,"15号板卡未注册 ");
////		}
////		else
////		{
////			input_text(fun_addF,16,"15号板卡已注册 ");			
////		}
//}
//void fun10(void){}
//void fun11(void){}
//void fun12(void)//显示每个板卡内容信息。板卡型号；主机型号；软件版本；接口说明；
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
////	input_small_picture(0x1680,0x0000);//保存去除光标选中。
////	input_small_picture(0x1681,0x0000);//取消去除光标选中。
////	input_text(add1,16,"                ");			
////	input_text(add2,16,"                ");	
////	input_text(add3,16,"                ");		
////	switch(fun12_Cursor)
////	{
////		case 0:		//波特率设置
////			change_back_picture(0x000D);
////		break;
////		case 1:		//奇偶校验位
////			change_back_picture(0x000E);
////		break;
////		case 2:		//停止位
////			change_back_picture(0x000F);
////		break;
////		case 3:		//保存
////			change_back_picture(0x0010);
////			input_small_picture(0x1680,0x0001);//保存去除光标选中。
////			input_small_picture(0x1681,0x0000);//取消去除光标选中。
////		break;
////		case 4:		//退出
////			change_back_picture(0x0010);
////			input_small_picture(0x1680,0x0000);//保存去除光标选中。
////			input_small_picture(0x1681,0x0001);//取消去除光标选中。
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

//	change_back_picture(0x0033);//调取用户信息传输装置基本信息背景图片。
////	if( (BK_Message[fun9_Cursor][0]==0)&&(BK_Message[fun9_Cursor][1]==0)&&(BK_Message[fun9_Cursor][2]==0) )//如果发现板卡没有被注册。
////	{
////		switch(fun9_Cursor)
////		{
////			case 0:		//0号板卡
////				input_text(fun_add0,14,"00号板卡未注册  ");
////			break;
////			case 1:		//1号板卡
////				input_text(fun_add0,14,"01号板卡未注册  ");
////			break;
////			case 2:		//2号板卡
////				input_text(fun_add0,14,"02号板卡未注册  ");
////			break;
////			case 3:		//3号板卡
////				input_text(fun_add0,14,"03号板卡未注册  ");
////			break;
////			case 4:		//4号板卡
////				input_text(fun_add0,14,"04号板卡未注册  ");
////			break;
////			case 5:		//5号板卡
////				input_text(fun_add0,14,"05号板卡未注册  ");
////			break;
////			case 6:		//6号板卡
////				input_text(fun_add0,14,"06号板卡未注册  ");
////			break;
////			case 7:		//7号板卡
////				input_text(fun_add0,14,"07号板卡未注册  ");
////			break;
////			case 8:		//8号板卡
////				input_text(fun_add0,14,"08号板卡未注册  ");
////			break;
////			case 9:		//9号板卡
////				input_text(fun_add0,14,"09号板卡未注册  ");
////			break;
////			case 10:	//10号板卡
////				input_text(fun_add0,14,"10号板卡未注册  ");
////			break;
////			case 11:	//11号板卡
////				input_text(fun_add0,14,"11号板卡未注册  ");
////			break;
////			case 12:	//12号板卡
////				input_text(fun_add0,14,"12号板卡未注册  ");
////			break;
////			case 13:	//13号板卡
////				input_text(fun_add0,14,"13号板卡未注册  ");
////			break;
////			case 14:	//14号板卡
////				input_text(fun_add0,14,"14号板卡未注册  ");
////			break;
////			case 15:	//15号板卡
////				input_text(fun_add0,14,"15号板卡未注册  ");
////			break;		
////			default:		
////				break;
////		}				
////	}
////	else
////	{
//		switch(fun9_Cursor)
//		{
//			case 0:		//光标选中0号板卡
//				input_text(fun_add0,12,"00号板卡信息  ");		
//			break;
//			case 1:		//光标选中1号板卡
//				input_text(fun_add0,12,"01号板卡信息  ");
//			break;
//			case 2:		//光标选中2号板卡
//				input_text(fun_add0,12,"02号板卡信息  ");
//			break;
//			case 3:		//光标选中3号板卡
//				input_text(fun_add0,12,"03号板卡信息  ");
//			break;
//			case 4:		//光标选中4号板卡
//				input_text(fun_add0,12,"04号板卡信息  ");
//			break;
//			case 5:		//光标选中5号板卡
//				input_text(fun_add0,12,"05号板卡信息  ");
//			break;
//			case 6:		//光标选中6号板卡
//				input_text(fun_add0,12,"06号板卡信息  ");
//			break;
//			case 7:		//光标选中7号板卡
//				input_text(fun_add0,12,"07号板卡信息  ");
//			break;
//			case 8:		//光标选中8号板卡
//				input_text(fun_add0,12,"08号板卡信息  ");
//			break;
//			case 9:		//光标选中9号板卡
//				input_text(fun_add0,12,"09号板卡信息  ");
//			break;
//			case 10:		//光标选中10号板卡
//				input_text(fun_add0,12,"10号板卡信息  ");
//			break;
//			case 11:		//光标选中11号板卡
//				input_text(fun_add0,12,"11号板卡信息  ");
//			break;
//			case 12:		//光标选中12号板卡
//				input_text(fun_add0,12,"12号板卡信息  ");
//			break;
//			case 13:		//光标选中13号板卡
//				input_text(fun_add0,12,"13号板卡信息  ");
//			break;
//			case 14:		//光标选中14号板卡
//				input_text(fun_add0,12,"14号板卡信息  ");
//			break;
//			case 15:		//光标选中15号板卡
//				input_text(fun_add0,12,"15号板卡信息  ");
//			break;
//			default:
//				break;
//		}
////	}



////首先清空页面中所有文本框中的文字。
////通过SPI获取协议卡的信息。
//	
////主机接口显示信息：
//	
////板卡类型		
////显示协议类型（厂家型号）、
////版本号（协议解析程序版本号）
////接口类型方式（CAN/RS485、波特率、校验位、数据位、停止位）


////const unsigned char BKXH_msg[20]="XSBD-FB801Z-N";
////const unsigned char XFZJ_msg[20]="JB_QBL_bdqn11s";
////const unsigned char SOFT_msg[40]="FB801Z_BJBDQN11S_V1.0_20180131";
////const unsigned char ITFS_msg[20]="RS232 2400"

//////板卡型号；主机型号；软件版本；接口说明；
////const unsigned char BKXH_msg[20]="XSBD-FB801Z-N";
////const unsigned char XFZJ_msg[20]="JB_QBL_bdqn11s";
////const unsigned char SOFT_msg[40]="FB801Z_BJBDQN11S_V1.0_20180131";
////const unsigned char ITFS_msg[20]="RS232 2400";	


//////显示板卡型号（厂家型号）、
////	memset(data_local,0x20,100);//将100个0x20放进SEND_BUFER中。		
////	memcpy(data_local," 板卡类型： ",12);
////	for(i=0;i<20;i++)//清空buffer
////	{
////		data_local[12+i]=BK_Message[i];
////	}		
////	input_text(0x2E00,60,data_local);
//////显示主机型号（厂家型号）、
////	memcpy(data_local," 主机型号： ",12);
////	for(i=0;i<20;i++)//清空buffer
////	{
////		data_local[12+i]=BK_Message[i+20];
////	}			
////	input_text(0x2E50,60,data_local);
//////软件版本（协议解析软件版本）
////	memcpy(data_local," 版本号： ",10);
////	for(i=0;i<40;i++)//清空buffer
////	{
////		data_local[10+i]=BK_Message[i+40];
////	}
////	input_text(0x2F00,60,data_local);
//////接口说明（CAN/RS485、波特率、校验位、数据位、停止位）	
////	memcpy(data_local," 接口说明： ",12);
////	for(i=0;i<20;i++)//清空buffer
////	{
////		data_local[12+i]=BK_Message[i+80];
////	}
////	input_text(0x2F50,60,data_local);	
//	
////..\HARDWARE\APPLCD\lcd.c(1980): error:  #142: expression must have pointer-to-object type data_local[12+i]=BK_Message[fun9_Cursor][i];
////显示板卡型号（厂家型号）、
//	memset(data_local,0x20,100);//将100个0x20放进SEND_BUFER中。		
//	memcpy(data_local,"板卡类型：",10);
//	for(i=0;i<20;i++)//清空buffer
//	{
//		data_local[10+i]=BK_Message[fun9_Cursor][i];
//	}
//	input_text(fun_add1,60,data_local);
////显示主机型号（厂家型号）、
//	memset(data_local,0x20,100);//将100个0x20放进SEND_BUFER中。	
//	memcpy(data_local,"主机型号：",10);
//	for(i=0;i<20;i++)//清空buffer
//	{
//		data_local[10+i]=BK_Message[fun9_Cursor][i+20];
//	}			
//	input_text(fun_add2,60,data_local);
////软件版本（协议解析软件版本）
//	memset(data_local,0x20,100);//将100个0x20放进SEND_BUFER中。
//	memcpy(data_local,"版本号：",8);
//	for(i=0;i<40;i++)//清空buffer
//	{
//		data_local[8+i]=BK_Message[fun9_Cursor][i+40];
//	}
//	input_text(fun_add3,60,data_local);
////接口说明（CAN/RS485、波特率、校验位、数据位、停止位）	
//	memset(data_local,0x20,100);//将100个0x20放进SEND_BUFER中。
//	memcpy(data_local,"接口说明：",10);
//	for(i=0;i<20;i++)//清空buffer
//	{
//		data_local[10+i]=BK_Message[fun9_Cursor][i+80];
//	}
//	input_text(fun_add4,60,data_local);		
//}
//void fun13(void){}
//void fun14(void){}
//void fun15(void)//注册板卡和编码表导入界面
//{
//	unsigned char data_local[100]={0x20};
//	unsigned int schedule=0;//进度
//	unsigned int i=0;
//	u16 add1=0x1701;
//	unsigned char len=0;
//	OS_ERR err;
////	LCDTime_Init();
////	OSTimeDlyHMSM(0,0,20,10,OS_OPT_TIME_HMSM_STRICT,&err); //延时10ms
////	LCD_SET_OFF();//常亮屏幕
//	change_back_picture(0x0034);//调取用户信息传输装置导入编码表背景图片。
//	//不允许息屏，除了按取消键，其他键不使能。
////	SDDataInFlagt=1;
////	SD_DATAIN(); //执行编码表导入函数。
////	i=400;
//	cancel_flag=0;
//	if(register_card_flag==1)//如果是注册板卡界面。
//	{
//		Registered_Start=1;
//		register_card_flag=0;//清空标志位。

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
//		memcpy(data_local,"板卡注册",8);
//		input_text(0x2F70,40,data_local);		
//		
//		memset(data_local,0x20,100);//
//		memcpy(data_local,"板卡正在注册中，请稍候。。。",28);	
//		input_text(0x2F80,30,data_local);		

//		for(i=60;i>0;i--)//1秒钟刷新一次进度。不能大于且等于。不然会出现负数，然后显示乱码。
//		{						
//         LCD_SET_ON();//常亮屏幕
//			  back_light_change(Backlight_brightness);//调节屏幕亮度	
//				OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s								
//			  if( (i<2)&&(RegisteProComplete==0) )//如果时间变成0了。说明注册板卡失败。则退出界面。
//				{
//					memset(data_local,0x20,100);//
//					memcpy(data_local,"注册板卡失败",12);
//					input_text(0x2FE0,13,data_local);				
//					OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时3s	
//					func_index=3;		
//				}
//				else
//				{
//					if(RegisteProComplete==1)//如果注册完毕，则跳出。并允许关闭屏幕。
//					{
//						i=1;						
//						memset(data_local,0x20,100);//
//						memcpy(data_local,"注册板卡完成",12);
//						input_text(0x2FE0,13,data_local);							
// 						OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时2s						
//						func_index=3;				
//						RegisteProComplete=0;//清空标志位	
//						refresh_lcd();
//					}				
//				}			
//		}
////		LCD_SET_OFF();//常亮屏幕
////    refresh_lcd();		
//	}
//	else//如果是用户编码表导入界面。则进行用户编码表的导入进度显示。
//	{ 
//		SDDataInFlagt=1;
//		memset(data_local,0x20,100);//
//		memcpy(data_local,"用户编码表导入",14);
//		input_text(0x2F70,16,data_local);		
//		for(i=10;i>0;i--)//1秒钟刷新一次进度。不能大于且等于。不然会出现负数，然后显示乱码。
//		{
//			LCD_SET_ON();//常亮屏幕
//			schedule=FBWBReadStatus;
//			memset(data_local,0x20,100);//
//			memcpy(data_local,"导入进度：",10);
//			data_local[11]=schedule/100+0x30;// 取进度的百位数部分
//			data_local[12]=(schedule%100)/10+0x30;// 取进度的十位数部分
//			data_local[13]=((schedule%100)%10)%10+0x30;// 取进度的个位数部分
//			data_local[14]='%';
//			input_text(0x2F80,30,data_local);
//			memset(data_local,0x20,100);//
//			memcpy(data_local,"当前读取的条数：",16);
//			data_local[17]=FBWBReadNbr/10000+0x30;// 取进度的万位数部分
//			data_local[18]=(FBWBReadNbr%10000)/1000+0x30;// 取进度的千位数部分
//			data_local[19]=((FBWBReadNbr%10000)%1000)/100+0x30;// 取进度的百位数部分
//			data_local[20]=(((FBWBReadNbr%10000)%1000)%100)/10+0x30;// 取进度的十位数部分
//			data_local[21]=(((FBWBReadNbr%10000)%1000)%100)%10+0x30;// 取进度的个位数部分
//			input_text(0x2FA0,30,data_local);

//			memset(data_local,0x20,100);//
//			memcpy(data_local,"当前读取失败的条数：",20);
//			data_local[21]=FBWBReadFail/10000+0x30;// 取进度的万位数部分		
//			data_local[22]=(FBWBReadFail%10000)/1000+0x30;// 取进度的千位数部分		
//			data_local[23]=((FBWBReadFail%10000)%1000)/100+0x30;// 取进度的百位数部分			
//			data_local[24]=(((FBWBReadFail%10000)%1000)%100)/10+0x30;// 取进度的十位数部分				
//			data_local[25]=(((FBWBReadFail%10000)%1000)%100)%10+0x30;// 取进度的个位数部分			
//			input_text(0x2FC0,30,data_local);		
//	//if(cancel_flag==1)//导入过程中，退出。
//	//{
//	//	cancel_flag=0;
//	//	func_index=3;				
//	//	refresh_lcd();	
//	//}	
//			if(FBWBReadStatus>=100)
//			{
//				i=0;//跳出循环
//				memset(data_local,0x20,100);//		
//				memcpy(data_local,"编码表导入结束，4秒后重启。",27);
//				input_text(0x2FE0,27,data_local);		
//				OSTimeDlyHMSM(0,0,4,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时4s	
//			  SoftReset();//软件重启；
//				func_index=12;
//				break;
//			}
//			else
//			{
//				memset(data_local,0x20,100);//				
//				input_text(0x2FE0,30,data_local);			
//				if(i<2)//如果时间变成0了。说明SD卡损坏或者没有插卡。则退出界面。
//				{
//					memset(data_local,0x20,100);//
//					memcpy(data_local,"未插入SD卡或SD卡已损坏",22);
//					input_text(0x2FE0,23,data_local);				
//					OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s	
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
//						memcpy(data_local,"倒计时间：",10);
//						data_local[11]=i/100+0x30;// 取进度的百位数部分
//						data_local[12]=(i%100)/10+0x30;// 取进度的十位数部分
//						data_local[13]=((i%100)%10)%10+0x30;// 取进度的个位数部分
//						data_local[14]=0xC3;//秒
//						data_local[15]=0xEB;//
//						input_text(0x2FE0,16,data_local);						
//					}
//				}
//			}
//			OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s					
//		}		
//	}
//						

////		LCD_SET_ON();//关闭屏幕
//}
//void fun16(void){}
//void fun17(void){}
//void fun18(void)//背光调节。
//{
//	u16 add1=0x1733;
//	u16 add2=0x1734;
//	input_small_picture(add1,0x0000);
//	input_small_picture(add2,0x0000);
//	switch(fun18_Cursor)//
//	{
//		case 0:					//设置背光亮度
//			back_light_change(Backlight_brightness);//调节亮度
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
//		case 1:					//设置背光时间
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
//		case 2:					//保存
//			change_back_picture(0x0020);//背光亮度和时间调节都没有选中
//			input_small_picture(add1,0x0001);//保存去除光标选中。
//			input_small_picture(add2,0x0000);//取消去除光标不选中。		
//		break;	
//		case 3:					//退出
//			change_back_picture(0x0020);//背光亮度和时间调节都没有选中
//			input_small_picture(add1,0x0000);//保存去除光标不选中。
//			input_small_picture(add2,0x0001);//取消去除光标选中。		
//		break;		
//		default:
//		break;
//	}
//}
//void fun19(void){}
//void fun20(void){}
//void fun21(void)
//{
//	u8 SEND_TEXT[40];//定义串口2发送数组
//	u16 add0=0x1820;//输入密码的第0号地址
//	u16 add1=0x1822;//输入密码的第1号地址
//	u16 add2=0x1824;//输入密码的第2号地址
//	u16 add3=0x1826;//输入密码的第3号地址
//	u16 add4=0x1801;//输入提示字符的位置
//	change_back_picture(0x0021);//输入密码图片	

//	memset(SEND_TEXT,0x20,40);
//	input_text(add4,28,SEND_TEXT);//显示：清空当前密码提示区位置文字。
//	//显示当前输入的密码值
//	if(input_password_times==0)
//	{	//显示输入的第一次密码。
//		input_text(add4,10,"请输入密码");
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
//	{	//显示输入的第二次密码。
//		input_text(add4,14,"请再次输入密码");
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
//void fun22(void)//声音设置。显示用户信息传输装置基本信息。
//{ //本函数 需进入临界区保护20180503
//	unsigned int i=0;	
//	unsigned int add=0x50000;
////	unsigned char data_local[250]={0};	
//	unsigned char message_data[256]={0};//读取flash中存储的用户信息传输装置信息。
//	unsigned char data_local[100]={0x2D};
//	unsigned char test[30]={0};
//	unsigned char data[40]={0};	
//	unsigned char printf_data[40]={0};		
//	unsigned int 	port=0;
////	CPU_SR_ALLOC();
////	OS_CRITICAL_ENTER();//进入临界区
//	
////	unsigned char data_local[50]={"服务器地址："};

////	unsigned char data_local[10][100]={"程序版本号：","本机地址：","服务器地址：","服务器端口号：","超级网口K2静态IP：",	
////																		"程序版本号：","本机地址：","服务器地址：","服务器地址：","超级网口K2静态IP："};																		

////2.设置新的界面显示本机信息在屏幕上，包含以下内容：
////本机地址（从FLASH中读取）、
////服务器地址（从FLASH中读取）、
////K2信息（动态静态设置、本机IP、目标地址端口号、连接状态，从K2中读取）、
////GM3信息（IMEI、SN、目标地址端口号、连接状态、无线信号强度，从GM3中读取）、
////主板程序版本号（从FLASH中读取）	
//	change_back_picture(0x0031);//调取用户信息传输装置基本信息。	
//	//首先清空页面中所有文本框中的文字。

////程序版本号：
////本机地址：
////服务器地址：
////服务器端口号：
////超级网口K2静态IP：
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
////	其中：16~45 是服务器地址,共计30个。

//	if(net_fixed==2)//采用的是无线连接方式
//	{
//		for(i=0;i<100;i++)//清空buffer
//		{
//			data_local[i]=0x20;
//		}
//		memcpy(data_local,"查询中，请稍候。",16);
//		input_text(0x2520,20,data_local);		
//	}
//	else
//	{
//		for(i=0;i<100;i++)//清空buffer
//		{
//			data_local[i]=0x20;
//		}		
//		memcpy(data_local,"有线连接",8);
//		input_text(0x2520,20,data_local);		
//	}

//	for(i=0;i<100;i++)//清空buffer
//	{
//		data_local[i]=0x20;
//	}	
//	memcpy(data_local,"服务器地址：",12);
//	for(i=0;i<message_data[0x2D];i++)
//	{
//		data_local[13+i]=message_data[16+i];
//	}
//	input_text(0x2000,43,data_local);
//	for(i=0;i<100;i++)//清空buffer
//	{
//		data_local[i]=0x20;
//	}	
////	其中：46~47 是端口号。共计2个。
//	memcpy(data_local,"服务器端口号：",14);
//	port=message_data[46];//计算服务器端口号
//	port=port<<8;
//	port=port+message_data[47];	
//	test[0]=(port/10000)+0x30;// 取整数部分
//	port=port%10000;	 // 取余数
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
//	for(i=0;i<100;i++)//清空buffer
//	{
//		data_local[i]=0x20;
//	}		
////	其中：48是服务器接口方式，为1时-无线；为0时-有线；默认无线；。
////	switch(Config[48])
//	switch(net_fixed)		
//	{
//		case 1:
//			memcpy(data_local,"服务器接口方式： 有线连接",26);
//			input_text(0x2100,26,data_local);			
//		break;
//		case 2:
//			memcpy(data_local,"服务器接口方式： 无线连接",26);
//			input_text(0x2100,26,data_local);
//		break;
//		case 3:
//			memcpy(data_local,"服务器接口方式： 自由切换",26);
//			input_text(0x2100,26,data_local);
//		break;	
//		default:			
//			memcpy(data_local,"服务器接口方式： 自由切换",26);
//			input_text(0x2100,26,data_local);	
//		break;
//	}
////	if(Config[48]==0)//有线连接方式
////	{
////		memcpy(data_local,"服务器接口方式： 有线连接",26);
////		input_text(0x2100,26,data_local);			
////	}
////	else if(Config[48]==1)//无线连接方式
////	{
////		memcpy(data_local,"服务器接口方式： 无线连接",26);
////		input_text(0x2100,26,data_local);
////	}
////	else
////	{
////		memcpy(data_local,"服务器接口方式： 自由切换",26);
////		input_text(0x2100,26,data_local);
////	}	
//	for(i=0;i<100;i++)//清空buffer
//	{
//		data_local[i]=0x20;
//	}
////0x31：有线ip配置方式，为1时DHCP，为0时静态ip；默认DHCP；
//	if(message_data[49]==0)//静态ip连接方式
//	{
//		memcpy(data_local,"有线IP配置方式： 静态IP",24);
//		input_text(0x2150,24,data_local);			
//	}
//	else//DHCP连接方式
//	{
//		memcpy(data_local,"有线IP配置方式： DHCP",22);
//		input_text(0x2150,22,data_local);			
//	}
//	for(i=0;i<100;i++)//清空buffer
//	{
//		data_local[i]=0x20;
//	}
////0x40-0x4f：ip，子网掩码，网关，dns；	
////0x50	GM3的SN码.15位
//	memcpy(data_local,"GM3无线模块SN码：",17);
//	for(i=0;i<15;i++)
//	{
//		data_local[18+i]=message_data[80+i];
////		data_local[18+i]=Config[80+i]+0x30;		
//	}
//	input_text(0x2200,34,data_local);	
//	for(i=0;i<100;i++)//清空buffer
//	{
//		data_local[i]=0x20;
//	}		
////0x60	GM3的IMEI码.15位
//	memcpy(data_local,"GM3无线模块IMEI码：",19);
//	for(i=0;i<15;i++)
//	{
//		data_local[20+i]=message_data[96+i];
////		data_local[20+i]=Config[96+i]+0x30;		
//	}
//	input_text(0x2250,36,data_local);	
//	for(i=0;i<100;i++)//清空buffer
//	{
//		data_local[i]=0x20;
//	}		
//////0x70	GM3的PHONE码.13位
////	memcpy(data_local,"GM3无线模块PHONE码：",20);
////	for(i=0;i<13;i++)
////	{
////		data_local[21+i]=Config[112+i];
//////		data_local[21+i]=Config[112+i]+0x30;		
////	}
////	input_text(0x2300,35,data_local);	
////0xf0-0xf5：本机地址，；

//	memcpy(data_local,"传输装置本机地址：",18);
////	for(i=0;i<6;i++)
////	{
////		data_local[27+i]=Config[240+i];
////	}		
////handle_arrays_u8(test,6,printf_data)		
////	input_text(0x2300,33,data_local);			
////			for(i=0;i<12;i++)
////		{//将本机ID号更新到25Q16中。
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
//	for(i=0;i<100;i++)//清空buffer
//	{
//		data_local[i]=0x20;
//	}
////0xb4-0xb7：二级密码，默认为0119；
//	memcpy(data_local,"二级密码：",10);
//	for(i=0;i<4;i++)
//	{
//		data_local[11+i]=message_data[0xB4+i];
//	}
//	input_text(0x2350,15,data_local);	

////	for(i=0;i<4;i++)
////	{
////		data[i]=Config[180+i];
////	}
////	handle_arrays_u8(data,4,printf_data);//处理数组，DATA是要处理的数组，SIZE是数组的长度。//比如将0x25分开就是0x32和0x35.。0xAB分开就是0x40,0x41. 
////	for(i=0;i<8;i++)
////	{
////		data_local[11+i]=printf_data[i];
////	}
////	input_text(0x2350,20,data_local);

//	for(i=0;i<100;i++)//清空buffer
//	{
//		data_local[i]=0x20;
//	}
////0xc0-0xc1：硬件版本，低字节为主版本，高字节为次版本，默认1.0；
//	memcpy(data_local,"硬件版本：",10);
//	data_local[11]=message_data[0xC0];	
//	data_local[13]=0x2E;//小数点	
//	data_local[15]=message_data[0xC1];	
//	input_text(0x2400,16,data_local);			

//	for(i=0;i<100;i++)//清空buffer
//	{
//		data_local[i]=0x20;
//	}
////0xD0	K2的MAC地址.6位
//	memcpy(data_local,"K2的MAC地址：",16);
//	input_text(0x2500,16,data_local);		
//	for(i=0;i<100;i++)//清空buffer
//	{
//		data_local[i]=0x20;
//	}
//	for(i=0;i<12;i++)
//	{
//		data_local[i]=message_data[0xD0+i];
//	}	
//	input_text(0x2510,12,data_local);		

//	for(i=0;i<100;i++)//清空buffer
//	{
//		data_local[i]=0x20;
//	}		

//////0xD0	K2的MAC地址.6位
////	for(i=0;i<100;i++)//清空buffer
////	{
////		data_local[i]=0x20;
////	}			
////	memcpy(data_local,"当前GM3信号强度：25",25);
////	input_text(0x2550,25,data_local);				
//	
////	for(i=0;i<100;i++)//清空buffer
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
////	handle_arrays_u8(data,2,printf_data);//处理数组，DATA是要处理的数组，SIZE是数组的长度。//比如将0x25分开就是0x32和0x35.。0xAB分开就是0x40,0x41. 
////	for(i=0;i<4;i++)
////	{
////		data_local[11+i]=printf_data[i];
////	}
////	input_text(0x2400,15,data_local);	
//	for(i=0;i<100;i++)//清空buffer
//	{
//		data_local[i]=0x20;
//	}
////0xc8-0xc9：软件版本，低字节为主版本，高字节为次版本，默认1.0；
//	memcpy(data_local,"软件版本：",10);
//	data_local[11]=message_data[0xC8];
//	data_local[13]=0x2E;//小数点
//	data_local[15]=message_data[0xC9];	
//	input_text(0x2450,16,data_local);		

//	if(net_fixed==2)//采用的是无线连接方式
//	{
//		sendata_flag=1;
//		gm3_send_cmd("+++","a",800);//握手 	
//	//	printf("%s\n",usart2_receive_data);
//	//	USART1_Send_Data(usart2_receive_data,20);	
//		sendata_flag=1;
//		gm3_send_cmd("a","+ok",800);//
//	//	USART1_Send_Data(usart2_receive_data,20);
//	//	printf("%s\n",usart2_receive_data);
//		gm3_send_cmd("AT+CSQ","",800);//读取GM3的SN号码
////	USART1_Send_Data(usart2_receive_data,20);
//		
////		printf("%s\n",usart2_receive_data);
//		for(i=0;i<100;i++)//清空buffer
//		{
//			data_local[i]=0x20;
//		}//7,8
////		data_local[0]=usart2_receive_data[6];//显示信号强度
////		data_local[1]=usart2_receive_data[7];
////		data_local[2]=usart2_receive_data[8];
////		data_local[3]=usart2_receive_data[9];
//		if(usart2_receive_data[9]==0x2C)//如果遇到了逗号，说明信号强度为0~9。则显示00~09
//		{
//			data_local[0]=0x30;
//			data_local[1]=usart2_receive_data[8];
//		}
//		else
//		{
//			data_local[0]=usart2_receive_data[8];//显示信号强度
//			data_local[1]=usart2_receive_data[9];			
//		}
//		//20180514超出范围值，归零。
//		if((data_local[0]+data_local[1])>0x6C)//如果超出范围，则让信号强度为 00 。
//		{
//			data_local[0]=0x30;
//			data_local[1]=0x30;
//		}
//		
//		input_text(0x2520,20,data_local);

//		gm3_send_cmd("AT+ENTM","",800);//恢复透传
//	//	USART1_Send_Data(usart2_receive_data,20);
//	//	printf("%s\n",usart2_receive_data);//测试				
//	}
//	else
//	{
////		for(i=0;i<100;i++)//清空buffer
////		{
////			data_local[i]=0x20;
////		}		
////		memcpy(data_local,"有线连接",8);
////		input_text(0x2520,20,data_local);		
//	}




////1.2 AT+CSQ
////命令解释：检查网络信号强度
////命令格式：AT+CSQ
////命令返回：+CSQ: **,##
////其中**应在 10 到 31 之间，数值越大表明信号质量越好，##为误码
////率,值在 0 到 99 之间。
////否则应检查天线或 SIM 卡是否正确安装
////测试结果：
////AT+CSQ
////+CSQ: 20,5













////	for(i=0;i<2;i++)
////	{
////		data[i]=Config[200+i];
////	}	
////	handle_arrays_u8(data,2,printf_data);//处理数组，DATA是要处理的数组，SIZE是数组的长度。//比如将0x25分开就是0x32和0x35.。0xAB分开就是0x40,0x41. 
////	for(i=0;i<4;i++)
////	{
////		data_local[11+i]=printf_data[i];
////	}
////	input_text(0x2450,15,data_local);		
////	for(i=0;i<100;i++)//清空buffer
////	{
////		data_local[i]=0x20;
////	}
//////0xf0-0xf5：本机地址，；
////	memcpy(data_local,"用户信息传输装置本机地址：",26);
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
////	handle_arrays_u8(data,6,printf_data);//处理数组，DATA是要处理的数组，SIZE是数组的长度。//比如将0x25分开就是0x32和0x35.。0xAB分开就是0x40,0x41. 
////	for(i=0;i<12;i++)
////	{
////		data_local[27+i]=printf_data[i];
////	}
////	input_text(0x2500,40,data_local);	
////	for(i=0;i<100;i++)//清空buffer
////	{
////		data_local[i]=0x20;
////	}

////	u16 add0=0x1834;
////	u16 add1=0x1835;
////	u16 add2=0x184E;
////	u16 add3=0x184F;
////	input_small_picture(add0,0x0000);//保存去除光标选中。
////	input_small_picture(add1,0x0000);//取消去除光标选中。
////	if(fun22_Cursor<2)
////	{
////		change_back_picture(0x0022+fun22_Cursor);
////	}
////	else if(fun22_Cursor==2)
////	{
////		input_small_picture(add0,0x0001);//保存去除光标选中。
////		input_small_picture(add1,0x0000);//取消去除光标不选中。
////		change_back_picture(0x0024);
////	}else if(fun22_Cursor==3)
////	{
////		input_small_picture(add0,0x0000);//保存去除光标选中。
////		input_small_picture(add1,0x0001);//取消去除光标不选中。
////		change_back_picture(0x0024);
////	}
////	
////	if(sound_fixed==0)
////	{
////		input_small_picture(add2,0x000A);//小光标选中。
////		input_small_picture(add3,0x0000);//小光标选中。
////	}else if(sound_fixed==1)
////	{
////		input_small_picture(add2,0x0000);//小光标选中。
////		input_small_picture(add3,0x000A);//小光标选中。
////	}
////	OS_CRITICAL_EXIT();	//退出临界区	 
//}
//void fun23(void){}
//void fun24(void){}
//void fun25(void)//时间设置
//{
//	unsigned char time_data[6]={0};
//	u8 SEND_TEXT[40];//定义串口2发送数组
//	unsigned char buf[0x06];
//	u16 add0=0x1860;//输入 年份 地址
//	u16 add1=0x1870;//输入 月份 地址
//	u16 add2=0x1880;//输入 日期 地址
//	u16 add3=0x1890;//输入 小时 地址
//	u16 add4=0x18A0;//输入 分钟 地址
//	u16 add5=0x18B0;//输入 秒   地址
//	u16 add6=0x1858;//保存
//	u16 add7=0x1859;//退出
//	u16 add8=0x19E0;// 自动校时/手动校时 字体位置	
//	u16 add9=0x19F0;// 自动校时/手动校时 光标地址 		
//	input_small_picture(add6,0x0000);//保存去除光标不选中。
//	input_small_picture(add7,0x0000);//取消去除光标不选中。
//	input_small_picture(add9,0x0000);//自动校时/手动校时 取消选中
//	
////	RTC_Get(time_data);//获取时间。保证进入时间设置的时候，能够正确的显示当前的时间。
////	sec=time_data[0];
////	minute=time_data[1];
////	hours=time_data[2];
////	date=time_data[3];
////	month=time_data[4];
////	year=time_data[5];	
//	
//	if(correction_time_way==1)
//	{
//		input_text(add8,8,"自动校时");
//	}
//	else
//	{
//		input_text(add8,8,"手动校时");
//	}	
//	
//	if(fun25_Cursor<6)//切换背景图片。0~6共7张。
//	{
//		input_small_picture(add9,0x0000);//自动校时/手动校时 取消选中		
//		change_back_picture(0x0025+fun25_Cursor);
//	}else if(fun25_Cursor==6)
//	{
//		change_back_picture(0x002B);
//		input_small_picture(add9,0x0000);//自动校时/手动校时 取消选中
//		input_small_picture(add6,0x0001);//保存去除光标选中。
//		input_small_picture(add7,0x0000);//取消去除光标不选中。
//	}else if(fun25_Cursor==7)
//	{
//		change_back_picture(0x002B);
//		input_small_picture(add9,0x0000);//自动校时/手动校时 取消选中
//		input_small_picture(add6,0x0000);//保存去除光标不选中。
//		input_small_picture(add7,0x0001);//取消去除光标选中。
//		RTC_Get((char *)buf);		
//		sec=buf[0];
//		minute=buf[1];
//		hours=buf[2];
//		date=buf[3];
//		month=buf[4];
//		year=buf[5];
//		
////		printf("年月日\r\n");
//		
//	}else if(fun25_Cursor==8)//选中 自动校时/手动校时 。
//	{
//		change_back_picture(0x002B);		
//		input_small_picture(add9,0x0001);//自动校时/手动校时 选中
//		input_small_picture(add6,0x0000);//保存去除光标选中。
//		input_small_picture(add7,0x0000);//取消去除光标不选中。		
//	}

////	SEND_TEXT[0]=0x32;//年
////	SEND_TEXT[1]=0x30;		
////	SEND_TEXT[2]=year/16+0x30;
////	SEND_TEXT[3]=year%16+0x30;
////	input_text(add0,4,SEND_TEXT);

////	SEND_TEXT[0]=month/16+0x30;//月
////	SEND_TEXT[1]=month%16+0x30;
////	input_text(add1,2,SEND_TEXT);
////	
////	SEND_TEXT[0]=date/16+0x30;//日
////	SEND_TEXT[1]=date%16+0x30;
////	input_text(add2,2,SEND_TEXT);
////	
////	SEND_TEXT[0]=hours/16+0x30;//时
////	SEND_TEXT[1]=hours%16+0x30;
////	input_text(add3,2,SEND_TEXT);
////	
////	SEND_TEXT[0]=minute/16+0x30;//分
////	SEND_TEXT[1]=minute%16+0x30;
////	input_text(add4,2,SEND_TEXT);
////	
////	SEND_TEXT[0]=sec/16+0x30;//秒
////	SEND_TEXT[1]=sec%16+0x30;
////	input_text(add5,2,SEND_TEXT);
//	
//	SEND_TEXT[0]=0x32;//年
//	SEND_TEXT[1]=0x30;		
//	SEND_TEXT[2]=year/10+0x30;
//	SEND_TEXT[3]=year%10+0x30;
//	input_text(add0,4,SEND_TEXT);

//	SEND_TEXT[0]=month/10+0x30;//月
//	SEND_TEXT[1]=month%10+0x30;
//	input_text(add1,2,SEND_TEXT);
//	
//	SEND_TEXT[0]=date/10+0x30;//日
//	SEND_TEXT[1]=date%10+0x30;
//	input_text(add2,2,SEND_TEXT);
//	
//	SEND_TEXT[0]=hours/10+0x30;//时
//	SEND_TEXT[1]=hours%10+0x30;
//	input_text(add3,2,SEND_TEXT);
//	
//	SEND_TEXT[0]=minute/10+0x30;//分
//	SEND_TEXT[1]=minute%10+0x30;
//	input_text(add4,2,SEND_TEXT);
//	
//	SEND_TEXT[0]=sec/10+0x30;//秒
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
//	u16 add4=0x14C0;//密码输入提示区间
//	u16 add5=0x1901;//消息提示区
//	change_back_picture(0x0004);//切换底部图片到输入三级密码界面
//	memset(SEND_TEXT,0x20,28);
//	input_text(add4,28,SEND_TEXT);//显示：清空当前密码提示区位置文字。
//	input_text(add5,28,SEND_TEXT);//显示：清空当前密码提示区位置文字。
//	
//	switch(password_three_ok)
//	{
//		case 0:
//			input_text(add4,10,"请输入密码");//显示：请输入密码
//		break;
//		case 2:
//			input_text(add4,14,"请重新输入密码");//密码错误
//		break;
//		default:			break;
//	}
//	//显示密码区
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
//void (*current_operation_index)();//跟刷新菜单有关。
//typedef struct
//{
//	unsigned char current;//菜单索引
//	unsigned char key_note; 	//记录键
//	unsigned char key_set; 		//设置键
//	unsigned char key_up;     //上键
//	unsigned char key_down;   //下键		
//	unsigned char key_cancel; //取消键
//	unsigned char key_enter;  //确认键
//	unsigned char key_release_alarm; 			//警情解除
//	unsigned char key_release_breakdown;  //故障消除
//	unsigned char key_self_check; 				//本机自检
//	unsigned char key_noise_reduction ;  	//消音
//	unsigned char key_reset; 							//复位
//	unsigned char key_Manual_alarm ;  		//手动报警
//	unsigned char key_response; 					//查岗应答
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
////很巧妙的函数指针
//key_table const table[30]=
//{
///*   
//(0) 按键: 记录查询
//(1) 按键: 设置
//(2) 按键: 上
//(3) 按键: 下
//(4) 按键: 取消
//(5) 按键: 确认
//(6) 按键: 警情解除
//(7) 按键: 故障消除
//(8) 按键: 本机自检
//(9) 按键: 消音
//(10)按键: 复位
//(11)按键: 手动报警
//(12)按键: 查岗应答
//(13)按键: 0
//(14)按键: 1
//(15)按键: 2
//(16)按键: 3
//(17)按键: 4
//(18)按键: 5
//(19)按键: 6
//(20)按键: 7
//(12)按键: 8
//(22)按键: 9
//*/
///*    0.  1.  2.  3.  4.  5.  6.  7.  8.  9.  10. 11. 12. 13. 14. 15. 16. 17. 18. 19. 20. 21. 22.    */
// {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,(*fun0)},//显示home界面	
// {1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,(*fun1)},//显示信息查询界面	 
// {2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,(*fun2)},//显示输入密码界面
//	 
// {3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,(*fun3)},//显示设置界面	 
// {4,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,(*fun4)},//设置界面光标移动加	
// {5,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,(*fun5)},//设置界面光标移动减	
//	 
// {6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,(*fun6)},//显示网络参数设置界面
// {7,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,(*fun7)},//设置界面光标移动加	
// {8,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,(*fun8)},//设置界面光标移动减	

// {9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,(*fun9)},//显示主机通信设置界面
// {10, 9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,(*fun10)},//设置界面光标移动加	
// {11, 9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,(*fun11)},//设置界面光标移动减	
//	 
// {12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,(*fun12)},//设置RS232/RS485波特率		 
// {13, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,(*fun13)},//设置界面光标移动加 
// {14, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,(*fun14)},//设置界面光标移动减

// {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,(*fun15)},//设置CAN波特率		
// {16, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,(*fun16)},//显示声音设置界面
// {17, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,(*fun17)},//设置界面光标移动加	
//	 
// {18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,(*fun18)},//显示背光调节界面
// {19, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,(*fun19)},//设置界面光标移动加		 
// {20, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,(*fun20)},//设置界面光标移动减		 
//	 
// {21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,(*fun21)},//显示密码设置界面

// {22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,(*fun22)},//显示声音设置界面
// {23, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,(*fun23)},//设置界面光标移动加	
// {24, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,(*fun24)},//设置界面光标移动减	

// {25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,(*fun25)},//显示时间设置界面
// {26, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,(*fun26)},//设置界面光标移动加	
// {27, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,(*fun27)},//设置界面光标移动减	
//	 
// {28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,(*fun28)},//二级密码输入
//};
////将数字按键转换为char
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
//	if(fun0_Cursor==0)	//进入设置的输入二级密码界面
//	{
//		func_index=2;
//	}
//	else				//进入信息查询界面
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
//			if(fun1_page_fixed>1)//判断，防止出界。
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
//			if(fun1_page>page_MAXnum)//判断，防止出界。
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
////		  LCD_SET_ON();//常亮屏幕
//		  LCD_SET_LONG_ON();//常亮屏幕
//			back_light_change(Backlight_brightness);//调节屏幕亮度	
//		  master_exportmessage_to_SD(0);
//			FileFlag=filesysint();
//		  if(FileFlag==0)
//		  { printf("文件系统正常！%d\n\r,FileFlag");
//				filesysInfRed(); 
//			  master_exportmessage_to_SD(1);
//			}
//			else
//			{ printf("文件系统异常！%d\n\r,FileFlag");
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
//		password_three_ok=2;//密码错误
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
//		case 0:	func_index=6;	break;//进入网络参数设置界面。
//		case 1:	func_index=9;	break;//进入主机通信设置界面。
//		case 2:	func_index=18;	break;//进入背光调节界面。
//		case 3:	func_index=21;	break;//进入密码设置界面。
//		case 4:	func_index=22;	break;//进入声音设置界面。
//		case 5:	func_index=25;	break;//进入时间设置界面。
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
////	fun6_Cursor=0;//这个菜单的所有标志位都清零。
////	func_index=3;	
//	switch(fun6_Cursor)//查看选中了哪个光标。
//	{
//		case 0:	
//			net_fixed=1;
//			func_index=6;
//		break;//选中有线
//		case 1:	
//			net_fixed=2;
//			func_index=6;
//		break;//选中无线
////		case 2:	
////			net_fixed=3;
////			func_index=6;
////		break;//选中服务器
//		case 2:			//选中保存
//			//ASL...//保存网络通信方式net_fixed//ASL...
//			buf[0]=net_fixed;
//			W25q16_Lock();
//			Writeflash_Config(buf,SERVER_INTERFACE_CONFIG_CMD);
//			Net_Init();
//			W25q16_UnLock();
//			fun6_Cursor=0;//这个菜单的所有标志位都清零。
//			func_index=3;
//		break;
//		case 3:			//选中退出
//			fun6_Cursor=0;//这个菜单的所有标志位都清零。
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
//		case 16://选中注册板卡
//			func_index=15;		
//			register_card_flag=1;//注册板卡标志位置1.

//		break;
//		case 17://选中编码表导入
//			func_index=15;			
//			register_card_flag=0;//编码表导入标志位置1.
//		break;
//		case 18://选中退出
//			fun9_Cursor=0;
//			func_index=3;
//		break;		
//		default://选中查看板卡信息		0~15
//			func_index=12;//选中板卡信息显示					
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
//		input_password_times=0;//密码错误
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
//	if(correction_time_way==1)//自动对时。则跳过年月日时分秒的设置。
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
//								if(date>=28)date=1;//平年，有28天。
//								else date++;					
//							}
//							else
//							{
//								if(date>=29)date=1;//闰年29天
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
//	if(correction_time_way==1)//自动对时。则跳过年月日时分秒的设置。
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
//							if(date<2)date=28;//平年，有28天。
//							else date--;						
//						}
//						else
//						{
//							if(date<2)date=29;//闰年29天
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
//			if(correction_time_way==0)//翻转校正时间的方式。
//			{
//				correction_time_way=1;//自动校时
//			}
//			else
//			{
//				correction_time_way=0;//手动校时
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
//								back_light_change(10);//调节亮度，关闭背光。	
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
//		password_three_ok=2;//密码错误
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

unsigned char	System_Mod=1;//1=工作，0=测试；
extern void RefreshRunLedState(void);


void key_Explain(unsigned int key)
{
	unsigned char FLAG=1;

#ifdef FB1010
	//权限锁读取
	FLAG=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9);//通，则低电平，进入II和III级权限。断，就是I级权限，没有功能。
#endif

#ifdef FB1030
	FLAG=0;
#endif	
	
	
	if(FLAG==1)//断 I  手报，消音
	{
				switch(key)
				{
						case 0x00000400:
						FaultMute_Key();//发数据
						VoiceActionClear();//控制灯和喇叭
						break;//消音=发送数据然后消音；
						
						case 0x00000100:
						ManAlarm_Key();//发送手报数据；
						break;//手报；
						
				    default:break;
				}	
  }		
	else if(FLAG==0)//通 II\ III  所有功能
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
						break;//设为，测试与工作切换按键
						
					case 0x00000400:
						FaultMute_Key();//发数据
						VoiceActionClear();//控制灯和喇叭
						break;//消音=发送数据然后消音；
					
					case 0x00000080:
						VoiceActionClear();//控制灯和喇叭
						SelfCheckStep1();//给个显示；
						Reste_Key();//发送数据，计时，复位；
						break;//复位
					
					case 0x00000100:
						ManAlarm_Key();//发送手报数据；
						break;//手报；
					
					case 0x00000001:
						JingQingXiaoChu_Key();//警情消除
						break;//警情消除；
					
					case 0x00000040:
						Duty_Key(); //查岗应答
					break;//查岗应答
					
					case 0x00000800:
						SelfCheck_Key();
					 break;//自检
					
					default:break;
				}
  }
	else
	{}
}



//void refresh_lcd(void)//刷新屏幕
//{
//	OS_ERR err;
//	unsigned char flag=0;
//	flag=LcdOn();				//点亮LCD后延时800ms以上再发送才能显示正常,若已经点亮则无需延时
//	back_light_change(Backlight_brightness);//调节屏幕亮度
//	if(flag==1)
//	{
//		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
////		back_light_change(Backlight_brightness);//调节屏幕亮度
////		LCDTime_Init();
//	}
//	current_operation_index=table[func_index].current_operation;
//	(*current_operation_index)();//执行当前操作函数
//}
OS_TMR selfchecktmr;
unsigned char g_self_check_step=0;
extern OS_TCB	lcdTaskTCB;
extern unsigned char g_RunState;
void selfcheck_stop(void)
{
	OS_STATE state;
	OS_ERR err;
	
	if(g_self_check_step!=0)//如果在自检过程，则停止自检
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

void check_self_fun(unsigned char state)//自检
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
//	u16 add0=0x1001;//月
//	u16 add1=0x1003;//日
//	u16 add2=0x1970;//用户编码表导入中。。。
//	u16 add3=0x1990;//用户编码表导入成功。	
//	u16 add4=0x19B0;//请断电，取出内存卡。然后重新上电。	
//	unsigned char SEND_TEXT[100];//定义串口2发送数组
//	unsigned char len=0;	
//	unsigned char step=1;//进展到的步骤。
//	change_back_picture(0x002F);//调取纯底色背景图片
//	input_text(add0,2,"月");//显示：月
//	input_text(add1,2,"日");//显示：日
//	switch(step)
//	{
//		case 0:	//用户编码表导入中。。。
//			input_text(add2,22,"用户编码表导入中。。。");
//			input_text(add3,20,"                    ");	
//			input_text(add4,34,"                                  ");			
//		break;
//		case 1:	
//			input_text(add2,22,"用户编码表导入中。。。");
//			input_text(add3,20,"用户编码表导入成功。");	
//			input_text(add4,34,"请断电，取出内存卡。然后重新上电。");		
//		break;	
//		default:	
//			break;
//	}	
//}

///*软件重启*/
//void SysSoftReset1(void)
//{
//	printf("导出软件重启！\r\n");		
//	__set_FAULTMASK(1); 
//	NVIC_SystemReset(); 
//}

//void master_exportmessage_to_SD(unsigned char step)//显示报警信息导出至SD卡界面。  step代表进展到的步骤。
//{
//	u16 add0=0x1001;//月
//	u16 add1=0x1003;//日
//	u16 add2=0x1900;//历史报警数据导出中。。。
//	u16 add3=0x1920;//历史报警数据导出成功。	
//	u16 add4=0x1940;//请断电，取出内存卡。然后重新上电。	
//	unsigned char SEND_TEXT[100];//定义串口2发送数组
//	unsigned char len=0;	
//	u32 count=0;//历史报警数据导出成功。		
//	
//	
//	change_back_picture(0x002F);//调取编码表导入图片
//	input_text(add0,2,"月");//显示：月
//	input_text(add1,2,"日");//显示：日
//	switch(step)
//	{
//		case 0:	//历史报警数据导出中。。。
//			input_text(add2,24,"历史报警数据导出中。。。");
//			input_text(add3,22,"                    ");	
//			input_text(add4,34,"                                  ");			
//		break;
//		
//		case 1:	
//			input_text(add2,24,"历史报警数据导出中。。。");
//			input_text(add3,22,"历史报警数据导出成功。");	
//			input_text(add4,34,"请断电，取出内存卡。然后重新上电。");		
//		  while(1)
//			{
//				count++;
//			 if(count>10000000)
//			 {SysSoftReset1();}
//			}
//		break;

//			default:	
//			input_text(add2,24,"历史报警数据导出中。。。");
//			input_text(add3,22,"历史报警数据导出失败。");	
//			input_text(add4,34,"请断电，插入内存卡。然后重新上电。");		
//		  while(1)
//			{
//				count++;
//			 if(count>10000000)
//			 {SysSoftReset1();}			
//			;}
//		break;
//	}	
//}
//void SD_input_message_to_master(unsigned char step)//显示用户信息编码表通过SD卡导入到用户信息传输装置中  step代表进展到的步骤。
//{
//	u16 add0=0x1001;//月
//	u16 add1=0x1003;//日
//	u16 add2=0x1970;//用户编码表导入中。。。
//	u16 add3=0x1990;//用户编码表导入成功。	
//	u16 add4=0x19B0;//请断电，取出内存卡。然后重新上电。	
//	unsigned char SEND_TEXT[100];//定义串口2发送数组
//	unsigned char len=0;
//	change_back_picture(0x0030);//调取纯底色背景图片
//	input_text(add0,2,"月");//显示：月
//	input_text(add1,2,"日");//显示：日
//	switch(step)
//	{
//		case 0:	//用户编码表导入中。。。
//			input_text(add2,22,"用户编码表导入中。。。");
//			input_text(add3,20,"                    ");	
//			input_text(add4,34,"                                  ");			
//		break;
//		case 1:	
//			input_text(add2,22,"用户编码表导入中。。。");
//			input_text(add3,20,"用户编码表导入成功。");	
//			input_text(add4,34,"请断电，取出内存卡。然后重新上电。");		
//			while(1)
//			{;}
//		break;	
//		case 2:	
//			input_text(add2,22,"用户编码表导入中。。。");
//			input_text(add3,20,"用户编码表导入失败。");	
//			input_text(add4,34,"请断电，检查内存卡。然后重新上电。");		
//			while(1)
//			{;}
//		break;			
//		default:	
//			break;
//	}
//}

//void display_SD_schedule()//显示SD卡导入用户编码表进度。
//{
//	unsigned char data_local[100]={0x20};
//	unsigned int schedule=0;//进度	
//	unsigned int i=0;
//	unsigned char len=0;
//		LCD_SET_ON();//常亮屏幕
//		schedule=FBWBReadStatus;
//		memset(data_local,0x20,100);//		
//		memcpy(data_local,"导入进度：",10);
//		data_local[11]=schedule/100+0x30;// 取进度的百位数部分		
//		data_local[12]=(schedule%100)/10+0x30;// 取进度的十位数部分
//		data_local[13]=((schedule%100)%10)%10+0x30;// 取进度的个位数部分
//		data_local[14]='%';		
//		input_text(0x2F80,15,data_local);

//		memset(data_local,0x20,100);//		
//		memcpy(data_local,"当前读取的条数：",16);
//		data_local[17]=FBWBReadNbr/10000+0x30;// 取进度的万位数部分		
//		data_local[18]=(FBWBReadNbr%10000)/1000+0x30;// 取进度的千位数部分		
//		data_local[19]=((FBWBReadNbr%10000)%1000)/100+0x30;// 取进度的百位数部分			
//		data_local[20]=(((FBWBReadNbr%10000)%1000)%100)/10+0x30;// 取进度的十位数部分				
//		data_local[21]=(((FBWBReadNbr%10000)%1000)%100)%10+0x30;// 取进度的个位数部分			
//		input_text(0x2FA0,22,data_local);

//		memset(data_local,0x20,100);//
//		memcpy(data_local,"当前读取失败的条数：",20);
//		data_local[21]=FBWBReadFail/10000+0x30;// 取进度的万位数部分
//		data_local[22]=(FBWBReadFail%10000)/1000+0x30;// 取进度的千位数部分
//		data_local[23]=((FBWBReadFail%10000)%1000)/100+0x30;// 取进度的百位数部分
//		data_local[24]=(((FBWBReadFail%10000)%1000)%100)/10+0x30;// 取进度的十位数部分
//		data_local[25]=(((FBWBReadFail%10000)%1000)%100)%10+0x30;// 取进度的个位数部分
//		input_text(0x2FC0,26,data_local);

//		memset(data_local,0x20,100);//
//		memcpy(data_local,"剩余时间为：",12);
//		data_local[13]=i/100+0x30;// 取进度的百位数部分
//		data_local[14]=(i%100)/10+0x30;// 取进度的十位数部分
//		data_local[15]=((i%100)%10)%10+0x30;// 取进度的个位数部分
//		data_local[16]=0xC3;//秒	
//		data_local[17]=0xEB;//			
//		input_text(0x2FE0,18,data_local);
//}


//void LCD_backlight_low(void)
//{
//	unsigned char test[50];	
//	unsigned char SEND_BUFER[50];//定义串口发送数组
//	unsigned int crc1;
//	unsigned char i=0;	
//	OS_ERR err;
//	for(i=0;i<50;i++)
//	{
//		uart5_receive_data[i]=0;
//	}
//	uart5_receive_data_inc=0;

//	SEND_BUFER[0]=0x5A;//设置屏幕亮度
//	SEND_BUFER[1]=0xA5;
//	SEND_BUFER[2]=0x05;//数据长度,数据的长度包含校验位的2位。	
//	SEND_BUFER[3]=0x80;
//	SEND_BUFER[4]=0x01;
//	SEND_BUFER[5]=0x00;//屏幕亮度的值。00~40  00是息屏。
//	crc1=crc16((unsigned char*)&SEND_BUFER[3],3);//校验包括。数组3，4，5这三个元素。
//	SEND_BUFER[6]=crc1&0x00ff;//校验和的低位。
//	SEND_BUFER[7]=crc1>>8;		 //校验和的高位。
////	USART1_Send_Data(SEND_BUFER,8);//测试		
////5A A5 05 80 01 20 70 60

//	for(i=0;i<3;i++)//防止一次读取不成功。
//	{
//		i++;
//		SendPacketToLcd1(SEND_BUFER,8);
//		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
//		if( (uart5_receive_data[19]==uart5_receive_data[18])&&(uart5_receive_data[18]==uart5_receive_data[17])&&(uart5_receive_data[19]==0) )
//		{
//			
//		}
//		else
//		{
//			i=4;//跳出for循环。
//		}
//	}
////	USART1_Send_Data(uart5_receive_data,50);//测试	
//}



//声音设置
//2.设置新的界面显示本机信息在屏幕上，包含以下内容：
//本机地址（从FLASH中读取）、
//服务器地址（从FLASH中读取）、
//K2信息（动态静态设置、本机IP、目标地址端口号、连接状态，从K2中读取）、
//GM3信息（IMEI、SN、目标地址端口号、连接状态、无线信号强度，从GM3中读取）、
//主板程序版本号（从FLASH中读取）

//主机通信设置
//主机接口显示信息：显示协议类型（厂家型号）、版本号（协议解析程序版本号）及接口类型方式（CAN/RS485、波特率、校验位、数据位、停止位）；（曹）；4天；

//配接主机或者电气火灾设备发生火警等时，主机号不显示；（曹）1天

//将工装测试程序与FBAT更改ID、更改GM3设置、更改K2设置的程序合并为同一个程序，评审后发给现场及生产使用；（曹）
//22日前完成；

//故障代码指示不明确，用户无法直观理解；
//按钮框颜色更改；

void ID_Print(void)
{
	unsigned char i=0;
	
	printf("1.设备ID=");
	for(i=0;i<6;i++)
	{
	printf("%x_",Local_Addres[i]);
	}
	printf("\r\n");
}
