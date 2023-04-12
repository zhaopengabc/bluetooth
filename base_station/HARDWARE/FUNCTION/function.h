#ifndef __FUNCTION_H
#define __FUNCTION_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
extern unsigned short judge_data_u16[10]; 
extern unsigned char sumcheck(unsigned char *data,unsigned char star,unsigned char end);//计算校验和	
void DATA_change(unsigned char *data,unsigned int size,unsigned char *target_data);//data是准备转换的数组。size是转换数组中元素的数量，必须是偶数,target_data是数据转换后存放的数组。 
void handle_arrays_u8(unsigned char *data,unsigned char size,unsigned char *printf_data);//处理数组，DATA是要处理的数组，SIZE是数组的长度。//比如将0x25分开就是0x32和0x35.。0xAB分开就是0x40,0x41. 
//DATA是输入的数组，SIZE是数组中元素的数量，Printf_data是整理完后用于测试打印的数组。
void handle_arrays_u16(unsigned int *DATA,unsigned int SIZE);//处理数组，DATA是要处理的数组，SIZE是数组的长度。比如将0x2542分开就是0x32,0x35,0x34,0x32.。0xABCD分开就是0x40,0x41,0x42x0x43. 
void DATA_change_u8_to_u16(unsigned char* data_u8,unsigned int size,unsigned short* data_u16); //将两个8位数据转换成一个16位数据。 
void DATA_change_u16_to_u8(unsigned short *data_u16,unsigned int size,unsigned char *data_u8); //将一个16位数据转换成两个8位数据。 
void Printf_device_type(unsigned char type);//测试打印设备类型。根据国标。
void Judge_ID(unsigned short *In_put_ID);//首先判断合法性。然后依据大小进行排列。
extern unsigned int Save_ROUTER_ID(unsigned char *ROUTER_ID);//保存网关ID.如果保存成功。返回1。如果保存失败，则返回0。
void Judge_ID1(unsigned int In_put_ID);//首先判断合法性。然后依据大小进行排列。
void Delete_NODE_ID(unsigned char *In_put_ID);//删除设备ID.
extern unsigned int Save_NODE_ID(unsigned char *In_put_ID);//保存设备ID.如果保存成功。返回保存的地址号。如果保存失败，则返回0xFF。
void Delete_ALL_NODE_ID(void);//删除所以注册的设备ID
extern unsigned char  Sting_Compare(unsigned char  *Sting1,unsigned char  *Sting2,unsigned char Len);//字符串比较
extern unsigned int str_int(unsigned char *str);//将数组转换成int型数据。比如数组str为：0x31,0x32,0x33,0x34,0x35,0x36。转换完成后就变成：123456了
#endif
