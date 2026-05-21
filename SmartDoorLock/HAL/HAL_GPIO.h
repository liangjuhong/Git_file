#ifndef __HAL_GPIO_H
#define __HAL_GPIO_H
//Ê¹ÄÜLED²âÊÔº¯Êý
//#define HAL_MCU_LED_TEST_ENABLE
void HAL_GPIO_Init();

#ifdef HAL_MCU_LED_TEST_ENABLE
void HAL_MCU_LED_TEST();
#endif

#endif