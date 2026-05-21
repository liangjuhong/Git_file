#ifndef __LIGHT_SENSOR_H
#define __LIGHT_SENSOR_H

#define LW_1 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)//读取PB12电平
#define LW_2 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)//读取PB13电平
#define LW_3 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)//读取PB14电平
//#define LW_4 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) //读取PB1电平

void LightSensor_Init(void);
void LightSensor_Start(void);

#endif
