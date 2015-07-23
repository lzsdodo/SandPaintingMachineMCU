#include "motion.h"
#define MAX_MULTIPLE 3
struct Point Point_cache[SIZE];
const u16 Criterion_ARR[21]={1500,900,450,300,225,150,90,75,56,50,45,50,56,75,90,150,225,300,450,900,1500};
//const u16 Criterion_ARR2[21]={12000,7200,3600,2400,1800,1200,720,600,450,400,360,400,450,600,720,1200,1800,2400,3600,7200,12000};
/*
	该函数用于调节PWM频率，从而调节电机转速。
	调节分为三个过程加速-匀速-减速，每个阶段各占不同比例
	加速和减速又可分为20个阶段
*/
void Acceleration_curves(int p_c_n,u16 dir1,u16 timer,float type)//p_c_n表示所需的脉冲数,dir表示方向,timer表示计数器
{
	u16 i;
	int tem;	 
	//TIM2_CH=ch;
	 if(p_c_n>0)
	 {
		if(timer==TIM_1)
		{
			for(i=0;i<21;i++)
			{
				PWM_counter_part1[i]=0;
				ARR1[i]=Criterion_ARR[i];
			}
			//分配不同加速阶段的PWM脉冲个数
			if(p_c_n>=300||type>0)
			{
				PWM_counter_part1[0]=10;
				PWM_counter_part1[20]=10;
				tem=p_c_n/30.0;
				for(i=1;i<10;i++)
				{
					PWM_counter_part1[i]=tem;
					PWM_counter_part1[20-i]=tem;	
				}
				PWM_counter_part1[10]=p_c_n-PWM_counter_part1[1]*18-PWM_counter_part1[0]-PWM_counter_part1[20];	
			}
			else if(p_c_n>100)
			{
				PWM_counter_part1[0]=10;
				PWM_counter_part1[20]=10;
				tem=p_c_n-PWM_counter_part1[0]*2;
				tem=tem/8;
				 for(i=1;i<4;i++)
				 {
				 	 PWM_counter_part1[i]=tem;
					 PWM_counter_part1[20-i]=tem;
				 }
				 PWM_counter_part1[4]=tem;
				 PWM_counter_part1[16]=p_c_n-tem*7-20;
			}
			else
			{
				tem=p_c_n/4;
				PWM_counter_part1[0]=tem;
				PWM_counter_part1[19]=tem;
				PWM_counter_part1[20]=tem;
				PWM_counter_part1[1]=p_c_n-tem*3;
			}
			tem=0;
			for(i=0;i<21;i++)
			{
				tem+=PWM_counter_part1[i];
				encoder1[i]=tem*PWM_to_Encoder;
			}	
			if(dir1>0)
			{
				DIR_X=1;
				Original_Dir1=1;
			}
			else
			{
				DIR_X=0;
				Original_Dir1=0;
			}
			if(type>0)
				{
					if(type>MAX_MULTIPLE)
					{
						for(i=0;i<21;i++)
						{
							ARR1[i]=Criterion_ARR[i]/MAX_MULTIPLE;
						}
					}
					else if(type>2)
					{
						for(i=0;i<21;i++)
						{
							ARR1[i]=Criterion_ARR[i]*2/type;
						}
					}
					else 
					{
					   for(i=0;i<21;i++)
						{
							ARR1[i]=Criterion_ARR[i]/type;
						}
					}
				}	
		}
		if(timer==TIM_2)
		{
			for(i=0;i<21;i++)
			{
				PWM_counter_part2[i]=0;
				ARR2[i]=Criterion_ARR[i];
			}
			//分配不同加速阶段的PWM脉冲个数
			if(p_c_n>=300||type>0)
			{
				PWM_counter_part2[0]=10;
				PWM_counter_part2[20]=10;
				tem=p_c_n/30;
				for(i=1;i<10;i++)
				{
					PWM_counter_part2[i]=tem;
					PWM_counter_part2[20-i]=tem;	
				}
				PWM_counter_part2[10]=p_c_n-PWM_counter_part2[1]*18-PWM_counter_part2[0]-PWM_counter_part2[20];				
			}
			else if(p_c_n>100)
			{
				PWM_counter_part2[0]=10;
				PWM_counter_part2[20]=10;
				tem=p_c_n-PWM_counter_part2[0]*2;
				tem=tem/8;
				 for(i=1;i<4;i++)
				 {
				 	 PWM_counter_part2[i]=tem;
					 PWM_counter_part2[20-i]=tem;
				 }
				 PWM_counter_part2[4]=tem;
				 PWM_counter_part2[16]=p_c_n-tem*7-20;
				
			}
			else
			{
				tem=p_c_n/4;
				PWM_counter_part2[0]=tem;
				PWM_counter_part2[19]=tem;
				PWM_counter_part2[20]=tem;
				PWM_counter_part2[1]=p_c_n-tem*3;
			}
			tem=0;
			if(dir1>0)
			{
				DIR_Y=1;
			}
			else
			{
				DIR_Y=0;
			 }
			 	if(type>0)
				{
					if(type>MAX_MULTIPLE)
					{
						for(i=0;i<21;i++)
						{
							ARR2[i]=Criterion_ARR[i]*type/MAX_MULTIPLE;
						}
					}
					else if(type>2)
					{
						for(i=0;i<21;i++)
						{
							ARR2[i]=Criterion_ARR[i]*2;
						}
					}
					else 
					{
					   for(i=0;i<21;i++)
						{
							ARR2[i]=Criterion_ARR[i];
						}
					}
				}		
		}	  
		if(timer==TIM_4)
		{
			for(i=0;i<21;i++)
			{
				PWM_counter_part4[i]=0;
				ARR4[i]=Criterion_ARR[i];
			}
			//分配不同加速阶段的PWM脉冲个数
				PWM_counter_part4[0]=10;
				PWM_counter_part4[20]=10;
				tem=p_c_n/30;
				for(i=1;i<10;i++)
				{
					PWM_counter_part4[i]=tem;
					PWM_counter_part4[20-i]=tem;	
				}
				PWM_counter_part4[10]=p_c_n-PWM_counter_part4[1]*18-PWM_counter_part4[0]-PWM_counter_part4[20];
				if(dir1>0)
				DIR_Z=1;
				else
				DIR_Z=0;
		}
	 }
}

/*
	判断运动类型,画直线则返回0，画斜线则返回1
*/
float Motion_mode(struct Coordinate* next_point)
{
	float type=0;
	float change_x=0;//x轴改变量
	float change_y=0;//y轴改变量
	if((next_point->NOW_X==Position_x)||(next_point->NOW_Y==Position_y))
	{
		return 0;
	}
	else
	{
		if(next_point->NOW_X>Position_x)
		{
			change_x=next_point->NOW_X-Position_x;
		}
		else
		{
			change_x=Position_x-next_point->NOW_X;
		}
		if(next_point->NOW_Y>Position_y)
		{
			change_y=next_point->NOW_Y-Position_y;
		}
		else
		{
			change_y=Position_y-next_point->NOW_Y;
		}
		type=(change_x*1.0)/(change_y*1.0);
		return type;
	}	
}

/*
	该函数主要实现单个轴的移动，传递参数为axle用于区分轴，dir方向，type表示运动类型
*/
u16 Move_one_axle(u16 axle,u16 dir,int p_c_n,u16 type)
{
	if(axle==AXLE_X)//移动X轴
	{
		Acceleration_curves(p_c_n,dir,TIM_1,type);
		PWM1_CH1_Init(ARR1[0],59,p_c_n);		
	}
	if(axle==AXLE_Y)
	{
		Acceleration_curves(p_c_n,dir,TIM_2,type);
		PWM2_CH2_Init(ARR2[0],59,p_c_n);			
	}
	if(axle==AXLE_Z)
	{
		Acceleration_curves(p_c_n,dir,TIM_4,type);
		PWM4_CH2_Init(ARR4[0],59,p_c_n);
	}
	return 0;
}

/*
	运动到下一个点
*/
u16 Move_to(struct Coordinate* next_point,u16 special_type)
{
	float type=0;//运动类型,表示走斜线或走直线 
	u16 result=0;
	struct PWM_and_DIR p_a_d={0,0,0,0,0,0};//用于一个记录一个坐标对应的PWM和方向信息
	if(special_type==0)	  //正常移动
	{
		type=Motion_mode(next_point);
		result=XYZ_to_pwm(next_point->NOW_X,next_point->NOW_Y,next_point->NOW_Z,&p_a_d );
		if(result==0)//坐标不合法，超出最大值
		{
			return 0;
		}
		if(p_a_d.PWM_z>0)
		{
			Move_one_axle(AXLE_Z,p_a_d.dir_z,p_a_d.PWM_z,type);	
		}
		while(TIM4_finish!=1)
		{
			delay_ms(1);
		}//等待Z轴走完
		if(p_a_d.PWM_x>0)
		{
			Move_one_axle(AXLE_X,p_a_d.dir_x,p_a_d.PWM_x,type);
		}
		if(p_a_d.PWM_y>0)
		{
			Move_one_axle(AXLE_Y,p_a_d.dir_y,p_a_d.PWM_y,type);
		}
		Position_x=next_point->NOW_X;
		Position_y=next_point->NOW_Y;
		Position_z=next_point->NOW_Z;//更新坐标
		Send_result(1,1);
	}
	else if(special_type==1)
	{	
	//画圆
	}
	else
	{
	//画椭圆
	}
	 return 1;
}

/*
	该函数主要实现画椭圆，画圆弧，画斜线 , 对于椭圆来说type=a/b
*/	  
u16 Draw(float type,struct Coordinate* point1)
{
	u16 result=0;
	int length=0,i;//length表示画线所需的点的个数
	struct Point tem_point1;
	struct Point tem_point2;
	float m_x=type;
	float m_y=1.0/type;
	struct Coordinate c_tem={0,0,0};
	tem_point1.x=Position_x;
	tem_point1.y=Position_y;
	tem_point2.x=point1->NOW_X;
	tem_point2.y=point1->NOW_Y;
	 if(type==0.0)	   //画直线
	 {
	 	 length=Draw_slash(&tem_point1,&tem_point2,Point_cache);
		 if(length<SIZE)
		 {
		 	for(i=0;i<length;i++)
		 	{
		 		c_tem.NOW_X=(u16)Point_cache[i].x;
				c_tem.NOW_Y=(u16)Point_cache[i].y;
				c_tem.NOW_Z=0;
				while((TIM1_finish==0)||(TIM2_finish==0))
				{
					delay_ms(1);
				}
				result=Move_to(&c_tem,result);
		 	}
	 	}
	}
	 if(type==1.0)	 //画圆
	 {
	 	length=Draw_round(&tem_point1,&tem_point2,Point_cache);
		if(length<SIZE)
		{
			for(i=0;i<length;i++)
			{
				c_tem.NOW_X=(u16)Point_cache[i].x;
				c_tem.NOW_Y=(u16)Point_cache[i].y;
				c_tem.NOW_Z=0;
				result=Motion_mode(&c_tem); 
				while((TIM1_finish==0)||(TIM2_finish==0))
				{
					delay_ms(1);
				}
				result=Move_to(&c_tem,0);
			}
		}
	 }
	 if(type>1.0)  //画椭圆
	 {
	 	length=Draw_ellipse(&tem_point1,&tem_point2,Point_cache,m_x,m_y);
		if(length<SIZE)
		{
			for(i=0;i<length;i++)
			{
				c_tem.NOW_X=(u16)Point_cache[i].x;
				c_tem.NOW_Y=(u16)Point_cache[i].y;
				c_tem.NOW_Z=0;
				result=Motion_mode(&c_tem);
				while((TIM1_finish==0)||(TIM2_finish==0))
				{
					delay_ms(1);
				}
				result=Move_to(&c_tem,result);
			}
		}
	 }
	 return 0;
}

/*
//原点对准
u16 toOrigin()
{
	u16 dirx=1;
	u16 diry=1;
	u16 tem=0;
	u16 times=0;
//	LASER_Init();
//	LASER_OUT=1;
	struct Coordinate point={0,0,0};
	while(times<20)
	{
		if(finish1>0&&finish2>0)
		{Adjust_speed(60,dirx,1,1);
		times++;}
		if(finish2>0&&finish2>0)
		{Adjust_speed(462,diry,2,2);}
		tem++;
		diry=tem%2;
		tem=tem%100;
		if(LASER_IN==0)
		{
			delay_ms(5);
			if(LASER_IN==0)
		//	TIM2_STOP();
		//	TIM1_STOP();
			break;
		}
	}
	if(times<20)
	return 1;
	else
	return 0;
//	PWM2_CH2_Init();//100HZ	1CM	
} 	   */

