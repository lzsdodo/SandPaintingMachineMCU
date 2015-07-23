#include "timer.h"
#include "led.h"
#include "delay.h"
#include "exti.h"

//u16 ENCODER1=0;
u16 Encodervalue1=0;//��������ֵ

int PWM_counter2=0;//��¼��ʱ��2�Ѳ���PWM������� 

int PWM_counter1=0;//��¼��ʱ��1�Ѿ�������PWM�������
int PWM_counter4=0;

u16 ARR1[21]={1500,900,450,300,225,150,90,75,56,50,45,50,56,75,90,150,225,300,450,900,1500};

int PWM_counter_part1[21]={0};//��ʱ��1��5���׶����������PWM�������
u16 ARR2[21]={1500,900,450,300,225,150,90,75,56,50,45,50,56,75,90,150,225,300,450,900,1500};
u16 ARR4[21]={1500,900,450,300,225,150,90,75,56,50,45,50,56,75,90,150,225,300,450,900,1500};
int PWM_counter_part2[21]={0};
int PWM_counter_part4[21]={0};
u16 index1=0; //����ָ��,ͬʱ������ǰ���������������һ�׶�
u16 index2=0;
u16 index4=0;
u8 Original_Dir1=0;
u8 Original_Dir2=0;//���ڱ���һ���ƶ����������
u16 TIM2_CH=0;	  //�ж�ͨ��
u16 TIM1_finish=0; //TIM1��ɱ�־
u16 TIM2_finish=0;//TIM2��ɱ�־
u16 TIM4_finish=0;//TIM4��ɱ�־
u16 BUCHANG1=0;
u16 BUCHANG2=0;
float encoder1[21]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//��¼��ʱ��1�������ʱ�̵ı�������Ҫ��¼��ֵ
float PWM_to_Encoder=0.04;//��PWM����ת��Ϊ����������ֵ��25600/r	 ת��һ�ܱ���������Ϊ1024

/*
	��ʱ��2���жϷ�����
*/
void TIM2_IRQHandler(void)
{ 		    		 			    
	if(TIM2->SR&0X0001)//����ж�
	{	
		if((LIMITER_Z_1==0)||(LIMITER_Z_2==0))
	 	{
		 	delay_ms(1);//��������
			if((LIMITER_Z_1==0)||(LIMITER_Z_2==0))
			{	
				LED1=0;
				TIM4_CH2_PWM_VAL=0;
				TIM2->CR1&=0xFFFE;	//�رն�ʱ��2
				PWM_counter2=0;//������������
				index2=0;
			} 
		}
		else
		{
			PWM_counter2++;		  //���������1
			if(PWM_counter2>=PWM_counter_part2[index2])//�ж�index�׶��Ƿ��Ѳ����㹻��PWM����
			{
					index2++;
				if(index2>20) //�����ȫ�����
				{
					
							TIM2_CH2_PWM_VAL=0;
							TIM2->CR1&=0xFFFE;	//�رն�ʱ��1
							PWM_counter2=0;
							index2=0;
							TIM2_finish=1;
				}
				else
				{
				
					while(PWM_counter_part2[index2]==0)	//��һ�׶�PWM������Ϊ��������
					{
						index2++;
					}
					PWM_counter2=0;	
					TIM2->ARR=ARR2[index2];
					TIM2_CH2_PWM_VAL=ARR2[index2]/2;
					TIM2->EGR|=1<<0;
					delay_us(20);
					TIM2->EGR&=0xFFFE;
				}
			} 
		}
	}	  	    				   				     	    					   
	TIM2->SR&=~(1<<0);//����жϱ�־λ 	    
}

/*
	��ʱ��1���жϷ�����
*/
void TIM1_UP_IRQHandler(void)
{
	 if(TIM1->SR&0X0001)//����ж�
	 {	
		if(LIMITER_X_1==0||LIMITER_X_2==0)
		{
			delay_ms(1);//��������
			if(LIMITER_X_1==0||LIMITER_X_2==0)
			{
				LED1=0;
				TIM1_CH1_PWM_VAL=0;	
				TIM1->CR1&=0xFFFE;	//�رն�ʱ��1
				PWM_counter1=0;
				index1=0;
			}
				
		}
		else
		{
			PWM_counter1++;
			if(PWM_counter1>=PWM_counter_part1[index1])//���index�׶��Ѳ����㹻��PWM�����������һ�׶�
			{
				PWM_counter_part1[index1]=0;																			 
				index1++;
				 Encodervalue1=TIM3->CNT;
				// Encodervalue1=encoder1[index1-1];
				if(Encodervalue1>32767)
				{
					Encodervalue1=65535-Encodervalue1+1;
				}
				if(index1>20) //��������
				{
					if(encoder1[index1-1]>Encodervalue1)
					{
						BUCHANG1=(encoder1[index1-1]-Encodervalue1)/PWM_to_Encoder;
						if(BUCHANG1>25)
						{
							DIR_X=Original_Dir1;
							index1--;
						   PWM_counter_part1[index1]=BUCHANG1;
						   PWM_counter1=0;
						   TIM1->ARR=ARR1[index1-1];
						}
						else
						{
							TIM1_CH1_PWM_VAL=0;	
							TIM1->CR1&=0xFFFE;	//�رն�ʱ��1
							PWM_counter1=0;
							index1=0;
							TIM1_finish=1;
						}	
					}
				/*	else if(encoder1[index1-1]<Encodervalue1)	  //��ʱ�����ǹ������󲹳�
					{
					  	BUCHANG1=(Encodervalue1-encoder1[index1-1])/PWM_to_Encoder;
						if(BUCHANG1>4)
					  	{
							DIR_X=!DIR_X;
							index1--;
							PWM_counter1=0;
							PWM_counter_part1[index1]=BUCHANG1;
							test_index2[index1]++;
							test_PWM_index2[index1]+=PWM_counter_part1[index1];
						}
						else
						{
							TIM1_CH1_PWM_VAL=0;	
							TIM1->CR1&=0xFFFE;	//�رն�ʱ��1
						//	PWM_test=PWM_test+PWM_counter1;//������
							PWM_counter1=0;
							//	PWM_counter_need1=0;//������������
							//TIM3->CNT=0;
							index1=0;
							TIM1_finish=1;
						//	RUN=2;	
						}
					}*/
					else
					{
						TIM1_CH1_PWM_VAL=0;	
						TIM1->CR1&=0xFFFE;	//�رն�ʱ��1
						PWM_counter1=0;
						index1=0;
						TIM1_finish=1;
					}
				}
				else
				{
					while(PWM_counter_part1[index1]==0)	//��һ�׶�PWM������Ϊ��������
					{
						index1++;
					}
					if(encoder1[index1-1]>Encodervalue1) //����ʧ��
					{
						BUCHANG1=encoder1[index1-1]-Encodervalue1;
						if(BUCHANG1>0)
						{
							PWM_counter_part1[index1]=PWM_counter_part1[index1]+BUCHANG1/PWM_to_Encoder;
						}
					}
					if(encoder1[index1-1]<Encodervalue1)//���ֹ���
					{
						BUCHANG1=Encodervalue1-encoder1[index1-1];
						BUCHANG1=BUCHANG1/PWM_to_Encoder;
						if(BUCHANG1>4&&BUCHANG1<PWM_counter_part1[index1])
						{
							
							PWM_counter_part1[index1]=PWM_counter_part1[index1]-BUCHANG1;
						}
						else
						{
							PWM_counter_part1[index1]=PWM_counter_part1[index1]/2;	
						}	
					}
					PWM_counter1=0;	
					TIM1->ARR=ARR1[index1];
					TIM1_CH1_PWM_VAL=ARR1[index1]/2;
					TIM1->EGR|=1<<0;
					delay_us(20);
					TIM1->EGR&=0xFFFE;
				}
			}	
		}	  	    				   				     	    	
	}				   
	TIM1->SR&=~(1<<0);//����жϱ�־λ
}

/*
	��ʱ��4�ն˷�����
*/
void TIM4_IRQHandler(void)
{ 		    		 			    
	if(TIM4->SR&0X0001)//����ж�
	{	
		if((LIMITER_Z_1==0)||(LIMITER_Z_2==0))
	 	{
		 	delay_ms(1);//��������
			if((LIMITER_Z_1==0)||(LIMITER_Z_2==0))
			{	
				LED1=0;
				TIM4_CH2_PWM_VAL=0;
				TIM4->CR1&=0xFFFE;	//�رն�ʱ��2
				PWM_counter4=0;//������������
				index4=0;
			} 
		}
		else
		{
			PWM_counter4++;		  //���������1
			if(PWM_counter4>=PWM_counter_part4[index4])//�ж�index�׶��Ƿ��Ѳ����㹻��PWM����
			{
					index4++;
				if(index4>20) //�����ȫ�����
				{
					
							TIM4_CH2_PWM_VAL=0;
							TIM4->CR1&=0xFFFE;	//�رն�ʱ��1
							PWM_counter4=0;
							index4=0;
							TIM4_finish=1;
				}
				else
				{
				
					while(PWM_counter_part4[index4]==0)	//��һ�׶�PWM������Ϊ��������
					{
						index4++;
					}
					PWM_counter4=0;	
					TIM4->ARR=ARR4[index4];
					TIM4_CH2_PWM_VAL=ARR4[index4]/2;
					TIM4->EGR|=1<<0;
					delay_us(20);
					TIM4->EGR&=0xFFFE;
				}
			} 
		}
	}	  	    				   				     	    					   
	TIM4->SR&=~(1<<0);//����жϱ�־λ 	    
}

//TIM2��ʱ��PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void PWM2_CH2_Init(u16 arr,u16 psc,u16 p_c_n)
{		 					 
	//�˲������ֶ��޸�IO������
	RCC->APB1ENR|=1<<0;       //TIM2ʱ��ʹ��
	GPIOA->CRL&=0XFFFFFF0F;	//PA1���
	GPIOA->CRL|=0XB0;  //���ù������
	GPIOA->ODR|=1<<1;  //PA1����
	TIM2->ARR=arr;//�趨�������Զ���װֵ 
	TIM2->PSC=psc;//Ԥ��Ƶ������Ƶ
	TIM2_CH2_PWM_VAL=arr/2;
	//CCMR1	OC2M��Ϊ111��ѡ��ģʽ2
	TIM2->CCMR1|=7<<12;  //CH2 PWM2ģʽ		 
	TIM2->CCMR1|=1<<11; //CH2Ԥװ��ʹ��	   
	TIM2->CCER|=1<<4;   //OC2 ���ʹ��
		
	// 9:8 ��ʱ��ʱ��Ƶ��CK_INT Tdts=Tck_int
	// 7 ARPE���Զ���װ��Ԥװ������λ  
	//6:5 00:���ض���ģʽ
	// 4 DIR��0���������ϼ���
	// 3 OPM:������ģʽ 0���ڷ��������¼�ʱ����������ֹͣ
	//2  URS:0��������ж�
	// 1 UDIS: 0����UEV
	// 0 CEN:1����������
	TIM2->CR1=0x8000;   //ARPEʹ�� 
	TIM2->CR1|=0x01;    //ʹ�ܶ�ʱ��2 
	TIM2->DIER|=1<<0;   //��������ж�				
	TIM2->DIER|=1<<6;   //�������ж�		  							    
  	MY_NVIC_Init(2,3,TIM2_IRQChannel,2);//��ռ2�������ȼ�3����2	
	index2=0;
	TIM2_finish=0;
	TIM2_CH=2;
//	TIM4->CNT=0;										  
}  	 

/*void PWM2_CH3_Init(u16 arr,u16 psc,u16 p_c_n)
{
	RCC->APB1ENR|=1<<0;       //TIM2ʱ��ʹ��  	  	
	GPIOA->CRL&=0xFFFFF0FF;	 //PA2
	GPIOA->CRL|=0xB00;		 //PA2�������
	//GPIOA->ODR|=1<<2;
	TIM2->ARR=arr;//�趨�������Զ���װֵ 
	TIM2->PSC=psc;//Ԥ��Ƶ������Ƶ
	TIM2_CH3_PWM_VAL=arr/2;
		
	TIM2->CCMR2|=7<<4;  //CH3 PWM2ģʽ		 
	TIM2->CCMR2|=1<<3; //CH3Ԥװ��ʹ��	   
//	TIM2->CCMR2&=0xFFFC; //CH3����Ϊ���
	TIM2->CCER|=1<<8;   //OC3 ���ʹ��	
	TIM2->DIER|=1<<0;   
	TIM2->DIER|=1<<6;
	TIM2->CR1=0x0080;   //ARPEʹ�� 
	TIM2->CR1|=0x01;    //ʹ�ܶ�ʱ��2	
	TIM2->EGR|=0x01;
	delay_us(20);
	TIM2->EGR&=0xFFFE;
	MY_NVIC_Init(1,3,TIM2_IRQChannel,2);//��ռ1�������ȼ�3����2
	index2=0;
	TIM2_finish=0;
	TIM2_CH=3;
//	TIM8->CNT=0;
} */ 

/*
	��ʼ������,��ʼ��TIM1
*/
void PWM1_CH1_Init(u16 arr,u16 psc,u16 p_c_n)
{
	RCC->APB2ENR|=1<<11;//��ʱ��TIM1ʹ��
	RCC->APB2ENR|=1<<2;//GPIOA ʱ��ʱ��
	GPIOA->CRH&=0xFFFFFFF0;//����PA8
	GPIOA->CRH|=0xB;  //���ù����������
	GPIOA->ODR|=1<<8;;//PA8����
	TIM1->ARR=arr;//���ü������Զ���װ��
	TIM1->PSC=psc;//Ԥ��Ƶ
	TIM1_CH1_PWM_VAL=arr/2;
	TIM1->CCMR1|=7<<4;//CH1_PWM2ģʽ
	TIM1->CCMR1|=1<<3;//CH1Ԥװ��ʹ��
	TIM1->CCER|=1<<0;//OC1���ʹ��
	TIM1->BDTR|=0x8000;
	TIM1->CR1|=0x1;//ʹ�ܶ�ʱ��1
	TIM1->CR1&=0xFFFD;//��������ж�
	//TIM1->CR1&=0xFF7F; //APRE=0ʱ��Ԥװ�ؼĴ��������ݿ�����ʱ���͵�Ӱ�ӼĴ���,TIM1_ARR�Ĵ���û�л���
	TIM1->DIER|=1<<0;//��������ж�
	TIM1->DIER|=1<<6;//	�������ж�
	MY_NVIC_Init(2,2,TIM1_UP_IRQChannel,2);//��ռ1�������ȼ�2����2
	index1=0;
	TIM1_finish=0;
	TIM3->CNT=0;
}

/*
	��ʼ����ʱ��3�����ڼ�¼��������ֵ����Ӧ��ʱ��1��PWM���
*/
void TIM3_CNT_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;//TIM3ʱ��ʹ��
	RCC->APB2ENR|=1<<2;//ʹ��PORTAʱ��
	GPIOA->CRL&=0x00FFFFFF;//
	GPIOA->CRL|=0x44000000;//PA6 PA7����Ϊ����
	TIM3->ARR=arr;//��Ƽ������Զ�װ��
	TIM3->PSC=psc;//Ԥ��Ƶ��
	TIM3->SMCR&=0xFFF8;
	TIM3->SMCR|=0x0003;//������ģʽ��T2��T1���ؼ���

	//λ7:4����TI1����Ĳ���Ƶ��,0001����Ƶ��fSAMPLING=fCK_INT,N=8
	TIM3->CCMR1|=0x0011;//CC1ͨ��������Ϊ���룬IC1ӳ����TI1��

	//CC1ͨ������Ϊ���룬0�������ࣺ��������IC1������
	TIM3->CCER&=~(1<<1);
	TIM3->CCER&=~(1<<5);
	TIM3->CCER|=1<<0;//���������
	TIM3->CR1|=0x01;//ʹ�ܶ�ʱ��3
	TIM3->CR1&=~(1<<4);
}

/*
	��ʼ����ʱ��4,��ʱ��4���ڼ�¼��������ֵ����Ӧ��ʱ��2��PWM���
*/
/*
void TIM4_CNT_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<2;//TIM4ʱ��ʹ��
	TIM4->ARR=arr;
	TIM4->PSC=psc;
	//TIM4->DIER|=1<<0;//��������ж�
	GPIOB->CRL&=0x00FFFFFF;
	GPIOB->CRL|=0x44000000;//PB6,PB7����Ϊ����ʹ��
	TIM4->SMCR&=0xFFF8;
	TIM4->SMCR|=0x0003;//������ģʽ��T2��T1���ؼ���
	//λ7:4����TI1����Ĳ���Ƶ��,0001����Ƶ��fSAMPLING=fCK_INT,N=8
	TIM4->CCMR1|=0x0011;//cc1ͨ��������Ϊ���룬IC1ӳ����TI1��

	//cc1ͨ������Ϊ���룬0��������:��������IC1������
	TIM4->CCER&=~(1<<1);
	TIM4->CCER&=~(1<<5);
	TIM4->CCER|=1<<0;//���������
	TIM4->CR1|=0x01;//ʹ�ܶ�ʱ��4
}*/
 
 //��ʼ����ʱ��4 ch2��ΪZ�����PWM
void PWM4_CH2_Init(u16 arr,u16 psc,u16 p_c_n)
{
	RCC->APB1ENR|=1<<2;//TIM4ʱ��ʹ��
	GPIOB->CRL&=0x0FFFFFFF;
	GPIOB->CRL|=0xB0000000;//PB7�������
	GPIOA->ODR|=1<<7;
	TIM4->ARR=arr;
	TIM4->PSC=psc;
	TIM4_CH2_PWM_VAL=arr/2;
	TIM4->CCMR1|=7<<12;
	TIM4->CCMR1|=1<<11;
	TIM4->CCER|=1<<4;
	TIM4->CR1|=0x8000;
	TIM4->CR1|=0x01;
	TIM4->DIER|=1<<0;
	TIM4->DIER|=1<<6;
	MY_NVIC_Init(2,1,TIM4_IRQChannel,2);
	index4=0;
	PWM_counter4=0;
	TIM4_finish=0;
} 
/*
	��ʼ����ʱ��TIM8,TIM8��Ϊ��������¼
*/
/*void TIM8_CNT_Init(u16 arr,u16 psc)
{
	RCC->APB2ENR|=1<<13;//��ʱ��8ʹ��
	GPIOC->CRL&=0x00FFFFFF;
	GPIOC->CRL|=0x44000000;	//PC6 PC7 ����Ϊ����ʹ��
	TIM8->ARR=arr;
	TIM8->PSC=psc;
	TIM8->SMCR&=0xFFFD;
	TIM8->SMCR|=0x0002;

	//λ[1:0]CC1ͨ��������Ϊ���룬IC1ӳ����TI1��
	//λ[3:2]��Ԥ��Ƶ��������������ϼ�⵽��ÿһ�����ض�����һ�β���
	TIM8->CCMR1&=0xFF00;
	TIM8->CCMR1|=0x0011;

	TIM8->CCER&=~(1<<1);//������
	TIM8->CCER&=~(1<<5);
	TIM8->CCER|=1<<0;
	TIM8->CR1|=0x01;//ʹ�ܶ�ʱ��8
	TIM8->CR1&=~(1<<4);//���������ϼ���
}*/








