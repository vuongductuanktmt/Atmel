/*
 * lcd.h
 *
 * Created: 2/8/2017 6:10:38 PM
 *  Author: AnhTuan
 */ 


#ifndef LCD_H_
#define LCD_H_
#define LCD_RS 0 // pin for LCD R/S (eg PB0)
#define LCD_E 1 // pin for LCD enable
#define DAT4 2 // pin for d4
#define DAT5 3 // pin for d5
#define DAT6 4 // pin for d6
#define DAT7 5 // pin for d7
void PulseEnableLine ();
void SendNibble(byte data);
void SendByte (byte data);
void LCD_Cmd (byte cmd);
void LCD_Char (byte ch);
void LCD_Init();
void LCD_Clear();
void LCD_Home();
void LCD_Goto(byte x, byte y);
void LCD_Line(byte row);
void LCD_String(const char *text);
void LCD_Hex(int data);
void LCD_Integer(int data);
#endif /* LCD_H_ */