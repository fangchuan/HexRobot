#ifndef __COMMON_H
#define __COMMON_H

#include "stdlib.h"
#include "math.h"
#include "string.h"
#include "stdio.h"
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
#define  L3_Length     130      //mm   第三关节连杆的长度
#define  L3L3          16900    //L3的平方
#define  L0            80       //mm   机体中心到第一关节的距离
#define  L0L0          6400     //L0的平方
#define  L0X2          160      //2*L0
#define  L2X2          180      //2*L2
#define  L2L3X2        21600    //2*L2*L3
#define  D1_DEF        165      //臀关节到立足点的直线距离  默认值
#define  D1_STRATCH    240      //腿伸展状态下  臀关节到立足点的直线距离

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

#define  DIRECTION_C   1       //顺时针
#define  DIRECTION_CC  -1      //逆时针

#define ID_MIN    1 
#define ID_MAX    6
#define ID_LEG_1  1
#define ID_LEG_2  2
#define ID_LEG_3  3
#define ID_LEG_4  4
#define ID_LEG_5  5
#define ID_LEG_6  6

#define PWM_MAX  		2500
#define PWM_MIN  		500
#define PWM_MID     1500
#define ANGLE_MAX   225
#define ANGLE_MIN   0
#define PARAMETER            (float)((PWM_MAX-PWM_MIN)/ANGLE_MAX) 
#define HEAD_LEFT_MARGIN   1000
#define HEAD_RIGHT_MARGIN  2000
#define HEAD_STEP          100


#define DEGREE_TO_RAD        (float)0.017453292519943295769236907684886
#define RAD_TO_DEGREE        (float)57.295779513082320876798154814105
#define min(a,b)             ((a)<(b)?(a):(b))
#define max(a,b)             ((a)>(b)?(a):(b))
#define constrain(amt,low,high)   ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define radians(deg)         ((deg)*DEGREE_TO_RAD)
#define degrees(rad)         ((rad)*RAD_TO_DEGREE)

//复位状态下关节角度的值
#define HIP_1_DEF_ANGLE        41
#define HIP_2_DEF_ANGLE        0  //
#define HIP_3_DEF_ANGLE        319
#define HIP_4_DEF_ANGLE        221
#define HIP_5_DEF_ANGLE        180
#define HIP_6_DEF_ANGLE        139
#define KNEE_DEF_ANGLE         75
#define ANKLE_DEF_ANGLE        122
//关节角度的极限值
#define HIP_MIN                -60
#define HIP_MAX                60
#define KNEE_MIN               -40
#define KNEE_MAX               40
#define ANKLE_MIN              -60
#define ANKLE_MAX              60

//4个临界方向角
#define  CRITICAL_ALPHA_1    (float)0.349
#define  CRITICAL_ALPHA_2    (float)2.79
#define  CRITICAL_ALPHA_3    (float)3.49
#define  CRITICAL_ALPHA_4    (float)5.934
#define  PI_1_2              (float)1.570796
#define  PI                  (float)3.1415926
#define  PI_3_2              (float)4.712388
#define  PI_2                (float)0
	
//一个周期的步长
#define  STRIDE              150//mm
//默认转角--20°
#define  DEF_TURN_ANGLE      0.349065
//站立状态下的机体高度
#define  HEIGHT_BODY         130//mm
//摆动腿的足端高度
#define  HEIGHT_TRANSFOOT    -50 //mm
//舵机转动速度--w 
#define ACTION_SPEED 	       7		 //range:0 - 9
#define SHRINK_SPEED 	      ((u8)((0.6)*ACTION_SPEED))
#define ACTION_DELAY         20      //ms	 //10   
#define TURN_DELAY 		       20      //ms  //3
#define STRAIGHT_DELAY       20      //ms	 //3

//直线行走方向
#define  STRAIGHT_FORWARD    PI_1_2
#define  STRAIGHT_BACKWARD   PI_3_2
#define  STRAIGHT_LEFT       PI
#define  STRAIGHT_RIGHT      PI_2
//占地系数，决定步态的生成
#define  DUTY_25             25
#define  DUTY_50             50
#define  DUTY_75             75

#define ANGLE_TO_PWM(angle)  (angle*PARAMETER)
#define PWM_TO_ANGLE(pwm)    (float)((pwm - PWM_MIN)*0.25)
	
//
//机器人控制模式
//
typedef enum _CONTROLMODE{
	            
	            NRF_MODE = 1,
	            ANDROID_MODE = 2,
	            PC_MODE = 3,
	            AUTO_MODE = 4,
	
}_ControlMode;
//
//机器人运动类型
//
typedef enum _ORDER {
             GO_FORWARD = 1,
	           GO_BACKWARD= 2,
	           TURN_LEFT  = 3,
	           TURN_RIGHT = 4,
	           STOP       = 5,
	           LIFT_LEG1  = 6,
	           LIFT_LEG3  = 7,
	           LIFT_LEG4  = 8,
	           LIFT_LEG6  = 9,
	           STAMP      = 10,
	           TURN_HEAD_LEFT = 11,
	           TURN_HEAD_RIGHT = 12,
}_Order;
//
//robot control task ADT
//
#define  FIFO_MAX_SIZE    31   //最大支持30个任务
#define  FIFO_EMPTY       -1
#define  FIFO_FULL        -2
#define  NO_ERR           0
typedef struct {
        _Order   task_array[FIFO_MAX_SIZE];
	      unsigned short  front;
	      unsigned short  rear;
	      unsigned short  size;
}_Task;
//
//描述关节位置的数据结构
//
typedef struct {
				float    x;
	      float    y;
	      float    z;
}Position;

//
//描述单腿的数据结构
//
typedef struct {
				unsigned char     id;//腿的标号
	      float      hip_angle;//0--180
	      float     knee_angle;
	      float    ankle_angle;
	      int         velocity;//腿的速度
	      int          hip_pwm;//最终输出的pwm
	      int         knee_pwm;
	      int        ankle_pwm;
//	      Position* position;//足端位置
}LEG;

//
//矩阵数据结构
//
typedef struct _Vector3f
{
	      float x, y, z;
	
} Vector3f;

typedef struct _Matrix3f
{
	      Vector3f a, b, c;
	
} Matrix3f;

//
//描述机体传感器的数据结构
//
#define  SAFE_ULTRASNIO_DISTANCE    25     //超声波安全距离
typedef struct {
	      int    candela_1;   //1号传感器的光强
	      int    candela_2;   //2
	      int    candela_3;   //3
	      int    candela_4;   //4
	      float left_distance; //左侧超声波距离
	      float right_distance;//右侧
   
}_Sensor;
extern int issafe(int value, int min, int max);
extern float constrain_180(float error_angle);
extern void update_DCM(float angle, Matrix3f* m);
extern void update_DCM_T(float angle, Matrix3f* m);
extern Position DCM_Multiply_Position(Position* p, Matrix3f* m);
extern void update_Jacobi(float a1, float a2,  float a3, Matrix3f* m);
extern void init_task_fifo(_Task* task);
extern int fifo_put_task(_Task*task, _Order order);
extern int fifo_get_task(_Task* task, _Order* current_order);
#endif
/***************************** 阿波罗科技 www.apollorobot.com (END OF FILE) *********************************/
