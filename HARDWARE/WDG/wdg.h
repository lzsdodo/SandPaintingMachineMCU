#ifndef __WDG_H
#define __WDG_H
#include "sys.h"

//V1.1 20120903
//增加了窗口看门狗相关函数。									  
////////////////////////////////////////////////////////////////////////////////// 

void IWDG_Init(u8 prer,u16 rlr);
void IWDG_Feed(void);
void WWDG_Init(u8 tr,u8 wr,u8 fprer);
void WWDG_Set_Counter(u8 cnt);
#endif




























