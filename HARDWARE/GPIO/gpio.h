#ifndef __GPIO_H
#define __GPIO_H	 
#include "sys.h"
 

//LED端口定义
#define LED0 PBout(5)// DS0
#define LED1 PEout(5)// DS1	
#define DIR_Y PDout(1)//PD1
#define DIR_X PDout(2)//PD2
#define DIR_Z PDout(3)//PD3	方向输出
#define LIMITER_Y_1 PCin(0)//PC0 x轴的限位开关输入
#define LIMITER_Y_2 PCin(1)//pc1
#define	LIMITER_X_1 PCin(2)//pc2
#define LIMITER_X_2 PCin(3)//pc3
#define LIMITER_Z_1 PCin(4)//PC4
#define LIMITER_Z_2 PCin(5)//PC5
#define PWM_OUT_Y PAout(1)
#define PWM_OUT_X PAout(8)
#define PWM_OUT_Z PBout(7)
#define LASER_OUT   PDout(5) //PD5
#define LASER_IN    PDin(4)   //PD4
void GPIO_Init(void);//初始化
void LASER_Init(void);//激光校准初始化		 				    
#endif

















