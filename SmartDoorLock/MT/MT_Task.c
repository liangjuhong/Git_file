#include "MT_TASK.H"
#include "MT_TK_LED.H"
#include "MT_TFTLCD.H"
#include "MT_DS1302.H"
#include "HAL_TFTLCD_SPI.H"
#include "MT_MODBUS.H"
#include "MT_PWMB_RFID.H"
void MT_Task_Init()
{
	MT_TKLED_Init();
	MT_TFTLCD_BColorfill(0,0,160,128,BLACK);
	HAL_TFTLCD_BL_Control(HAL_TFTLCD_ENABLE);
	MT_DS1302_Init();
	MT_PWMB_RFID_Init();
	mt_modbus_Init();
}
void MT_Task()
{
	MT_PWMB_RFID_Process();//处理RFID数据
	MT_TKLED_pro();//触摸按键更新函数
	mt_modbus_Pro();//MODBUS更新函数
}
