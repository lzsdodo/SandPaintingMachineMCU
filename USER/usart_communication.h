#ifndef _USART_COMMUNICATION_H
#define _USART_COMMUNICATION_H
#include "sys.h"
#include "usart.h"

/*
	���ļ���Ҫʵ�ִӴ��ڽ������ݣ����ӼĴ����ж�ȡ��8λ����ƴ��16Ϊ����
*/
#define LENGTH_CACHE  361 					//��������СΪ360
#define MESSAGE_START 7	  					//USART_RX_BUF[7]Ϊ��Ч���ݵ�һ���ֽ�
#define TYPE_START    6	  					//TYPE_START ��������ΪUSART_RX_BUF[6]
#define LENGTH_START  4	  					//������Ч���ݳ��ȵ���ʼ�ֽ�ΪUSART_RX_BUF[LENGTH_START];
#define STOP_START    13 						//������־λ

//������Ϣ ��������
#define RIGHT          		1  				//��ȷ����
#define CHECKSUM_ERROR 		2  				//У��ʹ���
#define QUEUE_FULL  			3					//������
#define CONTINUE_ACCEPT 	4					//��������
#define PACKET_HEAD_ERROR 5					//���ݰ�ͷ����

extern volatile u8 UC_CLEAN_SAND;		//ɨɳ��־λ��UC_CLEAN_SAND=0��ɨɳ UC_CLEAN_SAND=6 ɨɳ
extern volatile u8 UC_Stop_flag;		//���ͽ�����־λ 0��ʾ���� 1��ʾ����

extern u16 UC_cache[LENGTH_CACHE];	//���ڻ������
extern u16 UC_head;									//���ڻ�����е�ͷ
extern u16 UC_tail;									//���ڻ�����е�β
extern u16 UC_Length;								//���建����еĳ���
extern u8	 Type;										//��������
extern u16 Message_number;					//���ݸ���
extern u8  Check_Sum;								//У���

u8 Calculate_check_sum(u8 *message,u16 len);	//����У���

//result=1��ʾ��ȷ���� 
//result=2��ʾ���ڻ���������� 
//result=3��ʾ���껺��������� 
//result=4�������ݱ�����
void Send_result(u8 type,u16 result);	//����Ӧ����Ϣ

u16 Check(void);
u16 Is_full(u16 head,u16 tail);
u16 Is_empty(u16 head,u16 tail);
u16 Length_of_queue(u16 head,u16 tail);
u16 Insert_queue(void);
u16 Push_queue(void);
#endif


