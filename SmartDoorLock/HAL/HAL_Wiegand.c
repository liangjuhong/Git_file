/*
本程序用于处理发送Wiegand数据包(数据由定时器中断自行发送)
通过定时器3产生时间间隔
检测接收到的RFID数据，并通过Wiegand协议发送数据
*/
#include <STC8H.H>
#include "HAL_WIEGAND.H"
#include "HAL_PWMB_RFID.H"
#include "STRING.h"
#include "MT_TFTLCD.H"
#define HAL_WGD0 P61//Wiegand数据线0,输出0
#define HAL_WGD1 P62//Wiegand数据线1,输出1

#define HAL_WGD0_High HAL_WGD0=0//Wiegand数据线0高电平
#define HAL_WGD0_Low HAL_WGD0=1//Wiegand数据线0低电平
#define HAL_WGD1_High HAL_WGD1=0//Wiegand数据线1高电平
#define HAL_WGD1_Low HAL_WGD1=1//Wiegand数据线1低电平

//200us=1/0.0002
#define HAL_Timer3_200us (65536-MAIN_Fosc/12/5000)
//2ms=1/0.002
#define HAL_Timer3_2ms (65536-MAIN_Fosc/12/500)
xdata HAL_WiegandTypedef HAL_WG_Struct;
void HAL_Timer3_Config(void)//定时器3初始化
{
    T4T3M &=0xF0;//定时器3配置12T模式
    TM3PS=0x00;//定时器3预分频为1
    T4T3M &=0xF7;//关闭定时器3
    IE2 &=0xDF;//关闭定时器3中断请求 1101 1111
}
void HAL_Wiegand_Init(void)
{
    HAL_Timer3_Config();//定时器3初始化
		HAL_WG_Struct.Swit=0;//任务开关
    HAL_WG_Struct.RxInterVal=0;//发送间隔计时器
    HAL_WG_Struct.DataSendState=HAL_WG_data_pulse;//数据脉冲状态
    HAL_WG_Struct.SentDatNum=0;//已发送位数计数器
    HAL_WG_Struct.EvenParity_Bit0=0;//偶校验位,第0位校验
    memset(HAL_WG_Struct.Data,0,3);//初始化数据缓冲区
    HAL_WG_Struct.OddParity_Bit25=0;//奇校验位,第25位校验
    memset(HAL_WG_Struct.RFIDBuffer,0,5);//清RFID缓冲区清0
    HAL_WGD0_High;//数据线0空闲状态
    HAL_WGD1_High;//数据线1空闲状态
}
//奇偶校验函数
//static unsigned char HAL_ParityCheck(unsigned char dat)//校验位校验
//{
//    xdata unsigned char i,parity=0;
//    for(i=0;i<8;i++)
//    {
//        if(dat&0x80)//判断最高位是否为1
//        {
//            parity++;
//        }
//        dat<<=1;
//    }
//    return parity;
//}
//数据包准备发送,WG26
void HAL_WG_DataReady(unsigned char *dat)
{
    xdata unsigned char Parity_count;
		HAL_WG_Struct.Data[0]=dat[0];
    HAL_WG_Struct.Data[1]=dat[1];
    HAL_WG_Struct.Data[2]=dat[2];
	
    Parity_count=(HAL_WG_Struct.Data[1])&0xf0;//第2位到第13位校验  
		Parity_count^=HAL_WG_Struct.Data[0];
		Parity_count^=Parity_count>>4;
		Parity_count^=Parity_count>>2;
		Parity_count^=Parity_count>>1;
    HAL_WG_Struct.EvenParity_Bit0=!(Parity_count&1);
	
    Parity_count=(HAL_WG_Struct.Data[1])&0x0f;
		Parity_count^=HAL_WG_Struct.Data[2];
		Parity_count^=Parity_count>>4;
		Parity_count^=Parity_count>>2;
		Parity_count^=Parity_count>>1;
    HAL_WG_Struct.OddParity_Bit25=Parity_count&1;
	
    HAL_WG_Struct.Swit=1;//任务开关,请求发送数据包
}
//此函数用于发送单个数据（0 和 1）
void HAL_WG_SendData(HAL_WiegandStateTypedef State,unsigned char dat)//发送数据
{
    if(State==HAL_WG_data_pulse)//数据脉冲状态
    {
        T3H=HAL_Timer3_200us>>8;
        T3L=HAL_Timer3_200us;
        if(dat)
        {
            HAL_WGD1_Low;//数据线1低电平
        }
        else
        {
            HAL_WGD0_Low;//数据线0低电平
        }
    }
    if(State==HAL_WG_High_level)//高电平状态
    {
        T3H=HAL_Timer3_2ms>>8;
        T3L=HAL_Timer3_2ms;
        HAL_WGD0_High;//数据线0高电平
        HAL_WGD1_High;//数据线1高电平
    }
}
void HAL_WG_process(void)//处理数据包
{
    if(HAL_WG_Struct.RxInterVal)
    {
        HAL_WG_Struct.RxInterVal++;
        if(HAL_WG_Struct.RxInterVal>=HAL_Timer_number)//延时300ms
        {
            HAL_WG_Struct.RxInterVal=0;
        }
        return;
    }
    if(HAL_WG_Struct.Swit)//判断是否需要发送数据脉冲
    {
      if(HAL_WG_Struct.EvenParity_Bit0)
      {
         HAL_WG_SendData(HAL_WG_data_pulse,1);//发送数据脉冲,第0位为1
      }
			else
      {
         HAL_WG_SendData(HAL_WG_data_pulse,0);//发送数据脉冲,第0位为0
      }
      HAL_WG_Struct.DataSendState=HAL_WG_High_level;//状态切换标志,下次中断要发送高电平间隔
      HAL_WG_Struct.RxInterVal=1;//发送间隔计时器,重置为1
      HAL_WG_Struct.SentDatNum=1;
      T4T3M |=0x08;//开启定时器3
      IE2 |=0x20;//开启定时器3中断请求
    }
}

void HAL_Timer3_Interrupt() interrupt 19
{
    if(HAL_WG_Struct.DataSendState==HAL_WG_High_level)//判断是否需要发送数据包
    {
        HAL_WG_Struct.DataSendState=HAL_WG_data_pulse;//状态切换标志,下次中断要发送高电平间隔
        HAL_WG_SendData(HAL_WG_High_level,0);//发送高电平间隔,第0位为0
        if(HAL_WG_Struct.SentDatNum>=26)
        {
            HAL_WG_Struct.Swit=0;//任务开关,发送完成,等待下一次发送
            T4T3M &=0xF7;//关闭定时器3
            IE2 &=0xDF;//关闭定时器3中断请求 1101 1111
        }
    }
    if(HAL_WG_Struct.DataSendState==HAL_WG_data_pulse)//判断是否需要发送高电平间隔
    {
        unsigned char cnt_char,cnt_bit,dat;
        HAL_WG_Struct.DataSendState=HAL_WG_High_level;//状态切换标志,下次中断要发送数据脉冲
        cnt_char=(HAL_WG_Struct.SentDatNum-1)/8;
        cnt_bit=(HAL_WG_Struct.SentDatNum-1)%8;
        dat=HAL_WG_Struct.Data[cnt_char];
        if(HAL_WG_Struct.SentDatNum<25)
        {
            if((dat<<cnt_bit)&0x80)//0000 0000
            {
                HAL_WG_SendData(HAL_WG_data_pulse,1);//发送数据脉冲
            }
            else
            {
                HAL_WG_SendData(HAL_WG_data_pulse,0);//发送数据脉冲
            }
        }
        if(HAL_WG_Struct.SentDatNum==25)
        {
            HAL_WG_SendData(HAL_WG_data_pulse, HAL_WG_Struct.OddParity_Bit25);//发送数据脉冲,第26位为奇校验位
        }
        HAL_WG_Struct.SentDatNum++;
    }
}
