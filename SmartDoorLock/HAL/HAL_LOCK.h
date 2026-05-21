#ifndef _HAL_LOCK_H
#define _HAL_LOCK_H
void HAL_LOCK_Init();//初始化锁
void Hal_Lock_TriggerOpen(void);//触发开锁
void Hal_Lock_Pro(void);//锁处理函数
unsigned char Hal_Lock_GetPutKeySta(void);//获取锁的状态
void Hal_LockKey_Pro(void);//内部开关按键处理函数
#endif