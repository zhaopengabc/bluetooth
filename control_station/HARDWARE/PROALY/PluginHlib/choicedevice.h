//#include "DeviceCommon.h"
//////////////////宏定义说明////////////////////
// #define MCLineJudge_Enable 主机断线判断功能使能；有接收到主机心跳包；
// #define UARTBUFFER_Enable 串口数据自动接收；
// #define BMB_analyse 程序编码表解析宏；
#ifdef XYB_CXY

#ifdef  P001_Printer
#include "P001_Printer.h"
#define UARTBUFFER_Enable 
//#define MCLineJudge_Enable 
#endif

#ifdef  P002_HW100
#include "P002_HW100.h"
#define UARTBUFFER_Enable 
//#define MCLineJudge_Enable 
#endif

#ifdef  P003_NT8007
#include "P003_NT8007.h"
#define UARTBUFFER_Enable 
//#define MCLineJudge_Enable 
#endif

#ifdef  D041_NDFR6000
#include "D041_NDFR6000.h"
//#define MCLineJudge_Enable
#define UARTBUFFER_Enable 
#define RS232_Enable
#endif

#ifdef  D040_FAT_6100A
#include "D040_FAT_6100A.h"
#define  MCLineJudge_Enable
#define  CANBUFFER_Enable
#define  CAN_Enable
#endif

#ifdef  D039_HB_G250FM
#include "D039_HB_G250FM.h"
//#define MCLineJudge_Enable
#define UARTBUFFER_Enable 
#define RS232_Enable
#endif

#ifdef  D038_THA_3000
#include "D038_THA_3000.h"
//#define MCLineJudge_Enable
#define UARTBUFFER_Enable 
#define RS485_Enable
#endif

#ifdef  D037_YKSY_4046
#include "D037_YKSY_4046.h"
//#define MCLineJudge_Enable 
#define CANBUFFER_Enable
#define CAN_Enable
#endif

#ifdef  D036_MB9500
#include "D036_MB9500.h"
//#define MCLineJudge_Enable
#define UARTBUFFER_Enable 
#define RS485_Enable
#endif

#ifdef  D035_NDFR3030
#include "D035_NDFR3030.h"
//#define MCLineJudge_Enable
#define UARTBUFFER_Enable 
#define RS232_Enable
#endif

#ifdef  D034_AD8000
#include "D034_AD8000.h"
//#define MCLineJudge_Enable
#define UARTBUFFER_Enable 
#define RS232_Enable
#endif

#ifdef  D033_HTCK803
#include "D033_HTCK803.h"
//#define MCLineJudge_Enable
#define UARTBUFFER_Enable 
#define RS485_Enable
#endif

#ifdef  D032_CZLH128
#include "D032_CZLH128.h"
//#define MCLineJudge_Enable
#define UARTBUFFER_Enable 
#define RS232_Enable
#endif

#ifdef  D031_DX200
#include "D031_DX200.h"
//#define MCLineJudge_Enable
#define UARTBUFFER_Enable 
#define RS232_Enable
#endif

#ifdef  D030_NT8001
#include "D030_NT8001.h"
//#define MCLineJudge_Enable
#define UARTBUFFER_Enable 
#define RS232_Enable
#endif

#ifdef  D029_MB4000
#include "D029_MB4000.h"
//#define MCLineJudge_Enable
#define UARTBUFFER_Enable 
#define RS485_Enable
#endif

#ifdef  D028_SJ_YunAn2002
#include "D028_SJ_YunAn2002.h"
#define MCLineJudge_Enable 
#define UARTBUFFER_Enable 
#define RS232_Enable
#endif

#ifdef  D027_SJ_YunAn3101
#include "D027_SJ_YunAn3101.h"
#define MCLineJudge_Enable 
#define UARTBUFFER_Enable 
#define RS232_Enable
#endif

#ifdef  D026_HW200
#include "D026_HW200.h"
#define UARTBUFFER_Enable 
//#define MCLineJudge_Enable 
#define RS232_Enable
#endif

#ifdef  D025_XM_AnDe2008
#include "D025_XM_AnDe2008.h" 
#define  CANBUFFER_Enable 
//#define MCLineJudge_Enable
#define CAN_Enable
#endif

#ifdef  D024_SJ_YunAn3102
#include "D024_SJ_YunAn3102.h"
#define MCLineJudge_Enable 
#define UARTBUFFER_Enable 
#define RS232_Enable
#endif

#ifdef  D021_SongJiang3208G
#include "D021_SongJiang3208G.h"
#define UARTBUFFER_Enable 
//#define MCLineJudge_Enable 
#define RS232_Enable
#endif

#ifdef  D020_HaiWan9000
#include "D020_HaiWan9000.h" 
#define UARTBUFFER_Enable 
#define MCLineJudge_Enable 
#define RS232_Enable
#endif

#ifdef  D019_FA_5050
#include "D019_FA_5050.h"
//#define MCLineJudge_Enable
#define UARTBUFFER_Enable 
#define RS232_Enable
#endif

#ifdef  D018_FSR_V6851
#include "D018_FSR_V6851.h"
//#define MCLineJudge_Enable
#define UARTBUFFER_Enable 
#define RS232_Enable
#endif

#ifdef  D017_THA_3016
#include "D017_THA_3016.h"
#define MCLineJudge_Enable
#define UARTBUFFER_Enable 
//#define RS232_Enable
#define RS485_Enable
#endif

#ifdef  D016_DPS_5115
#include "D016_DPS_5115.h" 
#define UARTBUFFER_Enable 
//#define MCLineJudge_Enable
#define RS232_Enable
#endif

#ifdef  D015_GTYA_GK601
#include "D015_GTYA_GK601.h"
//#define MCLineJudge_Enable
#define UARTBUFFER_Enable 
#define RS232_Enable
#endif

#ifdef  D012_YKSY_YBZ127
#include "D012_YKSY_YBZ127.h"
//#define MCLineJudge_Enable 
#define CANBUFFER_Enable
#define CAN_Enable
#endif

#ifdef  D008_JB_QBL_LD128E
#include "D008_JB_QBL_LD128E.h"
#define  MCLineJudge_Enable
#define  CANBUFFER_Enable
#define  CAN_Enable
#endif

#ifdef  D007_JB_QBL_bdqn11s
#include "D007_JB_QBL_bdqn11s.h"
//#define MCLineJudge_Enable
#define CAN_Enable
#define CANBUFFER_Enable
#define RS232_Enable
#define UARTBUFFER_Enable 
#endif

#ifdef  D006_YI_6000G
#include "D006_YI_6000G.h"
#define UARTBUFFER_Enable 
#define MCLineJudge_Enable
#define RS232_Enable
#endif

#ifdef  D004_JB_QBL_fs8200
#include "D004_JB_QBL_fs8200.h"
#define MCLineJudge_Enable
#define CAN_Enable
#endif

#ifdef  D003_DH9000
#include "D003_DH9000.h"
#define UARTBUFFER_Enable 
#define MCLineJudge_Enable
#define RS232_Enable
#endif

#ifdef  D002_JB_QBL_9108
#include "D002_JB_QBL_9108.h"
#define MCLineJudge_Enable 
#define CAN_Enable
#define RS485_Enable
#define UARTBUFFER_Enable 
#endif

extern PluginInterface DevicePlugin_Install(void);

#endif







