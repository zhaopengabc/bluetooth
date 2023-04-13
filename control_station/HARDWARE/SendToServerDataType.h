#ifndef __SENDTOSERVERTYPE_H
#define __SENDTOSERVERTYPE_H

//#define SERVER_DATA_NUM 0x400
#define SERVER_DATA_NUM 512
//#define SERVER_DATA_NUM 100
//修改下面宏 必须关注这个函数中的定义->pSendToServerData 
#define SERVER_GROUPNUM 3
#define PRIORITY_GROUPNUM 3
#define MANALARM_GROUPNUM 3

typedef struct
{
    volatile unsigned char Flag;
	volatile unsigned int Lenth;
	volatile unsigned char data[SERVER_DATA_NUM];
}Server_Data_Typedef;

Server_Data_Typedef * IsReportAddr(unsigned char *data,unsigned int lenth,unsigned char *pServer_Rd_Addr);
void Reset_RdSendToServerData(Server_Data_Typedef *pSendToServer_Data,unsigned char *pSendToServer_Rd_Addr);
unsigned int Data_TransToServer(unsigned char *data,unsigned int lenth);//放入发送暂存区
Server_Data_Typedef *pSendToServerData(unsigned char **pRd_Addr);		//从发送暂存区读取

extern unsigned char DataSendToStatus(void);
extern unsigned char SendToServerData_Reset(unsigned int num);
#endif


