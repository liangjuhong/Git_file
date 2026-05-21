#ifndef _HAL_DS1302_H
#define _HAL_DS1302_H

void HAL_DS1302_W_Byte(unsigned char dat);
unsigned char HAL_DS1302_R_Byte();
void HAL_DS1302_W_DATA(unsigned char add,unsigned char dat);
unsigned char HAL_DS1302_R_DATA(unsigned char add);

#endif