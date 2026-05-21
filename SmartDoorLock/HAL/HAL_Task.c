#include <STC8H.H>
#include "HAL_TASK.H"
#include "HAL_BACKLIGHT.H"
#include "HAL_TIMER4.H"
#include "HAL_GPIO.H"
#include "HAL_TOUCHKEY.H" 
#include "HAL_TFTLCD_SPI.H"
#include "HAL_DS1302.H"
#include "HAL_PWMA_HORN.H"
#include "HAL_PWMB_RFID.H"
#include "HAL_UART.H"
#include "HAL_RS485.H"
#include "HAL_WIEGAND.H"
#include "HAL_WIEGAND_RECEIVE.H"
#include "HAL_LOCK.H"
#include "HAL_IIC.h"
void HAL_Task_Init()
{
	P_SW2 |=0x80;//访问外部特殊功能寄存器
	HAL_GPIO_Init();//IO口初始化
	HAL_Timer4_Init();//定时器4初始化函数,定时器4用作时间片轮询基准
	HAL_BackLight_All_OFF();//背光灯熄灭
	HAL_TouchKey_Init();//触摸按键初始化
	HAL_TFTLCD_SpiInit();//SPI初始化
	HAL_TFTLCD_Init();//TFT液晶屏初始化
	HAL_PWMA_Init();//PWMA喇叭初始化
	HAL_PWMB_RFID_Init();//RFID功能初始化
	HAL_UART1_Init();//串口1初始化
	HAL_RS485_Init();//RS485硬件初始化
	HAL_Wiegand_Init();//Wiegand初始化
	HAL_Wiegand_Receive_Init();//接收Wiegand传来的数据
	HAL_LOCK_Init();//锁初始化
	HAL_IICInit();//IIC初始化
}
void HAL_Task()
{
	HAL_TouchKey_pro();//处理触摸按键
	HAL_RS485_Process();//处理接收数据
	HAL_WG_process();//处理Wiegand发送数据包
	HAL_Wiegand_Receive_Process();//处理Wiegand传来的数据
	Hal_Lock_Pro();//处理锁的开闭
	Hal_LockKey_Pro();//处理内部开关按键
	#ifdef HAL_BACKLIGHT_TEST_ENABLE
	HAL_BackLight_TEST();
	#endif
	#ifdef HAL_MCU_LED_TEST_ENABLE
  	HAL_MCU_LED_TEST();
	#endif
}
