#ifndef __COMMON_H
#define __COMMON_H

#include "stdlib.h"
#include "math.h"
#include "string.h"
/*********************************************************************
*
*       MACRO Define
*
**********************************************************************
*/
#define  L1_Length      25      //mm   ��һ�ؽ����˵ĳ���
#define  L1L1           625     //L1��ƽ��
#define  L2_Length      90      //mm   �ڶ��ؽ����˵ĳ���
#define  L2L2          8100     //L2��ƽ��
#define  L3_Length     130      //mm   �����ؽ����˵ĳ���
#define  L3L3          16900    //L3��ƽ��
#define  L0            80       //mm   �������ĵ���һ�ؽڵľ���
#define  L0L0          6400     //L0��ƽ��
#define  L0X2          160      //2*L0
#define  L2X2          180      //2*L2
#define  L2L3X2        21600    //2*L2*L3

#define  COSTH1        -0.5     //cos120  Leg1��һ�ؽڵ��������ĵļн�
#define  SINTH1        0.866    //sin120
#define  COSTH2        -1       //cos180  Leg2��һ�ؽڵ��������ĵļн�
#define  SINTH2         0       //sin180
#define  COSTH3        -0.5     //cos240  Leg3��һ�ؽڵ��������ĵļн�
#define  SINTH3        -0.866   //sin240
#define  COSTH4        0.5      //cos300  Leg4��һ�ؽڵ��������ĵļн�
#define  SINTH4        -0.866   //sin300
#define  COSTH5        1        //cos0    Leg5��һ�ؽڵ��������ĵļн�
#define  SINTH5        0        //sin0
#define  COSTH6        0.5      //cos60   Leg6��һ�ؽڵ��������ĵļн�
#define  SINTH6        0.866    //sin60

#define  DIRECTION_C   1       //˳ʱ��
#define  DIRECTION_CC  -1      //��ʱ��

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
#define DEGREE_TO_RAD        (float)0.017453292519943295769236907684886
#define RAD_TO_DEGREE        (float)57.295779513082320876798154814105

#define min(a,b)             ((a)<(b)?(a):(b))
#define max(a,b)             ((a)>(b)?(a):(b))
#define constrain(amt,low,high)   ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define radians(deg)         ((deg)*DEG_TO_RAD)
#define degrees(rad)         ((rad)*RAD_TO_DEG)

//��λ״̬�¹ؽڽǶȵ�ֵ
#define HIP_1_DEF_ANGLE        41
#define HIP_2_DEF_ANGLE        0  //
#define HIP_3_DEF_ANGLE        319
#define HIP_4_DEF_ANGLE        221
#define HIP_5_DEF_ANGLE        180
#define HIP_6_DEF_ANGLE        139
#define KNEE_DEF_ANGLE         75
#define ANKLE_DEF_ANGLE        122
//�ؽڽǶȵļ���ֵ
#define HIP_MIN                -60
#define HIP_MAX                60
#define KNEE_MIN               -40
#define KNEE_MAX               40
#define ANKLE_MIN              -60
#define ANKLE_MAX              60

//4���ٽ緽���
#define  CRITICAL_ALPHA_1    (float)0.349
#define  CRITICAL_ALPHA_2    (float)2.79
#define  CRITICAL_ALPHA_3    (float)3.49
#define  CRITICAL_ALPHA_4    (float)5.934
#define  PI_1_2              (float)1.570796
#define  PI                  (float)3.1415926
#define  PI_3_2              (float)4.712388
#define  PI_2                (float)0
	
//һ�����ڵĲ���
#define  STRIDE              180//mm
//Ĭ��ת��--20��
#define  DEF_TURN_ANGLE      0.349065
//վ��״̬�µĻ���߶�
#define  HEIGHT_BODY         130//mm
//�ڶ��ȵ���˸߶�
#define  HEIGHT_TRANSFOOT    -70 //mm
//ֱ�����߷���
#define  STRAIGHT_FORWARD    PI_1_2
#define  STRAIGHT_BACKWARD   PI_3_2
#define  STRAIGHT_LEFT       PI
#define  STRAIGHT_RIGHT      PI_2
//ռ��ϵ����������̬������
#define  DUTY_25             25
#define  DUTY_50             50
#define  DUTY_75             75

#define ANGLE_TO_PWM(angle)  (angle*PARAMETER)
#define PWM_TO_ANGLE(pwm)    (float)((pwm - PWM_MIN)*0.25)
	
//
//�����ؽ�λ�õ����ݽṹ
//
typedef struct {
				float    x;
	      float    y;
	      float    z;
}Position;

//
//�������ȵ����ݽṹ
//
typedef struct {
				unsigned char     id;//�ȵı��
	      float      hip_angle;//0--180
	      float     knee_angle;
	      float    ankle_angle;
	      int         velocity;//�ȵ��ٶ�
	      int          hip_pwm;//���������pwm
	      int         knee_pwm;
	      int        ankle_pwm;
//	      Position* position;//���λ��
}LEG;

//
//�������ݽṹ
//
typedef struct _Vector3f
{
	      float x, y, z;
	
} Vector3f;

typedef struct _Matrix3f
{
	      Vector3f a, b, c;
	
} Matrix3f;

extern int issafe(int value, int min, int max);
extern float constrain_180(float error_angle);
extern void update_DCM(float angle, Matrix3f* m);
extern void update_DCM_T(float angle, Matrix3f* m);
extern Position DCM_Multiply_Position(Position* p, Matrix3f* m);

#endif
/***************************** �����޿Ƽ� www.apollorobot.com (END OF FILE) *********************************/
