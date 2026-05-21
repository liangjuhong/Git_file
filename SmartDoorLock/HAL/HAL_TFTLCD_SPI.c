#include <STC8H.H>
#include "HAL_TFTLCD_SPI.H"
void HAL_TFTLCD_SpiInit()//硬件SPI初始化
{
	HAL_CS_SET;     	//关闭片选信号 禁止对TFTLCD屏操作
	P_SW1 &= 0xF3;
	P_SW1 |= 0x08;
	//使能SPI主机模式 SPCTL.SSIG为零则由SS引脚来确认器件是主机还是从机，STC8H的SPI的SS引脚默认为1
	SPCTL = 0xD0; 		//1101 0000   CPOL\CPHA都为零
	SPSTAT = 0xc0;    //清中断标志
	HAL_SCL_CLR;			//时钟线空闲状态低电平
	HAL_DC_SET;				//默认写数据
}
void HAL_TFTLCD_BL_Control(HAL_TFTLCD_Typedef LCDState)
{
	if(LCDState==HAL_TFTLCD_ENABLE)
	{
		HAL_LEDK_SET;
	}
	else
	{
		HAL_LEDK_CLR;
	}
}
void HAL_TFTLCD_Init()
{
	HAL_RES_CLR; //复位脚拉低 TFTLCD 复位
//	HAL_delay_ms(100); //复位延时
	HAL_RES_SET; //复位脚拉高 TFTLCD正常工作
	
//	HAL_delay_ms(100); //复位后延时 
	HAL_TFTLCD_BL_Control(HAL_TFTLCD_DISABLE);//关闭背光
//	HAL_delay_ms(100); //打开背光位后延时 
	
	//************* Start Initial Sequence **********//
	HAL_TFTLCD_WRREG(0x11);    //TFTLCD退出休眠 Sleep out 
//	HAL_delay_ms(120);              //Delay 120ms 
	//------------------------------------ST7735S Frame Rate-----------------------------------------// 
	HAL_TFTLCD_WRREG(0xB1); 
	HAL_TFTLCD_WRData8(0x05); 
	HAL_TFTLCD_WRData8(0x3C); 
	HAL_TFTLCD_WRData8(0x3C); 
	HAL_TFTLCD_WRREG(0xB2); 
	HAL_TFTLCD_WRData8(0x05);
	HAL_TFTLCD_WRData8(0x3C); 
	HAL_TFTLCD_WRData8(0x3C); 
	HAL_TFTLCD_WRREG(0xB3); 
	HAL_TFTLCD_WRData8(0x05); 
	HAL_TFTLCD_WRData8(0x3C); 
	HAL_TFTLCD_WRData8(0x3C); 
	HAL_TFTLCD_WRData8(0x05); 
	HAL_TFTLCD_WRData8(0x3C); 
	HAL_TFTLCD_WRData8(0x3C); 
	//------------------------------------End ST7735S Frame Rate---------------------------------// 
	HAL_TFTLCD_WRREG(0xB4);  //Dot inversion 
	HAL_TFTLCD_WRData8(0x03); 
	//------------------------------------ST7735S Power Sequence---------------------------------// 
	HAL_TFTLCD_WRREG(0xC0); 
	HAL_TFTLCD_WRData8(0x28); 
	HAL_TFTLCD_WRData8(0x08); 
	HAL_TFTLCD_WRData8(0x04); 
	HAL_TFTLCD_WRREG(0xC1); 
	HAL_TFTLCD_WRData8(0XC0); 
	HAL_TFTLCD_WRREG(0xC2); 
	HAL_TFTLCD_WRData8(0x0D); 
	HAL_TFTLCD_WRData8(0x00); 
	HAL_TFTLCD_WRREG(0xC3); 
	HAL_TFTLCD_WRData8(0x8D); 
	HAL_TFTLCD_WRData8(0x2A); 
	HAL_TFTLCD_WRREG(0xC4); 
	HAL_TFTLCD_WRData8(0x8D); 
	HAL_TFTLCD_WRData8(0xEE); 
	//---------------------------------End ST7735S Power Sequence-------------------------------------// 
	HAL_TFTLCD_WRREG(0xC5); //VCOM 
	HAL_TFTLCD_WRData8(0x1A); 

	HAL_TFTLCD_WRREG(0x36); //MX, MY, RGB mode 
	#if(USE_HORIZONTAL==0)
		  HAL_TFTLCD_WRData8(0x00); //0b0000 0000
	#elif(USE_HORIZONTAL==1)
		  HAL_TFTLCD_WRData8(0xC0);//0b1100 0000
	#elif(USE_HORIZONTAL==2)
	    HAL_TFTLCD_WRData8(0x70);//0b0111 0000
	#else 
		  HAL_TFTLCD_WRData8(0xA0); //0b1010 0000
	#endif
	//------------------------------------ST7735S Gamma Sequence---------------------------------// 
	HAL_TFTLCD_WRREG(0xE0); 
	HAL_TFTLCD_WRData8(0x04); 
	HAL_TFTLCD_WRData8(0x22); 
	HAL_TFTLCD_WRData8(0x07); 
	HAL_TFTLCD_WRData8(0x0A); 
	HAL_TFTLCD_WRData8(0x2E); 
	HAL_TFTLCD_WRData8(0x30); 
	HAL_TFTLCD_WRData8(0x25); 
	HAL_TFTLCD_WRData8(0x2A); 
	HAL_TFTLCD_WRData8(0x28); 
	HAL_TFTLCD_WRData8(0x26); 
	HAL_TFTLCD_WRData8(0x2E); 
	HAL_TFTLCD_WRData8(0x3A); 
	HAL_TFTLCD_WRData8(0x00); 
	HAL_TFTLCD_WRData8(0x01); 
	HAL_TFTLCD_WRData8(0x03); 
	HAL_TFTLCD_WRData8(0x13); 
	HAL_TFTLCD_WRREG(0xE1); 
	HAL_TFTLCD_WRData8(0x04); 
	HAL_TFTLCD_WRData8(0x16); 
	HAL_TFTLCD_WRData8(0x06); 
	HAL_TFTLCD_WRData8(0x0D); 
	HAL_TFTLCD_WRData8(0x2D); 
	HAL_TFTLCD_WRData8(0x26); 
	HAL_TFTLCD_WRData8(0x23); 
	HAL_TFTLCD_WRData8(0x27); 
	HAL_TFTLCD_WRData8(0x27); 
	HAL_TFTLCD_WRData8(0x25); 
	HAL_TFTLCD_WRData8(0x2D); 
	HAL_TFTLCD_WRData8(0x3B); 
	HAL_TFTLCD_WRData8(0x00); 
	HAL_TFTLCD_WRData8(0x01); 
	HAL_TFTLCD_WRData8(0x04); 
	HAL_TFTLCD_WRData8(0x13); 
	//------------------------------------End ST7735S Gamma Sequence-----------------------------// 
	HAL_TFTLCD_WRREG(0x3A); //65k mode 
	HAL_TFTLCD_WRData8(0x05); //每个点16 bits 2个字节
	HAL_TFTLCD_WRREG(0x29); //Display on 打开TFTLCD屏幕的显示功能
}
void HAL_TFTLCD_WRData8(unsigned char dat)//写入8位数据
{
//	unsigned char xdata i;
	HAL_CS_CLR;//打开片选信号，开始操作LCD屏幕
	SPDAT=dat;
	while(!(SPSTAT & 0x80));
	SPSTAT = 0xc0;    //清中断标志1100
//	for(i=0;i<8;i++)
//	{
//		HAL_SCL_CLR;
//		if(dat&0x80)
//		{
//			HAL_SDA_SET;
//		}
//		else
//		{
//			HAL_SDA_CLR;
//		}
//		HAL_SCL_SET;
//		dat<<=1;
//	}
//HAL_SCL_CLR;
	HAL_CS_SET;
}
void HAL_TFTLCD_WRData16(unsigned int dat)//写入16位数据
{
	HAL_TFTLCD_WRData8(dat>>8);
	HAL_TFTLCD_WRData8(dat);
}
void HAL_TFTLCD_WRREG(unsigned char dat)
{
	HAL_DC_CLR;
	HAL_TFTLCD_WRData8(dat);
	HAL_DC_SET;
}