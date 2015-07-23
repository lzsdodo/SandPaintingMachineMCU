#include "sys.h"
#include "usart2.h"	  
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos 使用	  
#endif

//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
	USART2->DR = (u8) ch;      
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////

#if EN_USART2_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit14，是否接受完一个数据包，如果接受完则置为1
//bit13,接受过程中是否出错，如果出错则置为1
//bit12, 表示数据长度的两个字节是否接收完，接受完置为1
//bit10:11, 表示接收到数据长度的两字节的个数
//bit9,数据包头是否接受完，接受完则置为1
//bit7:8 记录接收到的数据包头的字节数
//bit6~0，接收到的有效字节数目
u16 USART2_RX_STA=0;       //接收状态标记	
u16 Length_Message2=6;	  
u8 ERROR_INFORMATION2=0; 
void USART2_IRQHandler(void)
{
	u8 res;	
	u16 length_tem=0;   
#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
	OSIntEnter();    
#endif
	if(USART2->SR&(1<<5))//接收到数据
	{	 
		res=USART2->DR;
		if(((USART2_RX_STA)&0x4000)==0)//数据包尚未接收完
		{
			if(USART2_RX_STA&0x200) //数据包头是否已经接收完
			{
				//数据包头已经接收完
				if(USART2_RX_STA&0x1000)//数据包表示长度的两个字节已经收到
				{
					 USART2_RX_BUF[USART2_RX_STA&0x7F]=res;
				 
					 if((USART2_RX_STA&0x7F)>=Length_Message2)//如果已经完成一个数据包的接收则将14位置为1
					 {
					 	USART2_RX_STA|=0x4000;
						Length_Message2=6;	
					 }
					  USART2_RX_STA++;//接收到的字节数加1	
				}
				else
				{
					USART2_RX_BUF[USART2_RX_STA&0x7F]=res;
					USART2_RX_STA++;
					USART2_RX_STA+=0x400;
					if((USART2_RX_STA&0x0800)>0)
					{
						USART2_RX_STA|=0x1000;//已接收到两字节的数据长度表示位
						length_tem=USART2_RX_BUF[(USART2_RX_STA&0x7F)-1];
						length_tem=length_tem<<8;
						length_tem=length_tem+USART2_RX_BUF[((USART2_RX_STA&0x7F)-2)];
						Length_Message2=length_tem+Length_Message2;	//从零开始加上一字节的校验和
					}
				}
			}
			else //接收数据包头 4个0xFF
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
#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
	OSIntExit();  											 
#endif
} 
#endif										 
//初始化IO 串口1
//pclk2:PCLK2时钟频率(Mhz)
//bound:波特率
//CHECK OK
//091209
void uart2_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<8;   	//使能PORTG口时钟  
 	GPIOG->CRH&=0XFFFFFF0F;	//IO状态设置
	GPIOG->CRH|=0X00000030;	//IO状态设置
	RCC->APB2ENR|=1<<2;   	//使能PORTA口时钟  
	GPIOA->CRL&=0XFFFF00FF;	//IO状态设置
	GPIOA->CRL|=0X00008B00;	//IO状态设置	 
	RCC->APB1ENR|=1<<17;  	//使能串口时钟 	 
	RCC->APB1RSTR|=1<<17;   //复位串口2
	RCC->APB1RSTR&=~(1<<17);//停止复位	  
     	   
	//波特率设置
 	USART2->BRR=mantissa; // 波特率设置	 
	USART2->CR1|=0X200C;  //1位停止,无校验位.
#ifdef USART2_RX_EN		  	//如果使能了接收
	//使能接收中断
	USART2->CR1|=1<<8;    	//PE中断使能
	USART2->CR1|=1<<5;    	//接收缓冲区非空中断使能	    	
	MY_NVIC_Init(2,2,USART2_IRQChannel,2);//组2，最低优先级 
	USART2_RX_STA=0;		//清零
#endif		
}
