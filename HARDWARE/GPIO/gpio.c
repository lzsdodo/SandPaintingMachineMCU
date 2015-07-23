#include "gpio.h"

//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void GPIO_Init(void)
{
	RCC->APB2ENR|=1<<3;    //使能PORTB时钟	   	 
	RCC->APB2ENR|=1<<6;    //使能PORTE时钟
	RCC->APB2ENR|=1<<2;    //使能PORTA时钟
	RCC->APB2ENR|=1<<4;    //使能PORTC时钟
	RCC->APB2ENR|=1<<5;    //使能PORTD时钟	
	
	//X轴PWM脉冲输出引脚
	GPIOA->CRL&=0XFFFFFF0F;	//PA1输出
	GPIOA->CRL|=0X30;  //推完输出
	//GPIOA->ODR|=1<<1;
	
	//Y轴PWM输出引脚
	GPIOA->CRH&=0xFFFFFFF0;//配置PA8
	GPIOA->CRH|=0x3;  //推完输出
	GPIOA->ODR|=1<<8;;//PA8上拉
	
// 	//X轴对应PC6 Y轴对应PC7
// 	GPIOC->CRL&=0x00FFFFFF;
// 	GPIOC->CRL|=0x33000000;

// 	//Z轴PWM脉冲输出引脚
// 	GPIOB->CRL&=0x0FFFFFFF;
// 	GPIOB->CRL|=0xB0000000;//PB7复用输出
// 	GPIOB->ODR|=1<<7;
	//Z轴PWM脉冲输出引脚
	GPIOB->CRL&=0x0FFFFFFF;
	GPIOB->CRL|=0x30000000;//PB7   输出
	GPIOB->ODR|=1<<7;


	GPIOB->CRL&=0XFF0FFFFF; 
	GPIOB->CRL|=0X00300000;//PB.5 推挽输出   	 
  GPIOB->ODR|=1<<5;      //PB.5 输出高
											  
	GPIOE->CRL&=0XFF0FFFFF;
	GPIOE->CRL|=0X00300000;//PE.5推挽输出
	GPIOE->ODR|=1<<5;      //PE.5输出高 

   //沙漏减速电机控制引脚
   GPIOE->CRH&=0xFFFF0000;
   GPIOE->CRH|=0x00003333;

	//方向
	GPIOD->CRL&=0xFFFF000F;
	GPIOD->CRL|=0x00003330;//PD1 PD2 PD3 推挽输出
	GPIOD->ODR|=1<<1;
	GPIOD->ODR|=1<<2;
	GPIOD->ODR|=1<<3;

	//限位开关	  PC0至PC5
	GPIOC->CRL&=0xFF000000;//上拉输入
	GPIOC->CRL|=0x00888888;


	//编码器输入
//	GPIOA->CRL&=0x00FFFFFF;
//	GPIOA->CRL|=0x88000000;//TIM3 CH1 CH2配置为输入		PA6 PA7
// GPIOB->CRL&=0x00FFFFFF;//TIM4 CH1 CH2 配置为输入	PB6 PB7
//	GPIOB->CRL|=0x88000000;
	//GPIOC->CRL&=0x00FFFFFF;//TIM8 CH1 CH2 配置为输入  PC6 PC7
	//GPIOC->CRL|=0x88000000;

}

// void LASER_Init(void)//激光校准初始化
// {
// 	//PD5输入，PD4输出
// 	GPIOD->CRL&=0xFF0FFFFF;	//PD5输入
// 	GPIOD->CRL|=0x00800000;
// 	GPIOD->CRL&=0xFFF0FFFF;//PD4配置为输出
// 	GPIOD->CRL|=0x00030000;
// }	




