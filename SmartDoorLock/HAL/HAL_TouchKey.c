#include <STC8H.H>
#include "HAL_TOUCHKEY.H"
unsigned int code DIFF[]={5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000};//16个通道差值
typedef struct
{
	unsigned char zero_Flag;//零点标志位,1则代表已完成零度读取
	unsigned char zero_cnt;//零点计数
	unsigned char ReadTouchkeyok;//扫描16个通道完成标志位
	unsigned char ScanFreq;//按键扫描次数阈值
	unsigned int TK_cnt[HAL_Touchkey_SUM];//存储按键实时数值
	unsigned int TK_zero[HAL_Touchkey_SUM];//存储零点数值
	unsigned int TK_differ[HAL_Touchkey_SUM];//触发阈值
//	unsigned int TK_lowest[HAL_Touchkey_SUM];//最低有效阈值
	unsigned char Zero_UP_cnt[HAL_Touchkey_SUM];//先延时再执行零点上移
	unsigned char Zero_Down_cnt[HAL_Touchkey_SUM];//先延时再零点下移计数器
	unsigned char TK_counter[HAL_Touchkey_SUM];//延时消抖
	unsigned char touchkey_state[HAL_Touchkey_SUM];//按键状态，1代表按键按下
}HAL_TouchKey_Typedef;

HAL_TouchKey_Typedef xdata TouchKey_Task;
void HAL_TouchKey_Init()
{
	unsigned char xdata i;
	TSCHEN1=0xFF;//使能16个触摸按键通道
	TSCHEN2=0xFF;
	//0000 0111   0111 0011
	TSCFG1  = (7<<4) + 3;	//开关电容工作频率 = fosc/(2*(TSCFG1[6:4]+1)), 放电时间(系统时钟周期数) 0(125) 1(250) 2(500) 3(1000) 4(2000) 5(2500) 6(5000) 7(7500) 最小3
	TSCFG2  = 2;
	
	TSRT=0x00;//用于配置触摸按键和LED驱动器分时多工
	TSCTRL =0xA3;//触摸按键重复扫描，重复扫描次数为4次
	IE2 |=0x80;//开启触摸按键中断
	EA=1;
	for(i=0;i<HAL_Touchkey_SUM;i++)
	{
		TouchKey_Task.TK_differ[i]=DIFF[i];//零点与触摸值的差值
//		TouchKey_Task.TK_lowest[i]=DIFF[i]*3;//触摸按键最低阈值
		TouchKey_Task.Zero_UP_cnt[i]=0;
		TouchKey_Task.Zero_Down_cnt[i]=0;
		TouchKey_Task.TK_counter[i]=0;
		TouchKey_Task.touchkey_state[i]=0;
	}
	TouchKey_Task.zero_cnt=0;
	TouchKey_Task.zero_Flag=0;
	TouchKey_Task.ReadTouchkeyok=0;
	TouchKey_Task.ScanFreq=HAL_Touchkey_cnt;//零点扫描次数
}

void HAL_TouchKey_pro()
{
	unsigned char xdata TKj;
	if(TouchKey_Task.ReadTouchkeyok)
	{
		TouchKey_Task.ReadTouchkeyok=0;
		if(!TouchKey_Task.zero_Flag)
		{
			HAL_TouchKey_Set_Zero();
			return;
		}
		else
		{
			for(TKj=0;TKj<HAL_Touchkey_SUM;TKj++)
			{
				if(TouchKey_Task.TK_cnt[TKj]>TouchKey_Task.TK_zero[TKj])//触摸按键未按下，零点上移
				{
					TouchKey_Task.Zero_Down_cnt[TKj]=0;
					if(TouchKey_Task.Zero_UP_cnt[TKj]<20)//先延时，在执行零点跟随
					{
						TouchKey_Task.Zero_UP_cnt[TKj]+=1;
					}
					else
						TouchKey_Task.TK_zero[TKj]++;
					
					if(TouchKey_Task.TK_counter[TKj]>0)
						TouchKey_Task.TK_counter[TKj]--;
					else
						TouchKey_Task.touchkey_state[TKj]=0;
				}//从通道零开始依次比较至通道15
				else if(TouchKey_Task.TK_cnt[TKj]<TouchKey_Task.TK_zero[TKj]-TouchKey_Task.TK_differ[TKj])//触摸按键已按下
				{
					TouchKey_Task.Zero_UP_cnt[TKj]=0;//零点跟随计数清零
					TouchKey_Task.Zero_Down_cnt[TKj]=0;
					
					TouchKey_Task.TK_counter[TKj]++;//触摸按键延时消抖
					if(TouchKey_Task.TK_counter[TKj]>HAL_Touchkey_Timercnt)
					TouchKey_Task.touchkey_state[TKj]=1;//表示通道TKJ已经按下
				}
				else
				{
					TouchKey_Task.Zero_UP_cnt[TKj]=0;
					if(TouchKey_Task.Zero_Down_cnt[TKj]<100)//先延时，在执行零点跟随，零点下移
					{
						TouchKey_Task.Zero_Down_cnt[TKj]++;
					}
					else
					{
						if(TouchKey_Task.TK_cnt[TKj]<TouchKey_Task.TK_zero[TKj])
						{
							TouchKey_Task.TK_zero[TKj]-=1;
						}
					}
					
					if(TouchKey_Task.TK_counter[TKj]>0)
						TouchKey_Task.TK_counter[TKj]--;
					else
						TouchKey_Task.touchkey_state[TKj]=0;
				}
			}
		}
		
	}
}
void HAL_TouchKey_Set_Zero()
{
	unsigned char xdata TKi;
	if(!TouchKey_Task.zero_Flag)
	{
		//先扫描零点触摸值20次(TouchKey_Task.ScanFreq),在将值放入零点值缓冲区
		if(TouchKey_Task.zero_cnt>TouchKey_Task.ScanFreq)
		{
			TouchKey_Task.zero_Flag=1;
			for(TKi=0;TKi<HAL_Touchkey_SUM;TKi++)
			{
				TouchKey_Task.TK_zero[TKi]=TouchKey_Task.TK_cnt[TKi];
			}
		}
	}
}
unsigned char HAL_TouchKey_GetState(unsigned char TKnum)
{
	if(TouchKey_Task.touchkey_state[TKnum])
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

#ifdef HAL_TOUCHKEY_TEST_ENABLE

#endif
void HAL_TouchKey_Interrupt() interrupt 35
{
	unsigned int xdata databuff;
	databuff=TSSTA2;
	if(databuff&0x40)
	{
		TSSTA2 |=0x40;
	}
	if(databuff&0x80)
	{
		TSSTA2 |=0x80;
		databuff &=0x0F;//TSSTA2前四位是已扫描完成通道值
		TouchKey_Task.TK_cnt[databuff]=TSDAT;
		if(databuff == HAL_Touchkey_SUM-1)
		{
			TouchKey_Task.zero_cnt++;
			TouchKey_Task.ReadTouchkeyok=1;
		}
	}
}