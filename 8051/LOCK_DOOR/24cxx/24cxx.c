#include "24cxx.h"
void EEP_24CXX_Init() {
 I2C_Init();
}
unsigned char EEP_24CXX_Read(unsigned char address) {
 bit AckTemp = 1;
 unsigned char Data;
 I2C_Start();
 I2C_Write(0xa0);
 AckTemp = I2C_CheckAck();
 I2C_Write(address);
 AckTemp = I2C_CheckAck();
 I2C_Start();
 I2C_Write(0xa1);
 AckTemp = I2C_CheckAck();
 Data = I2C_Read();
 AckTemp = I2C_CheckAck();
 I2C_Stop();
 return Data;
}
void EEP_24CXX_Write(unsigned char address, unsigned char Data) {
 bit AckTemp = 1;
 I2C_Start();
 I2C_Write(0xa0);
 AckTemp = I2C_CheckAck();
 I2C_Write(address);
 AckTemp = I2C_CheckAck();
 I2C_Write(Data);
 AckTemp = I2C_CheckAck();
 I2C_Stop();
}
void EEP_24CXX_WriteS(unsigned char address, unsigned char * s) {
 while ( * s) {
  EEP_24CXX_Write(address++, * s);
  s++;
 }
}
void EEPROM_24CXX_ReadS(unsigned char address, unsigned char lenght, unsigned char * s) {
 unsigned char i = 0;
 while (lenght) {
  if (EEP_24CXX_Read(address) != 255) {
   s[i++] = EEP_24CXX_Read(address);
  }
  address++;
  lenght--;

 }
 s[++i] = 0;
}