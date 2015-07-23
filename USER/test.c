#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "gpio.h" 
#include "key.h"
#include "exti.h"
#include "wdg.h"
#include "timer1.h"
#include "beep.h"
#include "usart2.h"
//#include "motion.h"
#include "improved_motion_strategy.h"

/*
	�ú�����Ҫ�������Ӳ���ĳ�ʼ��
*/
void Hardware_Init()
{
	uart_init(72,115200);			//���ڳ�ʼ�� 
	uart2_init(36,115200);
	GPIO_Init();		  	 			//��ʼ����LED���ӵ�Ӳ���ӿ�
	EXTIX_Init(); 						//�ⲿ�жϳ�ʼ��
	KEY_Init();								//������ʼ��
	BEEP_Init();
	TIM5_Int_Init(1500,71);		//��ʼ����ʱ��5
														//��ʱ��3��ʼ�����Ƕ�200
	PWM3_CH2_Init(2000,719);
														//ȷ��ɳ©��ʼ״̬Ϊ��
	DIR_shalou=0;							//��
	TIM2_Int_Init(1000,71);
}

//�ú������ϵͳԭ��λ�õĳ�ʼ��
void Go_to_origin()
{
	const int PWM_number_Y = 5115;	//�뿪��λ����2cm   һ���׶�Ӧ341
	const int PWM_number_X = 10230;	//�뿪��λ����3cm
	const int PWM_number_Z = 30690;	//�뿪��λ����9cm
	int i = 0;
	u32 speed_y = 50;								//����X����ٶ�
	
	//z���ƶ���ԭ��
	DIR_Z = RETREAT;
	while(1)
	{
		if( LIMITER_Z_2==0 )	 				//Զ��������λ����Ϊ2
		{
			delay_us(5);
			if( LIMITER_Z_2==0 )
			{
				break;
			}
		}
		PWM_OUT_Z = (!PWM_OUT_Z);
		delay_us(30);
		PWM_OUT_Z = (!PWM_OUT_Z);
		delay_us(30);
	}
	DIR_Z = ADVANCE;
	for( i=0; i<PWM_number_Z; i++ )
	{
		PWM_OUT_Z = (!PWM_OUT_Z);
		delay_us(30);
		PWM_OUT_Z = (!PWM_OUT_Z);
		delay_us(30);
	}
	
	//Y���ƶ���ԭ��
	DIR_Y = RETREAT;
	while(1)
	{
		if( LIMITER_Y_2==0 )
		{
			delay_us(5);
			if( LIMITER_Y_2==0 )
			{
				break;
			}
		}
		PWM_OUT_Y = (!PWM_OUT_Y);
		delay_us(speed_y);
		PWM_OUT_Y = (!PWM_OUT_Y);
		delay_us(speed_y);
	}
	DIR_Y=ADVANCE;
	for(i=0;i<PWM_number_Y;i++)
	{
		if((PWM_number_Y-i)<200)
		{
			speed_y=100;
		}
		PWM_OUT_Y = (!PWM_OUT_Y);
		delay_us(speed_y);
		PWM_OUT_Y = (!PWM_OUT_Y);
		delay_us(speed_y);
	}
	
	//X���ƶ���ԭ��
	DIR_X = RETREAT;
	while(1)
	{
		if( LIMITER_X_2==0 )
		{
			delay_us(5);
			if( LIMITER_X_2==0 )
			{
				break;
			}
		}
		PWM_OUT_X = (!PWM_OUT_X);
		delay_us(30);
		PWM_OUT_X = (!PWM_OUT_X);
		delay_us(30);
	}
	DIR_X = ADVANCE;
	for( i=0; i<PWM_number_X; i++ )
	{
		PWM_OUT_X = (!PWM_OUT_X);
		delay_us(30);
		PWM_OUT_X = (!PWM_OUT_X);
		delay_us(30);
	}
}

//���ɨɳ_�������
void clean()															//TIM4_CH2_PWM_VAL=100 200
{
	const int Z_MOVE_PWM_NUMBER_UP   = 13640;	//z���ƶ��趨�ľ�������������������4CM
	const int Z_MOVE_PWM_NUMBER_DOWN = 8525;	//z���ƶ��趨�ľ�������������������2.5CM
	const int Y_MOVE_PWM_NUMBER = 126170;			//X���ƶ�����,37cm
	const int X_MOVE_PWM_NUMBER = 17050;			//Y���ƶ�����,5cm
	int i,j;
	u32 speed_go   = 26;
	u32 speed_back = 15;
	
	//1.1 ������ԭ����ص�ԭ��,Ȼ��ʼɨɳ
	if((Position_x!=0)||(Position_y!=0)||(Position_z!=0))		
	{
 		Go_to_origin();
	}
	//1.2 У׼��z������4cm
	DIR_Z = RETREAT;
	for( j=0; j<Z_MOVE_PWM_NUMBER_UP; j++)
	{
		PWM_OUT_Z = (!PWM_OUT_Z);
 		delay_us(60);
	//	BEEP=1;
		PWM_OUT_Z = (!PWM_OUT_Z);
 		delay_us(60);
	//	BEEP=0;
	}
	//1.3 ��һ�в�ɨ
	DIR_X = ADVANCE;
	for( j=0; j<(X_MOVE_PWM_NUMBER+6820); j++ )
		{
			PWM_OUT_X = (!PWM_OUT_X);
			delay_us(30);
			PWM_OUT_X = (!PWM_OUT_X);
			delay_us(30);
		}
	//1.4 ˢ�ӷ���
	TIM3_CH2_PWM_VAL = 170;
		
	//2.0 ********ɨɳѭ��**********
	for( i=0; i<8; i++ )
	{	
			speed_go   = 26;				//ǰ���ٶȲο�ֵ
			speed_back = 15;			//�����ٶȲο�ֵ
		
			//2.1 z�����2.5cm
			DIR_Z = ADVANCE;				
			for( j=0; j<Z_MOVE_PWM_NUMBER_DOWN; j++ )
			{
					PWM_OUT_Z = (!PWM_OUT_Z);
					delay_us(40);
					PWM_OUT_Z = (!PWM_OUT_Z);
					delay_us(40);
			}
			
			//2.2 ɨһ��
			DIR_Y = ADVANCE;//DIR_X=ADVANCE;
			for( j=0; j<Y_MOVE_PWM_NUMBER; j++ )
			{
					if( (Y_MOVE_PWM_NUMBER-j)<200 )
					{
							speed_go = 80;
					}
					PWM_OUT_Y = (!PWM_OUT_Y);
					delay_us(speed_go);
					PWM_OUT_Y = (!PWM_OUT_Y);
					delay_us(speed_go);
			}
			
			//2.3 z������2.5cm
			DIR_Z = RETREAT;
			for( j=0; j<Z_MOVE_PWM_NUMBER_DOWN; j++ )
			{
					PWM_OUT_Z = (!PWM_OUT_Z);
					delay_us(40);
					PWM_OUT_Z = (!PWM_OUT_Z);
					delay_us(40);
			}
			
			//2.4 �ص�Y=0��,ˢ�ӷ���
			DIR_Y = RETREAT;
			speed_back = 80;
			for( j=0; j<Y_MOVE_PWM_NUMBER; j++ )
			{
				if( j>200 )
				{
						speed_back = 15;
				}
				if( (Y_MOVE_PWM_NUMBER-j)<200 )
				{
						speed_back = 80;
				}
				PWM_OUT_Y = (!PWM_OUT_Y);
				delay_us(speed_back);
				PWM_OUT_Y = (!PWM_OUT_Y);
				delay_us(speed_back);
			}	
			
			//2.5 X���ƶ�һ��
			DIR_X = ADVANCE;
			for( j=0;j<X_MOVE_PWM_NUMBER;j++)
			{
				if((LIMITER_X_1==0)||(LIMITER_X_2==0))
				{
					delay_us(5);
					if((LIMITER_X_2==0)||(LIMITER_X_1==0))
					{
						break;
					}
				}
				PWM_OUT_X=(!PWM_OUT_X);
				delay_us(30);
				PWM_OUT_X=(!PWM_OUT_X);
				delay_us(30);
			}	
		}
		
		//3.0 ɨ��ˢ��̧��
		TIM3_CH2_PWM_VAL=130;
		
		//4.0 ����ԭ��
		Go_to_origin();

}
/*
	PWM�������ΪPA1����ʱ��2 CH2 Y�ᣩ,PA2��TIM2 Ch3 Z�ᣩ��PA8(��ʱ��1 CH1 X��)
	�����ʱ��3��PA7���
	���� DIR_X PD1 ; DIR_Y PD2 ; DIR_Z PD3
	�������� X PC0 PC1(��ӦTIM3 CH1��CH2) ; Y PC2 PC3 (��ӦTIM4 CH1��CH2) ; Z PC4 �� PC5 (PC0-PC5��Ӧ6��5��2��1��4��3)
	����У׼��PD4 PD5;
*/
int main(void)
{	
	u16 result = 0;
	u16 length = 0;
	u16 i = 0;
	u16 times  = 0;
	u16 flag_timer5 = 1;									//��ʱ��5���ر�־ 0��ʾ�� 1��ʾ��
	const u16 forecast_length = 10;				//��ʾ��ǰԤ����ٸ���
	struct Coordinate c_point1 = {0,0,0};	//��ʼ��(0,0,0)
	Stm32_Clock_Init(9); 									//ϵͳʱ������
	delay_init(72);	     									//��ʱ��ʼ��
	Hardware_Init();											//Ӳ����ʼ��

	//��ʼͨ��ʱ��������ܻ���������ʱ���ȶ�����ʱһ��ʱ�����
	while( i<4 )
	{
	  delay_ms(1000);
		LED0=!LED0;
		LED1=!LED1;
		i++;
	}	  
	LED0 = 1;												//LED0��LED1��ָʾ��,��ʼ״̬����
	LED1 = 1;
	Cq_insert( &c_point1 );					//���Ȳ�����ʼ���꣨0,0,0��
 	Go_to_origin();									//�豸�ƶ���ԭ��
	Hardware_Init();								//Ӳ����ʼ��
	
	//��ѭ��
	while( 1 )
	{
			//key2ϵͳ��ʼ���У�key3ֹͣ
			while( RUN ) 
			{
				LED1 = 0;
				LED0 = 0;
				if( UC_CLEAN_SAND == 6 )				//ɨɳ��־λ 0-��ɨɳ 6-ɨɳ	
				{
					clean();											//ɨɳ����
					UC_CLEAN_SAND = 0;						//ɨɳ��������־λ-UC_CLEAN_SAND��0
				}
				if( UC_Stop_flag == 1 )					//���ͽ�����־λ 0-���� 1-������ ���ͽ���
				{
					//	printf("�رն�ʱ��5");
						if( flag_timer5 == 1 )			//��ʱ��5�򿪱�־
						{
							if( (Mp_isempty(QJ_IMS_Mp_head,QJ_IMS_Mp_tail)) && (Cq_is_empty()) && (QJ_TIM_finish_xy==1) )
							{			//����Ϊ�� && �������Ϊ�� && XY���ƶ��Ѿ����
								TIM_control(5,0);				//��ʾ�رն�ʱ��5
								flag_timer5 = 0;				//��ʱ��5�رձ�־
							}
						}
				}
				length = Cq_length();
				if( length<LENGTH_CQ_USED )						//���껺����в������������һ������
				{
					result = Accept_message();
					
					if( result==0 )
					{
						times++;
					}
				}
				result = Mp_length(QJ_IMS_Mp_head,QJ_IMS_Mp_tail);
				result = MP_LENGTH - result - 1;				//����MP������ʣ��ռ��ж��
				printf("result: %d\r\n\r",result);
				if( result>forecast_length )						//X Y��Ļ�����о�ʣ��ռ����forecast_length
				{
					if( times>100 )
					{
						times = 0;
						if( length>0 )
						{
							Forecast_analysis(length);
						}
					}
					else
					{
						if( flag_timer5==0&&UC_Stop_flag==0 )		//��ʱ��5�ر��ţ��ҷ��ͱ�־λΪ����
						{
							TIM_control(5,1);											//��ʾ�򿪶�ʱ��5
							flag_timer5=1;												//��ʱ���Ѿ���
						}
						if( length>=forecast_length )						//�ж����껺������Ƿ���forecast_length����
						{	
							Forecast_analysis(forecast_length);
						}
					}
				}
				delay_us(10);
			}
	}
}











/*�ɰ汾ɨɳ
//���ɨɳ
void clean()
{
	const int Z_MOVE_PWM_NUMBER=3410;//z���ƶ��趨�ľ�������������������,�����ƶ�1CM
	const int Y_MOVE_PWM_NUMBER=126170;//X���ƶ�����
	const int X_MOVE_PWM_NUMBER=17050;//Y���ƶ�����
	u32 speed=30;
	int i=0,j=0;	
	//Z��PWM�����������
	GPIOB->CRL&=0x0FFFFFFF;
	GPIOB->CRL|=0x30000000;//PB7�������
	GPIOB->ODR|=1<<7;	
	//Z���������ƶ�
	DIR_Z=RETREAT;
	for(i=0;i<Z_MOVE_PWM_NUMBER;i++)
	{
		PWM_OUT_Z=(!PWM_OUT_Z);
		delay_us(60);
		PWM_OUT_Z=(!PWM_OUT_Z);
		delay_us(60);
	}
	
	if((Position_x!=0)||(Position_y!=0)||(Position_z!=0))//������ԭ����ص�ԭ��,Ȼ��ʼɨɳ
	{
		Go_to_origin();
	}
	
	for(i=0;i<8;i++)
	{
		//ˢ���½�
		DIR_Z=RETREAT;
		for(j=0;j<Z_MOVE_PWM_NUMBER;j++)
		{
			if((LIMITER_Z_1==0)||(LIMITER_Z_2==0))
			{
				delay_us(5);
				if((LIMITER_Z_2==0)||(LIMITER_Z_1==0))
				{
					break;
				}
			}
			PWM_OUT_Z=(!PWM_OUT_Z);
			delay_us(40);
			PWM_OUT_Z=(!PWM_OUT_Z);
			delay_us(40);
		}
		
		//ɨһ��
		DIR_X=ADVANCE;
		for(j=0;j<Y_MOVE_PWM_NUMBER;j++)
		{
			if((LIMITER_Y_1==0)||(LIMITER_Y_2==0))
			{
				delay_us(5);
				if((LIMITER_Y_2==0)||(LIMITER_Y_1==0))
				{
					break;
				}
			}
			if((Y_MOVE_PWM_NUMBER-j)<200)
			{
				speed=100;
			}
			PWM_OUT_Y=(!PWM_OUT_Y);
			delay_us(speed);
			PWM_OUT_Y=(!PWM_OUT_Y);
			delay_us(speed);
		}
		
		//ˢ��̧��
		DIR_Z=ADVANCE;
		for(j=0;j<Z_MOVE_PWM_NUMBER;j++)
		{
			if((LIMITER_Z_1==0)||(LIMITER_Z_2==0))
			{
				delay_us(5);
				if((LIMITER_Z_2==0)||(LIMITER_Z_1==0))
				{
					break;
				}
			}
			PWM_OUT_Z=(!PWM_OUT_Z);
			delay_us(40);
			PWM_OUT_Z=(!PWM_OUT_Z);
			delay_us(40);
		}
		
		//�ص�Y=0��,ˢ�ӷ���
		DIR_Y=RETREAT;
		for(j=0;j<Y_MOVE_PWM_NUMBER;j++)
		{
				if((LIMITER_Y_1==0)||(LIMITER_Y_2==0))
				{
					delay_us(5);
					if((LIMITER_Y_2==0)||(LIMITER_Y_1==0))
					{
						break;
					}
				}
			if((Y_MOVE_PWM_NUMBER-j)<200)
			{
				speed=100;
			}
			PWM_OUT_Y=(!PWM_OUT_Y);
			delay_us(speed);
			PWM_OUT_Y=(!PWM_OUT_Y);
			delay_us(speed);
		}
		
		//X���ƶ�һ��
		DIR_X=ADVANCE;
		for(j=0;j<X_MOVE_PWM_NUMBER;j++)
		{
			if((LIMITER_X_1==0)||(LIMITER_X_2==0))
			{
				delay_us(5);
				if((LIMITER_X_2==0)||(LIMITER_X_1==0))
				{
					break;
				}
			}
			PWM_OUT_X=(!PWM_OUT_X);
			delay_us(30);
			PWM_OUT_X=(!PWM_OUT_X);
			delay_us(30);
		}	
	}
	
	//����ԭ��
	Go_to_origin();
}*/