/*
*定时器2作为串口2的波特率发生器，通过RS485发送数据
*/
#include <STC8H.H>
#include <string.h>
#include "HAL_RS485.H"
#include "HAL_PWMB_RFID.H"
#include "HAL_RS485_CRC16.H"
#define HAL_RS485_Enable P32//控制RS485的接受和发送，RS485_Enable为1时是发送状态，为0时是接收状态
#define BRT (65536-MAIN_Fosc/115200UL/4)
//static xdata unsigned char HAL_RS485_Sendflag=0;
xdata HAL_RS485_TaskTypedef HAL_RS485_Task;
void HAL_RS485_Config()//配置RS485任务参数
{
	T2L=BRT;
	T2H=BRT>>8;
	AUXR &= 0xE3;
	AUXR |=0x14;//定时器2为定时器模式，1T模式,开启定时器2
	
	P_SW2 |=0x01;//串口2的引脚TXD设置为P4.7，RXD设置为P4.6
	S2CON |= 0x10;//串口2设置为模式0可变波特率8位数据方式，并允许串口接受数据
	IE2 |= 0x01;//允许串口2中断
	HAL_RS485_Enable=0;//默认处于接收状态
}
static void HAL_RXBuffer_Clear()//清空接收缓冲区
{
	memset(HAL_RS485_Task.RXBuffer,0,HAL_RS485_RXBuffer_SIZE);//接收缓冲区清0
}
static void HAL_TXBuffer_Clear()//清空发送缓冲区
{
	memset(HAL_RS485_Task.TXBuffer,0,HAL_RS485_TXBuffer_SIZE);//发送缓冲区清0
}
void HAL_RS485_Init()
{
	xdata unsigned int *address;//地址字节
	address=0xFDFF;//地址字节为0xFDFF
	HAL_RS485_Config();//配置RS485任务参数
	HAL_RS485_Task.Address=*address;//地址字节
	if((HAL_RS485_Task.Address < 1) || (HAL_RS485_Task.Address > 247))//修改后的程序代码
  {
    HAL_RS485_Task.Address = SLAVE_ADDR_MODBUS;                
  }
	HAL_RS485_Task.TX_Busy=0;//数据发送标志位
	HAL_RS485_Task.RXFlag=0;//数据接收完成标志位
	HAL_RS485_Task.datadelay=0;//数据延迟字节，0表示不延迟，1表示延迟1个字节，2表示延迟2个字节，以此类推
	HAL_RS485_Task.datalen=0;//数据长度字节
	HAL_RXBuffer_Clear();//清空接收缓冲区
	HAL_TXBuffer_Clear();//清空发送缓冲区
}
//void HAL_RS485_SendByte(unsigned char senddata)//发送一个byte数据
//{
//	HAL_RS485_Enable=1;//发送状态
//	S2BUF=senddata;
//	while(!HAL_RS485_Sendflag);//等待发送完成
//	HAL_RS485_Enable=0;//接收状态
//	HAL_RS485_Sendflag=0;//发送完成
//}
//void HAL_RS485_SendData(unsigned int senddata)//发送16位数据，高8位在前，低8位在后
//{
//	HAL_RS485_SendByte(senddata>>8);//发送高8位数据
//	HAL_RS485_SendByte(senddata);//发送低8位数据
//}
//void HAL_RS485_Sendstring(unsigned char *str)//发送字符串
//{
//	while(*str)
//	{
//		HAL_RS485_SendByte(*str++);
//	}
//}

unsigned int verify_Crc16(unsigned char *nData, unsigned int wLength)//计算CRC16校验值
{
   unsigned char nTemp;
   unsigned int wCRCWord = 0xFFFF;
   while (wLength--)
   {
     nTemp = *nData++ ^ wCRCWord;
     wCRCWord >>= 8;
     wCRCWord  ^= wCRCTable[nTemp];
   }
   return wCRCWord;
} // End: CRC16
void HAL_RS485_Process()//处理接收数据
{
	unsigned int xdata RXCRC16=0;
	unsigned char crc_low, crc_high;
	if(HAL_RS485_Task.TX_Busy)
	{
		return;
	}
  if(HAL_RS485_Task.TX_Busy == 0)
	{
		if(HAL_RS485_Task.TXDataSize)
		{
			HAL_RS485_Task.TX_Busy = 1;
			HAL_RS485_Enable = 1;
			S2BUF = HAL_RS485_Task.TXBuffer[0];
			HAL_RS485_Task.TXDatIdx = 0;
		}
	}	
	if(HAL_RS485_Task.datadelay)
	{
		HAL_RS485_Task.datadelay++;
		if(HAL_RS485_Task.datadelay>4)//等接收完成一个字节，延时4ms，再处理数据
		{
			HAL_RS485_Task.datadelay=0;//数据延迟字节清0
			if(HAL_RS485_Task.RXFlag==1)//判断是否接收完成一个字节
			{
				HAL_RS485_Task.RXFlag=0;//清除接收完成标志位
				if((HAL_RS485_Task.RXBuffer[0]==HAL_RS485_Task.Address)&&
				   (HAL_RS485_Task.RXBuffer[1] == MODBUS_COM_READ) &&
				   (HAL_RS485_Task.RXBuffer[2] == (MODBUS_REGISTER_START_ADDR >> 8)) &&
				   (HAL_RS485_Task.RXBuffer[3] == MODBUS_REGISTER_START_ADDR) &&
				   (HAL_RS485_Task.RXBuffer[4] == (MODBUS_REGISTER_COUNT >> 8)) &&
				   (HAL_RS485_Task.RXBuffer[5] == MODBUS_REGISTER_COUNT))
					{
							RXCRC16=verify_Crc16(HAL_RS485_Task.RXBuffer,6);//计算CRC16校验值
							crc_low = (unsigned char)RXCRC16;
							crc_high = (unsigned char)(RXCRC16 >> 8);
							if((HAL_RS485_Task.RXBuffer[6]==crc_low)&&(HAL_RS485_Task.RXBuffer[7]==crc_high))
							{
								HAL_RS485_TXBuffer_Send();//发送数据包
							}
					}
			}
		}
	}
}
void HAL_RS485_TXBuffer_Send()//发送MODBUS数据包
{
	HAL_RS485_Task.TXDataSize = 13;
}
void hal_modbus_UpdateDat(unsigned char *dat,unsigned char len)
{
  unsigned char idx = 0;
	unsigned char i;//
	unsigned int crc16;
//	lenx = len / 2;
	//IE2 &= ~0x01; //关闭串口2中断
	HAL_RS485_Task.TXBuffer[idx++] = SLAVE_ADDR_MODBUS;
	HAL_RS485_Task.TXBuffer[idx++] = MODBUS_COM_READ;
	HAL_RS485_Task.TXBuffer[idx++] = len;  //数据长度
	for(i=0;i<len;i++)
	{
		HAL_RS485_Task.TXBuffer[idx++] = *dat;
		dat++;
	}
	crc16 = verify_Crc16(HAL_RS485_Task.TXBuffer,idx);
	HAL_RS485_Task.TXBuffer[idx++] = crc16;
	HAL_RS485_Task.TXBuffer[idx++] = crc16 >> 8;
}

void UART2_Isr() interrupt 8
{
	if(S2CON & 0x02)//判断是否发送完成一个字节TXD
	{
		S2CON &=~0x02;
		HAL_RS485_Task.TXDatIdx++;
		if(HAL_RS485_Task.TXDatIdx >= HAL_RS485_Task.TXDataSize)
		{
			HAL_RS485_Enable = 0;
			HAL_RS485_Task.TXDataSize = 0;
			HAL_RS485_Task.TX_Busy = 0;
		}
		else
		{
			S2BUF = HAL_RS485_Task.TXBuffer[HAL_RS485_Task.TXDatIdx];
		}
	}
	if(S2CON & 0x01)//判断是否接收完成一个字节RXD
	{
		S2CON &= ~0x01;
		HAL_RS485_Task.RXBuffer[HAL_RS485_Task.datalen++]=S2BUF;//将接收到的数据字节存储到接收缓冲区
		if(HAL_RS485_Task.datalen==HAL_RS485_RXBuffer_SIZE)//判断是否接收完成一个数据包
		{
			HAL_RS485_Task.datalen=0;//数据长度字节清0
			HAL_RS485_Task.RXFlag=1;//设置接收完成标志位
		}
		HAL_RS485_Task.datadelay=1;
	}
}
