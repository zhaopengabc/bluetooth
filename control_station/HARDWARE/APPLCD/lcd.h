#ifndef __LCD_H
#define __LCD_H

extern unsigned char g_self_check_step;
//extern unsigned char Loop_Part_flag;//区分用户编码表编号和正常回路部件号的标志位。1的时候，为用户编码表号码，0的时候为回路部件号。
//void LCD_Init(void);
//void LCDTime_Init(void);
//void refresh_lcd(void);
void key_Explain(unsigned int key);
void lcd_SelfCheck(unsigned char RunState);
//void master_exportmessage_to_SD(unsigned char step);//显示报警信息导出至SD卡界面。
//void SD_input_message_to_master(unsigned char step);//显示用户信息编码表通过SD卡导入到用户信息传输装置中
//extern void LCD_SET_ON(void);
//extern void LCD_SET_OFF(void);	
//void display_SD_schedule(void);//显示SD卡导入用户编码表进度。
//void dwin_time_set(const char *data);//输入时间更改迪文屏内部RC时间
//unsigned char dwin_time_read(char *time_data);//输入时间更改迪文屏内部RC时间
//void LCD_backlight_low(void);
//void back_light_change(unsigned char Backlight_brightness);//调节亮度
//extern unsigned short register_flag;//需要显示的板卡信息  //注册成功标志位。1的时候，注册成功，0的时候注册失败。
//extern unsigned char RegisteProComplete;
//extern unsigned char LcdOn(void);
//extern void LCD_SET_LONG_ON(void);

void selfcheck_stop(void);
void ID_Print(void);
#endif






































//#ifndef __LCD_H
//#define __LCD_H

//extern unsigned char g_self_check_step;
//void LCD_Init(void);
//void LCDTime_Init(void);
//void refresh_lcd(void);
//void key_Explain(unsigned int key);
//void lcd_SelfCheck(unsigned char RunState);
//void master_exportmessage_to_SD(unsigned char step);//显示报警信息导出至SD卡界面。
//void SD_input_message_to_master(unsigned char step);//显示用户信息编码表通过SD卡导入到用户信息传输装置中
//extern void LCD_SET_ON(void);
//extern void LCD_SET_OFF(void);	
//void display_SD_schedule(void);//显示SD卡导入用户编码表进度。
//void dwin_time_set(const char *data);//输入时间更改迪文屏内部RC时间
//unsigned char dwin_time_read(char *time_data);//输入时间更改迪文屏内部RC时间
//#endif


