#ifndef __HAL_RS485_H
#define __HAL_RS485_H
#define HAL_RS485_RXBuffer_SIZE 8
#define HAL_RS485_TXBuffer_SIZE 13

#define SLAVE_ADDR_MODBUS            0X01  //从设备地址
#define MODBUS_COM_READ              0X04  //读取寄存器命令(功能码)
#define MODBUS_REGISTER_START_ADDR   0x00  //寄存器起始地址
#define MODBUS_REGISTER_COUNT        0x04  //寄存器数量
typedef struct
{
	unsigned int Address;//地址字节
	unsigned int TX_Busy;//发送标志位
	unsigned char RXFlag;//数据接收完成标志位
	unsigned char datadelay;//延时
	unsigned char datalen;//接收数据计数器
	unsigned char TXDataSize;//将要发送的数据长度
	unsigned char TXDatIdx; //发送数据的数组下标
	unsigned char RXBuffer[HAL_RS485_RXBuffer_SIZE];//接收缓冲区
	unsigned char TXBuffer[HAL_RS485_TXBuffer_SIZE];//发送缓冲区
}HAL_RS485_TaskTypedef;
void HAL_RS485_Config();//配置RS485任务参数
void HAL_RS485_Init();//初始化RS485
void HAL_RS485_SendByte(unsigned char senddata);//发送一个字节数据
void HAL_RS485_Sendstring(unsigned char *str);//发送字符串
void HAL_RS485_SendData(unsigned int senddata);//发送16位数据，高8位在前，低8位在后
void HAL_RS485_Process();//处理接收数据
unsigned int verify_Crc16(unsigned char *nData, unsigned int wLength);//计算CRC16校验值
void HAL_RS485_TXBuffer_Send();//发送数据包
void hal_modbus_UpdateDat(unsigned char *dat,unsigned char len);//RFID更新函数
#endif