#include "stm32f10x.h"                  // Device header

void Timer_Init(void)
{
	//发生更新时进入中断
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIM_InternalClockConfig(TIM3);//如果不调用此函数，默认使用该内部时钟
	
	//计数器溢出频率  CK_CNT_OV = CK_CNT / (ARR + 1) = CK_PSC / (PSC + 1) / (ARR + 1)
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;//1分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period=100-1;
	TIM_TimeBaseInitStructure.TIM_Prescaler=72-1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;//该计数器为重复计数器 是高级才有的		
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	//72000000/100 = 72000/72 = 1000   1/1000 1ms进一次中断
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);//清除更新中断标志位，避免初始化完直接进入中断的问题
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//开启更新中断到NVIC的通道
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//打开TIMER2中断通道
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM3,ENABLE);//开启定时器
}

/*
void TIM4_IRQHandler(void) //中断函数模板
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update) == SET)//检查中断标志位
	{
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	}
}*/
