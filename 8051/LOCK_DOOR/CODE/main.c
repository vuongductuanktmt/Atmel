#include "main.h"
#include "lock/lock.h"

/*********************************MAIN PROGRAM***********************************/
void main() {

 unsigned char i;
 unsigned char stt = 0, kt = 0;
 unsigned char dem;
 LCD_init();

 EEP_24CXX_Init();
 BUZZER = 0;
 Tb_A = 0;
 screen:
  screen_time();
 lcd_custom_char(0, Character2);
 lcd_custom_char(2, Character1);
 lcd_custom_char(3, ok);
 strcpy(pass, ' ');
 EEPROM_24CXX_ReadS(0x00, 8, pass);
 newcode();
 while (1) {
  dem = 4;
  Repass:
   LCD_Clear();
  LCD_putstr("Enter password:");
  if (stt == 0) {
   lcd_custom_char(1, icon[dem--]);
  }
  for (i = 0; i < 8; i++) {
   LCDcursorxy(2, 5 + i);
   LCDWriteData(0x00);
  }
  LCDcursorxy(2, 16);
  LCDWriteData(0x01);
  LCDcursorxy(2, 5);
  if (getinput(8, 0) == 1) goto screen;

  if (check(input, pass, 8) == 1) {
   LCD_Clear();
   LCDWriteData(0x03);
   LCD_putstr(" Unlock device");
   delay_ms(1000);
   if (menu() == 1) {
    dem = 4;
    goto Repass;
   }
   stt = 1;
   while (1);
  } else {
   LCD_Clear();
   LCD_putstr("Pass not true!!!");
   buzzers(3);
   if (dem == -1 && kt == 1) {
    LCD_Clear();
    while (1) {
     LCD_putstr("Warning ");
     LCDWriteData(0x02);
     LCDcursorxy(1, 1);
     BUZZER = 1;
    }
   }
   if (dem == -1 && kt == 0) {
    LCD_Clear();
    LCD_putstr("Wait to retype:");

    for (i = 30; i > 0; i--) {
     itoa(i, x);
     LCDcursorxy(2, 8);
     LCD_putstr(x);
     delay_ms(1000);
    }

    kt = 1;
    dem = 4;
   }
   LCD_Clear();
   goto Repass;
  }
 }
}