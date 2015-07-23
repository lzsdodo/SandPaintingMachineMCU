#include "timer1.h"
#include "gpio.h"
#include "delay.h"
#include "beep.h"
#include "exti.h"
#include "improved_motion_strategy.h"

//4λ��� ����1 �ۣ�2 �ƣ�3 �ȣ�4
const u16 CCW[8] = {0x08,0x0c,0x4,0x06,0x02,0x03,0x01,0x09};
const u16 Shalou_need_PWM = 512;
volatile u16 index_timer2 = 0;  							//ɳ©���ʹ��
volatile u16 DIR_shalou = 0;									//ɳ©�������
//volatile u16 TIM2_ARR = 0;									//���ƶ�ʱ��2���ʼ�ɳ©����
//const u16 DIANJI_to_SHALOU = 28;						//����ƶ��ٶ���ɳ©ת�ٵĶ�Ӧ����
volatile u16 SHOULOU_Change_Speed_flag = 0;		//ɳ©���ٱ�־ ������1����٣�����0���ñ���
volatile u16 QJ_TIM_TIM2_finish_shalou = 1;

volatile int PWM_counter_y  = 0;				//��¼Y���Ѿ�������������
volatile int PWM_counter_x  = 0;				//��¼x����������������
volatile int PWM_counter_xy = 0;				//���ڶ�ʱ���Ƿ������һ�׶�
volatile int PWM_counter_z  = 0;
volatile int PWM_counter_shalou = 0;
volatile u16 index_xy = 0; 							//����ָ��,ͬʱ������ǰ���������������һ�׶Σ�Xy��ʹ��
volatile u16 index_z  = 0;							//Z��ʹ�ã�����ͬ��

volatile u16 QJ_TIM_finish_xy = 1;			//1��ʾ�Ѿ���ɣ�0��ʾδ���
float QJ_TIM_PWM_to_Encoder   = 0.04;		//��PWM����ת��Ϊ����������ֵ��25600/r	 ת��һ�ܱ���������Ϊ1024

volatile u16 QJ_TIM_TIM4_finish_Z = 1;	//Z���ƶ���ɱ�־λ=0;//1��ʾz�ᣨ��ʱ��4��������ƶ���0��ʾz��δ����ƶ�

//static u16 Encodervalue1=0;						//��¼X��ı�����ֵ 
struct Move_parameter timer5_move_xy;

/*
	��������־λ���ڱ����Ƿ�������X,Y���һ���������������
	��һ���ж���ֻ�ܷ�תһ��IO�ڣ�
*/
volatile u16 flag_x_out = 0;
volatile u16 flag_y_out = 0;

//��ʱ��2�жϷ������	 
void TIM2_IRQHandler(void)
{ 		 
	if(TIM2->SR&0X0001)						//����ж�
	{
			LED1=!LED1;
		if(PWM_counter_shalou<Shalou_need_PWM)
		{
			if(DIR_shalou==0)					//��ɳ©
			{
				GPIOE->ODR&=0xF0FF;
				GPIOE->ODR|=CCW[index_timer2]<<8;
				index_timer2=(index_timer2+1)%8;
			}
			else 											//��ɳ©
			{
				GPIOE->ODR&=0xF0FF;
				GPIOE->ODR|=CCW[7-index_timer2]<<8;
				index_timer2=(index_timer2+1)%8;
			}
			PWM_counter_shalou++;
		}
		else
		{
			QJ_TIM_TIM2_finish_shalou=1;
			TIM2->CR1&=0xFFFE;				//�رն�ʱ��2
			PWM_counter_shalou=0;
		}
	}				   
	TIM2->SR&=~(1<<0);						//����жϱ�־λ 	    
}

///*
//	��ʱ��4�ն˷�����
//	����Z���PWM�ź����
//*/
//void TIM4_IRQHandler(void)
//{ 		    	
//	if(TIM4->SR&0X0001)//����ж�
//	{	
//		if((LIMITER_Z_1==0)||(LIMITER_Z_2==0))
//	 	{
//		 	delay_ms(1);//��������
//			if((LIMITER_Z_1==0)||(LIMITER_Z_2==0))
//			{	
//				LED1=0;
//				TIM4_CH2_PWM_VAL=0;
//				TIM4->CR1&=0xFFFE;	//�رն�ʱ��2
//				PWM_counter_z=0;//������������
//				index_z=0;
//			} 
//		}
//		else
//		{
//			PWM_counter_z++;		  //���������1
//			if(PWM_counter_z>=QJ_IMS_Mp_TIM4_Z.PWM_counter_part[index_z])//�ж�index�׶��Ƿ��Ѳ����㹻��PWM����
//			{
//				index_z++;
//				if(index_z>=JIASU_STEP) //�����ȫ�����
//				{
//					TIM4_CH2_PWM_VAL=0;
//					TIM4->CR1&=0xFFFE;	//�رն�ʱ��4
//					PWM_counter_z=0;
//					index_z=0;
//					QJ_TIM_TIM4_finish_Z=1;
//				}
//				else
//				{
//					PWM_counter_z=0;	
//					TIM4->ARR=QJ_IMS_Mp_TIM4_Z.TIM_ARR[index_z];
//					TIM4_CH2_PWM_VAL=QJ_IMS_Mp_TIM4_Z.TIM_ARR[index_z]/2;
//					TIM4->EGR|=1<<0;
//					delay_us(2);
//					TIM4->EGR&=0xFFFE;
//				}
//			} 
//		}
//	}	  	    				   			     	    					   
//	TIM4->SR&=~(1<<0);//����жϱ�־λ 	    
//}

void TIM5_IRQHandler(void)
{
	int i=0,t;
	int F_xy=0;									//��¼��ֵ
	u8  check = 0;
	u8  position_data[15] = {0xFF,0xFF,0xFF,0xFF,0x8,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
	
	if(TIM5->SR&0x0001)
	{
		if((LIMITER_X_1==0)||(LIMITER_X_2==0)||(LIMITER_Y_1==0)||(LIMITER_Y_2==0))
	 	{
		 	delay_ms(1);						//��������
			if((LIMITER_X_1==0)||(LIMITER_X_2==0)||(LIMITER_Y_1==0)||(LIMITER_Y_2==0))
			{	
				LED1=0;
				BEEP=0;
				TIM5->CR1&=0xFFFE;		//�رն�ʱ��5
				PWM_counter_x=0;			//������������
				PWM_counter_y=0;
				index_xy=0;
			} 
		}
		else
		{
			if(QJ_TIM_finish_xy==1)										//XY���ƶ��Ѿ����
			{
				if(QJ_IMS_Mp_head!=QJ_IMS_Mp_tail)			//������в�Ϊ��
				{
					timer5_move_xy.dir_x = QJ_IMS_Mp_queue[QJ_IMS_Mp_head].dir_x;
					timer5_move_xy.dir_y = QJ_IMS_Mp_queue[QJ_IMS_Mp_head].dir_y;
					timer5_move_xy.guaxian = QJ_IMS_Mp_queue[QJ_IMS_Mp_head].guaxian;
					timer5_move_xy.PWM_counter_needed_X = QJ_IMS_Mp_queue[QJ_IMS_Mp_head].PWM_counter_needed_X;
					timer5_move_xy.PWM_counter_needed_Y = QJ_IMS_Mp_queue[QJ_IMS_Mp_head].PWM_counter_needed_Y;
					
					//ȡ������������Ϣ
					timer5_move_xy.Now_Position_X = QJ_IMS_Mp_queue[QJ_IMS_Mp_head].Now_Position_X;
					timer5_move_xy.Now_Position_Y = QJ_IMS_Mp_queue[QJ_IMS_Mp_head].Now_Position_Y;
					timer5_move_xy.Now_Position_Z = QJ_IMS_Mp_queue[QJ_IMS_Mp_head].Now_Position_Z;				
					//��Ϣ����Ϊ���ݰ�
					position_data[TYPE_START] 		 = RIGHT;
					position_data[MESSAGE_START]   = timer5_move_xy.Now_Position_X & 0xFF;
					position_data[MESSAGE_START+1] = timer5_move_xy.Now_Position_X / 256;
					position_data[MESSAGE_START+2] = timer5_move_xy.Now_Position_Y & 0xFF;
					position_data[MESSAGE_START+3] = timer5_move_xy.Now_Position_Y / 256;
					position_data[MESSAGE_START+4] = timer5_move_xy.Now_Position_Z & 0xFF;
					position_data[MESSAGE_START+5] = timer5_move_xy.Now_Position_Z / 256;
					//������
					for( t=6; t<13; t++ )
					{
						check += position_data[t];
					}
					check = ~check;
					position_data[14] = check;
					//�������ݰ�
					for( t=0; t<15; t++ )
					{
						while( (USART1->SR&0x40)==0 );//�ȴ����ͽ���
						USART1->DR = position_data[t];
					}
					
					for(i=0;i<JIASU_STEP;i++)
					{
						timer5_move_xy.PWM_counter_part[i]=QJ_IMS_Mp_queue[QJ_IMS_Mp_head].PWM_counter_part[i];
						timer5_move_xy.TIM_ARR[i]=QJ_IMS_Mp_queue[QJ_IMS_Mp_head].TIM_ARR[i];
					}
					QJ_IMS_Mp_head=(QJ_IMS_Mp_head+1)%MP_LENGTH;
					if(timer5_move_xy.dir_x>0)
					{
						DIR_X=1;
					}
					else
					{
						DIR_X=0;
					}
					if(timer5_move_xy.dir_y>0)
					{
						DIR_Y=1;
					}
					else
					{
						DIR_Y=0;
					}
					TIM5->ARR=timer5_move_xy.TIM_ARR[0];
// 					TIM2_ARR=timer5_move_xy.TIM_ARR[0]*DIANJI_to_SHALOU;
// 					SHOULOU_Change_Speed_flag=1;
					QJ_TIM_finish_xy=0;
					index_xy=0;
					PWM_counter_x=0;
					PWM_counter_xy=0;
					PWM_counter_y=0;
				}
			}//XY���ƶ������,ȡ��
			
			if(QJ_TIM_finish_xy==0)
			{
				if((flag_x_out>=1)||(flag_y_out>=1))
				{
					if(flag_x_out>=1)
					{
						PWM_OUT_X=!PWM_OUT_X;
						flag_x_out=0;
					}
					if(flag_y_out>=1)
					{
						PWM_OUT_Y=!PWM_OUT_Y;
						flag_y_out=0;
					}
				}
				else
				{
					
					//�������y=x�·� ��һ����
					if(timer5_move_xy.guaxian==1)
					{
						F_xy=timer5_move_xy.PWM_counter_needed_X*PWM_counter_y - timer5_move_xy.PWM_counter_needed_Y*PWM_counter_x;
						if(F_xy>=0)
						{
							PWM_OUT_X=0;
							PWM_counter_x++;
							PWM_counter_xy++;
							flag_x_out=1;
						}
						else
						{
							PWM_OUT_X=0;
							PWM_OUT_Y=0;
							PWM_counter_x++;
							PWM_counter_xy++;//�ü��ٲ����������������һ
							PWM_counter_y++;
							flag_x_out=1;
							flag_y_out=1;
						}
						if(PWM_counter_xy>=timer5_move_xy.PWM_counter_part[index_xy])
						{
							PWM_counter_xy=0;
							index_xy++;
							while(timer5_move_xy.PWM_counter_part[index_xy]==0)
							{
								index_xy++;
								if(index_xy>=JIASU_STEP)
								{
									QJ_TIM_finish_xy=1;
									break;
								}
							}
							if(index_xy<JIASU_STEP)
							{
								TIM5->ARR=timer5_move_xy.TIM_ARR[index_xy];
						//		TIM2_ARR=timer5_move_xy.TIM_ARR[index_xy]*DIANJI_to_SHALOU;
								SHOULOU_Change_Speed_flag=1;
							}
						}
						if(PWM_counter_x >= timer5_move_xy.PWM_counter_needed_X)
						{
							QJ_TIM_finish_xy=1;
						}
					}//����1
					
					//�������y=x�Ϸ� �ڶ�����
					if(timer5_move_xy.guaxian==2)
					{
						F_xy=timer5_move_xy.PWM_counter_needed_X*PWM_counter_y - timer5_move_xy.PWM_counter_needed_Y*PWM_counter_x;
						if(F_xy>=0)
						{
							//��ֱ���Ϸ� x���һ������ Y���һ������
							PWM_OUT_X=0;
							PWM_OUT_Y=0;
							PWM_counter_x++;
							PWM_counter_xy++;//�ü��ٲ����������������һ
							PWM_counter_y++;
							flag_x_out=1;
							flag_y_out=1;
						}
						else
						{
							//��ֱ���·��� Y���һ������
							PWM_OUT_Y=0;
							PWM_counter_y++;
							PWM_counter_xy++;
							flag_y_out=1;
						}
						if(PWM_counter_xy>=timer5_move_xy.PWM_counter_part[index_xy])
						{
							PWM_counter_xy=0;
							index_xy++;
							while(timer5_move_xy.PWM_counter_part[index_xy]==0)
							{
								index_xy++;
								if(index_xy>=JIASU_STEP)
								{
									QJ_TIM_finish_xy=1;
									break;
								}
							}
							if(index_xy<JIASU_STEP)
							{
								TIM5->ARR=timer5_move_xy.TIM_ARR[index_xy];
								TIM5->ARR=timer5_move_xy.TIM_ARR[index_xy];
							//	TIM2_ARR=timer5_move_xy.TIM_ARR[index_xy]*DIANJI_to_SHALOU;
								SHOULOU_Change_Speed_flag=1;
							}
						}
						if(PWM_counter_y >= timer5_move_xy.PWM_counter_needed_Y)
						{
							QJ_TIM_finish_xy=1;
						}
					}//����2
				}
			}//ΪXY���������
		}//������λ����
	}
	TIM5->SR&=~(1<<0);//����жϱ�־λ
}

void TIM2_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<0;;	//TIM2ʱ��ʹ��    
 	TIM2->ARR=arr;  	//�趨�������Զ���װֵ//�պ�1ms    
	TIM2->PSC=psc;  	//Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��		  
	TIM2->DIER|=1<<0;   //��������ж�	  
	TIM2->CR1|=0x01;    //ʹ�ܶ�ʱ��2
	PWM_counter_shalou=0;
	QJ_TIM_TIM2_finish_shalou=0;
  MY_NVIC_Init(2,2,TIM2_IRQChannel,2);//��ռ2�������ȼ�3����2									 
}

// /*
// 	��ʼ����ʱ��3�����ڼ�¼��������ֵ����ӦX���ƶ�
// */
// void TIM3_CNT_Init(u16 arr,u16 psc)
// {
// 	RCC->APB1ENR|=1<<1;//TIM3ʱ��ʹ��
// 	RCC->APB2ENR|=1<<2;//ʹ��PORTAʱ��
// 	GPIOA->CRL&=0x00FFFFFF;//
// 	GPIOA->CRL|=0x44000000;//PA6 PA7����Ϊ����
// 	TIM3->ARR=arr;//��Ƽ������Զ�װ��
// 	TIM3->PSC=psc;//Ԥ��Ƶ��
// 	TIM3->SMCR&=0xFFF8;
// 	TIM3->SMCR|=0x0003;//������ģʽ��T2��T1���ؼ���

// 	//λ7:4����TI1����Ĳ���Ƶ��,0001����Ƶ��fSAMPLING=fCK_INT,N=8
// 	TIM3->CCMR1|=0x0011;//CC1ͨ��������Ϊ���룬IC1ӳ����TI1��

// 	//CC1ͨ������Ϊ���룬0�������ࣺ��������IC1������
// 	TIM3->CCER&=~(1<<1);
// 	TIM3->CCER&=~(1<<5);
// 	TIM3->CCER|=1<<0;//���������
// 	TIM3->CR1|=0x01;//ʹ�ܶ�ʱ��3
// 	TIM3->CR1&=~(1<<4);
// }

//��ʼ����ʱ��3 
void PWM3_CH2_Init(u16 arr,u16 psc)
{
	//Z��PWM�����������
  //GPIOB->CRL&=0x0FFFFFFF;
	//GPIOB->CRL|=0xB0000000;//PB7�������
	//GPIOB->ODR|=1<<7;
	GPIOA->CRL&=0x0FFFFFFF;
	GPIOA->CRL|=0xB0000000;//PA7�������
	RCC->APB2ENR|=1<<2;//io�˿�Aʱ�ӿ���
	RCC->APB1ENR|=1<<1;//TIM3ʱ��ʹ��
	//RCC->APB1ENR|=1<<2;//TIM4ʱ��ʹ��
	TIM3->ARR=arr;//�Զ�װ�ؼĴ���
	TIM3->PSC=psc;//Ԥ��Ƶ�Ĵ���
	TIM3->CCMR1|=7<<12;
	TIM3->CCMR1|=1<<11;
	TIM3->CCER|=1<<4;//����
	TIM3->CCER|=1<<5;//�͵�ƽ��Ч
	//TIM3->CR1|=0x8000;
	//TIM3->CR1|=0x01;
	TIM3->CR1|=0x0080;
	TIM3->CR1|=0x01;
	TIM3_CH2_PWM_VAL=130;//PWM�������ռ�ձ�
	//TIM3->DIER|=1<<0;
	//TIM3->DIER|=1<<6;
	//MY_NVIC_Init(2,1,TIM4_IRQChannel,2);
	//index_z=0;
	//PWM_counter_z=0;
	//QJ_TIM_TIM4_finish_Z=0;
	//DIR_Z=QJ_IMS_Mp_TIM4_Z.dir_z;
} 

//��ʱ��5��ʼ��
void TIM5_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<3;
	TIM5->ARR=arr;
	TIM5->PSC=psc;
	TIM5->DIER|=1<<0;
	TIM5->CR1|=0x01;
	MY_NVIC_Init(2,0,TIM5_IRQChannel,2);//��ռ���ȼ�1����Ӧ���ȼ�2
	flag_x_out=0;
	flag_y_out=0;
	QJ_TIM_finish_xy=1;
}

//��ʱ�����ƺ���
void TIM_control(u16 timer, u16 control)
{
	//�رն�ʱ��
	if(control==0)
	{
		switch(timer)
		{
			case 2:
				TIM2->CR1&=0xFFFE;  //�رն�ʱ��2
			break;
			case 3:
				TIM3->CR1&=0xFFFE;  //�رն�ʱ��3
			break;
			case 5:								//�رն�ʱ��5
				TIM5->CR1&=0xFFFE;
			break;
			default:
				break;
		}
	}
	else  //�򿪶�ʱ��
	{
		switch(timer)
		{
			case 2:								
				TIM2_Int_Init(4500,143);
				break;
			case 3:
				PWM3_CH2_Init(1500,71);
				break;
			case 5:
				TIM5_Int_Init(1500,71);
				break;
			default:
				break;
		}
	}
}

