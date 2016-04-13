/*
*********************************************************************************************************
*
*	模块名称 : 逆运动学计算模块
*	文件名称 : inverse_knematic.c
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
#include "inverse_knematic.h"

extern unsigned short  D1_Length ;      //臀关节到立足点距离,初始值为复位状态的值
extern unsigned short  D2_Length ;      //L2\L3组成的三角形的第三条边长,初始值为复位状态的值

static 	    Matrix3f  dcm;
static  	  Matrix3f  dcm_t;
static      Matrix3f  jacobi;
/****************************************************************************
*函数原型:	   float invSqrt(float x)
*功　　能:	   快速计算 1/Sqrt(x) 	
输入参数： 要计算的值
输出参数： 结果
*******************************************************************************/
static float invSqrt(float x) {
			float halfx = 0.5f * x;
			float y = x;
			long i = *(long*)&y;
			i = 0x5f3759df - (i>>1);
			y = *(float*)&i;
			y = y * (1.5f - (halfx * y * y));
			return y;
}
/*********************************************************************************************************
*	函 数 名: get_Hip_angle
*	功能说明: 经过逆运动学计算出臀关节的角度
*          求得的Hip角实际上不是D-H坐标系中的hip角  ，求得的hip + 身体中心到第一关节的连线与XB的夹角 = D-H坐标系下的hip角
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/					
void get_hip_angle(LEG* leg, Position* p)						
{
//	     float cth0,sth0,cth1,sth1;
//	     float temp,temp0,temp1,temp2;
//	     float temp02,temp12,temp2s;
//	     int d0 ;
	     float temp, angle;
	     float x = p->x;
	     float y = p->y;
//	     
//	     switch(leg->id)
//			 {
//				 case ID_LEG_1:
//					              cth0 = COSTH1;
//				                sth0 = SINTH1;
//					 break;
//				 case ID_LEG_2:
//					              cth0 = COSTH2;
//				                sth0 = SINTH2;
//					 break;
//				 case ID_LEG_3:
//					              cth0 = COSTH3;
//				                sth0 = SINTH3;
//					 break;
//				 case ID_LEG_4:
//					              cth0 = COSTH4;
//				                sth0 = SINTH4;
//					 break;
//				 case ID_LEG_5:
//					              cth0 = COSTH5;
//				                sth0 = SINTH5;
//					 break;
//				 case ID_LEG_6:
//					              cth0 = COSTH6;
//				                sth0 = SINTH6;
//					 break;
//			 }
//			 
//			 temp0 = x - L0*cth0; temp02 = temp0*temp0;
//			 temp1 = y - L0*sth0; temp12 = temp1*temp1;
//			 temp2 = temp02 + temp12; temp2s = invSqrt(temp2);
//			 d0 = x*x + y*y;
//			 
//			 cth1 = (d0 - temp2 - L0L0) * temp2s / L0X2;
//			 if( y >= 0)
//			 {
//				 sth1 = sqrt(1-cth1*cth1);
//			 }
//			 else
//			 {
//				 sth1 = -sqrt(1-cth1*cth1);
//			 }
			 
			 temp = PI - atan2(y, x);
			 
			 switch(leg->id)
			 {
				 case ID_LEG_1:
              angle = temp * RAD_TO_DEGREE - HIP_1_DEF_ANGLE;
					 break;
				 case ID_LEG_2:
							angle = temp * RAD_TO_DEGREE - HIP_2_DEF_ANGLE;
					 break;
				 case ID_LEG_3:
							angle = temp * RAD_TO_DEGREE - HIP_3_DEF_ANGLE;
					 break;
				 case ID_LEG_4:
							angle = temp * RAD_TO_DEGREE - HIP_4_DEF_ANGLE;
					 break;
				 case ID_LEG_5:
							angle = temp * RAD_TO_DEGREE - HIP_5_DEF_ANGLE;
					 break;
				 case ID_LEG_6:
							angle = temp * RAD_TO_DEGREE - HIP_6_DEF_ANGLE;
					 break;
			 }
			 leg->hip_angle = constrain_180(angle);
			 
}
/*********************************************************************************************************
*	函 数 名: get_Knee_angle
*	功能说明: 经过逆运动学计算出膝关节的角度
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/					
void get_knee_angle(LEG*leg, Position* p)						
{
	     float d0,d02;
	     float temp,temp0, temp1;
	     float cth2,sth2;
	     float  height = fabs(p->z);
	     float angle;
	
	     d0 = D1_Length - L1_Length;
	     d02 = d0 * d0;
	     temp0 = height * height;
	     temp1 = invSqrt(temp0 + d02);
	     
	     cth2 = (L2L2 + d02 + temp0 - L3L3 )* temp1 /L2X2;
	     sth2 = sqrt(1- cth2*cth2);
	
	     temp = atan2(sth2, cth2) - atan2(height, d0);
	     
			 angle = temp * RAD_TO_DEGREE - KNEE_DEF_ANGLE;
	    
	    leg->knee_angle = constrain_180(angle);
}
/*********************************************************************************************************
*	函 数 名: get_Ankle_angle
*	功能说明: 经过逆运动学计算出踝关节的角度
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/					
void get_ankle_angle(LEG* leg, Position* p)						
{
	     float cth3,sth3;
	     float temp,temp0,temp1,temp12,temp2;
	     float height = fabs(p->z);
	     float angle;
	
	     temp0 = height * height;
	     temp1 = D1_Length - L1_Length;
	     temp12 = temp1 * temp1;
	     temp2 = temp0 + temp12;
	     
	     cth3 = (temp2 - L2L2 - L3L3)/L2L3X2;
	     sth3 = sqrt(1-cth3*cth3);
	    
	     temp = atan2(sth3, cth3);
	
	     angle = temp * RAD_TO_DEGREE - ANKLE_DEF_ANGLE;
	
	     leg->ankle_angle = constrain_180(angle);
}
/*********************************************************************************************************
*	函 数 名: set_position
*	功能说明: 设置期望位置
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/					
void set_position(Position*p, float x_e, float y_e, float z_e)						
{
			p->x = x_e;
	    p->y = y_e;
	    p->z = z_e;
}
/*********************************************************************************************************
*	函 数 名: angle_to_pwm
*	功能说明: 相关关节的角度换算成pwm值
*	形    参：
*	返 回 值: 0:成功     -1:失败
*********************************************************************************************************
*/					
void angle_to_pwm(LEG*leg)						
{
			int   pwm_hip,pwm_knee,pwm_ankle ;
	    float  angle_hip, angle_knee, angle_ankle;
	 
	    angle_hip = leg->hip_angle;
	    angle_knee = leg->knee_angle;
	    angle_ankle = leg->ankle_angle;
	
	    switch(leg->id)
			{
				case ID_LEG_1:
						pwm_hip = ANGLE_TO_PWM(angle_hip);
						leg->hip_pwm = PWM_MID + pwm_hip;

						pwm_knee = ANGLE_TO_PWM(angle_knee);
						leg->knee_pwm = PWM_MID + pwm_knee;

						pwm_ankle = ANGLE_TO_PWM(angle_ankle);
						leg->ankle_pwm = PWM_MID + pwm_ankle;
					break;
				case ID_LEG_2:
						pwm_hip = ANGLE_TO_PWM(angle_hip);
						leg->hip_pwm = PWM_MID + pwm_hip;

						pwm_knee = ANGLE_TO_PWM(angle_knee);
						leg->knee_pwm = PWM_MID + pwm_knee;

						pwm_ankle = ANGLE_TO_PWM(angle_ankle);
						leg->ankle_pwm = PWM_MID + pwm_ankle;
					break;
				case ID_LEG_3:
						pwm_hip = ANGLE_TO_PWM(angle_hip);
						leg->hip_pwm = PWM_MID + pwm_hip;

						pwm_knee = ANGLE_TO_PWM(angle_knee);
						leg->knee_pwm = PWM_MID + pwm_knee;

						pwm_ankle = ANGLE_TO_PWM(angle_ankle);
						leg->ankle_pwm = PWM_MID + pwm_ankle;
					break;
				case ID_LEG_4:
						pwm_hip = ANGLE_TO_PWM(angle_hip);
						leg->hip_pwm = PWM_MID + pwm_hip;

						pwm_knee = ANGLE_TO_PWM(angle_knee);
						leg->knee_pwm = PWM_MID - pwm_knee;

						pwm_ankle = ANGLE_TO_PWM(angle_ankle);
						leg->ankle_pwm = PWM_MID - pwm_ankle;
					break;
				case ID_LEG_5:
						pwm_hip = ANGLE_TO_PWM(angle_hip);
						leg->hip_pwm = PWM_MID + pwm_hip;

						pwm_knee = ANGLE_TO_PWM(angle_knee);
						leg->knee_pwm = PWM_MID - pwm_knee;

						pwm_ankle = ANGLE_TO_PWM(angle_ankle);
						leg->ankle_pwm = PWM_MID - pwm_ankle;
					break;
				case ID_LEG_6:
						pwm_hip = ANGLE_TO_PWM(angle_hip);
						leg->hip_pwm = PWM_MID + pwm_hip;

						pwm_knee = ANGLE_TO_PWM(angle_knee);
						leg->knee_pwm = PWM_MID - pwm_knee;

						pwm_ankle = ANGLE_TO_PWM(angle_ankle);
						leg->ankle_pwm = PWM_MID - pwm_ankle;
					break;
			}

}

/*********************************************************************************************************
*	函 数 名: translate_center
*	功能说明: 平移机体重心
*	形    参：yaw:方向角(rad)    stride :步长
*	返 回 值: 
*********************************************************************************************************
*/					
void translate_center(float yaw, float stride, Position** p)						
{
			float   cos_yaw,sin_yaw;
	    unsigned char i;
			cos_yaw = cos(yaw);
		  sin_yaw = sin(yaw);
	
			for(i =0; i < ID_MAX; i ++)
			{
					p[i]->x -= stride*cos_yaw;
					p[i]->y -= stride*sin_yaw;
	
			}
}

/*********************************************************************************************************
*	函 数 名: update_world_position
*	功能说明: 更新大地坐标系下的位置向量,整个机体
*	形    参：angle:旋转角度rad  
*	返 回 值: 
*********************************************************************************************************
*/					
void update_world_position(float angle, Position** p_b, Position** p_w)	
{
			unsigned char  i ;
	
	    update_DCM(angle, &dcm);
			for(i = 0; i < ID_MAX; i ++)
			{
         *p_w[i] = DCM_Multiply_Position(*p_b++, &dcm);
			}
}
/*********************************************************************************************************
*	函 数 名: update_world_position_one
*	功能说明: 更新大地坐标系下的位置向量,一条腿的
*	形    参：angle:旋转角度rad  
*	返 回 值: 
*********************************************************************************************************
*/					
void update_world_position_one(float angle, Position* p_b, Position* p_w)	
{
	
	    update_DCM(angle, &dcm);
       
	    *p_w = DCM_Multiply_Position(p_b, &dcm);

}
/*********************************************************************************************************
*	函 数 名: update_body_position
*	功能说明: 更新机体坐标系下的位置向量
*	形    参：angle:旋转角度rad  
*	返 回 值: 
*********************************************************************************************************
*/					
void update_body_position(float angle, Position** p_b, Position** p_w)						
{
			unsigned char  i;
	
	    update_DCM_T(angle, &dcm_t);
			for(i = 0; i < ID_MAX; i ++)
			{
         *p_b[i] = DCM_Multiply_Position(*p_w++, &dcm_t);
	        
			}
}

/*********************************************************************************************************
*	函 数 名: update_body_position_one
*	功能说明: 更新机体坐标系下的位置向量,一条腿
*	形    参：angle:旋转角度rad  
*	返 回 值: 
*********************************************************************************************************
*/					
void update_body_position_one(float angle, Position* p_b, Position* p_w)						
{
	
	    update_DCM_T(angle, &dcm_t);
	
      *p_b = DCM_Multiply_Position(p_w, &dcm_t);
	        
}
/*********************************************************************************************************
*	函 数 名: calc_gait
*	功能说明: 根据前进方向角更新步态
*	形    参：yaw:方向角  
*	返 回 值: 
*********************************************************************************************************
*/	
//void calc_gait(float yaw)
//{
//	/*
////	    if(yaw >= 0 && yaw <= CRITICAL_ALPHA_1)
////	    {

////				
////				return ;
////			}
////			if(yaw >CRITICAL_ALPHA_1 && yaw <= PI_1_2)
////			{

////				
////				return ;
////			}
////			if(yaw > PI_1_2 && yaw <= CRITICAL_ALPHA_2)
////			{
////				
////				return ;
////			}
////			if(yaw > CRITICAL_ALPHA_2 && yaw <= PI)
////			{
////				 
////				return ;
////			}
////			if(yaw > PI && yaw <= CRITICAL_ALPHA_3)
////			{
////				 
////				return ;
////			}
////			if(yaw > CRITICAL_ALPHA_3 && yaw <= PI_3_2)
////			{
////				 
////				return ;
////			}
////			if(yaw > PI_3_2 && yaw <= CRITICAL_ALPHA_4)
////			{
////				 
////				return ;
////			}
////			if(yaw > CRITICAL_ALPHA_4 && yaw <= PI_2)
////			{
////				 
////				return ;
////			}
//*/
//}
/***************************** 阿波罗科技 www.apollorobot.com (END OF FILE) *********************************/
