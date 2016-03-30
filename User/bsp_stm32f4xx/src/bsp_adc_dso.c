/*
*********************************************************************************************************
*
*	模块名称 : 示波器模块ADC底层的驱动
*	文件名称 : bsp_adc_dso.c
*	版    本 : V1.1
*	说    明 : 使用STM32内部ADC，同步采集两路波形。占用了部分GPIO控制示波器模块的增益和耦合方式。
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2015-10-06  armfly  正式发布。 支持 D112-1 版本的示波器模块
*		V1.1    2015-10-06  armfly  增加对 D112-2 版本的示波器模块，多2
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
	---------------------------------------------------------
	D112-1 版本的示波器模块 （新版）
		示波器通道1
			PC0 = ADC123_IN10
			AC1 = PH11  控制AC-DC切换, 1表示DC, 0表示AC
			G1  = PH9   控制衰减比, 1表示1:1， 0表示1:5

		示波器通道2
			PF10 = ADC3_IN8
			AC2 = PA0  控制AC-DC切换, 1表示DC, 0表示AC
			G2  = PH9   控制衰减比, 1表示1:1， 0表示1:5

	---------------------------------------------------------
	D112-2 版本的示波器模块  （新版）
		示波器通道1
			PC0 = ADC123_IN10
			AC1 = PA0    控制AC-DC切换, 1表示DC, 0表示AC
			G1A  = PH9   控制衰减比
			G1B  = PH10  控制衰减比

		示波器通道2
			PF10 = ADC3_IN8
			AC2  = PF9   控制AC-DC切换, 1表示DC, 0表示AC
			G2A  = PH11  控制衰减比
			G2B  = PA5   控制衰减比
			
*/
#define GPIO_AC1	GPIOA
#define PIN_AC1		GPIO_Pin_0

#define GPIO_AC2	GPIOF
#define PIN_AC2		GPIO_Pin_9

#define GPIO_G1A	GPIOH
#define PIN_G1A		GPIO_Pin_9

#define GPIO_G1B	GPIOH
#define PIN_G1B		GPIO_Pin_10

#define GPIO_G2A	GPIOH
#define PIN_G2A		GPIO_Pin_11

#define GPIO_G2B	GPIOA
#define PIN_G2B		GPIO_Pin_5


#define AC1_0()		GPIO_AC1->BSRRH = PIN_AC1
#define AC1_1()		GPIO_AC1->BSRRL = PIN_AC1

#define AC2_0()		GPIO_AC2->BSRRH = PIN_AC2
#define AC2_1()		GPIO_AC2->BSRRL = PIN_AC2

#define G1A_0()		GPIO_G1A->BSRRH = PIN_G1A
#define G1A_1()		GPIO_G1A->BSRRL = PIN_G1A

#define G1B_0()		GPIO_G1B->BSRRH = PIN_G1B
#define G1B_1()		GPIO_G1B->BSRRL = PIN_G1B

#define G2A_0()		GPIO_G2A->BSRRH = PIN_G2A
#define G2A_1()		GPIO_G2A->BSRRL = PIN_G2A

#define G2B_0()		GPIO_G2B->BSRRH = PIN_G2B
#define G2B_1()		GPIO_G2B->BSRRL = PIN_G2B

/*
*********************************************************************************************************
*	函 数 名: DSO_ConfigCtrlGPIO
*	功能说明: 配置控制用通道耦合和增益的GPIO
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void DSO_ConfigCtrlGPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOA, ENABLE);
	
	/* 配置引脚为推挽输出模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* 设为输出口 */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* 上下拉电阻不使能 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;	/* IO口最大速度 */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_5;
	GPIO_Init(GPIOA, &GPIO_InitStructure);		

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_Init(GPIOH, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOF, &GPIO_InitStructure);		

#if 0	
	DSO_SetDC(1, 0);	/* CH1选择AC耦合 */
	DSO_SetDC(2, 0);	/* CH1选择AC耦合 */
	DSO_SetGain(1, 0);	/* CH1选择小增益 衰减1/5, 第2个参数1表示不衰减1;1 */
	DSO_SetGain(2, 0);	/* CH2选择小增益 衰减1/5, 第2个参数1表示不衰减1;1 */	
#endif	
}

/*
*********************************************************************************************************
*	函 数 名: DSO_SetDC
*	功能说明: 控制AC DC耦合模式
*	形    参: _ch : 通道1或2
*			  _mode : 0或1.  1表示DC耦合 0表示AC耦合
*	返 回 值: 无
*********************************************************************************************************
*/
void DSO_SetDC(uint8_t _ch, uint8_t _mode)
{	
#ifdef	D112_1
	if (_ch == 1)
	{
		GPIO_WriteBit(GPIOH, GPIO_Pin_11, (BitAction)_mode);
	}
	else
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, (BitAction)_mode);
	}
#else	/* 升级版 */
	if (_ch == 1)
	{
		if (_mode == 1)
		{
			AC1_1();
		}
		else
		{
			AC1_0();
		}
	}
	else
	{
		if (_mode == 1)
		{
			AC2_1();
		}
		else
		{
			AC2_0();
		}
	}	
#endif
}

/*
*********************************************************************************************************
*	函 数 名: SetGainHigh
*	功能说明: 控制增益模式
*	形    参: _ch : 通道1或2
*			  _gain : 0或1.  1表示1:1， 0表示衰减1/5；    0,1,2,3
*			  
*	返 回 值: 无
*********************************************************************************************************
*/
void DSO_SetGain(uint8_t _ch, uint8_t _gain)
{	
#ifdef	D112_1	/* 旧板 */
	if (_ch == 1)
	{
		GPIO_WriteBit(GPIOH, GPIO_Pin_9, (BitAction)_gain);
	}
	else
	{
		GPIO_WriteBit(GPIOH, GPIO_Pin_10, (BitAction)_gain);
	}
#else		/* 升级版 */
	if (_ch == 1)
	{
		if (_gain == 0)
		{
			G1A_0();
			G1B_0();
		}
		else if (_gain == 1)
		{
			G1A_1();
			G1B_0();
		}		
		else if (_gain == 2)
		{
			G1A_0();
			G1B_1();
		}
		else if (_gain == 3)
		{
			G1A_1();
			G1B_1();
		}			
	}
	else
	{
		if (_gain == 0)
		{
			G2A_0();
			G2B_0();
		}
		else if (_gain == 1)
		{
			G2A_1();
			G2B_0();
		}		
		else if (_gain == 2)
		{
			G2A_0();
			G2B_1();
		}
		else if (_gain == 3)
		{
			G2A_1();
			G2B_1();
		}	
	}
#endif	
}

/*
*********************************************************************************************************
*	函 数 名: DSO_StartADC
*	功能说明: 主要完成模拟量GPIO的配置、ADC的配置、定时器的配置以及DMA的配置。
*			- PC0/ADC123_10 ： CH1通道，ADC1，DMA2_Stream0
*			- PF10/ADC3_1N8 ： CH2通道, ADC3，DMA2_Stream1
*			- 两个ADC工作在独立模式
*			- 具有相同的外部触发，ADC_ExternalTrigConv_T1_CC1
*			- TIM1的CC1频率的决定了采样频率
*
*	形    参: _uiBufAddr1 : DMA目标地址，CH1数据存放的缓冲区地址
*			  _uiBufAddr2 : DMA目标地址，CH2数据存放的缓冲区地址
*			  _uiCount : 缓冲区的样本个数 (不是字节数)，两通道同步采集.
*			  _uiFreq : 采样频率， Hz
*	返 回 值: 无
*********************************************************************************************************
*/
void DSO_StartADC(uint32_t _uiBufAddr1, uint32_t _uiBufAddr2, uint32_t _uiCount, uint32_t _uiFreq)
{			
	/* 配置GPIO. PC0 + PF10 */
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOF, ENABLE);
		
		/* 配置ADC引脚为模拟输入模式  PC0 / ADC123_10 ******************************/
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		/* 配置ADC引脚为模拟输入模式  PF10 / ADC3_IN8 ******************************/
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_Init(GPIOF, &GPIO_InitStructure);		
	}

	/* DMA的配置选择见手册 205页 表 35. DMA1 请求映射 + 表 36. DMA2 请求映射 */ 
	{
		DMA_InitTypeDef       DMA_InitStructure;
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
		
		/* DMA2 Stream 0 channel 0 配置用于ADC1 **************************************/
		DMA_DeInit(DMA2_Stream0);	/* 内部会清一些标志，必须加这句话，否则再次启动DMA无反应 */
		DMA_InitStructure.DMA_Channel = DMA_Channel_0;
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
		DMA_InitStructure.DMA_Memory0BaseAddr = _uiBufAddr1;	/* 目标地址 */
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
		DMA_InitStructure.DMA_BufferSize = _uiCount;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	// 无需循环模式 DMA_Mode_Circular;
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		DMA_Init(DMA2_Stream0, &DMA_InitStructure);
		DMA_Cmd(DMA2_Stream0, ENABLE);
	}

	{
		DMA_InitTypeDef       DMA_InitStructure;
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
		
		/* DMA2 Stream 1 channel 2 配置用于ADC3 **************************************/
		DMA_DeInit(DMA2_Stream1);	/* 内部会清一些标志，必须加这句话，否则再次启动DMA无反应 */
		DMA_InitStructure.DMA_Channel = DMA_Channel_2;
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC3->DR;
		DMA_InitStructure.DMA_Memory0BaseAddr = _uiBufAddr2;	/* 目标地址 */
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
		DMA_InitStructure.DMA_BufferSize = _uiCount;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	// 无需循环模式 DMA_Mode_Circular;
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		DMA_Init(DMA2_Stream1, &DMA_InitStructure);
		
		DMA_Cmd(DMA2_Stream1, ENABLE);
	}
	
	/* ADC公共部分初始化**********************************************************/
	{
		/*
		***************************************************************************
		  PCLK2 = HCLK / 2
		  下面选择的是2分频
		  ADCCLK = PCLK2 /2 = HCLK / 4 = 168 / 4 = 42M  [手册最大时36M，此时有超频]
		  ADC采样频率： Sampling Time + Conversion Time = 3 + 12 cycles = 15cyc
						Conversion Time = 42MHz / 15cyc = 2.8Mbps.		
		****************************************************************************
		*/
		ADC_CommonInitTypeDef ADC_CommonInitStructure;

		ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; //?
		ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
		ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
		ADC_CommonInit(&ADC_CommonInitStructure);
	}
	
	/* 配置 PC0/ADC1_IN10  ******************************************************************/
	{
		ADC_InitTypeDef       ADC_InitStructure;
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
		
		ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
		ADC_InitStructure.ADC_ScanConvMode = DISABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
		ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		ADC_InitStructure.ADC_NbrOfConversion = 1;

		/* ADC1 规则通道配置 -------------------------------------------------------*/
		ADC_Init(ADC1, &ADC_InitStructure);
		ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_3Cycles);

		/* 使能 ADC1 DMA */
		ADC_DMACmd(ADC1, ENABLE);

		/* 使能DMA请求 最后一个传输结束后继续使能DMA, 对于循环模式必须使能 -------*/
		//ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

		/* Enable ADC1 --------------------------------------------------------------*/
		ADC_Cmd(ADC1, ENABLE);
	}
	
	/* 配置 PF10/ADC3_IN8  ******************************************************************/
	{
		ADC_InitTypeDef       ADC_InitStructure;
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
		
		ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
		ADC_InitStructure.ADC_ScanConvMode = DISABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
		ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		ADC_InitStructure.ADC_NbrOfConversion = 1;

		/* ADC1 规则通道配置 -------------------------------------------------------*/
		ADC_Init(ADC3, &ADC_InitStructure);
		ADC_RegularChannelConfig(ADC3, ADC_Channel_8, 1, ADC_SampleTime_3Cycles);

		/* 使能 ADC3 DMA */
		ADC_DMACmd(ADC3, ENABLE);

		/* 使能DMA请求 最后一个传输结束后继续使能DMA, 对于循环模式必须使能 -------*/
		//ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);

		/* Enable ADC3 --------------------------------------------------------------*/
		ADC_Cmd(ADC3, ENABLE);
	}

	/* 配置采样触发定时器，使用TIM1 CC1 */
	DSO_SetSampRate(_uiFreq);	/* 修改采样频率，并启动TIM */
}

/*
*********************************************************************************************************
*	函 数 名: DSO_StopADC
*	功能说明: 关闭ADC采样所有的外设。ADC, DMA, TIM
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void DSO_StopADC(void)
{
	TIM_Cmd(TIM1, DISABLE);
	
	DMA_Cmd(DMA2_Stream1, DISABLE);
	DMA_Cmd(DMA2_Stream0, DISABLE);

	ADC_DMACmd(ADC1, DISABLE);
	ADC_Cmd(ADC1, DISABLE);	

	ADC_DMACmd(ADC3, DISABLE);
	ADC_Cmd(ADC3, DISABLE);		
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, DISABLE);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, DISABLE);	
}

/*
*********************************************************************************************************
*	函 数 名: PauseADC
*	功能说明: 暂停ADC采样。准备处理数据。保证下次DMA正常启动。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void DSO_PauseADC(void)
{
	TIM_Cmd(TIM1, DISABLE);

	ADC_DMACmd(ADC1, DISABLE);	/* 这句话必须有，否则无法启动下次DMA */
	ADC_DMACmd(ADC3, DISABLE);
}

/*
*********************************************************************************************************
*	函 数 名: SetSampRate
*	功能说明: 修改采样频率
*	形    参: freq : 采样频率 单位Hz
*	返 回 值: 无
*********************************************************************************************************
*/
void DSO_SetSampRate(uint32_t _ulFreq)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	/* 使能TIM1时钟 */

    TIM_Cmd(TIM1, DISABLE);
	TIM_SetCounter(TIM1, 0);
    /*
    ********************************************************************************
    system_stm32f4xx.c 文件中 void SetSysClock(void) 函数对时钟的配置如下：

    HCLK = SYSCLK / 1     (AHB1Periph)
    PCLK2 = HCLK / 2      (APB2Periph)
    PCLK1 = HCLK / 4      (APB1Periph)

    因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
    因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = PCLK2 x 2 = SystemCoreClock;

    APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14
    APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11

    TIM1 更新周期是 = TIM1CLK / （TIM_Period + 1）/（TIM_Prescaler + 1）
    ********************************************************************************
    */

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

    /* ARR自动重装载寄存器周期的值(定时时间）到设置频率后产生个更新或者中断. */
	TIM_TimeBaseStructure.TIM_Period =  168000000 / (_ulFreq) - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;	/* 不分频 */
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;		/* TIM1 和 TIM8 必须设置 */		
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
    /**************ADC1 ADC3的触发***********************************************/
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period + 1) / 2;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;		/* only for TIM1 and TIM8. 此处和正相引脚不同 */	
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;			/* only for TIM1 and TIM8. */		 
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;		/* only for TIM1 and TIM8. */
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;		/* only for TIM1 and TIM8. */		
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);	/* 使能PWM 输出, 不是输出到GPIO */
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
