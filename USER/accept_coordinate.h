#ifndef _ACCEPT_COORDINATE_H
#define _ACCEPT_COORDINATE_H
#include "sys.h"
#include "usart_communication.h"
#include "usart.h"

#define LENGTH_COORDINATE_QUEUE 561	//������г�
#define LENGTH_CQ_USED 540   //�������������ʹ�õ���󳤶�
#define MAX_X 400
#define MAX_Y 400
#define MAX_Z 40

//����ṹ�壬����x,y,z����
struct Coordinate{
	u16 NOW_X;
	u16 NOW_Y;
	u16 NOW_Z;
};

extern volatile u16 Position_x;
extern volatile u16 Position_y;
extern volatile u16 Position_z;

extern u16 Cq_head;
extern u16 Cq_tail;


u16 Cq_is_full(void);
u16 Cq_is_empty(void);
u16 Cq_insert(struct Coordinate* com);
u16 Cq_push(struct Coordinate* point);
u16 Cq_length(void);
void Copy_Coordinate(struct Coordinate *c_point1,struct Coordinate *c_point2);
u16 Accept_message(void);
#endif
