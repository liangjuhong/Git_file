#ifndef __HAL_WIEGAND_H
#define __HAL_WIEGAND_H
#define HAL_Timer_number 300
typedef enum
{
    HAL_WG_data_pulse,//数据脉冲状态
    HAL_WG_High_level,//高电平状态
}HAL_WiegandStateTypedef;
typedef struct
{
    unsigned char Swit;// 任务开关：0-不发送, 1-请求发送。由外部函数置1，主循环检测。
    unsigned int RxInterVal; // 发送间隔计时器：用于确保两次发送之间有足够的间隔（>250ms）
 // 状态切换标志：HAL_WG_data_pulse-表示下次中断要发送“数据脉冲”，HAL_WG_High_level-表示下次中断要发送“高电平间隔”。
    HAL_WiegandStateTypedef DataSendState;
    unsigned char SentDatNum; // 已发送位数计数器：记录当前发送到26位中的第几位了。
    unsigned char EvenParity_Bit0;//偶校验位,第0位校验
    unsigned char Data[3];//存放要发送的ID卡数据
    unsigned char OddParity_Bit25;//奇校验位,第25位校验
    unsigned char RFIDBuffer[5];//存放接收到的RFID数据
}HAL_WiegandTypedef;
void HAL_Timer3_Config(void);//定时器3配置
void HAL_Wiegand_Init(void);//Wiegand初始化
void HAL_WG_DataReady(unsigned char *dat);//数据包准备发送
void HAL_WG_SendData(HAL_WiegandStateTypedef State,unsigned char dat);//发送数据包
void HAL_WG_process(void);//处理数据包
#endif