#ifndef _COORDINATE_PWM_H
#define _COORDINATE_PWM_H
#include "sys.h"
/*
	�ò�����Ҫ��ɵĹ���Ϊ���ֱ����x,Y,Z ���ƶ������Ӧ��PWM�������
*/
/*
	(X,Y)���궨λ500*500��ÿһ��Ϊ1ƽ������,Z�ᶨΪ300��λΪ����
*/
#define	ADVANCE 1 //�����������������
#define RETREAT 0 //Զ�����������С
//static u16 NOW_X=0;//���ڼ�¼��ǰ����
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

//8ϸ��ʱÿһȦ��Ҫ���������1600
//16ϸ��ʱÿһȦ��Ҫ���������3200
//u16 LENGTH_OF_CIRCLE=75;//ÿתһȦ�߹��ľ���,��λΪ����

//u16 Max_length=60;
//float LENGTH_OF_PWM=0.0234;//ÿ��PWM�����߹��ľ��뵥λΪ����
u16 XYZ_to_pwm(u16 now_x,u16 now_y,u16 now_z,u16 new_x,u16 new_y,u16 new_z,struct PWM_and_DIR* p_a_d );
#endif
