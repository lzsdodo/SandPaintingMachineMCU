#include "sys.h"
#include "usart2.h"	  
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif

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
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
	USART2->DR = (u8) ch;      
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////

#if EN_USART2_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit14���Ƿ������һ�����ݰ����������������Ϊ1
//bit13,���ܹ������Ƿ���������������Ϊ1
//bit12, ��ʾ���ݳ��ȵ������ֽ��Ƿ�����꣬��������Ϊ1
//bit10:11, ��ʾ���յ����ݳ��ȵ����ֽڵĸ���
//bit9,���ݰ�ͷ�Ƿ�����꣬����������Ϊ1
//bit7:8 ��¼���յ������ݰ�ͷ���ֽ���
//bit6~0�����յ�����Ч�ֽ���Ŀ
u16 USART2_RX_STA=0;       //����״̬���	
u16 Length_Message2=6;	  
u8 ERROR_INFORMATION2=0; 
void USART2_IRQHandler(void)
{
	u8 res;	
	u16 length_tem=0;   
#ifdef OS_CRITICAL_METHOD 	//���OS_CRITICAL_METHOD������,˵��ʹ��ucosII��.
	OSIntEnter();    
#endif
	if(USART2->SR&(1<<5))//���յ�����
	{	 
		res=USART2->DR;
		if(((USART2_RX_STA)&0x4000)==0)//���ݰ���δ������
		{
			if(USART2_RX_STA&0x200) //���ݰ�ͷ�Ƿ��Ѿ�������
			{
				//���ݰ�ͷ�Ѿ�������
				if(USART2_RX_STA&0x1000)//���ݰ���ʾ���ȵ������ֽ��Ѿ��յ�
				{
					 USART2_RX_BUF[USART2_RX_STA&0x7F]=res;
				 
					 if((USART2_RX_STA&0x7F)>=Length_Message2)//����Ѿ����һ�����ݰ��Ľ�����14λ��Ϊ1
					 {
					 	USART2_RX_STA|=0x4000;
						Length_Message2=6;	
					 }
					  USART2_RX_STA++;//���յ����ֽ�����1	
				}
				else
				{
					USART2_RX_BUF[USART2_RX_STA&0x7F]=res;
					USART2_RX_STA++;
					USART2_RX_STA+=0x400;
					if((USART2_RX_STA&0x0800)>0)
					{
						USART2_RX_STA|=0x1000;//�ѽ��յ����ֽڵ����ݳ��ȱ�ʾλ
						length_tem=USART2_RX_BUF[(USART2_RX_STA&0x7F)-1];
						length_tem=length_tem<<8;
						length_tem=length_tem+USART2_RX_BUF[((USART2_RX_STA&0x7F)-2)];
						Length_Message2=length_tem+Length_Message2;	//���㿪ʼ����һ�ֽڵ�У���
					}
				}
			}
			else //�������ݰ�ͷ 4��0xFF
			{
				if(res!=0xFF)
				{
					USART2_RX_STA=0;
					USART2_RX_STA|=0x2000;
					ERROR_INFORMATION2=res;
				}		
				else
				{
					USART2_RX_BUF[(USART2_RX_STA)&0x7F]=res;
					USART2_RX_STA++;
					USART2_RX_STA+=0x80;
				}
			}
		} 		 									     
	}
#ifdef OS_CRITICAL_METHOD 	//���OS_CRITICAL_METHOD������,˵��ʹ��ucosII��.
	OSIntExit();  											 
#endif
} 
#endif										 
//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������
//CHECK OK
//091209
void uart2_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<8;   	//ʹ��PORTG��ʱ��  
 	GPIOG->CRH&=0XFFFFFF0F;	//IO״̬����
	GPIOG->CRH|=0X00000030;	//IO״̬����
	RCC->APB2ENR|=1<<2;   	//ʹ��PORTA��ʱ��  
	GPIOA->CRL&=0XFFFF00FF;	//IO״̬����
	GPIOA->CRL|=0X00008B00;	//IO״̬����	 
	RCC->APB1ENR|=1<<17;  	//ʹ�ܴ���ʱ�� 	 
	RCC->APB1RSTR|=1<<17;   //��λ����2
	RCC->APB1RSTR&=~(1<<17);//ֹͣ��λ	  
     	   
	//����������
 	USART2->BRR=mantissa; // ����������	 
	USART2->CR1|=0X200C;  //1λֹͣ,��У��λ.
#ifdef USART2_RX_EN		  	//���ʹ���˽���
	//ʹ�ܽ����ж�
	USART2->CR1|=1<<8;    	//PE�ж�ʹ��
	USART2->CR1|=1<<5;    	//���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(2,2,USART2_IRQChannel,2);//��2��������ȼ� 
	USART2_RX_STA=0;		//����
#endif		
}
