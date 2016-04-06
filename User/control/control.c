/*
*********************************************************************************************************
*
*	模块名称 : 舵机控制模块
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
#include "control.h"
#include "common.h"
#include "posture.h"
/*********************************************************************
*
*       MACRO Define
*
**********************************************************************
*/
#define  L1_Length      25      //mm   第一关节连杆的长度
#define  L1L1           625     //L1的平方
#define  L2_Length      90      //mm   第二关节连杆的长度
#define  L2L2          8100     //L2的平方
#define  L3_Length     120      //mm   第三关节连杆的长度
#define  L3L3          14400    //L3的平方
#define  L0            80       //mm   机体中心到第一关节的距离
#define  L0L0          6400     //L0的平方
#define  L0X2          160      //2*L0
#define  L2X2          180      //2*L2
#define  L2L3X2        21600    //2*L2*L3

#define  COSTH1        -0.5     //cos120  Leg1第一关节到机体中心的夹角
#define  SINTH1        0.866    //sin120
#define  COSTH2        -1       //cos180  Leg2第一关节到机体中心的夹角
#define  SINTH2         0       //sin180
#define  COSTH3        -0.5     //cos240  Leg3第一关节到机体中心的夹角
#define  SINTH3        -0.866   //sin240
#define  COSTH4        0.5      //cos300  Leg4第一关节到机体中心的夹角
#define  SINTH4        -0.866   //sin300
#define  COSTH5        1        //cos0    Leg5第一关节到机体中心的夹角
#define  SINTH5        0        //sin0
#define  COSTH6        0.5      //cos60   Leg6第一关节到机体中心的夹角
#define  SINTH6        0.866    //sin60

/*********************************************************************
*
*       Global Data
*
**********************************************************************
*/
Servo    leg1 = {1};//
Servo    leg2 = {2};//
Servo    leg3 = {3};//
Servo    leg4 = {4};//
Servo    leg5 = {5};//
Servo    leg6 = {6};//

//6个立足点在机体坐标系中的位置
//初始值都是复位状态下的值
Position  P1_body = {-Foot_1_Reset_X, Foot_1_Reset_Y, 0};
Position  P2_body = {-Foot_2_Reset_X, 0 ,  0};
Position  P3_body = {-Foot_1_Reset_X, -Foot_1_Reset_Y,0};
Position  P4_body = {Foot_1_Reset_X,  -Foot_1_Reset_Y,0};
Position  P5_body = {Foot_2_Reset_X,  0,   0};
Position  P6_body = {Foot_1_Reset_X,  Foot_1_Reset_Y, 0};

u16  D1_Length  =   130;      //臀关节到立足点距离,初始值为复位状态的值
u16  D2_Length  =   100;      //L2\L3组成的三角形的第三条边长,初始值为复位状态的值
/*********************************************************************
*
*       Static Data
*
**********************************************************************
*/
//default angle of all the engine 
static int pwm_init[26] = { 0,	   //no use	 
	 			     590,  535,    0,  475,  605,		   //( 1 - 5 )
				     790,  355,  530,  605,    0,	   	   //( 6 - 10)
				       0,  475,    0,    0,  793,	       //( 11- 15)
				     575,    0,  535,  535,  793,	       //( 16- 20)
				     375,  575,  575,  385,    0,	       //( 21- 25)
				    }; 

/*********************************************************************
*
*       Static Code
*
**********************************************************************
*/
						/**************************实现函数********************************************
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
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/					
static void get_hip_angle(Servo* leg, Position* p)						
{
	     float cth0,sth0,cth1,sth1;
	     float temp,temp0,temp1,temp2;
	     float temp02,temp12,temp2s;
	     float d0;
	     
	     switch(leg->id)
			 {
				 case ID_LEG_1:
					              cth0 = COSTH1;
				                sth0 = SINTH1;
					 break;
				 case ID_LEG_2:
					              cth0 = COSTH2;
				                sth0 = SINTH2;
					 break;
				 case ID_LEG_3:
					              cth0 = COSTH3;
				                sth0 = SINTH3;
					 break;
				 case ID_LEG_4:
					              cth0 = COSTH4;
				                sth0 = SINTH4;
					 break;
				 case ID_LEG_5:
					              cth0 = COSTH5;
				                sth0 = SINTH5;
					 break;
				 case ID_LEG_6:
					              cth0 = COSTH6;
				                sth0 = SINTH6;
					 break;
			 }
			 
			 temp0 = p->x - L0*cth0; temp02 = temp0*temp0;
			 temp1 = p->y - L0*sth0; temp12 = temp1*temp1;
			 temp2 = temp02 + temp12; temp2s = invSqrt(temp2);
			 d0 = p->x * p->x + p->y * p->y;
			 
			 cth1 = (d0  - temp2 - L0L0) * temp2s / L0X2;
			 sth1 = sqrt(1-cth1*cth1);
			 
			 temp = atan2(sth1,cth1);
			 leg->hip_angle = temp * RAD_TO_DEGREE;
}
/*********************************************************************************************************
*	函 数 名: get_Knee_angle
*	功能说明: 经过逆运动学计算出膝关节的角度
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/					
static void get_knee_angle(Servo* leg, Position* p)						
{
	     float d0,d02;
	     float temp,temp0, temp1;
	     float cth2,sth2;
	     float  height = fabs(p->z);
	
	     d0 = D1_Length - L1_Length;
	     d02 = d0 * d0;
	     temp0 = height * height;
	     temp1 = invSqrt(temp0 + d02);
	     
	     cth2 = (L2L2 + d02 + temp0 - L3L3 )* temp1 /L2X2;
	     sth2 = sqrt(1- cth2*cth2);
	
	     temp = atan2(sth2, cth2) - atan2(height, d0);
	     leg->knee_angle = temp * RAD_TO_DEGREE;
}
/*********************************************************************************************************
*	函 数 名: get_Ankle_angle
*	功能说明: 经过逆运动学计算出踝关节的角度
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/					
static void get_ankle_angle(Servo* leg, Position* p)						
{
	     float cth3,sth3;
	     float temp,temp0,temp1,temp12,temp2;
	     float height = fabs(p->z);
	
	     temp0 = height * height;
	     temp1 = D1_Length - L1_Length;
	     temp12 = temp1 * temp1;
	     temp2 = temp0 + temp12;
	     
	     cth3 = (temp2 - L2L2 - L3L3)/L2L3X2;
	     sth3 = sqrt(1-cth3*cth3);
	    
	     temp = atan2(sth3, cth3);
	     leg->ankle_angle = temp * RAD_TO_DEGREE;
}
/*********************************************************************************************************
*	函 数 名: set_position_inbody
*	功能说明: 在机体坐标系中设置期望位置
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/					
static void set_position_inbody(Position*p, int x_e, int y_e, int z_e)						
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
static void angle_to_pwm(Servo*leg)						
{
			int   pwm_hip,pwm_knee,pwm_ankle ;
	    float acture_hip,acture_knee,acture_ankle;
	      
	    switch(leg->id)
			{
				case ID_LEG_1:
					   acture_hip = leg->hip_angle - HIP_MID_ANGLE;
				     acture_knee = leg->knee_angle - KNEE_MID_ANGLE;
				     acture_ankle = leg->ankle_angle - ANKLE_MID_ANGLE;
					break;
				case ID_LEG_2:
					   acture_hip = leg->hip_angle - HIP_MID_ANGLE_LEG25;
				     acture_knee = leg->knee_angle - KNEE_MID_ANGLE;
				     acture_ankle = leg->ankle_angle - ANKLE_MID_ANGLE;
					break;
				case ID_LEG_3:
						 acture_hip = leg->hip_angle - HIP_MID_ANGLE;
						 acture_knee = leg->knee_angle - KNEE_MID_ANGLE;
						 acture_ankle = leg->ankle_angle - ANKLE_MID_ANGLE;
					break;
				case ID_LEG_4:
						 acture_hip = leg->hip_angle - HIP_MID_ANGLE;
						 acture_knee = leg->knee_angle - KNEE_MID_ANGLE;
						 acture_ankle = leg->ankle_angle - ANKLE_MID_ANGLE;

					break;
				case ID_LEG_5:
						 acture_hip = leg->hip_angle - HIP_MID_ANGLE_LEG25;
				     acture_knee = leg->knee_angle - KNEE_MID_ANGLE;
				     acture_ankle = leg->ankle_angle - ANKLE_MID_ANGLE;
					break;
				case ID_LEG_6:
					   acture_hip = leg->hip_angle - HIP_MID_ANGLE;
						 acture_knee = leg->knee_angle - KNEE_MID_ANGLE;
						 acture_ankle = leg->ankle_angle - ANKLE_MID_ANGLE;
					break;
			}
				pwm_hip = ANGLE_TO_PWM(acture_hip);
				leg->hip_pwm = pwm_hip;

				pwm_knee = ANGLE_TO_PWM(acture_knee);
				leg->knee_pwm = pwm_knee;

				pwm_ankle = ANGLE_TO_PWM(acture_ankle);
				leg->ankle_pwm = pwm_ankle;
}
/*********************************************************************************************************
*	函 数 名: pwm_to_angle
*	功能说明: 由当前PWM值推算出出相关关节的角度
*	形    参：
*	返 回 值: 关节角度
*********************************************************************************************************
*/					
//static float pwm_to_angle(Servo*joint)						
//{
//			float   angle = 0;

//			angle = PWM_TO_ANGLE(joint->pwm_out);
//			return angle;
//}
/*********************************************************************************************************
*	函 数 名: Drive_Hip
*	功能说明: 驱动Hip关节
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
static  void Drive_Hip(Servo* hip)
{
	      if(hip->id  < ID_MIN || hip->id  > ID_MAX)
					return ;
				
				switch(hip->id)
				{
					case ID_LEG_1:
						   HIP_1_OUT = hip->hip_pwm;
						break;
					case ID_LEG_2:
						   HIP_2_OUT = hip->hip_pwm;
						break;
					case ID_LEG_3:
						   HIP_3_OUT = hip->hip_pwm;
						break;
					case ID_LEG_4:
						   HIP_4_OUT = hip->hip_pwm;
						break;
					case ID_LEG_5:
						   HIP_5_OUT = hip->hip_pwm;
						break;
					case ID_LEG_6:
						   HIP_6_OUT = hip->hip_pwm;
						break;
				}
}
/*********************************************************************************************************
*	函 数 名: Drive_Knee
*	功能说明: 驱动Knee关节
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
static  void Drive_Knee (Servo* knee)
{
	      if(knee->id  < ID_MIN || knee->id  > ID_MAX)
					return ;
				switch(knee->id )
				{
					case ID_LEG_1:
						   KNEE_1_OUT = knee->knee_pwm;
						break;
					case ID_LEG_2:
						   KNEE_2_OUT = knee->knee_pwm;
						break;
					case ID_LEG_3:
						   KNEE_3_OUT = knee->knee_pwm;
						break;
					case ID_LEG_4:
						   KNEE_4_OUT = knee->knee_pwm;
						break;
					case ID_LEG_5:
						   KNEE_5_OUT = knee->knee_pwm;
						break;
					case ID_LEG_6:
						   KNEE_6_OUT = knee->knee_pwm;
						break;
				}
}
/*********************************************************************************************************
*	函 数 名: Drive_Ankle
*	功能说明: 驱动ANkle关节
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
static  void Drive_Ankle (Servo* ankle)
{
	      if(ankle->id  < ID_MIN || ankle->id  > ID_MAX)
					return ;
				switch(ankle->id )
				{
					case ID_LEG_1:
						   ANKLE_1_OUT = ankle->ankle_pwm;
						break;
					case ID_LEG_2:
						   ANKLE_2_OUT = ankle->ankle_pwm;
						break;
					case ID_LEG_3:
						   ANKLE_3_OUT = ankle->ankle_pwm;
						break;
					case ID_LEG_4:
						   ANKLE_4_OUT = ankle->ankle_pwm;
						break;
					case ID_LEG_5:
						   ANKLE_5_OUT = ankle->ankle_pwm;
						break;
					case ID_LEG_6:
						   ANKLE_6_OUT = ankle->ankle_pwm;
						break;
				}
}

/*********************************************************************************************************
*	函 数 名: Drive_Legx
*	功能说明: 驱动x号腿
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
static  void Drive_Legx (Servo* leg)
{
	      if(leg->id  < ID_MIN || leg->id  > ID_MAX)
					return ;
				switch(leg->id )
				{
					case ID_LEG_1:
						   HIP_1_OUT = PWM_MID + leg->hip_pwm;
					     KNEE_1_OUT = PWM_MID + leg->knee_pwm;
					     ANKLE_1_OUT = PWM_MID + leg->ankle_pwm;
						break;
					case ID_LEG_2:
						   HIP_2_OUT = PWM_MID + leg->hip_pwm;
					     KNEE_2_OUT = PWM_MID + leg->knee_pwm;
					     ANKLE_2_OUT = PWM_MID + leg->ankle_pwm;
						break;
					case ID_LEG_3:
						   HIP_3_OUT = PWM_MID + leg->hip_pwm;
					     KNEE_3_OUT = PWM_MID + leg->knee_pwm;
					     ANKLE_3_OUT = PWM_MID + leg->ankle_pwm;
						break;
					case ID_LEG_4:
						   HIP_4_OUT = PWM_MID + leg->hip_pwm;
					     KNEE_4_OUT = PWM_MID - leg->knee_pwm;
					     ANKLE_4_OUT = PWM_MID - leg->ankle_pwm;
						break;
					case ID_LEG_5:
						   HIP_5_OUT = PWM_MID + leg->hip_pwm;
					     KNEE_5_OUT = PWM_MID - leg->knee_pwm;
					     ANKLE_5_OUT = PWM_MID - leg->ankle_pwm;
						break;
					case ID_LEG_6:
						   HIP_6_OUT = PWM_MID + leg->hip_pwm;
					     KNEE_6_OUT = PWM_MID - leg->knee_pwm;
					     ANKLE_6_OUT = PWM_MID - leg->ankle_pwm;
						break;
				}
}
/*********************************************************************************************************
*	函 数 名: Pace_135_Motor1
*	功能说明: 摆动A组腿的Hip关节
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
static void Pace_135_Motor1(int pwm_default,int amplitude,int delay_time)
{
	int pwm_tmp = 0;
	OS_ERR      err;
	for( ; fabs(pwm_tmp)<=fabs(amplitude); )
	{
		OSTimeDlyHMSM(0,0,0,delay_time,OS_OPT_TIME_DLY, &err); 
		HIP_1_OUT = pwm_init[1] - pwm_default - pwm_tmp;	 //1-1-1  	   
	  HIP_3_OUT = pwm_init[9] - pwm_default - pwm_tmp;	 //3-1-9  	   
		HIP_5_OUT = pwm_init[8] + pwm_default + pwm_tmp;	 //5-1-8 
		
		if(amplitude > 0)
			pwm_tmp += ACTION_SPEED;
		else 
			pwm_tmp -= ACTION_SPEED;
	}
}
/*********************************************************************************************************
*	函 数 名: Pace_135_Motor2
*	功能说明: 摆动A组腿的Knee关节
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
static void Pace_135_Motor2(int pwm_default,int amplitude,int delay_time)
{
	int pwm_tmp = 0;
	OS_ERR      err;
	for(;fabs(pwm_tmp)<=fabs(amplitude);)
	{
			OSTimeDlyHMSM(0,0,0,delay_time,OS_OPT_TIME_DLY, &err);				 
			KNEE_1_OUT = pwm_init[20] - pwm_default - pwm_tmp;	 //1-2-20 
			KNEE_3_OUT = pwm_init[15] - pwm_default - pwm_tmp;	 //3-2-15 
			KNEE_5_OUT = pwm_init[21] + pwm_default + pwm_tmp;	 //5-2-21
		
			if(amplitude > 0)
				pwm_tmp += ACTION_SPEED;
			else 
				pwm_tmp -= ACTION_SPEED;
	}
}
/*********************************************************************************************************
*	函 数 名: Pace_135_Motor3
*	功能说明: 摆动A组腿的Ankle关节
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
static void Pace_135_Motor3(int pwm_default,int amplitude,int delay_time)
{
	int pwm_tmp = 0;
	OS_ERR      err;
	for(;fabs(pwm_tmp)<=fabs(amplitude);)
	{
			OSTimeDlyHMSM(0,0,0,delay_time,OS_OPT_TIME_DLY, &err);
			ANKLE_1_OUT = pwm_init[19] + pwm_default + pwm_tmp;	 //1-3-19
			ANKLE_3_OUT = pwm_init[16] + pwm_default + pwm_tmp;	 //3-3-16  	   
			ANKLE_5_OUT = pwm_init[22] - pwm_default - pwm_tmp;	 //5-3-22 
		
			if(amplitude > 0)
				pwm_tmp += ACTION_SPEED;
			else 
				pwm_tmp -= ACTION_SPEED;
	}
}
/*********************************************************************************************************
*	函 数 名: Pace_135_Motor3_Different
*	功能说明: 摆动A组腿的Ankle关节
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
static void Pace_135_Motor3_Different(int pwm_default,int amplitude,int delay_time)
{
	int pwm_tmp = 0;
	OS_ERR      err;
	for(;fabs(pwm_tmp)<=fabs(amplitude);)
	{
			OSTimeDlyHMSM(0,0,0,delay_time,OS_OPT_TIME_DLY, &err);
			ANKLE_1_OUT = pwm_init[19] + pwm_default + pwm_tmp;	 //1-3-19
			ANKLE_3_OUT = pwm_init[16] + pwm_default + pwm_tmp;	 //3-3-16  	   
			ANKLE_5_OUT = pwm_init[22] + pwm_default + pwm_tmp;	 //5-3-22 
		
			if(amplitude > 0)
				pwm_tmp += ACTION_SPEED;
			else 
				pwm_tmp -= ACTION_SPEED;
	}
}
/*********************************************************************************************************
*	函 数 名: Pace_246_Motor3
*	功能说明: 摆动B组腿的Knee关节
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
static void Pace_246_Motor2(int pwm_default,int amplitude,int delay_time)
{
	int pwm_tmp = 0;
	OS_ERR      err;
	for(;fabs(pwm_tmp)<=fabs(amplitude);)
	{
			OSTimeDlyHMSM(0,0,0,delay_time,OS_OPT_TIME_DLY, &err);
		  KNEE_2_OUT = pwm_init[6] - pwm_default - pwm_tmp;	 //2-2-6 
			KNEE_4_OUT = pwm_init[7]  + pwm_default + pwm_tmp;	 //4-2-7
			KNEE_6_OUT = pwm_init[24] + pwm_default + pwm_tmp;	 //6-2-24  	 
			 	 
			if(amplitude > 0)
				pwm_tmp += ACTION_SPEED;
			else 
				pwm_tmp -= ACTION_SPEED;
	}
}
/*********************************************************************************************************
*	函 数 名: Pace_246_Motor3
*	功能说明: 摆动B组腿的ANKLE关节
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
static void Pace_246_Motor3(int pwm_default,int amplitude,int delay_time)
{	   
	int pwm_tmp = 0;
	OS_ERR      err;
	for(;fabs(pwm_tmp)<=fabs(amplitude);)
	{
			OSTimeDlyHMSM(0,0,0,delay_time,OS_OPT_TIME_DLY, &err);
			ANKLE_2_OUT = pwm_init[2]  + pwm_default + pwm_tmp;	 //2-3-2  
			ANKLE_4_OUT = pwm_init[18] - pwm_default - pwm_tmp;	 //4-3-18
			ANKLE_6_OUT = pwm_init[23] - pwm_default - pwm_tmp;	 //6-3-23  	   
		
			if(amplitude > 0)
				pwm_tmp += ACTION_SPEED;
			else 
				pwm_tmp -= ACTION_SPEED;
	}
}
/*********************************************************************************************************
*	函 数 名: Pace_246_Motor3_Different
*	功能说明: 摆动B组腿的ANKLE关节
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
static void Pace_246_Motor3_Different(int pwm_default,int amplitude,int delay_time)
{	   
	int pwm_tmp = 0;
	OS_ERR      err;
	for(;fabs(pwm_tmp)<=fabs(amplitude);)
	{

			OSTimeDlyHMSM(0,0,0,delay_time,OS_OPT_TIME_DLY, &err);
			ANKLE_2_OUT = pwm_init[2]  + pwm_default + pwm_tmp;	 //2-3-2  
			ANKLE_4_OUT = pwm_init[18] + pwm_default + pwm_tmp;	 //4-3-18
			ANKLE_6_OUT = pwm_init[23] + pwm_default + pwm_tmp;	 //6-3-23  	   
		
			if(amplitude > 0)
				pwm_tmp += ACTION_SPEED;
			else 
				pwm_tmp -= ACTION_SPEED;
	}
}

/*********************************************************************************************************
*	函 数 名: Pace_135_246_Motor3
*	功能说明: 摆动ANKLE关节
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
static void Pace_135_246_Motor3(int pwm_def1,int pwm_def2,int amplitude,int delay_time)
{	   
	int pwm_tmp = 0;
	OS_ERR      err;
	for(;fabs(pwm_tmp)<=fabs(amplitude);)
	{
		OSTimeDlyHMSM(0,0,0,delay_time,OS_OPT_TIME_DLY, &err);

		ANKLE_1_OUT = pwm_init[19] + pwm_def1 + pwm_tmp;	 //1-3-19
		ANKLE_3_OUT = pwm_init[16] + pwm_def1 + pwm_tmp;	 //3-3-16  	   
		ANKLE_5_OUT = pwm_init[22] + pwm_def1 + pwm_tmp;	 //5-3-22  	   

		ANKLE_2_OUT = pwm_init[2]  + pwm_def2 + pwm_tmp;	 //2-3-2  
		ANKLE_4_OUT = pwm_init[18] + pwm_def2 + pwm_tmp;	 //4-3-18
		ANKLE_6_OUT = pwm_init[23] + pwm_def2 + pwm_tmp;	 //6-3-23  
		
		if(amplitude > 0)
			pwm_tmp += ACTION_SPEED;
		else 
			pwm_tmp -= ACTION_SPEED;
	}
}

/*********************************************************************
*
*       Public Code
*
**********************************************************************
*/
/*********************************************************************************************************
*	函 数 名: Position_Reset
*	功能说明: 复位动作
*	形    参：复位动作的延时时长
*	返 回 值: 无
*********************************************************************************************************
*/
void Position_Reset(int delay_time)
{
			OS_ERR  err;
	
      get_hip_angle(&leg1, &P1_body);	
			get_knee_angle(&leg1,&P1_body);
			get_ankle_angle(&leg1,&P1_body);
			angle_to_pwm(&leg1);
			Drive_Legx(&leg1);
			
			get_hip_angle(&leg2, &P2_body);	
			get_knee_angle(&leg2,&P2_body);
			get_ankle_angle(&leg2,&P2_body);
			angle_to_pwm(&leg2);
			Drive_Legx(&leg2);
			
			get_hip_angle(&leg3, &P3_body);	
			get_knee_angle(&leg3,&P3_body);
			get_ankle_angle(&leg3,&P3_body);
			angle_to_pwm(&leg3);
			Drive_Legx(&leg3);
			
			get_hip_angle(&leg4, &P4_body);	
			get_knee_angle(&leg4,&P4_body);
			get_ankle_angle(&leg4,&P4_body);
			angle_to_pwm(&leg4);
			Drive_Legx(&leg4);
			
			get_hip_angle(&leg5, &P5_body);	
			get_knee_angle(&leg5,&P5_body);
			get_ankle_angle(&leg5,&P5_body);
			angle_to_pwm(&leg5);
			Drive_Legx(&leg5);
			
			get_hip_angle(&leg6, &P6_body);	
			get_knee_angle(&leg6,&P6_body);
			get_ankle_angle(&leg6,&P6_body);
			angle_to_pwm(&leg6);
			Drive_Legx(&leg6);
			
			OSTimeDlyHMSM(0,0,delay_time,0,OS_OPT_TIME_DLY, &err);
		 	   
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
		int pwm_tmp = 0;						//motor 2
		int pwm_tmp2 = 0;						//motor 1
	  OS_ERR       err;
		for ( ; pwm_tmp<=STAND_HIGHT; )				 //150 efect the height of stand
		{
				OSTimeDlyHMSM(0,0,0,delay_time,OS_OPT_TIME_DLY, &err);
				
				KNEE_1_OUT = pwm_init[20] - pwm_tmp;	 //1-2-20
				KNEE_2_OUT = pwm_init[6]  - pwm_tmp;	 //2-2-6	   	   
				KNEE_3_OUT = pwm_init[15] - pwm_tmp;	 //3-2-15	
				KNEE_4_OUT = pwm_init[7]  + pwm_tmp;	 //4-2-7	
			  KNEE_5_OUT = pwm_init[21] + pwm_tmp;	 //5-2-21
				KNEE_6_OUT = pwm_init[24] + pwm_tmp;	 //6-2-24 
				
				HIP_1_OUT = pwm_init[1] - pwm_tmp2;	   //1-1-1  	   
				HIP_2_OUT = pwm_init[5] - pwm_tmp2;	   //2-1-5  	 
				HIP_3_OUT = pwm_init[9] - pwm_tmp2;	   //3-1-9  
				HIP_4_OUT = pwm_init[12] + pwm_tmp2;	 //4-1-12  	   
				HIP_5_OUT = pwm_init[8] + pwm_tmp2;	   //5-1-8  	   
				HIP_6_OUT = pwm_init[4] + pwm_tmp2;	   //6-1-4  	   
						 
				pwm_tmp += ACTION_SPEED;	
				pwm_tmp2 += SHRINK_SPEED;	
			}
			OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_DLY, &err);
//			Close_All_TIM();		//close all timer
}
/*********************************************************************************************************
*	函 数 名: Stand_Up
*	功能说明: 站立
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
void Stand_Up(int delay_time)
{

	  OS_ERR       err;

	  D1_Length = 165;//mm
	  D2_Length = 170;//mm
	 
	  set_position_inbody(&P1_body ,-Foot_1_Stand_X, Foot_1_Stand_Y, -Foot_1_Stand_Z);
		set_position_inbody(&P2_body, -Foot_2_Stand_X, Foot_2_Stand_Y,   -Foot_2_Stand_Z);
	  set_position_inbody(&P3_body, -Foot_1_Stand_X, -Foot_1_Stand_Y,-Foot_1_Stand_Z);
		set_position_inbody(&P4_body, Foot_1_Stand_X, -Foot_1_Stand_Y, -Foot_1_Stand_Z);
		set_position_inbody(&P5_body, Foot_2_Stand_X,  Foot_2_Stand_Y,   -Foot_2_Stand_Z);
		set_position_inbody(&P6_body, Foot_1_Stand_X, Foot_1_Stand_Y,  -Foot_1_Stand_Z);
		
		
		  get_hip_angle(&leg1, &P1_body);	
			get_knee_angle(&leg1,&P1_body);
			get_ankle_angle(&leg1,&P1_body);
			angle_to_pwm(&leg1);
			Drive_Legx(&leg1);
			
			get_hip_angle(&leg2, &P2_body);	
			get_knee_angle(&leg2,&P2_body);
			get_ankle_angle(&leg2,&P2_body);
			angle_to_pwm(&leg2);
			Drive_Legx(&leg2);
			
			get_hip_angle(&leg3, &P3_body);	
			get_knee_angle(&leg3,&P3_body);
			get_ankle_angle(&leg3,&P3_body);
			angle_to_pwm(&leg3);
			Drive_Legx(&leg3);
			
			get_hip_angle(&leg4, &P4_body);	
			get_knee_angle(&leg4,&P4_body);
			get_ankle_angle(&leg4,&P4_body);
			angle_to_pwm(&leg4);
			Drive_Legx(&leg4);
			
			get_hip_angle(&leg5, &P5_body);	
			get_knee_angle(&leg5,&P5_body);
			get_ankle_angle(&leg5,&P5_body);
			angle_to_pwm(&leg5);
			Drive_Legx(&leg5);
			
			get_hip_angle(&leg6, &P6_body);	
			get_knee_angle(&leg6,&P6_body);
			get_ankle_angle(&leg6,&P6_body);
			angle_to_pwm(&leg6);
			Drive_Legx(&leg6);
			
			OSTimeDlyHMSM(0,0,delay_time, 0,OS_OPT_TIME_DLY, &err);

}

/*********************************************************************************************************
*	函 数 名: Go_straight
*	功能说明: 组腿提起，2组向前平移，然后1组落下；2组提起，1组向前平移，然后2组落下；周而复始
*	形    参：输入参数为表头结点
*	返 回 值: 无
*********************************************************************************************************
*/
void Go_Straight(u8 Com_Num,u8 * Input_num)
{
	Pace_135_Motor2(0,100,STRAIGHT_DELAY);		//UP
	while(Com_Num == *Input_num)
	{
		  Pace_246_Motor3(0,80,STRAIGHT_DELAY);		//FRONT
	   	Pace_135_Motor2(100,-100,STRAIGHT_DELAY);	//DOWM
		  Pace_246_Motor2(0,100,STRAIGHT_DELAY);		//UP
		  Pace_246_Motor3(80,-80,STRAIGHT_DELAY);		//RETURN
		  Pace_135_Motor3(0,80,STRAIGHT_DELAY);		//FRONT
		  Pace_246_Motor2(100,-100,STRAIGHT_DELAY);	//DOWM
		  Pace_135_Motor2(0,100,STRAIGHT_DELAY);		//UP
		  Pace_135_Motor3(80,-80,STRAIGHT_DELAY);		//RETURN
	}
	
	Pace_135_Motor2(100,-100,STRAIGHT_DELAY);		//DOWM
	
}
/*********************************************************************************************************
*	函 数 名: Go_back
*	功能说明: 
*	形    参：输入参数为表头结点
*	返 回 值: 无
*********************************************************************************************************
*/
void Go_Back(u8 Com_Num,u8 * Input_num)
{
	Pace_135_Motor2(0,100,ACTION_DELAY);		//UP
	while(Com_Num == *Input_num)
	{
		Pace_246_Motor3(0,-80,ACTION_DELAY);	//BACK
	   	Pace_135_Motor2(100,-100,ACTION_DELAY);	//DOWM
		Pace_246_Motor2(0,100,ACTION_DELAY);	//UP
		Pace_246_Motor3(-80,80,ACTION_DELAY);	//RETURN
		Pace_135_Motor3(0,-80,ACTION_DELAY);	//BACK
		Pace_246_Motor2(100,-100,ACTION_DELAY);	//DOWM
		Pace_135_Motor2(0,100,ACTION_DELAY);	//UP
		Pace_135_Motor3(-80,80,ACTION_DELAY);	//RETURN
	}
	Pace_135_Motor2(100,-100,ACTION_DELAY);	//DOWM
}
/*********************************************************************************************************
*	函 数 名: Stamp
*	功能说明: 跺脚动作
*	形    参：输入参数为表头结点
*	返 回 值: 无
*********************************************************************************************************
*/
void Stamp(u8 Com_Num,u8 * Input_num)
{
	Pace_135_Motor2(0,150,STAMP_DELAY);		//UP
	while(Com_Num == *Input_num)
	{
	   	Pace_135_Motor2(150,-150,STAMP_DELAY);	//DOWM
		  Pace_246_Motor2(0,150,STAMP_DELAY);	//UP
		  Pace_246_Motor2(150,-150,STAMP_DELAY);	//DOWM
		  Pace_135_Motor2(0,150,STAMP_DELAY);	//UP
	}
	Pace_135_Motor2(150,-150,STAMP_DELAY);	//DOWM
}
/*********************************************************************************************************
*	函 数 名: Turn_Left
*	功能说明: 跺脚动作
*	形    参：输入参数为表头结点
*	返 回 值: 无
*********************************************************************************************************
*/
void Turn_Left(u8 Com_Num,u8 * Input_num)
{
	while(Com_Num == *Input_num)
	{
		Pace_135_246_Motor3(0,0,70,TURN_DELAY);     //turn body left

		Pace_246_Motor2(0,100,TURN_DELAY);			  //UP
		Pace_246_Motor3_Different(70,-70,TURN_DELAY);	  //2,4,6 leg return
		Pace_246_Motor2(100,-100,TURN_DELAY);		      //DOWM
		
		Pace_135_Motor2(0,100,TURN_DELAY);			  //UP
		Pace_135_Motor3_Different(70,-70,TURN_DELAY);	  //1,3,5 leg return
		Pace_135_Motor2(100,-100,TURN_DELAY);		      //DOWM
		
	}
}

/***************************** 阿波罗科技 www.apollorobot.com (END OF FILE) *********************************/
