/*
*********************************************************************************************************
*
*	ģ������ : ADCģ��
*	�ļ����� : bsp_adc.h
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
#ifndef  _BSP_ADC_H
#define  _BSP_ADC_H

#include "stm32f4xx.h"

//ÿ��ģ�⴫������Ӧ��ADCͨ��
#define  ANOLOG_Sensor_1   10
#define  ANOLOG_Sensor_2   11
#define  ANOLOG_Sensor_3   12
#define  ANOLOG_Sensor_4   13
#define  ADC_CH_TEMP       16


extern void  bsp_adc_init(void);
extern float Get_adc(u8 ch) ;
extern u16 Get_Adc_Average(u8 ch,u8 times);

#endif /*_BSP_ADC_H*/
/***************************** �����޿Ƽ� www.apollorobot.com (END OF FILE) *********************************/
