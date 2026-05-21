#include <STC8H.H>
#include "HAL_GPIO.H"

#define MCU_LED P67
void HAL_GPIO_Init()
{
	P0M0=0xE0;
	P0M1=0x00;
	P1M0=0x00;
	P1M1=0x00;
	P2M0=0x00;
	P2M1=0x00;
	P3M0=0x00;
	P3M1=0x00;
	P4M0=0x00;
	P4M1=0x00;
	P5M0=0x00;
	P5M1=0x00;
	P6M0=0x81;
	P6M1=0x00;
	P7M0=0x10;
	P7M1=0x00;
	P67=1;
}
#ifdef HAL_MCU_LED_TEST_ENABLE
void HAL_MCU_LED_TEST()
{
	static unsigned int HAL_LEDCounter;
	HAL_LEDCounter++;
	if(HAL_LEDCounter>499)
	{
		HAL_LEDCounter=0;
		P67=!P67;
	}
}
#endif
