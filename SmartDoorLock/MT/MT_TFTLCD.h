#ifndef _MT_TFTLCD_H
#define _MT_TFTLCD_H


//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40 //棕色
#define BRRED 			     0XFC07 //棕红色
#define GRAY  			     0X8430 //灰色
#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY 			     0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)

//设置地址
void MT_TFTLCD_ADDRESS_SET(unsigned int xsta,unsigned int ysta,unsigned int xend,unsigned int yend);
//填充颜色
void MT_TFTLCD_BColorfill(unsigned int xsta,unsigned int ysta,unsigned int xend,unsigned int yend,unsigned int color);
//画点
void MT_TFTLCD_PointColor(unsigned char x,unsigned char y,unsigned int color);
//显示一个字符
void MT_TFTLCD_ShowChar(unsigned char xsta,unsigned char ysta,unsigned short num,unsigned short fcolor,unsigned short bcolor,unsigned char fsizey,unsigned char mode);
//显示一个字符串
void MT_TFTLCD_Showstring(unsigned char xsta,unsigned char ysta,const unsigned char *p,unsigned short fcolor,unsigned short bcolor,unsigned char fsizey,unsigned char mode);
//显示一个图片
void MT_TFTLCD_ShowPicture(unsigned short xsta,unsigned short ysta,unsigned short xlen,unsigned short ylen,const unsigned char *p,unsigned short fcolor,unsigned short bcolor);
//显示一个门锁图片
void MT_TFTLCD_DoorPic(unsigned char i);
//画线
void MT_TFTLCD_DrawLine(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned int color);
#endif