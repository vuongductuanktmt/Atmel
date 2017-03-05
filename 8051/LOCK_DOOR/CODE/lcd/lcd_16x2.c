#include "lcd_16x2.h"
//Tao Xung
 void LCDWriteCmd(unsigned char c) //CT con ghi du lieu len LCD
 {
  LCD_PORT = c; 
  RS = 0; // khi RS high thi day la 1 lenh chu khong phai du lieu
  RW = 0; // khi Read/write low thi khong cho phep doc ghi du lieu
  EN = 1; // Enable khi high thi cho phep truyen du lieu
  delay_ms(5); // delay 5ms de xu ly
  EN = 0; 	// va ngat duong truyen

 }
 //==============================
void LCDWriteData(unsigned char c) //CT con doc du lieu tu LCD
 {
  LCD_PORT = c;
  RS = 1; // khi RS high thi cho du lieu vao
   RW = 0; // khi Read/write low thi khong cho phep doc ghi du lieu
  EN = 1; // Enable khi high thi cho phep truyen du lieu
  delay_ms(1); // delay 1ms de xu ly
  EN = 0; 	// va ngat duong truyen

 }
 //=============================
void LCDcursorxy(unsigned char x, unsigned char y) {
  if ((x < 1 || x > 2) && (y < 1 || y > 16)) {
   x = 1;
   y = 1;
  }
  if (x == 1)
   LCDWriteCmd(0x7F + y);
  else
   LCDWriteCmd(0xBF + y);
 }
 //===============================
void LCD_init() // Khoi tao LCD
 {
  delay_ms(5);
  LCDWriteCmd(0x3c); // Day la lenh truyen 8bit
  delay_ms(1);
  LCDWriteCmd(0x0C); // Lenh hien thi va con tro nhap nhay 	
  delay_ms(1);
  LCDWriteCmd(0x01); // Lenh xoa man hinh
  delay_ms(10);
  LCDWriteCmd(0x06); // Dich con tro sang phai 1 vi tri
  delay_ms(1);
 }
 //================================
void LCD_Clear() {
  LCDWriteCmd(0x01);
  delay_ms(20);
 }
 //===============================
 //=============================
void LCD_putstr(unsigned char * s) {
 while ( * s) {
  LCDWriteData( * s);
  s++;
  delay_ms(5);
 }
}





