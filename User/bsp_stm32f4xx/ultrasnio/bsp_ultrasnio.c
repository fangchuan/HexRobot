/*
*********************************************************************************************************
*
*	ģ������ : ͨ�ú���ģ��
*	�ļ����� : bsp_ultrasnio.c
*	��    �� : V1.0
*	˵    �� : 
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2016-04-2 ����  ��ʽ����
*
*	Copyright (C), 2015-2020, �����޿Ƽ� www.apollorobot.com
*
*********************************************************************************************************
*/

#include  "bsp_ultrasnio.h"
#include  "os.h"
#include  "common.h"
/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
uint8_t  TIM12CH1_CAPTURE_STA=1;  //���ز�׽��־λ��=1��ʾ��׽���������أ�=0��ʾ��׽�����½���
uint8_t  TIM12CH2_CAPTURE_STA=1;
uint16_t TIM12CH1_Rise,TIM12CH1_Fall,TIM12CH2_Rise,TIM12CH2_Fall;
uint16_t  TIM12_T;

/*********************************************************************
*
*       Global data
*
**********************************************************************
*/
_Ultrasnio ult;//��ǰ��þ��룬��λΪcm

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
//TIM12�ж�����
static void NVIC_TIM12_Configuration(void)
{ 
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the TIM12 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM8_BRK_TIM12_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_ULTRASNIO_PP;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_ULTRASNIO_SP;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
/**************************ʵ�ֺ���************************************
*����ԭ��: TIM12_PWMInCap_Init
*��    ��: PB14\PB15���ճ��������ظߵ�ƽ
*��������: ����TIM12Ϊ���벶��ģʽ������PWM����ģʽ��		
***********************************************************************/
static void TIM12_PWMInCap_Init(void)
{	 
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		TIM_ICInitTypeDef        TIM12_ICInitStructure;

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);	//ʹ��TIM12ʱ��
		
		//��ʼ��TIM8��������Ϊ1us~65.535ms֮��	 
		TIM_TimeBaseStructure.TIM_Period = 0xffff;     //�趨�������Զ���װֵ 
		TIM_TimeBaseStructure.TIM_Prescaler = 167 ; 	//�ҵ���Ƶ�ܵ�72Mhz��Ԥ��Ƶ��72��Ƶ=1Mhz��1us��һ����   
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����Ƶ
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
		TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); //��ʼ��TIM8����
	 
		//��ʼ��TIM12���벶�����,PB14
		TIM12_ICInitStructure.TIM_Channel = US_1_ECHO_CHANNEL; 
  	TIM12_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�������ز���
  	TIM12_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  	TIM12_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
  	TIM12_ICInitStructure.TIM_ICFilter = 0x0A;//IC1F=1010 �˵�1us���������ȵĸ���
  	TIM_ICInit(TIM12, &TIM12_ICInitStructure);
	  TIM_ITConfig(TIM12,TIM_IT_CC1,ENABLE);//����CC1IE���ز����ж�
	
		//��ʼ��TIM12���벶�����,PB15
		TIM12_ICInitStructure.TIM_Channel = US_2_ECHO_CHANNEL; 
  	TIM12_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�������ز���
  	TIM12_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI2��
  	TIM12_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
  	TIM12_ICInitStructure.TIM_ICFilter = 0x0A;//IC1F=1010 �˵�1us���������ȵĸ���
  	TIM_ICInit(TIM12, &TIM12_ICInitStructure);
		TIM_ITConfig(TIM12,TIM_IT_CC2,ENABLE);//����CC2IE���ز����ж�
			 
		NVIC_TIM12_Configuration();


		TIM_Cmd(TIM12,ENABLE ); 	//ʹ�ܶ�ʱ��8
}
//
static void Ultrasnio_Port_Init(void)
{
      GPIO_InitTypeDef  GPIO_InitStructure;

			/* ����GPIO */
			GPIO_InitStructure.GPIO_Pin = ULTRASNIO_ECHO_1 | ULTRASNIO_ECHO_2;	/* ������β� */
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;
			GPIO_Init(ULTRASNIO_PORT, &GPIO_InitStructure);

			/* ���ӵ�AF���� */
	    GPIO_PinAFConfig(ULTRASNIO_PORT, ECHO_1_PinSource, ULTRASNIO_AF_TIM);
			GPIO_PinAFConfig(ULTRASNIO_PORT, ECHO_2_PinSource, ULTRASNIO_AF_TIM);
	 
	    GPIO_InitStructure.GPIO_Pin = ULTRASNIO_TRIG_1 | ULTRASNIO_TRIG_2;	/* ������β� */
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
			GPIO_Init(ULTRASNIO_PORT, &GPIO_InitStructure);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
//��������ʼ��
void bsp_ultrasnio_init(void)
{
			Ultrasnio_Port_Init();
			TIM12_PWMInCap_Init();
}
//����һ��Trig���ţ�����һ�β�����
void Ultrasnio_1_update(void)
{	  
     Ultrasnio_Trigger1_H;
     bsp_DelayUS(40);//ʾ����������ʱ��ʱΪ40us
		 Ultrasnio_Trigger1_L;
}

void Ultrasnio_2_update(void)
{	  
     Ultrasnio_Trigger2_H;
     bsp_DelayUS(40);//ʾ����������ʱ��ʱΪ40us
		 Ultrasnio_Trigger2_L;
}
/**
  * @������ECHO���Ÿ��ݽ��ܵ��ĸߵ�ƽʱ����¾���
  */ 

void TIM8_BRK_TIM12_IRQHandler(void)
{  //�����ж�
	 CPU_SR_ALLOC();
	 CPU_CRITICAL_ENTER();
	 OSIntEnter();
	 CPU_CRITICAL_EXIT();
	
	  if (TIM_GetITStatus(TIM12, TIM_IT_CC1) != RESET)//ch1���������¼�
		{	
				TIM_ClearITPendingBit(TIM12, TIM_IT_CC1);   //����жϱ�־λ

				if(TIM12CH1_CAPTURE_STA == 1)	//����������
				{ 
						TIM12CH1_Rise = TIM_GetCapture1(TIM12);		           //��ȡ�����ص�����	  
						TIM12CH1_CAPTURE_STA = 0;		                        //ת����־λΪ�½���
						TIM_OC1PolarityConfig(TIM12,TIM_ICPolarity_Falling);		//����Ϊ�½��ز���	  			    			  
				}
				else  						    //�����½���
				{
						TIM12CH1_Fall = TIM_GetCapture1(TIM12);	      //��ȡ�½��ص�����	  
						TIM12CH1_CAPTURE_STA = 1;		//ת����־λΪ������
						if(TIM12CH1_Fall < TIM12CH1_Rise)     //���1����ʾ�ߵ�ƽ�����65535�������ֵ����ʱ״̬Ϊ�����ؽӽ�65535�������½��س�����65535��0��ʼ���㣬Tim8�����
						{
							TIM12_T = 65535;
						}
						else  //���2����ʾ��������������غ��½��ض���0-65535֮�䣬���½�����ֵ>��������ֵ��
						{
							TIM12_T = 0;
						}	
						ult.left_distance = (float)((TIM12CH1_Fall - TIM12CH1_Rise + TIM12_T)*0.018);  //�õ��ܵĸߵ�ƽʱ�䣬ֵ��
						TIM_OC1PolarityConfig(TIM12,TIM_ICPolarity_Rising); //CC4P=0 ����Ϊ�����ز���		
				}		    
		}
		if (TIM_GetITStatus(TIM12, TIM_IT_CC2) != RESET)//ch4���������¼�
		{	
				TIM_ClearITPendingBit(TIM12, TIM_IT_CC2);   //����жϱ�־λ

				if(TIM12CH2_CAPTURE_STA == 1)	//����������
				{ 
						TIM12CH2_Rise = TIM_GetCapture2(TIM12);		           //��ȡ�����ص�����	  
						TIM12CH2_CAPTURE_STA = 0;		                        //ת����־λΪ�½���
						TIM_OC2PolarityConfig(TIM12,TIM_ICPolarity_Falling);		//����Ϊ�½��ز���	  			    			  
				}
				else  						    //�����½���
				{
						TIM12CH2_Fall = TIM_GetCapture2(TIM12);	      //��ȡ�½��ص�����	  
						TIM12CH2_CAPTURE_STA = 1;		//ת����־λΪ������
						if(TIM12CH2_Fall < TIM12CH2_Rise)     //���1����ʾ�ߵ�ƽ�����65535�������ֵ����ʱ״̬Ϊ�����ؽӽ�65535�������½��س�����65535��0��ʼ���㣬Tim8�����
						{
							TIM12_T = 65535;
						}
						else  //���2����ʾ��������������غ��½��ض���0-65535֮�䣬���½�����ֵ>��������ֵ��
						{
							TIM12_T = 0;
						}	
						ult.right_distance = (float)((TIM12CH2_Fall - TIM12CH2_Rise + TIM12_T)*0.018);  //�õ��ܵĸߵ�ƽʱ�䣬ֵ��
						TIM_OC2PolarityConfig(TIM12,TIM_ICPolarity_Rising); //CC4P=0 ����Ϊ�����ز���		
				}		    
		}
		//�˳��ж�
		OSIntExit();
}

/***************************** �����޿Ƽ� www.apollorobot.com (END OF FILE) *********************************/
