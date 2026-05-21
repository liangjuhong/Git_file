#ifndef _HAL_PWMA_HORN_H
#define _HAL_PWMA_HORN_H
#define HAL_HornSound_Time_Cycle 150
//typedef enum
//{
//	HAL_PWMA_DISABLE,
//	HAL_PWMA_ENABLE
//}HAL_PWMA_ENUM;
typedef enum
{
	HAL_horn_sound_free,//没有声音
	HAL_horn_sound_Di,  //
	HAL_horn_sound_DiDong=3,
	HAL_horn_sound_SUM
}HAL_HornSound_enum;
typedef struct
{
	HAL_HornSound_enum soundSta;//声音的状态
	unsigned int Time_cycle;//声音持续的时间
	unsigned int frequency;//频率
	unsigned int duty;//占空比
	unsigned int DiDong_time;
}HAL_HornSound_struct;

void HAL_PWMA_Config();
void HAL_PWMA_Init();
void HAL_PWMA_main(HAL_HornSound_enum Soundtype);
#endif