#include <STC8H.H>
#include "HAL_DS1302.H"
#define HAL_DS1302_RST P71		//高电平可以操作DS1302芯片
#define HAL_DS1302_IO  P64		//数据线
#define HAL_DS1302_CLK  P65   //时钟线
//低电平写数据，上升沿数据写入DS1302
void HAL_DS1302_W_Byte(unsigned char dat)
{
	unsigned char xdata i;
	for(i=0;i<8;i++)
	{
		HAL_DS1302_CLK=0;
		if(dat&0x01)
			HAL_DS1302_IO=1;
		else
			HAL_DS1302_IO=0;
		dat>>=1;
		HAL_DS1302_CLK=1;
	}
}
//下降沿DS1302送出数据
unsigned char HAL_DS1302_R_Byte()
{
	unsigned char dat,j;
	dat=0;
	for(j=0;j<8;j++)
	{
		HAL_DS1302_CLK=0;
		dat>>=1;
		if(HAL_DS1302_IO)
			dat|=0x80;
		HAL_DS1302_CLK=1;
	}
	return dat;
}
void HAL_DS1302_W_DATA(unsigned char add,unsigned char dat)
{
	HAL_DS1302_RST=0;
	HAL_DS1302_CLK=0;
	HAL_DS1302_RST=1;
	HAL_DS1302_W_Byte(add);
	HAL_DS1302_W_Byte(dat);
	HAL_DS1302_CLK=0;
	HAL_DS1302_RST=0;
}
unsigned char HAL_DS1302_R_DATA(unsigned char add)
{
	unsigned char dat;
	HAL_DS1302_RST=0;
	HAL_DS1302_CLK=0;
	HAL_DS1302_RST=1;
	HAL_DS1302_W_Byte(add);
	dat=HAL_DS1302_R_Byte();
	HAL_DS1302_CLK=0;
	HAL_DS1302_RST=0;
	return dat;
}
