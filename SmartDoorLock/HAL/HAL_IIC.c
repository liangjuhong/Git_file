/*
IIC驱动EEPROM：AT24C128芯片
FOSC:40Mhz
功能：
写入一个字节 HAL_IICWriteByte
写入多个字节 HAL_IICWritePage
读一个字节	 HAL_IICReadByte
读多个字节	 HAL_IICReaddata
*/
#include <STC8H.H>
#include "HAL_IIC.h"
sbit SDA=P7^6;//IIC数据线接口
sbit SCL=P7^7;//IIC时钟线接口
void HAL_IICInit()
{
	P_SW2 &= 0xCF;//B5,B4位清零
	P_SW2 |= 0x20;//IIC引脚切换，SDA：P7^6 SCL:P7^7
	I2CCFG=0xE0;//允许IIC功能，主机模式，IIC总线速度：FOSC/2/(MSSPEED*2+4) 294khz
	I2CMSCR=0x80;//允许主机模式的中断
	SDA=1;
	SCL=1;
	I2CMSST=0x00;
}
void HAL_IICWait()
{
	while(!(I2CMSST & 0x40));
	I2CMSST &= ~0x40;
}
void HAL_IICStart()
{
	I2CMSCR=0x01;//发送START信号
	HAL_IICWait();
}
void HAL_IICSendData(unsigned char dat)
{
	I2CTXD=dat;
	I2CMSCR=0x02;//发送数据
	HAL_IICWait();
}
void HAL_IICRecACK()
{
	I2CMSCR=0x03;//接收ACK信号
	HAL_IICWait();
}
unsigned char HAL_IICRecdata()
{
	I2CMSCR=0x04;//接收数据
	HAL_IICWait();
	return I2CRXD;
}
void HAL_IICSendACK()//发送ACK信号
{
	I2CMSST=0x00;
	I2CMSCR=0x05;
	HAL_IICWait();
}
void HAL_IICSendNACK()
{
	I2CMSST=0x01;
	I2CMSCR=0x05;
	HAL_IICWait();
}
void HAL_IICStop()
{
	I2CMSCR=0x06;//发送停止信号
	HAL_IICWait();
}

