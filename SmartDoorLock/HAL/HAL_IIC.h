#ifndef _HAL_IIC_
#define _HAL_IIC_
void HAL_IICInit(void);//初始化
void HAL_IICWait(void);//等待命令发送完成
void HAL_IICStart(void);//发送起始信号
void HAL_IICSendData(unsigned char dat);//IIC发送单个数据
unsigned char HAL_IICRecdata(void);//接受数据
void HAL_IICRecACK(void);//接受应答信号
void HAL_IICSendACK(void);//发送应答信号
void HAL_IICSendNACK(void);//发送非应答信号
void HAL_IICStop(void);//停止信号


#endif