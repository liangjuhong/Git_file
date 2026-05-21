#include <STC8H.H>
#include "HAL_PWMB_RFID.H"
#include "string.h"
#define STATE_BASE  0  /* 基点检测状态（等待有效起始信号）*/
#define STATE_DATA  1  /* 数据采集状态（正在解析数据流）*/
#define HAL_RFIF_OUT P22
#define HAL_Timer0_125KHZ (65536-MAIN_Fosc/250000L)
HAL_RFID_TaskTypedef xdata RFID_Task;
static void Hal_PWMB_RFID_DatResetState(void);
static void Hal_PWMB_RFID_Process(unsigned char input);
static unsigned int Hal_PWMB_Signal_Analysis(unsigned int cnt, unsigned char is_falling);
void HAL_PWMB_RFID_Init()
{
	HAL_Timer0_Init();
	HAL_PWMB_Config();
	HAL_RFIF_OUT=1;
	RFID_Task.last_capture=0;      // 上次捕获的计数器值
	RFID_Task.current_capture=0;
	RFID_Task.state=STATE_BASE;       //RFID解析状态
 	RFID_Task.count_256=0;    //连续256μs脉冲计数器（用于处理空跳）  
 	RFID_Task.rfData_high_Low=0;    //检测的有效数据   有效取值0-1； 0xFF表示无效数据
	Hal_PWMB_RFID_DatResetState();
}
//定时器0用于产生125kHZ频率
void HAL_Timer0_Init()
{
	TMOD &=0xF0;//定时器0模式0
	TH0=HAL_Timer0_125KHZ>>8;
	TL0=HAL_Timer0_125KHZ;
	AUXR|=0x80;//1T模式
	INTCLKO|=0x01;//使能P3.5口的定时器0时钟输出功能
	TR0=1;
}
void HAL_PWMB_Config()
{
	PWMB_CCER2=0;//配置通道模式前应该先关闭通道使能
	PWMB_CCMR3=0x01;//IC7映射在TI7FP7上
	PWMB_CCMR4=0x02;//IC8映射在TI7FP8上
	PWMB_CCER2|=0x31;//使能CC7,CC8捕获功能，CC7捕获上升沿，CC8捕获下降沿
	
	PWMB_PSCRH=0;
	PWMB_PSCRL=0x27;//预分频为（39+1） 40000000/40=1MHZ
	
	PWMB_IER|=0x18;//允许通道7，8捕获中断
	PWMB_CR1|=0x01;//使能PWMB计数器
	EA=1;
}
static void Hal_PWMB_RFID_DatResetState(void)
{
	RFID_Task.current_state=STATE_HEADER_DETECT;
	RFID_Task.header_counter=0;
	RFID_Task.data_position=0;
	RFID_Task.row_parity_sum=0;
	memset(RFID_Task.col_parity_sum,0,sizeof(RFID_Task.col_parity_sum));
}
//接收RFID数据
static void Hal_PWMB_RFID_Process(unsigned char input)
{
 static unsigned char xdata row=0;  /* 当前数据位所在行(0-10) */
 static unsigned char xdata col=0;  /* 当前数据位所在列(0-4) */
  unsigned char xdata bit_pos; /* 在rfid_tag数据数组中的绝对位置(0-39) */
	if(input==0xFF)//数据无效
	{
		Hal_PWMB_RFID_DatResetState();
		return;
	}
	switch (RFID_Task.current_state)
	{//检测9个起始位
		case STATE_HEADER_DETECT:
		{
			if(input==1)
			{
				RFID_Task.header_counter++;
				if(RFID_Task.header_counter>=9)
				{
					RFID_Task.current_state=STATE_DATA_DETECT;//数据解析状态
					RFID_Task.data_position=0;
					memset(RFID_Task.rfid_tag,0,sizeof(RFID_Task.rfid_tag));
				}
			}
			else
			{
				RFID_Task.header_counter=0;
			}
			break;
		}
		case STATE_DATA_DETECT:
		{
			row=RFID_Task.data_position/5;
			col=RFID_Task.data_position%5;
			if(row<10)
			{
				if(col<4)
				{
					bit_pos=row*4+col;
					if(input==1)
					{
						RFID_Task.rfid_tag[bit_pos/8]|=(1<<(7-(bit_pos%8)));
					}
					RFID_Task.row_parity_sum+=input;//行校验累加器累加
					RFID_Task.col_parity_sum[col]+=input;//列校验累加器累加
				}
				if(col==4)//行校验位
				{	
					if((RFID_Task.row_parity_sum%2)!=input)
					{
						Hal_PWMB_RFID_DatResetState();
						return;
					}
					RFID_Task.row_parity_sum=0;//行校验累加器清零
				}
			}
			if(row==10)//列校验
			{
				if(col<4)
				{
					if((RFID_Task.col_parity_sum[col]%2)!=input)
					{
						Hal_PWMB_RFID_DatResetState();
						return;
					}
				}
				if(col==4)
				{
					memset(RFID_Task.col_parity_sum,0,sizeof(RFID_Task.col_parity_sum));//列校验累加器清零
					if(input!=0)
					{
						Hal_PWMB_RFID_DatResetState();
						return;
					}
				}
			}
			RFID_Task.data_position++;
			if(RFID_Task.data_position>=55)
			{
				RFID_Task.current_state=STATE_FINISH;//数据接收完成
				return;
			}
			break;
		}
		default:
			break;
	}
}
//0xFF表示数据无效,0xFE表示空跳
//#define TIME_512_MIN 435
//#define TIME_512_MAX 588
//#define TIME_256_MIN 186
//#define TIME_256_MAX 326
//分析信号
static unsigned int Hal_PWMB_Signal_Analysis(unsigned int cnt, unsigned char is_falling)
{
	if(RFID_Task.state==STATE_BASE)//等待有效起始信号
	{
		if(cnt>=TIME_512_MIN&&cnt<=TIME_512_MAX)//因为EM4100的停止位是0，所以开始时有一个512的信号
		{
			RFID_Task.state=STATE_DATA;
			RFID_Task.count_256=0;
		}
		else
		{
			return 0xFE;//表示无效数据
		}
	}

		if(cnt<TIME_256_MIN|| cnt>TIME_512_MAX)
		{
			RFID_Task.state=STATE_BASE;
			return 0xFF;//表示无效数据
		}
		if(cnt>TIME_512_MIN&&cnt<TIME_512_MAX)//完整有效数据
		{
			return is_falling?1:0;
		}
		if(cnt>=TIME_256_MIN&&cnt<=TIME_256_MAX)//空跳
		{
			RFID_Task.count_256++;
			if(RFID_Task.count_256%2==1)
				return 0xFE;//表示空跳
			if(RFID_Task.count_256%2==0)
			{
				RFID_Task.count_256=0;
				return is_falling?1:0;
			}
		}
		else
		{
			RFID_Task.state=STATE_BASE;
			RFID_Task.count_256=0;
		}
		return 0xFF;//表示无效数据
}
void Hal_PWMB_RFID_Pro()
{
	if((RFID_Task.rfData_high_Low==1)||(RFID_Task.rfData_high_Low==0)||(RFID_Task.rfData_high_Low==0xFF))
	{
		Hal_PWMB_RFID_Process(RFID_Task.rfData_high_Low);
		RFID_Task.rfData_high_Low=0xFE;//表示空跳
	}
}
unsigned char Hal_PWMB_GetRfidData(unsigned char *dat)
{
	unsigned char xdata i;
	if(RFID_Task.current_state==STATE_FINISH)
	{
		for(i=0;i<5;i++)
		{
			*dat=RFID_Task.rfid_tag[i];
			dat++;
		}	
		Hal_PWMB_RFID_DatResetState();
		return 1;//表示成功
	}
	return 0;//表示无效数据
}
void HAL_PWMB_Interrupt() interrupt 27
{
	unsigned int xdata cnt;
	if(PWMB_SR1&0x08)//判断通道7是否中断
	{
		PWMB_SR1&=~0x08;
		RFID_Task.current_capture=PWMB_CCR7;
		if(RFID_Task.current_capture>=RFID_Task.last_capture)
			cnt=RFID_Task.current_capture-RFID_Task.last_capture;
		else
			cnt=RFID_Task.current_capture+65536-RFID_Task.last_capture;
		RFID_Task.last_capture=RFID_Task.current_capture;
		RFID_Task.rfData_high_Low=Hal_PWMB_Signal_Analysis(cnt,1);//判断是否是上升沿
	}
	if(PWMB_SR1&0x10)//判断通道8是否中断
	{
		PWMB_SR1&=~0x10;
		RFID_Task.current_capture=PWMB_CCR8;
		if(RFID_Task.current_capture>=RFID_Task.last_capture)
			cnt=RFID_Task.current_capture - RFID_Task.last_capture;
		else
			cnt=RFID_Task.current_capture+65536-RFID_Task.last_capture;
		RFID_Task.last_capture=RFID_Task.current_capture;
		RFID_Task.rfData_high_Low=Hal_PWMB_Signal_Analysis(cnt,0);//判断是否是下降沿
	}

}