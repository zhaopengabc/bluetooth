#ifndef __NET_H
#define __NET_H

void Net_Init(void);
unsigned int Net_Send_Infor(unsigned char *data,unsigned int Lenth);
unsigned int Net_Rx_Read(unsigned char *Rxdata);
unsigned char NetWork_State(void);
void Net_Restart(void);
unsigned char NetStatus(void);
	
extern volatile unsigned int NetHeart_Cont; 
extern volatile unsigned int NetHeart_SndCot; 
extern volatile unsigned char ResumeNbr;
#endif
