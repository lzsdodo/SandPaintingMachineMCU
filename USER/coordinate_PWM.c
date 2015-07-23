#include "coordinate_PWM.h"
#include "accept_coordinate.h"

float PWM_per_mm=341.333;//每毫米所需的脉冲数(25600/r)
u16 Z_MOVE=6826;	 //z轴每次抬升距离为1厘米
/*
	该函数主要实现对应当前坐标，计算出移动到下一个坐标X,Y,Z轴所需移动的方向和脉冲个数
	输入参数：new_x,new_y,new_z表示新坐标，p_a_d表示存储脉冲个数和方向的结构体
*/
u16 XYZ_to_pwm(u16 now_x,u16 now_y,u16 now_z,u16 new_x,u16 new_y,u16 new_z,struct PWM_and_DIR* p_a_d )
{
	int change_x=0;
	int change_y=0;
	int change_z=0;
	if( (new_x>=MAX_X)||(new_y>=MAX_Y)||(new_z>=MAX_Z) )	//坐标不合法返回0
	{								    
		return 0;
	}
	change_x = new_x-now_x;
	p_a_d->dir_x = ADVANCE;
	change_y = new_y-now_y;
	p_a_d->dir_y = ADVANCE;
	change_z = new_z-now_z;
	p_a_d->dir_z = ADVANCE;
	if((change_x&0x8000)>0)	//如果change_x溢出，则反码加1将其转换为正数
	{
		change_x = ~change_x+1;
		p_a_d->dir_x = RETREAT;	//方向改为后退
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
		

