#include "bsp_FB1000Y.h"
#include "MsgCANProcessAppExt.h"
//#include "CAN1ProtocolParsing.h" 
	
//void TIM3_Int_Init(unsigned short int arr, unsigned short int psc)
//{
//    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//    NVIC_InitTypeDef NVIC_InitStructure;

//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

//    //��ʱ��TIM3��ʼ��
//    TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
//    TIM_TimeBaseStructure.TIM_Prescaler = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
//    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
//    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
//    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

//    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

//    //�ж����ȼ�NVIC����
//    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
//    NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

//    TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx
//}


/*�������*/
void SysSoftReset(void)
{
	printf("�������������\r\n");		
	__set_FAULTMASK(1); 
	NVIC_SystemReset(); 
}
static unsigned int tmr_ms=0;//1S
static unsigned int tmr_sec=0;//1S
static unsigned int tmr_day=0;//1��=86400S
void SystemPeriod_Reset(void)
{
	  tmr_ms++; 
	  if(tmr_ms>100)//1S����
		{  tmr_ms=0;
		   tmr_sec++;	 			
		}	
	  if(tmr_sec>86400)	//1�쵽��	
		{  tmr_sec=0;	
			 tmr_day++;	
		}
	  if(tmr_day>6)	//7�쵽��	
    {	 tmr_day=0;
       SysSoftReset();//�����λ��			
		}
}

//��ʱ��3�жϷ������ 10MS
//void TIM3_IRQHandler(void)   //TIM3�ж�
//{
//    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
//    {
//        TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־
////			  tmrInspect_callback();//CAN���ݽ��ճ�ʱ�жϣ�
//        SystemPeriod_Reset();//ϵͳ����������
//		}
//}
void TIM4_Int_Init(unsigned short int arr, unsigned short int psc);

void bsp_init(void)
{
	delay_init();       //��ʱ��ʼ��
//	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x6000);        //�ж���������ӳ�䡢�ر�Զ������
	delay_ms(1000);     //�˳������ϵ磬��������
	TIM4_Int_Init(100,7200);	//���ڽ���uart1��ʱ����ʼ��,����10mS��
	uart_init(115200);    //���ڲ���������
	LED_Init();         //LED��ʼ��
	KEY_Init();
 // IWDG_Init(IWDG_Prescaler_256, 2500);//�򿪿��Ź�
  Buzzer_Init();
	Voice_Init();
	PowerDetec_Init();
	W25q16_Init();
	RTC_Init();
//	LCD_Init();`
	Net_Init();
	
//  Clear_LCD_Num();20181031
//	ProtocolBoardInterfaceInit();
//	CAN1_Mode_Init(CAN1,gCAN1SystemParameter);   //CANģʽ��ʼ��ʹ��
//	TIM3_Int_Init(100, 7200);//10mS: CAN��ʱ�жϣ�ϵͳ����������ÿ3��������
}
