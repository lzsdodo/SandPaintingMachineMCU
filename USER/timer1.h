#ifndef _TIMER1_H
#define _TIMER1_H
#include "sys.h" 

//ͨ���ı�TIM3->CCR2��ֵ���ı�ռ�ձȣ��Ӷ����������ռ�ձ�
#define TIM2_CH2_PWM_VAL TIM2->CCR2 
#define TIM1_CH1_PWM_VAL TIM1->CCR1
#define TIM3_CH2_PWM_VAL TIM3->CCR2


//extern volatile u16 test_TIM5_times;
extern volatile u16 QJ_TIM_finish_xy;

extern volatile u16 QJ_TIM_TIM4_finish_Z;//Z���ƶ���ɱ�־
extern volatile u16 QJ_TIM_TIM2_finish_shalou;//ɳ©���ƿ�����ɱ�־
extern volatile u16 DIR_shalou;
extern float QJ_TIM_PWM_to_Encoder;//QJ_TIM_PWM_to_Encoder=������ֵ ����Ӧ��PWM�������



//��ʱ��2��ͨ��2��ʼ�� �������Y��PWM�����ź�
//void PWM2_CH2_Init(u16 arr,u16 psc);  

//��ʱ��1��ͨ��1��ʼ��  �������X���PWM�ź�
//void PWM1_CH1_Init(u16 arr,u16 psc); 

//��ʱ��3��ʼ�� 
void PWM3_CH2_Init(u16 arr,u16 psc); 

//X�����������ȡX���������ֵ
//void TIM3_CNT_Init(u16 arr,u16 psc); 

//��ʱ��5��ʼ������ʱ��5����Ϊ��ʱ��1�Ͷ�ʱ��2�������PWM�ź�����Ĳ���
void TIM5_Int_Init(u16 arr,u16 psc);

//��ʱ��2��ʼ������ʱ��2�������ɳ©
void TIM2_Int_Init(u16 arr,u16 psc);

//���ض�ʱ�� timerΪ��ʱ�����1����8 , control Ϊ�����źţ�control==0��ʾ�أ�control>0��ʾ��
void TIM_control(u16 timer,u16 control);

#endif
