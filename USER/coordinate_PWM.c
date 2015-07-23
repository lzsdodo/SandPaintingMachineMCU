#include "coordinate_PWM.h"
#include "accept_coordinate.h"

float PWM_per_mm=341.333;//ÿ���������������(25600/r)
u16 Z_MOVE=6826;	 //z��ÿ��̧������Ϊ1����
/*
	�ú�����Ҫʵ�ֶ�Ӧ��ǰ���꣬������ƶ�����һ������X,Y,Z�������ƶ��ķ�����������
	���������new_x,new_y,new_z��ʾ�����꣬p_a_d��ʾ�洢��������ͷ���Ľṹ��
*/
u16 XYZ_to_pwm(u16 now_x,u16 now_y,u16 now_z,u16 new_x,u16 new_y,u16 new_z,struct PWM_and_DIR* p_a_d )
{
	int change_x=0;
	int change_y=0;
	int change_z=0;
	if( (new_x>=MAX_X)||(new_y>=MAX_Y)||(new_z>=MAX_Z) )	//���겻�Ϸ�����0
	{								    
		return 0;
	}
	change_x = new_x-now_x;
	p_a_d->dir_x = ADVANCE;
	change_y = new_y-now_y;
	p_a_d->dir_y = ADVANCE;
	change_z = new_z-now_z;
	p_a_d->dir_z = ADVANCE;
	if((change_x&0x8000)>0)	//���change_x����������1����ת��Ϊ����
	{
		change_x = ~change_x+1;
		p_a_d->dir_x = RETREAT;	//�����Ϊ����
	}
	if((change_y&0x8000)>0)
	{
		change_y = ~change_y+1;
		p_a_d->dir_y = RETREAT;
	}
	if((change_z&0x8000)>0)
	{
		change_z = ~change_z+1;
		p_a_d->dir_z = RETREAT;
	}
	p_a_d->PWM_x = change_x*PWM_per_mm;
	p_a_d->PWM_y = change_y*PWM_per_mm;
	p_a_d->PWM_z = change_z*Z_MOVE;
	return 1;
}
		

