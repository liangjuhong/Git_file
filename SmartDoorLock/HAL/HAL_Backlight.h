#ifndef __HAL_BACKLIGHT_H
#define __HAL_BACKLIGHT_H
//背光灯测试
//#define HAL_BACKLIGHT_TEST_ENABLE
typedef enum
{
	HAL_BackLight3,
	HAL_BackLight4,
	HAL_BackLight5,
	HAL_BackLight6,
	
	HAL_BackLight7,
	HAL_BackLight8,
	HAL_BackLight9,
	HAL_BackLight10,
	
	HAL_BackLight11,
	HAL_BackLight12,
	HAL_BackLight13,
	HAL_BackLight14,
	
	HAL_BackLight15,
	HAL_BackLight16,
	HAL_BackLight17,
	HAL_BackLight18,
	HAL_BackLightSUM
}HAL_BackLightTypedef;
//所有背光灯熄灭
void HAL_BackLight_All_OFF();
//所有背光灯点亮
void HAL_BackLight_All_ON();

void HAL_BackLight_Control(HAL_BackLightTypedef Light);
#ifdef HAL_BACKLIGHT_TEST_ENABLE
void HAL_BackLight_TEST();
#endif

#endif
