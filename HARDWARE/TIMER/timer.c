#include "timer.h"
#include "led.h"
#include "delay.h"
#include "exti.h"

//u16 ENCODER1=0;
u16 Encodervalue1=0;//编码器的值

int PWM_counter2=0;//记录定时器2已产生PWM脉冲个数 

int PWM_counter1=0;//记录定时器1已经产生的PWM脉冲个数
int PWM_counter4=0;

u16 ARR1[21]={1500,900,450,300,225,150,90,75,56,50,45,50,56,75,90,150,225,300,450,900,1500};

int PWM_counter_part1[21]={0};//计时器1的5个阶段所需输出的PWM脉冲个数
u16 ARR2[21]={1500,900,450,300,225,150,90,75,56,50,45,50,56,75,90,150,225,300,450,900,1500};
u16 ARR4[21]={1500,900,450,300,225,150,90,75,56,50,45,50,56,75,90,150,225,300,450,900,1500};
int PWM_counter_part2[21]={0};
int PWM_counter_part4[21]={0};
u16 index1=0; //数组指针,同时表明当前计数器输出处于那一阶段
u16 index2=0;
u16 index4=0;
u8 Original_Dir1=0;
u8 Original_Dir2=0;//用于表明一次移动的最初方向
u16 TIM2_CH=0;	  //判断通道
u16 TIM1_finish=0; //TIM1完成标志
u16 TIM2_finish=0;//TIM2完成标志
u16 TIM4_finish=0;//TIM4完成标志
u16 BUCHANG1=0;
u16 BUCHANG2=0;
float encoder1[21]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//记录定时器1三个监测时刻的编码器需要记录的值
float PWM_to_Encoder=0.04;//将PWM脉冲转换为编码器计数值，25600/r	 转动一周编码器计数为1024

/*
	定时器2的中断服务函数
*/
void TIM2_IRQHandler(void)
{ 		    		 			    
	if(TIM2->SR&0X0001)//溢出中断
	{	
		if((LIMITER_Z_1==0)||(LIMITER_Z_2==0))
	 	{
		 	delay_ms(1);//消除抖动
			if((LIMITER_Z_1==0)||(LIMITER_Z_2==0))
			{	
				LED1=0;
				TIM4_CH2_PWM_VAL=0;
				TIM2->CR1&=0xFFFE;	//关闭定时器2
				PWM_counter2=0;//计数变量清零
				index2=0;
			} 
		}
		else
		{
			PWM_counter2++;		  //脉冲个数加1
			if(PWM_counter2>=PWM_counter_part2[index2])//判断index阶段是否已产生足够的PWM脉冲
			{
					index2++;
				if(index2>20) //已完成全部输出
				{
					
							TIM2_CH2_PWM_VAL=0;
							TIM2->CR1&=0xFFFE;	//关闭定时器1
							PWM_counter2=0;
							index2=0;
							TIM2_finish=1;
				}
				else
				{
				
					while(PWM_counter_part2[index2]==0)	//下一阶段PWM脉冲数为零则跳过
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
	TIM2->SR&=~(1<<0);//清除中断标志位 	    
}

/*
	定时器1的中断服务函数
*/
void TIM1_UP_IRQHandler(void)
{
	 if(TIM1->SR&0X0001)//溢出中断
	 {	
		if(LIMITER_X_1==0||LIMITER_X_2==0)
		{
			delay_ms(1);//消除抖动
			if(LIMITER_X_1==0||LIMITER_X_2==0)
			{
				LED1=0;
				TIM1_CH1_PWM_VAL=0;	
				TIM1->CR1&=0xFFFE;	//关闭定时器1
				PWM_counter1=0;
				index1=0;
			}
				
		}
		else
		{
			PWM_counter1++;
			if(PWM_counter1>=PWM_counter_part1[index1])//如果index阶段已产生足够的PWM脉冲则进入下一阶段
			{
				PWM_counter_part1[index1]=0;																			 
				index1++;
				 Encodervalue1=TIM3->CNT;
				// Encodervalue1=encoder1[index1-1];
				if(Encodervalue1>32767)
				{
					Encodervalue1=65535-Encodervalue1+1;
				}
				if(index1>20) //已完成输出
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
							TIM1->CR1&=0xFFFE;	//关闭定时器1
							PWM_counter1=0;
							index1=0;
							TIM1_finish=1;
						}	
					}
				/*	else if(encoder1[index1-1]<Encodervalue1)	  //暂时不考虑过冲的最后补偿
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
							TIM1->CR1&=0xFFFE;	//关闭定时器1
						//	PWM_test=PWM_test+PWM_counter1;//调试用
							PWM_counter1=0;
							//	PWM_counter_need1=0;//计数变量清零
							//TIM3->CNT=0;
							index1=0;
							TIM1_finish=1;
						//	RUN=2;	
						}
					}*/
					else
					{
						TIM1_CH1_PWM_VAL=0;	
						TIM1->CR1&=0xFFFE;	//关闭定时器1
						PWM_counter1=0;
						index1=0;
						TIM1_finish=1;
					}
				}
				else
				{
					while(PWM_counter_part1[index1]==0)	//下一阶段PWM脉冲数为零则跳过
					{
						index1++;
					}
					if(encoder1[index1-1]>Encodervalue1) //出现失步
					{
						BUCHANG1=encoder1[index1-1]-Encodervalue1;
						if(BUCHANG1>0)
						{
							PWM_counter_part1[index1]=PWM_counter_part1[index1]+BUCHANG1/PWM_to_Encoder;
						}
					}
					if(encoder1[index1-1]<Encodervalue1)//出现过冲
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
	TIM1->SR&=~(1<<0);//清楚中断标志位
}

/*
	定时器4终端服务函数
*/
void TIM4_IRQHandler(void)
{ 		    		 			    
	if(TIM4->SR&0X0001)//溢出中断
	{	
		if((LIMITER_Z_1==0)||(LIMITER_Z_2==0))
	 	{
		 	delay_ms(1);//消除抖动
			if((LIMITER_Z_1==0)||(LIMITER_Z_2==0))
			{	
				LED1=0;
				TIM4_CH2_PWM_VAL=0;
				TIM4->CR1&=0xFFFE;	//关闭定时器2
				PWM_counter4=0;//计数变量清零
				index4=0;
			} 
		}
		else
		{
			PWM_counter4++;		  //脉冲个数加1
			if(PWM_counter4>=PWM_counter_part4[index4])//判断index阶段是否已产生足够的PWM脉冲
			{
					index4++;
				if(index4>20) //已完成全部输出
				{
					
							TIM4_CH2_PWM_VAL=0;
							TIM4->CR1&=0xFFFE;	//关闭定时器1
							PWM_counter4=0;
							index4=0;
							TIM4_finish=1;
				}
				else
				{
				
					while(PWM_counter_part4[index4]==0)	//下一阶段PWM脉冲数为零则跳过
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
	TIM4->SR&=~(1<<0);//清除中断标志位 	    
}

//TIM2定时器PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void PWM2_CH2_Init(u16 arr,u16 psc,u16 p_c_n)
{		 					 
	//此部分需手动修改IO口设置
	RCC->APB1ENR|=1<<0;       //TIM2时钟使能
	GPIOA->CRL&=0XFFFFFF0F;	//PA1输出
	GPIOA->CRL|=0XB0;  //复用功能输出
	GPIOA->ODR|=1<<1;  //PA1上拉
	TIM2->ARR=arr;//设定计数器自动重装值 
	TIM2->PSC=psc;//预分频器不分频
	TIM2_CH2_PWM_VAL=arr/2;
	//CCMR1	OC2M置为111，选择模式2
	TIM2->CCMR1|=7<<12;  //CH2 PWM2模式		 
	TIM2->CCMR1|=1<<11; //CH2预装载使能	   
	TIM2->CCER|=1<<4;   //OC2 输出使能
		
	// 9:8 定时器时钟频率CK_INT Tdts=Tck_int
	// 7 ARPE：自动重装载预装载允许位  
	//6:5 00:边沿对齐模式
	// 4 DIR：0计数器向上计数
	// 3 OPM:单脉冲模式 0：在发生更新事件时，计数器不停止
	//2  URS:0允许更新中断
	// 1 UDIS: 0允许UEV
	// 0 CEN:1开启计数器
	TIM2->CR1=0x8000;   //ARPE使能 
	TIM2->CR1|=0x01;    //使能定时器2 
	TIM2->DIER|=1<<0;   //允许更新中断				
	TIM2->DIER|=1<<6;   //允许触发中断		  							    
  	MY_NVIC_Init(2,3,TIM2_IRQChannel,2);//抢占2，子优先级3，组2	
	index2=0;
	TIM2_finish=0;
	TIM2_CH=2;
//	TIM4->CNT=0;										  
}  	 

/*void PWM2_CH3_Init(u16 arr,u16 psc,u16 p_c_n)
{
	RCC->APB1ENR|=1<<0;       //TIM2时钟使能  	  	
	GPIOA->CRL&=0xFFFFF0FF;	 //PA2
	GPIOA->CRL|=0xB00;		 //PA2复用输出
	//GPIOA->ODR|=1<<2;
	TIM2->ARR=arr;//设定计数器自动重装值 
	TIM2->PSC=psc;//预分频器不分频
	TIM2_CH3_PWM_VAL=arr/2;
		
	TIM2->CCMR2|=7<<4;  //CH3 PWM2模式		 
	TIM2->CCMR2|=1<<3; //CH3预装载使能	   
//	TIM2->CCMR2&=0xFFFC; //CH3配置为输出
	TIM2->CCER|=1<<8;   //OC3 输出使能	
	TIM2->DIER|=1<<0;   
	TIM2->DIER|=1<<6;
	TIM2->CR1=0x0080;   //ARPE使能 
	TIM2->CR1|=0x01;    //使能定时器2	
	TIM2->EGR|=0x01;
	delay_us(20);
	TIM2->EGR&=0xFFFE;
	MY_NVIC_Init(1,3,TIM2_IRQChannel,2);//抢占1，子优先级3，组2
	index2=0;
	TIM2_finish=0;
	TIM2_CH=3;
//	TIM8->CNT=0;
} */ 

/*
	初始化函数,初始化TIM1
*/
void PWM1_CH1_Init(u16 arr,u16 psc,u16 p_c_n)
{
	RCC->APB2ENR|=1<<11;//定时器TIM1使能
	RCC->APB2ENR|=1<<2;//GPIOA 时钟时能
	GPIOA->CRH&=0xFFFFFFF0;//配置PA8
	GPIOA->CRH|=0xB;  //复用功能推完输出
	GPIOA->ODR|=1<<8;;//PA8上拉
	TIM1->ARR=arr;//设置计数器自动重装载
	TIM1->PSC=psc;//预分频
	TIM1_CH1_PWM_VAL=arr/2;
	TIM1->CCMR1|=7<<4;//CH1_PWM2模式
	TIM1->CCMR1|=1<<3;//CH1预装载使能
	TIM1->CCER|=1<<0;//OC1输出使能
	TIM1->BDTR|=0x8000;
	TIM1->CR1|=0x1;//使能定时器1
	TIM1->CR1&=0xFFFD;//允许更新中断
	//TIM1->CR1&=0xFF7F; //APRE=0时，预装载寄存器的内容可以随时传送到影子寄存器,TIM1_ARR寄存器没有缓冲
	TIM1->DIER|=1<<0;//允许更新中断
	TIM1->DIER|=1<<6;//	允许触发中断
	MY_NVIC_Init(2,2,TIM1_UP_IRQChannel,2);//抢占1，子优先级2，组2
	index1=0;
	TIM1_finish=0;
	TIM3->CNT=0;
}

/*
	初始化定时器3，用于记录编码器的值，对应定时器1的PWM输出
*/
void TIM3_CNT_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;//TIM3时钟使能
	RCC->APB2ENR|=1<<2;//使能PORTA时钟
	GPIOA->CRL&=0x00FFFFFF;//
	GPIOA->CRL|=0x44000000;//PA6 PA7设置为输入
	TIM3->ARR=arr;//设计计数器自动装填
	TIM3->PSC=psc;//预分频器
	TIM3->SMCR&=0xFFF8;
	TIM3->SMCR|=0x0003;//编码器模式，T2、T1边沿计数

	//位7:4定义TI1输入的采样频率,0001采样频率fSAMPLING=fCK_INT,N=8
	TIM3->CCMR1|=0x0011;//CC1通道被配置为输入，IC1映射在TI1上

	//CC1通道配置为输入，0：不反相：捕获发生在IC1上升沿
	TIM3->CCER&=~(1<<1);
	TIM3->CCER&=~(1<<5);
	TIM3->CCER|=1<<0;//允许计数器
	TIM3->CR1|=0x01;//使能定时器3
	TIM3->CR1&=~(1<<4);
}

/*
	初始化定时器4,定时器4用于记录编码器的值，对应定时器2的PWM输出
*/
/*
void TIM4_CNT_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<2;//TIM4时钟使能
	TIM4->ARR=arr;
	TIM4->PSC=psc;
	//TIM4->DIER|=1<<0;//允许更新中断
	GPIOB->CRL&=0x00FFFFFF;
	GPIOB->CRL|=0x44000000;//PB6,PB7设置为输入使能
	TIM4->SMCR&=0xFFF8;
	TIM4->SMCR|=0x0003;//编码器模式，T2、T1边沿计数
	//位7:4定义TI1输入的采样频率,0001采样频率fSAMPLING=fCK_INT,N=8
	TIM4->CCMR1|=0x0011;//cc1通道被配置为输入，IC1映射在TI1上

	//cc1通道配置为输入，0：不反相:捕获发生在IC1上升沿
	TIM4->CCER&=~(1<<1);
	TIM4->CCER&=~(1<<5);
	TIM4->CCER|=1<<0;//允许计数器
	TIM4->CR1|=0x01;//使能定时器4
}*/
 
 //初始化定时器4 ch2作为Z轴输出PWM
void PWM4_CH2_Init(u16 arr,u16 psc,u16 p_c_n)
{
	RCC->APB1ENR|=1<<2;//TIM4时钟使能
	GPIOB->CRL&=0x0FFFFFFF;
	GPIOB->CRL|=0xB0000000;//PB7复用输出
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
	初始化定时器TIM8,TIM8作为计数器记录
*/
/*void TIM8_CNT_Init(u16 arr,u16 psc)
{
	RCC->APB2ENR|=1<<13;//定时器8使能
	GPIOC->CRL&=0x00FFFFFF;
	GPIOC->CRL|=0x44000000;	//PC6 PC7 设置为输入使能
	TIM8->ARR=arr;
	TIM8->PSC=psc;
	TIM8->SMCR&=0xFFFD;
	TIM8->SMCR|=0x0002;

	//位[1:0]CC1通道被配置为输入，IC1映射在TI1上
	//位[3:2]无预分频器，捕获输入口上检测到的每一个边沿都触发一次捕获；
	TIM8->CCMR1&=0xFF00;
	TIM8->CCMR1|=0x0011;

	TIM8->CCER&=~(1<<1);//不反相
	TIM8->CCER&=~(1<<5);
	TIM8->CCER|=1<<0;
	TIM8->CR1|=0x01;//使能定时器8
	TIM8->CR1&=~(1<<4);//计数器向上计数
}*/








