#ifndef __HAL_TIMER4_H
#define __HAL_TIMER4_H
#define FOSC 40000000L
//¶¨Ê±1ms
#define Timer4_Initial_Value (65536-FOSC/1000/12)
extern unsigned char systick_ms;
void HAL_Timer4_Init();
#endif