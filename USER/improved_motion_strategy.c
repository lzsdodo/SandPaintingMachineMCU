#include "improved_motion_strategy.h"
/*
改进部分：
每次取10个点进行预测。
速度调节将分为加速曲线，匀速，减速三部分
当前阶段的运动方式将有下个点决定
*/

#define MAX_MULTIPLE 3


//定时器ARR的标准值
const u16 Criterion_ARR[JIASU_STEP]={300,256,238,218,196,173,150,126,103,61,54,61,103,126,150,173,196,218,238,256,300};
//const u16 Criterion_ARR[JIASU_STEP]={500,420,391,358,300,250,200,150,100,70,50,70,100,150,200,250,300,358,391,420,500};
//const u16 Criterion_ARR[JIASU_STEP]={1237,623,317,147,87,60,46,38,33,31,30,31,33,38,46,60,87,147,317,623,1237};
//该队列供直线插补使用
struct Move_parameter QJ_IMS_Mp_queue[MP_LENGTH];		//定义一个参数缓存队列
static struct Coordinate pre_point={0,0,1},now_point={0,0,1},next_point={0,0,0}; 
volatile u16 QJ_IMS_Mp_head;		//队列头
volatile u16 QJ_IMS_Mp_tail;		//队列尾

//Z轴移动的参数
struct Move_parameter QJ_IMS_Mp_TIM4_Z;

static u16 Pre_status=20; //用于记录前一个运动状态,初始值为20

// void Show_TIMARR(struct Move_parameter *mp)
// {
// 	int i=0;
// 	for(i=0;i<JIASU_STEP;i++)
// 	{
// 		printf("mp->TIM_ARR[%d]: %d\n",i,mp->TIM_ARR[i]);
// 		printf("mp->PWM_part[%d]: %d \n",i,mp->PWM_counter_part[i]);
// 	}
// }

/*
	将mp2的值赋给mp1
*/
void Copy_Move_parameter(struct Move_parameter *mp1,struct Move_parameter *mp2)
{
	u16 i=0;
	for(i=0;i<JIASU_STEP;i++)
	{
		mp1->TIM_ARR[i] = mp2->TIM_ARR[i];
		mp1->encoder[i] = mp2->encoder[i];
		mp1->PWM_counter_part[i] = mp2->PWM_counter_part[i];
	}
	mp1->dir_x = mp2->dir_x;
	mp1->dir_y = mp2->dir_y;
	mp1->dir_z = mp2->dir_z;
	mp1->PWM_counter_needed_X = mp2->PWM_counter_needed_X;
	mp1->PWM_counter_needed_Y = mp2->PWM_counter_needed_Y;
	mp1->PWM_counter_needed_Z = mp2->PWM_counter_needed_Z;
	mp1->guaxian = mp2->guaxian;
	//14.07.05加入，获取坐标信息
	mp1->Now_Position_X = mp2->Now_Position_X;
	mp1->Now_Position_Y = mp2->Now_Position_Y;
	mp1->Now_Position_Z = mp2->Now_Position_Z;
}

u16 Mp_isempty(u16 head,u16 tail)
{
	if(head==tail)
	{
		return 1;
	}
	else
	return 0;
}

u16 Mp_isfull(u16 head,u16 tail)
{
	if(((tail+1)%MP_LENGTH)==head)
	{
		return 1;
	}
	else
		return 0;
}

u16 Mp_insert(struct Move_parameter Mp_queue[],struct Move_parameter *mp1,volatile u16 *Mp_head,volatile u16 *Mp_tail)
{
	if(Mp_isfull(*Mp_head,*Mp_tail))
	{
		return 0;
	}
	Copy_Move_parameter( Mp_queue+*Mp_tail,mp1 );
	*Mp_tail = (*Mp_tail+1)%MP_LENGTH;
	return 1;
}

u16 Mp_push(struct Move_parameter Mp_queue[],struct Move_parameter *mp2,u16* Mp_head,u16* Mp_tail)
{
	if(Mp_isempty(*Mp_head,*Mp_tail)>0)
	{
		return 0;
	}
	Copy_Move_parameter(mp2,Mp_queue + *Mp_head);
	*Mp_head=(*Mp_head+1)%MP_LENGTH;
	return 1;
}

u16 Mp_length(u16 head, u16 tail)
{
	u16 length1=0;
	length1=(tail+MP_LENGTH-head)%MP_LENGTH;
	return length1;
}

u16 Get_pre_status()
{
	return Pre_status;//返回前一个运动的状态
}

void Updata_pre_status(u16 new_status)
{
	Pre_status = new_status;
}

//分析各个点的运动状态，当前点坐标，下一个点的坐标，下下一个点的坐标 
//now_point表示当前所在位置,next_point表示接下来要移动到的坐标，point_after_next表示下下一个点
u16 Motion_status_analysis(u16 prestatus,struct Coordinate *now_point,struct Coordinate *next_point,struct Coordinate *point_after_next)
{
	float cos1=0.0;
	struct My_vector mv1={0,0},mv2={0,0};
	mv1.Vector_x = next_point->NOW_X - now_point->NOW_X;
	mv1.Vector_y = next_point->NOW_Y - now_point->NOW_Y;
	mv2.Vector_x = point_after_next->NOW_X - next_point->NOW_X;
	mv2.Vector_y = point_after_next->NOW_Y - next_point->NOW_Y;
	if(point_after_next->NOW_Z != next_point->NOW_Z)
	{
		return JIANSU3;
	}
	cos1=Cos_vector(&mv1,&mv2);
	if(prestatus<=YUNSU)
	{
		if(cos1>=0.8)
		{
			return YUNSU;
		}
		else if(cos1>0&&cos1<0.8)
		{
			return JIANSU1;
		}
		else if(cos1==0)
		{
			return JIANSU2;
		}
		else if(cos1<0)
		{
			return JIANSU3;
		}
	}
	if(prestatus>=JIANSU1)
	{
		if(cos1>=0.8)
		{
			return 20-prestatus;
		}
		if(cos1>0&&cos1<0.8)
		{
			return JIANSU2;
		}
		else
			return JIANSU3;
	}
	return 0;
}


//该函数主要负责给Move_parameter结构体的TIM_ARR赋值
void Assign_TIMARR(struct Move_parameter *mp)
{
	u16 i=0;
	for( i=0; i<JIASU_STEP; i++ )
	{
		mp->TIM_ARR[i] = Criterion_ARR[i];
	}
}

//该函数主要负责为匀速阶段分配PWM信号个数,下一个阶段的运动状态为匀速
u16 YUNSU_allocation_PWM(u16 prestatus,u16 nowstatus,int p_c_n,struct Move_parameter *mp)
{
	u16 i=0;
	int tem=0;
	if(prestatus>=JIASU_STEP||nowstatus>=JIASU_STEP)
	{
		return 0;
	}
	for(i=0;i<JIASU_STEP;i++)
	{
		mp->PWM_counter_part[i]=0;
	}
	if(p_c_n>0)
	{
		tem=p_c_n/15;
		if(tem>200)
		{
			tem=200;
		}
		for(i=1;i<=3;i++)
		{
			mp->PWM_counter_part[YUNSU+i]=tem;
			mp->PWM_counter_part[YUNSU-i]=tem;
		}
		mp->PWM_counter_part[YUNSU]=p_c_n-tem*6;
	}
	return 1;
}

/*
	本函数主要实现加速类型的PWM脉冲个数分配
	输入参数有前一个运动状态pre_status，当前所计算的运动状态now_status，定时器编号 timer,
	PWM脉冲个数p_c_n, 运动参数结构体
*/
u16 JIASU_allocation_PWM(u16 prestatus,u16 nowstatus,int p_c_n,struct Move_parameter *mp)
{
	u16 i=0;
	int tem=0;
	if((prestatus>=JIASU_STEP)||(nowstatus>=JIASU_STEP))
	{
		return 0;
	}
	for(i=0;i<JIASU_STEP;i++)
	{
		mp->PWM_counter_part[i]=0;
	}
	if(p_c_n<=683)
	{
		//如果当前计算阶段运动状态为加速状态则前一个运动状态比为减速状态
		tem=p_c_n/20;
		mp->PWM_counter_part[nowstatus]=30;
		for(i=nowstatus+1;i<YUNSU;i++)
		{
			mp->PWM_counter_part[i]=tem;
		}
		mp->PWM_counter_part[YUNSU]=p_c_n - tem*(YUNSU-nowstatus-1)-30;
	}
	else
	{
		//如果当前计算阶段运动状态为加速状态则前一个运动状态比为减速状态
		tem=p_c_n/30;
		if(tem>200)
		{
			tem=200;
		}
		mp->PWM_counter_part[nowstatus]=10;
		mp->PWM_counter_part[nowstatus+1]=20;
		for(i=nowstatus+2;i<YUNSU;i++)
		{
			mp->PWM_counter_part[i]=tem;
		}
		mp->PWM_counter_part[YUNSU]=p_c_n - tem*(YUNSU-nowstatus-2)-10-20;
	}
	return 1;
}

/*
	减速曲线的PWM脉冲分配
*/
u16 JIANSU_allocation_PWM(u16 prestatus,u16 nowstatus,int p_c_n,struct Move_parameter *mp)
{
	u16 i=0;
	int tem=0;
	if((prestatus>=JIASU_STEP)||(nowstatus>=JIASU_STEP)||(nowstatus<=YUNSU))
	{
		return 0;
	}
	for(i=0;i<JIASU_STEP;i++)
	{
		mp->PWM_counter_part[i]=0;
	}
	if(p_c_n<683)
	{
		if(prestatus<=YUNSU)//前一阶段为匀速运动状 或加速运动状态 不过最终结果都一样
		{
			tem=p_c_n/11;
			for(i=YUNSU+1;i<=nowstatus;i++)
			{
				mp->PWM_counter_part[i]=tem;
			}
	//		mp->PWM_counter_part[nowstatus]=10;
			mp->PWM_counter_part[YUNSU]=p_c_n-tem*(nowstatus-YUNSU);
		}
		else  //前一个阶段为减速阶段，最终速度小于匀速速度，需要先加速后减速
		{
			tem=p_c_n/20;
			for(i=JIASU_STEP-prestatus;i<YUNSU;i++)
			{
				mp->PWM_counter_part[i]=tem;
			}
			for(i=YUNSU+1;i<=nowstatus;i++)
			{
				mp->PWM_counter_part[i]=tem;
			}
			//mp->PWM_counter_part[JIASU_STEP-prestatus-1]=10;
			//mp->PWM_counter_part[nowstatus]=10;
			
			//10*2代表mp->PWM_counter_part[nowstatus]和mp->PWM_counter_part[JIASU_STEP-prestatus-1]
			mp->PWM_counter_part[YUNSU]=p_c_n - tem*(nowstatus-(JIASU_STEP-prestatus-1));
		}
	}
	else
	{
		if( prestatus <= YUNSU )//前一阶段为匀速运动状 或加速运动状态 不过最终结果都一样
		{
			tem = p_c_n / 20;
			if( tem > 200 )
			{
				tem = 200;
			}
			for( i=YUNSU+1; i<nowstatus; i++ )
			{
				mp->PWM_counter_part[i] = tem;
			}
			mp->PWM_counter_part[nowstatus-1] = 20;
			mp->PWM_counter_part[nowstatus]   = 20;
			mp->PWM_counter_part[YUNSU] = p_c_n-tem*(nowstatus-YUNSU-2) - 20 * 2 ;
		}
		else  //前一个阶段为减速阶段，最终速度小于匀速速度，需要先加速后减速
		{
			tem = p_c_n / 30;
			if( tem > 200 )
			{
				tem = 200;
			}
			for( i=JIASU_STEP-prestatus; i<YUNSU; i++ )
			{
				mp->PWM_counter_part[i] = tem;
			}
			for( i=YUNSU+1; i<nowstatus; i++ )
			{
				mp->PWM_counter_part[i] = tem;
			}
			mp->PWM_counter_part[JIASU_STEP-prestatus-1] = 30;
			mp->PWM_counter_part[nowstatus] = 30;
			
			//10*2代表mp->PWM_counter_part[nowstatus]和mp->PWM_counter_part[JIASU_STEP-prestatus-1]
			mp->PWM_counter_part[YUNSU] = p_c_n - tem * ( nowstatus - ( JIASU_STEP-prestatus - 1 ) - 2 ) - 30 * 2;
		}
	}
	return 1;
}

/*
改进的加速曲线分配函数
主要负责完成 运动方向 对PWM脉冲的分配 和 频率的设定
*/
void Improved_acceleration_curves(u16 pre_status,u16 now_status,int p_c_n,struct Move_parameter *mp)
{
	Assign_TIMARR(mp);
	if(p_c_n>0)
	{
		if(now_status<JIASU_STEP)//mp的运动状态值合法
		{
			if(now_status<YUNSU)
			{
				JIASU_allocation_PWM(pre_status,now_status,p_c_n,mp);
			}
		 if(now_status==YUNSU)//匀速运动
			{
				YUNSU_allocation_PWM(pre_status,now_status,p_c_n,mp);
			}
			if(now_status>YUNSU)
			{
				JIANSU_allocation_PWM(pre_status,now_status,p_c_n,mp);
			}
		}		
	}
}

//沙漏控制函数，control等于0则打开，control大于0则关闭
void Shalou(u16 control)
{
	if(control==0)
	{
		DIR_shalou=1;//开
		//TIM_control(2,1); //关闭定时器2,停止流沙
	}
	else
	{
		DIR_shalou=0;//关
		//TIM_control(2,0); //打开定时器2，开始流沙
	}
	TIM2_Int_Init(1000,71);//开启定时器2
}

/*
	该函数主要负责完成对预测点的PWM脉冲分配和频率设定， 既完成对Move_parameter各项参数的赋值
	该函数中将新点的Move_parameter插入队列中
	使用全局变量MP_queue
*/
u16 Forecast_analysis(u16 forecast_num)//struct Coordinate *C_point)
{
	u16 i = 0;
	u16 result = 0;
	u16 prestatus = 0; 
	u16 nowstatus = 0;
	
	//当前所在点和接下来要去的点的X、Y轴坐标之差的绝对值
	u16 X_difference = 0;
	u16 Y_difference = 0;
	struct Move_parameter mp_timer;//mp_timer
	struct PWM_and_DIR p_a_d;
	
	/*
		pre_point表示前一个点(即当前所在位置)，now_point表示当前要计算的点 当前要去的点，next_point表示下一个要计算的点
		Transition_point为过渡点，用于当单次移动超过最大限制时的过渡
	*/
	struct Coordinate Transition_point;
	
	for( i=0; i<forecast_num; i++ )
	{
		LED0 = 0;
		if( Mp_isfull(QJ_IMS_Mp_head,QJ_IMS_Mp_tail) ) //如果队列满
		{
			Send_result(QUEUE_FULL,0);
			return i;
		}
		result = Cq_push(&next_point);
		if(result==0)//未能成功取出一个点的坐标
		{
			return 0;
		}
		//两个点之间X轴坐标差值的绝对值
		if(pre_point.NOW_X > now_point.NOW_X)
		{
			X_difference = pre_point.NOW_X - now_point.NOW_X;
		}
		else
		{
			X_difference = now_point.NOW_X - pre_point.NOW_X;
		}
		
		//两个点之间Y轴坐标差值的绝对值
		if(pre_point.NOW_Y > now_point.NOW_Y)
		{
			Y_difference = pre_point.NOW_Y - now_point.NOW_Y;
		}
		else
		{
			Y_difference = now_point.NOW_Y - pre_point.NOW_Y;
		}
		//确定卦限
		if( X_difference > Y_difference )
		{
			mp_timer.guaxian = 1;
		}
		else
		{
			mp_timer.guaxian = 2;
		}
		
		//为了防止数据过大，产生溢出，设定X、Y单次移动最大距离不超过170个单位
		//点两个点之间的距离超过最大限制
		if((X_difference>170)||(Y_difference>170))
		{
			//移动到Transition_point 
			Transition_point.NOW_X = (pre_point.NOW_X+now_point.NOW_X)/2;
			Transition_point.NOW_Y = (pre_point.NOW_Y+now_point.NOW_Y)/2;
			Transition_point.NOW_Z = now_point.NOW_Z;
			prestatus = Get_pre_status();
			nowstatus = Motion_status_analysis(prestatus,&pre_point,&Transition_point,&now_point);
			result = XYZ_to_pwm(pre_point.NOW_X,pre_point.NOW_Y,pre_point.NOW_Z,Transition_point.NOW_X,Transition_point.NOW_Y,Transition_point.NOW_Z,&p_a_d);
			if(p_a_d.PWM_z>0)
			{
				QJ_IMS_Mp_TIM4_Z.PWM_counter_needed_Z = p_a_d.PWM_z;
				QJ_IMS_Mp_TIM4_Z.dir_z = p_a_d.dir_z;
				Improved_acceleration_curves(JIANSU3,JIANSU3,p_a_d.PWM_z,&QJ_IMS_Mp_TIM4_Z);
			//	Send_result(QUEUE_FULL,0);
				while( !Mp_isempty(QJ_IMS_Mp_head,QJ_IMS_Mp_tail) )
				{
					delay_ms(1);
					LED1=0;
				}
				while( !QJ_TIM_finish_xy )//等待XY轴执行完
				{
					delay_ms(1);
					LED1=0;
				}
				Shalou( now_point.NOW_Z );//控制沙漏
				//PWM4_CH2_Init(1500,71);//Z轴开始移动
				while( QJ_TIM_TIM2_finish_shalou==0 )//等待Z轴移动完成
				{
					delay_ms(1);
					LED1 = 0;
				}
				LED1 = 1;
			}
			mp_timer.PWM_counter_needed_X = p_a_d.PWM_x;
			mp_timer.PWM_counter_needed_Y = p_a_d.PWM_y;
			mp_timer.dir_x = p_a_d.dir_x;
			mp_timer.dir_y = p_a_d.dir_y;
			if( mp_timer.guaxian==1 )
			{
				Improved_acceleration_curves(prestatus,nowstatus,p_a_d.PWM_x,&mp_timer);
			}
			else
			{
				Improved_acceleration_curves(prestatus,nowstatus,p_a_d.PWM_y,&mp_timer);
			}
			Mp_insert(QJ_IMS_Mp_queue,&mp_timer,&QJ_IMS_Mp_head,&QJ_IMS_Mp_tail);
			
			//运动到now_point
		  prestatus = nowstatus;
			nowstatus = Motion_status_analysis(prestatus,&Transition_point,&now_point,&next_point);
			result = XYZ_to_pwm(Transition_point.NOW_X,Transition_point.NOW_Y,Transition_point.NOW_Z,now_point.NOW_X,now_point.NOW_Y,now_point.NOW_Z,&p_a_d);
			
			//此时Z轴不会再运动
			mp_timer.dir_x = p_a_d.dir_x;
			mp_timer.dir_y = p_a_d.dir_y;
			mp_timer.dir_z = p_a_d.dir_z;
			mp_timer.PWM_counter_needed_X = p_a_d.PWM_x;
			mp_timer.PWM_counter_needed_Y = p_a_d.PWM_y;
			mp_timer.PWM_counter_needed_Z = p_a_d.PWM_z;
			//14.07.05修改	加入坐标信息
			mp_timer.Now_Position_X = now_point.NOW_X;
			mp_timer.Now_Position_Y = now_point.NOW_Y;
			mp_timer.Now_Position_Z = now_point.NOW_Z;
			
			if( mp_timer.guaxian==1 )
			{
				Improved_acceleration_curves(prestatus,nowstatus,p_a_d.PWM_x,&mp_timer);
			}
			else
			{
				Improved_acceleration_curves(prestatus,nowstatus,p_a_d.PWM_y,&mp_timer);
			}
			Mp_insert(QJ_IMS_Mp_queue,&mp_timer,&QJ_IMS_Mp_head,&QJ_IMS_Mp_tail);

			Updata_pre_status(nowstatus);//更新当前运动状态
		}
		else
		{
			prestatus = Get_pre_status();
			nowstatus = Motion_status_analysis(prestatus,&pre_point,&now_point,&next_point);
			result = XYZ_to_pwm(pre_point.NOW_X,pre_point.NOW_Y,pre_point.NOW_Z,now_point.NOW_X,now_point.NOW_Y,now_point.NOW_Z,&p_a_d);
			mp_timer.dir_x = p_a_d.dir_x;
			mp_timer.dir_y = p_a_d.dir_y;
			mp_timer.PWM_counter_needed_Y = p_a_d.PWM_y;
			mp_timer.PWM_counter_needed_X = p_a_d.PWM_x;
			//14.07.05修改	加入坐标信息
			mp_timer.Now_Position_X = now_point.NOW_X;
			mp_timer.Now_Position_Y = now_point.NOW_Y;
			mp_timer.Now_Position_Z = now_point.NOW_Z;
			
			if( p_a_d.PWM_z>0 )
			{
				QJ_IMS_Mp_TIM4_Z.PWM_counter_needed_Z = p_a_d.PWM_z;
				QJ_IMS_Mp_TIM4_Z.dir_z = p_a_d.dir_z;
				Improved_acceleration_curves(JIANSU3,JIANSU3,p_a_d.PWM_z,&QJ_IMS_Mp_TIM4_Z);
			//	Send_result(QUEUE_FULL,0);
				while( !Mp_isempty(QJ_IMS_Mp_head,QJ_IMS_Mp_tail) )
				{
					delay_ms(1);
					LED1 = 0;
				}
				while( !QJ_TIM_finish_xy )
				{
					delay_ms(1);
					LED1 = 0;
				}
				Shalou(now_point.NOW_Z);//控制沙漏
			//	PWM4_CH2_Init(1500,59);//Z轴开始移动
				while( QJ_TIM_TIM2_finish_shalou==0 )//等待沙漏打开或关闭
				{
					delay_ms(1);
					LED1 = 0;
				}
				LED1 = 1;
		//	Send_result(CONTINUE_ACCEPT,0);
			}
			
			if( mp_timer.guaxian==1 )
			{
				Improved_acceleration_curves(prestatus,nowstatus,p_a_d.PWM_x,&mp_timer);
			}
			else
			{
				Improved_acceleration_curves(prestatus,nowstatus,p_a_d.PWM_y,&mp_timer);
			}
			Mp_insert(QJ_IMS_Mp_queue,&mp_timer,&QJ_IMS_Mp_head,&QJ_IMS_Mp_tail);
			
			Updata_pre_status(nowstatus);//更新当前运动状态
			}
			Copy_Coordinate(&pre_point,&now_point);  //pre_point=now_point
			Copy_Coordinate(&now_point,&next_point); //now_point=nexy_point
	}
	return 1;
}

