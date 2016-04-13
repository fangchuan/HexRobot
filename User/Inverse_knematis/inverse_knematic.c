/*
*********************************************************************************************************
*
*	ģ������ : ���˶�ѧ����ģ��
*	�ļ����� : inverse_knematic.c
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
#include "inverse_knematic.h"

extern unsigned short  D1_Length ;      //�ιؽڵ���������,��ʼֵΪ��λ״̬��ֵ
extern unsigned short  D2_Length ;      //L2\L3��ɵ������εĵ������߳�,��ʼֵΪ��λ״̬��ֵ

static 	    Matrix3f  dcm;
static  	  Matrix3f  dcm_t;
static      Matrix3f  jacobi;
/****************************************************************************
*����ԭ��:	   float invSqrt(float x)
*��������:	   ���ټ��� 1/Sqrt(x) 	
��������� Ҫ�����ֵ
��������� ���
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
*	�� �� ��: get_Hip_angle
*	����˵��: �������˶�ѧ������ιؽڵĽǶ�
*          ��õ�Hip��ʵ���ϲ���D-H����ϵ�е�hip��  ����õ�hip + �������ĵ���һ�ؽڵ�������XB�ļн� = D-H����ϵ�µ�hip��
*	��    �Σ�
*	�� �� ֵ: 
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
*	�� �� ��: get_Knee_angle
*	����˵��: �������˶�ѧ�����ϥ�ؽڵĽǶ�
*	��    �Σ�
*	�� �� ֵ: 
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
*	�� �� ��: get_Ankle_angle
*	����˵��: �������˶�ѧ������׹ؽڵĽǶ�
*	��    �Σ�
*	�� �� ֵ: 
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
*	�� �� ��: set_position
*	����˵��: ��������λ��
*	��    �Σ�
*	�� �� ֵ: 
*********************************************************************************************************
*/					
void set_position(Position*p, float x_e, float y_e, float z_e)						
{
			p->x = x_e;
	    p->y = y_e;
	    p->z = z_e;
}
/*********************************************************************************************************
*	�� �� ��: angle_to_pwm
*	����˵��: ��عؽڵĽǶȻ����pwmֵ
*	��    �Σ�
*	�� �� ֵ: 0:�ɹ�     -1:ʧ��
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
*	�� �� ��: translate_center
*	����˵��: ƽ�ƻ�������
*	��    �Σ�yaw:�����(rad)    stride :����
*	�� �� ֵ: 
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
*	�� �� ��: update_world_position
*	����˵��: ���´������ϵ�µ�λ������,��������
*	��    �Σ�angle:��ת�Ƕ�rad  
*	�� �� ֵ: 
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
*	�� �� ��: update_world_position_one
*	����˵��: ���´������ϵ�µ�λ������,һ���ȵ�
*	��    �Σ�angle:��ת�Ƕ�rad  
*	�� �� ֵ: 
*********************************************************************************************************
*/					
void update_world_position_one(float angle, Position* p_b, Position* p_w)	
{
	
	    update_DCM(angle, &dcm);
       
	    *p_w = DCM_Multiply_Position(p_b, &dcm);

}
/*********************************************************************************************************
*	�� �� ��: update_body_position
*	����˵��: ���»�������ϵ�µ�λ������
*	��    �Σ�angle:��ת�Ƕ�rad  
*	�� �� ֵ: 
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
*	�� �� ��: update_body_position_one
*	����˵��: ���»�������ϵ�µ�λ������,һ����
*	��    �Σ�angle:��ת�Ƕ�rad  
*	�� �� ֵ: 
*********************************************************************************************************
*/					
void update_body_position_one(float angle, Position* p_b, Position* p_w)						
{
	
	    update_DCM_T(angle, &dcm_t);
	
      *p_b = DCM_Multiply_Position(p_w, &dcm_t);
	        
}
/*********************************************************************************************************
*	�� �� ��: calc_gait
*	����˵��: ����ǰ������Ǹ��²�̬
*	��    �Σ�yaw:�����  
*	�� �� ֵ: 
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
/***************************** �����޿Ƽ� www.apollorobot.com (END OF FILE) *********************************/
