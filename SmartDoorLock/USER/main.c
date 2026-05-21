#include "HAL_TIMER4.H"
#include "HAL_TASK.H"
#include "MT_TASK.H"
#include "APP_TASK.H" 
#include "HAL_PWMB_RFID.h"
#include <stdio.h>
#include <STC8H.H>

void main()
{
	HAL_Task_Init();
	MT_Task_Init();
	APP_Task_Init();
	printf("a");
	while(1)
	{
		Hal_PWMB_RFID_Pro();
		if(systick_ms==1)
		{
			systick_ms=0;
			HAL_Task();
			MT_Task();
			APP_Task();
		}
	}
}
// 自己写 fputc，不用管头文件有没有
int fputc(int ch)
{
    SBUF = ch;         // 串口发送
    while(!TI);        // 等待发完
    TI = 0;            // 清标志
    return ch;
}
