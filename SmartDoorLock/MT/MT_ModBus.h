#ifndef __MT_MODBUS_H
#define __MT_MODBUS_H

typedef struct
{
	unsigned char userId_Hb;
	unsigned char userId_Lb;
	unsigned char Type_Hb;
	unsigned char Type_Lb;
	unsigned char Data[4];
}tru_ModBusDat;

#define TIM_HOLD_MODBUSDAT   5000  ///Modbus持续时间。

void mt_modbus_Init(void);
void mt_modbus_Pro(void);
void mt_modbus_UpdateDat(unsigned char userid,unsigned char type,unsigned char *dat);
#endif