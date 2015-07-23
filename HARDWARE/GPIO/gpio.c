#include "gpio.h"

//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void GPIO_Init(void)
{
	RCC->APB2ENR|=1<<3;    //ʹ��PORTBʱ��	   	 
	RCC->APB2ENR|=1<<6;    //ʹ��PORTEʱ��
	RCC->APB2ENR|=1<<2;    //ʹ��PORTAʱ��
	RCC->APB2ENR|=1<<4;    //ʹ��PORTCʱ��
	RCC->APB2ENR|=1<<5;    //ʹ��PORTDʱ��	
	
	//X��PWM�����������
	GPIOA->CRL&=0XFFFFFF0F;	//PA1���
	GPIOA->CRL|=0X30;  //�������
	//GPIOA->ODR|=1<<1;
	
	//Y��PWM�������
	GPIOA->CRH&=0xFFFFFFF0;//����PA8
	GPIOA->CRH|=0x3;  //�������
	GPIOA->ODR|=1<<8;;//PA8����
	
// 	//X���ӦPC6 Y���ӦPC7
// 	GPIOC->CRL&=0x00FFFFFF;
// 	GPIOC->CRL|=0x33000000;

// 	//Z��PWM�����������
// 	GPIOB->CRL&=0x0FFFFFFF;
// 	GPIOB->CRL|=0xB0000000;//PB7�������
// 	GPIOB->ODR|=1<<7;
	//Z��PWM�����������
	GPIOB->CRL&=0x0FFFFFFF;
	GPIOB->CRL|=0x30000000;//PB7   ���
	GPIOB->ODR|=1<<7;


	GPIOB->CRL&=0XFF0FFFFF; 
	GPIOB->CRL|=0X00300000;//PB.5 �������   	 
  GPIOB->ODR|=1<<5;      //PB.5 �����
											  
	GPIOE->CRL&=0XFF0FFFFF;
	GPIOE->CRL|=0X00300000;//PE.5�������
	GPIOE->ODR|=1<<5;      //PE.5����� 

   //ɳ©���ٵ����������
   GPIOE->CRH&=0xFFFF0000;
   GPIOE->CRH|=0x00003333;

	//����
	GPIOD->CRL&=0xFFFF000F;
	GPIOD->CRL|=0x00003330;//PD1 PD2 PD3 �������
	GPIOD->ODR|=1<<1;
	GPIOD->ODR|=1<<2;
	GPIOD->ODR|=1<<3;

	//��λ����	  PC0��PC5
	GPIOC->CRL&=0xFF000000;//��������
	GPIOC->CRL|=0x00888888;


	//����������
//	GPIOA->CRL&=0x00FFFFFF;
//	GPIOA->CRL|=0x88000000;//TIM3 CH1 CH2����Ϊ����		PA6 PA7
// GPIOB->CRL&=0x00FFFFFF;//TIM4 CH1 CH2 ����Ϊ����	PB6 PB7
//	GPIOB->CRL|=0x88000000;
	//GPIOC->CRL&=0x00FFFFFF;//TIM8 CH1 CH2 ����Ϊ����  PC6 PC7
	//GPIOC->CRL|=0x88000000;

}

// void LASER_Init(void)//����У׼��ʼ��
// {
// 	//PD5���룬PD4���
// 	GPIOD->CRL&=0xFF0FFFFF;	//PD5����
// 	GPIOD->CRL|=0x00800000;
// 	GPIOD->CRL&=0xFFF0FFFF;//PD4����Ϊ���
// 	GPIOD->CRL|=0x00030000;
// }	




