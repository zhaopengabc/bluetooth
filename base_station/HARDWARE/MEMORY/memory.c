#include "memory.h"



//����һ���洢��
OS_MEM STORAGE_MEM;	
//�洢���д洢������
#define STORAGE_MEM_NUM		10
//ÿ���洢���С
//����һ��ָ�����ռ��4�ֽ����Կ�Ĵ�Сһ��ҪΪ4�ı���
//���ұ������һ��ָ�����(4�ֽ�)ռ�õĿռ�,����Ļ��洢�鴴�����ɹ�
#define STORAGE_MEMBLOCK_SIZE	404
//�洢�����ڴ�أ�ʹ���ڲ�RAM
CPU_INT08U Storage_RamMemp[STORAGE_MEM_NUM][STORAGE_MEMBLOCK_SIZE];

//#define BT_Q_NUM 20
OS_Q BT_Msg;


void POST(OS_Q* Original_MSG,u32 length,u8* str,OS_MEM* memory)
{
    OS_ERR err;
    CPU_INT08U *storage_buf;
    u16 null_size;
    u8  *null_buf;
    storage_buf=OSMemGet((OS_MEM*)memory,//&STORAGE_MEM,
                         (OS_ERR*)&err);
    if(err == OS_ERR_NONE) //�ڴ�����ɹ�
	{
       memcpy((u8*)storage_buf,str,length);
       OSQPost((OS_Q*		)Original_MSG,		
			   (void*		)storage_buf,
			   (OS_MSG_SIZE )length,
			   (OS_OPT		)OS_OPT_POST_FIFO,
			   (OS_ERR*	)&err);
                    //ResetUartBuf(&uart_rs485);
	}
	if((err == OS_ERR_MEM_NO_FREE_BLKS)||(memory->NbrFree<=1)) //�ڴ�鲻��
	{
		null_buf=OSQPend((OS_Q*		    )Original_MSG,   
				         (OS_TICK	    )0,
                         (OS_OPT	    )OS_OPT_PEND_NON_BLOCKING,
                         (OS_MSG_SIZE*	)&null_size,	
                         (CPU_TS*		)0,
                         (OS_ERR*		)&err);
        OSMemPut((OS_MEM*	)memory,		//�ͷ��ڴ�
				 (void*		)null_buf,
				 (OS_ERR* 	)&err);
	}
}
u32 PEND(OS_Q* Original_MSG,u8* str,OS_MEM* memory)
{
    u8 *DATA_BUF;
    u16 data_size;
    OS_ERR err;
    DATA_BUF=OSQPend((OS_Q*		    )Original_MSG,   
				     (OS_TICK		)0,
                     (OS_OPT		)OS_OPT_PEND_NON_BLOCKING,
                     (OS_MSG_SIZE*	)&data_size,	
                     (CPU_TS*		)0,
                     (OS_ERR*		)&err);
    if((data_size>0)&&(data_size<STORAGE_MEMBLOCK_SIZE))
    {
        memcpy(str,DATA_BUF,data_size);
        //Gprs_Send_buf.Buffer_length=data_size;
        //Gprs_Send_buf.Buffer_en=1;
        OSMemPut((OS_MEM*	)memory,		//�ͷ��ڴ�
                (void*		)DATA_BUF,
                (OS_ERR* 	)&err);
        return data_size;
    }
    else return 0;
}

u32 Creat_Memory(void)
{
    OS_ERR err;
    OSMemCreate((OS_MEM*	)&STORAGE_MEM,
				(CPU_CHAR*	)"Storage Mem",
				(void*		)&Storage_RamMemp[0][0],
				(OS_MEM_QTY	)STORAGE_MEM_NUM,
				(OS_MEM_SIZE)STORAGE_MEMBLOCK_SIZE,
				(OS_ERR*	)&err);
}
u32 Create_Msg(void)
{
    OS_ERR err;
    OSQCreate ((OS_Q*		)&BT_Msg,	
                (CPU_CHAR*	)"BT Msg",	
                (OS_MSG_QTY	)BT_Q_NUM,	
                (OS_ERR*	)&err);	
    
}



