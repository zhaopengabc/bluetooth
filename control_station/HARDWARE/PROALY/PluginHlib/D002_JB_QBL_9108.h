#include <includes.h>
#include "DeviceCommon.h"
//#include "MsgCANProcessAppExt.h"
#include <string.h>
 

#define SystemStatusPackageLength (9u) 
#define DeviceStatusPackageLength (13u) 
#define OperationPackageLength    (2u) 

typedef struct
{
	unsigned char Data[SystemStatusPackageLength];  
} SystemStatusPackageType;

typedef struct
{
	unsigned char Data[DeviceStatusPackageLength];  
} DeviceStatusPackageType;

typedef struct
{
	unsigned char Data[OperationPackageLength];  
} OperationStatusPackageType;


extern PluginInterface D002_JB_QBL_9108_Plugin;



