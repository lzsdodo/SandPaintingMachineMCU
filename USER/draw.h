#ifndef _DRAW_H
#define _DRAW_H
#include "sys.h"
#define SIZE 1000
struct Point
{
	float x;
	float y;
};
int Draw_slash(struct Point *now_point,struct Point *next_point,struct Point array[SIZE]);//��б����
int Draw_round(struct Point* now_point,struct Point* next_point,struct Point array[SIZE]);//��Բ��
int Draw_ellipse(struct Point* now_point,struct Point* next_point,struct Point array[SIZE],float m_x,float m_y);//����Բ

#endif
