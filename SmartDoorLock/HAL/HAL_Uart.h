#ifndef __HAL_UART_H
#define __HAL_UART_H
void HAL_UART1_Init();
void HAL_UART1_SendByte(unsigned char dat);
void HAL_UART1_SendString(unsigned char *dat);
#endif