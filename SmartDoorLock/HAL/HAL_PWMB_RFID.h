#ifndef __HAL_PWMB_RFID_H
#define __HAL_PWMB_RFID_H
#define TIME_512_MIN 435
#define TIME_512_MAX 588

#define TIME_256_MIN 186
#define TIME_256_MAX 326
/* 状态枚举 */
typedef enum
{
    STATE_HEADER_DETECT,     /* 帧头检测状态 */
    STATE_DATA_DETECT,       /* 数据解析状态 */
    STATE_FINISH,            /* 数据解析完成 */
} En_RfidDatState;
typedef struct
{
	En_RfidDatState current_state;    // 当前状态机状态  
    unsigned char header_counter;     //帧头连续1计数器  
    unsigned char data_position;      //数据位位置计数器  取值范围0-55
    unsigned char row_parity_sum;     //当前行校验累加器  
    unsigned char rfid_tag[5];        //存储最终40位标签数据  
    unsigned char col_parity_sum[4];  //列校验累加器（4列）
	//用于解析数据是否合格
	unsigned char state;       //RFID解析状态
  unsigned char count_256;    //连续256μs脉冲计数器（用于处理空跳）  
  unsigned int rfData_high_Low;    //检测的有效数据   有效取值0-1； 0xFF表示无效数据
	//用于记录高低电平持续的时间
	unsigned int last_capture;      // 上次捕获的计数器值
  unsigned int current_capture;//当前捕获的计数器值
}HAL_RFID_TaskTypedef;
void HAL_PWMB_RFID_Init();
void HAL_Timer0_Init();
void HAL_PWMB_Config();
unsigned int Hal_PWMB_Signal_Analysis(unsigned int cnt, unsigned char is_falling);
void Hal_PWMB_RFID_Process(unsigned char input);
void Hal_PWMB_RFID_Pro();//实时调度
unsigned char Hal_PWMB_GetRfidData(unsigned char *dat);//获取RFID标签数据	
#endif