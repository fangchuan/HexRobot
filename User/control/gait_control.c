/*
*********************************************************************************************************
*
*	模块名称 : 机器人控制模块
*	文件名称 : control.c
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
//六足
static LEG    leg1 = {1};//
static LEG    leg2 = {2};//
static LEG    leg3 = {3};//
static LEG    leg4 = {4};//
static LEG    leg5 = {5};//
static LEG    leg6 = {6};//
//6个立足点在机体坐标系中的位置
//初始值都是复位状态下的值
static Position  P1_body = {-Foot_1_Reset_X, Foot_1_Reset_Y, Foot_1_Reset_Z};
static Position  P2_body = {-Foot_2_Reset_X, Foot_2_Reset_Y, Foot_2_Reset_Z};
static Position  P3_body = {-Foot_1_Reset_X, -Foot_1_Reset_Y,Foot_1_Reset_Z};
static Position  P4_body = {Foot_1_Reset_X,  -Foot_1_Reset_Y,Foot_1_Reset_Z};
static Position  P5_body = {Foot_2_Reset_X,  Foot_2_Reset_Y, Foot_2_Reset_Z};
static Position  P6_body = {Foot_1_Reset_X,  Foot_1_Reset_Y, Foot_1_Reset_Z};
//六个立足点的机体坐标系下的位置指针数组
static Position* position_body[6] = {&P1_body, &P2_body, &P3_body, &P4_body, &P5_body, &P6_body};
static Position* A_position_body[3] = {&P1_body, &P3_body, &P5_body};//A组腿的立足点
static Position* B_position_body[3] = {&P2_body, &P4_body, &P6_body};//B组腿的立足点
//6个立足点在大地坐标系中的位置
//初始值都是站立状态下的值
static Position  P1_world = {Foot_1_Stand_X, Foot_1_Stand_Y, Foot_1_Stand_Z};
static Position  P2_world = {Foot_2_Stand_X, Foot_2_Stand_Y, Foot_2_Stand_Z};
static Position  P3_world = {Foot_3_Stand_X, Foot_3_Stand_Y, Foot_3_Stand_Z};
static Position  P4_world = {Foot_4_Stand_X, Foot_4_Stand_Y, Foot_4_Stand_Z};
static Position  P5_world = {Foot_5_Stand_X, Foot_5_Stand_Y, Foot_5_Stand_Z};
static Position  P6_world = {Foot_6_Stand_X, Foot_6_Stand_Y, Foot_6_Stand_Z}; 
//六个立足点的大地坐标系下的位置指针数组
static Position* position_world[6] = {&P1_world, &P2_world, &P3_world, &P4_world, &P5_world, &P6_world};
static Position* A_position_world[3] = {&P1_world, &P3_world, &P5_world};//A组腿的立足点
static Position* B_position_world[3] = {&P2_world, &P4_world, &P6_world};//B组腿的立足点

//机身站立状态下固定参数的立足点情况
static  Position  P1_const = {Foot_1_Stand_X, Foot_1_Stand_Y, Foot_1_Stand_Z};
static  Position  P2_const = {Foot_2_Stand_X, Foot_2_Stand_Y, Foot_2_Stand_Z};
static  Position  P3_const = {Foot_3_Stand_X, Foot_3_Stand_Y, Foot_3_Stand_Z};
static  Position  P4_const = {Foot_4_Stand_X, Foot_4_Stand_Y, Foot_4_Stand_Z};
static  Position  P5_const = {Foot_5_Stand_X, Foot_5_Stand_Y, Foot_5_Stand_Z};
static  Position  P6_const = {Foot_6_Stand_X, Foot_6_Stand_Y, Foot_6_Stand_Z}; 
static  Position* position_const[6] = {&P1_const, &P2_const, &P3_const, &P4_const, &P5_const, &P6_const};
static  Position* A_position_const[3] = {&P1_const, &P3_const, &P5_const};//A组腿的立足点
static  Position* B_position_const[3] = {&P2_const, &P4_const, &P6_const};//B组腿的立足点
//static Position  P0_world ;//机体坐标系的重心

static LEG*      leg[6] ={&leg1, &leg2, &leg3, &leg4, &leg5, &leg6};//六条腿的指针数组
static LEG*      A_leg[3] = {&leg1, &leg3, &leg5};                  //A组腿的指针数组
static LEG*      B_leg[3] = {&leg2, &leg4, &leg6};                  //B组腿的指针数组
/*********************************************************************
*
*       Global Data
*
**********************************************************************
*/

unsigned short  D1_Length  =   130;      //臀关节到立足点距离,初始值为复位状态的值
unsigned short  D2_Length  =   100;      //L2\L3组成的三角形的第三条边长,初始值为复位状态的值

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
*	函 数 名: legs_up_down
*	功能说明: A/B组腿举起或者落下
*	形    参：height:腿举起来时的Z轴坐标
*	返 回 值: 
*********************************************************************************************************
*/					
static void legs_up_down(Position** p, float height)						
{
	
		  p[0]->z = height;
	    p[1]->z = height;
	    p[2]->z = height;
	    
}
/*********************************************************************************************************
*	函 数 名: Trans_Aleg
*	功能说明: 摆动A组腿,A组腿立足点迈出S，机体重心平移至S/2处
*	形    参：
*	返 回 值: 无
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
*	函 数 名: Turn_Aleg
*	功能说明: 摆动A组腿,绕Z轴转默认度数的角
*	形    参：a0:机身当前绕Z轴的转角    def_angle:一次转弯动作的默认转角
*	返 回 值: 无
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
*	函 数 名: Putdown_Aleg
*	功能说明: 放下A组腿，使A组腿着地
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
static  void Putdown_Aleg(void)
{
	     u8     i;
	
			//完成摆动后放下
			legs_up_down(A_position_body, Foot_1_Stand_Z);

			for( i = 0;i < 3; i++)
			{
					get_knee_angle(A_leg[i],A_position_body[i]);
					angle_to_pwm(A_leg[i]);
					Drive_Legx(A_leg[i]);
			}
		
}

/*********************************************************************************************************
*	函 数 名: Trans_Bleg
*	功能说明: 摆动B组腿,B组腿立足点迈出S，机体重心平移至S处
*	形    参：
*	返 回 值: 无
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
*	函 数 名: Turn_Bleg
*	功能说明: 摆动B组腿,绕Z轴转默认度数的角, B组腿的立足点已经被固定好，无须再根据当前机体坐标系下的立足点位置再做正向旋转变换
*	形    参：a0:机身当前绕Z轴的转角       def_angle:一次转弯动作的默认转角
*	返 回 值: 无
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
*	函 数 名: Putdown_Bleg
*	功能说明: 放下B组腿，使B组腿着地
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
static  void Putdown_Bleg(void)
{
	     u8     i;
	
			//完成摆动后放下
	    legs_up_down(B_position_body, Foot_1_Stand_Z);

			for( i = 0;i < 3; i++)
			{
					get_knee_angle(B_leg[i],B_position_body[i]);
					angle_to_pwm(B_leg[i]);
					Drive_Legx(B_leg[i]);
			}
		
}
/*********************************************************************************************************
*	函 数 名: Drive_Hip
*	功能说明: 驱动Hip关节
*	形    参：
*	返 回 值: 无
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
*	函 数 名: Drive_A_Hip
*	功能说明: 驱动A组的Hip关节
*	形    参：
*	返 回 值: 无
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
*	函 数 名: Drive_B_Hip
*	功能说明: 驱动B Group Hip关节
*	形    参：
*	返 回 值: 无
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
*	函 数 名: Drive_Knee
*	功能说明: 驱动Knee关节
*	形    参：
*	返 回 值: 无
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
*	函 数 名: Drive_A_Knee
*	功能说明: 驱动A Group Knee关节
*	形    参：
*	返 回 值: 无
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
*	函 数 名: Drive_B_Knee
*	功能说明: 驱动B Group Knee关节
*	形    参：
*	返 回 值: 无
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
*	函 数 名: Drive_Ankle
*	功能说明: 驱动ANkle关节
*	形    参：
*	返 回 值: 无
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
*	函 数 名: Drive_A_Ankle
*	功能说明: 驱动A group ankle关节
*	形    参：
*	返 回 值: 无
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
*	函 数 名: Drive_B_Ankle
*	功能说明: 驱动B group ANkle关节
*	形    参：
*	返 回 值: 无
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
*	函 数 名: Drive_Legx
*	功能说明: 驱动x号腿
*	形    参：
*	返 回 值: 无
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
*	函 数 名: Lift_down_legx
*	功能说明: 抬腿动作
*	形    参：
*	返 回 值: 无
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
*	函 数 名: Sit_Down
*	功能说明: 坐下
*	形    参：
*	返 回 值: 无
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
*	函 数 名: Stand_Up
*	功能说明: 站立
*	形    参：
*	返 回 值: 无
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
*	函 数 名: Go_straight
*	功能说明: 组腿提起，2组向前平移，然后1组落下；2组提起，1组向前平移，然后2组落下；周而复始
*           方向角在不同范围内，步态也不同
*	形    参：yaw:方向角(rad)      duty:占地系数     stride：步幅    steps:步数
*	返 回 值: 无
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
			 /*重心前移s/4*/
			 translate_center(yaw, s0, position_body);
			 Drive_Hip();
			 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
			 /*驱动A组腿*/
			 Trans_Aleg(yaw);
			 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
			 /*放下A组退*/
			 Putdown_Aleg();
			 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
		
			 /*重心前移s/4*/
			 translate_center(yaw, s0, position_body);
			 Drive_Hip();
			 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
		
			 /*驱动B组腿*/
			 Trans_Bleg(yaw);
			 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
			 /*放下B族退*/
			 Putdown_Bleg();
			 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
		
//			 /*重心前移s/4*/
//			 translate_center(yaw, s0, position_body);
//			 Drive_Hip();
//			 OSTimeDlyHMSM(0,0,0, 100,OS_OPT_TIME_DLY, &err);
			 
			 /*重心前移3s/4*/
			 translate_center(yaw, s1, position_body);
			 Drive_Hip();
			 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
	   }
	
}

/*********************************************************************************************************
*	函 数 名: Stamp
*	功能说明: 跺脚动作
*	形    参：counts:跺脚的次数
*	返 回 值: 无
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
*	函 数 名: Turn_Around
*	功能说明: 原地转向动作
*	形    参：
*	返 回 值: 无
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
					//顺时针--向右转
					case DIRECTION_C:
						   
						 /*每次旋转都将大地坐标系下的位置向量设置为初始站立状态下的位置*/
					     set_position(&P1_world ,Foot_1_Stand_X, Foot_1_Stand_Y, Foot_1_Stand_Z);
							 set_position(&P2_world, Foot_2_Stand_X, Foot_2_Stand_Y, Foot_2_Stand_Z);
							 set_position(&P3_world, Foot_3_Stand_X, Foot_3_Stand_Y, Foot_3_Stand_Z);
							 set_position(&P4_world, Foot_4_Stand_X, Foot_4_Stand_Y, Foot_4_Stand_Z);
							 set_position(&P5_world, Foot_5_Stand_X, Foot_5_Stand_Y, Foot_5_Stand_Z);
							 set_position(&P6_world, Foot_6_Stand_X, Foot_6_Stand_Y, Foot_6_Stand_Z);
						   /*旋转机体 a/4*/
						   update_body_position(a0, position_body, position_world);
					     Drive_Hip();
					     OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
					     /*A组腿转动a度*/
               Turn_Aleg(a0, a_def);
							 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
							 Putdown_Aleg();
							 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
							 /*机体转动到a/2*/	
							 update_body_position(a1, position_body, position_world);
					     Drive_Hip();
					     OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
					     /*B组腿转动a度*/
					     Turn_Bleg(a1, a_def);
							 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
							 Putdown_Bleg();
							 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
							 
						break;
					//逆时针--向左转
					case DIRECTION_CC:
						   
						   /*每次旋转都将大地坐标系下的位置向量设置为初始站立状态下的位置*/
					     set_position(&P1_world ,Foot_1_Stand_X, Foot_1_Stand_Y, Foot_1_Stand_Z);
							 set_position(&P2_world, Foot_2_Stand_X, Foot_2_Stand_Y, Foot_2_Stand_Z);
							 set_position(&P3_world, Foot_3_Stand_X, Foot_3_Stand_Y, Foot_3_Stand_Z);
							 set_position(&P4_world, Foot_4_Stand_X, Foot_4_Stand_Y, Foot_4_Stand_Z);
							 set_position(&P5_world, Foot_5_Stand_X, Foot_5_Stand_Y, Foot_5_Stand_Z);
							 set_position(&P6_world, Foot_6_Stand_X, Foot_6_Stand_Y, Foot_6_Stand_Z);
						   /*旋转机体 a/4*/
						   update_body_position(a0, position_body, position_world);
					     Drive_Hip();
					     OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
					     /*B组腿转动a度*/
               Turn_Bleg(a0, a_def);
							 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
								
							 Putdown_Bleg();
							 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
							 /*机体转动到a/2*/	
							 update_body_position(a1, position_body, position_world);
					     Drive_Hip();
					     OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
					     /*A组腿转动a度*/
					     Turn_Aleg(a1, a_def);
							 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
								
							 Putdown_Aleg();
							 OSTimeDlyHMSM(0,0,0, 200,OS_OPT_TIME_DLY, &err);
								
						break;
				}
					  	 /*每次旋转结束后都将机体坐标系下的位置向量设置为初始站立状态下的位置
				         来使机体方向旋转到跟初始状态一样*/
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
*	函 数 名: Shake_Head
*	功能说明: 摇头动作
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
void Shake_Head(int direction)
{
//      unsigned int   c;
	    OS_ERR       err;
      
	    switch(direction)
			{
				//逆时针--头向左
				case DIRECTION_CC:
					   HEAD_OUT += HEAD_STEP; 
					break;
				//顺时针--头向右
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



/***************************** 阿波罗科技 www.apollorobot.cn (END OF FILE) *********************************/
