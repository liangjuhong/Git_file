#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5|GPIO_Pin_6 | GPIO_Pin_7|GPIO_Pin_8 | GPIO_Pin_9|GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);//A轮(左前轮)
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);//B轮(右前轮)
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
	GPIO_ResetBits(GPIOA,GPIO_Pin_9);//C轮(右后轮)
	GPIO_ResetBits(GPIOA,GPIO_Pin_10);
	GPIO_ResetBits(GPIOA,GPIO_Pin_11);//B轮(左后轮)
	
	PWM_Init();
}

void Motor_up(void)		//前进
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);//0101 0101
	GPIO_SetBits(GPIOA,GPIO_Pin_5);
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
	GPIO_SetBits(GPIOA,GPIO_Pin_7);
	GPIO_ResetBits(GPIOA,GPIO_Pin_11);
	GPIO_SetBits(GPIOA,GPIO_Pin_10);
	GPIO_ResetBits(GPIOA,GPIO_Pin_9);
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
}

void Motor_Down(void)		//后退
{
	GPIO_SetBits(GPIOA,GPIO_Pin_4);//1010 1010
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
	GPIO_SetBits(GPIOA,GPIO_Pin_6);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	GPIO_SetBits(GPIOA,GPIO_Pin_11);
	GPIO_ResetBits(GPIOA,GPIO_Pin_10);
	GPIO_SetBits(GPIOA,GPIO_Pin_9);
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}

void Motor_Left(void)		//左横移
{
	GPIO_SetBits(GPIOA,GPIO_Pin_4);//1001 0110
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
	GPIO_SetBits(GPIOA,GPIO_Pin_7);
	GPIO_ResetBits(GPIOA,GPIO_Pin_11);
	GPIO_SetBits(GPIOA,GPIO_Pin_10);
	GPIO_SetBits(GPIOA,GPIO_Pin_9);
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}

void Motor_Right(void)		//右横移
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);//0110 1001
	GPIO_SetBits(GPIOA,GPIO_Pin_5);
	GPIO_SetBits(GPIOA,GPIO_Pin_6);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	GPIO_SetBits(GPIOA,GPIO_Pin_11);
	GPIO_ResetBits(GPIOA,GPIO_Pin_10);
	GPIO_ResetBits(GPIOA,GPIO_Pin_9);
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
}

void Motor_Clockwise(void)		//顺时针右转
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);//0110 0110
	GPIO_SetBits(GPIOA,GPIO_Pin_5);
	GPIO_SetBits(GPIOA,GPIO_Pin_6);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	GPIO_ResetBits(GPIOA,GPIO_Pin_11);
	GPIO_SetBits(GPIOA,GPIO_Pin_10);
	GPIO_SetBits(GPIOA,GPIO_Pin_9);
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}

void Motor_Anticlockwise(void)	//逆时针左转
{
	GPIO_SetBits(GPIOA,GPIO_Pin_4);//1001 1001
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
	GPIO_SetBits(GPIOA,GPIO_Pin_7);
	GPIO_SetBits(GPIOA,GPIO_Pin_11);
	GPIO_ResetBits(GPIOA,GPIO_Pin_10);
	GPIO_ResetBits(GPIOA,GPIO_Pin_9);
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
}

void Motor_Stop(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);//0000 0000
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	GPIO_ResetBits(GPIOA,GPIO_Pin_11);
	GPIO_ResetBits(GPIOA,GPIO_Pin_10);
	GPIO_ResetBits(GPIOA,GPIO_Pin_9);
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}

void Enable(void)
{
	TIM_SetCompare1(TIM2, 30);
	TIM_SetCompare2(TIM2, 30);
	TIM_SetCompare3(TIM2, 30);
	TIM_SetCompare4(TIM2, 30);
}

void Disenable(void)
{
	TIM_SetCompare1(TIM2, 0);
	TIM_SetCompare2(TIM2, 0);
	TIM_SetCompare3(TIM2, 0);
	TIM_SetCompare4(TIM2, 0);
}

//void Motor_SetSpeed(int8_t Speed)
//{
//	if (Speed ==0)
//	{	
//		PWM_SetCompare3(0);
//		PWM_SetCompare2(0);
//		PWM_SetCompare1(0);
//		PWM_SetCompare4(0);
//	}
//	if(Speed>0)
//	{
//		qianjin();
//		PWM_SetCompare3(Speed);
//		PWM_SetCompare2(Speed);
//		PWM_SetCompare1(Speed);
//		PWM_SetCompare4(Speed);
//	}
//	if(Speed<0)
//	{
//		houtui();
//		PWM_SetCompare3(-Speed);
//		PWM_SetCompare2(-Speed);
//		PWM_SetCompare1(-Speed);
//		PWM_SetCompare4(-Speed);
//	}
//}
