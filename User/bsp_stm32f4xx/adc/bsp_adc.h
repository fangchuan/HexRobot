/*
*********************************************************************************************************
*
*	模块名称 : ADC模块
*	文件名称 : bsp_adc.h
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
#ifndef  _BSP_ADC_H
#define  _BSP_ADC_H

#include "stm32f4xx.h"

//每个模拟传感器对应的ADC通道
#define  ANOLOG_Sensor_1   10
#define  ANOLOG_Sensor_2   11
#define  ANOLOG_Sensor_3   12
#define  ANOLOG_Sensor_4   13
#define  ADC_CH_TEMP       16


extern void  bsp_adc_init(void);
extern float Get_adc(u8 ch) ;
extern u16 Get_Adc_Average(u8 ch,u8 times);

#endif /*_BSP_ADC_H*/
/***************************** 阿波罗科技 www.apollorobot.com (END OF FILE) *********************************/
