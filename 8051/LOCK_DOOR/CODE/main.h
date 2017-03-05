	
#ifndef __MAIN_H
#define	__MAIN_H
/***************Dia chi giao tiep thiet bi ngoai*********************/
#define BUZZER P3_4  
#define Tb_A P3_6 
/***************Dia chi giao tiep Nhiet do*********************/
#define DQ P3_5    
/***************Dia chi giao tiep DS1307*********************/
#define DS1307_SEC      0x00 //Giay
#define DS1307_MIN       0x01 //Phut
#define DS1307_HOUR     0x02 //Gio
#define DS1307_DAY        0x03 //Thu
#define DS1307_DATE      0x04 //Ngay
#define DS1307_MONTH    0x05 //Thang
#define DS1307_YEAR       0x06 //Nam
/************************INCLUDE FILE********************************/
#include <regx52.h>
/*++++++++++++STANDAR C++++++++++++++*/
#include "stdio.h"
#include "string.h"
#include "math.h"
/*+++++++++++++USER+++++++++++++++++*/
#include "delay/delay.h"
#include "lcd/lcd_16x2.h"
#include "i2c/i2c.h"
#include "24cxx/24cxx.h"
#include "Keypad/Keypad.h"
#include "DS1307/DS1307.h"
//+++++++++++++++++++++++++LCD+++++++++++++++++++++++++++
#define RS P2_5
#define RW P2_6//RW=0 => ghi
#define EN P2_7//RW=1 => doc
//RS=0 => code
//RS=1 => data
#define LCD_PORT P0
//+++++++++++++++++++++++++24xcc+++++++++++++++++++++++++++
#define SCL P3_1   
#define SDA P3_0    
#define SCL_t    P3_2
#define SDA_t    P3_3

#endif
