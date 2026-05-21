#ifndef _HAL_TFTLCD_SPI_H
#define _HAL_TFTLCD_SPI_H
#include <STC8H.H>
#define USE_HORIZONTAL   2  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏
//#define USE_HORIZONTAL 0  竖屏 正常显示的  0°
//#define USE_HORIZONTAL 1  竖屏 旋转180°
//#define USE_HORIZONTAL 2  横屏 顺时针旋转了270；    
//#define USE_HORIZONTAL 3  横屏 顺时针选择了90； 
#define HAL_TFTLCD_LEDK P74//高电平点亮背光灯
#define HAL_TFTLCD_RES P73//低电平复位
#define HAL_TFTLCD_DC P42//0写入指令，1写入数据
#define HAL_TFTLCD_SDA P40//数据引脚
#define HAL_TFTLCD_SCL P43
#define HAL_TFTLCD_CS P44  //低电平使能
typedef enum
{
	HAL_TFTLCD_DISABLE,
	HAL_TFTLCD_ENABLE
}HAL_TFTLCD_Typedef;
#define HAL_LEDK_CLR HAL_TFTLCD_LEDK=0
#define HAL_LEDK_SET HAL_TFTLCD_LEDK=1

#define HAL_RES_CLR HAL_TFTLCD_RES=0
#define HAL_RES_SET HAL_TFTLCD_RES=1

#define HAL_DC_CLR HAL_TFTLCD_DC=0
#define HAL_DC_SET HAL_TFTLCD_DC=1

#define HAL_SDA_CLR HAL_TFTLCD_SDA=0
#define HAL_SDA_SET HAL_TFTLCD_SDA=1

#define HAL_SCL_CLR HAL_TFTLCD_SCL=0
#define HAL_SCL_SET HAL_TFTLCD_SCL=1

#define HAL_CS_CLR HAL_TFTLCD_CS=0
#define HAL_CS_SET HAL_TFTLCD_CS=1
//void HAL_delay_ms(unsigned int ms);
void HAL_TFTLCD_Init();
void HAL_TFTLCD_SpiInit();//硬件SPI初始化
void HAL_TFTLCD_WRData8(unsigned char dat);//写入8位数据
void HAL_TFTLCD_WRData16(unsigned int dat);//写入16位数据
void HAL_TFTLCD_WRREG(unsigned char dat);//写入命令
void HAL_TFTLCD_BL_Control(HAL_TFTLCD_Typedef LCDState);
#endif