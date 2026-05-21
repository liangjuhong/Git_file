#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Motor.h"
#include "PWM.h"
//#include "Key.h"
#include "Xunji.h"
#include "LightSensor.h"
#include "HC_SR04.h"
#include "USART.h"
#include "Timer.h"
#include <stdlib.h>   // 必须加！malloc / free 都在这个头文件里
#define BLUETOOTH_UP		0x10
#define BLUETOOTH_DOWN		0x11
#define BLUETOOTH_LEFT		0x20
#define BLUETOOTH_RIGHT		0x21
#define BLUETOOTH_Start		0x30
#define BLUETOOTH_Stop		0x31
#define BLUETOOTH_LEFT_h	0x40
#define BLUETOOTH_RIGHT_h	0x41
#define BLUETOOTH_Mode_1	0x01
#define BLUETOOTH_Mode_2	0x02
#define BLUETOOTH_Mode_3	0x03
#define BLUETOOTH_Speed_ADD	0x04
#define BLUETOOTH_Speed_DRE	0x05

#define sudu 28
//uint8_t KeyNum;
uint8_t	 Data,Speed_ADD_Flag,Speed_DRE_Flag,Mode_Flag,Begin_Flag,Link_Flag=0;
uint16_t Speed=30,Counter,i,Counter1,j,Speed1;
uint16_t Turn_Flag;
int32_t Dis;

extern uint32_t Distance;

int main(void)
{
//	int p=(int)malloc(5);
	OLED_Init();
	Motor_Init();
	Xunji_Init();
	PWM_Init();
//	Key_Init();
	LightSensor_Init();
	sr04_init();
	Usart_Init();
	Timer_Init();
	
	OLED_ShowString(1,1,"PREPARING SYSTEM");
	OLED_ShowString(2,1,"----------------");
	OLED_ShowString(3,1,"----------------");
	OLED_ShowString(4,1,"--WAITING IINK--");

	while (1)
	{	
		if(Data!=0&&Link_Flag==0)
		{
			OLED_Clear();
			OLED_ShowString(1,1,"CONNECT  SUCCESS");
			OLED_ShowString(2,1,"--SYSTEM LOAD--");
			Enable();
			OLED_Clear();
			OLED_ShowString(1,1,"LOADING COMPLETE");
			OLED_ShowString(3,1,"WELCOME  TO  USE");
			Link_Flag=1;
		}
		if(Data==BLUETOOTH_Speed_ADD)
		{
			Speed_ADD_Flag=1;
		}
		if(Data==BLUETOOTH_Speed_DRE)
		{
			Speed_DRE_Flag=1;
		}
		if(Data==BLUETOOTH_Mode_1)
		{
			Mode_Flag=1;//模式一转换标志位
		}
		if(Data==BLUETOOTH_Mode_2)
		{
			Mode_Flag=2;//模式二转换标志位
		}
		if(Data==BLUETOOTH_Mode_3)
		{
			Mode_Flag=3;//模式三转换标志位
		}
		if(Data==BLUETOOTH_Start)
		{
//			if(Mode_Flag!=0)
//			{
//				OLED_Clear();
//				Mode_Flag=0;
//			}
			Begin_Flag=1;//小车启动标志位
			Speed=30;
			if(Link_Flag==1)
			{
				OLED_Clear();
				OLED_ShowString(1,1,"WAITING IINK");
				OLED_Clear();
				Delay_ms(50);
				Link_Flag=2;
			}
			else
			{
				OLED_ShowString(1,2,"Please choose");
				OLED_ShowString(2,4,"the Mode");
			}
		}
		
		if(Data==BLUETOOTH_Stop)
		{
			Begin_Flag=0;
			Mode_Flag=0;
			Speed_ADD_Flag=0;
			Speed_DRE_Flag=0;
			Motor_Stop();
			OLED_Clear();
			OLED_ShowString(1,1,"SYSTEM SHUTDOWN");
			OLED_ShowString(3,1,"GOOD-BYE");
			OLED_Clear();
		}
		
		if(Begin_Flag==1)
		{
			
			if(Speed_ADD_Flag&&(Data==0))
			{
				Speed+=5;
				Speed_ADD_Flag=0;
				if(Speed>=40) Speed=40;
				PWM_SetCompare1(Speed);
				PWM_SetCompare2(Speed);
				PWM_SetCompare3(Speed);
				PWM_SetCompare4(Speed);
			}
			if(Speed_DRE_Flag&&(Data==0))
			{
				if(Speed<=20) Speed+=5;
				Speed-=5;
				Speed_DRE_Flag=0;
				if(Speed<=20) Speed=20;
				PWM_SetCompare1(Speed);
				PWM_SetCompare2(Speed);
				PWM_SetCompare3(Speed);
				PWM_SetCompare4(Speed);
				
			}
			if(Data==BLUETOOTH_UP)
			{
				Motor_up();
				PWM_SetCompare1(Speed);
				PWM_SetCompare2(Speed);
				PWM_SetCompare3(Speed);
				PWM_SetCompare4(Speed);
			}
			if(Data==BLUETOOTH_DOWN)
			{
				Motor_Down();
				PWM_SetCompare1(Speed);
				PWM_SetCompare2(Speed);
				PWM_SetCompare3(Speed);
				PWM_SetCompare4(Speed);
			}	
			Speed1=Speed;
			if(Data==BLUETOOTH_LEFT)
			{
				Turn_Flag=1;
				Speed=30;
				Motor_Anticlockwise();
				Enable();
			}
			if(Data==BLUETOOTH_RIGHT)
			{
				Turn_Flag=1;
				Speed=30;
				Motor_Clockwise();
				Enable();
			}
			if(Data==BLUETOOTH_LEFT_h)
			{
				Turn_Flag=1;
				Speed=30;
				Motor_Left();
				Enable();
			}
			if(Data==BLUETOOTH_RIGHT_h)
			{
				Turn_Flag=1;
				Speed=30;
				Motor_Right();
				Enable();
			}			
			if(Turn_Flag!=0)
			{
				Speed=Speed1;
				Turn_Flag=0;
			}
			
			if(Mode_Flag==1)
			{
				if(Data==0x04||Data==0x05||Data==0x10||Data==0x20||Data==0x21||Data==0x11||Data==0x40||Data==0x41)
				{
				PWM_SetCompare1(Speed);
				PWM_SetCompare2(Speed);
				PWM_SetCompare3(Speed);
				PWM_SetCompare4(Speed);
				}
				else
				{Motor_Stop();}
				OLED_ShowString(1,1,"----MODE1-----");
				OLED_ShowString(2,1,"SPEED:");
				OLED_ShowString(3,11,"           ");
				OLED_ShowSignedNum(2,7,5*Speed-100 ,4);
				OLED_ShowString(2,12,"    ");
			}
			
			while(Mode_Flag==2)
			{	
				Motor_Stop();

				OLED_ShowString(1,1,"----MODE2-----");
				OLED_ShowString(2,1,"-----Xunji-----");
				OLED_ShowString(3,11,"             ");
				Xunji_Start();
				if(Data!=0x02) Mode_Flag=0;
			}
			
			while(Mode_Flag==3)
			{
				Motor_Stop();
				OLED_ShowString(1,1,"----MODE3-----");
				OLED_ShowString(2,1,"Distance:");
				OLED_ShowSignedNum(2,10,Distance,4);
				OLED_ShowString(2,15," ");
				OLED_ShowString(3,1,"                              ");
				LightSensor_Start();
				if(Data!=0x03) Mode_Flag=0;
			}
		}
	}

}

void USART3_IRQHandler(void)
{ //串口3中断服务程序
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!=0)
	{
		Data = USART_ReceiveData(USART3);  //接收手机发送的信息
		USART_SendData(USART3,Data);
	}
	USART_ClearFlag(USART3,USART_IT_RXNE);
}

//void TIM3_IRQHandler(void) //中断函数模板
//{
//	Counter++;//100us 自加一次
//	Counter%=100; 
//	if(Counter<Speed) Motor_up();
//	else Motor_Stop();
//	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)//检查中断标志位
//	{
//		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
//	}
//}
