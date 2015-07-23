#include "usart_communication.h"
#include "accept_coordinate.h"

volatile u8 UC_CLEAN_SAND = 0;	//ɨɳ��־λ��UC_CLEAN_SAND=0��ɨɳ UC_CLEAN_SAND=6 ɨɳ
volatile u8 UC_Stop_flag  = 0;	//���ͽ�����־λ 0��ʾ���� 1��ʾ���� Ĭ��ֵ����0

/*
	��λ��ǰ����λ�ں󣬸��ļ���Ҫʵ�ֽ��Ӵ��ڽ��յ���u8���͵����ݣ�����ת��Ϊu16���ͣ���������ݰ��Ľ�������У��
*/
u16 cache[LENGTH_CACHE] = {0};	//����������
u16 UC_head=0,UC_tail   = 0;		//UCΪ�����ļ�����д
u16 UC_Length = 361;						//���建����еĳ���
u8	Type = 0;										//��������
u16 Message_number = 0;					//���ݸ���
u8  Check_Sum = 0;							//У���

/*
	����У���
*/
u8 Calculate_check_sum(u8 *message,u16 len)
{
	u16 t;
	u8 sum=0;
	for( t=0; t<len; t++ )
	{
		sum += message[t];
	}
	sum = ~sum;
	return sum;
}

/*
	���ͽ��յ��Ľ��,result��ʾ������Ϣ�Ľ����type=1��ʾ�����������ĸ���	
*/
void Send_result(u8 type, u16 result)
{
	u8 check_sum=0,t;
	u8 tem2[15] = {0xFF,0xFF,0xFF,0xFF,0x8,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
	tem2[TYPE_START] = type;
	if( type == RIGHT )
	{
		//tem2[MESSAGE_START]=result>>8;		//��8λ
		//tem2[MESSAGE_START+1]=result&0xF;	//��8λ
		tem2[MESSAGE_START]   = Position_x&0xFF;
		tem2[MESSAGE_START+1] = Position_x/256;
		tem2[MESSAGE_START+2] = Position_y&0xFF;
		tem2[MESSAGE_START+3] = Position_y/256;
		tem2[MESSAGE_START+4] = Position_z&0xFF;
		tem2[MESSAGE_START+5] = Position_z/256;
	}
	if( type==QUEUE_FULL||type==CONTINUE_ACCEPT )
	{
		tem2[MESSAGE_START]   = Cq_head&0xFF;
		tem2[MESSAGE_START+1] = Cq_head/256;
		tem2[MESSAGE_START+2] = Cq_tail&0xFF;
		tem2[MESSAGE_START+3] = Cq_tail/256;
		tem2[MESSAGE_START+4] = 0;
		tem2[MESSAGE_START+5] = 0;
	}
	if( type==CHECKSUM_ERROR||type==PACKET_HEAD_ERROR )
	{
		tem2[MESSAGE_START]   = cache[0]&0xFF;
		tem2[MESSAGE_START+1] = cache[0]/256;
		tem2[MESSAGE_START+2] = cache[1]&0xFF;
		tem2[MESSAGE_START+3] = cache[1]/256;
		tem2[MESSAGE_START+4] = 0;
		tem2[MESSAGE_START+5] = 0;
	}
	check_sum = Calculate_check_sum(tem2+LENGTH_START,15-LENGTH_START-1);
	tem2[14] = check_sum;
	for( t=0; t<15; t++ )
	{
		while( (USART1->SR&0x40)==0 );//�ȴ����ͽ���
		USART1->DR = tem2[t];
	}
}


/*
	�ú�����Ҫ��ɶԽ��յ������ݽ��н���������ͨ��У�����������յ��������Ƿ���ȷ
	У��ͼ���������ȣ����ͺ���Ч����
	�ú�����Ҫ��Insert����֮ǰ���ã��ȶ����ݽ��м�⡣
*/
u16 Check(void)
{
	u16 len=0,t;
	u8  sum = 0;
	len = USART_RX_STA&0x7F;
	Check_Sum = USART_RX_BUF[len-1];
	Type = USART_RX_BUF[TYPE_START];
	for( t=LENGTH_START; t<len; t++ )
	{
		sum += USART_RX_BUF[t];
	} 
	sum = ~sum;
	if( sum>0 )
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/*
	�ж��Ƿ�����Ƿ����������򷵻�1
*/
u16 Is_full(u16 head,u16 tail)
{
	//β��360+1 �� ���г���361 = ��ͷ0
	if( ((tail+1)%UC_Length)==head )
	{
		return 1;
	}
	else
	return 0;
}

/*
	�ж϶����Ƿ�Ϊ�գ��շ���1
*/
u16 Is_empty(u16 head,u16 tail)
{
	//ͷ=β,Ϊ�ն���
	if( tail==head )	
	{
		return 1;
	}
	else
	return 0;
}

/*
	���ض��г���
*/
u16 Length_of_queue(u16 head,u16 tail)
{
	u16 long1 = 0;

	if( Is_empty(head,tail) )
	{
		return 0;
	}
	long1 = (tail+UC_Length-head) % UC_Length;
	return long1;
}

/*
	������в����ȡ�ֽڳ��ȣ����128���ֽ�
*/
u16 Insert_queue(void)
{
	if(Is_full(UC_head,UC_tail))
		return 0;
	else
		{
			u16 empty=0,len;
			u16 t = 0;
			len = USART_RX_STA&0x7F;
			empty = Length_of_queue(UC_head,UC_tail);
			UC_CLEAN_SAND = USART_RX_BUF[TYPE_START];
			UC_Stop_flag  = USART_RX_BUF[STOP_START];
			printf("USART_RX_BUF[STOP_START]: %d\r\n\r",USART_RX_BUF[STOP_START]);
			empty = UC_Length-empty-1; //����ճ�һ���������ֶ������Ͷ��п�
			for( t=MESSAGE_START; (t<len-2)&&(empty>0); t++ ) //len-2��ʾ��Ч��Ϣ���ݲ�����У��ͣ�������־λ
			{
				cache[UC_tail] = 0;
				cache[UC_tail] |= USART_RX_BUF[t+1];
				cache[UC_tail] = cache[UC_tail]<<8;
				cache[UC_tail] = cache[UC_tail]|USART_RX_BUF[t];
				UC_tail = (UC_tail+1)%UC_Length;
				t++;
				Message_number++;
				empty--;
			}
			return 1;
		}
}

/*
	�����У�һ�ε���һ��
*/
u16 Push_queue(void)
{
	if(Is_empty(UC_head,UC_tail))
	{
		return 65534;
	}
	else
	{
		u16 tem;
		tem=cache[UC_head];
		UC_head=(UC_head+1)%UC_Length;
		Message_number--;
		return tem;
	}
}
