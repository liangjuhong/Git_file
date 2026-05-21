/*
本程序用于接收Wiegand协议发送的数据包
*/
#include <STC8H.H>
#include "HAL_WIEGAND_RECEIVE.H"
#include "STRING.h"
#include "MT_TFTLCD.H"
#define HAL_WG0_Receive P16//接收数据0
#define HAL_WG1_Receive P17//接收数据1
xdata HAL_Wiegand_Receive_Struct HAL_Wiegand_R_Struct;
xdata unsigned char WG_show_flag=0;//0：表示未接收到韦根数据，1：表示已接收到韦根数据
void HAL_Wiegand_Receive_Config()//配置IO口中断
{
    P1M0 &=0x3F;//P16,P17配置为准双向口
    P1M1 &=0x3F;
    P1IM0 &=0x3F;//P16,P17配置为下降沿中断
    P1IM1 &=0x3F;
    P1INTE |=0xC0;//开启P16,P17中断请求
    EA=1;//开启总中断
}
void HAL_Wiegand_Receive_Init()//接收数据初始化
{
    HAL_Wiegand_Receive_Config();//配置IO口中断
    HAL_Wiegand_R_Struct.DataNum=0;//初始化数据个数为0
    HAL_Wiegand_R_Struct.DATABuff=0;//初始化数据缓冲区为0
    memset(HAL_Wiegand_R_Struct.Data,0,3);//初始化数据缓冲区为0
    HAL_Wiegand_R_Struct.time_count=0;//初始化时间计数为0
}
void HAL_Wiegand_Receive_ShowHEX(unsigned char x,unsigned char y,unsigned char dat)//显示接收数据十六进制值
{
    unsigned char xdata buff[3],buff_hex,buff_char;
    buff_char=(dat>>4)&0x0F;
    if(buff_char>9)
    {
        buff[0]=buff_char+'A'-10;
    }
    else
    {
        buff[0]=buff_char+'0';
    }
    buff_hex=dat&0x0F;
    if(buff_hex>9)
    {
        buff[1]=buff_hex+'A'-10;
    }
    else
    {
        buff[1]=buff_hex+'0';
    }
    buff[2]=0;
    MT_TFTLCD_Showstring(x,y,buff,WHITE,BLACK,12,0);
}
void HAL_Wiegand_Receive_Process()//处理接收数据
{
    if(HAL_Wiegand_R_Struct.time_count)
    {
        HAL_Wiegand_R_Struct.time_count++;//时间计数增加
        if(HAL_Wiegand_R_Struct.time_count>=HAL_Wiegand_Receive_number)//判断是否接收完成数据
        {
            if(HAL_Wiegand_R_Struct.DataNum==26)
            {
                HAL_Wiegand_R_Struct.Data[0]=(unsigned char)((HAL_Wiegand_R_Struct.DATABuff>>17)&0xFF);
                HAL_Wiegand_R_Struct.Data[1]=(unsigned char)((HAL_Wiegand_R_Struct.DATABuff>>9)&0xFF);
                HAL_Wiegand_R_Struct.Data[2]=(unsigned char)((HAL_Wiegand_R_Struct.DATABuff>>1)&0xFF);
							
                WG_show_flag=1;
                HAL_Wiegand_R_Struct.DataNum=0;//数据个数重置为0
                HAL_Wiegand_R_Struct.DATABuff=0;//数据缓冲区重置为0
                HAL_Wiegand_R_Struct.time_count=0;//时间计数重置为0
            }
        }
    }
}
void HAL_WG_Receive_Show()
{
	if(WG_show_flag)
	{
		WG_show_flag=0;
		MT_TFTLCD_Showstring(90,0," WG: ",WHITE,BLACK,12,0);
		HAL_Wiegand_Receive_ShowHEX(115,0,HAL_Wiegand_R_Struct.Data[0]);//显示接收数据十六进制值
		HAL_Wiegand_Receive_ShowHEX(130,0,HAL_Wiegand_R_Struct.Data[1]);//显示接收数据十六进制值
		HAL_Wiegand_Receive_ShowHEX(145,0,HAL_Wiegand_R_Struct.Data[2]);//显示接收数据十六进制值
	}
	
}
void HAL_Wiegand_Receive_Interrupt() interrupt 38//接收数据中断服务函数
{
    unsigned char xdata inft;
    inft=P1INTF;
    if(inft)
    {
        HAL_Wiegand_R_Struct.time_count=1;//重置时间计数为1
        P1INTF=0x00;//清除P16,P17中断标志位
        if(inft&0x40)//P16中断,接收数据0
        {
            HAL_Wiegand_R_Struct.DATABuff<<=1;//数据缓冲区左移1位
            if(HAL_Wiegand_R_Struct.DataNum<32)
            {
                HAL_Wiegand_R_Struct.DataNum++;//数据个数增加
            }
        }
        if(inft&0x80)//P17中断,接收数据1
        {
            HAL_Wiegand_R_Struct.DATABuff<<=1;//数据缓冲区左移1位
            HAL_Wiegand_R_Struct.DATABuff|=0x01;//数据缓冲区最低位或上1
            if(HAL_Wiegand_R_Struct.DataNum<32)
            {
                HAL_Wiegand_R_Struct.DataNum++;//数据个数增加
            }
        }
    }
}