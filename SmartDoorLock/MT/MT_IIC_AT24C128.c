#include "HAL_IIC.h"
#include "MT_IIC_AT24C128.H"
void MT_IICWriteByte(unsigned int regadd,unsigned char *dataadd)
{
	HAL_IICStart();
	HAL_IICSendData(0xA0);//AT24C128芯片设备地址0xA0,写数据
	HAL_IICRecACK();//接收ACK信号
	HAL_IICSendData(regadd>>8);//写入寄存器地址高八位
	HAL_IICRecACK();
	HAL_IICSendData(regadd);//写入寄存器地址低八位
	HAL_IICRecACK();
	
	HAL_IICSendData(*dataadd);//写入数据
	HAL_IICRecACK();
	HAL_IICStop();
}
/*IIC页写，一次最多写入64
startadd EEPROM起始地址
writenumberadd  要写入数据的起始地址
datalen 写入数据的长度
*/
void MT_IICWritePage(unsigned int startadd,unsigned char xdata *writenumberadd,unsigned int datalen)
{
	unsigned char xdata i,j,k;
	unsigned int xdata startlen;//起始页数据长度（小于64）
	unsigned char xdata pagenumber;//整页写的页数
	unsigned char xdata ramaininglen;//中间页和结束页的数据长度
	unsigned char xdata lastlen;//结束页数据长度（小于64）
	unsigned int xdata currentadd;//
	unsigned char xdata *currentwriteadd;//保存要写入数据的起始地址
	
	currentadd=startadd;
	currentwriteadd=writenumberadd;
	//判断起始页地址是否是页起始地址
	if(currentadd%EEPROM_PAGE_SIZE)
	{
		startlen=EEPROM_PAGE_SIZE-currentadd%EEPROM_PAGE_SIZE;
		if(startlen>datalen)
		{
			startlen=datalen;
		}
	}
	else
	{
		startlen=0;
	}
	//起始页数据写入
	if(startlen)
	{
		HAL_IICStart();
		HAL_IICSendData(0xA0);//AT24C128芯片设备地址0xA0,写数据
		HAL_IICRecACK();//接收ACK信号
		HAL_IICSendData(currentadd>>8);//写入地址高八位
		HAL_IICRecACK();
		HAL_IICSendData(currentadd);//写入地址低八位
		HAL_IICRecACK();
		for(i=0;i<startlen;i++)
		{
			HAL_IICSendData(*currentwriteadd);//写入数据
			HAL_IICRecACK();
			currentwriteadd++;
		}
		HAL_IICStop();
	}
	
	ramaininglen=datalen-startlen;//剩下页数据长度
	pagenumber=ramaininglen/EEPROM_PAGE_SIZE;
	lastlen=ramaininglen%EEPROM_PAGE_SIZE;
	currentadd+=startlen;
	
	//中间页数据写入
	if(pagenumber)
	{
		for(i=0;i<pagenumber;i++)
		{
			HAL_IICStart();
			HAL_IICSendData(0xA0);//AT24C128芯片设备地址0xA0,写数据
			HAL_IICRecACK();//接收ACK信号
			HAL_IICSendData(currentadd>>8);//写入地址高八位
			HAL_IICRecACK();
			HAL_IICSendData(currentadd);//写入地址低八位
			HAL_IICRecACK();
			for(j=0;j<EEPROM_PAGE_SIZE;j++)
			{
				HAL_IICSendData(*currentwriteadd);//写入数据
				HAL_IICRecACK();
				currentwriteadd++;
			}
			HAL_IICStop();
			currentadd+=EEPROM_PAGE_SIZE;
		}
	}
	//结束页数据写入
	if(lastlen)
	{
		HAL_IICStart();
		HAL_IICSendData(0xA0);//AT24C128芯片设备地址0xA0,写数据
		HAL_IICRecACK();//接收ACK信号
		HAL_IICSendData(currentadd>>8);//写入地址高八位
		HAL_IICRecACK();
		HAL_IICSendData(currentadd);//写入地址低八位
		HAL_IICRecACK();
		for(k=0;k<lastlen;k++)
		{
			HAL_IICSendData(*currentwriteadd);//写入数据
			HAL_IICRecACK();
			currentwriteadd++;
		}
		HAL_IICStop();
		currentadd+=lastlen;
	}
}

void MT_IICReadByte(unsigned int regadd,unsigned char *dataadd)
{
	HAL_IICStart();
	HAL_IICSendData(0xA0);
	HAL_IICRecACK();
	HAL_IICSendData(regadd>>8);//写入地址高八位
	HAL_IICRecACK();
	HAL_IICSendData(regadd);//写入地址低八位
	HAL_IICRecACK();
	
	HAL_IICStart();
	HAL_IICSendData(0xA1);
	HAL_IICRecACK();
	*dataadd=HAL_IICRecdata();
	HAL_IICSendNACK();
	HAL_IICStop();
}
void MT_IICReaddata(unsigned int startadd,unsigned char xdata *readnumberadd,unsigned int datalen)
{
	unsigned int xdata i;
	HAL_IICStart();
	HAL_IICSendData(0xA0);
	HAL_IICRecACK();
	HAL_IICSendData(startadd>>8);//写入地址高八位
	HAL_IICRecACK();
	HAL_IICSendData(startadd);//写入地址低八位
	HAL_IICRecACK();
	
	HAL_IICStart();
	HAL_IICSendData(0xA1);
	HAL_IICRecACK();
	for(i=0;i<(datalen-1);i++)
	{
		*readnumberadd=HAL_IICRecdata();
		HAL_IICSendACK();
		readnumberadd++;
	}
	*readnumberadd=HAL_IICRecdata();//防止指针溢出
	HAL_IICSendNACK();
	HAL_IICStop();
}