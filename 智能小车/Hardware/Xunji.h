#ifndef __XUNJI_H
#define __XUNJI_H

#define HW_1 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)//读取PB3电平
#define HW_2 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)//读取PB3电平
#define HW_3 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)//读取PB5电平
#define HW_4 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)//读取PB6电平

void Xunji_Init(void);
void Xunji_Start(void);

#endif
