/*
*********************************************************************************************************
*
*	模块名称 : TIM基本定时中断和PWM驱动模块
*	文件名称 : bsp_tim_pwm.c
*	版    本 : V1.4
*	说    明 : 利用STM32F4内部TIM输出PWM信号， 并实现基本的定时中断
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2013-08-16 armfly  正式发布
*		V1.1	2014-06-15 armfly  完善 bsp_SetTIMOutPWM，当占空比=0和100%时，关闭定时器，GPIO配置为输出
*		V1.2	2015-05-08 armfly  解决TIM8不能输出PWM的问题。
*		V1.3	2015-07-23 armfly  初始化定时器，必须设置 TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x0000;		
*								   TIM1 和 TIM8 必须设置。否则蜂鸣器的控制不正常。
*		V1.4	2015-07-30 armfly  增加反相引脚输出PWM函数 bsp_SetTIMOutPWM_N();
*
*	Copyright (C), 2015-2020, 阿波罗科技 www.apollorobot.com
*
*********************************************************************************************************
*/

#include "bsp.h"


/*
	可以输出到GPIO的TIM通道:

	TIM1_CH1, PA8,	PE9,
	TIM1_CH2, PA9,	PE11
	TIM1_CH3, PA10,	PE13
	TIM1_CH4, PA11,	PE14

	TIM2_CH1, PA15 (仅限429，439) 407没有此脚
	TIM2_CH2, PA1,	PB3
	TIM2_CH3, PA2,	PB10
	TIM2_CH4, PA3,	PB11

	TIM3_CH1, PA6,  PB4, PC6
	TIM3_CH2, PA7,	PB5, PC7
	TIM3_CH3, PB0,	PC8
	TIM3_CH4, PB1,	PC9

	TIM4_CH1, PB6,  PD12
	TIM4_CH2, PB7,	PD13
	TIM4_CH3, PB8,	PD14
	TIM4_CH4, PB9,	PD15

	TIM5_CH1, PA0,  PH10
	TIM5_CH2, PA1,	PH11
	TIM5_CH3, PA2,	PH12
	TIM5_CH4, PA3,	PI10

	TIM8_CH1, PC6,  PI5
	TIM8_CH2, PC7,	PI6
	TIM8_CH3, PC8,	PI7
	TIM8_CH4, PC9,	PI2

	TIM9_CH1, PA2,  PE5
	TIM9_CH2, PA3,	PE6

	TIM10_CH1, PB8,  PF6

	TIM11_CH1, PB9,  PF7

	TIM12_CH1, PB14,  PH6
	TIM12_CH2, PB15,  PH9

	TIM13_CH1, PA6,  PF8
	TIM14_CH1, PA7,  PF9

	APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14 
	APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11
	

	APB1 定时器的输入时钟 TIMxCLK = SystemCoreClock / 2; 84M
	APB2 定时器的输入时钟 TIMxCLK = SystemCoreClock; 168M
*/

/*
*********************************************************************************************************
*	函 数 名: bsp_GetRCCofGPIO
*	功能说明: 根据GPIO 得到RCC寄存器
*	形    参：无
*	返 回 值: GPIO外设时钟名
*********************************************************************************************************
*/
uint32_t bsp_GetRCCofGPIO(GPIO_TypeDef* GPIOx)
{
	uint32_t rcc = 0;

	if (GPIOx == GPIOA)
	{
		rcc = RCC_AHB1Periph_GPIOA;
	}
	else if (GPIOx == GPIOB)
	{
		rcc = RCC_AHB1Periph_GPIOB;
	}
	else if (GPIOx == GPIOC)
	{
		rcc = RCC_AHB1Periph_GPIOC;
	}
	else if (GPIOx == GPIOD)
	{
		rcc = RCC_AHB1Periph_GPIOD;
	}
	else if (GPIOx == GPIOE)
	{
		rcc = RCC_AHB1Periph_GPIOE;
	}
	else if (GPIOx == GPIOF)
	{
		rcc = RCC_AHB1Periph_GPIOF;
	}
	else if (GPIOx == GPIOG)
	{
		rcc = RCC_AHB1Periph_GPIOG;
	}
	else if (GPIOx == GPIOH)
	{
		rcc = RCC_AHB1Periph_GPIOH;
	}
	else if (GPIOx == GPIOI)
	{
		rcc = RCC_AHB1Periph_GPIOI;
	}

	return rcc;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_GetPinSource
*	功能说明: 根据 GPIO_Pin_X 得到 GPIO_PinSource
*	形    参：gpio_pin
*	返 回 值: GPIO_PinSource
*********************************************************************************************************
*/
uint16_t bsp_GetPinSource(uint16_t gpio_pin)
{
	uint16_t ret = 0;

	if (gpio_pin == GPIO_Pin_0)
	{
		ret = GPIO_PinSource0;
	}
	else if (gpio_pin == GPIO_Pin_1)
	{
		ret = GPIO_PinSource1;
	}
	else if (gpio_pin == GPIO_Pin_2)
	{
		ret = GPIO_PinSource2;
	}
	else if (gpio_pin == GPIO_Pin_3)
	{
		ret = GPIO_PinSource3;
	}
	else if (gpio_pin == GPIO_Pin_4)
	{
		ret = GPIO_PinSource4;
	}
	else if (gpio_pin == GPIO_Pin_5)
	{
		ret = GPIO_PinSource5;
	}
	else if (gpio_pin == GPIO_Pin_6)
	{
		ret = GPIO_PinSource6;
	}
	else if (gpio_pin == GPIO_Pin_7)
	{
		ret = GPIO_PinSource7;
	}
	else if (gpio_pin == GPIO_Pin_8)
	{
		ret = GPIO_PinSource8;
	}
	else if (gpio_pin == GPIO_Pin_9)
	{
		ret = GPIO_PinSource9;
	}
	else if (gpio_pin == GPIO_Pin_10)
	{
		ret = GPIO_PinSource10;
	}
	else if (gpio_pin == GPIO_Pin_11)
	{
		ret = GPIO_PinSource11;
	}
	else if (gpio_pin == GPIO_Pin_12)
	{
		ret = GPIO_PinSource12;
	}
	else if (gpio_pin == GPIO_Pin_13)
	{
		ret = GPIO_PinSource13;
	}
	else if (gpio_pin == GPIO_Pin_14)
	{
		ret = GPIO_PinSource14;
	}
	else if (gpio_pin == GPIO_Pin_15)
	{
		ret = GPIO_PinSource15;
	}

	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_GetRCCofTIM
*	功能说明: 根据TIM 得到RCC寄存器
*	形    参：无
*	返 回 值: TIM外设时钟名
*********************************************************************************************************
*/
uint32_t bsp_GetRCCofTIM(TIM_TypeDef* TIMx)
{
	uint32_t rcc = 0;

	/*
		APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14
		APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11
	*/
	if (TIMx == TIM1)
	{
		rcc = RCC_APB2Periph_TIM1;
	}
	else if (TIMx == TIM8)
	{
		rcc = RCC_APB2Periph_TIM8;
	}
	else if (TIMx == TIM9)
	{
		rcc = RCC_APB2Periph_TIM9;
	}
	else if (TIMx == TIM10)
	{
		rcc = RCC_APB2Periph_TIM10;
	}
	else if (TIMx == TIM11)
	{
		rcc = RCC_APB2Periph_TIM11;
	}
	/* 下面是 APB1时钟 */
	else if (TIMx == TIM2)
	{
		rcc = RCC_APB1Periph_TIM2;
	}
	else if (TIMx == TIM3)
	{
		rcc = RCC_APB1Periph_TIM3;
	}
	else if (TIMx == TIM4)
	{
		rcc = RCC_APB1Periph_TIM4;
	}
	else if (TIMx == TIM5)
	{
		rcc = RCC_APB1Periph_TIM5;
	}
	else if (TIMx == TIM6)
	{
		rcc = RCC_APB1Periph_TIM6;
	}
	else if (TIMx == TIM7)
	{
		rcc = RCC_APB1Periph_TIM7;
	}
	else if (TIMx == TIM12)
	{
		rcc = RCC_APB1Periph_TIM12;
	}
	else if (TIMx == TIM13)
	{
		rcc = RCC_APB1Periph_TIM13;
	}
	else if (TIMx == TIM14)
	{
		rcc = RCC_APB1Periph_TIM14;
	}

	return rcc;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_GetAFofTIM
*	功能说明: 根据TIM 得到AF寄存器配置
*	形    参: 无
*	返 回 值: AF寄存器配置
*********************************************************************************************************
*/
uint8_t bsp_GetAFofTIM(TIM_TypeDef* TIMx)
{
	uint8_t ret = 0;

	/*
		APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14
		APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11
	*/
	if (TIMx == TIM1)
	{
		ret = GPIO_AF_TIM1;
	}
	else if (TIMx == TIM8)
	{
		ret = GPIO_AF_TIM8;
	}
	else if (TIMx == TIM9)
	{
		ret = GPIO_AF_TIM9;
	}
	else if (TIMx == TIM10)
	{
		ret = GPIO_AF_TIM10;
	}
	else if (TIMx == TIM11)
	{
		ret = GPIO_AF_TIM11;
	}
	/* 下面是 APB1时钟 */
	else if (TIMx == TIM2)
	{
		ret = GPIO_AF_TIM2;
	}
	else if (TIMx == TIM3)
	{
		ret = GPIO_AF_TIM3;
	}
	else if (TIMx == TIM4)
	{
		ret = GPIO_AF_TIM4;
	}
	else if (TIMx == TIM5)
	{
		ret = GPIO_AF_TIM5;
	}
	/* 没有 TIM6 TIM7 */
	else if (TIMx == TIM8)
	{
		ret = GPIO_AF_TIM8;
	}
	else if (TIMx == TIM9)
	{
		ret = GPIO_AF_TIM9;
	}
	else if (TIMx == TIM12)
	{
		ret = GPIO_AF_TIM12;
	}
	else if (TIMx == TIM13)
	{
		ret = GPIO_AF_TIM13;
	}
	else if (TIMx == TIM14)
	{
		ret = GPIO_AF_TIM14;
	}

	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_ConfigTimGpio
*	功能说明: 配置GPIO和TIM时钟， GPIO连接到TIM输出通道
*	形    参: GPIOx
*			 GPIO_PinX
*			 TIMx
*			 _ucChannel
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_ConfigTimGpio(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinX, TIM_TypeDef* TIMx, uint8_t _ucChannel)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* 使能GPIO时钟 */
	RCC_AHB1PeriphClockCmd(bsp_GetRCCofGPIO(GPIOx), ENABLE);

  	/* 使能TIM时钟 */
	if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10) || (TIMx == TIM11))
	{
		RCC_APB2PeriphClockCmd(bsp_GetRCCofTIM(TIMx), ENABLE);
	}
	else
	{
		RCC_APB1PeriphClockCmd(bsp_GetRCCofTIM(TIMx), ENABLE);
	}

	/* 配置GPIO */
	GPIO_InitStructure.GPIO_Pin = GPIO_PinX;	/* 带入的形参 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOx, &GPIO_InitStructure);

	/* 连接到AF功能 */
	GPIO_PinAFConfig(GPIOx, bsp_GetPinSource(GPIO_PinX), bsp_GetAFofTIM(TIMx));
}

/*
*********************************************************************************************************
*	函 数 名: bsp_ConfigGpioOut
*	功能说明: 配置GPIO为推挽输出。主要用于PWM输出，占空比为0和100的情况。
*	形    参: GPIOx
*			  GPIO_PinX
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_ConfigGpioOut(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinX)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* 使能GPIO时钟 */
	RCC_AHB1PeriphClockCmd(bsp_GetRCCofGPIO(GPIOx), ENABLE);

	/* 配置GPIO */
	GPIO_InitStructure.GPIO_Pin = GPIO_PinX;		/* 带入的形参 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	/* 输出 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 推挽 */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;	/* 无上拉 */
	GPIO_Init(GPIOx, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SetTIMOutPWM
*	功能说明: 设置引脚输出的PWM信号的频率和占空比.  当频率为0，并且占空为0时，关闭定时器，GPIO输出0；
*			  当频率为0，占空比为100%时，GPIO输出1.
*	形    参: _ulFreq : PWM信号频率，单位Hz  (实际测试，最大输出频率为 168M / 4 = 42M）. 0 表示禁止输出
*			  _ulDutyCycle : PWM信号占空比，单位：万分之一。如5000，表示50.00%的占空比
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_SetTIMOutPWM(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, TIM_TypeDef* TIMx, uint8_t _ucChannel,
	 uint32_t _ulFreq, uint32_t _ulDutyCycle)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	uint16_t usPeriod;
	uint16_t usPrescaler;
	uint32_t uiTIMxCLK;

	if (_ulDutyCycle == 0)
	{		
		TIM_Cmd(TIMx, DISABLE);		/* 关闭PWM输出 */
		bsp_ConfigGpioOut(GPIOx, GPIO_Pin);	/* 配置GPIO为推挽输出 */		
		GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_RESET);	/* PWM = 0 */		
		return;
	}
	else if (_ulDutyCycle == 10000)
	{
		TIM_Cmd(TIMx, DISABLE);		/* 关闭PWM输出 */

		bsp_ConfigGpioOut(GPIOx, GPIO_Pin);	/* 配置GPIO为推挽输出 */		
		GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_SET);	/* PWM = 1 */	
		return;
	}
	

	bsp_ConfigTimGpio(GPIOx, GPIO_Pin, TIMx, _ucChannel);	/* 使能GPIO和TIM时钟，并连接TIM通道到GPIO */
	
    /*-----------------------------------------------------------------------
		system_stm32f4xx.c 文件中 void SetSysClock(void) 函数对时钟的配置如下：

		HCLK = SYSCLK / 1     (AHB1Periph)
		PCLK2 = HCLK / 2      (APB2Periph)
		PCLK1 = HCLK / 4      (APB1Periph)

		因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
		因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = PCLK2 x 2 = SystemCoreClock;

		APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM6, TIM12, TIM13,TIM14
		APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11

	----------------------------------------------------------------------- */
	if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10) || (TIMx == TIM11))
	{
		/* APB2 定时器 */
		uiTIMxCLK = SystemCoreClock;
	}
	else	/* APB1 定时器 */
	{
		uiTIMxCLK = SystemCoreClock / 2;
	}

	if (_ulFreq < 100)
	{
		usPrescaler = 10000 - 1;					/* 分频比 = 10000 */
		usPeriod =  (uiTIMxCLK / 10000) / _ulFreq  - 1;		/* 自动重装的值 */
	}
	else if (_ulFreq < 3000)
	{
		usPrescaler = 100 - 1;					/* 分频比 = 100 */
		usPeriod =  (uiTIMxCLK / 100) / _ulFreq  - 1;		/* 自动重装的值 */
	}
	else	/* 大于4K的频率，无需分频 */
	{
		usPrescaler = 0;					/* 分频比 = 1 */
		usPeriod = uiTIMxCLK / _ulFreq - 1;	/* 自动重装的值 */
	}

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = usPeriod;
	TIM_TimeBaseStructure.TIM_Prescaler = usPrescaler;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;		/* TIM1 和 TIM8 必须设置 */	

	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = (_ulDutyCycle * usPeriod) / 10000;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;	/* only for TIM1 and TIM8. */	
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;			/* only for TIM1 and TIM8. */		
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;		/* only for TIM1 and TIM8. */
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;		/* only for TIM1 and TIM8. */
	
	if (_ucChannel == 1)
	{
		TIM_OC1Init(TIMx, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
	}
	else if (_ucChannel == 2)
	{
		TIM_OC2Init(TIMx, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);
	}
	else if (_ucChannel == 3)
	{
		TIM_OC3Init(TIMx, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);
	}
	else if (_ucChannel == 4)
	{
		TIM_OC4Init(TIMx, &TIM_OCInitStructure);
		TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);
	}

	TIM_ARRPreloadConfig(TIMx, ENABLE);

	/* TIMx enable counter */
	TIM_Cmd(TIMx, ENABLE);

	/* 下面这句话对于TIM1和TIM8是必须的，对于TIM2-TIM6则不必要 */
	if ((TIMx == TIM1) || (TIMx == TIM8))
	{
		TIM_CtrlPWMOutputs(TIMx, ENABLE);
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SetTIMOutPWM_N
*	功能说明: 设置TIM的反相引脚（比如TIM8_CH3N) 输出的PWM信号的频率和占空比.  当频率为0，并且占空为0时，关闭定时器，GPIO输出0；
*			  当频率为0，占空比为100%时，GPIO输出1.
*	形    参: _ulFreq : PWM信号频率，单位Hz  (实际测试，最大输出频率为 168M / 4 = 42M）. 0 表示禁止输出
*			  _ulDutyCycle : PWM信号占空比，单位：万分之一。如5000，表示50.00%的占空比
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_SetTIMOutPWM_N(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, TIM_TypeDef* TIMx, uint8_t _ucChannel,
	 uint32_t _ulFreq, uint32_t _ulDutyCycle)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	uint16_t usPeriod;
	uint16_t usPrescaler;
	uint32_t uiTIMxCLK;

	if (_ulDutyCycle == 0)
	{		
		TIM_Cmd(TIMx, DISABLE);		/* 关闭PWM输出 */
		bsp_ConfigGpioOut(GPIOx, GPIO_Pin);	/* 配置GPIO为推挽输出 */		
		GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_RESET);	/* PWM = 0 */		
		return;
	}
	else if (_ulDutyCycle == 10000)
	{
		TIM_Cmd(TIMx, DISABLE);		/* 关闭PWM输出 */

		bsp_ConfigGpioOut(GPIOx, GPIO_Pin);	/* 配置GPIO为推挽输出 */		
		GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_SET);	/* PWM = 1 */	
		return;
	}
	

	bsp_ConfigTimGpio(GPIOx, GPIO_Pin, TIMx, _ucChannel);	/* 使能GPIO和TIM时钟，并连接TIM通道到GPIO */
	
    /*-----------------------------------------------------------------------
		system_stm32f4xx.c 文件中 void SetSysClock(void) 函数对时钟的配置如下：

		HCLK = SYSCLK / 1     (AHB1Periph)
		PCLK2 = HCLK / 2      (APB2Periph)
		PCLK1 = HCLK / 4      (APB1Periph)

		因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
		因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = PCLK2 x 2 = SystemCoreClock;

		APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM6, TIM12, TIM13,TIM14
		APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11

	----------------------------------------------------------------------- */
	if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10) || (TIMx == TIM11))
	{
		/* APB2 定时器 */
		uiTIMxCLK = SystemCoreClock;
	}
	else	/* APB1 定时器 */
	{
		uiTIMxCLK = SystemCoreClock / 2;
	}

	if (_ulFreq < 100)
	{
		usPrescaler = 10000 - 1;					/* 分频比 = 10000 */
		usPeriod =  (uiTIMxCLK / 10000) / _ulFreq  - 1;		/* 自动重装的值 */
	}
	else if (_ulFreq < 3000)
	{
		usPrescaler = 100 - 1;					/* 分频比 = 100 */
		usPeriod =  (uiTIMxCLK / 100) / _ulFreq  - 1;		/* 自动重装的值 */
	}
	else	/* 大于4K的频率，无需分频 */
	{
		usPrescaler = 0;					/* 分频比 = 1 */
		usPeriod = uiTIMxCLK / _ulFreq - 1;	/* 自动重装的值 */
	}

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = usPeriod;
	TIM_TimeBaseStructure.TIM_Prescaler = usPrescaler;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;		/* TIM1 和 TIM8 必须设置 */	

	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;		/* 此处和正相引脚不同 */
	TIM_OCInitStructure.TIM_Pulse = (_ulDutyCycle * usPeriod) / 10000;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;		/* only for TIM1 and TIM8. 此处和正相引脚不同 */	
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;			/* only for TIM1 and TIM8. */		 
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;		/* only for TIM1 and TIM8. */
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;		/* only for TIM1 and TIM8. */
	
	if (_ucChannel == 1)
	{
		TIM_OC1Init(TIMx, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
	}
	else if (_ucChannel == 2)
	{
		TIM_OC2Init(TIMx, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);
	}
	else if (_ucChannel == 3)
	{
		TIM_OC3Init(TIMx, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);
	}
	else if (_ucChannel == 4)
	{
		TIM_OC4Init(TIMx, &TIM_OCInitStructure);
		TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);
	}

	TIM_ARRPreloadConfig(TIMx, ENABLE);

	/* TIMx enable counter */
	TIM_Cmd(TIMx, ENABLE);

	/* 下面这句话对于TIM1和TIM8是必须的，对于TIM2-TIM6则不必要 */
	if ((TIMx == TIM1) || (TIMx == TIM8))
	{
		TIM_CtrlPWMOutputs(TIMx, ENABLE);
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SetTIMforInt
*	功能说明: 配置TIM和NVIC，用于简单的定时中断. 开启定时中断。 中断服务程序由应用程序实现。
*	形    参: TIMx : 定时器
*			  _ulFreq : 定时频率 （Hz）。 0 表示关闭。
*			  _PreemptionPriority : 中断优先级分组
*			  _SubPriority : 子优先级
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_SetTIMforInt(TIM_TypeDef* TIMx, uint32_t _ulFreq, uint8_t _PreemptionPriority, uint8_t _SubPriority)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	uint16_t usPeriod;
	uint16_t usPrescaler;
	uint32_t uiTIMxCLK;

  	/* 使能TIM时钟 */
	if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10) || (TIMx == TIM11))
	{
		RCC_APB2PeriphClockCmd(bsp_GetRCCofTIM(TIMx), ENABLE);
	}
	else
	{
		RCC_APB1PeriphClockCmd(bsp_GetRCCofTIM(TIMx), ENABLE);
	}

	if (_ulFreq == 0)
	{
		TIM_Cmd(TIMx, DISABLE);		/* 关闭定时输出 */
		return;
	}

    /*-----------------------------------------------------------------------
		system_stm32f4xx.c 文件中 void SetSysClock(void) 函数对时钟的配置如下：

		HCLK = SYSCLK / 1     (AHB1Periph)
		PCLK2 = HCLK / 2      (APB2Periph)
		PCLK1 = HCLK / 4      (APB1Periph)

		因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
		因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = PCLK2 x 2 = SystemCoreClock;

		APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13,TIM14
		APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11

	----------------------------------------------------------------------- */
	if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10) || (TIMx == TIM11))
	{
		/* APB2 定时器 */
		uiTIMxCLK = SystemCoreClock;
	}
	else	/* APB1 定时器 */
	{
		uiTIMxCLK = SystemCoreClock / 2;
	}

	if (_ulFreq < 100)
	{
		usPrescaler = 10000 - 1;					/* 分频比 = 1000 */
		usPeriod =  (uiTIMxCLK / 10000) / _ulFreq  - 1;		/* 自动重装的值 */
	}
	else if (_ulFreq < 3000)
	{
		usPrescaler = 100 - 1;					/* 分频比 = 100 */
		usPeriod =  (uiTIMxCLK / 100) / _ulFreq  - 1;		/* 自动重装的值 */
	}
	else	/* 大于4K的频率，无需分频 */
	{
		usPrescaler = 0;					/* 分频比 = 1 */
		usPeriod = uiTIMxCLK / _ulFreq - 1;	/* 自动重装的值 */
	}

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = usPeriod;
	TIM_TimeBaseStructure.TIM_Prescaler = usPrescaler;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;		/* TIM1 和 TIM8 必须设置 */

	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);

	TIM_ARRPreloadConfig(TIMx, ENABLE);

	/* TIM Interrupts enable */
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);

	/* TIMx enable counter */
	TIM_Cmd(TIMx, ENABLE);

	/* 配置TIM定时更新中断 (Update) */
	{
		NVIC_InitTypeDef NVIC_InitStructure;	/* 中断结构体在 misc.h 中定义 */
		uint8_t irq = 0;	/* 中断号, 定义在 stm32f4xx.h */

		if ((TIMx == TIM1) || (TIMx == TIM10))
			irq = TIM1_UP_TIM10_IRQn;
		else if (TIMx == TIM2)
			irq = TIM2_IRQn;
		else if (TIMx == TIM3)
			irq = TIM3_IRQn;
		else if (TIMx == TIM4)
			irq = TIM4_IRQn;
		else if (TIMx == TIM5)
			irq = TIM5_IRQn;
		else if (TIMx == TIM6)
			irq = TIM6_DAC_IRQn;
		else if (TIMx == TIM7)
			irq = TIM7_IRQn;
		else if (TIMx == TIM7)
			irq = TIM7_IRQn;
		else if (TIMx == TIM7)
			irq = TIM7_IRQn;
		else if ((TIMx == TIM8) || (TIMx == TIM13))
			irq = TIM8_UP_TIM13_IRQn;
		else if (TIMx == TIM9)
			irq = TIM1_BRK_TIM9_IRQn;
		else if (TIMx == TIM11)
			irq = TIM1_TRG_COM_TIM11_IRQn;
		else if (TIMx == TIM12)
			irq = TIM8_BRK_TIM12_IRQn;
		else if (TIMx == TIM12)
			irq = TIM8_TRG_COM_TIM14_IRQn;

		NVIC_InitStructure.NVIC_IRQChannel = irq;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = _PreemptionPriority;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = _SubPriority;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
}

/*
*********************************************************************************************************
*                                            Public Code
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*	函 数 名: bsp_Servo_GPIOConfig
*	功能说明: 配置舵机信号引脚复用为PWM引脚
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_Servo_GPIOConfig(void)
{
				GPIO_InitTypeDef  GPIO_InitStructure;

				/* 使能GPIO时钟 */
				RCC_AHB1PeriphClockCmd(HIP_PORT_CLK, ENABLE);
				RCC_AHB1PeriphClockCmd(KNEE_PORT_CLK, ENABLE);
	      RCC_AHB1PeriphClockCmd(ANKLE_PORT_CLK, ENABLE);
	      RCC_AHB1PeriphClockCmd(HEADTAIL_PORT_CLK, ENABLE);

				/* 配置HIP    GPIO */
				GPIO_InitStructure.GPIO_Pin = HIP_1_Pin | HIP_2_Pin | HIP_3_Pin | HIP_4_Pin;	/* 带入的形参 */
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
				GPIO_Init(HIP_PORT_1, &GPIO_InitStructure);
	      /* 连接到AF功能 */
				GPIO_PinAFConfig(HIP_PORT_1, HIP_1_PinSource, HIP_PORT_AF_1);
	      GPIO_PinAFConfig(HIP_PORT_1, HIP_2_PinSource, HIP_PORT_AF_1);
				GPIO_PinAFConfig(HIP_PORT_1, HIP_3_PinSource, HIP_PORT_AF_1);
				GPIO_PinAFConfig(HIP_PORT_1, HIP_4_PinSource, HIP_PORT_AF_1);

				/* 配置GPIO */
				GPIO_InitStructure.GPIO_Pin = HIP_5_Pin | HIP_6_Pin ;	/* 带入的形参 */
	      GPIO_Init(HIP_PORT_2, &GPIO_InitStructure);
				/* 连接到AF功能 */
				GPIO_PinAFConfig(HIP_PORT_2, HIP_5_PinSource, HIP_PORT_AF_2);
				GPIO_PinAFConfig(HIP_PORT_2, HIP_6_PinSource, HIP_PORT_AF_2);
				
				
				/* 配置KNEE    GPIO */
				GPIO_InitStructure.GPIO_Pin = KNEE_1_Pin | KNEE_2_Pin | KNEE_3_Pin | KNEE_4_Pin | KNEE_5_Pin | KNEE_6_Pin;	/* 带入的形参 */
				GPIO_Init(KNEE_PORT, &GPIO_InitStructure);
				/* 连接到AF功能 */
				GPIO_PinAFConfig(KNEE_PORT, KNEE_1_PinSource, KNEE_PORT_AF_1);
				GPIO_PinAFConfig(KNEE_PORT, KNEE_2_PinSource, KNEE_PORT_AF_1);
				GPIO_PinAFConfig(KNEE_PORT, KNEE_3_PinSource, KNEE_PORT_AF_2);
				GPIO_PinAFConfig(KNEE_PORT, KNEE_4_PinSource, KNEE_PORT_AF_2);		
				GPIO_PinAFConfig(KNEE_PORT, KNEE_5_PinSource, KNEE_PORT_AF_1);
				GPIO_PinAFConfig(KNEE_PORT, KNEE_6_PinSource, KNEE_PORT_AF_1);
				
				
				/* 配置ANKLE    GPIO */
				GPIO_InitStructure.GPIO_Pin = ANKLE_1_Pin | ANKLE_2_Pin | ANKLE_3_Pin | ANKLE_4_Pin ;	/* 带入的形参 */
				GPIO_Init(ANKLE_PORT_1, &GPIO_InitStructure);
				/* 连接到AF功能 */
				GPIO_PinAFConfig(ANKLE_PORT_1, ANKLE_1_PinSource, ANKLE_PORT_AF_1);
				GPIO_PinAFConfig(ANKLE_PORT_1, ANKLE_2_PinSource, ANKLE_PORT_AF_1);
				GPIO_PinAFConfig(ANKLE_PORT_1, ANKLE_3_PinSource, ANKLE_PORT_AF_1);
				GPIO_PinAFConfig(ANKLE_PORT_1, ANKLE_4_PinSource, ANKLE_PORT_AF_1);
				
				GPIO_InitStructure.GPIO_Pin = ANKLE_5_Pin | ANKLE_6_Pin;	/* 带入的形参 */
				GPIO_Init(ANKLE_PORT_2, &GPIO_InitStructure);				
				GPIO_PinAFConfig(ANKLE_PORT_2, ANKLE_5_PinSource, ANKLE_PORT_AF_2);
				GPIO_PinAFConfig(ANKLE_PORT_2, ANKLE_6_PinSource, ANKLE_PORT_AF_2);
				
				/*配置HEAD  TAIL 的GPIO*/
			  GPIO_InitStructure.GPIO_Pin = HEAD_Pin | TAIL_Pin;	/* 带入的形参 */
				GPIO_Init(HEADTAIL_PORT, &GPIO_InitStructure);				
				GPIO_PinAFConfig(HEADTAIL_PORT, HEAD_PinSource, HEADTAIL_PORT_AF);
				GPIO_PinAFConfig(HEADTAIL_PORT, TAIL_PinSource, HEADTAIL_PORT_AF);
}

/*
*********************************************************************************************************
*	函 数 名: bsp_Servo_TIMConfig
*	功能说明: 配置PWM引脚输出频率--50HZ  初始高电平时间 1.5ms  占空比--7.5%
*          1ms(左端极限位置) --2ms(右端极限位置)
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_Servo_TIMConfig(void)
{
	 	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		TIM_OCInitTypeDef          TIM_OCInitStructure;
	  u16                       CCR_VAL = DefaultPWM;
	
			RCC_APB1PeriphClockCmd( HIP_TIM_CLK | ANKLE_TIM_CLK | HEADTAIL_TIM_CLK, ENABLE);
			
			/* Time base configuration */
			TIM_TimeBaseStructure.TIM_Period = TIMPeriod;
			TIM_TimeBaseStructure.TIM_Prescaler = TIMAPB1_Prescaler;
			TIM_TimeBaseStructure.TIM_ClockDivision = 0;
			TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

			TIM_TimeBaseInit(HIP_TIM_1, &TIM_TimeBaseStructure);
			TIM_TimeBaseInit(HIP_TIM_2, &TIM_TimeBaseStructure);
	    TIM_TimeBaseInit(ANKLE_TIM_1, &TIM_TimeBaseStructure);
	    TIM_TimeBaseInit(HEADTAIL_TIM, &TIM_TimeBaseStructure);
			/* PWM1 Mode configuration: Channel1 */
			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
			TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
			TIM_OCInitStructure.TIM_Pulse = CCR_VAL;
			TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
			
			/*TIM5 Initialize*/
			TIM_OC1Init(HIP_TIM_1, &TIM_OCInitStructure);
			TIM_OC1PreloadConfig(HIP_TIM_1, TIM_OCPreload_Enable);
			TIM_OC2Init(HIP_TIM_1, &TIM_OCInitStructure);
			TIM_OC2PreloadConfig(HIP_TIM_1, TIM_OCPreload_Enable);
			TIM_OC3Init(HIP_TIM_1, &TIM_OCInitStructure);
			TIM_OC3PreloadConfig(HIP_TIM_1, TIM_OCPreload_Enable);
			TIM_OC4Init(HIP_TIM_1, &TIM_OCInitStructure);
			TIM_OC4PreloadConfig(HIP_TIM_1, TIM_OCPreload_Enable);
			TIM_ARRPreloadConfig(HIP_TIM_1, ENABLE);
			/* TIMx enable counter */
	    TIM_Cmd(HIP_TIM_1, ENABLE);
			
			/*TIM4 Initialize*/
			TIM_OC1Init(HIP_TIM_2, &TIM_OCInitStructure);
			TIM_OC1PreloadConfig(HIP_TIM_2, TIM_OCPreload_Enable);
			TIM_OC2Init(HIP_TIM_2, &TIM_OCInitStructure);
			TIM_OC2PreloadConfig(HIP_TIM_2, TIM_OCPreload_Enable);
			TIM_OC3Init(HIP_TIM_2, &TIM_OCInitStructure);
			TIM_OC3PreloadConfig(HIP_TIM_2, TIM_OCPreload_Enable);
			TIM_OC4Init(HIP_TIM_2, &TIM_OCInitStructure);
			TIM_OC4PreloadConfig(HIP_TIM_2, TIM_OCPreload_Enable);
			TIM_ARRPreloadConfig(HIP_TIM_2, ENABLE);
			/* TIMx enable counter */
	    TIM_Cmd(HIP_TIM_2, ENABLE);
			
			/*TIM3 Initialize*/
			TIM_OC1Init(ANKLE_TIM_1, &TIM_OCInitStructure);
			TIM_OC1PreloadConfig(ANKLE_TIM_1, TIM_OCPreload_Enable);
			TIM_OC2Init(ANKLE_TIM_1, &TIM_OCInitStructure);
			TIM_OC2PreloadConfig(ANKLE_TIM_1, TIM_OCPreload_Enable);
			TIM_OC3Init(ANKLE_TIM_1, &TIM_OCInitStructure);
			TIM_OC3PreloadConfig(ANKLE_TIM_1, TIM_OCPreload_Enable);
			TIM_OC4Init(ANKLE_TIM_1, &TIM_OCInitStructure);
			TIM_OC4PreloadConfig(ANKLE_TIM_1, TIM_OCPreload_Enable);
			TIM_ARRPreloadConfig(ANKLE_TIM_1, ENABLE);
			/* TIMx enable counter */
	    TIM_Cmd(ANKLE_TIM_1, ENABLE);
			
			/*TIM2 Initialize*/
			TIM_OC3Init(HEADTAIL_TIM, &TIM_OCInitStructure);
			TIM_OC3PreloadConfig(HEADTAIL_TIM, TIM_OCPreload_Enable);
			TIM_OC4Init(HEADTAIL_TIM, &TIM_OCInitStructure);
			TIM_OC4PreloadConfig(HEADTAIL_TIM, TIM_OCPreload_Enable);
			TIM_ARRPreloadConfig(HEADTAIL_TIM, ENABLE);
			/* TIMx enable counter */
	    TIM_Cmd(HEADTAIL_TIM, ENABLE);
			
			
			/*Enable  TIM1 and TIM9*/
			RCC_APB2PeriphClockCmd( KNEE_TIM_CLK, ENABLE);
			/* Time base configuration */
	    TIM_TimeBaseStructure.TIM_Period = TIMPeriod;
			TIM_TimeBaseStructure.TIM_Prescaler = TIMAPB2_Prescaler;
			TIM_TimeBaseStructure.TIM_ClockDivision = 0;
			TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
			TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;		/* TIM1 和 TIM8 必须设置 */	

			TIM_TimeBaseInit(KNEE_TIM_1, &TIM_TimeBaseStructure);
			TIM_TimeBaseInit(KNEE_TIM_2, &TIM_TimeBaseStructure);
			
			/* PWM1 Mode configuration: Channel1 */
			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
			TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
			TIM_OCInitStructure.TIM_Pulse = CCR_VAL;
			TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
			
			/*TIM9 Initialize*/
			TIM_OC1Init(KNEE_TIM_2, &TIM_OCInitStructure);
			TIM_OC1PreloadConfig(KNEE_TIM_2, TIM_OCPreload_Enable);
			TIM_OC2Init(KNEE_TIM_2, &TIM_OCInitStructure);
			TIM_OC2PreloadConfig(KNEE_TIM_2, TIM_OCPreload_Enable);
			TIM_ARRPreloadConfig(KNEE_TIM_2, ENABLE);
			/* TIMx enable counter */
	    TIM_Cmd(KNEE_TIM_2, ENABLE);
			
			TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;	/* only for TIM1 and TIM8. */	
			TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;			/* only for TIM1 and TIM8. */		
			TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;		/* only for TIM1 and TIM8. */
			TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;		/* only for TIM1 and TIM8. */
			/*TIM1 Initialize*/
			TIM_OC1Init(KNEE_TIM_1, &TIM_OCInitStructure);
			TIM_OC1PreloadConfig(KNEE_TIM_1, TIM_OCPreload_Enable);
			TIM_OC2Init(KNEE_TIM_1, &TIM_OCInitStructure);
			TIM_OC2PreloadConfig(KNEE_TIM_1, TIM_OCPreload_Enable);
			TIM_OC3Init(KNEE_TIM_1, &TIM_OCInitStructure);
			TIM_OC3PreloadConfig(KNEE_TIM_1, TIM_OCPreload_Enable);
			TIM_OC4Init(KNEE_TIM_1, &TIM_OCInitStructure);
			TIM_OC4PreloadConfig(KNEE_TIM_1, TIM_OCPreload_Enable);
			TIM_ARRPreloadConfig(KNEE_TIM_1, ENABLE);
			/* TIMx enable counter */
	    TIM_Cmd(KNEE_TIM_1, ENABLE);
			TIM_CtrlPWMOutputs(KNEE_TIM_1, ENABLE);
}

/*
*********************************************************************************************************
*	函 数 名: bsp_ServoInit
*	功能说明: 舵机配置初始化
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_ServoInit(void)
{
	   bsp_Servo_GPIOConfig();
	   bsp_Servo_TIMConfig();
}
/***************************** 阿波罗科技 www.apollorobot.com (END OF FILE) *********************************/
