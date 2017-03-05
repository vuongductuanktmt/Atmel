#ifndef __24CXX_H
#define __24CXX_H
#include "main.h"
void EEP_24CXX_Init();
unsigned char EEP_24CXX_Read(unsigned char address);
void EEP_24CXX_Write(unsigned char address,unsigned char Data);
void EEP_24CXX_WriteS(unsigned char address,unsigned char *s);
void EEPROM_24CXX_ReadS(unsigned char address, unsigned char lenght, unsigned char *s);
#endif