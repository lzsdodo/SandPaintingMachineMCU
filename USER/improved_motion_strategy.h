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

#define FORECAST_NUM 10  		//һ��Ԥ���ĸ���

#define JIASU1  2 	//��������1,��ARR[2] 2666HZ��ʼ
#define JIASU2  1 	//��������2,��ARR[1] 1333HZ��ʼ
#define JIASU3  0 	//��������3, ��0��ʼ
#define JIANSU1 18 	//��������1,����ARR[18]
#define JIANSU2 19 	//��������2������ARR[19]
#define JIANSU3 20	//��������3������ARR[20]
#define YUNSU   10	//����

#define MP_LENGTH	201	//����������г���
#define AXLE_X 1 			//x��
#define AXLE_Y 2 			//Y��
#define AXLE_Z 3 			//Z��
#define TIM_4  4			//��ʱ��4
#define JIASU_STEP 21 //�������߲���

/*
	�ýṹ�����ڴ洢��ʱ����ARR�Ĵ���ֵ������ʱ����Ƶ�ʡ�
	�ƶ�����һ���㶨ʱ������������������,��֮�����ƶ�����һ��������Ĳ�����
	����������Ϣ
*/
struct Move_parameter{
	u16 TIM_ARR[JIASU_STEP];	//��ʱ��1��ͬ�׶�ARR�Ĵ�����ֵ ���Ƶ���Զ�ʱ��1Ϊ׼
	int PWM_counter_needed_X;	//X����Ҫ�����PWM�������
	int PWM_counter_needed_Y;	//Y����Ҫ�����PWM����
	int PWM_counter_needed_Z;	//Z����Ҫ�����PWM����
	u16 dir_x;								//X�᷽��
	u16 dir_y;								//Y�᷽��
	u16 dir_z;								//Z�᷽��
	u16 guaxian;							//ȷ��������һ������
	int PWM_counter_part[JIASU_STEP];//X����׶�Ҫ�����PWM�������
	float encoder[JIASU_STEP];//���׶α�������ֵ,������X��������Ƶ��
	//�������14.07.05���ӵı���
	u16 Now_Position_X;
	u16 Now_Position_Y;
	u16 Now_Position_Z;
};

//�ö��й�ֱ�߲岹ʹ��
extern struct Move_parameter QJ_IMS_Mp_queue[MP_LENGTH];//����һ�������������
extern volatile u16 QJ_IMS_Mp_head;//����ͷ
extern volatile u16 QJ_IMS_Mp_tail;//����β

//Z���ƶ��Ĳ���
extern struct Move_parameter QJ_IMS_Mp_TIM4_Z;

u16 Mp_isempty(u16 head,u16 tail);			//�����Ƿ�Ϊ��
u16 Mp_isfull(u16 head,u16 tail);				//�����Ƿ�Ϊ��
u16 Mp_insert(struct Move_parameter Mp_queue[],struct Move_parameter *mp1,volatile u16 *Mp_head,volatile u16 *Mp_tail);		//����һ����
u16 Mp_push(struct Move_parameter Mp_queue[],struct Move_parameter *mp2,u16* Mp_head,u16* Mp_tail);												//����һ����
u16 Mp_length(u16 head,u16 tail);				//���ض��г���
void Copy_Move_parameter(struct Move_parameter *mp1,struct Move_parameter *mp2);

u16 Forecast_analysis(u16 forecast_num);//ȡǰN����Ԥ�����

u16 Continue_motion(struct Coordinate *c_point,u16 point_num);	//���ݷ���Ԥ��õ���һ�����ݸ����˶�����

//����һ�����˶�״̬Ԥ��
u16 Motion_status_analysis(u16 prestatus,struct Coordinate *now_point,struct Coordinate *next_point,struct Coordinate *point_after_next);

//�ú�����Ҫ�����Move_parameter�ṹ���TIM_ARR��ֵ
void Assign_TIMARR(struct Move_parameter *mp);

//�ú�����Ҫ����Ϊ���ٽ׶η���PWM�źŸ���,��һ���׶ε��˶�״̬Ϊ����
u16 YUNSU_allocation_PWM(u16 prestatus,u16 nowstatus,int p_c_n,struct Move_parameter *mp);

//�ú�������ʵ�ּ������߹��ܣ�����������/����̨�׵�PWM�������
u16 JIASU_allocation_PWM(u16 prestatus,u16 nowstatus,int p_c_n,struct Move_parameter *mp);

u16 JIANSU_allocation_PWM(u16 prestatus,u16 nowstatus,int p_c_n,struct Move_parameter *mp);

void Improved_acceleration_curves(u16 pre_status,u16 now_status,int p_c_n,struct Move_parameter *mp);

#endif
