#ifndef  __HCSR04_H
#define  __HCSR04_H

//超声波模块初始化
void sr04_init(void);

//超声波模块测距
int32_t sr04_get_distance(void);

#endif
