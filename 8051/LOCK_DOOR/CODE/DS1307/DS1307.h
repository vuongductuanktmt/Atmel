#ifndef __DS1307_H
#define __DS1307_H
#include "main.h"

/****************Cac ham giao tiep I2C***************************/
void I2C_time_start();
void I2C_time_stop();
unsigned char I2C_write_t(unsigned char dat);
unsigned char I2C_read_t(void);
void DS1307_Write(unsigned char addr,unsigned char dat);
unsigned char DS1307_Read(unsigned char addr);
void DS1307_Set();
void DS1307_GetTime();
void DS1307_GetDate();
#endif