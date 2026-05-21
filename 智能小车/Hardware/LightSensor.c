#include "stm32f10x.h"                  // Device header
#include "Motor.h"
#include "Delay.h"
#include "HC_SR04.h"
#include "OLED.h"
#define LW_1 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)//读取PB12电平
#define LW_2 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)//读取PB13电平
#define LW_3 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)//读取PB14电平
//#define LW_4 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) //读取PB1电平
#define LightSensor_speed 32

int32_t	 Distance,Distance1,Distance2,Distance3;

void LightSensor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef  GPIO_InitStruture;
	GPIO_InitStruture.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStruture.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_1;
	GPIO_InitStruture.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruture);
	
	void sr04_init();
	
}

void LightSensor_Start(void)//寻光
{   
//		Distance1=sr04_get_distance();
//		Delay_ms(20);
//		Distance2 =sr04_get_distance();
//		Delay_ms(20);
//		Distance3 =sr04_get_distance();
//		Delay_ms(20);
//		if(Distance1>0&&Distance2>0&&Distance3>0)
//		Distance=(Distance1+Distance2+Distance3)/3;
//		else if(Distance1<=0&&Distance2>0&&Distance3>0)
//		Distance=(Distance2+Distance3)/2;
//		else if(Distance1>0&&Distance2<=0&&Distance3>0)
//		Distance=(Distance1+Distance3)/2;
//		else if(Distance1>0&&Distance2>0&&Distance3<=0)
//		Distance=(Distance1+Distance2)/2;
//		else if(Distance1>0) Distance=Distance1;
//		else if(Distance2>0) Distance=Distance2;
//		else Distance=Distance3;
//		Delay_ms(20);
		Distance=sr04_get_distance();
//		if(LW_4==0)
//		{
//			Motor_Anticlockwise();
//			TIM_SetCompare1(TIM2, LightSensor_speed);
//			TIM_SetCompare2(TIM2, LightSensor_speed);
//			TIM_SetCompare3(TIM2, LightSensor_speed);
//			TIM_SetCompare4(TIM2, LightSensor_speed);
//			Delay_ms(50);
//		}
		
		if(Distance<20)
		{
			Motor_Stop();
			Delay_ms(1800);
			Motor_Anticlockwise();
			TIM_SetCompare1(TIM2, LightSensor_speed);
			TIM_SetCompare2(TIM2, LightSensor_speed);
			TIM_SetCompare3(TIM2, LightSensor_speed);
			TIM_SetCompare4(TIM2, LightSensor_speed);
			Delay_ms(1200);
		}
		
		else
		{
		if(LW_1==1&&LW_2==1&&LW_3==1)
		{
			Motor_Stop();
			TIM_SetCompare1(TIM2, 0);
			TIM_SetCompare2(TIM2, 0);
			TIM_SetCompare3(TIM2, 0);
			TIM_SetCompare4(TIM2, 0);
			Delay_ms(100);
		}
		
		if(LW_1==0&&LW_2==0&&LW_3==1)
		{
			Motor_Clockwise();
			TIM_SetCompare1(TIM2, LightSensor_speed);
			TIM_SetCompare2(TIM2, LightSensor_speed);
			TIM_SetCompare3(TIM2, LightSensor_speed);
			TIM_SetCompare4(TIM2, LightSensor_speed);
			Delay_ms(120);
		}
		
		if(LW_1==0&&LW_2==1&&LW_3==1)
		{
			Motor_Clockwise();
			TIM_SetCompare1(TIM2, LightSensor_speed);
			TIM_SetCompare2(TIM2, LightSensor_speed);
			TIM_SetCompare3(TIM2, LightSensor_speed);
			TIM_SetCompare4(TIM2, LightSensor_speed);
			Delay_ms(120);
		}

		if(LW_1==1&&LW_2==1&&LW_3==0)
		{
			Motor_Anticlockwise();
			TIM_SetCompare1(TIM2, LightSensor_speed);
			TIM_SetCompare2(TIM2, LightSensor_speed);
			TIM_SetCompare3(TIM2,LightSensor_speed);
			TIM_SetCompare4(TIM2, LightSensor_speed);
			Delay_ms(120);
		}
		
		if(LW_1==1&&LW_2==0&&LW_3==0)
		{
			Motor_Anticlockwise();
			TIM_SetCompare1(TIM2, LightSensor_speed);
			TIM_SetCompare2(TIM2, LightSensor_speed);
			TIM_SetCompare3(TIM2, LightSensor_speed);
			TIM_SetCompare4(TIM2, LightSensor_speed);
			Delay_ms(120);
		}
		
		if(LW_1==0&&LW_2==0&&LW_3==0)
		{
			Motor_up();
			TIM_SetCompare1(TIM2, LightSensor_speed+1);
			TIM_SetCompare2(TIM2, LightSensor_speed+1);
			TIM_SetCompare3(TIM2, LightSensor_speed+1);
			TIM_SetCompare4(TIM2, LightSensor_speed+1);
			Delay_ms(150);
		}
		if(LW_1==1&&LW_2==0&&LW_3==1)
		{
			Motor_up();
			TIM_SetCompare1(TIM2, LightSensor_speed+1);
			TIM_SetCompare2(TIM2, LightSensor_speed+1);
			TIM_SetCompare3(TIM2, LightSensor_speed+1);
			TIM_SetCompare4(TIM2, LightSensor_speed+1);
			Delay_ms(150);
		}
		}
	
}
