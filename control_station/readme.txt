#宏定义
//FB1010
XYB_DYJ,FB1010
D007_JB_QBL_bdqn11s,XYB_CXY,FB1010
D020_HaiWan9000,XYB_CXY,FB1010
D029_MB4000,XYB_CXY,FB1010



//FB1030
XYB_DYJ,FB1030
D007_JB_QBL_bdqn11s,XYB_CXY,FB1030

//本程序 远程升级不能用
关闭了NVIC_SetVectorTable、

BC35遗留问题
  没电，灯闪，初始化加延时。巡检返回当前状态，bug处理。用户信息上传正常监视。
  AT_CEREG01 AT_CEREG05 新增，排除CEREG？返回值0 1 和 0 5网络正常下的误判,查岗需要清零数组gprs_check_connect。OK
  心跳1小时不准，硬件定时器！OK
  NB信号强度上发！
  NB下行无数据；ok
  看门狗2，喂狗；取消2；OK
  W25Q16初始化；
  手报喇叭响，复位，喇叭还响；20190107 ok
  随机死机；20190107 OK

  报警当当当然后故障声。ok
  最终代码ID，mac无权限修改。ok
  shutdown初始化改为高。
  CAN的发送和不正确，与三合一协议板不一样


//BC35测试绑定的ID  0x12,0x07,0x07,0x00,0x09,0x11。
//安少龙发现自检BUG，stop函数，是否当前在进行自检。



//注意事项
//NB模块与设备ID在平台绑定后，才能使用NB功能。
//mac地址开始：00A1001B7B30
//ID地址开始： 120707000101

//协议部分注意事项
需要设置波特率功能
D015_GTYA_GK601
D027_SJ_YunAn3101
D028_SJ_YunAn2002


1.IO使用配置：
//////////语音//////////
PC5：NVC_BUSY(5VIO) 
PB0：NVC_CLK
PB1: NVC_SDA
PC4: NVC_4990_ShutDown

//////////蜂鸣器//////////
PC9：Bell

//////////RS485//////////UART3
PB11: 485_RXD    
PA8:  485_SWITCH
PB10: 485_TXD

//////////RS232//////////UART2
PA3：232_RXD
PA2：232_TXD

//////////CAN1//////////CAN1
PA12：CAN_TXD
PA11：CAN_RXD

//////////CAN2//////////备用
PB6： CAN_TXD2
PB5： CAN_RXD2

//////////以太网//////////SPI2
PC7： W5500_INTn
PC6： W5500_RST
PB12：W5500_CS
PB13：W5500_SCLK
PB14：W5500_MISO
PB15：W5500_MOSI

//////////存储器//////////SPI1
PA4：25CL_CS
PA5：25CL_SLK
PA6：25CL_MISO
PA7：25CL_MOSI

//////////主电//////////
PA0：AD_5V

//////////备电//////////有主电，断线采样。没有主电直接采样。
PC8：AD_SWITCH
PA1：AD_3.6V

//////////LED//////////
PB9:  LED_RCK
PB8:  LED_SCK
PB7:  LED_DATA

//////////按键/////////
PC0： KEY1
PC1： KEY2
PC2： KEY3
PC3： KEY4
PB3： KEY5
PB4： KEY6


//调试记录单
1.20180810 RS232接口接收报警数据，网口可以上报。
2.
3.



//电路图修改
VCC4V3
VCC3V3
V3V3

//工装设计
1.锂电池检测3.6V，AD
