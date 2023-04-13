#include "ChoiceDevice.h"

#ifdef XYB_CXY

PluginInterface DevicePlugin_Install(void)
{
	PluginInterface gPluginInterface;

#ifdef   P001_Printer
 	gPluginInterface.cPLUGIN_Install 	= P001_Printer_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= P001_Printer_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = P001_Printer_Plugin.cPLUGIN_Interrupt;
#endif		
	
#ifdef   P002_HW100
 	gPluginInterface.cPLUGIN_Install 	= P002_HW100_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= P002_HW100_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = P002_HW100_Plugin.cPLUGIN_Interrupt;
#endif	
	
#ifdef   P003_NT8007
 	gPluginInterface.cPLUGIN_Install 	= P003_NT8007_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= P003_NT8007_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = P003_NT8007_Plugin.cPLUGIN_Interrupt;
#endif	

#ifdef   D041_NDFR6000
 	gPluginInterface.cPLUGIN_Install 	= D041_NDFR6000_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D041_NDFR6000_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D041_NDFR6000_Plugin.cPLUGIN_Interrupt;
#endif

#ifdef   D040_FAT_6100A
 	gPluginInterface.cPLUGIN_Install = D040_FAT_6100A_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run =     D040_FAT_6100A_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt =  D040_FAT_6100A_Plugin.cPLUGIN_Interrupt;
#endif

#ifdef   D039_HB_G250FM
 	gPluginInterface.cPLUGIN_Install = D039_HB_G250FM_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run =     D039_HB_G250FM_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt =  D039_HB_G250FM_Plugin.cPLUGIN_Interrupt;
#endif

#ifdef   D038_THA_3000
 	gPluginInterface.cPLUGIN_Install = D038_THA_3000_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run =     D038_THA_3000_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt =  D038_THA_3000_Plugin.cPLUGIN_Interrupt;
#endif	
	
#ifdef   D037_YKSY_4046
 	gPluginInterface.cPLUGIN_Install = D037_YKSY_4046_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run = D037_YKSY_4046_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt =  D037_YKSY_4046_Plugin.cPLUGIN_Interrupt;
#endif

#ifdef   D036_MB9500
 	gPluginInterface.cPLUGIN_Install 	= D036_MB9500_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D036_MB9500_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D036_MB9500_Plugin.cPLUGIN_Interrupt;
#endif

#ifdef   D035_NDFR3030
 	gPluginInterface.cPLUGIN_Install 	= D035_NDFR3030_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D035_NDFR3030_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D035_NDFR3030_Plugin.cPLUGIN_Interrupt;
#endif

#ifdef   D034_AD8000
 	gPluginInterface.cPLUGIN_Install 	= D034_AD8000_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D034_AD8000_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D034_AD8000_Plugin.cPLUGIN_Interrupt;
#endif

#ifdef   D033_HTCK803
 	gPluginInterface.cPLUGIN_Install 	= D033_HTCK803_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D033_HTCK803_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D033_HTCK803_Plugin.cPLUGIN_Interrupt;
#endif	

#ifdef   D032_CZLH128
 	gPluginInterface.cPLUGIN_Install 	= D032_CZLH128_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D032_CZLH128_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D032_CZLH128_Plugin.cPLUGIN_Interrupt;
#endif	

#ifdef   D031_DX200
 	gPluginInterface.cPLUGIN_Install 	= D031_DX200_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D031_DX200_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D031_DX200_Plugin.cPLUGIN_Interrupt;
#endif	

#ifdef   D030_NT8001
 	gPluginInterface.cPLUGIN_Install 	= D030_NT8001_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D030_NT8001_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D030_NT8001_Plugin.cPLUGIN_Interrupt;
#endif		

#ifdef   D029_MB4000
 	gPluginInterface.cPLUGIN_Install 	= D029_MB4000_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D029_MB4000_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D029_MB4000_Plugin.cPLUGIN_Interrupt;
#endif	
	
#ifdef   D028_SJ_YunAn2002
 	gPluginInterface.cPLUGIN_Install 	= D028_SJ_YunAn2002_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D028_SJ_YunAn2002_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D028_SJ_YunAn2002_Plugin.cPLUGIN_Interrupt;
#endif	
	
#ifdef   D027_SJ_YunAn3101
 	gPluginInterface.cPLUGIN_Install 	= D027_SJ_YunAn3101_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D027_SJ_YunAn3101_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D027_SJ_YunAn3101_Plugin.cPLUGIN_Interrupt;
#endif	
	
#ifdef   D026_HW200
 	gPluginInterface.cPLUGIN_Install 	= D026_HW200_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D026_HW200_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D026_HW200_Plugin.cPLUGIN_Interrupt;
#endif	
	
#ifdef   D025_XM_AnDe2008
 	gPluginInterface.cPLUGIN_Install 	= D025_XM_AnDe2008_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D025_XM_AnDe2008_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D025_XM_AnDe2008_Plugin.cPLUGIN_Interrupt;
#endif		
	
	
#ifdef   D024_SJ_YunAn3102
 	gPluginInterface.cPLUGIN_Install 	= D024_SJ_YunAn3102_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D024_SJ_YunAn3102_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D024_SJ_YunAn3102_Plugin.cPLUGIN_Interrupt;
#endif	
	
	
#ifdef   D023_SJ_YunAn2002
 	gPluginInterface.cPLUGIN_Install 	= D023_SJ_YunAn2002_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D023_SJ_YunAn2002_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D023_SJ_YunAn2002_Plugin.cPLUGIN_Interrupt;
#endif	
	

#ifdef   D022_LiDa128E
 	gPluginInterface.cPLUGIN_Install 	= D022_LiDa128E_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D022_LiDa128E_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D022_LiDa128E_Plugin.cPLUGIN_Interrupt;
#endif	

	
#ifdef   D021_SongJiang3208G
 	gPluginInterface.cPLUGIN_Install 	= D021_SongJiang3208G_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D021_SongJiang3208G_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D021_SongJiang3208G_Plugin.cPLUGIN_Interrupt;
#endif		
	
#ifdef   D020_HaiWan9000
 	gPluginInterface.cPLUGIN_Install 	= D020_HaiWan9000_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D020_HaiWan9000_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D020_HaiWan9000_Plugin.cPLUGIN_Interrupt;
#endif		
	 
#ifdef   D019_FA_5050
 	gPluginInterface.cPLUGIN_Install 	= D019_FA_5050_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D019_FA_5050_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D019_FA_5050_Plugin.cPLUGIN_Interrupt;
#endif	

#ifdef   D018_FSR_V6851
 	gPluginInterface.cPLUGIN_Install 	= D018_FSR_V6851_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D018_FSR_V6851_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D018_FSR_V6851_Plugin.cPLUGIN_Interrupt;
#endif	

#ifdef   D017_THA_3016
 	gPluginInterface.cPLUGIN_Install 	= D017_THA_3016_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D017_THA_3016_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D017_THA_3016_Plugin.cPLUGIN_Interrupt;
#endif

#ifdef   D016_DPS_5115
 	gPluginInterface.cPLUGIN_Install 	= D016_DPS_5115_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D016_DPS_5115_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D016_DPS_5115_Plugin.cPLUGIN_Interrupt;
#endif

#ifdef   D015_GTYA_GK601
 	gPluginInterface.cPLUGIN_Install 	= D015_GTYA_GK601_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run 		= D015_GTYA_GK601_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt  = D015_GTYA_GK601_Plugin.cPLUGIN_Interrupt;
#endif

#ifdef   D012_YKSY_YBZ127
 	gPluginInterface.cPLUGIN_Install = D012_YKSY_YBZ127_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run = D012_YKSY_YBZ127_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt =  D012_YKSY_YBZ127_Plugin.cPLUGIN_Interrupt;
#endif
	
#ifdef   D009_JB_QBL_bdqn5012
 	gPluginInterface.cPLUGIN_Install = D009_JB_QBL_bdqn5012_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run = D009_JB_QBL_bdqn5012_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt =  D009_JB_QBL_bdqn5012_Plugin.cPLUGIN_Interrupt;
#endif	
	
#ifdef   D008_JB_QBL_LD128E
 	gPluginInterface.cPLUGIN_Install = D008_JB_QBL_LD128E_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run = D008_JB_QBL_LD128E_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt =  D008_JB_QBL_LD128E_Plugin.cPLUGIN_Interrupt;
#endif

#ifdef   D007_JB_QBL_bdqn11s
 	gPluginInterface.cPLUGIN_Install = D007_JB_QBL_bdqn11s_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run = D007_JB_QBL_bdqn11s_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt =  D007_JB_QBL_bdqn11s_Plugin.cPLUGIN_Interrupt;
#endif
	
#ifdef   D006_YI_6000G
 	gPluginInterface.cPLUGIN_Install = D006_YI_6000G_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run = D006_YI_6000G_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt =  D006_YI_6000G_Plugin.cPLUGIN_Interrupt;
#endif
	
#ifdef   D004_JB_QBL_fs8200
 	gPluginInterface.cPLUGIN_Install = D004_JB_QBL_fs8200_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run = D004_JB_QBL_fs8200_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt =  D004_JB_QBL_fs8200_Plugin.cPLUGIN_Interrupt;
#endif
	
#ifdef   D003_DH9000
 	gPluginInterface.cPLUGIN_Install = D003_DH9000_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run = D003_DH9000_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt =  D003_DH9000_Plugin.cPLUGIN_Interrupt;
#endif
	
#ifdef   D002_JB_QBL_9108
 	gPluginInterface.cPLUGIN_Install = D002_JB_QBL_9108_Plugin.cPLUGIN_Install;
	gPluginInterface.cPLUGIN_Run = D002_JB_QBL_9108_Plugin.cPLUGIN_Run;
	gPluginInterface.cPLUGIN_Interrupt = D002_JB_QBL_9108_Plugin.cPLUGIN_Interrupt;
#endif
 	return gPluginInterface;
}

#endif
