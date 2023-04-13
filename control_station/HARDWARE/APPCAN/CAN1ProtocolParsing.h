#ifndef _CAN1ProtocolParsing_h_
#define _CAN1ProtocolParsing_h_


/********************************************************
//					�궨��
********************************************************/
#define  RegYes				1        	//��ע��
#define  RegNo				0			//δע��

#define  ReceiveTimeOut		20			//�ȴ���ʱ����ʱ��
#define	 ProtocolBoard_CNT	16			//Э�������

/********************************************************
//					�ṹ�嶨��
********************************************************/

//ע����Ϣ���սṹ��
typedef struct
{	
    unsigned char  	Board_Type;   					//Э�������
    unsigned char  	Hardware_Version[20];   		//Ӳ���汾
	unsigned char  	Software_Version[40];   		//����汾
	unsigned char 	BT_Version[30];   				//BT�汾
	unsigned char  	Host_Interface[20];   			//��������ӿ�	
	unsigned char  	Host_Machine_Type[20];   		//�����ͺ�
	unsigned char  	Reserved_Date[10];   			//����
//	unsigned char  	TimeData[6];   					//ʱ��
	
}Registered_InforDataStr;



//Э��巢��Ѳ��״̬�ṹ��
typedef	struct
{
	unsigned char ProtocolBoardRegistered_Operation;			//��ǰЭ���ע�����״̬
	unsigned char ProtocolBoardRegistered_SendTimes;			//��ǰЭ���ע�ᷢ�ʹ���
	
	unsigned char ProtocolBoardRegisteredSendState;				//ע�ᷢ��״̬
	unsigned char ProtocolBoardRegisteredReceiveTimOut;			//���ճ�ʱ
	unsigned char ProtocolBoardRegisteredReceiveOK;				//�������
}RegisteredState;




//Э���ע�����ṹ��
typedef  union
{
	unsigned char Reg_data[15];
	struct
	{
		unsigned char Registered_OneIsOrNo;
		unsigned char Registered_TwoIsOrNo;
		unsigned char Registered_ThreeIsOrNo;
		unsigned char Registered_FourIsOrNo;
		unsigned char Registered_FiveIsOrNo;
		unsigned char Registered_SixIsOrNo;
		unsigned char Registered_SevenIsOrNo;
		unsigned char Registered_EightIsOrNo;
		unsigned char Registered_nineIsOrNo;
		unsigned char Registered_TenIsOrNo;
		unsigned char Registered_ElevenIsOrNo;
		unsigned char Registered_TwelveIsOrNo;
		unsigned char Registered_ThirteenIsOrNo;
		unsigned char Registered_FourteenIsOrNo;
		unsigned char Registered_FifteenIsOrNo;
	}Regis;
}ProtocolBoardRegistered;






//Э��巢��Ѳ��״̬�ṹ��
typedef	struct
{
	unsigned char ProtocolBoardInspectSendState;			//Ѳ�췢��״̬
	unsigned char ProtocolBoardInspectReceiveTimOut;		//���ճ�ʱ
	unsigned char ProtocolBoardInspectReceiveOK;			//�������
}InspectState;









//�ڲ���������
extern InspectState S_InspectState[15];					//Ѳ��״̬�ṹ������
extern unsigned int Inspect_TimCount;					//Ѳ�������ʱ������
extern unsigned char Registered_Start;				//����ע��
extern unsigned int CanLineCount;	//CAN�߼�����
extern unsigned char PriorityDataFlag;//�𾯿ռ䣻
extern unsigned char Data_SendToServerFlag;//�𾯿ռ䣻

extern unsigned char Send_InspectionToProtocolBoard(unsigned char BoardNumber);   		//����Ѳ�캯��
extern unsigned char Send_RegisterToProtocolBoard(unsigned char BoardNumber);   		//����ע��ָ���
extern unsigned char Send_ConfirmToProtocolBoard(unsigned char BoardNumber);   			//����ȷ��ָ���
extern void tmrInspect_callback(void);    //Ѳ�춨ʱ���ص�����
extern unsigned char ThreeInOne_ProtocolBoard_Run(void);   		//Э������һ��CANЭ��ִ�к���
extern void ProID_Init(void);
extern unsigned int MClineFlaut_flag;
extern unsigned char CAN_LINE_CHECK(void);
#endif
