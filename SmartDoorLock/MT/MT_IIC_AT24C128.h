#ifndef _MT_IIC_AT24C128_
#define _MT_IIC_AT24C128_
#define EEPROM_PAGE_SIZE 64
void MT_IICWriteByte(unsigned int regadd,unsigned char *dataadd);
void MT_IICWritePage(unsigned int startadd,unsigned char xdata *writenumberadd,unsigned int datalen);
void MT_IICReadByte(unsigned int regadd,unsigned char *dataadd);
void MT_IICReaddata(unsigned int startadd,unsigned char xdata *readnumberadd,unsigned int datalen);
#endif