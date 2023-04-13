#ifndef __FRAME_H
#define __FRAME_H
#include "SendToServerDataType.h"
#include "rtc.h"
#include "string.h"
#define OPERATION_WORK		0x00
#define OPERATION_RESET		0x01
#define OPERATION_SILENCE	0x02
#define OPERATION_MANALARM	0x04
#define OPERATION_ALARMREMOVE	0x08
#define OPERATION_SELFCHECK 0x10
#define OPERATION_RESPONSE	0x20
#define OPERATION_TEST		0x40
#define OPERATION_FAULTREMOVE	0x80

#define OPERATlECEL_SECOND 2
#define OPERATlECEL_THIRD 3

unsigned int ReportToServer_InformationFrame(unsigned int serial_number,unsigned char *data,unsigned int lenth,unsigned char *destdata);
unsigned int ReplyToServer_ConfirmationFrame(unsigned int serial_number,unsigned char command,unsigned char *destdata);
unsigned int ReplyToServer_InformationFrame(unsigned int serial_number,unsigned char *data,unsigned int lenth,unsigned char *destdata);

unsigned char Device_RunningState(unsigned char *buf);
unsigned char Device_Operation_Information(unsigned char *buf,unsigned char Operation,unsigned char level);
unsigned char Device_Version(unsigned char *buf);
unsigned char Device_SysTime(unsigned char *buf);

unsigned int ReportToServer(unsigned char type,unsigned char *data,unsigned int len,unsigned char *destdata);
#endif


