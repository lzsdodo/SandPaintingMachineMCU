#ifndef _MOTION_H
#define _MOTION_H
#include "sys.h"
#include "accept_coordinate.h"
#include "timer.h"
#include "coordinate_PWM.h"
#include "draw.h"
#include "delay.h"
#include "led.h"

#define AXLE_X 1 //x轴
#define AXLE_Y 2 //Y轴
#define AXLE_Z 3 //Z轴
#define TIM_1 1
#define TIM_2 2 //定时器1和定时器2
#define TIM_4 4//定时器4

//该结构体用于记录移动到下一组坐标对应的x，y,z轴所需输出的PWM脉冲个数以及运动方向
static struct Point Draw_point[SIZE]={{0,0}};
extern struct PWM_and_DIR p_a_d;
float Motion_mode(struct Coordinate* next_point);//该函数用于判断移到下一个点的移动类型，斜线或直线
void Acceleration_curves(int p_c_n,u16 dir1,u16 timer,float type);//该函数用于实现加速曲线功能，负责分配各加/减速台阶的PWM脉冲个数
u16 Move_one_axle(u16 axle,u16 dir,int p_c_n,u16 type);//该函数主要完成移动一个轴
u16 Move_to(struct Coordinate* next_point,u16 special_type);//该函数完成移动到下一个点 ,画圆弧和椭圆为special_type

//画斜线，圆，椭圆,type选择图像类型,type=0,画斜线，type=1,画圆弧，type>1画椭圆,point1表示起点，point2表示终点
u16 Draw(float type,struct Coordinate* point1);
#endif

