#ifndef __LCD_H
#define __LCD_H

extern unsigned char g_self_check_step;
//extern unsigned char Loop_Part_flag;//�����û�������ź�������·�����ŵı�־λ��1��ʱ��Ϊ�û��������룬0��ʱ��Ϊ��·�����š�
//void LCD_Init(void);
//void LCDTime_Init(void);
//void refresh_lcd(void);
void key_Explain(unsigned int key);
void lcd_SelfCheck(unsigned char RunState);
//void master_exportmessage_to_SD(unsigned char step);//��ʾ������Ϣ������SD�����档
//void SD_input_message_to_master(unsigned char step);//��ʾ�û���Ϣ�����ͨ��SD�����뵽�û���Ϣ����װ����
//extern void LCD_SET_ON(void);
//extern void LCD_SET_OFF(void);	
//void display_SD_schedule(void);//��ʾSD�������û��������ȡ�
//void dwin_time_set(const char *data);//����ʱ����ĵ������ڲ�RCʱ��
//unsigned char dwin_time_read(char *time_data);//����ʱ����ĵ������ڲ�RCʱ��
//void LCD_backlight_low(void);
//void back_light_change(unsigned char Backlight_brightness);//��������
//extern unsigned short register_flag;//��Ҫ��ʾ�İ忨��Ϣ  //ע��ɹ���־λ��1��ʱ��ע��ɹ���0��ʱ��ע��ʧ�ܡ�
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
//void master_exportmessage_to_SD(unsigned char step);//��ʾ������Ϣ������SD�����档
//void SD_input_message_to_master(unsigned char step);//��ʾ�û���Ϣ�����ͨ��SD�����뵽�û���Ϣ����װ����
//extern void LCD_SET_ON(void);
//extern void LCD_SET_OFF(void);	
//void display_SD_schedule(void);//��ʾSD�������û��������ȡ�
//void dwin_time_set(const char *data);//����ʱ����ĵ������ڲ�RCʱ��
//unsigned char dwin_time_read(char *time_data);//����ʱ����ĵ������ڲ�RCʱ��
//#endif


