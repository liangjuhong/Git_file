#include <STC8H.H>
#include "HAL_LOCK.h"
//电控锁引脚定义
#define HAL_LOCK_PIN P72
#define LOCK_OPEN_LEVEL    1  // 高电平触发开锁
#define LOCK_CLOSE_LEVEL   0  // 低电平触发关锁

//内部开关按键定义，按下KEY1则为高电平，松开则为低电平
#define HAL_KEY1_PIN P34

//锁的开闭脉冲宽度，单位：ms
#define LOCK_OPEN_PULSE_MS  50 
#define KEY_DEBOUNCE_TIME_MS 5 // 10ms消抖时间
static unsigned int xdata g_u16LockOpenTimer = 0;
static unsigned char xdata g_u8KeyDebounceTimer = 0;
void HAL_LOCK_Init()
{
    P3M0 |=0x10;//P34配置为推挽输出模式
    HAL_KEY1_PIN=0;//设置KEY1为低电平

		P7M0 |=0x04;//设置P72为推挽输出模式
    g_u16LockOpenTimer = 0;//初始化软件定时器为0，表示锁未开
    HAL_LOCK_PIN=LOCK_CLOSE_LEVEL;//默认关闭锁
}
//触发开锁
void Hal_Lock_TriggerOpen(void)
{
    // 只有在当前不在开锁状态时，才允许触发新的开锁动作
    if (g_u16LockOpenTimer == 0)
    {
        HAL_LOCK_PIN = LOCK_OPEN_LEVEL; // 立即开锁
        g_u16LockOpenTimer = LOCK_OPEN_PULSE_MS; // 装填软件定时器
    }
}
//处理锁的开闭
void Hal_Lock_Pro(void)
{
    // 如果软件定时器不为0，则进行倒计时
    if (g_u16LockOpenTimer > 0)
    {
        g_u16LockOpenTimer--;
        // 当倒计时结束
        if (g_u16LockOpenTimer == 0)
        {
            HAL_LOCK_PIN = LOCK_CLOSE_LEVEL; // 自动关锁
        }
    }
}
void Hal_LockKey_Pro(void)
{
    if(HAL_KEY1_PIN)
    {///内部开关按下
        if(g_u8KeyDebounceTimer  < KEY_DEBOUNCE_TIME_MS )
        {
            g_u8KeyDebounceTimer  ++;
            if(g_u8KeyDebounceTimer   == KEY_DEBOUNCE_TIME_MS )
            {
                Hal_Lock_TriggerOpen();        
            }
        }
    }
    else
    {
        g_u8KeyDebounceTimer = 0;  
    }
}
//获取锁的状态
unsigned char Hal_Lock_GetPutKeySta(void)
{
    if(g_u16LockOpenTimer)
    {//非0 表示 电锁 打开
        return LOCK_OPEN_LEVEL;
    }
    else
    {
        return LOCK_CLOSE_LEVEL;
    }
}