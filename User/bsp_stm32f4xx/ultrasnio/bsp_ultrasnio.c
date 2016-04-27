/*
*********************************************************************************************************
*
*	模块名称 : 通用函数模块
*	文件名称 : bsp_ultrasnio.c
*	版    本 : V1.0
*	说    明 : 
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2016-04-2 方川  正式发布
*
*	Copyright (C), 2015-2020, 阿波罗科技 www.apollorobot.com
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
uint8_t  TIM12CH1_CAPTURE_STA=1;  //边沿捕捉标志位，=1表示捕捉到了上升沿，=0表示捕捉到了下降沿
uint8_t  TIM12CH2_CAPTURE_STA=1;
uint16_t TIM12CH1_Rise,TIM12CH1_Fall,TIM12CH2_Rise,TIM12CH2_Fall;
uint16_t  TIM12_T;

/*********************************************************************
*
*       Global data
*
**********************************************************************
*/
_Ultrasnio ult;//当前测得距离，单位为cm

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
//TIM12中断配置
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
/**************************实现函数************************************
*函数原型: TIM12_PWMInCap_Init
*引    脚: PB14\PB15接收超声波返回高电平
*功　　能: 配置TIM12为输入捕获模式，不是PWM输入模式。		
***********************************************************************/
static void TIM12_PWMInCap_Init(void)
{	 
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		TIM_ICInitTypeDef        TIM12_ICInitStructure;

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);	//使能TIM12时钟
		
		//初始化TIM8，脉冲宽度为1us~65.535ms之间	 
		TIM_TimeBaseStructure.TIM_Period = 0xffff;     //设定计数器自动重装值 
		TIM_TimeBaseStructure.TIM_Prescaler = 167 ; 	//我的主频跑到72Mhz，预分频器72分频=1Mhz，1us计一个数   
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //不分频
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
		TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); //初始化TIM8参数
	 
		//初始化TIM12输入捕获参数,PB14
		TIM12_ICInitStructure.TIM_Channel = US_1_ECHO_CHANNEL; 
  	TIM12_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//先上升沿捕获
  	TIM12_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  	TIM12_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  	TIM12_ICInitStructure.TIM_ICFilter = 0x0A;//IC1F=1010 滤掉1us以下脉冲宽度的干扰
  	TIM_ICInit(TIM12, &TIM12_ICInitStructure);
	  TIM_ITConfig(TIM12,TIM_IT_CC1,ENABLE);//允许CC1IE边沿捕获中断
	
		//初始化TIM12输入捕获参数,PB15
		TIM12_ICInitStructure.TIM_Channel = US_2_ECHO_CHANNEL; 
  	TIM12_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//先上升沿捕获
  	TIM12_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI2上
  	TIM12_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  	TIM12_ICInitStructure.TIM_ICFilter = 0x0A;//IC1F=1010 滤掉1us以下脉冲宽度的干扰
  	TIM_ICInit(TIM12, &TIM12_ICInitStructure);
		TIM_ITConfig(TIM12,TIM_IT_CC2,ENABLE);//允许CC2IE边沿捕获中断
			 
		NVIC_TIM12_Configuration();


		TIM_Cmd(TIM12,ENABLE ); 	//使能定时器8
}
//
static void Ultrasnio_Port_Init(void)
{
      GPIO_InitTypeDef  GPIO_InitStructure;

			/* 配置GPIO */
			GPIO_InitStructure.GPIO_Pin = ULTRASNIO_ECHO_1 | ULTRASNIO_ECHO_2;	/* 带入的形参 */
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;
			GPIO_Init(ULTRASNIO_PORT, &GPIO_InitStructure);

			/* 连接到AF功能 */
	    GPIO_PinAFConfig(ULTRASNIO_PORT, ECHO_1_PinSource, ULTRASNIO_AF_TIM);
			GPIO_PinAFConfig(ULTRASNIO_PORT, ECHO_2_PinSource, ULTRASNIO_AF_TIM);
	 
	    GPIO_InitStructure.GPIO_Pin = ULTRASNIO_TRIG_1 | ULTRASNIO_TRIG_2;	/* 带入的形参 */
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
//超声波初始化
void bsp_ultrasnio_init(void)
{
			Ultrasnio_Port_Init();
			TIM12_PWMInCap_Init();
}
//触发一次Trig引脚，进行一次测距更新
void Ultrasnio_1_update(void)
{	  
     Ultrasnio_Trigger1_H;
     bsp_DelayUS(40);//示波器测量此时延时为40us
		 Ultrasnio_Trigger1_L;
}

void Ultrasnio_2_update(void)
{	  
     Ultrasnio_Trigger2_H;
     bsp_DelayUS(40);//示波器测量此时延时为40us
		 Ultrasnio_Trigger2_L;
}
/**
  * @超声波ECHO引脚根据接受到的高电平时间更新距离
  */ 

void TIM8_BRK_TIM12_IRQHandler(void)
{  //进入中断
	 CPU_SR_ALLOC();
	 CPU_CRITICAL_ENTER();
	 OSIntEnter();
	 CPU_CRITICAL_EXIT();
	
	  if (TIM_GetITStatus(TIM12, TIM_IT_CC1) != RESET)//ch1发生捕获事件
		{	
				TIM_ClearITPendingBit(TIM12, TIM_IT_CC1);   //清除中断标志位

				if(TIM12CH1_CAPTURE_STA == 1)	//捕获到上升沿
				{ 
						TIM12CH1_Rise = TIM_GetCapture1(TIM12);		           //获取上升沿的数据	  
						TIM12CH1_CAPTURE_STA = 0;		                        //转换标志位为下降沿
						TIM_OC1PolarityConfig(TIM12,TIM_ICPolarity_Falling);		//设置为下降沿捕获	  			    			  
				}
				else  						    //捕获到下降沿
				{
						TIM12CH1_Fall = TIM_GetCapture1(TIM12);	      //获取下降沿的数据	  
						TIM12CH1_CAPTURE_STA = 1;		//转换标志位为上升沿
						if(TIM12CH1_Fall < TIM12CH1_Rise)     //情况1：表示高电平跨过了65535这个特殊值，此时状态为上升沿接近65535，并且下降沿超过了65535从0开始计算，Tim8有溢出
						{
							TIM12_T = 65535;
						}
						else  //情况2：表示正常情况，上升沿和下降沿都在0-65535之间，且下降沿数值>上升沿数值。
						{
							TIM12_T = 0;
						}	
						ult.left_distance = (float)((TIM12CH1_Fall - TIM12CH1_Rise + TIM12_T)*0.018);  //得到总的高电平时间，值域
						TIM_OC1PolarityConfig(TIM12,TIM_ICPolarity_Rising); //CC4P=0 设置为上升沿捕获		
				}		    
		}
		if (TIM_GetITStatus(TIM12, TIM_IT_CC2) != RESET)//ch4发生捕获事件
		{	
				TIM_ClearITPendingBit(TIM12, TIM_IT_CC2);   //清除中断标志位

				if(TIM12CH2_CAPTURE_STA == 1)	//捕获到上升沿
				{ 
						TIM12CH2_Rise = TIM_GetCapture2(TIM12);		           //获取上升沿的数据	  
						TIM12CH2_CAPTURE_STA = 0;		                        //转换标志位为下降沿
						TIM_OC2PolarityConfig(TIM12,TIM_ICPolarity_Falling);		//设置为下降沿捕获	  			    			  
				}
				else  						    //捕获到下降沿
				{
						TIM12CH2_Fall = TIM_GetCapture2(TIM12);	      //获取下降沿的数据	  
						TIM12CH2_CAPTURE_STA = 1;		//转换标志位为上升沿
						if(TIM12CH2_Fall < TIM12CH2_Rise)     //情况1：表示高电平跨过了65535这个特殊值，此时状态为上升沿接近65535，并且下降沿超过了65535从0开始计算，Tim8有溢出
						{
							TIM12_T = 65535;
						}
						else  //情况2：表示正常情况，上升沿和下降沿都在0-65535之间，且下降沿数值>上升沿数值。
						{
							TIM12_T = 0;
						}	
						ult.right_distance = (float)((TIM12CH2_Fall - TIM12CH2_Rise + TIM12_T)*0.018);  //得到总的高电平时间，值域
						TIM_OC2PolarityConfig(TIM12,TIM_ICPolarity_Rising); //CC4P=0 设置为上升沿捕获		
				}		    
		}
		//退出中断
		OSIntExit();
}

/***************************** 阿波罗科技 www.apollorobot.com (END OF FILE) *********************************/
