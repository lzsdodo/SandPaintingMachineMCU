#include<stdio.h>

#define SIZE 500;

struct Point
{
	int x;
	int y;
};

int Draw_slash(struct Point *now_point,struct Piont *next_point,struct Piont *array)//画斜线
{
	//x_move和y_move表示如果有溢出，x_move和y_move表示将移动到哪一个坐标,rx表示Xe,ry表示Ye,reg_x和reg_y表示余数寄存器。
	int rx=0,reg_x=0,x_move=0,ry=0,reg_y=0,y_move=0;
	int N=8;//表示累加器的值
	int flag_x=0,flag_y=0;//表示x,y向前或向后移动
	int i=0;
	int length=0;//记录存入坐标个数
	int overflow=0;// 保存溢出值
	int times_x=0,times_y=0;//记录x,y的溢出个数
	if(next_point->x > now_point->x)
	{
		flag_x=1;
		rx=next_point->x-now_point->x;
	}
	else
	{
		flag_x=0;
		rx=now_point->x-now_point->x;
	}
	if(next_point->y > now_point->y)
	{
		flag_y=1;
		ry=next_point->y-now_point->y;
	}
	else
	{
		flag_y=0;
		ry=now_point->y - next_point->y;
	}
	for(i=16384;i>1;i=i/2)
	{
		if((i&next_point->x)>0||(i&next_point->y)>0)
		{
			N=i*2;
			break;
		}
	}
	reg_x=N/2;//半加载
	reg_y=N/2;
	do
	{
		if(times_x<=rx)
		{
			reg_x=reg_x+rx;
			overflow=reg_x/N;
			if(overflow>0)
			{
				++times_x;
				if(flag_x>0)
					array[length].x=now_point->x+overflow;
				else
					array[length].x=now_point->x-overflow;
				array[length].y=0;
				length++;
				reg_x=reg_x%N;
			}
		if(times_y<=ry)
		{
			reg_y=reg_y+ry;
			overflow=reg_y/N;
			if(overflow>0)
			{
				++times_y;
				if(flag_y>0)
					array[length].y=now_point->y+overflow;
				else
					array[length].y=now_point->y-overflow;
				reg_y=reg_y%N;
				array[length].x=0;
				length++;
			}
		}
	}while((array[length].x!=next_point->x)||(array[length].y!=next_point->y));
		return length;
}
int Draw_round(struct Piont* p1,struct Piont* p2,int* array)//画圆弧
{
		
}
void Show_point(struct Point *p1)
{
	printf("X:%d \n",p1->x);
	printf("Y:%d \n",p1->y);
}
int main()
{
	int step_number;//画斜线或圆所需的步数
	int i=0;
	struct Point Draw_point[SIZE]={{0,0}};
	struct Point slash[2]={{0,0},{0,0}};
	slash[1].x=5;
	slash[1].y=5;
//	step_number=Draw_slash(slash+1,slash+2,Draw_point);
//	step_number=Draw_round(slash)
	for(i=0;i<step_number;i++)
	{
		Show_point(Draw_point+i);
	}
	return 0;
}
