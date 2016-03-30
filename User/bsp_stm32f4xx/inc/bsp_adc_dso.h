/*
*********************************************************************************************************
*
*	ģ������ : ʾ����ģ��ADC�ײ������
*	�ļ����� : bsp_adc_dso.h
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_ADC_DSO_H
#define __BSP_ADC_DSO_H

/* ѡ��ʾ����ģ���ͺ� */
//#define D112_1
#define D112_2

void DSO_ConfigCtrlGPIO(void);
void DSO_SetDC(uint8_t _ch, uint8_t _mode);
void DSO_SetGain(uint8_t _ch, uint8_t _gain);

void DSO_StartADC(uint32_t _uiBufAddr1, uint32_t _uiBufAddr2, uint32_t _uiSampleCount, uint32_t _uiSampleFreq);
void DSO_PauseADC(void);
void DSO_StopADC(void);

void DSO_SetSampRate(uint32_t _ulFreq);

#endif
