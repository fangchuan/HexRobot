/*
*********************************************************************************************************
*
*	ģ������ : �ײ�����ģ��
*	�ļ����� : bsp.h
*	��    �� : V1.0
*	˵    �� : ���ǵײ�����ģ�����е�h�ļ��Ļ����ļ���
*	 	       Ӧ�ó���ֻ�� #include bsp.h ���ɣ�����Ҫ#include ÿ��ģ��� h �ļ�
*
*	�޸ļ�¼ :
*		�汾��  ����         ����    ˵��
*		v1.0    2012-12-17  Eric2013  ST�̼���V1.0.2�汾��
*	
*********************************************************************************************************
*/

#ifndef _BSP_H_
#define _BSP_H_

#define STM32_V5
//#define STM32_X3


/* ����Ƿ����˿������ͺ� */
#if !defined (STM32_V5) && !defined (STM32_X3)
	#error "Please define the board model : STM32_X3 or STM32_V5"
#endif

/* ���� BSP �汾�� */
#define __STM32F1_BSP_VERSION		"1.1"

/* CPU����ʱִ�еĺ��� */
//#define CPU_IDLE()		bsp_Idle()

/* ʹ����Դ�ļ���ʹ��uCOS-III�ĺ���, �����Դ�ļ���Ҫ��ָBSP�����ļ� */
#define uCOS_EN       1

#if uCOS_EN == 1    
	#include "os.h"   

	#define  ENABLE_INT()      OS_CRITICAL_EXIT()     /* ʹ��ȫ���ж� */
	#define  DISABLE_INT()     OS_CRITICAL_ENTER()    /* ��ֹȫ���ж� */
#else
	/* ����ȫ���жϵĺ� */
	#define ENABLE_INT()	__set_PRIMASK(0)	/* ʹ��ȫ���ж� */
	#define DISABLE_INT()	__set_PRIMASK(1)	/* ��ֹȫ���ж� */
#endif

/* ���������ڵ��Խ׶��Ŵ� */
#define BSP_Printf		printf
//#define BSP_Printf(...)

#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef TRUE
	#define TRUE  1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

/*
	EXTI9_5_IRQHandler ���жϷ�������ɢ�ڼ��������� bsp�ļ��С�
	��Ҫ���ϵ� stm32f4xx_it.c �С�

	���������б�ʾEXTI9_5_IRQHandler��ں������зŵ� stm32f4xx_it.c��
*/
#define EXTI9_5_ISR_MOVE_OUT

#define DEBUG_GPS_TO_COM1	/* ��ӡGPS���ݵ�����1 */

/* ͨ��ȡ��ע�ͻ������ע�͵ķ�ʽ�����Ƿ�����ײ�����ģ�� */
#include "bsp_uart_fifo.h"
#include "bsp_led.h"
#include "bsp_timer.h"
#include "bsp_key.h"
#include "bsp_tim_pwm.h"
#include "bsp_cpu_flash.h"
#include "bsp_sdio_sd.h"
#include "bsp_i2c_gpio.h"
#include "bsp_spi_bus.h"
#include "bsp_spi_nrf.h"
#include "bsp_spi_flash.h"
#include "bsp_tim_pwm.h"
#include "bsp_ioctl.h"
//#include "bsp_eeprom_24xx.h"
//#include "bsp_si4730.h"
//#include "bsp_hmc5883l.h"
//#include "bsp_mpu6050.h"
//#include "bsp_bh1750.h"
//#include "bsp_bmp085.h"
//#include "bsp_wm8978.h"
//#include "bsp_gt811.h"
//#include "bsp_ts_ft5x06.h"
//#include "LCD_RA8875.h"
//#include "LCD_SPFD5420.h"
//#include "LCD_ILI9488.h"
//#include "bsp_ra8875_port.h"
//#include "bsp_tft_lcd.h"
//#include "bsp_touch.h"
//#include "bsp_spi_flash.h"
//#include "bsp_tsc2046.h"
//#include "bsp_ra8875_flash.h"



void bsp_Init(void);
void bsp_DelayMS(uint32_t _ulDelayTime);
void bsp_DelayUS(uint32_t _ulDelayTime);
void BSP_Tick_Init (void);
static void NVIC_Configuration(void);

#endif

/***************************** �����޿Ƽ� www.apollorobot.com (END OF FILE) *********************************/
