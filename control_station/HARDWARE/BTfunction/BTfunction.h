#ifndef __BTFUNCTION_H
#define __BTFUNCTION_H

//外部FLASH存储区划分
//下载存储
#define APP_DownLoadAddress 0x150000
//备份存储
#define APP_RecoverAddress  0x1B0000


typedef struct
{
	unsigned char m_HeaderL;
	unsigned char m_HeaderH;	
	unsigned char m_DeviceType;
	unsigned char m_CountL;		
	unsigned char m_CountH;	
	unsigned char m_VersionL;		
	unsigned char m_VersionH;
	unsigned char m_SN[15];		
	unsigned char m_IMEI[15];	
	unsigned char m_PhoneNumber[11];		
	unsigned char m_Type;	
	unsigned char m_Encrypt;	
	unsigned char m_LengthL;	
	unsigned char m_LengthH;	
	unsigned char m_Data[4];	
	unsigned char m_Check;	
	unsigned char m_EndL;		
	unsigned char m_EndH;		
}BT_frame;

#endif
