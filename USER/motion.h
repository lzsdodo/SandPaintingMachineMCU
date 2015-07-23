#ifndef _MOTION_H
#define _MOTION_H
#include "sys.h"
#include "accept_coordinate.h"
#include "timer.h"
#include "coordinate_PWM.h"
#include "draw.h"
#include "delay.h"
#include "led.h"

#define AXLE_X 1 //x��
#define AXLE_Y 2 //Y��
#define AXLE_Z 3 //Z��
#define TIM_1 1
#define TIM_2 2 //��ʱ��1�Ͷ�ʱ��2
#define TIM_4 4//��ʱ��4

//�ýṹ�����ڼ�¼�ƶ�����һ�������Ӧ��x��y,z�����������PWM��������Լ��˶�����
static struct Point Draw_point[SIZE]={{0,0}};
extern struct PWM_and_DIR p_a_d;
float Motion_mode(struct Coordinate* next_point);//�ú��������ж��Ƶ���һ������ƶ����ͣ�б�߻�ֱ��
void Acceleration_curves(int p_c_n,u16 dir1,u16 timer,float type);//�ú�������ʵ�ּ������߹��ܣ�����������/����̨�׵�PWM�������
u16 Move_one_axle(u16 axle,u16 dir,int p_c_n,u16 type);//�ú�����Ҫ����ƶ�һ����
u16 Move_to(struct Coordinate* next_point,u16 special_type);//�ú�������ƶ�����һ���� ,��Բ������ԲΪspecial_type

//��б�ߣ�Բ����Բ,typeѡ��ͼ������,type=0,��б�ߣ�type=1,��Բ����type>1����Բ,point1��ʾ��㣬point2��ʾ�յ�
u16 Draw(float type,struct Coordinate* point1);
#endif

