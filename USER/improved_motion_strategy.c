#include "improved_motion_strategy.h"
/*
�Ľ����֣�
ÿ��ȡ10�������Ԥ�⡣
�ٶȵ��ڽ���Ϊ�������ߣ����٣�����������
��ǰ�׶ε��˶���ʽ�����¸������
*/

#define MAX_MULTIPLE 3


//��ʱ��ARR�ı�׼ֵ
const u16 Criterion_ARR[JIASU_STEP]={300,256,238,218,196,173,150,126,103,61,54,61,103,126,150,173,196,218,238,256,300};
//const u16 Criterion_ARR[JIASU_STEP]={500,420,391,358,300,250,200,150,100,70,50,70,100,150,200,250,300,358,391,420,500};
//const u16 Criterion_ARR[JIASU_STEP]={1237,623,317,147,87,60,46,38,33,31,30,31,33,38,46,60,87,147,317,623,1237};
//�ö��й�ֱ�߲岹ʹ��
struct Move_parameter QJ_IMS_Mp_queue[MP_LENGTH];		//����һ�������������
static struct Coordinate pre_point={0,0,1},now_point={0,0,1},next_point={0,0,0}; 
volatile u16 QJ_IMS_Mp_head;		//����ͷ
volatile u16 QJ_IMS_Mp_tail;		//����β

//Z���ƶ��Ĳ���
struct Move_parameter QJ_IMS_Mp_TIM4_Z;

static u16 Pre_status=20; //���ڼ�¼ǰһ���˶�״̬,��ʼֵΪ20

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
	��mp2��ֵ����mp1
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
	//14.07.05���룬��ȡ������Ϣ
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
	return Pre_status;//����ǰһ���˶���״̬
}

void Updata_pre_status(u16 new_status)
{
	Pre_status = new_status;
}

//������������˶�״̬����ǰ�����꣬��һ��������꣬����һ��������� 
//now_point��ʾ��ǰ����λ��,next_point��ʾ������Ҫ�ƶ��������꣬point_after_next��ʾ����һ����
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


//�ú�����Ҫ�����Move_parameter�ṹ���TIM_ARR��ֵ
void Assign_TIMARR(struct Move_parameter *mp)
{
	u16 i=0;
	for( i=0; i<JIASU_STEP; i++ )
	{
		mp->TIM_ARR[i] = Criterion_ARR[i];
	}
}

//�ú�����Ҫ����Ϊ���ٽ׶η���PWM�źŸ���,��һ���׶ε��˶�״̬Ϊ����
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
	��������Ҫʵ�ּ������͵�PWM�����������
	���������ǰһ���˶�״̬pre_status����ǰ��������˶�״̬now_status����ʱ����� timer,
	PWM�������p_c_n, �˶������ṹ��
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
		//�����ǰ����׶��˶�״̬Ϊ����״̬��ǰһ���˶�״̬��Ϊ����״̬
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
		//�����ǰ����׶��˶�״̬Ϊ����״̬��ǰһ���˶�״̬��Ϊ����״̬
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
	�������ߵ�PWM�������
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
		if(prestatus<=YUNSU)//ǰһ�׶�Ϊ�����˶�״ ������˶�״̬ �������ս����һ��
		{
			tem=p_c_n/11;
			for(i=YUNSU+1;i<=nowstatus;i++)
			{
				mp->PWM_counter_part[i]=tem;
			}
	//		mp->PWM_counter_part[nowstatus]=10;
			mp->PWM_counter_part[YUNSU]=p_c_n-tem*(nowstatus-YUNSU);
		}
		else  //ǰһ���׶�Ϊ���ٽ׶Σ������ٶ�С�������ٶȣ���Ҫ�ȼ��ٺ����
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
			
			//10*2����mp->PWM_counter_part[nowstatus]��mp->PWM_counter_part[JIASU_STEP-prestatus-1]
			mp->PWM_counter_part[YUNSU]=p_c_n - tem*(nowstatus-(JIASU_STEP-prestatus-1));
		}
	}
	else
	{
		if( prestatus <= YUNSU )//ǰһ�׶�Ϊ�����˶�״ ������˶�״̬ �������ս����һ��
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
		else  //ǰһ���׶�Ϊ���ٽ׶Σ������ٶ�С�������ٶȣ���Ҫ�ȼ��ٺ����
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
			
			//10*2����mp->PWM_counter_part[nowstatus]��mp->PWM_counter_part[JIASU_STEP-prestatus-1]
			mp->PWM_counter_part[YUNSU] = p_c_n - tem * ( nowstatus - ( JIASU_STEP-prestatus - 1 ) - 2 ) - 30 * 2;
		}
	}
	return 1;
}

/*
�Ľ��ļ������߷��亯��
��Ҫ������� �˶����� ��PWM����ķ��� �� Ƶ�ʵ��趨
*/
void Improved_acceleration_curves(u16 pre_status,u16 now_status,int p_c_n,struct Move_parameter *mp)
{
	Assign_TIMARR(mp);
	if(p_c_n>0)
	{
		if(now_status<JIASU_STEP)//mp���˶�״ֵ̬�Ϸ�
		{
			if(now_status<YUNSU)
			{
				JIASU_allocation_PWM(pre_status,now_status,p_c_n,mp);
			}
		 if(now_status==YUNSU)//�����˶�
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

//ɳ©���ƺ�����control����0��򿪣�control����0��ر�
void Shalou(u16 control)
{
	if(control==0)
	{
		DIR_shalou=1;//��
		//TIM_control(2,1); //�رն�ʱ��2,ֹͣ��ɳ
	}
	else
	{
		DIR_shalou=0;//��
		//TIM_control(2,0); //�򿪶�ʱ��2����ʼ��ɳ
	}
	TIM2_Int_Init(1000,71);//������ʱ��2
}

/*
	�ú�����Ҫ������ɶ�Ԥ����PWM��������Ƶ���趨�� ����ɶ�Move_parameter��������ĸ�ֵ
	�ú����н��µ��Move_parameter���������
	ʹ��ȫ�ֱ���MP_queue
*/
u16 Forecast_analysis(u16 forecast_num)//struct Coordinate *C_point)
{
	u16 i = 0;
	u16 result = 0;
	u16 prestatus = 0; 
	u16 nowstatus = 0;
	
	//��ǰ���ڵ�ͽ�����Ҫȥ�ĵ��X��Y������֮��ľ���ֵ
	u16 X_difference = 0;
	u16 Y_difference = 0;
	struct Move_parameter mp_timer;//mp_timer
	struct PWM_and_DIR p_a_d;
	
	/*
		pre_point��ʾǰһ����(����ǰ����λ��)��now_point��ʾ��ǰҪ����ĵ� ��ǰҪȥ�ĵ㣬next_point��ʾ��һ��Ҫ����ĵ�
		Transition_pointΪ���ɵ㣬���ڵ������ƶ������������ʱ�Ĺ���
	*/
	struct Coordinate Transition_point;
	
	for( i=0; i<forecast_num; i++ )
	{
		LED0 = 0;
		if( Mp_isfull(QJ_IMS_Mp_head,QJ_IMS_Mp_tail) ) //���������
		{
			Send_result(QUEUE_FULL,0);
			return i;
		}
		result = Cq_push(&next_point);
		if(result==0)//δ�ܳɹ�ȡ��һ���������
		{
			return 0;
		}
		//������֮��X�������ֵ�ľ���ֵ
		if(pre_point.NOW_X > now_point.NOW_X)
		{
			X_difference = pre_point.NOW_X - now_point.NOW_X;
		}
		else
		{
			X_difference = now_point.NOW_X - pre_point.NOW_X;
		}
		
		//������֮��Y�������ֵ�ľ���ֵ
		if(pre_point.NOW_Y > now_point.NOW_Y)
		{
			Y_difference = pre_point.NOW_Y - now_point.NOW_Y;
		}
		else
		{
			Y_difference = now_point.NOW_Y - pre_point.NOW_Y;
		}
		//ȷ������
		if( X_difference > Y_difference )
		{
			mp_timer.guaxian = 1;
		}
		else
		{
			mp_timer.guaxian = 2;
		}
		
		//Ϊ�˷�ֹ���ݹ��󣬲���������趨X��Y�����ƶ������벻����170����λ
		//��������֮��ľ��볬���������
		if((X_difference>170)||(Y_difference>170))
		{
			//�ƶ���Transition_point 
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
				while( !QJ_TIM_finish_xy )//�ȴ�XY��ִ����
				{
					delay_ms(1);
					LED1=0;
				}
				Shalou( now_point.NOW_Z );//����ɳ©
				//PWM4_CH2_Init(1500,71);//Z�Ὺʼ�ƶ�
				while( QJ_TIM_TIM2_finish_shalou==0 )//�ȴ�Z���ƶ����
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
			
			//�˶���now_point
		  prestatus = nowstatus;
			nowstatus = Motion_status_analysis(prestatus,&Transition_point,&now_point,&next_point);
			result = XYZ_to_pwm(Transition_point.NOW_X,Transition_point.NOW_Y,Transition_point.NOW_Z,now_point.NOW_X,now_point.NOW_Y,now_point.NOW_Z,&p_a_d);
			
			//��ʱZ�᲻�����˶�
			mp_timer.dir_x = p_a_d.dir_x;
			mp_timer.dir_y = p_a_d.dir_y;
			mp_timer.dir_z = p_a_d.dir_z;
			mp_timer.PWM_counter_needed_X = p_a_d.PWM_x;
			mp_timer.PWM_counter_needed_Y = p_a_d.PWM_y;
			mp_timer.PWM_counter_needed_Z = p_a_d.PWM_z;
			//14.07.05�޸�	����������Ϣ
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

			Updata_pre_status(nowstatus);//���µ�ǰ�˶�״̬
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
			//14.07.05�޸�	����������Ϣ
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
				Shalou(now_point.NOW_Z);//����ɳ©
			//	PWM4_CH2_Init(1500,59);//Z�Ὺʼ�ƶ�
				while( QJ_TIM_TIM2_finish_shalou==0 )//�ȴ�ɳ©�򿪻�ر�
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
			
			Updata_pre_status(nowstatus);//���µ�ǰ�˶�״̬
			}
			Copy_Coordinate(&pre_point,&now_point);  //pre_point=now_point
			Copy_Coordinate(&now_point,&next_point); //now_point=nexy_point
	}
	return 1;
}

