#include "stm32f10x.h"  

void Usart_Init(void){
  
    GPIO_InitTypeDef GPIO_InitStructure;   //GPIO端口设置
	USART_InitTypeDef USART_InitStructure;  //USART配置
	NVIC_InitTypeDef NVIC_InitStructure;  //中断配置
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);	//使能USART1，GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB10
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB11 

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	
	
	USART_Init(USART3, &USART_InitStructure); //初始化串口3
	
	USART_Cmd(USART3, ENABLE); //使能串口3
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断

}

void USART_SendString(USART_TypeDef* USARTx,char *DataString)
{
	USART_ClearFlag(USARTx,USART_FLAG_TC);
	int i=0;
	while(DataString[i]!='\0')
	{
		USART_SendData(USARTx,DataString[i]);
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==0);
		USART_ClearFlag(USARTx,USART_FLAG_TC);
		i++;
	}
}

