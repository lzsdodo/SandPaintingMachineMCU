#ifndef _USART_COMMUNICATION_H
#define _USART_COMMUNICATION_H
#include "sys.h"
#include "usart.h"

/*
	该文件主要实现从串口接收数据，将从寄存器中读取的8位数据拼成16为数据
*/
#define LENGTH_CACHE  361 					//缓冲区大小为360
#define MESSAGE_START 7	  					//USART_RX_BUF[7]为有效数据第一个字节
#define TYPE_START    6	  					//TYPE_START 数据类型为USART_RX_BUF[6]
#define LENGTH_START  4	  					//保存有效数据长度的起始字节为USART_RX_BUF[LENGTH_START];
#define STOP_START    13 						//结束标志位

//错误信息 代码类型
#define RIGHT          		1  				//正确接受
#define CHECKSUM_ERROR 		2  				//校验和错误
#define QUEUE_FULL  			3					//队列满
#define CONTINUE_ACCEPT 	4					//继续发送
#define PACKET_HEAD_ERROR 5					//数据包头错误

extern volatile u8 UC_CLEAN_SAND;		//扫沙标志位，UC_CLEAN_SAND=0不扫沙 UC_CLEAN_SAND=6 扫沙
extern volatile u8 UC_Stop_flag;		//发送结束标志位 0表示继续 1表示结束

extern u16 UC_cache[LENGTH_CACHE];	//串口缓存队列
extern u16 UC_head;									//串口缓存队列的头
extern u16 UC_tail;									//串口缓存队列的尾
extern u16 UC_Length;								//定义缓冲队列的长度
extern u8	 Type;										//数据类型
extern u16 Message_number;					//数据个数
extern u8  Check_Sum;								//校验和

u8 Calculate_check_sum(u8 *message,u16 len);	//计算校验和

//result=1表示正确接收 
//result=2表示串口缓存队列已满 
//result=3表示坐标缓存队列已满 
//result=4接收数据报错误
void Send_result(u8 type,u16 result);	//发送应答信息

u16 Check(void);
u16 Is_full(u16 head,u16 tail);
u16 Is_empty(u16 head,u16 tail);
u16 Length_of_queue(u16 head,u16 tail);
u16 Insert_queue(void);
u16 Push_queue(void);
#endif


