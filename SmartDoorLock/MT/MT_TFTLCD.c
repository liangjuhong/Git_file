#include "MT_TFTLCD.H"
#include "HAL_TFTLCD_SPI.H"
#include "MT_TFTLCDPONT.H"
//设置屏幕范围
void MT_TFTLCD_ADDRESS_SET(unsigned int xsta,unsigned int ysta,unsigned int xend,unsigned int yend)
{
	#if (USE_HORIZONTAL==0)
	{
		HAL_TFTLCD_WRREG(0x2a);//列地址设置
		HAL_TFTLCD_WRData16(xsta+2); //x起始地址
		HAL_TFTLCD_WRData16(xend+2); //x结束地址
		
		HAL_TFTLCD_WRREG(0x2b);//行地址设置
		HAL_TFTLCD_WRData16(ysta+1); //y起始地址
		HAL_TFTLCD_WRData16(yend+1);	//y结束地址	
		
		HAL_TFTLCD_WRREG(0x2c);//储存器写
	}
	#elif (USE_HORIZONTAL==1)
	{
		HAL_TFTLCD_WRREG(0x2a);//列地址设置
		HAL_TFTLCD_WRData16(xsta+2);
		HAL_TFTLCD_WRData16(xend+2);
		HAL_TFTLCD_WRREG(0x2b);//行地址设置
		HAL_TFTLCD_WRData16(ysta+1);
		HAL_TFTLCD_WRData16(yend+1);
		HAL_TFTLCD_WRREG(0x2c);//储存器写
	}
	#elif (USE_HORIZONTAL==2)
	{
		HAL_TFTLCD_WRREG(0x2a);//列地址设置
		HAL_TFTLCD_WRData16(xsta+1);
		HAL_TFTLCD_WRData16(xend+1);
		HAL_TFTLCD_WRREG(0x2b);//行地址设置
		HAL_TFTLCD_WRData16(ysta+2);
		HAL_TFTLCD_WRData16(yend+2);
		HAL_TFTLCD_WRREG(0x2c);//储存器写
	}
	#else
	{
		HAL_TFTLCD_WRREG(0x2a);//列地址设置
		HAL_TFTLCD_WRData16(xsta+1);
		HAL_TFTLCD_WRData16(xend+1);
		HAL_TFTLCD_WRREG(0x2b);//行地址设置
		HAL_TFTLCD_WRData16(ysta+2);
		HAL_TFTLCD_WRData16(yend+2);
		HAL_TFTLCD_WRREG(0x2c);//储存器写
	}
	#endif
}
//给指定范围填充背景颜色
//xsta X轴起始坐标，ysta Y轴起始坐标，xend X轴结束坐标，yend Y轴结束坐标
void MT_TFTLCD_BColorfill(unsigned int xsta,unsigned int ysta,unsigned int xend,unsigned int yend,unsigned int color)
 {
	unsigned char xdata i,j;
	MT_TFTLCD_ADDRESS_SET(xsta,ysta,xend-1,yend-1);//屏幕显示范围
	for(i=ysta;i<yend;i++)
	{
		for(j=xsta;j<xend;j++)
		{
			HAL_TFTLCD_WRData16(color);
		}
	}
}
//画线
void MT_TFTLCD_DrawLine(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned int color)
{
	unsigned int xdata t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1;
	uRow=x1;//画线起点坐标
	uCol=y1;
	if(delta_x>0)
		incx=1; //设置单步方向 
	else if (delta_x==0)
		incx=0;//垂直线 
	else 
	{
		incx=-1;
		delta_x=-delta_x;
	}

	if(delta_y>0)
		incy=1;
	else if (delta_y==0)
		incy=0;//水平线 
	else 
	{
		incy=-1;
		delta_y=-delta_y;
	}

	if(delta_x>delta_y)
		distance=delta_x; //选取基本增量坐标轴 
	else 
		distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		MT_TFTLCD_PointColor(uRow,uCol,color);//画点
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}
//添加指定颜色的光标
void MT_TFTLCD_PointColor(unsigned char x,unsigned char y,unsigned int color)
{
	MT_TFTLCD_ADDRESS_SET(x,y,x,y);
	HAL_TFTLCD_WRData16(color);
}

//显示一个字符
/*
显示X轴起始值：xsta
显示Y轴起始值：ysta
字符串首地址：p
字体颜色：fcolor
背景颜色：bcolor
字体尺寸：fsizey
模式：mode 0非叠加模式 1叠加模式
*/
void MT_TFTLCD_ShowChar(unsigned char xsta,unsigned char ysta,unsigned short num,unsigned short fcolor,unsigned short bcolor,unsigned char fsizey,unsigned char mode)
{
	unsigned char xdata i,m,x0,t;
	unsigned short xdata temp,numx;
	unsigned char xdata fsizex;//字符水平方向像素
	unsigned char xdata fontnumber;//一个字符占有的字节数
	num=num-' ';//传入字符的偏移量
	numx=num-'0';
	x0=xsta;
	fsizex=fsizey/2;
	fontnumber=(fsizex/8+((fsizex%8)?1:0))*fsizey;
	MT_TFTLCD_ADDRESS_SET(xsta,ysta,xsta+fsizex-1,ysta+fsizey-1);//设置一个字符的屏显范围
	for(i=0;i<fontnumber;i++)
	{
		if(fsizey==12)
			temp=ascii_1206[num][i];
		else if(fsizey==16)
			temp=ascii_1608[num][i];
		else if(fsizey==24)
			temp=ascii_2412[num][i];
		else if(fsizey==32)
			temp=ascii_3216[num][i];
		else if(fsizey==48)
			temp=ascii_4824[numx][i];
		else
			return;
		for(m=0;m<8;m++)
		{
			if(!mode)//非叠加模式，具有自动刷新的功能
			{
				if(temp & (0x01<<m) )
				HAL_TFTLCD_WRData16(fcolor);//字体颜色
				else
				HAL_TFTLCD_WRData16(bcolor);//背景颜色
				t++;
				if(t%fsizex==0)//一个字符的水平像素为fsizex
				{
					t=0;
					break;
				}
			}
			else//叠加模式，没有自动刷新的功能
			{
				if(temp & (0x01<<m) )
				MT_TFTLCD_PointColor(xsta,ysta,fcolor);
				xsta++;
				if((xsta-x0)==fsizex)
				{
					xsta=x0;
					ysta++;
					break;
				}
			}
		}
	}
}
/*
显示字符串(屏幕为横屏时)
*/

void MT_TFTLCD_Showstring(unsigned char xsta,unsigned char ysta,const unsigned char *p,unsigned short fcolor,unsigned short bcolor,unsigned char fsizey,unsigned char mode)
{
//	unsigned short xdata number;
	while(*p != '\0')
	{
		MT_TFTLCD_ShowChar(xsta,ysta,*p,fcolor,bcolor,fsizey,mode);
		p++;
		xsta+=fsizey/2;
//		number+=fsizey/2;
//		switch(fsizey)
//		{
//			case 12:if(number>154)//一个12号字的水平像素为6
//							{
//								number=0;
//								xsta=0;
//								ysta+=fsizey;
//							}
//							break;
//			case 16:if(number>152)//一个16号字的水平像素为8
//							{
//								number=0;
//								xsta=0;
//								ysta+=fsizey;
//							}
//							break;
//			case 24:if(number>148)//一个24号字的水平像素为12
//							{
//								number=0;
//								xsta=0;
//								ysta+=fsizey;
//							}
//							break;
//			case 32:if(number>144)//一个32号字的水平像素为16
//							{
//								number=0;
//								xsta=0;
//								ysta+=fsizey;
//							}
//							break;
//			default :break;
//			
//		}	
	}
}

/*显示图片,图片像素为24*30*/
void MT_TFTLCD_ShowPicture(unsigned short xsta,unsigned short ysta,unsigned short xlen,unsigned short ylen, const unsigned char *p,unsigned short fcolor,unsigned short bcolor)
{
	unsigned char xdata i,j,t,x0;
	unsigned short xdata temp;
	x0=xlen/8;
	MT_TFTLCD_ADDRESS_SET(xsta,ysta,xsta+xlen-1,ysta+ylen-1);
	for(i=0;i<x0;i++)
	{
		for(j=0;j<ylen;j++)
		{
			temp=*p;
			for(t=0;t<8;t++)
			{
				if(temp&0x80)
				{
					HAL_TFTLCD_WRData16(fcolor);
				}
				else
				{
					HAL_TFTLCD_WRData16(bcolor);
				}
				temp<<=1;
			}
			p++;
		}
	}
}
//显示开门，关门图片
void MT_TFTLCD_DoorPic(unsigned char i)
{
	switch(i)
	{
		case 0:
			MT_TFTLCD_ShowPicture(5,0,24,30,pic_doorState[0],WHITE,BLACK);
			break;
		case 1:
			MT_TFTLCD_ShowPicture(5,0,24,30,pic_doorState[1],WHITE,BLACK);
			break;
		case 2:
			MT_TFTLCD_ShowPicture(5,0,24,30,pic_doorState[2],WHITE,BLACK);
			break;
	}
}
