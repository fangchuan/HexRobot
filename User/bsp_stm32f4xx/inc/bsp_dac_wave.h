	/*
*********************************************************************************************************
*
*	ģ������ : DAC�������
*	�ļ����� : dac_wave.h
*	��    �� : V1.0
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_DAC_WAVE_H
#define __BSP_DAC_WAVE_H

/* ��Щ������ͨ�õ����ã��������DAC���� */
void bsp_InitDAC1(void);
void bsp_SetDAC1(uint16_t _dac);

void bsp_InitDAC2(void);
void bsp_SetDAC2(uint16_t _dac);

/* ����ĺ�������DMA���η����� */
void dac1_InitForDMA(uint32_t _BufAddr, uint32_t _Count, uint32_t _DacFreq);
void dac1_SetSinWave(uint16_t _vpp, uint32_t _freq);
void dac1_SetRectWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty);
void dac1_SetTriWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty);
void dac1_StopWave(void);

void dac2_InitForDMA(uint32_t _BufAddr, uint32_t _Count, uint32_t _DacFreq);
void dac2_SetSinWave(uint16_t _vpp, uint32_t _freq);
void dac2_SetRectWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty);
void dac2_SetTriWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty);
void dac2_StopWave(void);

extern uint16_t g_Wave1[128];
extern uint16_t g_Wave2[128];

#endif
