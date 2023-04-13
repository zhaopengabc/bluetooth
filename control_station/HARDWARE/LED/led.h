#ifndef __LED_H
#define __LED_H
#include "sys.h"


#ifdef FB1010
//外设状态
#define HuoJing_LED		   (0x00000001<<4) //1 OK
#define JianGuan_LED		 (0x00000001<<7) //2 OK 状态           
#define GuZhang_LED			 (0x00000001<<5) //3 OK
#define PingBi_LED		   (0x00000001<<6) //4 OK

//本机操作
//工作、测试
#define GongZuo_LED		   (0x00000001<<17)  //5 OK
#define CeShi_LED		     (0x00000001<<16)  //5 OK
#define ZongGuZhang_LED	 (0x00000001<<14)  //11 OK
#define ZhuJiTongXin_GLED	  (0x00000001<<3) //13 OK
#define ZhuJiTongXin_YLED	  (0x00000001<<2) //13 OK
//网络状态；
#define WangLuoTongXin_GLED	(0x00000001<<1) //12 OK
#define WangLuoTongXin_YLED	(0x00000001<<0) //12 OK

#define ZhuDianGuZhang_GLED	(0x00000001<<11) //9 OK
#define ZhuDianGuZhang_YLED	(0x00000001<<10) //9 OK
#define BeiDianGuZhang_GLED	(0x00000001<<13) //10 OK
#define BeiDianGuZhang_YLED	(0x00000001<<12) //10 OK
 
#define JingQingXiaoChu_LED	(0x00000001<<18) //6 OK
#define XiaoYin_LED			    (0x00000001<<15) //14 OK
#define ShouDongBaoJing_LED	(0x00000001<<8)  //7 OK
#define ChaGangYingDa_LED	  (0x00000001<<9)  //8 OK

#endif

#ifdef FB1030
//外设状态
#define HuoJing_LED		   (0x00000001<<4) //1 OK
#define JianGuan_LED		 (0x00000001<<5) //2 OK 状态           
#define GuZhang_LED			 (0x00000001<<5) //3 OK
#define PingBi_LED		   (0x00000001<<5) //4 OK

//本机操作
//工作、测试
#define GongZuo_LED		   (0x00000001<<17)  //5 OK
#define CeShi_LED		     (0x00000001<<16)  //5 OK
#define ZongGuZhang_LED	 (0x00000001<<14)  //11 OK
#define ZhuJiTongXin_GLED	  (0x00000001<<3) //13 OK
#define ZhuJiTongXin_YLED	  (0x00000001<<2) //13 OK
//网络状态；
#define WangLuoTongXin_GLED	(0x00000001<<1) //12 OK
#define WangLuoTongXin_YLED	(0x00000001<<0) //12 OK

#define ZhuDianGuZhang_GLED	(0x00000001<<11) //9 OK
#define ZhuDianGuZhang_YLED	(0x00000001<<10) //9 OK
#define BeiDianGuZhang_GLED	(0x00000001<<13) //10 OK
#define BeiDianGuZhang_YLED	(0x00000001<<12) //10 OK
 
#define JingQingXiaoChu_LED	(0x00000001<<18) //6 OK
#define XiaoYin_LED			    (0x00000001<<15) //14 OK
#define ShouDongBaoJing_LED	(0x00000001<<4)  //7 OK 修改
#define ChaGangYingDa_LED	  (0x00000001<<9)  //8 OK

#endif



#define LED_ALL	0xffffff

#define LED_OFF	0x00
#define LED_ON	0x01
#define LED_TOGGLE	0x02

//#define HuoJing_LED			(0x00000001<<0)
//#define JianGuan_LED		(0x00000001<<1)
//#define GuZhang_LED			(0x00000001<<2)
//#define PingBi_LED			(0x00000001<<3)
//#define GongZuo_LED			(0x00000001<<6)
//#define ZongGuZhang_LED		(0x00000001<<7)

//#define ZhuJiTongXin_GLED	(0x00000001<<9)
//#define ZhuJiTongXin_YLED	(0x00000001<<8)
//#define WangLuoTongXin_GLED	(0x00000001<<10)
//#define WangLuoTongXin_YLED	(0x00000001<<11)
//#define ZhuDianGuZhang_GLED	(0x00000001<<12)
//#define ZhuDianGuZhang_YLED	(0x00000001<<13)
//#define BeiDianGuZhang_GLED	(0x00000001<<15)
//#define BeiDianGuZhang_YLED	(0x00000001<<14)

//#define WangLuoGuZhang_LED	(0x00000001<<4)
//#define XianLuGuZhang_LED	(0x00000001<<5)
//#define JingQingXiaoChu_LED	(0x00000001<<21)
//#define GuZhangXiaoChu_LED	(0x00000001<<20)
//#define BenJiZiJian_LED		(0x00000001<<19)
//#define XiaoYin_LED			(0x00000001<<18)
//#define ShouDongBaoJing_LED	(0x00000001<<17)
//#define ChaGangYingDa_LED	(0x00000001<<16)

//#define LED_ALL	0xffffff

//#define LED_OFF	0x00
//#define LED_ON	0x01
//#define LED_TOGGLE	0x02
////端口定义
//#define SCK_595  PBout(8)	// 	 
//#define RCK_595  PBout(9)	// 
//#define DATA_595 PBout(7)	// 
//端口定义
#define SCK_595  PBout(7)	// 	 
#define RCK_595  PAout(15)// 
#define DATA_595 PBout(8)	//

extern unsigned char	System_Mod;//1=工作，0=测试；

void LED_Init(void);
unsigned char Leds_Action(unsigned int ledn,unsigned char ledaction,unsigned int nmsec);
unsigned int ReadLedToggleStatus(void);
void LED_Test(void);
#endif

/***********************end of file**************************/
