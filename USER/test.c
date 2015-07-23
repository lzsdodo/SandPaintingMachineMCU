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
	该函数主要用于完成硬件的初始化
*/
void Hardware_Init()
{
	uart_init(72,115200);			//串口初始化 
	uart2_init(36,115200);
	GPIO_Init();		  	 			//初始化与LED连接的硬件接口
	EXTIX_Init(); 						//外部中断初始化
	KEY_Init();								//按键初始化
	BEEP_Init();
	TIM5_Int_Init(1500,71);		//初始化定时器5
														//定时器3初始化，角度200
	PWM3_CH2_Init(2000,719);
														//确保沙漏初始状态为关
	DIR_shalou=0;							//关
	TIM2_Int_Init(1000,71);
}

//该函数完成系统原点位置的初始化
void Go_to_origin()
{
	const int PWM_number_Y = 5115;	//离开限位开关2cm   一毫米对应341
	const int PWM_number_X = 10230;	//离开限位开关3cm
	const int PWM_number_Z = 30690;	//离开限位开关9cm
	int i = 0;
	u32 speed_y = 50;								//控制X轴的速度
	
	//z轴移动到原点
	DIR_Z = RETREAT;
	while(1)
	{
		if( LIMITER_Z_2==0 )	 				//远离电机的限位开关为2
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
	
	//Y轴移动到原点
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
	
	//X轴移动到原点
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

//完成扫沙_舵机控制
void clean()															//TIM4_CH2_PWM_VAL=100 200
{
	const int Z_MOVE_PWM_NUMBER_UP   = 13640;	//z轴移动设定的距离所需输出的脉冲个数4CM
	const int Z_MOVE_PWM_NUMBER_DOWN = 8525;	//z轴移动设定的距离所需输出的脉冲个数2.5CM
	const int Y_MOVE_PWM_NUMBER = 126170;			//X轴移动距离,37cm
	const int X_MOVE_PWM_NUMBER = 17050;			//Y轴移动距离,5cm
	int i,j;
	u32 speed_go   = 26;
	u32 speed_back = 15;
	
	//1.1 若不在原点则回到原点,然后开始扫沙
	if((Position_x!=0)||(Position_y!=0)||(Position_z!=0))		
	{
 		Go_to_origin();
	}
	//1.2 校准完z轴上移4cm
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
	//1.3 第一列不扫
	DIR_X = ADVANCE;
	for( j=0; j<(X_MOVE_PWM_NUMBER+6820); j++ )
		{
			PWM_OUT_X = (!PWM_OUT_X);
			delay_us(30);
			PWM_OUT_X = (!PWM_OUT_X);
			delay_us(30);
		}
	//1.4 刷子放下
	TIM3_CH2_PWM_VAL = 170;
		
	//2.0 ********扫沙循环**********
	for( i=0; i<8; i++ )
	{	
			speed_go   = 26;				//前进速度参考值
			speed_back = 15;			//返回速度参考值
		
			//2.1 z轴放下2.5cm
			DIR_Z = ADVANCE;				
			for( j=0; j<Z_MOVE_PWM_NUMBER_DOWN; j++ )
			{
					PWM_OUT_Z = (!PWM_OUT_Z);
					delay_us(40);
					PWM_OUT_Z = (!PWM_OUT_Z);
					delay_us(40);
			}
			
			//2.2 扫一列
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
			
			//2.3 z轴上移2.5cm
			DIR_Z = RETREAT;
			for( j=0; j<Z_MOVE_PWM_NUMBER_DOWN; j++ )
			{
					PWM_OUT_Z = (!PWM_OUT_Z);
					delay_us(40);
					PWM_OUT_Z = (!PWM_OUT_Z);
					delay_us(40);
			}
			
			//2.4 回到Y=0处,刷子返回
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
			
			//2.5 X轴移动一步
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
		
		//3.0 扫完刷子抬起
		TIM3_CH2_PWM_VAL=130;
		
		//4.0 返回原点
		Go_to_origin();

}
/*
	PWM输出引脚为PA1（定时器2 CH2 Y轴）,PA2（TIM2 Ch3 Z轴），PA8(定时器1 CH1 X轴)
	舵机定时器3，PA7输出
	方向： DIR_X PD1 ; DIR_Y PD2 ; DIR_Z PD3
	编码器： X PC0 PC1(对应TIM3 CH1和CH2) ; Y PC2 PC3 (对应TIM4 CH1和CH2) ; Z PC4 和 PC5 (PC0-PC5对应6、5、2、1、4、3)
	激光校准：PD4 PD5;
*/
int main(void)
{	
	u16 result = 0;
	u16 length = 0;
	u16 i = 0;
	u16 times  = 0;
	u16 flag_timer5 = 1;									//定时器5开关标志 0表示关 1表示开
	const u16 forecast_length = 10;				//表示向前预测多少个点
	struct Coordinate c_point1 = {0,0,0};	//起始点(0,0,0)
	Stm32_Clock_Init(9); 									//系统时钟设置
	delay_init(72);	     									//延时初始化
	Hardware_Init();											//硬件初始化

	//开始通电时开发板可能会重启，此时不稳定，延时一段时间接收
	while( i<4 )
	{
	  delay_ms(1000);
		LED0=!LED0;
		LED1=!LED1;
		i++;
	}	  
	LED0 = 1;												//LED0和LED1作指示灯,初始状态都灭
	LED1 = 1;
	Cq_insert( &c_point1 );					//首先插入起始坐标（0,0,0）
 	Go_to_origin();									//设备移动到原点
	Hardware_Init();								//硬件初始化
	
	//主循环
	while( 1 )
	{
			//key2系统开始运行，key3停止
			while( RUN ) 
			{
				LED1 = 0;
				LED0 = 0;
				if( UC_CLEAN_SAND == 6 )				//扫沙标志位 0-不扫沙 6-扫沙	
				{
					clean();											//扫沙函数
					UC_CLEAN_SAND = 0;						//扫沙结束，标志位-UC_CLEAN_SAND置0
				}
				if( UC_Stop_flag == 1 )					//发送结束标志位 0-继续 1-结束； 发送结束
				{
					//	printf("关闭定时器5");
						if( flag_timer5 == 1 )			//定时器5打开标志
						{
							if( (Mp_isempty(QJ_IMS_Mp_head,QJ_IMS_Mp_tail)) && (Cq_is_empty()) && (QJ_TIM_finish_xy==1) )
							{			//队列为空 && 坐标队列为空 && XY轴移动已经完成
								TIM_control(5,0);				//表示关闭定时器5
								flag_timer5 = 0;				//定时器5关闭标志
							}
						}
				}
				length = Cq_length();
				if( length<LENGTH_CQ_USED )						//坐标缓存队列不满，则接受下一个坐标
				{
					result = Accept_message();
					
					if( result==0 )
					{
						times++;
					}
				}
				result = Mp_length(QJ_IMS_Mp_head,QJ_IMS_Mp_tail);
				result = MP_LENGTH - result - 1;				//计算MP队列中剩余空间有多大
				printf("result: %d\r\n\r",result);
				if( result>forecast_length )						//X Y轴的缓存队列均剩余空间大于forecast_length
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
						if( flag_timer5==0&&UC_Stop_flag==0 )		//定时器5关闭着，且发送标志位为继续
						{
							TIM_control(5,1);											//表示打开定时器5
							flag_timer5=1;												//定时器已经打开
						}
						if( length>=forecast_length )						//判断坐标缓存队列是否有forecast_length个点
						{	
							Forecast_analysis(forecast_length);
						}
					}
				}
				delay_us(10);
			}
	}
}











/*旧版本扫沙
//完成扫沙
void clean()
{
	const int Z_MOVE_PWM_NUMBER=3410;//z轴移动设定的距离所需输出的脉冲个数,向下移动1CM
	const int Y_MOVE_PWM_NUMBER=126170;//X轴移动距离
	const int X_MOVE_PWM_NUMBER=17050;//Y轴移动距离
	u32 speed=30;
	int i=0,j=0;	
	//Z轴PWM脉冲输出引脚
	GPIOB->CRL&=0x0FFFFFFF;
	GPIOB->CRL|=0x30000000;//PB7推挽输出
	GPIOB->ODR|=1<<7;	
	//Z轴先向下移动
	DIR_Z=RETREAT;
	for(i=0;i<Z_MOVE_PWM_NUMBER;i++)
	{
		PWM_OUT_Z=(!PWM_OUT_Z);
		delay_us(60);
		PWM_OUT_Z=(!PWM_OUT_Z);
		delay_us(60);
	}
	
	if((Position_x!=0)||(Position_y!=0)||(Position_z!=0))//若不在原点则回到原点,然后开始扫沙
	{
		Go_to_origin();
	}
	
	for(i=0;i<8;i++)
	{
		//刷子下降
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
		
		//扫一列
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
		
		//刷子抬起
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
		
		//回到Y=0处,刷子返回
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
		
		//X轴移动一步
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
	
	//返回原点
	Go_to_origin();
}*/