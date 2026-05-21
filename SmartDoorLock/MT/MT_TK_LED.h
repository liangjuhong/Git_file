#ifndef __MT_TK_LED_H
#define __MT_TK_LED_H

#define MT_KeyBackLigntTime 2000	//触摸按键背光灯持续时间
#define MT_TFTLcdBackTime 50000 	//LED背光灯持续时间
#define MT_DelayTime 1						//延时消抖
#define TOUCH_KEY_SUM 16
//触摸按键通道值（通道0——通道15）
typedef enum
{
	channel_0,
	channel_1,
	channel_2,
	channel_3,
	channel_4,
	channel_5,
	channel_6,
	channel_7,
	channel_8,
	channel_9,
	channel_10,
	channel_11,
	channel_12,
	channel_13,
	channel_14,
	channel_15,
}MT_Channel_enum;
//15个通道对应的值
typedef enum
{
	BackLight_NUM1,
	BackLight_NUM2,
	BackLight_NUM3,
	BackLight_Ring,
	BackLight_NUM4,
	BackLight_NUM5,
	BackLight_NUM6,
	BackLight_UP,
	BackLight_NUM7,
	BackLight_NUM8,
	BackLight_NUM9,
	BackLight_DOWN,
	BackLight_Delete,
	BackLight_NUM0,
	BackLight_BACK,
	BackLight_ENTER,
}MT_BackLight_enum;

void MT_TKLED_Init();
void MT_TKLED_pro();
unsigned char  MT_GetKeyState();//获取按键状态
void Mt_SetTftlcdBackLightTim(unsigned int tim);//设置背光灯时间
unsigned int Mt_Key_GetBackLightTim();//获取背光灯点亮时间
void MT_Key_SetBacklighttime(unsigned int time_number);
#endif