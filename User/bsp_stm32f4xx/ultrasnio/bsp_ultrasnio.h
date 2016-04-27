/*
*********************************************************************************************************
*
*	模块名称 : 超声波模块
*	文件名称 : bsp_ultrasnio.h
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
#ifndef  __BSP_ULTRASNIO_H
#define  __BSP_ULTRASNIO_H
#include  "bsp.h"

#define  ULTRASNIO_TRIG_1        GPIO_Pin_12
#define  ULTRASNIO_ECHO_1				 GPIO_Pin_14
#define  ECHO_1_PinSource        GPIO_PinSource14
#define  ULTRASNIO_TRIG_2        GPIO_Pin_13
#define  ULTRASNIO_ECHO_2				 GPIO_Pin_15
#define  ECHO_2_PinSource        GPIO_PinSource15
#define  ULTRASNIO_PORT          GPIOB
#define  ULTRASNIO_AF_TIM        GPIO_AF_TIM12
#define  NVIC_ULTRASNIO_PP       14
#define  NVIC_ULTRASNIO_SP       0

#define US_1_ECHO_CHANNEL        TIM_Channel_1
#define US_2_ECHO_CHANNEL        TIM_Channel_2

#define  Ultrasnio_Trigger1_H   digitalHi(ULTRASNIO_PORT,ULTRASNIO_TRIG_1)
#define  Ultrasnio_Trigger1_L	  digitalLo(ULTRASNIO_PORT,ULTRASNIO_TRIG_1)
#define  Ultrasnio_Trigger2_H   digitalHi(ULTRASNIO_PORT,ULTRASNIO_TRIG_2)
#define  Ultrasnio_Trigger2_L	  digitalLo(ULTRASNIO_PORT,ULTRASNIO_TRIG_2)

#define  Ultranio_Echo_1         GPIO_ReadInputDataBit(ULTRASNIO_PORT , ULTRASNIO_ECHO_1)
#define  Ultranio_Echo_2         GPIO_ReadInputDataBit(ULTRASNIO_PORT , ULTRASNIO_ECHO_2)

extern void bsp_ultrasnio_init(void);
extern void Ultrasnio_1_update(void);
extern void Ultrasnio_2_update(void);

#endif /*_BSP_ULTRASNIO_H*/

/***************************** 阿波罗科技 www.apollorobot.com (END OF FILE) *********************************/
