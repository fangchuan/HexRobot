/*
*********************************************************************************************************
*
*	模块名称 : DAC波形发生器
*	文件名称 : bsp_dac_wave.c
*	版    本 : V1.0
*	说    明 : 使用STM32内部DAC输出波形。支持DAC1和DAC2输出不同的波形。
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2015-10-06  armfly  正式发布
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
	PA4用作 DAC_OUT1
	PA5用作 DAC_OUT2

	DAC1使用了TIM6作为定时触发， DMA通道: DMA1_Stream5
	DAC2使用了TIM7作为定时触发， DMA通道: DMA2_Stream6	
	
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable 开启了DAC输出缓冲，增加驱动能力,
	开了缓冲之后，靠近0V和参考电源时，失真厉害，最低50mV
	不开缓冲波形较好，到0V目测不出明显失真。
	
	功能：
	1、输出正弦波，幅度和频率可调节
	2、输出方波，幅度偏移可调节，频率可调节，占空比可以调节
	3、输出三角波，幅度可调节，频率可调节，上升沿占比可调节
	4、基本的DAC输出直流电平的函数
*/

/*  正弦波数据，12bit，1个周期128个点, 0-4095之间变化 */
const uint16_t g_SineWave128[] = {
	2047 ,
	2147 ,
	2248 ,
	2347 ,
	2446 ,
	2544 ,
	2641 ,
	2737 ,
	2830 ,
	2922 ,
	3012 ,
	3099 ,
	3184 ,
	3266 ,
	3346 ,
	3422 ,
	3494 ,
	3564 ,
	3629 ,
	3691 ,
	3749 ,
	3803 ,
	3852 ,
	3897 ,
	3938 ,
	3974 ,
	4006 ,
	4033 ,
	4055 ,
	4072 ,
	4084 ,
	4092 ,
	4094 ,
	4092 ,
	4084 ,
	4072 ,
	4055 ,
	4033 ,
	4006 ,
	3974 ,
	3938 ,
	3897 ,
	3852 ,
	3803 ,
	3749 ,
	3691 ,
	3629 ,
	3564 ,
	3494 ,
	3422 ,
	3346 ,
	3266 ,
	3184 ,
	3099 ,
	3012 ,
	2922 ,
	2830 ,
	2737 ,
	2641 ,
	2544 ,
	2446 ,
	2347 ,
	2248 ,
	2147 ,
	2047 ,
	1947 ,
	1846 ,
	1747 ,
	1648 ,
	1550 ,
	1453 ,
	1357 ,
	1264 ,
	1172 ,
	1082 ,
	995  ,
	910  ,
	828  ,
	748  ,
	672  ,
	600  ,
	530  ,
	465  ,
	403  ,
	345  ,
	291  ,
	242  ,
	197  ,
	156  ,
	120  ,
	88   ,
	61   ,
	39   ,
	22   ,
	10   ,
	2    ,
	0    ,
	2    ,
	10   ,
	22   ,
	39   ,
	61   ,
	88   ,
	120  ,
	156  ,
	197  ,
	242  ,
	291  ,
	345  ,
	403  ,
	465  ,
	530  ,
	600  ,
	672  ,
	748  ,
	828  ,
	910  ,
	995  ,
	1082 ,
	1172 ,
	1264 ,
	1357 ,
	1453 ,
	1550 ,
	1648 ,
	1747 ,
	1846 ,
	1947
};

/* 正弦波 (32样本，适合高频） */
const uint16_t g_SineWave32[32] = {
                      2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
                      3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
                      599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};

/* DMA波形缓冲区 */
uint16_t g_Wave1[128];
uint16_t g_Wave2[128];

/*
*********************************************************************************************************
*	函 数 名: bsp_InitDAC1
*	功能说明: 配置PA4/DAC1
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitDAC1(void)
{	
	/* 配置GPIO */
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		
		/* 配置ADC引脚为模拟输入模式  PA4 / DAC_OUT1 */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}	

	/* DAC channel1 Configuration */
	{
		DAC_InitTypeDef DAC_InitStructure;
		
		/* DAC Periph clock enable */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);		

		DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;	/* 选择软件触发, 软件修改DAC数据寄存器 */
		DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
		DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
		//DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
		DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
		DAC_Init(DAC_Channel_1, &DAC_InitStructure);
		
		/* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is
		 automatically connected to the DAC converter. */
		DAC_Cmd(DAC_Channel_1, ENABLE);
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitDAC2
*	功能说明: 配置PA5/DAC2
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitDAC2(void)
{	
	/* 配置GPIO */
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		
		/* 配置ADC引脚为模拟输入模式  PA5 / DAC_OUT2 */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}	

	/* DAC channel1 Configuration */
	{
		DAC_InitTypeDef DAC_InitStructure;
		
		/* DAC Periph clock enable */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);		

		DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;	/* 选择软件触发, 软件修改DAC数据寄存器 */
		DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
		DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
		//DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
		DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
		DAC_Init(DAC_Channel_2, &DAC_InitStructure);
		
		/* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.05 is
		 automatically connected to the DAC converter. */
		DAC_Cmd(DAC_Channel_2, ENABLE);
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SetDAC1
*	功能说明: 设置DAC1输出数据寄存器，改变输出电压
*	形    参: _dac : DAC数据，0-4095
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_SetDAC1(uint16_t _dac)
{
	DAC->DHR12R1 = _dac;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SetDAC2
*	功能说明: 设置DAC2输出数据寄存器，改变输出电压
*	形    参: _dac : DAC数据，0-4095
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_SetDAC2(uint16_t _dac)
{
	DAC->DHR12R2 = _dac;
}

/*
*********************************************************************************************************
*	                                    DAC1 - DMA 部分的代码 *
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*	函 数 名: dac1_InitForDMA
*	功能说明: 配置PA4 为DAC_OUT1, 启用DMA2
*	形    参: _BufAddr : DMA数据缓冲区地址
*			  _Count : 缓冲区样本个数
*			 _DacFreq : DAC样本更新频率
*	返 回 值: 无
*********************************************************************************************************
*/
void dac1_InitForDMA(uint32_t _BufAddr, uint32_t _Count, uint32_t _DacFreq)
{		
	DMA_Cmd(DMA1_Stream5, DISABLE);
	DAC_DMACmd(DAC_Channel_1, DISABLE);
	TIM_Cmd(TIM6, DISABLE);
		
	/* 配置GPIO.*/
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		
		/* 配置ADC引脚为模拟输入模式  PA4 / DAC_OUT1 ******************************/
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}	

	/* TIM6 Configuration */
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

		TIM_PrescalerConfig(TIM6, 0, TIM_PSCReloadMode_Update);

		/* 决定DAC输出的采样频率
		  x = (168000000 / 2) / 频率
		*/
		TIM_SetAutoreload(TIM6, (SystemCoreClock / 2) / _DacFreq - 1);

		/* TIM6 TRGO selection */
		TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
	}

	/* DAC channel1 Configuration */
	{
		DAC_InitTypeDef DAC_InitStructure;
		
		/* DAC Periph clock enable */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);		

		DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
		DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
		DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
		//DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
		DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
		DAC_Init(DAC_Channel_1, &DAC_InitStructure);
		
		/* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is
		 automatically connected to the DAC converter. */
		DAC_Cmd(DAC_Channel_1, ENABLE);
	}

	/* DMA1_Stream5 */
	{
		DMA_InitTypeDef DMA_InitStructure;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

		/* DMA1 Stream 5 channel 7 配置用于DAC1 **************************************/
		DMA_InitStructure.DMA_Channel = DMA_Channel_7;
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&DAC->DHR12R1; /* DAC1数据寄存器 12位右对齐格式 */
		DMA_InitStructure.DMA_Memory0BaseAddr = _BufAddr;	/* 内存地址-源地址 */
		DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;		/* 内存到外设 */
		DMA_InitStructure.DMA_BufferSize = _Count;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		DMA_Init(DMA1_Stream5, &DMA_InitStructure);
		DMA_Cmd(DMA1_Stream5, ENABLE);

		/* Enable DMA for DAC Channel1 */
		DAC_DMACmd(DAC_Channel_1, ENABLE);
	}

	/* TIM6 enable counter */
	TIM_Cmd(TIM6, ENABLE);
}

/*
*********************************************************************************************************
*	函 数 名: dac1_SetSinWave
*	功能说明: DAC1输出正弦波
*	形    参: _vpp : 幅度 0-2047;
*			  _freq : 频率
*	返 回 值: 无
*********************************************************************************************************
*/
void dac1_SetSinWave(uint16_t _vpp, uint32_t _freq)
{	
	uint32_t i;
	uint32_t dac;
	
	TIM_Cmd(TIM6, DISABLE);
		
	/* 调整正弦波幅度 */		
	for (i = 0; i < 128; i++)
	{
		dac = (g_SineWave128[i] * _vpp) / 2047;
		if (dac > 4095)
		{
			dac = 4095;	
		}
		g_Wave1[i] = dac;
	}
	
	dac1_InitForDMA((uint32_t)&g_Wave1, 128, _freq * 128);
}

/*
*********************************************************************************************************
*	函 数 名: dac1_SetRectWave
*	功能说明: DAC1输出方波
*	形    参: _low : 低电平时DAC, 
*			  _high : 高电平时DAC
*			  _freq : 频率 Hz
*			  _duty : 占空比 2% - 98%, 调节步数 1%
*	返 回 值: 无
*********************************************************************************************************
*/
void dac1_SetRectWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty)
{	
	uint16_t i;
	TIM_Cmd(TIM6, DISABLE);
	
	for (i = 0; i < (_duty * 128) / 100; i++)
	{
		g_Wave1[i] = _high;
	}
	for (; i < 128; i++)
	{
		g_Wave1[i] = _low;
	}
	
	dac1_InitForDMA((uint32_t)&g_Wave1, 128, _freq * 128);
}

/*
*********************************************************************************************************
*	函 数 名: dac1_SetTriWave
*	功能说明: DAC1输出三角波
*	形    参: _low : 低电平时DAC, 
*			  _high : 高电平时DAC
*			  _freq : 频率 Hz
*			  _duty : 占空比
*	返 回 值: 无
*********************************************************************************************************
*/
void dac1_SetTriWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty)
{	
	uint32_t i;
	uint16_t dac;
	uint16_t m;
	
	TIM_Cmd(TIM6, DISABLE);
		
	/* 构造三角波数组，128个样本，从 _low 到 _high */		
	m = (_duty * 128) / 100;
	
	if (m == 0)
	{
		m = 1;
	}
	
	if (m > 127)
	{
		m = 127;
	}
	for (i = 0; i < m; i++)
	{
		dac = _low + ((_high - _low) * i) / m;
		g_Wave1[i] = dac;
	}
	for (; i < 128; i++)
	{
		dac = _high - ((_high - _low) * (i - m)) / (128 - m);
		g_Wave1[i] = dac;
	}	
	
	dac1_InitForDMA((uint32_t)&g_Wave1, 128, _freq * 128);
}

/*
*********************************************************************************************************
*	函 数 名: dac1_StopWave
*	功能说明: 停止DAC1输出
*	形    参: 无
*			  _freq : 频率 0-5Hz
*	返 回 值: 无
*********************************************************************************************************
*/
void dac1_StopWave(void)
{	
	DMA_Cmd(DMA1_Stream5, DISABLE);
	DAC_DMACmd(DAC_Channel_1, DISABLE);
	TIM_Cmd(TIM6, DISABLE);
}

/*
********************************************************************************************************
                                           DAC2 DMA 部分的代码
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*	函 数 名: dac2_InitForDMA
*	功能说明: 配置PA4 为DAC_OUT1, 启用DMA2
*	形    参: _BufAddr : DMA数据缓冲区地址
*			  _Count : 缓冲区样本个数
*			 _DacFreq : DAC样本更新频率
*	返 回 值: 无
*********************************************************************************************************
*/
void dac2_InitForDMA(uint32_t _BufAddr, uint32_t _Count, uint32_t _DacFreq)
{		
	DMA_Cmd(DMA1_Stream6, DISABLE);
	DAC_DMACmd(DAC_Channel_2, DISABLE);
	TIM_Cmd(TIM7, DISABLE);
		
	/* 配置GPIO.*/
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		
		/* 配置ADC引脚为模拟输入模式  PA5 / DAC_OUT2 ******************************/
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}	

	/* TIM6 Configuration */
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

		TIM_PrescalerConfig(TIM7, 0, TIM_PSCReloadMode_Update);

		/* 决定DAC输出的采样频率
		  x = (168000000 / 2) / 频率
		*/
		TIM_SetAutoreload(TIM6, (SystemCoreClock / 2) / _DacFreq - 1);

		/* TIM7 TRGO selection */
		TIM_SelectOutputTrigger(TIM7, TIM_TRGOSource_Update);
	}

	/* DAC channel1 Configuration */
	{
		DAC_InitTypeDef DAC_InitStructure;
		
		/* DAC Periph clock enable */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);		

		DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
		DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
		DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
		//DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable; 
		DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
		DAC_Init(DAC_Channel_2, &DAC_InitStructure);
		
		/* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.05 is
		 automatically connected to the DAC converter. */
		DAC_Cmd(DAC_Channel_2, ENABLE);
	}

	/* DMA1_Stream5 */
	{
		DMA_InitTypeDef DMA_InitStructure;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

		/* DMA2 Stream 5 channel 7 配置用于DAC1 **************************************/
		DMA_InitStructure.DMA_Channel = DMA_Channel_7;
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&DAC->DHR12R2;		/* DAC2数据寄存器 12位右对齐格式 */
		DMA_InitStructure.DMA_Memory0BaseAddr = _BufAddr;	/* 内存地址-源地址 */
		DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;		/* 内存到外设 */
		DMA_InitStructure.DMA_BufferSize = _Count;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		DMA_Init(DMA1_Stream6, &DMA_InitStructure);
		DMA_Cmd(DMA1_Stream6, ENABLE);

		/* Enable DMA for DAC Channel1 */
		DAC_DMACmd(DAC_Channel_2, ENABLE);
	}

	/* TIM6 enable counter */
	TIM_Cmd(TIM7, ENABLE);
}

/*
*********************************************************************************************************
*	函 数 名: dac2_SetSinWave
*	功能说明: DAC1输出正弦波
*	形    参: _vpp : 幅度 0-2047;
*			  _freq : 频率 0-5Hz
*	返 回 值: 无
*********************************************************************************************************
*/
void dac2_SetSinWave(uint16_t _vpp, uint32_t _freq)
{	
	uint32_t i;
	uint32_t dac;
	
	TIM_Cmd(TIM7, DISABLE);
		
	/* 调整正弦波幅度 */		
	for (i = 0; i < 128; i++)
	{
		dac = (g_SineWave128[i] * _vpp) / 2047;
		if (dac > 4095)
		{
			dac = 4095;	
		}
		g_Wave2[i] = dac;
	}
	
	dac2_InitForDMA((uint32_t)&g_Wave2, 128, _freq * 128);
}

/*
*********************************************************************************************************
*	函 数 名: dac2_SetRectWave
*	功能说明: DAC2输出方波
*	形    参: _low : 低电平时DAC, 
*			  _high : 高电平时DAC
*			  _freq : 频率 Hz
*			  _duty : 占空比 2% - 98%, 调节步数 1%
*	返 回 值: 无
*********************************************************************************************************
*/
void dac2_SetRectWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty)
{	
	uint16_t i;
	TIM_Cmd(TIM7, DISABLE);
	
	for (i = 0; i < (_duty * 128) / 100; i++)
	{
		g_Wave2[i] = _high;
	}
	for (; i < 128; i++)
	{
		g_Wave2[i] = _low;
	}
	
	dac2_InitForDMA((uint32_t)&g_Wave2, 128, _freq * 128);
}

/*
*********************************************************************************************************
*	函 数 名: dac2_SetTriWave
*	功能说明: DAC1输出三角波
*	形    参: _low : 低电平时DAC, 
*			  _high : 高电平时DAC
*			  _freq : 频率 Hz
*			  _duty : 占空比
*	返 回 值: 无
*********************************************************************************************************
*/
void dac2_SetTriWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty)
{	
	uint32_t i;
	uint16_t dac;
	uint16_t m;
	
	TIM_Cmd(TIM7, DISABLE);
		
	/* 构造三角波数组，128个样本，从 _low 到 _high */		
	m = (_duty * 128) / 100;
	
	if (m == 0)
	{
		m = 1;
	}
	
	if (m > 127)
	{
		m = 127;
	}
	for (i = 0; i < m; i++)
	{
		dac = _low + ((_high - _low) * i) / m;
		g_Wave2[i] = dac;
	}
	for (; i < 128; i++)
	{
		dac = _high - ((_high - _low) * (i - m)) / (128 - m);
		g_Wave2[i] = dac;
	}	
	
	dac2_InitForDMA((uint32_t)&g_Wave2, 128, _freq * 128);
}

/*
*********************************************************************************************************
*	函 数 名: dac2_StopWave
*	功能说明: 停止DAC2输出
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void dac2_StopWave(void)
{	
	DMA_Cmd(DMA2_Stream6, DISABLE);
	DAC_DMACmd(DAC_Channel_2, DISABLE);
	TIM_Cmd(TIM7, DISABLE);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
