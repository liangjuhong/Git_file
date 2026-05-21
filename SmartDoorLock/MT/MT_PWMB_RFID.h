#ifndef __MT_PWMB_RFID_H
#define __MT_PWMB_RFID_H

#define MT_RFID_TIMER_MAX 200
typedef struct
{
    unsigned char state;//0:未接收数据，1:已接收数据，未被上层应用调用 2:数据接收完成
    unsigned char rfid_receive_flag;//0:上层未接收数据，1:上层已接收数据
    unsigned char rfid_receive_timer;//接收数据定时器
    unsigned char RfidData[5];//RFID数据缓存
}MT_RFID_Task;
void MT_PWMB_RFID_Init();
void MT_PWMB_RFID_Process();
unsigned char MT_PWMB_GetRfidData(unsigned char *rfidNum);
#endif