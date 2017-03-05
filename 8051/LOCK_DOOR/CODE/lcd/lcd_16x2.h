/**
  ******************************************************************************
  * Ten Tep		:		  lcd_16x2.h
  * Tac Gia		:	  	Nguyen Quy Nhat
  * Cong Ty		:			MinhHaGroup
  *	Website 	:			BanLinhKien.Vn
  * Phien Ban	:			V1.0.0
  * Ngay			:    	31-07-2012
  * Tom Tat   :     Khai bao cac ham dieu khien LCD 16x2.
  *									Dinh nghia mot so hang su dung cho LCD.
  *           
  *
  ******************************************************************************
  * Chu Y		:
  *							 
  ******************************************************************************
  */
#ifndef __LCD_16X2_H
#define __LCD_16X2_H
#include "main.h"
// Dinh Nghia Cac Hang So
 void LCDWriteCmd(unsigned char c);	//Tao xung Enable LCD
 void LCDWriteData(unsigned char c);//Ham Gui 4 Bit Du Lieu Ra LCD
 void LCDcursorxy(unsigned char x,unsigned char y);// Ham Gui 1 Lenh Cho LCD
void LCD_init() ;// Ham Khoi Tao LCD
void LCD_Clear();// Ham Thiet Lap Vi Tri Con Tro
 void LCD_home();// Ham Xoa Man Hinh LCD
 void LCD_putstr(unsigned char *s);// Ham Gui 1 Ki Tu Len LCD

#endif




