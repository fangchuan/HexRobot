/*
*********************************************************************************************************
*
*	ģ������ : �����˿���ģ��
*	�ļ����� : control.c
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
#include "gait_control.h"
#include "common.h"
#include "posture.h"
#include "inverse_knematic.h"
/*********************************************************************
*
*       Static Data
*
**********************************************************************
*/
//����
static LEG    leg1 = {1};//
static LEG    leg2 = {2};//
static LEG    leg3 = {3};//
static LEG    leg4 = {4};//
static LEG    leg5 = {5};//
static LEG    leg6 = {6};//
//6��������ڻ�������ϵ�е�λ��
//��ʼֵ���Ǹ�λ״̬�µ�ֵ
static Position  P1_body = {-Foot_1_Reset_X, Foot_1_Reset_Y, Foot_1_Reset_Z};
static Position  P2_body = {-Foot_2_Reset_X, Foot_2_Reset_Y, Foot_2_Reset_Z};
static Position  P3_body = {-Foot_1_Reset_X, -Foot_1_Reset_Y,Foot_1_Reset_Z};
static Position  P4_body = {Foot_1_Reset_X,  -Foot_1_Reset_Y,Foot_1_Reset_Z};
static Position  P5_body = {Foot_2_Reset_X,  Foot_2_Reset_Y, Foot_2_Reset_Z};
static Position  P6_body = {Foot_1_Reset_X,  Foot_1_Reset_Y, Foot_1_Reset_Z};
//���������Ļ�������ϵ�µ�λ��ָ������
static Position* position_body[6] = {&P1_body, &P2_body, &P3_body, &P4_body, &P5_body, &P6_body};
static Position* A_position_body[3] = {&P1_body, &P3_body, &P5_body};//A���ȵ������
static Position* B_position_body[3] = {&P2_body, &P4_body, &P6_body};//B���ȵ������
//6��������ڴ������ϵ�е�λ��
//��ʼֵ����վ��״̬�µ�ֵ
static Position  P1_world = {Foot_1_Stand_X, Foot_1_Stand_Y, Foot_1_Stand_Z};
static Position  P2_world = {Foot_2_Stand_X, Foot_2_Stand_Y, Foot_2_Stand_Z};
static Position  P3_world = {Foot_3_Stand_X, Foot_3_Stand_Y, Foot_3_Stand_Z};
static Position  P4_world = {Foot_4_Stand_X, Foot_4_Stand_Y, Foot_4_Stand_Z};
static Position  P5_world = {Foot_5_Stand_X, Foot_5_Stand_Y, Foot_5_Stand_Z};
static Position  P6_world = {Foot_6_Stand_X, Foot_6_Stand_Y, Foot_6_Stand_Z}; 
//���������Ĵ������ϵ�µ�λ��ָ������
static Position* position_world[6] = {&P1_world, &P2_world, &P3_world, &P4_world, &P5_world, &P6_world};
static Position* A_position_world[3] = {&P1_world, &P3_world, &P5_world};//A���ȵ������
static Position* B_position_world[3] = {&P2_world, &P4_world, &P6_world};//B���ȵ������

//����վ��״̬�¹̶���������������
static  Position  P1_const = {Foot_1_Stand_X, Foot_1_Stand_Y, Foot_1_Stand_Z};
static  Position  P2_const = {Foot_2_Stand_X, Foot_2_Stand_Y, Foot_2_Stand_Z};
static  Position  P3_const = {Foot_3_Stand_X, Foot_3_Stand_Y, Foot_3_Stand_Z};
static  Position  P4_const = {Foot_4_Stand_X, Foot_4_Stand_Y, Foot_4_Stand_Z};
static  Position  P5_const = {Foot_5_Stand_X, Foot_5_Stand_Y, Foot_5_Stand_Z};
static  Position  P6_const = {Foot_6_Stand_X, Foot_6_Stand_Y, Foot_6_Stand_Z}; 
static  Position* position_const[6] = {&P1_const, &P2_const, &P3_const, &P4_const, &P5_const, &P6_const};
static  Position* A_position_const[3] = {&P1_const, &P3_const, &P5_const};//A���ȵ������
static  Position* B_position_const[3] = {&P2_const, &P4_const, &P6_const};//B���ȵ������
//static Position  P0_world ;//��������ϵ������

static LEG*      leg[6] ={&leg1, &leg2, &leg3, &leg4, &leg5, &leg6};//�����ȵ�ָ������
static LEG*      A_leg[3] = {&leg1, &leg3, &leg5};                  //A���ȵ�ָ������
static LEG*      B_leg[3] = {&leg2, &leg4, &leg6};                  //B���ȵ�ָ������
/*********************************************************************
*
*       Global Data
*
**********************************************************************
*/

unsigned short  D1_Length  =   130;      //�ιؽڵ���������,��ʼֵΪ��λ״̬��ֵ
unsigned short  D2_Length  =   100;      //L2\L3��ɵ������εĵ������߳�,��ʼֵΪ��λ״̬��ֵ

/*********************************************************************
*
*       Static Code
*
**********************************************************************
*/
static  void Trans_Aleg(float yaw);
static  void  Turn_Aleg(float a0, float def_angle);
static  void Trans_Bleg(float yaw);
static  void  Turn_Bleg(float a0, float def_angle);
static  void Putdown_Aleg(void);
static  void Putdown_Bleg(void);
static  void legs_up_down(Position** p, float height);
static  void Drive_Hip   (void);
static  void Drive_A_Hip (void);
static  void Drive_B_Hip (void);
static  void Drive_Knee  (void);
static  void Drive_A_Knee (void);
static  void Drive_B_Knee (void);
static  void Drive_Ankle (void);
static  void Drive_A_Ankle (void);
static  void Drive_B_Ankle (void);
static  void Drive_Legx (LEG* leg);

/*********************************************************************************************************
*	�� �� ��: legs_up_down
*	����˵��: A/B���Ⱦ����������
*	��    �Σ�height:�Ⱦ�����ʱ��Z������
*	�� �� ֵ: 
*********************************************************************************************************
*/					
static void legs_up_down(Position** p, float height)						
{
	
		  p[0]->z = height;
	    p[1]->z = height;
	    p[2]->z = height;
	    
}
/*********************************************************************************************************
*	�� �� ��: Trans_Aleg
*	����˵��: �ڶ�A����,A�������������S����������ƽ����S/2��
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void Trans_Aleg(float yaw)
{
	     float cos_yaw,sin_yaw;
	     u8                i;
	     cos_yaw = cos(yaw);
	     sin_yaw = sin(yaw);
	    
	    P1_body.x += STRIDE*cos_yaw;
      P1_body.y += STRIDE*sin_yaw;
	    P1_body.z = HEIGHT_TRANSFOOT;
	
			P3_body.x += STRIDE*cos_yaw;
	    P3_body.y += STRIDE*sin_yaw;
	    P3_body.z = HEIGHT_TRANSFOOT;

			P5_body.x += STRIDE*cos_yaw;
	    P5_body.y += STRIDE*sin_yaw;
			P5_body.z = HEIGHT_TRANSFOOT;

			for( i = 0;i < 3; i++)
			{
					get_hip_angle(A_leg[i], A_position_body[i]);	
					get_knee_angle(A_leg[i],A_position_body[i]);
					get_ankle_angle(A_leg[i],A_position_body[i]);
					angle_to_pwm(A_leg[i]);
					Drive_Legx(A_leg[i]);
			}
		
}
/*********************************************************************************************************
*	�� �� ��: Turn_Aleg
*	����˵��: �ڶ�A����,��Z��תĬ�϶����Ľ�
*	��    �Σ�a0:����ǰ��Z���ת��    def_angle:һ��ת�䶯����Ĭ��ת��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void  Turn_Aleg(float a0, float def_angle)
{	
	      u8   i;
	      
	      for(i = 0; i < 3; i ++)
	      {
						update_world_position_one(def_angle, A_position_const[i], A_position_world[i]);
						update_body_position_one (a0,   A_position_body[i], A_position_world[i]);
	      }
	      legs_up_down(A_position_body, HEIGHT_TRANSFOOT);
	
			  for( i = 0;i < 3; i ++)
			  {
						get_hip_angle(A_leg[i], A_position_body[i]);	
						get_knee_angle(A_leg[i],A_position_body[i]);
						get_ankle_angle(A_leg[i],A_position_body[i]);
						angle_to_pwm(A_leg[i]);
						Drive_Legx(A_leg[i]);
				}
}
/*********************************************************************************************************
*	�� �� ��: Putdown_Aleg
*	����˵��: ����A���ȣ�ʹA�����ŵ�
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void Putdown_Aleg(void)
{
	     u8     i;
	
			//��ɰڶ������
			legs_up_down(A_position_body, Foot_1_Stand_Z);

			for( i = 0;i < 3; i++)
			{
					get_knee_angle(A_leg[i],A_position_body[i]);
					angle_to_pwm(A_leg[i]);
					Drive_Legx(A_leg[i]);
			}
		
}

/*********************************************************************************************************
*	�� �� ��: Trans_Bleg
*	����˵��: �ڶ�B����,B�������������S����������ƽ����S��
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void Trans_Bleg(float yaw)
{
	     float cos_yaw,sin_yaw;
	     u8                i;
	     cos_yaw = cos(yaw);
	     sin_yaw = sin(yaw);

			P2_body.x += STRIDE*cos_yaw;
	    P2_body.y += STRIDE*sin_yaw;
	    P2_body.z = HEIGHT_TRANSFOOT;

			P4_body.x += STRIDE*cos_yaw;
	    P4_body.y += STRIDE*sin_yaw;
	    P4_body.z = HEIGHT_TRANSFOOT;
	
		  P6_body.x += STRIDE*cos_yaw;
	    P6_body.y += STRIDE*sin_yaw;
	    P6_body.z = HEIGHT_TRANSFOOT;
			
			for( i = 0;i < 3; i ++)
			{
					get_hip_angle(B_leg[i], B_position_body[i]);	
					get_knee_angle(B_leg[i],B_position_body[i]);
					get_ankle_angle(B_leg[i],B_position_body[i]);
					angle_to_pwm(B_leg[i]);
					Drive_Legx(B_leg[i]);
			}
			
}
/*********************************************************************************************************
*	�� �� ��: Turn_Bleg
*	����˵��: �ڶ�B����,��Z��תĬ�϶����Ľ�, B���ȵ�������Ѿ����̶��ã������ٸ��ݵ�ǰ��������ϵ�µ������λ������������ת�任
*	��    �Σ�a0:����ǰ��Z���ת��       def_angle:һ��ת�䶯����Ĭ��ת��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void  Turn_Bleg(float a0, float def_angle)
{	
	      u8   i;
	      for(i = 0; i < 3; i ++)
      	{
						update_world_position_one(def_angle, B_position_const[i], B_position_world[i]);
						update_body_position_one(a0,    B_position_body[i], B_position_world[i]);
	      }
	      legs_up_down(B_position_body, HEIGHT_TRANSFOOT);
	
			  for( i = 0;i < 3; i ++)
			  {
						get_hip_angle(B_leg[i], B_position_body[i]);	
						get_knee_angle(B_leg[i],B_position_body[i]);
						get_ankle_angle(B_leg[i],B_position_body[i]);
						angle_to_pwm(B_leg[i]);
						Drive_Legx(B_leg[i]);
				}
}
/*********************************************************************************************************
*	�� �� ��: Putdown_Bleg
*	����˵��: ����B���ȣ�ʹB�����ŵ�
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void Putdown_Bleg(void)
{
	     u8     i;
	
			//��ɰڶ������
	    legs_up_down(B_position_body, Foot_1_Stand_Z);

			for( i = 0;i < 3; i++)
			{
					get_knee_angle(B_leg[i],B_position_body[i]);
					angle_to_pwm(B_leg[i]);
					Drive_Legx(B_leg[i]);
			}
		
}
/*********************************************************************************************************
*	�� �� ��: Drive_Hip
*	����˵��: ����Hip�ؽ�
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void Drive_Hip(void)
{
	     
	     get_hip_angle(&leg1, &P1_body);
	     get_hip_angle(&leg2, &P2_body);
			 get_hip_angle(&leg3, &P3_body);
			 get_hip_angle(&leg4, &P4_body);
			 get_hip_angle(&leg5, &P5_body);
			 get_hip_angle(&leg6, &P6_body);
				
	     angle_to_pwm(&leg1);
	     angle_to_pwm(&leg2);
	     angle_to_pwm(&leg3);
	     angle_to_pwm(&leg4);
	     angle_to_pwm(&leg5);
	     angle_to_pwm(&leg6);

			 HIP_1_OUT = leg1.hip_pwm;
			 HIP_2_OUT = leg2.hip_pwm;
			 HIP_3_OUT = leg3.hip_pwm;
			 HIP_4_OUT = leg4.hip_pwm;
			 HIP_5_OUT = leg5.hip_pwm;
			 HIP_6_OUT = leg6.hip_pwm;

}
/*********************************************************************************************************
*	�� �� ��: Drive_A_Hip
*	����˵��: ����A���Hip�ؽ�
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void Drive_A_Hip(void)
{
	     
	     get_hip_angle(&leg1, &P1_body);
			 get_hip_angle(&leg3, &P3_body);
			 get_hip_angle(&leg5, &P5_body);
				
	     angle_to_pwm(&leg1);
	     angle_to_pwm(&leg3);
	     angle_to_pwm(&leg5);

			 HIP_1_OUT = leg1.hip_pwm;
			 HIP_3_OUT = leg3.hip_pwm;
			 HIP_5_OUT = leg5.hip_pwm;

}
/*********************************************************************************************************
*	�� �� ��: Drive_B_Hip
*	����˵��: ����B Group Hip�ؽ�
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void Drive_B_Hip(void)
{
	     
	     get_hip_angle(&leg2, &P2_body);
			 get_hip_angle(&leg4, &P4_body);
			 get_hip_angle(&leg6, &P6_body);
				
	     angle_to_pwm(&leg2);
	     angle_to_pwm(&leg4);
	     angle_to_pwm(&leg6);

			 HIP_2_OUT = leg2.hip_pwm;
			 HIP_4_OUT = leg4.hip_pwm;
			 HIP_6_OUT = leg6.hip_pwm;

}
/*********************************************************************************************************
*	�� �� ��: Drive_Knee
*	����˵��: ����Knee�ؽ�
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void Drive_Knee (void)
{
				get_knee_angle(&leg1, &P1_body);
				get_knee_angle(&leg2, &P2_body);
			  get_knee_angle(&leg3, &P3_body);
				get_knee_angle(&leg4, &P4_body);
			  get_knee_angle(&leg5, &P5_body);
				get_knee_angle(&leg6, &P6_body);
	
			  angle_to_pwm(&leg1);
				angle_to_pwm(&leg2);
			  angle_to_pwm(&leg3);
				angle_to_pwm(&leg4);
			  angle_to_pwm(&leg5);
				angle_to_pwm(&leg6);

				KNEE_1_OUT = leg1.knee_pwm;
				KNEE_2_OUT = leg2.knee_pwm;
				KNEE_3_OUT = leg3.knee_pwm;
				KNEE_4_OUT = leg4.knee_pwm;
				KNEE_5_OUT = leg5.knee_pwm;
				KNEE_6_OUT = leg6.knee_pwm;

}
/*********************************************************************************************************
*	�� �� ��: Drive_A_Knee
*	����˵��: ����A Group Knee�ؽ�
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void Drive_A_Knee (void)
{
				get_knee_angle(&leg1, &P1_body);
			  get_knee_angle(&leg3, &P3_body);
			  get_knee_angle(&leg5, &P5_body);
	
			  angle_to_pwm(&leg1);
			  angle_to_pwm(&leg3);
			  angle_to_pwm(&leg5);

				KNEE_1_OUT = leg1.knee_pwm;
				KNEE_3_OUT = leg3.knee_pwm;
        KNEE_5_OUT = leg5.knee_pwm;

}
/*********************************************************************************************************
*	�� �� ��: Drive_B_Knee
*	����˵��: ����B Group Knee�ؽ�
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void Drive_B_Knee (void)
{
				get_knee_angle(&leg2, &P2_body);
			  get_knee_angle(&leg4, &P4_body);
			  get_knee_angle(&leg6, &P6_body);
	
			  angle_to_pwm(&leg2);
			  angle_to_pwm(&leg4);
			  angle_to_pwm(&leg6);

				KNEE_2_OUT = leg2.knee_pwm;
				KNEE_4_OUT = leg4.knee_pwm;
				KNEE_6_OUT = leg6.knee_pwm;

}
/*********************************************************************************************************
*	�� �� ��: Drive_Ankle
*	����˵��: ����ANkle�ؽ�
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void Drive_Ankle (void)
{
	     	get_ankle_angle(&leg1, &P1_body);
				get_ankle_angle(&leg2, &P2_body);
			  get_ankle_angle(&leg3, &P3_body);
				get_ankle_angle(&leg4, &P4_body);
			  get_ankle_angle(&leg5, &P5_body);
				get_ankle_angle(&leg6, &P6_body);
	
			  angle_to_pwm(&leg1);
				angle_to_pwm(&leg2);
			  angle_to_pwm(&leg3);
				angle_to_pwm(&leg4);
			  angle_to_pwm(&leg5);
				angle_to_pwm(&leg6);
	
			 ANKLE_1_OUT = leg1.ankle_pwm;
			 ANKLE_2_OUT = leg2.ankle_pwm;
			 ANKLE_3_OUT = leg3.ankle_pwm;
			 ANKLE_4_OUT = leg4.ankle_pwm;
			 ANKLE_5_OUT = leg5.ankle_pwm;
			 ANKLE_6_OUT = leg6.ankle_pwm;
}
/*********************************************************************************************************
*	�� �� ��: Drive_A_Ankle
*	����˵��: ����A group ankle�ؽ�
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void Drive_A_Ankle (void)
{
	     get_ankle_angle(&leg1, &P1_body);
			 get_ankle_angle(&leg3, &P3_body);
			 get_ankle_angle(&leg5, &P5_body);
	
			 angle_to_pwm(&leg1);
			 angle_to_pwm(&leg3);
			 angle_to_pwm(&leg5);
	
			 ANKLE_1_OUT = leg1.ankle_pwm;
			 ANKLE_3_OUT = leg3.ankle_pwm;
			 ANKLE_5_OUT = leg5.ankle_pwm;
}
/*********************************************************************************************************
*	�� �� ��: Drive_B_Ankle
*	����˵��: ����B group ANkle�ؽ�
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void Drive_B_Ankle (void)
{
			 get_ankle_angle(&leg2, &P2_body);
			 get_ankle_angle(&leg4, &P4_body);
			 get_ankle_angle(&leg6, &P6_body);
	
			 angle_to_pwm(&leg2);
			 angle_to_pwm(&leg4);
			 angle_to_pwm(&leg6);
	
			 ANKLE_2_OUT = leg2.ankle_pwm;
			 ANKLE_4_OUT = leg4.ankle_pwm;
			 ANKLE_6_OUT = leg6.ankle_pwm;
}
/*********************************************************************************************************
*	�� �� ��: Drive_Legx
*	����˵��: ����x����
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void Drive_Legx (LEG* leg)
{

				switch(leg->id )
				{
					case ID_LEG_1:
						   HIP_1_OUT = leg->hip_pwm;
					     KNEE_1_OUT = leg->knee_pwm;
					     ANKLE_1_OUT = leg->ankle_pwm;
						break;
					case ID_LEG_2:
						   HIP_2_OUT = leg->hip_pwm;
					     KNEE_2_OUT = leg->knee_pwm;
					     ANKLE_2_OUT = leg->ankle_pwm;
						break;
					case ID_LEG_3:
						   HIP_3_OUT =  leg->hip_pwm;
					     KNEE_3_OUT =  leg->knee_pwm;
					     ANKLE_3_OUT =  leg->ankle_pwm;
						break;
					case ID_LEG_4:
						   HIP_4_OUT =  leg->hip_pwm;
					     KNEE_4_OUT =  leg->knee_pwm;
					     ANKLE_4_OUT = leg->ankle_pwm;
						break;
					case ID_LEG_5:
						   HIP_5_OUT =  leg->hip_pwm;
					     KNEE_5_OUT =  leg->knee_pwm;
					     ANKLE_5_OUT =  leg->ankle_pwm;
						break;
					case ID_LEG_6:
						   HIP_6_OUT =  leg->hip_pwm;
					     KNEE_6_OUT =  leg->knee_pwm;
					     ANKLE_6_OUT =  leg->ankle_pwm;
						break;
				}
}


/*********************************************************************
*
*       Public Code
*
**********************************************************************
*/
/*********************************************************************************************************
*	�� �� ��: Lift_down_legx
*	����˵��: ̧�ȶ���
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Lift_down_legx(u8  leg_id)
{

	    LEG*    leg;
	    Position* p;
	
	    D1_Length = D1_STRATCH;
	
	    switch(leg_id)
			{
				case ID_LEG_1:
					leg = &leg1;
					set_position(&P1_body ,-STRATCH_X, STRATCH_Y, STRATCH_Z);
				  p = &P1_body;
					break;
				case ID_LEG_2:
					leg = &leg2;
					set_position(&P2_body ,-STRATCH_X, 0,         STRATCH_Z);
				  p = &P2_body;
					break;
				case ID_LEG_3:
					leg = &leg3;
					set_position(&P3_body ,-STRATCH_X, -STRATCH_Y, STRATCH_Z);
				  p = &P3_body;
					break;
				case ID_LEG_4:
					leg = &leg4;
					set_position(&P4_body , STRATCH_X, -STRATCH_Y, STRATCH_Z);
				  p = &P4_body;
					break;
				case ID_LEG_5:
					leg = &leg5;
					set_position(&P5_body , STRATCH_X, 0,         STRATCH_Z);
				  p = &P5_body;
					break;
				case ID_LEG_6:
					leg = &leg6;
					set_position(&P6_body , STRATCH_X, STRATCH_Y,  STRATCH_Z);
				  p = &P6_body;
					break;
			}

					get_hip_angle(leg, p);	
					get_knee_angle(leg,p);
					get_ankle_angle(leg,p);
					angle_to_pwm(leg);
					Drive_Legx(leg);

		 	   
}
/*********************************************************************************************************
*	�� �� ��: Sit_Down
*	����˵��: ����
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Sit_Down(int delay_time)
{
	    u8    i = 0;
	    OS_ERR  err;
	
	    D1_Length = D1_DEF; 
	
			set_position(&P1_body ,-Foot_1_Reset_X, Foot_1_Reset_Y,  Foot_1_Reset_Z);
			set_position(&P2_body, -Foot_2_Reset_X, Foot_2_Reset_Y,  Foot_2_Reset_Z);
			set_position(&P3_body, -Foot_1_Reset_X, -Foot_1_Reset_Y, Foot_1_Reset_Z);
			set_position(&P4_body, Foot_1_Reset_X,  -Foot_1_Reset_Y, Foot_1_Reset_Z);
			set_position(&P5_body, Foot_2_Reset_X,  Foot_2_Reset_Y,  Foot_2_Reset_Z);
			set_position(&P6_body, Foot_1_Reset_X,  Foot_1_Reset_Y,  Foot_1_Reset_Z);
	    for( ;i < ID_MAX; i++)
			{
					get_hip_angle(leg[i], position_body[i]);	
					get_knee_angle(leg[i],position_body[i]);
					get_ankle_angle(leg[i],position_body[i]);
					angle_to_pwm(leg[i]);
					Drive_Legx(leg[i]);
			}
		
			OSTimeDlyHMSM(0,0,delay_time,0,OS_OPT_TIME_DLY, &err);

}
/*********************************************************************************************************
*	�� �� ��: Stand_Up
*	����˵��: վ��
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Stand_Up(void)
{

		u8    i = 0;
	
	  D1_Length = D1_DEF;//mm
	  D2_Length = 170;//mm
	 
	  set_position(&P1_body ,Foot_1_Stand_X, Foot_1_Stand_Y, Foot_1_Stand_Z);
		set_position(&P2_body, Foot_2_Stand_X, Foot_2_Stand_Y, Foot_2_Stand_Z);
	  set_position(&P3_body, Foot_3_Stand_X, Foot_3_Stand_Y, Foot_3_Stand_Z);
		set_position(&P4_body, Foot_4_Stand_X, Foot_4_Stand_Y, Foot_4_Stand_Z);
		set_position(&P5_body, Foot_5_Stand_X, Foot_5_Stand_Y, Foot_5_Stand_Z);
		set_position(&P6_body, Foot_6_Stand_X, Foot_6_Stand_Y, Foot_6_Stand_Z);
		
			 
	    for( ;i < ID_MAX; i++)
			{
					get_hip_angle(leg[i], position_body[i]);	
					get_knee_angle(leg[i],position_body[i]);
					get_ankle_angle(leg[i],position_body[i]);
					angle_to_pwm(leg[i]);
					Drive_Legx(leg[i]);
			}
			
}

/*********************************************************************************************************
*	�� �� ��: Go_straight
*	����˵��: ��������2����ǰƽ�ƣ�Ȼ��1�����£�2������1����ǰƽ�ƣ�Ȼ��2�����£��ܶ���ʼ
*           ������ڲ�ͬ��Χ�ڣ���̬Ҳ��ͬ
*	��    �Σ�yaw:�����(rad)      duty:ռ��ϵ��     stride������    steps:����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Go_Straight(float yaw, float duty, float stride, int steps)
{
	   OS_ERR               err;
     float   s0 = stride / 4;
	   float   s1 = stride / 2;
     u8       i ;
	
	   D1_Length = D1_DEF;
	   
	   for(i = 0; i < steps; i++)
		 {
			 /*����ǰ��s/4*/
			 translate_center(yaw, s0, position_body);
			 Drive_Hip();
			 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
			 /*����A����*/
			 Trans_Aleg(yaw);
			 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
			 /*����A����*/
			 Putdown_Aleg();
			 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
		
			 /*����ǰ��s/4*/
			 translate_center(yaw, s0, position_body);
			 Drive_Hip();
			 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
		
			 /*����B����*/
			 Trans_Bleg(yaw);
			 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
			 /*����B����*/
			 Putdown_Bleg();
			 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
		
//			 /*����ǰ��s/4*/
//			 translate_center(yaw, s0, position_body);
//			 Drive_Hip();
//			 OSTimeDlyHMSM(0,0,0, 100,OS_OPT_TIME_DLY, &err);
			 
			 /*����ǰ��3s/4*/
			 translate_center(yaw, s1, position_body);
			 Drive_Hip();
			 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
	   }
	
}

/*********************************************************************************************************
*	�� �� ��: Stamp
*	����˵��: ��Ŷ���
*	��    �Σ�counts:��ŵĴ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Stamp(int counts)
{
	   u8   i;
	   OS_ERR  err;
	
	   D1_Length = D1_DEF;
	   
	   for( i = 0; i < counts; i ++)
	  {
		      legs_up_down(A_position_body, HEIGHT_TRANSFOOT);//A Group  lift
          Drive_A_Knee();
			    OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
					legs_up_down(A_position_body, HEIGHT_BODY);//A Group  left
          Drive_A_Knee();
			    OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
			
			    legs_up_down(B_position_body, HEIGHT_TRANSFOOT);//B Group  lift
          Drive_B_Knee();
			    OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
					legs_up_down(B_position_body, HEIGHT_BODY);//B Group  left
          Drive_B_Knee();
			    OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
			    
	  }

}
/*********************************************************************************************************
*	�� �� ��: Turn_Around
*	����˵��: ԭ��ת����
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Turn_Around(int direction, float angle, unsigned int counts )
{
      unsigned int   c;
	    OS_ERR       err;
	    float    a0 = angle / 4;
	    float    a1 = angle / 2;
	    float    a_def;
	
	    D1_Length  = D1_DEF;
	    
	    if(direction == DIRECTION_C)
				  a_def = -angle;
			else
				  a_def = angle;
			
	    for(c = 0; c < counts; c ++)
			{
				switch (direction)
				{
					//˳ʱ��--����ת
					case DIRECTION_C:
						   
						 /*ÿ����ת�����������ϵ�µ�λ����������Ϊ��ʼվ��״̬�µ�λ��*/
					     set_position(&P1_world ,Foot_1_Stand_X, Foot_1_Stand_Y, Foot_1_Stand_Z);
							 set_position(&P2_world, Foot_2_Stand_X, Foot_2_Stand_Y, Foot_2_Stand_Z);
							 set_position(&P3_world, Foot_3_Stand_X, Foot_3_Stand_Y, Foot_3_Stand_Z);
							 set_position(&P4_world, Foot_4_Stand_X, Foot_4_Stand_Y, Foot_4_Stand_Z);
							 set_position(&P5_world, Foot_5_Stand_X, Foot_5_Stand_Y, Foot_5_Stand_Z);
							 set_position(&P6_world, Foot_6_Stand_X, Foot_6_Stand_Y, Foot_6_Stand_Z);
						   /*��ת���� a/4*/
						   update_body_position(a0, position_body, position_world);
					     Drive_Hip();
					     OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
					     /*A����ת��a��*/
               Turn_Aleg(a0, a_def);
							 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
							 Putdown_Aleg();
							 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
							 /*����ת����a/2*/	
							 update_body_position(a1, position_body, position_world);
					     Drive_Hip();
					     OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
					     /*B����ת��a��*/
					     Turn_Bleg(a1, a_def);
							 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
							 Putdown_Bleg();
							 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
							 
						break;
					//��ʱ��--����ת
					case DIRECTION_CC:
						   
						   /*ÿ����ת�����������ϵ�µ�λ����������Ϊ��ʼվ��״̬�µ�λ��*/
					     set_position(&P1_world ,Foot_1_Stand_X, Foot_1_Stand_Y, Foot_1_Stand_Z);
							 set_position(&P2_world, Foot_2_Stand_X, Foot_2_Stand_Y, Foot_2_Stand_Z);
							 set_position(&P3_world, Foot_3_Stand_X, Foot_3_Stand_Y, Foot_3_Stand_Z);
							 set_position(&P4_world, Foot_4_Stand_X, Foot_4_Stand_Y, Foot_4_Stand_Z);
							 set_position(&P5_world, Foot_5_Stand_X, Foot_5_Stand_Y, Foot_5_Stand_Z);
							 set_position(&P6_world, Foot_6_Stand_X, Foot_6_Stand_Y, Foot_6_Stand_Z);
						   /*��ת���� a/4*/
						   update_body_position(a0, position_body, position_world);
					     Drive_Hip();
					     OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
					     /*B����ת��a��*/
               Turn_Bleg(a0, a_def);
							 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
								
							 Putdown_Bleg();
							 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
							 /*����ת����a/2*/	
							 update_body_position(a1, position_body, position_world);
					     Drive_Hip();
					     OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
					     /*A����ת��a��*/
					     Turn_Aleg(a1, a_def);
							 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
								
							 Putdown_Aleg();
							 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
								
						break;
				}
					  	 /*ÿ����ת�����󶼽���������ϵ�µ�λ����������Ϊ��ʼվ��״̬�µ�λ��
				         ��ʹ���巽����ת������ʼ״̬һ��*/
					     set_position(&P1_body ,Foot_1_Stand_X, Foot_1_Stand_Y, Foot_1_Stand_Z);
							 set_position(&P2_body, Foot_2_Stand_X, Foot_2_Stand_Y, Foot_2_Stand_Z);
							 set_position(&P3_body, Foot_3_Stand_X, Foot_3_Stand_Y, Foot_3_Stand_Z);
							 set_position(&P4_body, Foot_4_Stand_X, Foot_4_Stand_Y, Foot_4_Stand_Z);
							 set_position(&P5_body, Foot_5_Stand_X, Foot_5_Stand_Y, Foot_5_Stand_Z);
							 set_position(&P6_body, Foot_6_Stand_X, Foot_6_Stand_Y, Foot_6_Stand_Z);
							 Drive_Hip();
					     OSTimeDlyHMSM(0,0,0, 250,OS_OPT_TIME_DLY, &err);
				
			}
		
}

/*********************************************************************************************************
*	�� �� ��: Shake_Head
*	����˵��: ҡͷ����
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Shake_Head(int direction)
{
//      unsigned int   c;
	    OS_ERR       err;
      
	    switch(direction)
			{
				//��ʱ��--ͷ����
				case DIRECTION_CC:
					   HEAD_OUT += HEAD_STEP; 
					break;
				//˳ʱ��--ͷ����
				case DIRECTION_C:
					   HEAD_OUT -= HEAD_STEP; 
					break;
			}

//			if(HEAD_OUT < HEAD_LEFT_MARGIN )
//			{
//				 HEAD_OUT = HEAD_LEFT_MARGIN;
//			}
//			if( HEAD_OUT > HEAD_RIGHT_MARGIN)
//			{
//				 HEAD_OUT = HEAD_RIGHT_MARGIN;
//			}
				
    	OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
		
}



/***************************** �����޿Ƽ� www.apollorobot.cn (END OF FILE) *********************************/
