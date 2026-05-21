#include "stm32f10x.h"                  // Device header
#include "Motor.h"
#include "Delay.h"
#define HW_1 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)//读取PB3电平
#define HW_2 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)//读取PB4电平
#define HW_3 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)//读取PB5电平
#define HW_4 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)//读取PB6电平
#define Xunji_Speed 31

void Xunji_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void Xunji_Start(void)
{
		if(HW_1==0&&HW_2==0&&HW_3==0&&HW_4==0)
		{
			Motor_up();
			TIM_SetCompare1(TIM2, Xunji_Speed);
			TIM_SetCompare2(TIM2, Xunji_Speed);
			TIM_SetCompare3(TIM2, Xunji_Speed);
			TIM_SetCompare4(TIM2, Xunji_Speed);
			Delay_ms(100);
		}
		if(HW_1==0&&HW_2==1&&HW_3==1&&HW_4==0)
		{
			Motor_up();
			TIM_SetCompare1(TIM2, Xunji_Speed);
			TIM_SetCompare2(TIM2, Xunji_Speed);
			TIM_SetCompare3(TIM2, Xunji_Speed);
			TIM_SetCompare4(TIM2, Xunji_Speed);
			Delay_ms(100);
		}
		if(HW_1==0&&HW_2==1&&HW_3==0&&HW_4==0)
		{
			Motor_Anticlockwise();
			TIM_SetCompare1(TIM2, Xunji_Speed);
			TIM_SetCompare2(TIM2, Xunji_Speed);
			TIM_SetCompare3(TIM2, Xunji_Speed);
			TIM_SetCompare4(TIM2, Xunji_Speed);
			Delay_ms(20);
		}
		if(HW_1==1&&HW_2==0&&HW_3==0&&HW_4==0)
		{
			Motor_Anticlockwise();
			TIM_SetCompare1(TIM2, Xunji_Speed);
			TIM_SetCompare2(TIM2, Xunji_Speed);
			TIM_SetCompare3(TIM2, Xunji_Speed);
			TIM_SetCompare4(TIM2, Xunji_Speed);
			Delay_ms(150);
		}
		if(HW_1==1&&HW_2==1&&HW_3==0&&HW_4==0)
		{
			Motor_Anticlockwise();
			TIM_SetCompare1(TIM2, Xunji_Speed);
			TIM_SetCompare2(TIM2, Xunji_Speed);
			TIM_SetCompare3(TIM2, Xunji_Speed);
			TIM_SetCompare4(TIM2, Xunji_Speed);
			Delay_ms(200);
		}
		if(HW_1==0&&HW_2==0&&HW_3==1&&HW_4==0)
		{
			Motor_Clockwise();
			TIM_SetCompare1(TIM2, Xunji_Speed);
			TIM_SetCompare2(TIM2, Xunji_Speed);
			TIM_SetCompare3(TIM2, Xunji_Speed);
			TIM_SetCompare4(TIM2, Xunji_Speed);
			Delay_ms(20);
		}
			if(HW_1==0&&HW_2==0&&HW_3==0&&HW_4==1)
		{
			Motor_Clockwise();
			TIM_SetCompare1(TIM2, Xunji_Speed);
			TIM_SetCompare2(TIM2, Xunji_Speed);
			TIM_SetCompare3(TIM2, Xunji_Speed);
			TIM_SetCompare4(TIM2, Xunji_Speed);
			Delay_ms(150);
		}
			if(HW_1==0&&HW_2==0&&HW_3==1&&HW_4==1)
		{
			Motor_Clockwise();
			TIM_SetCompare1(TIM2, Xunji_Speed);
			TIM_SetCompare2(TIM2, Xunji_Speed);
			TIM_SetCompare3(TIM2, Xunji_Speed);
			TIM_SetCompare4(TIM2, Xunji_Speed);
			Delay_ms(200);
		}
			if(HW_1==1&&HW_2==1&&HW_3==1&&HW_4==1)
		{
			Motor_Stop();
			TIM_SetCompare1(TIM2, 0);
			TIM_SetCompare2(TIM2, 0);
			TIM_SetCompare3(TIM2, 0);
			TIM_SetCompare4(TIM2, 0);
			Delay_ms(150);
		}
}
