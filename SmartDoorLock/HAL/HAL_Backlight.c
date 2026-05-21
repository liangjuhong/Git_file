#include <STC8H.H>
#include "HAL_BACKLIGHT.H"
#define HAL_TKEY_LED1 P05
#define HAL_TKEY_LED2 P06
#define HAL_TKEY_LED3 P07
#define HAL_TKEY_LED4 P60

#define HAL_TKEY_COM1 P04
#define HAL_TKEY_COM2 P45
#define HAL_TKEY_COM3 P75
#define HAL_TKEY_COM4 P33
//所有背光灯熄灭
void HAL_BackLight_All_OFF()
{
	HAL_TKEY_LED1=0;
	HAL_TKEY_LED2=0;
	HAL_TKEY_LED3=0;
	HAL_TKEY_LED4=0;

	HAL_TKEY_COM1=1;
	HAL_TKEY_COM2=1;
	HAL_TKEY_COM3=1;
	HAL_TKEY_COM4=1;
}
//所有背光灯点亮
void HAL_BackLight_All_ON()
{
	HAL_TKEY_LED1=1;
	HAL_TKEY_LED2=1;
	HAL_TKEY_LED3=1;
	HAL_TKEY_LED4=1;

	HAL_TKEY_COM1=0;
	HAL_TKEY_COM2=0;
	HAL_TKEY_COM3=0;
	HAL_TKEY_COM4=0;
}
//Light范围是0~15
void HAL_BackLight_Control(HAL_BackLightTypedef Light)
{
	unsigned char xdata numA,numB;
	HAL_BackLight_All_OFF();
	numA=Light/4;
	numB=Light%4;
	switch(numA)
	{
		case 0:HAL_TKEY_LED1=1;
		break;
		case 1:HAL_TKEY_LED2=1;
		break;
		case 2:HAL_TKEY_LED3=1;
		break;
		case 3:HAL_TKEY_LED4=1;
		break;
	}
	switch(numB)
	{
		case 0:HAL_TKEY_COM1=0;
		break;
		case 1:HAL_TKEY_COM2=0;
		break;
		case 2:HAL_TKEY_COM3=0;
		break;
		case 3:HAL_TKEY_COM4=0;
		break;
	}
}

#ifdef HAL_BACKLIGHT_TEST_ENABLE
void HAL_BackLight_TEST()
{
	static unsigned int xdata BLcounter;
	static unsigned char xdata i;
	BLcounter++;
	if(BLcounter>999)
	{
		BLcounter=0;
		HAL_BackLight_Control(i++);
		if(i==16)
		{
			i=0;
		}
	}
}
#endif