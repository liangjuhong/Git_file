#ifndef _OS_SYSTEM_H
#define _OS_SYSTEM_H
typedef enum
{
	OS_taskSleep,
	OS_taskRun=!OS_taskSleep
}OS_TaskStateTypedef;//任务状态枚举类型
typedef enum
{
	task1,
	task2,
	tasksum
}TaskIDTypedef;//任务ID枚举类型
typedef struct
{
	void (*ptask)(void);//任务指针
	OS_TaskStateTypedef state;//任务状态
	unsigned short RunPeriod;//任务运行周期
	unsigned short Runtimer;//任务运行定时器
}TaskTypedef;//任务结构体类型
//任务初始化
void System_taskInit();//
//任务创建
void System_taskcreat(void (*p)(void),TaskIDTypedef ID,unsigned short taskperiod);
//任务调度函数,函数应放在定时器中断中
void System_ClockInterrupt();
//任务启动函数
void System_TaskdStart();


#endif
