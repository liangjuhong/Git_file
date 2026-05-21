#ifndef _MT_DS1302_H
#define _MT_DS1302_H
#define HAL_second_W 0x80
#define HAL_minute_W 0x82
#define HAL_hour_W   0x84
#define HAL_day_W    0x86
#define HAL_week_W   0x8A
#define HAL_month_W  0x88
#define HAL_year_W   0x8C
#define HAL_Write_protect_W   0x8E
#define HAL_charge_W 0x90

#define HAL_second_R 0x81
#define HAL_minute_R 0x83
#define HAL_hour_R   0x85
#define HAL_day_R    0x87
#define HAL_week_R   0x8B
#define HAL_month_R  0x89
#define HAL_year_R   0x8D
typedef struct
{
	unsigned char second;
	unsigned char minute;
	unsigned char hour;
	unsigned char day;
	unsigned char month;
	unsigned int year;
	unsigned char week;
}MT_TIME_Typedef;
extern MT_TIME_Typedef xdata TIME_Task;
//写入时间
void MT_DS1302_SET_Time();
//读取时间
void MT_DS1302_Read_Time();
void MT_DS1302_Init();
//BCD码转为字符串
void MT_DS1302_BCDTOString(unsigned char dat,unsigned char *datBuff);
#endif