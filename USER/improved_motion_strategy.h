#ifndef _IMPROVED_MOTION_STRATEGY_H
#define _IMPROVED_MOTION_STRATEGY_H
#include "sys.h"
#include "accept_coordinate.h"
#include "timer1.h"
#include "coordinate_PWM.h"
#include "draw.h"
#include "delay.h"
#include "gpio.h"
#include "usart_communication.h"
#include "cos.h"

#define FORECAST_NUM 10  		//一次预测点的个数

#define JIASU1  2 	//加速类型1,从ARR[2] 2666HZ开始
#define JIASU2  1 	//加速类型2,从ARR[1] 1333HZ开始
#define JIASU3  0 	//加速类型3, 从0开始
#define JIANSU1 18 	//减速类型1,减至ARR[18]
#define JIANSU2 19 	//减速类型2，减至ARR[19]
#define JIANSU3 20	//减速类型3，减至ARR[20]
#define YUNSU   10	//匀速

#define MP_LENGTH	201	//输出参数队列长度
#define AXLE_X 1 			//x轴
#define AXLE_Y 2 			//Y轴
#define AXLE_Z 3 			//Z轴
#define TIM_4  4			//定时器4
#define JIASU_STEP 21 //加速曲线步数

/*
	该结构体用于存储定时器的ARR寄存器值，即定时器的频率。
	移动到下一个点定时器所需输出的脉冲个数,总之就是移动到下一个点所需的参数。
	加入坐标信息
*/
struct Move_parameter{
	u16 TIM_ARR[JIASU_STEP];	//定时器1不同阶段ARR寄存器的值 输出频率以定时器1为准
	int PWM_counter_needed_X;	//X轴需要输出的PWM脉冲个数
	int PWM_counter_needed_Y;	//Y轴需要输出的PWM个数
	int PWM_counter_needed_Z;	//Z轴需要输出的PWM个数
	u16 dir_x;								//X轴方向
	u16 dir_y;								//Y轴方向
	u16 dir_z;								//Z轴方向
	u16 guaxian;							//确定是在哪一个卦限
	int PWM_counter_part[JIASU_STEP];//X轴各阶段要输出的PWM脉冲个数
	float encoder[JIASU_STEP];//各阶段编码器的值,将根据X轴来调节频率
	//下面参数14.07.05增加的变量
	u16 Now_Position_X;
	u16 Now_Position_Y;
	u16 Now_Position_Z;
};

//该队列供直线插补使用
extern struct Move_parameter QJ_IMS_Mp_queue[MP_LENGTH];//定义一个参数缓存队列
extern volatile u16 QJ_IMS_Mp_head;//队列头
extern volatile u16 QJ_IMS_Mp_tail;//队列尾

//Z轴移动的参数
extern struct Move_parameter QJ_IMS_Mp_TIM4_Z;

u16 Mp_isempty(u16 head,u16 tail);			//队列是否为空
u16 Mp_isfull(u16 head,u16 tail);				//队列是否为满
u16 Mp_insert(struct Move_parameter Mp_queue[],struct Move_parameter *mp1,volatile u16 *Mp_head,volatile u16 *Mp_tail);		//插入一个点
u16 Mp_push(struct Move_parameter Mp_queue[],struct Move_parameter *mp2,u16* Mp_head,u16* Mp_tail);												//弹出一个点
u16 Mp_length(u16 head,u16 tail);				//返回队列长度
void Copy_Move_parameter(struct Move_parameter *mp1,struct Move_parameter *mp2);

u16 Forecast_analysis(u16 forecast_num);//取前N个点预测分析

u16 Continue_motion(struct Coordinate *c_point,u16 point_num);	//根据分析预测得到的一组数据更新运动参数

//到下一个点运动状态预测
u16 Motion_status_analysis(u16 prestatus,struct Coordinate *now_point,struct Coordinate *next_point,struct Coordinate *point_after_next);

//该函数主要负责给Move_parameter结构体的TIM_ARR赋值
void Assign_TIMARR(struct Move_parameter *mp);

//该函数主要负责为匀速阶段分配PWM信号个数,下一个阶段的运动状态为匀速
u16 YUNSU_allocation_PWM(u16 prestatus,u16 nowstatus,int p_c_n,struct Move_parameter *mp);

//该函数用于实现加速曲线功能，负责分配各加/减速台阶的PWM脉冲个数
u16 JIASU_allocation_PWM(u16 prestatus,u16 nowstatus,int p_c_n,struct Move_parameter *mp);

u16 JIANSU_allocation_PWM(u16 prestatus,u16 nowstatus,int p_c_n,struct Move_parameter *mp);

void Improved_acceleration_curves(u16 pre_status,u16 now_status,int p_c_n,struct Move_parameter *mp);

#endif
