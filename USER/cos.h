#ifndef _COS_H
#define _COS_H
#include "sys.h"

//ƽ������ṹ��
struct Point_2D{
	int point_x;
	int point_y;
};
//ƽ�������ṹ��
struct My_vector{
	int Vector_x;
	int Vector_y;
};

float InvSqrt(float x);//����ƽ����
float Vector_Length(struct My_vector *mv);//������������
float Cos_vector(struct My_vector *mv1,struct My_vector *mv2);//���������н�����
#endif
