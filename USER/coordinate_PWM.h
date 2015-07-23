#ifndef _COORDINATE_PWM_H
#define _COORDINATE_PWM_H
#include "sys.h"
/*
	该部分主要完成的功能为，分别计算x,Y,Z 轴移动距离对应的PWM脉冲个数
*/
/*
	(X,Y)坐标定位500*500，每一格为1平方毫米,Z轴定为300单位为毫米
*/
#define	ADVANCE 1 //靠近电机，坐标增大
#define RETREAT 0 //远离电机，坐标变小
//static u16 NOW_X=0;//用于记录当前坐标
//static u16 NOW_Y=0;
//static u16 NOW_Z=0;
struct PWM_and_DIR
{
	u16 dir_x;
	u16 dir_y;
	u16 dir_z;
	int PWM_x;
	int PWM_y;
	int PWM_z;
};

//8细分时每一圈需要的脉冲个数1600
//16细分时每一圈需要的脉冲个数3200
//u16 LENGTH_OF_CIRCLE=75;//每转一圈走过的距离,单位为毫米

//u16 Max_length=60;
//float LENGTH_OF_PWM=0.0234;//每个PWM脉冲走过的距离单位为毫米
u16 XYZ_to_pwm(u16 now_x,u16 now_y,u16 now_z,u16 new_x,u16 new_y,u16 new_z,struct PWM_and_DIR* p_a_d );
#endif
