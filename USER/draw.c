#include "draw.h"

//��б��
int Draw_slash(struct Point *now_point,struct Point *next_point,struct Point array[SIZE])
{
	//rx��ʾXe,ry��ʾYe,reg_x��reg_y��ʾ�����Ĵ�����
	float rx=0,reg_x=0,ry=0,reg_y=0;
	int N=8,tem_n;//��ʾ�ۼ�����ֵ
	int flag_x=0,flag_y=0;//��ʾx,y��ǰ������ƶ�
	int i=0;
	int length=0;//��¼�����������
	int overflow=0;// �������ֵ
	int times_x=0,times_y=0;//��¼x,y���������
	float x_tem=now_point->x,y_tem=now_point->y;
	if(now_point==0||next_point==0)
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
	reg_x=N/2;//�����
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
			}
		}
		if(length>=SIZE)
		{
			break;
		}
	}while(times_x<rx||times_y<ry);
	return length;
}

//��Բ��
int Draw_round(struct Point* now_point,struct Point* next_point,struct Point array[SIZE])
{
	//rx��ʾXe,ry��ʾYe,reg_x��reg_y��ʾ�����Ĵ�����
	float rx=0,reg_x=0,ry=0,reg_y=0;
	int N=8,tem_n=0;//��ʾ�ۼ�����ֵ
	int flag_x=0,flag_y=0;//��ʾx,y��ǰ������ƶ�
	int i=0;
	int length=0;//��¼�����������
	int overflow_x=0,overflow_y=0;// �������ֵ
	int times_x=0,times_y=0;//��¼x,y���������
	float x_tem=now_point->x,y_tem=now_point->y;
	int tem_rx=0,tem_ry=0;//���������ֵ
	if(now_point==0||next_point==0)
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
		if(times_x<tem_rx)
		{
			reg_x=reg_x+rx;
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
				reg_x=reg_x-N;
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

//����Բ��m_x=a/b;m_y=b/a;
int Draw_ellipse(struct Point* now_point,struct Point* next_point,struct Point array[SIZE],float m_x,float m_y)
{
	//rx��ʾXe,ry��ʾYe,reg_x��reg_y��ʾ�����Ĵ�����
	float rx=0,reg_x=0,ry=0,reg_y=0;
	int N=8,tem_n=0;//��ʾ�ۼ�����ֵ
	int flag_x=0,flag_y=0;//��ʾx,y��ǰ������ƶ�
	int i=0;
	int length=0;//��¼�����������
	int overflow_x=0,overflow_y=0;// �������ֵ
	int times_x=0,times_y=0;//��¼x,y���������
	float x_tem=now_point->x,y_tem=now_point->y;
	int tem_rx=0,tem_ry=0;
	if(now_point==0||next_point==0)
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
		if(times_x<tem_rx)
		{
			reg_x=reg_x+rx;
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
				reg_x=reg_x-N;
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
