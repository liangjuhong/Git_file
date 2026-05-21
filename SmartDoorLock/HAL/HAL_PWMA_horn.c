#include <STC8H.H>
#include "HAL_PWMA_HORN.H"
#define HAL_PWMA_ENPort P70 //高电平功放芯片工作
#define HAL_PWMA_ARR_TIME_1KHZ MAIN_Fosc/1000
//DI的频率
#define HAL_PWMA_Sound_DI MAIN_Fosc/800
//DONG的频率
#define HAL_PWMA_Sound_DONG MAIN_Fosc/650

HAL_HornSound_struct xdata HAL_HornSound_task;
void HAL_PWMA_Config()
{
	IP2 |= 0x04;
	IP2H|= 0x04;
	PWMA_PS &= 0xFC;
	PWMA_PS |= 0x01;//高级PWM通道1输出脚切换为：PWM1P:P2.0 PWM1N:P2.1
	
	PWMA_CCER1 &=~0x05;//配置模式CCMR之前先清零通道使能CCER
	PWMA_CCMR1|=0x68;//0110 1000配置为PWM模式1，开启PWMA_CCR1寄存器的预装载功能
	PWMA_CCER1|=0x05;//开启OC1,OC1N比较输出
	
	PWMA_ENO |=0x03;//PWM1输出使能
	PWMA_CR1 |=0x80;//允许PWMA_ARR寄存器由预装载缓冲器缓冲
	
	PWMA_PSCRH=0x00;//预分频器不分频
	PWMA_PSCRL=0X00;
	
	PWMA_ARRH=HAL_PWMA_ARR_TIME_1KHZ>>8;//配置PWMA的周期
	PWMA_ARRL=HAL_PWMA_ARR_TIME_1KHZ;
	
	PWMA_CCR1H=(HAL_PWMA_ARR_TIME_1KHZ/4)>>8;//配置PWMA的占空比
	PWMA_CCR1L=HAL_PWMA_ARR_TIME_1KHZ/4;
	
	PWMA_BKR |=0x80;//使能主输出功能
	PWMA_DTR =0x0C;//设置死区时间
	PWMA_IER |=0x01;//开启PWMA中断
	EA=1;
}
void HAL_PWMA_Init()
{
	HAL_PWMA_Config();
	HAL_PWMA_ENPort=0;
	HAL_HornSound_task.soundSta=HAL_horn_sound_free;
	HAL_HornSound_task.Time_cycle=HAL_HornSound_Time_Cycle;
	HAL_HornSound_task.frequency=HAL_PWMA_ARR_TIME_1KHZ;
	HAL_HornSound_task.duty=HAL_PWMA_ARR_TIME_1KHZ>>4;
	HAL_HornSound_task.DiDong_time=0;
}
void HAL_PWMA_main(HAL_HornSound_enum Soundtype)
{
	switch(Soundtype)
	{
		case HAL_horn_sound_free:
			HAL_HornSound_task.soundSta=HAL_horn_sound_free;
			break;
		case HAL_horn_sound_Di:
			HAL_HornSound_task.soundSta=HAL_horn_sound_Di;
			HAL_HornSound_task.Time_cycle=HAL_HornSound_Time_Cycle;
			PWMA_ARRH=HAL_HornSound_task.frequency>>8;//配置PWMA的周期
			PWMA_ARRL=HAL_HornSound_task.frequency;
	
			PWMA_CCR1H=HAL_HornSound_task.duty>>8;//配置PWMA的占空比
			PWMA_CCR1L=HAL_HornSound_task.duty;

			HAL_PWMA_ENPort=1;
			PWMA_CR1 |=0x01;
			break;
		case HAL_horn_sound_DiDong:
			HAL_HornSound_task.soundSta=HAL_horn_sound_DiDong;
			PWMA_ARRH=HAL_PWMA_Sound_DI>>8;//配置PWMA的周期
			PWMA_ARRL=HAL_PWMA_Sound_DI;
	
			PWMA_CCR1H=(HAL_PWMA_Sound_DI>>4)>>8;//配置PWMA的占空比
			PWMA_CCR1L=(HAL_PWMA_Sound_DI>>4);
			HAL_PWMA_ENPort=1;
			PWMA_CR1 |=0x01;
			break;
	}
}
void HAL_PWMA_Interrupt() interrupt 26
{
	if(PWMA_SR1&0x01)
	{
		PWMA_SR1 &=~0x01;
		switch(HAL_HornSound_task.soundSta)
		{
			case HAL_horn_sound_free:
				break;
			case HAL_horn_sound_Di:
				if(HAL_HornSound_task.Time_cycle)
					HAL_HornSound_task.Time_cycle--;
				else
				{
					HAL_PWMA_ENPort=0;
					PWMA_CR1 &=0xFE;
					HAL_HornSound_task.soundSta=HAL_horn_sound_free;
				}
				break;
			case HAL_horn_sound_DiDong:
				HAL_HornSound_task.DiDong_time++;
				if(HAL_HornSound_task.DiDong_time==500)
				{
					HAL_PWMA_ENPort=0;
					PWMA_CR1 &=0xFE;
					PWMA_ARRH=HAL_PWMA_Sound_DONG>>8;//配置PWMA的周期
					PWMA_ARRL=HAL_PWMA_Sound_DONG;
			
					PWMA_CCR1H=(HAL_PWMA_Sound_DONG>>4)>>8;//配置PWMA的占空比
					PWMA_CCR1L=(HAL_PWMA_Sound_DONG>>4);
					HAL_PWMA_ENPort=1;
					PWMA_CR1 |=0x01;
				}
				if(HAL_HornSound_task.DiDong_time>1000)
				{
					HAL_HornSound_task.DiDong_time=0;
					HAL_PWMA_ENPort=0;
					PWMA_CR1 &=0xFE;
					HAL_HornSound_task.soundSta=HAL_horn_sound_free;
				}
				break;
			default:
				break;
		}
	}
}