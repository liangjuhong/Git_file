#include <STC8H.H>
#include "HAL_TIMER4.H"

unsigned char systick_ms;

void HAL_Timer4_Init()
{
	T4L=Timer4_Initial_Value;
	T4H=(Timer4_Initial_Value>>8);
	
	T4T3M &=0x0F;
	IE2 |=0x40;//打开定时器4中断
	EA=1;//打开总中断
	T4T3M |=0x80;//启动定时器
}

void Systick_Halder() interrupt 20
{
	systick_ms=1;
}