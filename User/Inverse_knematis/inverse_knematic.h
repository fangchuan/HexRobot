/*
*********************************************************************************************************
*
*	ģ������ : ���˶�ѧ����ģ��
*	�ļ����� : inverse_knematic.h
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
#ifndef __INVERSE_KNEMATIC_H
#define __INVERSE_KNEMATIC_H

#include "common.h"

extern void get_hip_angle(LEG*leg, Position* p);	
extern void get_knee_angle(LEG*leg, Position* p);
extern void get_ankle_angle(LEG* leg, Position* p)	;
extern void set_position(Position*p, float x_e, float y_e, float z_e);
extern void angle_to_pwm(LEG*leg);
extern void translate_center(float yaw, float stride, Position** p);
extern void update_world_position(float angle, Position** p_b, Position** p_w);
extern void update_world_position_one(float angle, Position* p_b, Position* p_w);
extern void update_body_position(float angle, Position** p_b, Position** p_w);
extern void update_body_position_one(float angle, Position* p_b, Position* p_w);
#endif
/***************************** �����޿Ƽ� www.apollorobot.com (END OF FILE) *********************************/
