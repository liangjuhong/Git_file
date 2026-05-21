#ifndef __HAL_WIEGAND_RECEIVE_H
#define __HAL_WIEGAND_RECEIVE_H
#define HAL_Wiegand_Receive_number 10//定时10ms
typedef struct
{
    unsigned long DATABuff;//存放IO口中断获取的数据
    unsigned char Data[3];//存放接收的数据
    unsigned char DataNum;//获取的数据个数

    unsigned char time_count;//时间计数
}HAL_Wiegand_Receive_Struct;
void HAL_Wiegand_Receive_Init();//接收数据初始化
void HAL_Wiegand_Receive_Process();//处理接收数据
void HAL_Wiegand_Receive_ShowHEX(unsigned char x,unsigned char y,unsigned char dat);//显示接收数据十六进制值
void HAL_WG_Receive_Show();//显示接收到的韦根数据
#endif