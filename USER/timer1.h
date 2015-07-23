#ifndef _TIMER1_H
#define _TIMER1_H
#include "sys.h" 

//通过改变TIM3->CCR2的值来改变占空比，从而控制输出的占空比
#define TIM2_CH2_PWM_VAL TIM2->CCR2 
#define TIM1_CH1_PWM_VAL TIM1->CCR1
#define TIM3_CH2_PWM_VAL TIM3->CCR2


//extern volatile u16 test_TIM5_times;
extern volatile u16 QJ_TIM_finish_xy;

extern volatile u16 QJ_TIM_TIM4_finish_Z;//Z轴移动完成标志
extern volatile u16 QJ_TIM_TIM2_finish_shalou;//沙漏控制开关完成标志
extern volatile u16 DIR_shalou;
extern float QJ_TIM_PWM_to_Encoder;//QJ_TIM_PWM_to_Encoder=编码器值 ：对应的PWM脉冲个数



//定时器2的通道2初始化 负责输出Y轴PWM脉冲信号
//void PWM2_CH2_Init(u16 arr,u16 psc);  

//定时器1的通道1初始化  负责输出X轴的PWM信号
//void PWM1_CH1_Init(u16 arr,u16 psc); 

//定时器3初始化 
void PWM3_CH2_Init(u16 arr,u16 psc); 

//X轴编码器，读取X轴编码器的值
//void TIM3_CNT_Init(u16 arr,u16 psc); 

//定时器5初始化，定时器5用于为定时器1和定时器2更新输出PWM信号所需的参数
void TIM5_Int_Init(u16 arr,u16 psc);

//定时器2初始化，定时器2负责控制沙漏
void TIM2_Int_Init(u16 arr,u16 psc);

//开关定时器 timer为定时器编号1——8 , control 为控制信号，control==0表示关，control>0表示开
void TIM_control(u16 timer,u16 control);

#endif
