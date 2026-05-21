#include "MT_DS1302.H"
#include "HAL_DS1302.H"
MT_TIME_Typedef xdata TIME_Task={0x59,0x59,0x23,0x28,0x03,0x26,6};
//写入时间
void MT_DS1302_SET_Time()
{
	HAL_DS1302_W_DATA(HAL_Write_protect_W,0x00);//关闭写保护
	HAL_DS1302_W_DATA(HAL_second_W,TIME_Task.second);
	HAL_DS1302_W_DATA(HAL_minute_W,TIME_Task.minute);
	HAL_DS1302_W_DATA(HAL_hour_W,TIME_Task.hour);
	HAL_DS1302_W_DATA(HAL_day_W,TIME_Task.day);
	HAL_DS1302_W_DATA(HAL_month_W,TIME_Task.month);
	HAL_DS1302_W_DATA(HAL_year_W,TIME_Task.year);
	HAL_DS1302_W_DATA(HAL_week_W,TIME_Task.week);
	HAL_DS1302_W_DATA(HAL_charge_W,0xB5);//禁止充电
	HAL_DS1302_W_DATA(HAL_Write_protect_W,0x80);//打开写保护
}
//读取时间
void MT_DS1302_Read_Time()
{
	TIME_Task.second=HAL_DS1302_R_DATA(HAL_second_R);
	TIME_Task.minute=HAL_DS1302_R_DATA(HAL_minute_R);
	TIME_Task.hour=HAL_DS1302_R_DATA(HAL_hour_R);
	TIME_Task.day=HAL_DS1302_R_DATA(HAL_day_R);
	TIME_Task.month=HAL_DS1302_R_DATA(HAL_month_R);
	TIME_Task.year=HAL_DS1302_R_DATA(HAL_year_R);
	TIME_Task.week=HAL_DS1302_R_DATA(HAL_week_R);
}
void MT_DS1302_Init()
{
	unsigned char dat;
	unsigned char stat;
	stat = 0;
	MT_DS1302_Read_Time();
	dat = ((TIME_Task.month & 0x0f) + (TIME_Task.month >> 4)*10);	
    if((dat > 0) && (dat < 13))
	{ ///月份有效取值1-12		
	}
	else
	{
		stat = 1;
	}
	dat = ((TIME_Task.day & 0x0f) + (TIME_Task.day >> 4)*10);		
    if((dat > 0) && (dat < 32))
	{ ///日1-31	  此处还可以进一步优化； vvcbcv
	}
	else
	{
		stat = 1;
	}	
	dat = ((TIME_Task.hour & 0x0f) + (TIME_Task.hour >> 4)*10);			
    if(dat < 24)
	{ ///小时 0-23
	}
	else
	{
		stat = 1;
	}	
	dat = ((TIME_Task.minute & 0x0f) + (TIME_Task.minute >> 4)*10);		
    if(dat < 60)
	{ ///分钟 0-59
	}
	else
	{
		stat = 1;
	}		 
	dat = ((TIME_Task.second & 0x0f) + (TIME_Task.second >> 4)*10);	
  if(dat < 60)
	{ ///秒 0-59
	}
	else
	{
		stat = 1;
	}	
	if(stat)
	{
		TIME_Task.year = 0x23;
		TIME_Task.month = 0x11;
		TIME_Task.day = 0x23;
		TIME_Task.hour = 0x23;
		TIME_Task.minute = 0x59;
		TIME_Task.second = 0x59;
		TIME_Task.week = 4;
		MT_DS1302_SET_Time();	
	}	
}
void MT_DS1302_BCDTOString(unsigned char dat,unsigned char *datBuff)
{
	datBuff[0]=(((dat>>4)&0x0F)+0x30);
	datBuff[1]=((dat&0x0F)+0x30);
	datBuff[2]=0;
//	datBuff[0] = (((dat >> 4) & 0x0f) + 0x30);
//	datBuff[1] = ((dat & 0x0f) + 0x30);
//	datBuff[2] = 0;
}