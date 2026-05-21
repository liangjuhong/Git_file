#include "HAL_UART.H"
#include <STC8H.H>
#define UART_BAUDRATE (65536-(MAIN_Fosc/4/115200))
void HAL_UART1_Init()
{
    P_SW1&=0x0F;
    SCON&=0x3F;//8位数据,1位停止位,无校验位
    SCON|=0x50;//串口1模式1，允许串口接收数据
    AUXR&=0xFE;//串口1波特率发生器配置为定时器1
    //定时器1配置
    TMOD&=0x0F;//定时器1配置为模式0
    TH1=UART_BAUDRATE>>8;
    TL1=UART_BAUDRATE;
    AUXR|=0x40;//定时器1为1T模式
    TR1=1;
}
void HAL_UART1_SendByte(unsigned char dat)
{
    SBUF=dat;
    while(!TI);
    TI=0;
}
void HAL_UART1_SendString(unsigned char *dat)
{
    while(*dat)
    {
        HAL_UART1_SendByte(*dat);
        dat++;
    }
}
