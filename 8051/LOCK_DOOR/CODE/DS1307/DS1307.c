#include "DS1307/DS1307.h"

/****************Cac ham giao tiep I2C***************************/
void I2C_time_start() {
 SCL_t = 1;
 SDA_t = 1;
 SDA_t = 0;
 SCL_t = 0;
}
void I2C_time_stop() {
 SCL_t = 1;
 SDA_t = 0;
 SDA_t = 1;
 SCL_t = 1;
}
unsigned char I2C_write_t(unsigned char dat) {
 unsigned char i;
 for (i = 0; i < 8; i++) {
  SDA_t = (dat & 0x80); ///? 1:0;
  SCL_t = 1;
  SCL_t = 0;
  dat <<= 1;
 }
 SCL_t = 1;
 SCL_t = 0;
 return dat;
}
unsigned char I2C_read_t(void) {
 bit rd_bit;
 unsigned char i, dat;
 dat = 0x00;
 for (i = 0; i < 8; i++) { /* For loop read data 1 byte */
  SCL_t = 1;
  rd_bit = SDA_t; /* Keep for check acknowledge */
  dat = dat << 1;
  dat = dat | rd_bit; /* Keep bit data in dat */
  SCL_t = 0; /* Clear SCL_t */
 }
 return dat;
}


void DS1307_Write(unsigned char addr, unsigned char dat) {
 unsigned int temp;
 temp = dat; /*HEX to BCD*/
 dat = (((dat / 10) * 16) | (temp % 10)); /*for Led 7seg*/
 I2C_time_start(); /* time_start i2c bus */
 I2C_write_t(0XD0); /* Connect to DS1307 */
 I2C_write_t(addr); /* Requetime_5 RAM address on DS1307 */
 I2C_write_t(dat);
 I2C_time_stop();
}
unsigned char DS1307_Read(unsigned char addr) {
 unsigned int tm, ret;
 I2C_time_start(); /* time_start i2c bus */
 I2C_write_t(0xD0); /* Connect to DS1307 */
 I2C_write_t(addr); /* Requetime_5 RAM address on DS1307 */
 I2C_time_start(); /* time_start i2c bus */
 I2C_write_t(0XD1); /* Connect to DS1307 for Read */
 ret = I2C_read_t(); /* Receive data */
 I2C_time_stop();
 tm = ret; /*BCD to HEX*/
 ret = (((ret / 16) * 10) + (tm & 0x0f)); /*for Led 7seg*/
 return ret;
}