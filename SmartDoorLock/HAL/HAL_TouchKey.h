#ifndef __HAL_TOUCHKEY_H
#define __HAL_TOUCHKEY_H
//触摸按键测试使能
#define HAL_TOUCHKEY_TEST_ENABLE
#define HAL_Touchkey_SUM 16//通道数目
#define HAL_Touchkey_cnt 20//零点扫描次数
#define HAL_Touchkey_Timercnt 1//按键延时消抖
void HAL_TouchKey_Init();
unsigned char HAL_TouchKey_GetState(unsigned char TKnum);//获取按键状态
void HAL_TouchKey_Set_Zero();//零点值设置
void HAL_TouchKey_pro();
#ifdef HAL_TOUCHKEY_TEST_ENABLE


#endif

#endif