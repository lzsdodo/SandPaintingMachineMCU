#include<stdio.h>

#define SIZE 1000

struct Point
{
	float x;
	float y;
};

int Draw_slash(struct Point *now_point,struct Point *next_point,struct Point array[SIZE])//画斜线
{
	//x_move和y_move表示如果有溢出，x_move和y_move表示将移动到哪一个坐标,rx表示Xe,ry表示Ye,reg_x和reg_y表示余数寄存器。
	float rx=0,reg_x=0,ry=0,reg_y=0;
	int N=8,tem_n;//表示累加器的值
	int flag_x=0,flag_y=0;//表示x,y向前或向后移动
	int i=0;
	int length=0;//记录存入坐标个数
	int overflow=0;// 保存溢出值
	int times_x=0,times_y=0;//记录x,y的溢出个数
	float x_tem=now_point->x,y_tem=now_point->y;
	if(now_point==NULL||next_point==NULL)
	{
		return 0;
	}
	if(next_point->x > now_point->x)
	{
		flag_x=1;
		rx=next_point->x - now_point->x;
	}
	else
	{
		flag_x=0;
		rx=now_point->x - next_point->x;
	}
	if(next_point->y > now_point->y)
	{
		flag_y=1;
		ry=next_point->y - now_point->y;
	}
	else
	{
		flag_y=0;
		ry=now_point->y - next_point->y;
	}
	if(rx>ry)
	{
		tem_n=rx;
	}
	else
	{tem_n=ry;}
	rx=0;
	for(i=16384;i>1;i=i/2)
	{
		if((tem_n&i)>0)
		{
			N=i*2;
			break;
		}
	}
//	printf("BBBBB\n");
//	printf("N: %d\n",N);
	reg_x=N/2;//半加载
	reg_y=N/2;
	do
	{
		if(times_x<rx)
		{
			reg_x=reg_x+rx;
			overflow=reg_x/N;
			if(overflow>1)
			{
				++times_x;
				if(flag_x>0)
				{
					array[length].x=x_tem+overflow;
				}
				else
				{
					array[length].x=x_tem-overflow;
				}
				x_tem=array[length].x;
				array[length].y=y_tem;
				length++;
				reg_x=reg_x-N;
			//	printf("CCCCC\n");
			//	printf("times_x: %d\n",times_x);
			}
		}
		if(times_y<ry)
		{
			reg_y=reg_y+ry;
			overflow=reg_y/N;
			if(overflow>1)
			{
				++times_y;
				if(flag_y>0)
					array[length].y=y_tem+overflow;
				else
					array[length].y=y_tem-overflow;
				reg_y=reg_y-N;
				y_tem=array[length].y;
				array[length].x=x_tem;
				length++;
			//	printf("CCCCC\n");
			//	printf("times_y: %d\n",times_y);
			}
		}
		if(length>=SIZE)
		{
			break;
		}
	}while(times_x<rx||times_y<ry);
	return length;
}

int Draw_round(struct Point* now_point,struct Point* next_point,struct Point array[SIZE])//画圆弧
{
	//rx表示Xe,ry表示Ye,reg_x和reg_y表示余数寄存器。
	float rx=0,reg_x=0,ry=0,reg_y=0;
	int N=8,tem_n=0;//表示累加器的值
	int flag_x=0,flag_y=0;//表示x,y向前或向后移动
	int i=0;
	int length=0;//记录存入坐标个数
	int overflow_x=0,overflow_y=0;// 保存溢出值
	int times_x=0,times_y=0;//记录x,y的溢出个数
	float x_tem=now_point->x,y_tem=now_point->y;
	int tem_rx=0,tem_ry=0;//保存坐标差值
	if(now_point==NULL||next_point==NULL)
	{
		return 0;
	}
	if(next_point->x > now_point->x)
	{
		flag_x=1;
		tem_rx=next_point->x - now_point->x;
	}
	else
	{
		flag_x=0;
		tem_rx=now_point->x - next_point->x;
	}
	if(next_point->y > now_point->y)
	{
		flag_y=1;
		ry=next_point->y - now_point->y;
	}
	else
	{
		flag_y=0;
		ry=now_point->y - next_point->y;
	}
	tem_ry=ry;
	rx=0;
	if(tem_rx>tem_ry)
	{
		tem_n=tem_rx;
	}
	else
	{tem_n=tem_ry;}
	rx=0;
	for(i=16384;i>1;i=i/2)
	{
		if((tem_n&i)>0)
		{
			N=i*2;
			break;
		}
	}
//	printf("BBBBB\n");
//	printf("N: %d\n",N);
	reg_x=0;
	reg_y=ry;
	do
	{
		if(times_y<tem_ry)
		{
			reg_y=reg_y+ry;
			overflow_y=reg_y/N;
			if(overflow_y>=1)
			{
				++times_y;
				overflow_y=1;
				if(flag_y>0)
				{
					array[length].y=y_tem+overflow_y;	
				}
				else
				{
					array[length].y=y_tem-overflow_y;
				}
				reg_y=reg_y-N;
				y_tem=array[length].y;
			//	array[length].x=x_tem;
				//length++;
			//	rx++;
			//	printf("CCCCC\n");
			//	printf("times_y: %d\n",times_y);
			//	printf("rx %d\n",rx);
			}
			else
			{
				array[length].y=y_tem;
				overflow_y=0;
			}
		}
		else
		{
			array[length].y=y_tem;
			overflow_y=0;
		}
	//	overflow=0;
		if(times_x<tem_rx)
		{
			reg_x=reg_x+rx;
		//	printf("CCCCC\n");
		//	printf("reg_x %d\n",reg_x);
			overflow_x=reg_x/N;
			if(overflow_x>=1)
			{
				++times_x;
				overflow_x=1;
				if(flag_x>0)
				{
					array[length].x=x_tem+overflow_x;
				}
				else
				{
					array[length].x=x_tem-overflow_x;
				}
				x_tem=array[length].x;
			//	array[length].y=y_tem;
			//	length++;
				reg_x=reg_x-N;
			//	printf("times_x: %d\n",times_x);
		//		printf("reg_x %d\n",reg_x);
			}
			else
			{
				array[length].x=x_tem;
				overflow_x=0;
			}
		}
		else
		{
			array[length].x=x_tem;
			overflow_x=0;
		}
		length++;
		ry=ry-overflow_x;
		rx=rx+overflow_y;
		if(length>=SIZE)
		{
			break;
		}
	}while(times_x<tem_rx||times_y<tem_ry);
	return length;	
}

int Draw_ellipse(struct Point* now_point,struct Point* next_point,struct Point array[SIZE],float m_x,float m_y)//画椭圆，m=(a*a)/(b*b)
{
	//rx表示Xe,ry表示Ye,reg_x和reg_y表示余数寄存器。
	float rx=0,reg_x=0,x_move=0,ry=0,reg_y=0,y_move=0;
	int N=8,tem_n=0;//表示累加器的值
	int flag_x=0,flag_y=0;//表示x,y向前或向后移动
	int i=0;
	int length=0;//记录存入坐标个数
	int overflow_x=0,overflow_y=0;// 保存溢出值
	int times_x=0,times_y=0;//记录x,y的溢出个数
	float x_tem=now_point->x,y_tem=now_point->y;
	int tem_rx=0,tem_ry=0;
	if(now_point==NULL||next_point==NULL)
	{
		return 0;
	}
	if(next_point->x > now_point->x)
	{
		flag_x=1;
		tem_rx=next_point->x - now_point->x;
	}
	else
	{
		flag_x=0;
		tem_rx=now_point->x - next_point->x;
	}
		
	if(next_point->y > now_point->y)
	{
		flag_y=1;
		ry=next_point->y - now_point->y;
	}
	else
	{
		flag_y=0;
		ry=now_point->y - next_point->y;
	}
	tem_ry=ry;
	if(tem_rx>tem_ry)
	{
		tem_n=tem_rx;
	}
	else
	{tem_n=tem_ry;}
	rx=0;
	for(i=16384;i>1;i=i/2)
	{
		if((tem_n&i)>0)
		{
			N=i*2;
			break;
		}
	}
//	printf("BBBBB\n");
//	printf("N: %d\n",N);
	reg_x=0;
	reg_y=tem_ry;
	
	do
	{
		if(times_y<tem_ry)
		{
			reg_y=reg_y+ry;
			overflow_y=reg_y/N;
			if(overflow_y>=1)
			{
				++times_y;
				overflow_y=1;
				if(flag_y>0)
				{
					array[length].y=y_tem+overflow_y;	
				}
				else
				{
					array[length].y=y_tem-overflow_y;
				}
				reg_y=reg_y-N;
				y_tem=array[length].y;
			//	array[length].x=x_tem;
				//length++;
			//	rx++;
			//	printf("CCCCC\n");
			//	printf("times_y: %d\n",times_y);
			//	printf("tem_ry: %d\n",tem_ry);
			//	printf("rx %f\n",rx);
			}
			else
			{
				array[length].y=y_tem;
				overflow_y=0;
			}
		}
		else
		{
			array[length].y=y_tem;
			overflow_y=0;
		}
	//	overflow=0;
		if(times_x<tem_rx)
		{
			reg_x=reg_x+rx;
		//	printf("CCCCC\n");
		//	printf("reg_x %f\n",reg_x);
			overflow_x=reg_x/N;
			if(overflow_x>=1)
			{
				++times_x;
				overflow_x=1;
				if(flag_x>0)
				{
					array[length].x=x_tem+overflow_x;
				}
				else
				{
					array[length].x=x_tem-overflow_x;
				}
				x_tem=array[length].x;
			//	array[length].y=y_tem;
			//	length++;
				reg_x=reg_x-N;
			//	printf("times_x: %d\n",times_x);
			//	printf("reg_x %f\n",reg_x);
			}
			else
			{
				array[length].x=x_tem;	
				overflow_x=0;
			}
		}
		else
		{
			array[length].x=x_tem;
			overflow_x=0;
		}
		length++;
		ry=ry-overflow_x*m_y;
		rx=rx+overflow_y*m_x;
		if(length>=SIZE)
		{
			break;
		}
	}while(times_x<tem_rx||times_y<tem_ry);
	return length;	
}
int Show_point(struct Point* p1)
{
	printf("X:%f ",p1->x);
	printf("Y:%f \n",p1->y);
	return 0;
}
int main()
{
	int step_number;//画斜线或圆所需的步数
	int i=0,result=0;
	float m_x=1.667;
	float m_y=0.6;
	struct Point Draw_point[SIZE]={{0,0}};
	struct Point slash[2]={{0.0,0},{0,0}};
	slash[1].x=300;
	slash[1].y=300.0;
	printf("AAAAA\n");

	//step_number=Draw_slash(slash,slash+1,Draw_point);
	step_number=Draw_round(slash,slash+1,Draw_point);
//	step_number=Draw_ellipse(slash,slash+1,Draw_point,m_x,m_y);
	printf("step_number: %d;\n",step_number);
	if(step_number<=0||step_number>=SIZE)
	{
		return 0;
	}
	for(i=0;i<step_number;i++)
	{
		result=Show_point(Draw_point+i);
	}
	return 0;
}
