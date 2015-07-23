#ifndef _COS_H
#define _COS_H
#include "sys.h"

//平面坐标结构体
struct Point_2D{
	int point_x;
	int point_y;
};
//平面向量结构体
struct My_vector{
	int Vector_x;
	int Vector_y;
};

float InvSqrt(float x);//计算平方根
float Vector_Length(struct My_vector *mv);//计算向量长度
float Cos_vector(struct My_vector *mv1,struct My_vector *mv2);//计算向量夹角余弦
#endif
