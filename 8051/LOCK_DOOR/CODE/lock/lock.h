unsigned char input[9], re_input[9], pass[9], x[5], check_menu = 0, hour_a, min_a, year_a, month_a, day_a;
unsigned char NgayThang[] = {
 0,
 31,
 28,
 31,
 30,
 31,
 30,
 31,
 31,
 30,
 31,
 30,
 31
};
unsigned char year, hour, min, sec, day, month;
unsigned char temp; //Bien luu gia tri nhiet do
unsigned char code Character1[8] = {
 17,
 14,
 17,
 27,
 21,
 10,
 14,
 17
}; // graph 
unsigned char code Character2[] = {
 0,
 0,
 10,
 31,
 31,
 14,
 4,
 0
}; // heart
unsigned char code ok[] = {
 0x0,
 0x1,
 0x3,
 0x16,
 0x1c,
 0x8,
 0x0,
 0x0
};
unsigned char code icon[5][8] = {
 {
  4,
  4,
  4,
  4,
  0,
  0,
  0,
  0
 }, //lan 1
 {
  10,
  10,
  10,
  10,
  0,
  0,
  0,
  0
 }, //lan 2
 {
  21,
  21,
  21,
  21,
  0,
  0,
  0,
  0
 }, //lan 3
 {
  21,
  21,
  21,
  18,
  0,
  0,
  0,
  0
 }, //lan 4
 {
  10,
  10,
  10,
  4,
  0,
  0,
  0,
  0
 } //lan 5
};
bit LCD_blink(unsigned char * s) {
 unsigned char i = 0, j = 15, k;
 while (s[j] != '\0') {
  LCDcursorxy(1, 1);
  for (k = i; k <= j; k++) {
   if (READ_SWITCHES() == 'C') {
    return 0;
   }
   LCDWriteData(s[k]);
  }
  i++;
  j++;
  delay_ms(350);
 }

 return 1;
}
unsigned char code alarm_[] = {
 27,
 14,
 17,
 21,
 21,
 17,
 14,
 10
};
/* reverse:	reverse string s in place */
void reverse(unsigned char s[], unsigned char n) {
 unsigned char i, j;
 unsigned char c;

 for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
  c = s[i];
  s[i] = s[j];
  s[j] = c;
 }
 if (n <= 9) {
  s[i + 1] = s[i];
  s[i] = '0';
 }
}


/* itoa:	convert n to characters in s */
void itoa(unsigned char n, unsigned char s[]) {
 unsigned char i, sign;
 if ((sign = n) < 0) /* record sign */
  n = -n; /* make n positive */
 i = 0;
 do { /* generate digits in reverse order */
  s[i++] = n % 10 + '0'; /* get next digit */
 } while ((n /= 10) > 0); /* delete it */
 if (sign < 0)
  s[i++] = '-';
 s[i] = '\0';
 reverse(s, sign);
}
void buzzers(unsigned char x) {
 unsigned char i;
 for (i = 0; i < x; i++) {
  BUZZER = 1;
  delay_ms(300);
  BUZZER = 0;
  delay_ms(400);
 }
}
char get_key(void) //get key from user
 {
  unsigned char key = 'n'; //assume no key pressed

  while (key == 'n') //wait untill a key is pressed
   key = READ_SWITCHES(); //scan the keys again and again
  if (key) {
   BUZZER = 1;
   delay_ms(100);
   BUZZER = 0;
  }
  return key; //when key pressed then return its value
 }
bit getinput(unsigned char len, unsigned char x) {
 unsigned char key, i = 0;
 while (len != 0) {
  check: while (!(key = get_key()));
  if (x == 1) {
   EEP_24CXX_Write(0x00 + i, key);
  }
  if (i <= 0 && key == 'C') {
   LCD_Clear();
   return 1;
  }
  if (key == 'C') {
   i--;
   input[i] = '\0';
   len++;
   LCDcursorxy(2, 13 - len);
   LCDWriteData(0x00);
   goto check;
  }

  input[i] = key;
  LCDcursorxy(2, 13 - len);
  LCDWriteData(key);
  if (x == 0) {
   delay_ms(100);
   LCDcursorxy(2, 13 - len);
   LCDWriteData('*');
  }

  len--;
  i++;
 }
 return 0;
}
bit check(unsigned char * first, unsigned char * second, unsigned char len) {
 unsigned char i = 0;
 for (i = 0; i < len; i++) {
  if (first[i] != second[i])
   return 0;
 }
 return 1;
}
void lcd_custom_char(unsigned char add, unsigned char * k) {
 unsigned char i;
 // cg ram address starts from 0x40 upto next 64 bytes
 if (add < 8) //If valid address
 {
  LCDWriteCmd(0x40 + (add * 8)); //Write to CGRAM address location
  for (i = 0; i < 8; i++)
   LCDWriteData(k[i]); //Write the character pattern to CGRAM location
 }
 LCDWriteCmd(0x80); //shift back to DDRAM location 0
}
void newcode() {
 unsigned char i;
 if (strlen(pass) != 8) {
  Reinput: LCD_Clear();
  LCD_putstr("New Password:");
  for (i = 0; i < 8; i++) {
   LCDcursorxy(2, 5 + i);
   LCDWriteData(0x00);
  }
  getinput(8, 1);
  strcpy(re_input, input);
  LCD_Clear();
  LCD_putstr("Retype new pass:");
  for (i = 0; i < 8; i++) {
   LCDcursorxy(2, 5 + i);
   LCDWriteData(0x00);
  }
  getinput(8, 1);
  if (check(re_input, input, 8)) {
   EEP_24CXX_WriteS(0x00, input);
  } else {
   LCD_Clear();
   LCD_putstr("Error not true!");
   buzzers(3);
   goto Reinput;
  }
  LCD_Clear();
  LCDWriteData(0x03);
  LCD_putstr(" Save Password");
  delay_ms(3000);
 }
}
void changecode() {
 unsigned char i;
 EEP_24CXX_Init();
 Retype:
  LCD_Clear();
 LCD_putstr("Old Password:");
 for (i = 0; i < 8; i++) {
  LCDcursorxy(2, 5 + i);
  LCDWriteData(0x00);
 }
 getinput(8, 0);
 if (check(input, pass, 8) == 1) {
  Reinput: LCD_Clear();
  LCD_putstr("New Password:");
  for (i = 0; i < 8; i++) {
   LCDcursorxy(2, 5 + i);
   LCDWriteData(0x00);
  }
  getinput(8, 1);
  strcpy(re_input, input);
  LCD_Clear();
  LCD_putstr("Retype new pass:");
  for (i = 0; i < 8; i++) {
   LCDcursorxy(2, 5 + i);
   LCDWriteData(0x00);
  }
  getinput(8, 1);
  if (check(re_input, input, 8)) {
   EEP_24CXX_WriteS(0x00, input);
   LCD_Clear();
   LCDWriteData(0x03);
   LCD_putstr(" Save Password");
   delay_ms(3000);

  } else {
   LCD_Clear();
   LCD_putstr("Error not true!");
   buzzers(3);
   goto Reinput;
  }
 } else {
  LCD_Clear();
  LCD_putstr("Pass not true!!");
  buzzers(3);
  goto Retype;
 }
}
void display_time(unsigned char i) {
 if (i == 0) {
	 day_a=day;
	 month_a=month;
	 year_a=year;
	 hour_a=hour;
	 min_a=min;
 }
  LCDWriteCmd(0x0C);
  LCDcursorxy(1, 1);
  itoa(day_a, x);
  LCD_putstr(x);
  LCD_putstr("-");
  itoa(month_a, x);
  LCD_putstr(x);
  LCD_putstr("-");
  itoa(year_a, x);
  LCD_putstr(x);
  LCDcursorxy(2, 1);
  itoa(hour_a, x);
  LCD_putstr(x);
  LCD_putstr(":");
  itoa(min_a, x);
  LCD_putstr(x);
  LCDcursorxy(1, 1);
  LCDWriteCmd(0x0F);
}
char change_time() {
 unsigned char tam = 1, c;
 LCD_Clear();
 DS1307_GetTime();
 DS1307_GetDate();
 display_time(0);
 LCDWriteCmd(0x14);
 c = ' ';
 while (c != 'C') {
  if (c == '=') {
   tam--;
  }
  if (tam == 0) {
   tam = 5;

  }
  if (c == '+') {
   tam++;
  }
  if (tam == 6) {
   tam = 1;
  }

  switch (tam) {
   case 1:
    c = ' ';
    LCDcursorxy(1, 2);
    while (c != '=' && c != '+') {
     c = get_key();
     if (c == '0') {
      sec = 0;
      DS1307_Set();
      LCD_Clear();
      LCD_putstr("Save Time!");
      LCDWriteCmd(0x0C);
      delay_ms(1000);
      return 1;
     }
     if (c == 'C') {
      return 1;
     }
     if (c == '=' && c == '+') {
      break;
     }
     switch (c) {
      case '-':
       day--;
       break;
      case 'x':
       day++;
       break;
      default:
       break;
     }
     if (year + 2000 % 4 == 0 && year + 2000 % 100 != 0 || year + 2000 % 400 == 0) {
      NgayThang[2] = 29;
     }
     if (day == NgayThang[month] + 1) day = 1;
     if (day == 0) day = NgayThang[month];
     LCDcursorxy(1, 1);
     itoa(day, x);
     LCD_putstr(x);
     LCDWriteCmd(0x10);

    }

    break;
   case 2:
    c = ' ';
    LCDcursorxy(1, 5);
    while (c != '=' && c != '+') {
     c = get_key();
     if (c == '0') {
      sec = 0;
      DS1307_Set();
      LCD_Clear();
      LCD_putstr("Save Time!");
      LCDWriteCmd(0x0C);
      delay_ms(1000);
      return 1;
     }
     if (c == 'C') {
      return 1;
     }
     if (c == '=' && c == '+') {
      break;
     }
     switch (c) {
      case '-':
       month--;
       break;
      case 'x':
       month++;
       break;
      default:
       break;
     }
     if (month == 0) month = 12;
     if (month == 13) month = 1;
     LCDcursorxy(1, 4);
     itoa(month, x);
     LCD_putstr(x);
     LCDWriteCmd(0x10);

    }
    break;
   case 3:
    c = ' ';
    LCDcursorxy(1, 8);
    while (c != '=' && c != '+') {
     c = get_key();
     if (c == '0') {
      sec = 0;
      DS1307_Set();
      LCD_Clear();
      LCD_putstr("Save Time!");
      LCDWriteCmd(0x0C);
      delay_ms(1000);
      return 1;
     }
     if (c == 'C') {
      return 1;
     }
     if (c == '=' && c == '+') {
      break;
     }
     switch (c) {
      case '-':
       year--;
       break;
      case 'x':
       year++;
       break;
      default:
       break;
     }
     LCDcursorxy(1, 7);
     itoa(year, x);
     LCD_putstr(x);
     LCDWriteCmd(0x10);

    }
    break;
   case 4:
    c = ' ';
    LCDcursorxy(2, 2);
    while (c != '=' && c != '+') {
     c = get_key();
     if (c == '0') {
      sec = 0;
      DS1307_Set();
      LCD_Clear();
      LCD_putstr("Save Time!");
      LCDWriteCmd(0x0C);
      delay_ms(1000);
      return 1;
     }
     if (c == 'C') {
      return 1;
     }
     if (c == '=' && c == '+') {
      break;
     }
     switch (c) {
      case '-':
       hour--;
       break;
      case 'x':
       hour++;
       break;
      default:
       break;
     }
     if (hour == 24) hour = 0;
     if (hour == -1) hour = 23;
     LCDcursorxy(2, 1);
     itoa(hour, x);
     LCD_putstr(x);
     LCDWriteCmd(0x10);

    }
    break;
   case 5:
    c = ' ';
    LCDcursorxy(2, 5);
    while (c != '=' && c != '+') {
     c = get_key();
     if (c == '0') {
      sec = 0;
      DS1307_Set();
      LCD_Clear();
      LCD_putstr("Save Time!");
      LCDWriteCmd(0x0C);
      delay_ms(1000);
      return 1;
     }
     if (c == 'C') {
      return 1;
     }
     if (c == '=' && c == '+') {
      break;
     }
     switch (c) {
      case '-':
       min--;
       break;
      case 'x':
       min++;
       break;
      default:
       break;
     }
     LCDcursorxy(2, 4);
     if (min == 60) min = 0;
     if (min == -1) min = 59;
     itoa(min, x);
     LCD_putstr(x);
     LCDWriteCmd(0x10);

    }
    break;
   default:
    tam = 1;
    break;
  }
 }
 return 0;
}
void alarm_buzzer() {
 unsigned char tam = 1;
 LCD_Clear();
 LCD_putstr("Please wake up!");
 LCDcursorxy(2, 3);
 LCD_putstr("First time");
 while (tam != 31) {
  if (READ_SWITCHES() == 'C') goto back;
  BUZZER=1;
	 delay_ms(200);
	 BUZZER=0;
	 delay_ms(200);
  tam++;
 }
 LCD_Clear();
 LCD_putstr("I feel sleepy");
 delay_ms(5000);
 LCD_Clear();
 LCD_putstr("Please wake up!");
 LCDcursorxy(2, 4);
 LCD_putstr("Last time");
 tam = 1;
 while (tam != 31) {
  if (READ_SWITCHES() == 'C') goto back;
BUZZER=1;
	 delay_ms(200);
	 BUZZER=0;
	 delay_ms(200);
  tam++;
 }
 back:
  LCD_Clear();
 LCD_putstr("A nice day ^_^!.");
 delay_ms(10000);
}
char alarm() {
 unsigned char tam = 1, c;
	if (year_a>year||month_a>month||day_a>day||hour_a > hour || min_a > min) {
			LCD_Clear();
			LCD_putstr("Cancel(0)  NO(1)");
			if(get_key()=='0')
			{
			goto cancel;
			}else{
       LCDWriteCmd(0x0C);
       goto ok;
			}
      }
			cancel:
 LCD_Clear();
 DS1307_GetTime();
 DS1307_GetDate();
 year_a = year;
 month_a = month;
 day_a = day;
 hour_a = hour;
 min_a = min;
 LCDcursorxy(1, 1);
 LCDcursorxy(2, 1);
 display_time(1);
 LCDWriteCmd(0x14);
 c = ' ';
 while (c != 'C') {
  if (c == '=') {
   tam--;
  }
  if (tam == 0) {
   tam = 5;

  }
  if (c == '+') {
   tam++;
  }
  if (tam == 6) {
   tam = 1;
  }

  switch (tam) {
   case 1:
    c = ' ';
    LCDcursorxy(1, 2);
    while (c != '=' && c != '+') {
     c = get_key();
     if (c == '0') {
       if (year_a>year||month_a>month||day_a>day||hour_a > hour || min_a > min) {
       LCDWriteCmd(0x0C);
       goto ok;
      }
     }
     if (c == 'C') {
			 day_a=month_a=year_a=hour_a=min_a=0;
      return 1;
     }
     if (c == '=' && c == '+') {
      break;
     }
     switch (c) {
      case '-':
       day_a--;
       break;
      case 'x':
       day_a++;
       break;
      default:
       break;
     }
     if (year_a + 2000 % 4 == 0 && year_a + 2000 % 100 != 0 || year_a + 2000 % 400 == 0) {
      NgayThang[2] = 29;
     }
     if (day_a == NgayThang[month_a] + 1) day_a = 1;
     if (day_a == 0) day_a = NgayThang[month_a];
     LCDcursorxy(1, 1);
     itoa(day_a, x);
     LCD_putstr(x);
     LCDWriteCmd(0x10);

    }

    break;
   case 2:
    c = ' ';
    LCDcursorxy(1, 5);
    while (c != '=' && c != '+') {
     c = get_key();
			if (c == '0') {
       if (year_a>year||month_a>month||day_a>day||hour_a > hour || min_a > min) {
       LCDWriteCmd(0x0C);
       goto ok;
      }
     }
     if (c == 'C') {
			 day_a=month_a=year_a=hour_a=min_a=0;
      return 1;
     }
     switch (c) {
      case '-':
       month_a--;
       break;
      case 'x':
       month_a++;
       break;
      default:
       break;
     }
     if (month_a == 0) month_a = 12;
     if (month_a == 13) month_a = 1;
     LCDcursorxy(1, 4);
     itoa(month_a, x);
     LCD_putstr(x);
     LCDWriteCmd(0x10);

    }
    break;
   case 3:
    c = ' ';
    LCDcursorxy(1, 8);
    while (c != '=' && c != '+') {
     c = get_key();
			if (c == '0') {
       if (year_a>year||month_a>month||day_a>day||hour_a > hour || min_a > min) {
       LCDWriteCmd(0x0C);
       goto ok;
      }
     }
     if (c == 'C') {
			 day_a=month_a=year_a=hour_a=min_a=0;
      return 1;
     }
     switch (c) {
      case '-':
       year_a--;
       break;
      case 'x':
       year_a++;
       break;
      default:
       break;
     }
     LCDcursorxy(1, 7);
     itoa(year_a, x);
     LCD_putstr(x);
     LCDWriteCmd(0x10);

    }
    break;
   case 4:
    c = ' ';
    LCDcursorxy(2, 2);
    while (c != '=' && c != '+') {
     c = get_key();
			if (c == '0') {
       if (year_a>year||month_a>month||day_a>day||hour_a > hour || min_a > min) {
       LCDWriteCmd(0x0C);
       goto ok;
      }
     }
     if (c == 'C') {
			 day_a=month_a=year_a=hour_a=min_a=0;
      return 1;
     }
     if (c == '=' && c == '+') {
      break;
     }
     switch (c) {
      case '-':
       hour_a--;
       break;
      case 'x':
       hour_a++;
       break;
      default:
       break;
     }
     if (hour_a == 24) hour_a = 0;
     if (hour_a == -1) hour_a = 23;
     LCDcursorxy(2, 1);
     itoa(hour_a, x);
     LCD_putstr(x);
     LCDWriteCmd(0x10);

    }
    break;
   case 5:
    c = ' ';
    LCDcursorxy(2, 5);
    while (c != '=' && c != '+') {
     c = get_key();
     if (c == '0') {
      if (year_a>year||month_a>month||day_a>day||hour_a > hour || min_a > min) {
       LCDWriteCmd(0x0C);
       goto ok;
      }
     }
     if (c == 'C') {
			 day_a=month_a=year_a=hour_a=min_a=0;
      return 1;
     }
     switch (c) {
      case '-':
       min_a--;
       break;
      case 'x':
       min_a++;
       break;
      default:
       break;
     }
     LCDcursorxy(2, 4);
     if (min_a == 60) min_a = 0;
     if (min_a == -1) min_a = 59;
     itoa(min_a, x);
     LCD_putstr(x);
     LCDWriteCmd(0x10);

    }
    break;
   default:
    tam = 1;
    break;
  }
 }
 ok:
  LCD_Clear();
 while (READ_SWITCHES() != 'C') {
  DS1307_GetTime();
  DS1307_GetDate();
  LCDWriteCmd(0x0C);
  LCDcursorxy(1, 1);
  itoa(day, x);
  LCD_putstr(x);
  LCD_putstr("-");
  itoa(month, x);
  LCD_putstr(x);
  LCD_putstr("-");
  itoa(year, x);
  LCD_putstr(x);
  LCD_putstr(" | ");
  itoa(hour, x);
  LCD_putstr(x);
  LCD_putstr(":");
  itoa(min, x);
  LCD_putstr(x);
  LCDcursorxy(2, 1);
  itoa(day_a, x);
  LCD_putstr(x);
  LCD_putstr("-");
  itoa(month_a, x);
  LCD_putstr(x);
  LCD_putstr("-");
  itoa(year_a, x);
  LCD_putstr(x);
  LCD_putstr(" | ");
  itoa(hour_a, x);
  LCD_putstr(x);
  LCD_putstr(":");
  itoa(min_a, x);
  LCD_putstr(x);



  if (year_a == year && month_a == month && day_a == day && hour_a == hour && min_a == min) {
   alarm_buzzer();
   break;
  }
 }
 return 1;
}
void DS1307_Set() {
 //Ghi du lieu ngay gio vao DS1307
 DS1307_Write(DS1307_SEC, sec);
 DS1307_Write(DS1307_MIN, min);
 DS1307_Write(DS1307_HOUR, hour);
 DS1307_Write(DS1307_DATE, day);
 DS1307_Write(DS1307_MONTH, month);
 DS1307_Write(DS1307_YEAR, year);
}
void DS1307_GetTime() {
 //Doc du lieu gio tu DS1307
 hour = DS1307_Read(DS1307_HOUR);
 min = DS1307_Read(DS1307_MIN);
 sec = DS1307_Read(DS1307_SEC);
}
void DS1307_GetDate() {
  //Doc du lieu ngay tu DS1307
  day = DS1307_Read(DS1307_DATE);
  month = DS1307_Read(DS1307_MONTH);
  year = DS1307_Read(DS1307_YEAR);
 } //End code DS1307
 /****************Ctr giao tiep IC DS18B20***************************/
void delay_18B20(unsigned int ti) {
 while (ti--);
}
void Init_18B20(void) {
 DQ = 1;
 delay_18B20(8);
 DQ = 0;
 delay_18B20(65);
 DQ = 1;
 delay_18B20(20);
}
unsigned char ReadOneChar(void) {
 unsigned char i = 0;
 unsigned char dat = 0;
 for (i = 8; i > 0; i--) {
  DQ = 0;
  dat >>= 1;
  DQ = 1;
  if (DQ)
   dat |= 0x80;
  delay_18B20(4);
 }
 return (dat);
}
void WriteOneChar(unsigned char dat) {
 unsigned char i = 0;
 for (i = 8; i > 0; i--) {
  DQ = 0;
  DQ = dat & 0x01;
  delay_18B20(5);
  DQ = 1;
  dat >>= 1;
 }
}
void ReadTemperature(void) {
  unsigned char a = 0;
  unsigned char b = 0;
  //EA=0;//Cam ngat tranh viec anh huong qua trinh doc nhiet do
  Init_18B20();
  WriteOneChar(0xCC); // Bo qua ROM
  WriteOneChar(0x44);
  delay_18B20(10);
  Init_18B20();
  WriteOneChar(0xCC);
  WriteOneChar(0xBE); //Doc nhiet do
  delay_18B20(10);
  a = ReadOneChar(); //Read temp low value
  b = ReadOneChar(); //Read temp high value
  temp = ((b * 256 + a) >> 4); //gia tri nhiet do luu vao bien temp
  //EA=1; //Doc xong thi cho phep ngat
 } //End code DS18B20
void screen_time() {
	unsigned char i=0;
 lcd_custom_char(4, alarm_);
 while (READ_SWITCHES() == 'n') {
  LCDcursorxy(1, 1);
  ReadTemperature();
  itoa(temp, x);
  LCD_putstr(x);
  LCDWriteData(0xDF);
  LCD_putstr("C  | ");
  DS1307_GetTime();
  DS1307_GetDate();
  if (year_a>year||month_a>month||day_a>day||hour_a > hour || min_a > min) {
   LCDcursorxy(1, 6);
   LCDWriteData(0x04);
  }
  if (year_a == year && month_a == month && day_a == day && hour_a == hour && min_a == min&&i==0){
   alarm_buzzer();
	i=1;
  }
  LCDcursorxy(1, 9);
  itoa(day, x);
  LCD_putstr(x);
  LCD_putstr("-");
  itoa(month, x);
  LCD_putstr(x);
  LCD_putstr("-");
  itoa(year, x);
  LCD_putstr(x);
  LCDcursorxy(2, 1);
  temp = (temp * 1.8) + 32;
  itoa(temp, x);
  LCD_putstr(x);
  LCDWriteData(0xDF);
  if (temp > 99) {
   LCD_putstr("F | ");
  } else {
   LCD_putstr("F  | ");
  }
  x[2] = '\0';
  itoa(hour, x);
  LCD_putstr(x);
  LCD_putstr(":");
  itoa(min, x);
  LCD_putstr(x);
  LCD_putstr(":");
  itoa(sec, x);
  LCD_putstr(x);
  if (READ_SWITCHES() != 'n') break;
 }
}

bit menu() {
 Home: if (check_menu == 0) {
   menu_1: LCD_Clear();
   LCDcursorxy(1, 1);
   LCD_putstr("Device");
   LCDcursorxy(2, 2);
   LCD_putstr("1");
   LCDcursorxy(1, 7);
   LCD_putstr("  Setting");
   LCDcursorxy(2, 12);
   LCD_putstr("2");
   ///
   switch (get_key()) {
    case '1':
     LCD_Clear();
     LCDcursorxy(1, 1);
     LCD_putstr("ON");
     LCDcursorxy(2, 1);
     LCD_putstr("1");
     LCDcursorxy(1, 8);
     LCD_putstr("OFF");
     LCDcursorxy(2, 8);
     LCD_putstr("0");

     while (1) {
      switch (get_key()) {
       case '0':
        Tb_A = 0;
        break;
       case '1':
        Tb_A = 1;
        break;
       case 'C':
        goto Home;
        break;
       default:

        break;
      }
     }
     break;
    case '2':
     setting:
      LCD_Clear();
     LCD_putstr("Ch.Pass ");
     LCD_putstr("Time");
     LCDcursorxy(2, 1);
     LCD_putstr("1");
     LCDcursorxy(2, 9);
     LCD_putstr("2");
     switch (get_key()) {
      case '1':
       changecode();
       goto setting;
       break;
      case '2':
       LCDWriteCmd(0x0f);
       change_time();
       LCDWriteCmd(0x0C);
       goto setting;
       break;
      case 'C':
       goto Home;
      default:
       goto setting;
       break;
     }
     break;
    case 'C':
     return 1;
     break;
    case '+':
     check_menu = 1;
     break;
    case '=':
     check_menu = 0;
    default:
     goto menu_1;
     break;
   }
  }
 if (check_menu == 1) {
  alarm: LCD_Clear();
  LCDcursorxy(1, 1);
  LCD_putstr("Alarm");
  LCDcursorxy(2, 2);
  LCD_putstr("1");
  LCDcursorxy(1, 7);
  LCD_putstr("Info");
  LCDcursorxy(2, 10);
  LCD_putstr("2");
  while (1) {
   switch (get_key()) {
    case '1':
     LCDWriteCmd(0x0f);
     alarm();
     LCDWriteCmd(0x0C);
     goto alarm;
     break;
    case '2':
     LCD_Clear();
     while (READ_SWITCHES() != 'C') {
      LCDcursorxy(2, 1);
      LCD_putstr("Ahihi ...^_^...");
      if (LCD_blink("Developer: AnhTuan, Facebook: Vuong Duc Tuan, Phone: 01686918781, Slogan: Tiet kiem thoi gian la tiet kiem suc khoe.") == 0) {
       break;
      }
     }
     goto alarm;
     break;
    case '=':
     check_menu = 0;
     goto menu_1;
     break;
    case 'C':
     check_menu = 0;
     goto Home;
     break;
    default:
     goto alarm;
     break;
   }
  }
 }
 return 0;
}