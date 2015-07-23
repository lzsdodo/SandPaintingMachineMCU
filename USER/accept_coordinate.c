#include "accept_coordinate.h"
#include "delay.h"

/*
���ļ���Ҫʵ�ִӴ��ڽ����������ݣ������������ݴ洢�����������
*/

volatile u16 Position_x = 0;
volatile u16 Position_y = 0;
volatile u16 Position_z = 0;
static struct Coordinate Coordinate_queue[LENGTH_COORDINATE_QUEUE] = {0};
u16 Cq_head = 0;
u16 Cq_tail = 0;

//static int flag_right_pre_point=0;//����ǰһ�����Ƿ��Ѿ���ȷ����,1��ʾ��ȷ���ܣ�0��ʾ����

void Copy_Coordinate(struct Coordinate *c_point1,struct Coordinate *c_point2)
{
	c_point1->NOW_X = c_point2->NOW_X;
	c_point1->NOW_Y = c_point2->NOW_Y;
	c_point1->NOW_Z = c_point2->NOW_Z;
}
/*
	�������򷵻�1
*/
u16 Cq_is_full(void)
{
	if( (Cq_tail+1)%LENGTH_COORDINATE_QUEUE==Cq_head )
	{
		return 1;
	}
	else
	return 0;
}

/*
	���пգ�����1
*/
u16 Cq_is_empty(void)
{
	if( Cq_head==Cq_tail )
	return 1;
	else
	return 0;	
}

/*
	������в���һ��Ԫ�� ,����ɹ�����1��ʧ�ܷ���0
*/
u16 Cq_insert(struct Coordinate *com)
{
	 if( Cq_is_full() )
	 {
	 	return 0;
	 }
	 
	 Coordinate_queue[Cq_tail].NOW_X = com->NOW_X;
	 Coordinate_queue[Cq_tail].NOW_Y = com->NOW_Y;
	 Coordinate_queue[Cq_tail].NOW_Z = com->NOW_Z;
	 Cq_tail = (Cq_tail+1)%LENGTH_COORDINATE_QUEUE;
	 return 1;
}

u16 Cq_push(struct Coordinate* point)
{
	u16 result=0;
	result = Cq_is_empty();
	if( result>0 )
	{
		return 0;
	}
	point->NOW_X = Coordinate_queue[Cq_head].NOW_X;
	point->NOW_Y = Coordinate_queue[Cq_head].NOW_Y;
	point->NOW_Z = Coordinate_queue[Cq_head].NOW_Z;
	Cq_head = (Cq_head+1)%LENGTH_COORDINATE_QUEUE;
	return 1;
}
u16 Cq_length(void)
{
	u16 length = 0;
	length = (Cq_tail+LENGTH_COORDINATE_QUEUE-Cq_head)%LENGTH_COORDINATE_QUEUE;
	return length;
}

/*
   ����������Ϣ��һ�ν���һ�����꣨����X��Y��Z�ᣩ,�����յ�������������������
*/
u16 Accept_message()
{
	u8 t;
	u16 times=0,result=0,tem=0;
	u16 success=0;
	u16 number_xyz[3]={0};
	struct Coordinate tem_point={0,0,0};

		Send_result(CONTINUE_ACCEPT,0);										//��������������ݰ�
		delay_ms(1);
		while(1)
		{
			if(USART_RX_STA&0x4000)													//���յ�һ�����ݰ�
			{
			//	printf("���յ����ݰ�\r\n\r");
				result = Check();															//������ݽ����Ƿ��д���		
				if(result) 																		//���յ�������ȷ
				{		
				 //   printf("accpet message sucess\r\n\r");
					tem = Insert_queue();												//���½��յ������ݰ����뻺�����
					if( tem>0 )
					{
						while( Message_number>=3 )
						{
							//printf("Message_number\n\r\n");
							for( t=0; t<3; t++ )
							{
								number_xyz[t] = Push_queue();	//��¼xyz������
							}
							tem_point.NOW_X = number_xyz[0];
							tem_point.NOW_Y = number_xyz[1];
							tem_point.NOW_Z = number_xyz[2];
							
							//printf("Cq_insert\r\n\r");
							//��������������ȥ
							if( UC_Stop_flag==0 )
							{
								tem = Cq_insert(&tem_point);
							}
							if( UC_Stop_flag==1 )
							{
								USART_RX_STA = 0;
								return 0;	 											//UC_stop_flag==1ֱ�ӷ���
							}	
							if( tem==0 )											//���룬��������Command_queueʧ��
							{
								USART_RX_STA = 0;
							//	printf("AC_124\n\r\n");
								Send_result(QUEUE_FULL,0);
								return 0;
							}
						}
						if( tem>0 )
						{
							USART_RX_STA = 0;
							Position_x = tem_point.NOW_X;
							Position_y = tem_point.NOW_Y;
							Position_z = tem_point.NOW_Z;
							success = 1;
//							printf("RIGHT\n");
//							printf("Position_x:%d\r\n\r",Position_x);
//							printf("Position_y: %d\r\n\r",Position_y);
//							printf("Position_z:%d\r\n\r",Position_z);
							Send_result(RIGHT,0);
							return success;
						}
					}
					else
					{
					//	printf("AC_146\n");
						Send_result(QUEUE_FULL,0);
						USART_RX_STA = 0;
						break;	
					}		
				}
				else   //���յ�����������
				{
					uart_init(72,115200);
					USART_RX_STA = 0;
				//	printf("�յ����ݰ�����\r\n\r");
					Send_result(CHECKSUM_ERROR,0);//�������ݱ�����У���
					break;
				}
			} 
			else if(USART_RX_STA&0x2000) //�������ݰ������г���δ�ܽ��յ����������ݰ�,��ͷ����
			{
				for( t=0;t<15;t++ )
				{
					USART_RX_BUF[t] = 0;
				}
				USART_RX_STA = 0;
			//	printf("ERROR_INFORMATION: %d \r\n\r",ERROR_INFORMATION);
				Send_result(PACKET_HEAD_ERROR,0);
				break;
			}
			else
			{
				//���ʵ�ֵȴ����ݷ��ͣ��ȴ�ʱ���Ϊ100us
				times++; 
				if(times>100)
				{
			//		printf("δ�յ����ݰ�\n\r\n");
					USART_RX_STA=0;
					break;
				}
				delay_us(100);  
			}
		}
		return success;
}
