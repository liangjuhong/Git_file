#include "MT_TK_LED.H"
#include "HAL_BACKLIGHT.H"
#include "HAL_TOUCHKEY.H"
#include "HAL_TFTLCD_SPI.H"
#include "MT_TFTLCD.H"
#include "HAL_PWMA_HORN.H"
typedef struct
{
	unsigned char shakeTim[TOUCH_KEY_SUM];//按键按下的延时计数器
	unsigned char keyDat;  //保存按键值，0xFF表示没有按键按下；
	unsigned int KeyBackLightTime;//按键背光灯持续时间
	unsigned int TftlcdBackTime;//TFT背光灯持续时间
}MT_BackLight_Typedef;

MT_BackLight_Typedef xdata MT_TK_LED;
void MT_TKLED_Init()
{
	unsigned char TKI;
	for(TKI=0;TKI<TOUCH_KEY_SUM;TKI++)
	{
		MT_TK_LED.shakeTim[TKI]=0;
	}
	MT_TK_LED.keyDat=0xFF;
	MT_TK_LED.KeyBackLightTime=MT_KeyBackLigntTime;
	MT_TK_LED.TftlcdBackTime=2000;
}
/*
*函数将通道值转化为对应的数值
*/
unsigned char MT_GetkeyState(unsigned char keynum)
{
	unsigned int returnDat;
	returnDat=0xFF;//0xFF默认按键没有按下
	switch(keynum)
	{
		case channel_0:returnDat=BackLight_BACK;
			break;
		case channel_1:returnDat=BackLight_NUM9;
			break;
		case channel_2:returnDat=BackLight_DOWN;
			break;
		case channel_3:returnDat=BackLight_ENTER;
			break;
		case channel_4:returnDat=BackLight_NUM6;
			break;
		case channel_5:returnDat=BackLight_NUM3;
			break;
		case channel_6:returnDat=BackLight_NUM4;	
			break;
		case channel_7:returnDat=BackLight_NUM7;
			break;
		case channel_8:returnDat=BackLight_UP;
			break;
		case channel_9:returnDat=BackLight_Ring;
			break;
		case channel_10:returnDat=BackLight_NUM5;
			break;
		case channel_11:returnDat=BackLight_NUM2;
			break;
		case channel_12:returnDat=BackLight_Delete;
			break;
		case channel_13:returnDat=BackLight_NUM1;
			break;
		case channel_14:returnDat=BackLight_NUM0;
			break;
		case channel_15:returnDat=BackLight_NUM8;
			break;
	}
	return returnDat;
}
void MT_TKLED_pro()
{
	unsigned char xdata TKBuff;
	unsigned char xdata TKj;
	for(TKj=0;TKj<TOUCH_KEY_SUM;TKj++)
	{
		if(HAL_TouchKey_GetState(TKj))//判断通道X是否按下
		{
			if(MT_TK_LED.shakeTim[TKj]<MT_DelayTime)
			{
				MT_TK_LED.shakeTim[TKj]++;
				if(MT_TK_LED.shakeTim[TKj]==MT_DelayTime)
				{
					TKBuff=MT_GetkeyState(TKj);//将通道值转换为按键板上对应的值
					HAL_BackLight_Control(TKBuff);//控制触摸按键背光灯
					HAL_PWMA_main(HAL_horn_sound_Di);//控制喇叭
					if(MT_TK_LED.TftlcdBackTime==0)
					{
						MT_TK_LED.keyDat=0xff;
						HAL_TFTLCD_BL_Control(HAL_TFTLCD_ENABLE);//打开背光灯
					}
					else
					{
						MT_TK_LED.keyDat=TKBuff;
					}
				}
			}
			MT_TK_LED.KeyBackLightTime=MT_KeyBackLigntTime;
			MT_TK_LED.TftlcdBackTime=MT_TFTLcdBackTime;
		}
		else
			MT_TK_LED.shakeTim[TKj]=0;
	}
	
	if(MT_TK_LED.TftlcdBackTime)
	{
		MT_TK_LED.TftlcdBackTime--;
	}
	else
		HAL_TFTLCD_BL_Control(HAL_TFTLCD_DISABLE);
	
	if(MT_TK_LED.KeyBackLightTime)
		MT_TK_LED.KeyBackLightTime--;
	else
		HAL_BackLight_All_OFF();
}
//获取按键状态
unsigned char  MT_GetKeyState()
{
	unsigned char returnKeyDat;
	returnKeyDat = MT_TK_LED.keyDat;
	MT_TK_LED.keyDat = 0xFF;//将按键状态重置为0xFF，表示没有按键按下
	return returnKeyDat;
}
//获取背光灯时间
unsigned int Mt_Key_GetBackLightTim()
{ 
   return MT_TK_LED.TftlcdBackTime;
}
//设置背光灯时间
void MT_Key_SetBacklighttime(unsigned int time_number)
{
	MT_TK_LED.TftlcdBackTime=time_number;
	HAL_TFTLCD_BL_Control(HAL_TFTLCD_ENABLE);
}