/*
*********************************************************************************************************
*
*	模块名称 : 逆运动学计算模块
*	文件名称 : inverse_knematic.h
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
/***************************** 阿波罗科技 www.apollorobot.com (END OF FILE) *********************************/
