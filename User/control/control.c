
#include "control.h"



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
*       Public Code
*
**********************************************************************
*/
//
//
//
void Position_Reset(void)
{
		HIP_1_OUT = pwm_init[1];	 //1-1-1  	   
		HIP_2_OUT = pwm_init[5];	 //2-1-5  	   
		HIP_3_OUT = pwm_init[9];	 //3-1-9  	   
		HIP_4_OUT = pwm_init[12];  //4-1-12
    HIP_5_OUT = pwm_init[8];   //5-1-8  	
	  HIP_6_OUT = pwm_init[4];	 //6-1-4  
	
	  KNEE_1_OUT = pwm_init[20];	 //1-2-20
	  KNEE_2_OUT = pwm_init[6];	   //2-2-6 
	  KNEE_3_OUT = pwm_init[15];	 //3-2-15
	  KNEE_4_OUT = pwm_init[7];	   //4-2-7
	  KNEE_5_OUT = pwm_init[21];	 //5-2-21
	  KNEE_6_OUT = pwm_init[24];	 //6-2-24
	
	  ANKLE_1_OUT = pwm_init[19];	 //1-3-19
	  ANKLE_2_OUT = pwm_init[2];	 //2-3-2
    ANKLE_3_OUT = pwm_init[16];	 //3-3-16 
		ANKLE_4_OUT = pwm_init[18];	 //4-3-18
		ANKLE_5_OUT = pwm_init[22];	 //5-3-22 
		ANKLE_6_OUT = pwm_init[23];	 //6-3-23  	   
		 	   
}
//
//
//
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
//
//
//
void Stand_Up(int delay_time)
{
		int pwm_tmp = 0;
		int pwm_tmp2 = 0;
	  OS_ERR       err;
//		Open_All_TIM();			//Open all timer
	  /*设置所有腿的Ankle关节*/
		ANKLE_2_OUT = pwm_init[2];	 //2-3-2
		ANKLE_4_OUT = pwm_init[18];	 //4-3-18
		ANKLE_6_OUT = pwm_init[23];	 //6-3-23  	   
		ANKLE_1_OUT = pwm_init[19];	 //1-3-19
		ANKLE_3_OUT = pwm_init[16];	 //3-3-16  	   
	  ANKLE_5_OUT = pwm_init[22];	 //5-3-22  	   
		for (;pwm_tmp<=STAND_HIGHT;)				 //150 efect the height of stand
		{
				OSTimeDlyHMSM(0,0,0,delay_time,OS_OPT_TIME_DLY, &err);
				/*设置所有退的Knee关节*/
				KNEE_3_OUT = pwm_init[15] - STAND_HIGHT + pwm_tmp;	 //3-2-15	   
				KNEE_1_OUT = pwm_init[20] - STAND_HIGHT + pwm_tmp;	 //1-2-20
				KNEE_2_OUT = pwm_init[6]  - STAND_HIGHT + pwm_tmp;	 //2-2-6	   	   
				KNEE_4_OUT = pwm_init[7]  + STAND_HIGHT - pwm_tmp;	 //4-2-7	
				KNEE_6_OUT = pwm_init[24] + STAND_HIGHT - pwm_tmp;   //6-2-24 
				KNEE_5_OUT = pwm_init[21] + STAND_HIGHT - pwm_tmp;	 //5-2-21
				
				/*设置所有腿的Hip关节*/
				HIP_1_OUT = pwm_init[1] - SHRINK_DEGREE + pwm_tmp2;	 //1-1-1  	   
				HIP_2_OUT = pwm_init[5] - SHRINK_DEGREE + pwm_tmp2;	 //2-1-5  	   
				HIP_3_OUT = pwm_init[9] - SHRINK_DEGREE + pwm_tmp2;	 //3-1-9  
				HIP_4_OUT = pwm_init[12] + SHRINK_DEGREE - pwm_tmp2; //4-1-12  	   
				HIP_5_OUT = pwm_init[8]  + SHRINK_DEGREE - pwm_tmp2; //5-1-8  	   
				HIP_6_OUT = pwm_init[4]  + SHRINK_DEGREE - pwm_tmp2; //6-1-4  	   
				
				pwm_tmp += ACTION_SPEED;	
				pwm_tmp2 += SHRINK_SPEED;	
		}
}
//
//
//
void Pace_135_Motor1(int pwm_default,int amplitude,int delay_time)
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
//
//
//
void Pace_135_Motor2(int pwm_default,int amplitude,int delay_time)
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
//
//
//
void Pace_135_Motor3(int pwm_default,int amplitude,int delay_time)
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
//
//
//
void Pace_135_Motor3_Different(int pwm_default,int amplitude,int delay_time)
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
//
//
//
void Pace_246_Motor2(int pwm_default,int amplitude,int delay_time)
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
//
//
//
void Pace_246_Motor3(int pwm_default,int amplitude,int delay_time)
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
//
//
//
void Pace_246_Motor3_Different(int pwm_default,int amplitude,int delay_time)
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
//
//
//
void Pace_135_246_Motor3(int pwm_def1,int pwm_def2,int amplitude,int delay_time)
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

//
//
//
/*1组腿提起，2组向前平移，然后1组落下；2组提起，1组向前平移，然后2组落下；周而复始*/
void Go_straight(u8 Com_Num,u8 * Input_num)
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
//
//
//
void Go_back(u8 Com_Num,u8 * Input_num)
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
//
//
//
/*跺脚动作*/
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
//
//
//
void Turn(u8 Com_Num,u8 * Input_num)
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
