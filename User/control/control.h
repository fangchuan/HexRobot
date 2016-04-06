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

#define PWM_MAX  		2500
#define PWM_MIN  		500
#define PWM_MID     1500
#define ANGLE_MAX   250
#define ANGLE_MIN   0
#define PARAMETER            (float)((PWM_MAX-PWM_MIN)/ANGLE_MAX) 
#define DEGREE_TO_RAD        0.0174
#define RAD_TO_DEGREE        57.29
//��λ״̬�¹ؽڽǶȵ�ֵ
#define HIP_MID_ANGLE        30
#define HIP_MID_ANGLE_LEG25  1  //Leg2\5��Hip��λֵΪ1��
#define KNEE_MID_ANGLE       75
#define ANKLE_MID_ANGLE      122

#define ANGLE_TO_PWM(angle)  (angle*PARAMETER)
#define PWM_TO_ANGLE(pwm)    (float)((pwm - PWM_MIN)*0.25)

#define ACTION_SPEED 	8		 //range:0 - 10
#define SHRINK_SPEED 	((u8)((0.6)*ACTION_SPEED))
#define ACTION_DELAY 	10      //ms	    //20
#define TURN_DELAY 		3      //4
#define STRAIGHT_DELAY 	3      //ms	   //3
#define STAMP_DELAY 	3      //stamp delay
#define STAND_HIGHT 	200
#define SHRINK_DEGREE 	((u8)((0.6)*STAND_HIGHT))

#define ID_MIN    1 
#define ID_MAX    6
#define ID_LEG_1  1
#define ID_LEG_2  2
#define ID_LEG_3  3
#define ID_LEG_4  4
#define ID_LEG_5  5
#define ID_LEG_6  6

//
//�����ؽ�λ�õ����ݽṹ
//
typedef struct {
				int    x;
	      int    y;
	      int    z;
}Position;

//
//�������ȵ����ݽṹ
//
typedef struct {
				u8          id;//�ȵı��
	      float      hip_angle;//0--180
	      float     knee_angle;
	      float    ankle_angle;
	      int         velocity;//�ȵ��ٶ�
	      int          hip_pwm;//���������pwm
	      int         knee_pwm;
	      int        ankle_pwm;
//	      Position* position;//���λ��
}Servo;
extern void Position_Reset(int delay_time);
extern void Sit_Down(int delay_time);
extern void Stand_Up(int delay_time);
extern void Go_Straight(u8 Com_Num,u8 * Input_num);
extern void Go_Back(u8 Com_Num,u8 * Input_num);
extern void Stamp(u8 Com_Num,u8 * Input_num);
extern void Turn_Left(u8 Com_Num,u8 * Input_num);

#endif

/***************************** �����޿Ƽ� www.apollorobot.com (END OF FILE) *********************************/
