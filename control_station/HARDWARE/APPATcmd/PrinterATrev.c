#include "PrinterATrev.h"
#include "stm32f10x.h" 
#include <string.h>
#include <stdio.h> 

#define UART1_RX_DATANUM 200
#define TASK_UART1_CNT 4  //40mS���ݽ��յȴ���
#define UART1_RX_GROUPNUM 3

typedef struct
{
  unsigned char Flag;
	unsigned int Lenth;
	unsigned char data[UART1_RX_DATANUM];
}Uart1_Data_Typedef;
static Uart1_Data_Typedef Uart1RxBuf[UART1_RX_GROUPNUM];
static unsigned char Uart1_Rx_Cnt=0;
static unsigned char Uart1_Wr_Addr=0;//дλ�ã�
static unsigned char Uart1_Rd_Addr=0;

//����uart1�������ݼ�ʱ����	
void TIM4_Int_Init(unsigned short int arr, unsigned short int psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�
	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx
}

//�����ں����е��ã��Զ�ȡ��ַ��д��ַ���и���
void Uart1_TimCnt(void)
{
	if(Uart1_Rx_Cnt!=0)//���յ��ֽڣ�
	{
			 Uart1_Rx_Cnt--;//��ȥ1��
		if(Uart1_Rx_Cnt==0)//����0����ʾһ֡������ϣ�
		{
			Uart1RxBuf[Uart1_Wr_Addr].Flag=1;//��ȡһ��֡�ɹ���ɣ�
			Uart1_Wr_Addr++;//�洢��һ֡����ţ�
			if(Uart1_Wr_Addr>=UART1_RX_GROUPNUM)Uart1_Wr_Addr=0;//����3����λ��
			if(Uart1_Rd_Addr==Uart1_Wr_Addr)
			{
				Uart1_Rd_Addr++;
				if(Uart1_Rd_Addr>=UART1_RX_GROUPNUM)Uart1_Rd_Addr=0;
			}
		}
	}
}
//��ʱ��4�жϷ������
void TIM4_IRQHandler(void)   //TIM3�ж�
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		    Uart1_TimCnt(); 
		}
}

void Reset_Uart1RxBuf(void)
{
	unsigned char i;
	Uart1_Rd_Addr=0;
	Uart1_Wr_Addr=0;
	for(i=0;i<UART1_RX_GROUPNUM;i++)
	{
				Uart1RxBuf[i].Flag=0;
        Uart1RxBuf[i].Lenth=0;
        memset(Uart1RxBuf[i].data,0,UART1_RX_DATANUM);
	}
}

//���жϺ����е��ã��Զ��洢
void Uart1_RxISR(unsigned char buf)
{
    Uart1_Rx_Cnt=TASK_UART1_CNT;//��λ�ֽڶ�ȡ����ʱʱ�䣻
	
    Uart1RxBuf[Uart1_Wr_Addr].Flag=0;//���㣻
	
 if(Uart1RxBuf[Uart1_Wr_Addr].Lenth>=UART1_RX_DATANUM-1) //���ȳ���;
		{Uart1RxBuf[Uart1_Wr_Addr].Lenth=0;}//��λ��
 
    Uart1RxBuf[Uart1_Wr_Addr].data[Uart1RxBuf[Uart1_Wr_Addr].Lenth++] = buf;//װ�
}
//����UART���ݽ����жϣ�
void USART1_IRQHandler(void)
{
  unsigned char data; 
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
			USART_ClearITPendingBit(USART1, USART_IT_RXNE);
			data=USART1->DR;
			Uart1_RxISR(data);
    }	
}
//�������ݶ�ȡ��
unsigned int Uart1_Rx_Read(unsigned char *Rxdata)
{
	unsigned int tmp=0;
	if(Uart1RxBuf[Uart1_Rd_Addr].Flag==1)//�������buff���������ݣ�
    {
				//��ȡ����
				tmp=Uart1RxBuf[Uart1_Rd_Addr].Lenth;//��ȡ���ȣ�
				memcpy(Rxdata,Uart1RxBuf[Uart1_Rd_Addr].data,Uart1RxBuf[Uart1_Rd_Addr].Lenth);//��ȡ���ݣ�
        Uart1RxBuf[Uart1_Rd_Addr].Flag=0;//���㣻
        Uart1RxBuf[Uart1_Rd_Addr].Lenth=0;//���㣻
        memset(Uart1RxBuf[Uart1_Rd_Addr].data,0,UART1_RX_DATANUM);//���㣻
        Uart1_Rd_Addr++;//�´ζ�ȡ��
        if(Uart1_Rd_Addr>=UART1_RX_GROUPNUM)Uart1_Rd_Addr=0;//���㣻
    }
	return tmp;
}