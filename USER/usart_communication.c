#include "usart_communication.h"
#include "accept_coordinate.h"

volatile u8 UC_CLEAN_SAND = 0;	//扫沙标志位，UC_CLEAN_SAND=0不扫沙 UC_CLEAN_SAND=6 扫沙
volatile u8 UC_Stop_flag  = 0;	//发送结束标志位 0表示继续 1表示结束 默认值等于0

/*
	低位在前，高位在后，该文件主要实现将从串口接收到的u8类型的数据，解析转换为u16类型，并完成数据包的解析，和校验
*/
u16 cache[LENGTH_CACHE] = {0};	//缓冲区数组
u16 UC_head=0,UC_tail   = 0;		//UC为定义文件的缩写
u16 UC_Length = 361;						//定义缓冲队列的长度
u8	Type = 0;										//数据类型
u16 Message_number = 0;					//数据个数
u8  Check_Sum = 0;							//校验和

/*
	计算校验和
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
	发送接收到的结果,result表示接受消息的结果，type=1表示发送完成坐标的个数	
*/
void Send_result(u8 type, u16 result)
{
	u8 check_sum=0,t;
	u8 tem2[15] = {0xFF,0xFF,0xFF,0xFF,0x8,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
	tem2[TYPE_START] = type;
	if( type == RIGHT )
	{
		//tem2[MESSAGE_START]=result>>8;		//高8位
		//tem2[MESSAGE_START+1]=result&0xF;	//低8位
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
		while( (USART1->SR&0x40)==0 );//等待发送结束
		USART1->DR = tem2[t];
	}
}


/*
	该函数主要完成对接收到的数据进行解析，并且通过校验和来检验接收到的数据是否正确
	校验和计算包括长度，类型和有效数据
	该函数需要在Insert函数之前调用，先对数据进行检测。
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
	判断是否队列是否已满，满则返回1
*/
u16 Is_full(u16 head,u16 tail)
{
	//尾巴360+1 余 队列长度361 = 开头0
	if( ((tail+1)%UC_Length)==head )
	{
		return 1;
	}
	else
	return 0;
}

/*
	判断队列是否为空，空返回1
*/
u16 Is_empty(u16 head,u16 tail)
{
	//头=尾,为空队列
	if( tail==head )	
	{
		return 1;
	}
	else
	return 0;
}

/*
	返回队列长度
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
	向队列中插入读取字节长度，最多128个字节
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
			empty = UC_Length-empty-1; //数组空出一个用于区分队列满和队列空
			for( t=MESSAGE_START; (t<len-2)&&(empty>0); t++ ) //len-2表示有效消息数据不包括校验和，结束标志位
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
	出队列，一次弹出一个
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
