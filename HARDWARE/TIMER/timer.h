#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h" 

extern int PWM_counter_part1[21];//计时器1的21个阶段所需输出的PWM脉冲个数
extern int PWM_counter_part2[21];
extern int PWM_counter_part4[21];

extern u16 TIM1_finish;//  用于判断PWM脉冲输出是否已完成0
extern u16 TIM2_finish;
extern u16 TIM4_finish;
extern u8 Original_Dir1;
extern u8 Original_Dir2;//记录一次移动的最初方向
extern float encoder1[21];

extern int PWM_counter2;
extern int PWM_counter1; //记录已经输出的PWM个数
extern int PWM_counter4;
extern float PWM_to_Encoder;
extern u16 ARR1[21];//定时器1不同阶段ARR寄存器的值
extern u16 ARR2[21];//定时器2不同阶段ARR寄存器的值
extern u16 ARR4[21];
//通过改变TIM3->CCR2的值来改变占空比，从而控制输出的占空比
#define TIM2_CH2_PWM_VAL TIM2->CCR2 
#define TIM1_CH1_PWM_VAL TIM1->CCR1
#define TIM4_CH2_PWM_VAL TIM4->CCR2
void PWM2_CH2_Init(u16 arr,u16 psc,u16 p_c_n);  //定时器2的通道2初始化
void PWM1_CH1_Init(u16 arr,u16 psc,u16 p_c_n); //定时器1的通道1初始化
void PWM4_CH2_Init(u16 arr,u16 psc,u16 p_c_n);
void TIM3_CNT_Init(u16 arr,u16 psc);
#endif
