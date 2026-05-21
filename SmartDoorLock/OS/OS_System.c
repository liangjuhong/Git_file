#include "OS_SYSTEM.H"
volatile TaskTypedef xdata task[tasksum];
//任务初始化
void System_taskInit()
{
	unsigned char xdata i;
	for(i=0;i<tasksum;i++)
	{
		task[i].ptask=0;
		task[i].state=OS_taskSleep;
		task[i].RunPeriod=0;
		task[i].Runtimer=0;
	}
}
//任务创建
void System_taskcreat(void (*p)(void),TaskIDTypedef ID,unsigned short taskperiod)
{
	if(!task[ID].ptask)
	{
		task[ID].ptask=p;
		task[ID].state=OS_taskSleep;
		task[ID].RunPeriod=taskperiod;
		task[ID].Runtimer=0;
	}
}
//任务调度函数,函数应放在定时器中断中
void System_ClockInterrupt()
{
	unsigned char xdata i;
	for(i=0;i<tasksum;i++)
	{
		if(task[i].ptask)
		{
			task[i].Runtimer++;
			if(task[i].Runtimer>task[i].RunPeriod-1)
			{
				task[i].Runtimer=0;
				task[i].state=OS_taskRun;
			}
		}
	}
}
//任务启动函数
void System_TaskdStart()
{
	unsigned char xdata i;
	while(1)
	{
		for(i=0;i<tasksum;i++)
		{
			if(task[i].state == OS_taskRun)
			{
				task[i].state=OS_taskSleep;
				(*(task[i].ptask))();
			}
		}
	}
}
