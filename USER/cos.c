#include "cos.h"

/*
	����ƽ�������������ƽ������
*/
float InvSqrt(float x) { 
    float xhalf = 0.5f*x;
    int i = *(int*)&x;        // get bits for floating VALUE
    i = 0x5f375a86- (i>>1);          // gives initial guess y0
    x = *(float*)&i;// convert bits BACK to
    x = x*(1.5f-xhalf*x*x);      // Newton step, repeating increases accuracy
	x=1.0/x;
    return x; 
}

/*
	������������
	���������ṹ��ָ��
*/
float Vector_Length(struct My_vector *mv)
{
	float length= (mv->Vector_x*mv->Vector_x+ mv->Vector_y*mv->Vector_y)*1.0;
	length=InvSqrt(length);
	return length;
}

/*
	���������ļн����ң��������������ṹ��ָ��
*/
float Cos_vector(struct My_vector *mv1,struct My_vector *mv2)
{
	float Length1=0.0,Length2=0.0;
	float cos=0.0;
	Length1=Vector_Length(mv1);
	Length2=Vector_Length(mv2);
	if(Length1>0&&Length2>0)
	{
		cos=(mv1->Vector_x*mv2->Vector_x + mv1->Vector_y*mv2->Vector_y)/(Length1*Length2);
	}
	return cos;
}
