#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h" 

extern int PWM_counter_part1[21];//��ʱ��1��21���׶����������PWM�������
extern int PWM_counter_part2[21];
extern int PWM_counter_part4[21];

extern u16 TIM1_finish;//  �����ж�PWM��������Ƿ������0
extern u16 TIM2_finish;
extern u16 TIM4_finish;
extern u8 Original_Dir1;
extern u8 Original_Dir2;//��¼һ���ƶ����������
extern float encoder1[21];

extern int PWM_counter2;
extern int PWM_counter1; //��¼�Ѿ������PWM����
extern int PWM_counter4;
extern float PWM_to_Encoder;
extern u16 ARR1[21];//��ʱ��1��ͬ�׶�ARR�Ĵ�����ֵ
extern u16 ARR2[21];//��ʱ��2��ͬ�׶�ARR�Ĵ�����ֵ
extern u16 ARR4[21];
//ͨ���ı�TIM3->CCR2��ֵ���ı�ռ�ձȣ��Ӷ����������ռ�ձ�
#define TIM2_CH2_PWM_VAL TIM2->CCR2 
#define TIM1_CH1_PWM_VAL TIM1->CCR1
#define TIM4_CH2_PWM_VAL TIM4->CCR2
void PWM2_CH2_Init(u16 arr,u16 psc,u16 p_c_n);  //��ʱ��2��ͨ��2��ʼ��
void PWM1_CH1_Init(u16 arr,u16 psc,u16 p_c_n); //��ʱ��1��ͨ��1��ʼ��
void PWM4_CH2_Init(u16 arr,u16 psc,u16 p_c_n);
void TIM3_CNT_Init(u16 arr,u16 psc);
#endif
