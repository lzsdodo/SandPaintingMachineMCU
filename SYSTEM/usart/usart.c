#include "sys.h"
#include "usart.h"	  


//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
// #if 1
// #pragma import(__use_no_semihosting)             
// //��׼����Ҫ��֧�ֺ���                 
// struct __FILE 
// { 
// 	int handle; 
// 	/* Whatever you require here. If the only file you are using is */ 
// 	/* standard output using printf() for debugging, no file handling */ 
// 	/* is required. */ 
// }; 
// /* FILE is typedef�� d in stdio.h. */ 
// FILE __stdout;       
// //����_sys_exit()�Ա���ʹ�ð�����ģʽ    
// _sys_exit(int x) 
// { 
// 	x = x; 
// } 
// //�ض���fputc���� 
// int fputc(int ch, FILE *f)
// {      
// 	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
// 	USART1->DR = (u8) ch;      
// 	return ch;
// }
// #endif 
//end
//////////////////////////////////////////////////////////////////

#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
volatile u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit14��		�Ƿ������һ�����ݰ����������������Ϊ1
//bit13,		���ܹ������Ƿ���������������Ϊ1
//bit12, 		��ʾ���ݳ��ȵ������ֽ��Ƿ�����꣬��������Ϊ1
//bit10:11, ��ʾ���յ����ݳ��ȵ����ֽڵĸ���
//bit9,			���ݰ�ͷ�Ƿ�����꣬����������Ϊ1
//bit7:8 		��¼���յ������ݰ�ͷ���ֽ���
//bit6~0��	���յ�����Ч�ֽ���Ŀ
volatile u16 USART_RX_STA   = 0;       //����״̬���	
volatile u16 Length_Message = 0;	 
const u16 LENGTH_MESSAGE1   = 6;
u8 ERROR_INFORMATION        = 0; 



void USART1_IRQHandler(void)
{
	u8 res;	
	u16 length_tem=0;   
	if(USART1->SR&(1<<5))//���յ�����
	{	 
		res=USART1->DR;
		if(((USART_RX_STA&0x4000)==0)&&((USART_RX_STA&0x2000)==0))//���ݰ���δ������,��ǰ��ǰ״̬����
		{
			if(USART_RX_STA&0x200) //���ݰ�ͷ�Ƿ��Ѿ�������
			{
				//���ݰ�ͷ�Ѿ�������
				if(USART_RX_STA&0x1000)//���ݰ���ʾ���ȵ������ֽ��Ѿ��յ�
				{
					 USART_RX_BUF[USART_RX_STA&0x7F]=res;
						
					 if((USART_RX_STA&0x7F)>=Length_Message)//����Ѿ����һ�����ݰ��Ľ�����14λ��Ϊ1
					 {
					 	 USART_RX_STA|=0x4000;
						 Length_Message=6;
					 }	
					 USART_RX_STA++;//���յ����ֽ�����1
				}
				else
				{
					USART_RX_BUF[USART_RX_STA&0x7F]=res;
					USART_RX_STA++;
					USART_RX_STA+=0x400;
					if((USART_RX_STA&0x0800)>0)
					{
						USART_RX_STA|=0x1000;//�ѽ��յ����ֽڵ����ݳ��ȱ�ʾλ
						length_tem=0;
						Length_Message=0;
						length_tem=USART_RX_BUF[(USART_RX_STA&0x7F)-1];
						length_tem=length_tem<<8;
						length_tem=length_tem+USART_RX_BUF[((USART_RX_STA&0x7F)-2)];
						Length_Message=length_tem+LENGTH_MESSAGE1;	//���㿪ʼ����һ�ֽڵ�У���
					}
				}
			}
			else //�������ݰ�ͷ 4��0xFF
			{
				if(res!=0xFF)
				{
					USART_RX_STA=0;
					USART_RX_STA|=0x2000;
					ERROR_INFORMATION=res;
				}		
				else
				{
					USART_RX_BUF[(USART_RX_STA)&0x7F]=res;
					USART_RX_STA++;
					USART_RX_STA+=0x80;
				}
			}
		} 		 									     
	}
} 
#endif										 
//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������
//CHECK OK
//091209
void uart_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
	RCC->APB2ENR|=1<<14;  //ʹ�ܴ���ʱ�� 
	GPIOA->CRH&=0XFFFFF00F;//IO״̬����
	GPIOA->CRH|=0X000008B0;//IO״̬����
  
	RCC->APB2RSTR|=1<<14;   //��λ����1
	RCC->APB2RSTR&=~(1<<14);//ֹͣ��λ	   	   
	//����������
 	USART1->BRR=mantissa; // ����������	 
	USART1->CR1|=0X200C;  //1λֹͣ,��У��λ.
	USART_RX_STA=0;
#if EN_USART1_RX		  //���ʹ���˽���
	//ʹ�ܽ����ж�
	USART1->CR1|=1<<8;    //PE�ж�ʹ��
	USART1->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(1,3,USART1_IRQChannel,2);//��2��������ȼ� ��ռ���ȼ���Ϊ1
#endif
}
