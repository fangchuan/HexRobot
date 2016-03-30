/*
*********************************************************************************************************
*
*	ģ������ : �����˶�������ģ��
*	�ļ����� : control.h
*	��    �� : V1.0
*	˵    �� : 
*	 	       Ӧ�ó���ֻ�� #include bsp.h ���ɣ�����Ҫ#include ÿ��ģ��� h �ļ�
*
*	�޸ļ�¼ :
*		�汾��  ����         ����    ˵��
*		v1.0    2016-3-24    ����  ST�̼���V1.0.2�汾��
*	
*********************************************************************************************************
*/
#ifndef __CONTROL_H
#define __CONTROL_H

#include "includes.h"


#define HIP_1_OUT    TIM5->CCR1
#define HIP_2_OUT    TIM5->CCR2
#define HIP_3_OUT    TIM5->CCR3
#define HIP_4_OUT    TIM5->CCR4
#define HIP_5_OUT    TIM4->CCR3
#define HIP_6_OUT    TIM4->CCR4

#define KNEE_1_OUT   TIM1->CCR3
#define KNEE_2_OUT   TIM1->CCR4
#define KNEE_3_OUT   TIM9->CCR1
#define KNEE_4_OUT   TIM9->CCR2
#define KNEE_5_OUT   TIM1->CCR1
#define KNEE_6_OUT   TIM1->CCR2

#define ANKLE_1_OUT  TIM3->CCR1
#define ANKLE_2_OUT  TIM3->CCR2
#define ANKLE_3_OUT  TIM3->CCR3
#define ANKLE_4_OUT  TIM3->CCR4
#define ANKLE_5_OUT  TIM4->CCR1
#define ANKLE_6_OUT  TIM4->CCR2

#define PWM_MAX  		1125
#define PWM_MIN  		225
#define PWM_TO_ANGLE	((PWM_MAX-PWM_MIN)/180) 

#define ACTION_SPEED 	8		 //range:0 - 10
#define SHRINK_SPEED 	((u8)((0.6)*ACTION_SPEED))
#define ACTION_DELAY 	10      //ms	    //20
#define TURN_DELAY 		3      //4
#define STRAIGHT_DELAY 	3      //ms	   //3
#define STAMP_DELAY 	3      //stamp delay
#define STAND_HIGHT 	200
#define SHRINK_DEGREE 	((u8)((0.6)*STAND_HIGHT))

#endif

/***************************** �����޿Ƽ� www.apollorobot.com (END OF FILE) *********************************/
