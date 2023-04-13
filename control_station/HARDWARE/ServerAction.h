#ifndef __SERVERACTION_H
#define __SERVERACTION_H
#include "SendToServerDataType.h"

unsigned char IsEquel(unsigned int num);
void IsOnDuty_Action(void);

void Write_LastFrameCnt(unsigned int FrameCnt);
void SendSuccessedAction(void);
void SendFailedAction(void);

unsigned char Server_Action(Server_Data_Typedef *pServerData,unsigned char *pServer_Rd_Addr);
void Reste_Key(void);
void FaultMute_Key(void);
void SystemMod_Key(void);
unsigned char SelfCheck_Key(void);
void JingQingXiaoChu_Key(void);
void GuZhangXiaoChu_Key(void);
void ManAlarm_Key(void);
void Duty_Key(void);

void SelfCheckStep1(void);
void SelfCheckStep2(void);
void SelfCheckStep3(void);
void NetCHECK(void);
void SelfCheckStep4(void);
void RunState_Action(unsigned char RunState);

unsigned char UpdataRunState(void);
void RunningStateFrame(void);
void HeartBeatFrame(void);

extern void LastTx_Frame_Increase(void);
extern void VoiceActionClear(void);
extern void ChaGangYingDa_stop(void);
#endif

