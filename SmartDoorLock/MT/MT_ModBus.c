#include "HAL_RS485.H"
#include "MT_MODBUS.H"

tru_ModBusDat xdata modBusDat;
unsigned int xdata DataClearDlayTim;

void mt_modbus_Init(void)
{
	DataClearDlayTim = TIM_HOLD_MODBUSDAT;	
}	
void mt_modbus_Pro(void)
{
	unsigned char buf[] ={0xff,0xff,0xff,0xff};
	if(DataClearDlayTim)
	{
		DataClearDlayTim ++;
		if(DataClearDlayTim > TIM_HOLD_MODBUSDAT)//TIM_HOLD_MODBUSDATµÈÓÚ5000
		{
			mt_modbus_UpdateDat(0xff,0xff,buf);
			DataClearDlayTim = 0;
		}	
	}
}
void mt_modbus_UpdateDat(unsigned char userid,unsigned char type,unsigned char *dat)
{
	modBusDat.userId_Hb = 0;
	modBusDat.userId_Lb = userid;
	modBusDat.Type_Hb = 0;
	modBusDat.Type_Lb = type;
	modBusDat.Data[0] = dat[0];
	modBusDat.Data[1] = dat[1];
	modBusDat.Data[2] = dat[2];
	modBusDat.Data[3] = dat[3];	
	hal_modbus_UpdateDat((unsigned char *)(&modBusDat.userId_Hb),8);  //
	DataClearDlayTim = 1;
}