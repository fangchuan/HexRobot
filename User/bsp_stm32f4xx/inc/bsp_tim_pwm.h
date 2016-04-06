/*
*********************************************************************************************************
*
*	模块名称 : 利用STM32F4内部TIM输出PWM信号,驱动18路舵机
*	文件名称 : bsp_tim_pwm.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2016, 阿波罗科技 www.apollorobot.com
*
*********************************************************************************************************
*/

#ifndef __BSP_TIM_PWM_H
#define __BSP_TIM_PWM_H

#include "stm32f4xx.h"

/*
HIP1:PA0,TIM5_CH1
HIP2:PA1,TIM5_CH2
HIP3:PA2,TIM5_CH3
HIP4:PA3,TIM5_CH4
HIP5:PD14,TIM4_CH3
HIP6:PD15,TIM4_CH4

KNEE1:PE13,TIM1_CH3
KNEE2:PE14,TIM1_CH4
KNEE3:PE5, TIM9_CH1
KNEE4:PE6, TIM9_CH2
KNEE5:PE9, TIM1_CH1
KNEE6:PE11,TIM1_CH2

ANKLE1:PC6,TIM3_CH1
ANKLE2:PC7,TIM3_CH2
ANKLE3:PC8,TIM3_CH3
ANKLE4:PC9,TIM3_CH4
ANKLE5:PD12,TIM4_CH1
ANKLE6:PD13,TIM4_CH2
*/
#define  HIP_PORT_CLK    (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOD)
#define  HIP_TIM_CLK     (RCC_APB1Periph_TIM4 | RCC_APB1Periph_TIM5)
#define  HIP_1_Pin        GPIO_Pin_0
#define  HIP_1_PinSource  GPIO_PinSource0
#define  HIP_2_Pin        GPIO_Pin_1
#define  HIP_2_PinSource  GPIO_PinSource1
#define  HIP_3_Pin        GPIO_Pin_2
#define  HIP_3_PinSource  GPIO_PinSource2
#define  HIP_4_Pin        GPIO_Pin_3
#define  HIP_4_PinSource  GPIO_PinSource3
#define  HIP_5_Pin        GPIO_Pin_14
#define  HIP_5_PinSource  GPIO_PinSource14
#define  HIP_6_Pin        GPIO_Pin_15
#define  HIP_6_PinSource  GPIO_PinSource15
#define  HIP_PORT_1       GPIOA
#define  HIP_PORT_AF_1    GPIO_AF_TIM5
#define  HIP_TIM_1        TIM5
#define  HIP_PORT_2       GPIOD
#define  HIP_PORT_AF_2    GPIO_AF_TIM4
#define  HIP_TIM_2        TIM4

#define  KNEE_PORT_CLK   (RCC_AHB1Periph_GPIOE )
#define  KNEE_TIM_CLK    (RCC_APB2Periph_TIM1 | RCC_APB2Periph_TIM9)
#define  KNEE_1_Pin        GPIO_Pin_13
#define  KNEE_1_PinSource  GPIO_PinSource13
#define  KNEE_2_Pin        GPIO_Pin_14
#define  KNEE_2_PinSource  GPIO_PinSource14
#define  KNEE_3_Pin        GPIO_Pin_5
#define  KNEE_3_PinSource  GPIO_PinSource5
#define  KNEE_4_Pin        GPIO_Pin_6
#define  KNEE_4_PinSource  GPIO_PinSource6
#define  KNEE_5_Pin        GPIO_Pin_9
#define  KNEE_5_PinSource  GPIO_PinSource9
#define  KNEE_6_Pin        GPIO_Pin_11
#define  KNEE_6_PinSource  GPIO_PinSource11
#define  KNEE_PORT         GPIOE
#define  KNEE_PORT_AF_1    GPIO_AF_TIM1
#define  KNEE_TIM_1        TIM1
#define  KNEE_PORT_AF_2    GPIO_AF_TIM9
#define  KNEE_TIM_2        TIM9

#define  ANKLE_PORT_CLK  (RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOC)
#define  ANKLE_TIM_CLK   (RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4)
#define  ANKLE_1_Pin        GPIO_Pin_6
#define  ANKLE_1_PinSource  GPIO_PinSource6
#define  ANKLE_2_Pin        GPIO_Pin_7
#define  ANKLE_2_PinSource  GPIO_PinSource7
#define  ANKLE_3_Pin        GPIO_Pin_8
#define  ANKLE_3_PinSource  GPIO_PinSource8
#define  ANKLE_4_Pin        GPIO_Pin_9
#define  ANKLE_4_PinSource  GPIO_PinSource9
#define  ANKLE_5_Pin        GPIO_Pin_12
#define  ANKLE_5_PinSource  GPIO_PinSource12
#define  ANKLE_6_Pin        GPIO_Pin_13
#define  ANKLE_6_PinSource  GPIO_PinSource13
#define  ANKLE_PORT_1       GPIOC
#define  ANKLE_PORT_2       GPIOD
#define  ANKLE_PORT_AF_1    GPIO_AF_TIM3
#define  ANKLE_TIM_1        TIM3
#define  ANKLE_PORT_AF_2    GPIO_AF_TIM4
#define  ANKLE_TIM_2        TIM4

#define  TIMAPB1_Prescaler     83
#define  TIMAPB2_Prescaler     167
#define  TIMPeriod             19999
#define  DefaultPWM            1500

void bsp_SetTIMOutPWM(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, TIM_TypeDef* TIMx, uint8_t _ucChannel,
	 uint32_t _ulFreq, uint32_t _ulDutyCycle);
	 
void bsp_SetTIMOutPWM_N(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, TIM_TypeDef* TIMx, uint8_t _ucChannel,
	 uint32_t _ulFreq, uint32_t _ulDutyCycle);	 

void bsp_SetTIMforInt(TIM_TypeDef* TIMx, uint32_t _ulFreq, uint8_t _PreemptionPriority, uint8_t _SubPriority);

extern void bsp_ServoInit(void);

#endif

/***************************** 阿波罗科技 www.apollorobot.com (END OF FILE) *********************************/
