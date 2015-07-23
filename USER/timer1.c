#include "timer1.h"
#include "gpio.h"
#include "delay.h"
#include "beep.h"
#include "exti.h"
#include "improved_motion_strategy.h"

//4位输出 蓝：1 粉：2 黄：3 橙：4
const u16 CCW[8] = {0x08,0x0c,0x4,0x06,0x02,0x03,0x01,0x09};
const u16 Shalou_need_PWM = 512;
volatile u16 index_timer2 = 0;  							//沙漏电机使用
volatile u16 DIR_shalou = 0;									//沙漏电机方向
//volatile u16 TIM2_ARR = 0;									//控制定时器2速率既沙漏流速
//const u16 DIANJI_to_SHALOU = 28;						//电机移动速度与沙漏转速的对应比例
volatile u16 SHOULOU_Change_Speed_flag = 0;		//沙漏变速标志 ，等于1则变速，等于0则不用变速
volatile u16 QJ_TIM_TIM2_finish_shalou = 1;

volatile int PWM_counter_y  = 0;				//记录Y轴已经输出的脉冲个数
volatile int PWM_counter_x  = 0;				//记录x轴以输出的脉冲个数
volatile int PWM_counter_xy = 0;				//用于定时器是否进入下一阶段
volatile int PWM_counter_z  = 0;
volatile int PWM_counter_shalou = 0;
volatile u16 index_xy = 0; 							//数组指针,同时表明当前计数器输出处于那一阶段，Xy轴使用
volatile u16 index_z  = 0;							//Z轴使用，作用同上

volatile u16 QJ_TIM_finish_xy = 1;			//1表示已经完成，0表示未完成
float QJ_TIM_PWM_to_Encoder   = 0.04;		//将PWM脉冲转换为编码器计数值，25600/r	 转动一周编码器计数为1024

volatile u16 QJ_TIM_TIM4_finish_Z = 1;	//Z轴移动完成标志位=0;//1表示z轴（定时器4）已完成移动，0表示z轴未完成移动

//static u16 Encodervalue1=0;						//记录X轴的编码器值 
struct Move_parameter timer5_move_xy;

/*
	这两个标志位用于表明是否继续完成X,Y轴的一个完整的脉冲输出
	在一次中断中只能翻转一次IO口，
*/
volatile u16 flag_x_out = 0;
volatile u16 flag_y_out = 0;

//定时器2中断服务程序	 
void TIM2_IRQHandler(void)
{ 		 
	if(TIM2->SR&0X0001)						//溢出中断
	{
			LED1=!LED1;
		if(PWM_counter_shalou<Shalou_need_PWM)
		{
			if(DIR_shalou==0)					//关沙漏
			{
				GPIOE->ODR&=0xF0FF;
				GPIOE->ODR|=CCW[index_timer2]<<8;
				index_timer2=(index_timer2+1)%8;
			}
			else 											//开沙漏
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
			TIM2->CR1&=0xFFFE;				//关闭定时器2
			PWM_counter_shalou=0;
		}
	}				   
	TIM2->SR&=~(1<<0);						//清除中断标志位 	    
}

///*
//	定时器4终端服务函数
//	负责Z轴的PWM信号输出
//*/
//void TIM4_IRQHandler(void)
//{ 		    	
//	if(TIM4->SR&0X0001)//溢出中断
//	{	
//		if((LIMITER_Z_1==0)||(LIMITER_Z_2==0))
//	 	{
//		 	delay_ms(1);//消除抖动
//			if((LIMITER_Z_1==0)||(LIMITER_Z_2==0))
//			{	
//				LED1=0;
//				TIM4_CH2_PWM_VAL=0;
//				TIM4->CR1&=0xFFFE;	//关闭定时器2
//				PWM_counter_z=0;//计数变量清零
//				index_z=0;
//			} 
//		}
//		else
//		{
//			PWM_counter_z++;		  //脉冲个数加1
//			if(PWM_counter_z>=QJ_IMS_Mp_TIM4_Z.PWM_counter_part[index_z])//判断index阶段是否已产生足够的PWM脉冲
//			{
//				index_z++;
//				if(index_z>=JIASU_STEP) //已完成全部输出
//				{
//					TIM4_CH2_PWM_VAL=0;
//					TIM4->CR1&=0xFFFE;	//关闭定时器4
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
//	TIM4->SR&=~(1<<0);//清除中断标志位 	    
//}

void TIM5_IRQHandler(void)
{
	int i=0,t;
	int F_xy=0;									//记录差值
	u8  check = 0;
	u8  position_data[15] = {0xFF,0xFF,0xFF,0xFF,0x8,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
	
	if(TIM5->SR&0x0001)
	{
		if((LIMITER_X_1==0)||(LIMITER_X_2==0)||(LIMITER_Y_1==0)||(LIMITER_Y_2==0))
	 	{
		 	delay_ms(1);						//消除抖动
			if((LIMITER_X_1==0)||(LIMITER_X_2==0)||(LIMITER_Y_1==0)||(LIMITER_Y_2==0))
			{	
				LED1=0;
				BEEP=0;
				TIM5->CR1&=0xFFFE;		//关闭定时器5
				PWM_counter_x=0;			//计数变量清零
				PWM_counter_y=0;
				index_xy=0;
			} 
		}
		else
		{
			if(QJ_TIM_finish_xy==1)										//XY轴移动已经完成
			{
				if(QJ_IMS_Mp_head!=QJ_IMS_Mp_tail)			//如果队列不为空
				{
					timer5_move_xy.dir_x = QJ_IMS_Mp_queue[QJ_IMS_Mp_head].dir_x;
					timer5_move_xy.dir_y = QJ_IMS_Mp_queue[QJ_IMS_Mp_head].dir_y;
					timer5_move_xy.guaxian = QJ_IMS_Mp_queue[QJ_IMS_Mp_head].guaxian;
					timer5_move_xy.PWM_counter_needed_X = QJ_IMS_Mp_queue[QJ_IMS_Mp_head].PWM_counter_needed_X;
					timer5_move_xy.PWM_counter_needed_Y = QJ_IMS_Mp_queue[QJ_IMS_Mp_head].PWM_counter_needed_Y;
					
					//取出本次坐标信息
					timer5_move_xy.Now_Position_X = QJ_IMS_Mp_queue[QJ_IMS_Mp_head].Now_Position_X;
					timer5_move_xy.Now_Position_Y = QJ_IMS_Mp_queue[QJ_IMS_Mp_head].Now_Position_Y;
					timer5_move_xy.Now_Position_Z = QJ_IMS_Mp_queue[QJ_IMS_Mp_head].Now_Position_Z;				
					//信息保存为数据包
					position_data[TYPE_START] 		 = RIGHT;
					position_data[MESSAGE_START]   = timer5_move_xy.Now_Position_X & 0xFF;
					position_data[MESSAGE_START+1] = timer5_move_xy.Now_Position_X / 256;
					position_data[MESSAGE_START+2] = timer5_move_xy.Now_Position_Y & 0xFF;
					position_data[MESSAGE_START+3] = timer5_move_xy.Now_Position_Y / 256;
					position_data[MESSAGE_START+4] = timer5_move_xy.Now_Position_Z & 0xFF;
					position_data[MESSAGE_START+5] = timer5_move_xy.Now_Position_Z / 256;
					//求检验和
					for( t=6; t<13; t++ )
					{
						check += position_data[t];
					}
					check = ~check;
					position_data[14] = check;
					//发送数据包
					for( t=0; t<15; t++ )
					{
						while( (USART1->SR&0x40)==0 );//等待发送结束
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
			}//XY轴移动已完成,取点
			
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
					
					//如果是在y=x下方 第一卦限
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
							PWM_counter_xy++;//该加速步骤所输出脉冲数加一
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
					}//卦限1
					
					//如果是在y=x上方 第二卦限
					if(timer5_move_xy.guaxian==2)
					{
						F_xy=timer5_move_xy.PWM_counter_needed_X*PWM_counter_y - timer5_move_xy.PWM_counter_needed_Y*PWM_counter_x;
						if(F_xy>=0)
						{
							//在直线上方 x输出一个脉冲 Y输出一个脉冲
							PWM_OUT_X=0;
							PWM_OUT_Y=0;
							PWM_counter_x++;
							PWM_counter_xy++;//该加速步骤所输出脉冲数加一
							PWM_counter_y++;
							flag_x_out=1;
							flag_y_out=1;
						}
						else
						{
							//在直线下方， Y输出一个脉冲
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
					}//卦限2
				}
			}//为XY轴分配脉冲
		}//不到限位开关
	}
	TIM5->SR&=~(1<<0);//清除中断标志位
}

void TIM2_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<0;;	//TIM2时钟使能    
 	TIM2->ARR=arr;  	//设定计数器自动重装值//刚好1ms    
	TIM2->PSC=psc;  	//预分频器7200,得到10Khz的计数时钟		  
	TIM2->DIER|=1<<0;   //允许更新中断	  
	TIM2->CR1|=0x01;    //使能定时器2
	PWM_counter_shalou=0;
	QJ_TIM_TIM2_finish_shalou=0;
  MY_NVIC_Init(2,2,TIM2_IRQChannel,2);//抢占2，子优先级3，组2									 
}

// /*
// 	初始化定时器3，用于记录编码器的值，对应X轴移动
// */
// void TIM3_CNT_Init(u16 arr,u16 psc)
// {
// 	RCC->APB1ENR|=1<<1;//TIM3时钟使能
// 	RCC->APB2ENR|=1<<2;//使能PORTA时钟
// 	GPIOA->CRL&=0x00FFFFFF;//
// 	GPIOA->CRL|=0x44000000;//PA6 PA7设置为输入
// 	TIM3->ARR=arr;//设计计数器自动装填
// 	TIM3->PSC=psc;//预分频器
// 	TIM3->SMCR&=0xFFF8;
// 	TIM3->SMCR|=0x0003;//编码器模式，T2、T1边沿计数

// 	//位7:4定义TI1输入的采样频率,0001采样频率fSAMPLING=fCK_INT,N=8
// 	TIM3->CCMR1|=0x0011;//CC1通道被配置为输入，IC1映射在TI1上

// 	//CC1通道配置为输入，0：不反相：捕获发生在IC1上升沿
// 	TIM3->CCER&=~(1<<1);
// 	TIM3->CCER&=~(1<<5);
// 	TIM3->CCER|=1<<0;//允许计数器
// 	TIM3->CR1|=0x01;//使能定时器3
// 	TIM3->CR1&=~(1<<4);
// }

//初始化定时器3 
void PWM3_CH2_Init(u16 arr,u16 psc)
{
	//Z轴PWM脉冲输出引脚
  //GPIOB->CRL&=0x0FFFFFFF;
	//GPIOB->CRL|=0xB0000000;//PB7复用输出
	//GPIOB->ODR|=1<<7;
	GPIOA->CRL&=0x0FFFFFFF;
	GPIOA->CRL|=0xB0000000;//PA7复用输出
	RCC->APB2ENR|=1<<2;//io端口A时钟开启
	RCC->APB1ENR|=1<<1;//TIM3时钟使能
	//RCC->APB1ENR|=1<<2;//TIM4时钟使能
	TIM3->ARR=arr;//自动装载寄存器
	TIM3->PSC=psc;//预分频寄存器
	TIM3->CCMR1|=7<<12;
	TIM3->CCMR1|=1<<11;
	TIM3->CCER|=1<<4;//开启
	TIM3->CCER|=1<<5;//低电平有效
	//TIM3->CR1|=0x8000;
	//TIM3->CR1|=0x01;
	TIM3->CR1|=0x0080;
	TIM3->CR1|=0x01;
	TIM3_CH2_PWM_VAL=130;//PWM脉冲输出占空比
	//TIM3->DIER|=1<<0;
	//TIM3->DIER|=1<<6;
	//MY_NVIC_Init(2,1,TIM4_IRQChannel,2);
	//index_z=0;
	//PWM_counter_z=0;
	//QJ_TIM_TIM4_finish_Z=0;
	//DIR_Z=QJ_IMS_Mp_TIM4_Z.dir_z;
} 

//定时器5初始化
void TIM5_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<3;
	TIM5->ARR=arr;
	TIM5->PSC=psc;
	TIM5->DIER|=1<<0;
	TIM5->CR1|=0x01;
	MY_NVIC_Init(2,0,TIM5_IRQChannel,2);//抢占优先级1，响应优先级2
	flag_x_out=0;
	flag_y_out=0;
	QJ_TIM_finish_xy=1;
}

//定时器控制函数
void TIM_control(u16 timer, u16 control)
{
	//关闭定时器
	if(control==0)
	{
		switch(timer)
		{
			case 2:
				TIM2->CR1&=0xFFFE;  //关闭定时器2
			break;
			case 3:
				TIM3->CR1&=0xFFFE;  //关闭定时器3
			break;
			case 5:								//关闭定时器5
				TIM5->CR1&=0xFFFE;
			break;
			default:
				break;
		}
	}
	else  //打开定时器
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

