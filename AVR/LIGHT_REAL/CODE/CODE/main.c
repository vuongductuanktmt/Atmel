//-----------------------------------------------------------------------------
// i2c01: Experiments with interfacing ATmega328 to an DS1307 RTC
//
// Author : Bruce E. Hall <bhall66@gmail.com>
// Website : w8bh.net
// Version : 1.1
// Date : 7 Sep 2013
// Target : ATTmega328P microcontroller
// Language : C, using AVR studio 6
// Size : 1386 bytes, using -O1 optimization
//
// Fuse settings: 8 MHz osc with 65 ms Delay, SPI enable; *NO* clock/8
// ---------------------------------------------------------------------------
// GLOBAL DEFINES
#define F_CPU 4000000L // run CPU at 4 MHz
#define RELAY 0 // Boarduino LED on PB5
#define OK PC1
#define UP PC2
#define DOWN PC3
#define ClearBit(x,y) x &= ~_BV(y) // equivalent to cbi(x,y)
#define SetBit(x,y) x |= _BV(y) // equivalent to sbi(x,y)
// ---------------------------------------------------------------------------
// INCLUDES
#include <avr/io.h> // deal with port registers
#include <util/delay.h> // used for _delay_ms function
#include <string.h> // string manipulation routines
#include <stdlib.h>
#include <avr/sfr_defs.h>
#include <avr/eeprom.h>
#include "ADC.h"
#include <stdio.h>
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
unsigned char heart[] = {
	0,
	0,
	10,
	31,
	31,
	14,
	4,
	0
}; // heart
void MainLoop();
// ---------------------------------------------------------------------------
uint16_t EEMEM hours__,min__,saveMyValueNeg,mode_,check_;
// TYPEDEFS
typedef uint8_t byte; // I just like byte & sbyte better
typedef int8_t sbyte;
// ---------------------------------------------------------------------------
// MISC ROUTINES
void InitAVR()
{
	DDRB = 0xff; // 1111.1111; set B0-B5 as outputs
	DDRC = 0x00; // 0000.0000; set PORTC as inputs
	PORTC= 0xFF;
	DDRD = 0xff;
}
void msDelay(int delay) // put into a routine
{   // to remove code inlining
	for (int i=0; i<delay; i++) // at cost of timing accuracy
	_delay_ms(1);
}
void FlashLED()
{
	SetBit(PORTC,OK);
	msDelay(250);
	ClearBit(PORTC,OK);
	msDelay(250);

}
// ---------------------------------------------------------------------------
// HD44780-LCD DRIVER ROUTINES
//
// Routines:
// LCD_Init initializes the LCD controller
// LCD_Cmd sends LCD controller command
// LCD_Char sends single ascii character to display
// LCD_Clear clears the LCD display & homes cursor
// LCD_Home homes the LCD cursor
// LCD_Goto puts cursor at position (x,y)
// LCD_Line puts cursor at start of line (x)
// LCD_Hex displays a hexadecimal value
// LCD_Integer displays an integer value
// LCD_String displays a string
//
// The LCD module requires 6 I/O pins: 2 control lines & 4 data lines.
// PortB is used for data communications with the HD44780-controlled LCD.
// The following defines specify which port pins connect to the controller:
#define LCD_RS 1 // pin for LCD R/S (eg PB0)
#define LCD_E 0 // pin for LCD enable
#define DAT4 2 // pin for d4
#define DAT5 3 // pin for d5
#define DAT6 4 // pin for d6
#define DAT7 5 // pin for d7
// The following defines are HD44780 controller commands
#define CLEARDISPLAY 0x01
#define SETCURSOR 0x80
void PulseEnableLine ()
{
	SetBit(PORTB,LCD_E); // take LCD enable line high
	_delay_us(40); // wait 40 microseconds
	ClearBit(PORTB,LCD_E); // take LCD enable line low
}
void SendNibble(byte data)
{
	PORTB &= 0xC3; // 1100.0011 = clear 4 data lines
	if (data & _BV(4)) SetBit(PORTB,DAT4);
	if (data & _BV(5)) SetBit(PORTB,DAT5);
	if (data & _BV(6)) SetBit(PORTB,DAT6);
	if (data & _BV(7)) SetBit(PORTB,DAT7);
	PulseEnableLine(); // clock 4 bits into controller
}
void SendByte (byte data)
{
	SendNibble(data); // send upper 4 bits
	SendNibble(data<<4); // send lower 4 bits
	ClearBit(PORTB,5); // turn off boarduino LED
}
void LCD_Cmd (byte cmd)
{
	ClearBit(PORTB,LCD_RS); // R/S line 0 = command data
	SendByte(cmd); // send it
}
void LCD_Char (byte ch)
{
	SetBit(PORTB,LCD_RS); // R/S line 1 = character data
	SendByte(ch); // send it
}
void LCD_Init()
{
	LCD_Cmd(0x33); // initialize controller
	LCD_Cmd(0x32); // set to 4-bit input mode
	LCD_Cmd(0x28); // 2 line, 5x7 matrix
	LCD_Cmd(0x0C); // turn cursor off (0x0E to enable)
	LCD_Cmd(0x06); // cursor direction = right
	LCD_Cmd(0x01); // start with clear display
	msDelay(3); // wait for LCD to initialize
}
void LCD_Clear() // clear the LCD display
{
	LCD_Cmd(CLEARDISPLAY);
	msDelay(3); // wait for LCD to process command
}
void LCD_Home() // home LCD cursor (without clearing)
{
	LCD_Cmd(SETCURSOR);
}
void LCD_Goto(byte x, byte y) // put LCD cursor on specified line
{
	byte addr = 0; // line 0 begins at addr 0x00
	switch (y)
	{
		case 1:
		addr = 0x40;
		break; // line 1 begins at addr 0x40
		case 2:
		addr = 0x14;
		break;
		case 3:
		addr = 0x54;
		break;
	}
	LCD_Cmd(SETCURSOR+addr+x); // update cursor with x,y position
}
void LCD_Line(byte row) // put cursor on specified line
{
	LCD_Goto(0,row);
}
void LCD_String(const char *text) // display string on LCD
{
	while (*text) // do until /0 character
	LCD_Char(*text++); // send char & update char pointer
}
void LCD_Hex(int data)
// displays the hex value of DATA at current LCD cursor position
{
	char st[8] = ""; // save enough space for result
	itoa(data,st,16); // convert to ascii hex
	//LCD_Message("0x"); // add prefix "0x" if desired
	LCD_String(st); // display it on LCD
}
void LCD_Integer(int data)
// displays the integer value of DATA at current LCD cursor position
{
	char st[8] = ""; // save enough space for result
	itoa(data,st,10); // convert to ascii
	if(data<10) {
		st[1]=st[0];
		st[0]='0';
		st[2]='\0';
	}
	LCD_String(st); // display in on LCD
}
// ---------------------------------------------------------------------------
// I2C (TWI) ROUTINES
//
// On the AVRmega series, PA4 is the data line (SDA) and PA5 is the clock (SCL
// The standard clock rate is 100 KHz, and set by I2C_Init. It depends on the AVR osc. freq.
#define F_SCL 100000L // I2C clock speed 100 KHz
#define READ 1
#define TW_START 0xA4 // send start condition (TWINT,TWSTA,TWEN)
#define TW_STOP 0x94 // send stop condition (TWINT,TWSTO,TWEN)
#define TW_ACK 0xC4 // return ACK to slave
#define TW_NACK 0x84 // don't return ACK to slave
#define TW_SEND 0x84 // send data (TWINT,TWEN)
#define TW_READY (TWCR & 0x80) // ready when TWINT returns to logic 1.
#define TW_STATUS (TWSR & 0xF8) // returns value of status register
#define I2C_Stop() TWCR = TW_STOP // inline macro for stop condition
void I2C_Init()
// at 16 MHz, the SCL frequency will be 16/(16+2(TWBR)), assuming prescalar of 0.
// so for 100KHz SCL, TWBR = ((F_CPU/F_SCL)-16)/2 = ((16/0.1)-16)/2 = 144/2 = 72.
{
	TWSR = 0; // set prescalar to zero
	TWBR = ((F_CPU/F_SCL)-16)/2; // set SCL frequency in TWI bit register
}
byte I2C_Detect(byte addr)
// look for device at specified address; return 1=found, 0=not found
{
	TWCR = TW_START; // send start condition
	while (!TW_READY); // wait
	TWDR = addr; // load device's bus address
	TWCR = TW_SEND; // and send it
	while (!TW_READY); // wait
	return (TW_STATUS==0x18); // return 1 if found; 0 otherwise
}
byte I2C_FindDevice(byte start)
// returns with address of first device found; 0=not found
{
	for (byte addr=start; addr<0xFF; addr++) // search all 256 addresses
	{
		if (I2C_Detect(addr)) // I2C detected?
		return addr; // leave as soon as one is found
	}
	return 0; // none detected, so return 0.
}
void I2C_Start (byte slaveAddr)
{
	I2C_Detect(slaveAddr);
}
byte I2C_Write (byte data) // sends a data byte to slave
{
	TWDR = data; // load data to be sent
	TWCR = TW_SEND; // and send it
	while (!TW_READY); // wait
	return (TW_STATUS!=0x28);
}
byte I2C_ReadACK () // reads a data byte from slave
{
	TWCR = TW_ACK; // ack = will read more data
	while (!TW_READY); // wait
	return TWDR;
	//return (TW_STATUS!=0x28);
}
byte I2C_ReadNACK () // reads a data byte from slave
{
	TWCR = TW_NACK; // nack = not reading more data
	while (!TW_READY); // wait
	return TWDR;
	//return (TW_STATUS!=0x28);
}
void I2C_WriteByte(byte busAddr, byte data)
{
	I2C_Start(busAddr); // send bus address
	I2C_Write(data); // then send the data byte
	I2C_Stop();
}
void I2C_WriteRegister(byte busAddr, byte deviceRegister, char data)
{
	unsigned int temp;
	temp = data; /*HEX to BCD*/
	data = (((data / 10) * 16) | (temp % 10)); /*for Led 7seg*/
	I2C_Start(busAddr); // send bus address
	I2C_Write(deviceRegister); // first byte = device register address
	I2C_Write(data); // second byte = data for device register
	I2C_Stop();
}
byte I2C_ReadRegister(byte busAddr, byte deviceRegister)
{
	byte data = 0;
	I2C_Start(busAddr); // send device address
	I2C_Write(deviceRegister); // set register pointer
	I2C_Start(busAddr+READ); // restart as a read operation
	data = I2C_ReadNACK(); // read the register data
	I2C_Stop(); // stop
	return data;
}
// ---------------------------------------------------------------------------
// DS1307 RTC ROUTINES
#define DS1307 0xD0 // I2C bus address of DS1307 RTC
#define SECONDS_REGISTER 0x00
#define MINUTES_REGISTER 0x01
#define HOURS_REGISTER 0x02
#define DAYOFWK_REGISTER 0x03
#define DAYS_REGISTER 0x04
#define MONTHS_REGISTER 0x05
#define YEARS_REGISTER 0x06
#define CONTROL_REGISTER 0x07
#define RAM_BEGIN 0x08
#define RAM_END 0x3F
//----------------------------------------------------------------------------
void DS1307_GetTime(byte *hours, byte *minutes, byte *seconds)
// returns hours, minutes, and seconds in BCD format
{
	*hours = I2C_ReadRegister(DS1307,HOURS_REGISTER);
	*minutes = I2C_ReadRegister(DS1307,MINUTES_REGISTER);
	*seconds = I2C_ReadRegister(DS1307,SECONDS_REGISTER);
	if (*hours & 0x40) // 12hr mode:
	*hours &= 0x1F; // use bottom 5 bits (pm bit = temp & 0x20)
	else *hours &= 0x3F; // 24hr mode: use bottom 6 bits
}
//----------------------------------------------------------------------------
void DS1307_GetDate(byte *months, byte *days, byte *years)
// returns months, days, and years in BCD format
{
	*months = I2C_ReadRegister(DS1307,MONTHS_REGISTER);
	*days = I2C_ReadRegister(DS1307,DAYS_REGISTER);
	*years = I2C_ReadRegister(DS1307,YEARS_REGISTER);
}
//----------------------------------------------------------------------------
void SetTimeDate( char years,char months,char days,char hours, char minutes, char seconds)
// simple, hard-coded way to set the date.
{
	I2C_WriteRegister(DS1307,MONTHS_REGISTER, months);
	I2C_WriteRegister(DS1307,DAYS_REGISTER, days);
	I2C_WriteRegister(DS1307,YEARS_REGISTER, years);
	I2C_WriteRegister(DS1307,HOURS_REGISTER, hours); // add 0x40 for PM
	I2C_WriteRegister(DS1307,MINUTES_REGISTER, minutes);
	I2C_WriteRegister(DS1307,SECONDS_REGISTER, seconds);
}
// ---------------------------------------------------------------------------
// APPLICATION ROUTINES
void ShowDevices()
// Scan I2C addresses and display addresses of all devices found
{
	LCD_Line(1);
	LCD_String("Found:");
	byte addr = 1;
	while (addr>0)
	{
		LCD_Char(' ');
		addr = I2C_FindDevice(addr);
		if (addr>0) LCD_Hex(addr++);
	}
}
//----------------------------------------------------------------------------
void LCD_TwoDigits(byte data)
// helper function for WriteDate()
// input is two digits in BCD format
// output is to LCD display at current cursor position
{
	byte temp = data>>4;
	LCD_Char(temp+'0');
	data &= 0x0F;
	LCD_Char(data+'0');
}
//----------------------------------------------------------------------------
char TwoDigits(byte data)
// helper function for WriteDate()
// input is two digits in BCD format
// output is to LCD display at current cursor position
{
	char temp1;
	byte temp = data>>4;
	temp1=(temp+0)*10;
	data &= 0x0F;
	temp1=temp1+(data+0);
	return temp1;
}
//----------------------------------------------------------------------------
void WriteDate()
{
	byte months, days, years;
	DS1307_GetDate(&months,&days,&years);
	LCD_TwoDigits(days);
	LCD_Char('/');
	LCD_TwoDigits(months);
	LCD_Char('/');
	LCD_TwoDigits(years);
}
//----------------------------------------------------------------------------
void WriteTime()
{
	byte hours, minutes, seconds;
	DS1307_GetTime(&hours,&minutes,&seconds);
	LCD_TwoDigits(hours);
	LCD_Char(':');
	LCD_TwoDigits(minutes);
	LCD_Char(':');
	LCD_TwoDigits(seconds);
}
//----------------------------------------------------------------------------
void WriteTime_()
{
	byte hours, minutes, seconds;
	DS1307_GetTime(&hours,&minutes,&seconds);
	LCD_TwoDigits(hours);
	LCD_Char(':');
	LCD_TwoDigits(minutes);
}
//----------------------------------------------------------------------------
void LCD_TimeDate()
{
	LCD_Goto(8,0);
	WriteTime();
	LCD_Goto(8,1);
	WriteDate();
}
//----------------------------------------------------------------------------
void lcd_custom_char(unsigned char add, unsigned char * k) {
	unsigned char i;
	// cg ram address starts from 0x40 upto next 64 bytes
	if (add < 8) //If valid address
	{
		LCD_Cmd(0x40 + (add * 8)); //Write to CGRAM address location
		for (i = 0; i < 8; i++)
		LCD_Char(k[i]); //Write the character pattern to CGRAM location
	}
	LCD_Cmd(0x80); //shift back to DDRAM location 0
}
//----------------------------------------------------------------------------
unsigned char LCD_blink( char * s) {
	unsigned char i = 0, j = 15, k;
	while (s[j] != '\0') {
		LCD_Goto(0, 0);
		for (k = i; k <= j; k++) {
			if((bit_is_clear(PINC,OK))) {
				return 0;
			}
			LCD_Char(s[k]);
		}
		i++;
		j++;
		_delay_ms(350);
	}

	return 1;
}
//----------------------------------------------------------------------------
void read_check(unsigned int*check__) {
	*check__= eeprom_read_word(&check_);
	if((*check__!=0)&&(*check__!=1)) {
		check__=0;
	}
}
//----------------------------------------------------------------------------
void read_light(unsigned int*light_) {
	*light_=eeprom_read_word(&saveMyValueNeg);
	if(*light_>1024) {
		*light_=512;
	}
}
//----------------------------------------------------------------------------
void read_info(unsigned int*hours,unsigned int*min) {
	*hours=eeprom_read_word(&hours__);
	*min=eeprom_read_word(&min__);
	if((*hours!=0)&&(*hours>24)&&(*min!=0)&&(*min>60)) {
		*hours=0;
		*min=0;
	}
}
//----------------------------------------------------------------------------
//ChangeTime
char change_time() {
	unsigned char tam = 0,years_,month_,day_,hours_,min_;
	byte years,month,day,hours,min,sec;
	LCD_Clear();
	DS1307_GetDate(&month,&day,&years);
	DS1307_GetTime(&hours,&min,&sec);
	years_=TwoDigits(years);
	month_=TwoDigits(month);
	day_=TwoDigits(day);
	hours_=TwoDigits(hours);
	min_=TwoDigits(min);
	while(1) {
		LCD_Home();
		WriteDate();
		LCD_Line(1);
		WriteTime_();
		while(1) {
			if(tam==5) {
				//save time and back
				SetTimeDate(years_,month_,day_,hours_,min_,0);
				LCD_Clear();
				LCD_Cmd(0x0C);
				LCD_String("Da Luu!");
				_delay_ms(1500);
				goto back;
			}
			switch(tam) {
				case 0:
				LCD_Goto(1,0);
				while(!(bit_is_clear(PINC,OK))) {
					if((bit_is_clear(PINC,UP))) {
						day_++;
						if ((years_ + 2000 % 4 == 0) & (years_ + 2000 % 100 != 0) || (years_ + 2000 % 400 == 0)) {
							NgayThang[2] = 29;
						}
						if (day_ == NgayThang[month_] + 1) day_ = 1;
						LCD_Goto(0,0);
						LCD_Integer(day_);
						LCD_Cmd(0x10);
						_delay_ms(250);
					}
					if((bit_is_clear(PINC,DOWN))) {
						day_--;
						if ((years_ + 2000 % 4 == 0) & (years_ + 2000 % 100 != 0) || (years_ + 2000 % 400 == 0)) {
							NgayThang[2] = 29;
						}
						if (day_ == 0) day_ = NgayThang[month_];
						LCD_Goto(0,0);
						LCD_Integer(day_);
						LCD_Cmd(0x10);
						_delay_ms(250);
					}
				}
				_delay_ms(250);
				tam++;
				break;
				case 1:
				LCD_Goto(4,0);
				while(!(bit_is_clear(PINC,OK))) {

					if((bit_is_clear(PINC,UP))) {
						month_++;
						if (month_ == 13) month_ = 1;
						LCD_Goto(3,0);
						LCD_Integer(month_);
						LCD_Cmd(0x10);
						_delay_ms(250);
					}
					if((bit_is_clear(PINC,DOWN))) {
						month_--;
						if (month_ == 0) month_ = 12;
						LCD_Goto(3,0);
						LCD_Integer(month_);
						LCD_Cmd(0x10);
						_delay_ms(250);
					}
				}
				_delay_ms(250);
				tam++;
				break;
				case 2:
				LCD_Goto(7,0);
				while(!(bit_is_clear(PINC,OK))) {

					if((bit_is_clear(PINC,UP))) {
						years_++;
						LCD_Goto(6,0);
						LCD_Integer(years_);
						LCD_Cmd(0x10);
						_delay_ms(250);
					}
					if((bit_is_clear(PINC,DOWN))) {
						years_--;
						LCD_Goto(6,0);
						LCD_Integer(years_);
						LCD_Cmd(0x10);
						_delay_ms(250);
					}
				}
				_delay_ms(250);
				tam++;
				break;
				case 3:
				LCD_Goto(1,1);
				while(!(bit_is_clear(PINC,OK))) {

					if((bit_is_clear(PINC,UP))) {
						hours_++;
						if (hours_ == 24) hours_ = 0;
						LCD_Goto(0,1);
						LCD_Integer(hours_);
						LCD_Cmd(0x10);
						_delay_ms(250);
					}
					if((bit_is_clear(PINC,DOWN))) {
						hours_--;
						if (hours_!=0&&hours_>23) hours_ = 23;
						LCD_Goto(0,1);
						LCD_Integer(hours_);
						LCD_Cmd(0x10);
						_delay_ms(250);
					}
				}
				_delay_ms(250);
				tam++;
				break;
				case 4:
				LCD_Goto(4,1);
				while(!(bit_is_clear(PINC,OK))) {

					if((bit_is_clear(PINC,UP))) {
						min_++;
						if (min_ == 60) min_ = 0;
						LCD_Goto(3,1);
						LCD_Integer(min_);
						LCD_Cmd(0x10);
						_delay_ms(250);
					}
					if((bit_is_clear(PINC,DOWN))) {
						min_--;
						if (min_!=0&&min_>59) {
							min_ = 59;
						}
						LCD_Goto(3,1);
						LCD_Integer(min_);
						LCD_Cmd(0x10);
						_delay_ms(250);
					}
				}
				_delay_ms(250);
				tam++;
				break;
				default:
				break;
			}

		}

	}
	back:
	return 0;
}
//----------------------------------------------------------------------------
//Ch?nh ánh sáng
void change_light() {
	unsigned int lights_ADC,temp1;
	lights_ADC=read_adc_channel();
	temp1=lights_ADC;
	LCD_Clear();
	LCD_String("Chinh Anh sang:");
	LCD_Line(1);
	LCD_String("AS:");
	read_light(&temp1);
	while(1) {
		lights_ADC=read_adc_channel();
		if(lights_ADC<1000) {
			LCD_Goto(3,1);
			LCD_Char(' ');
		}
		else {
			LCD_Goto(3,1);
		}
		LCD_Integer(lights_ADC);
		if(lights_ADC<100) {
			LCD_String(" ");
		}
		LCD_String("|ADC:");
		if(temp1<1000) {
			LCD_Goto(12,1);
			LCD_Char(' ');
		}
		else {
			LCD_Goto(12,1);
		}
		LCD_Integer(temp1);

		if((bit_is_clear(PINC,UP))) {
			temp1++;
			if(temp1==1024) {
				temp1=0;
			}
			_delay_ms(100);
		}
		if((bit_is_clear(PINC,DOWN))) {
			temp1--;
			if(temp1!=0&&temp1>1023) {
				temp1=1023;
			}
			_delay_ms(100);
		}
		if((bit_is_clear(PINC,OK))) {
			eeprom_write_word(&saveMyValueNeg,temp1);
			_delay_ms(250);
			LCD_Clear();
			LCD_Cmd(0x0C);
			LCD_String("Da Luu! ");
			_delay_ms(1500);
			LCD_Clear();
			break;
		}

	}

}
//----------------------------------------------------------------------------
void change_device() {
	unsigned int hours_=6,min_=0;
	read_info(&hours_,&min_);
	LCD_Clear();
	LCD_String("Hen Gio:");
	LCD_Line(1);
	LCD_Integer(hours_);
	LCD_Char(':');
	LCD_Integer(min_);
	while(1) {
		LCD_Goto(1,1);
		while(!(bit_is_clear(PINC,OK))) {

			if((bit_is_clear(PINC,UP))) {
				hours_++;
				if (hours_ == 24) hours_ = 0;
				LCD_Goto(0,1);
				LCD_Integer(hours_);
				LCD_Cmd(0x10);
				_delay_ms(250);
			}
			if((bit_is_clear(PINC,DOWN))) {
				hours_--;
				if (hours_!=0&&hours_>23) hours_ = 23;
				LCD_Goto(0,1);
				LCD_Integer(hours_);
				LCD_Cmd(0x10);
				_delay_ms(250);
			}
		}
		_delay_ms(250);
		LCD_Goto(4,1);
		while(!(bit_is_clear(PINC,OK))) {

			if((bit_is_clear(PINC,UP))) {
				min_++;
				if (min_ >= 60) min_ = 0;
				LCD_Goto(3,1);
				LCD_Integer(min_);
				LCD_Cmd(0x10);
				_delay_ms(250);
			}
			if((bit_is_clear(PINC,DOWN))) {
				min_--;
				if ((min_!=0&&min_>59)) {
					min_ = 59;
				}
				LCD_Goto(3,1);
				LCD_Integer(min_);
				LCD_Cmd(0x10);
				_delay_ms(250);
			}
		}
		LCD_Clear();
		LCD_Cmd(0x0C);
		LCD_String("Da Luu!");
		eeprom_write_word(&hours__,hours_);
		eeprom_write_word(&min__,min_);
		_delay_ms(1500);
		break;
	}
}
void read_mode(unsigned int*mode__) {
	*mode__=eeprom_read_word(&mode_);
	if((*mode__!=1)&&(*mode__!=2)) {
		*mode__=1;
	}
}
//----------------------------------------------------------------------------
void change_mode() {
	unsigned int dem;
	read_mode(&dem);
	LCD_Clear();
	LCD_String("Chon che do:");
	while(!(bit_is_clear(PINC,OK))) {
		if((bit_is_clear(PINC,UP))) {
			dem++;
			if(dem==3) {
				dem=1;
			}
			_delay_ms(250);
		}
		if((bit_is_clear(PINC,DOWN))) {

			dem--;
			if(dem==0) {
				dem=2;
			}
			_delay_ms(250);
		}
		switch(dem) {
			case 1:
			LCD_Goto(0,1);
			LCD_String("Ket hop         ");
			break;
			case 2:
			LCD_String("                ");
			LCD_Goto(0,1);
			LCD_String("Theo thoi gian  ");
			default:
			break;
		}
	}
	LCD_Clear();
	eeprom_write_word(&mode_,dem);
	LCD_String("Da luu!");
	_delay_ms(1500);
	LCD_Clear();
}
//----------------------------------------------------------------------------
//Menu
int menu() {
	unsigned char bac=0,counts=0;
	long count=0;
	LCD_Clear();
	while(1) {
		if(count>20000) {
			LCD_Clear();
			return 0;
		}
		if((bit_is_clear(PINC,UP))) {
			bac=(bac==4)?0:bac+1;
			LCD_Clear();
			_delay_ms(250);
		}

		if((bit_is_clear(PINC,DOWN))) {
			bac=(bac==0)?4:bac-1;
			LCD_Clear();
			_delay_ms(250);
		}

		switch(bac) {
			case 0:
			LCD_Home();
			LCD_String("Chinh Thoi Gian");
			LCD_Goto(7,1);
			LCD_String("OK");
			if((bit_is_clear(PINC,OK))) {
				//call function changetime
				_delay_ms(250);
				if((bit_is_clear(PINC,OK))) {
					counts++;
				}
				if(counts==0) {
					LCD_Cmd(0x0f);
					change_time();
					LCD_Cmd(0x0C);
					_delay_ms(250);
					} else {
					LCD_Clear();
					LCD_String("Cho xiu...");
					_delay_ms(2000);
					return 0;
				}

			}
			break;
			case 1:
			LCD_Home();
			LCD_String("Chinh Anh Sang");
			LCD_Goto(7,1);
			LCD_String("OK");
			if((bit_is_clear(PINC,OK))) {
				_delay_ms(250);
				//call function changelights
				if((bit_is_clear(PINC,OK))) {
					counts++;
				}
				if(counts==0) {
					change_light();
					_delay_ms(250);
					} else {
					LCD_Clear();
					LCD_String("Cho xiu...");
					_delay_ms(2000);
					return 0;
				}
			}
			break;
			case 2:
			LCD_Home();
			LCD_String("Chinh Thiet Bi");
			LCD_Goto(7,1);
			LCD_String("OK");
			if((bit_is_clear(PINC,OK))) {
				_delay_ms(250);
				//call function changedevice
				if((bit_is_clear(PINC,OK))) {
					counts++;
				}
				if(counts==0) {
					_delay_ms(250);
					LCD_Cmd(0x0f);
					change_device();
					LCD_Cmd(0x0C);
					_delay_ms(250);
					} else {
					LCD_Clear();
					LCD_String("Cho xiu...");
					_delay_ms(2000);
					counts=0;
					return 0;
				}
			}
			break;
			case 3:
			LCD_Home();
			LCD_String("Chon che do");
			LCD_Goto(7,1);
			LCD_String("OK");
			if((bit_is_clear(PINC,OK))) {
				_delay_ms(250);
				//call function changedevice
				if((bit_is_clear(PINC,OK))) {
					counts++;
				}
				if(counts==0) {
					_delay_ms(250);
					change_mode();
					_delay_ms(250);
					} else {
					LCD_Clear();
					LCD_String("Cho xiu...");
					_delay_ms(2000);
					counts=0;
					return 0;
				}
			}
			break;
			case 4:
			LCD_Home();
			LCD_String("Thong tin t/g   ");
			LCD_Goto(7,1);
			LCD_String("OK");
			if((bit_is_clear(PINC,OK))) {
				_delay_ms(250);
				//call function changedevice
				if((bit_is_clear(PINC,OK))) {
					counts++;
				}
				if(counts==0) {
					LCD_Home();
					_delay_ms(250);
					if(LCD_blink("San pham phat trien boi AnhTuan, san pham voi chuc nang bat tat thiet bi theo cuong do anh sang va thoi gian giup linh hoat trong qua trinh bat tat thiet bi.")) {
						goto break_;
					}
					_delay_ms(250);
					} else {
					LCD_Clear();
					LCD_String("Cho xiu...");
					_delay_ms(2000);
					counts=0;
					return 0;
				}
			}
			break_:
			break;
			default:
			break;
		}
		count++;
	}
	return 0;
}
//----------------------------------------------------------------------------
unsigned char get_key() {
	char count = 0;
	while(1) {
		if(count>1000) {
			LCD_Clear();
			return '1';
		}
		if((bit_is_clear(PINC,OK))) {
			_delay_ms(350);
			if((bit_is_clear(PINC,OK))) {
				LCD_Clear();
				LCD_String("Doi xiu...");
				_delay_ms(1500);
				LCD_Clear();
				return '1';
			}
			return 'o';
		}
		_delay_ms(20);
		if((bit_is_clear(PINC,UP))) {
			return 'u';
		}
		_delay_ms(20);
		if((bit_is_clear(PINC,DOWN))) {
			return 'd';
		}
		_delay_ms(20);
		count++;
	}
}
//----------------------------------------------------------------------------
void check_password() {
	unsigned char check[7],succsec[]= {'o','o','u','u','d','d','d','d'},ss,allow=0;
	top:
	ss=1;
	if(allow>=5) {
		unsigned int count = 60;
		LCD_Clear();
		LCD_String("Qua ");
		LCD_Integer(allow);
		LCD_String(" lan!");
		LCD_Line(1);
		LCD_String("Cho trong:");
		if(allow==10) {
			count = 600;
		}
		if(allow>5&&allow<10) {
			goto thoat;
		}
		while(count--) {
			LCD_Goto(10,1);
			if(count<100) {
				LCD_Char(' ');
			}
			LCD_Integer(count);
			LCD_String("(s)");
			_delay_ms(1000);
		}
		if(allow==10) {
			allow=0;
		}
		thoat:
		_delay_ms(10);
	}
	LCD_Clear();
	LCD_String("Enter password:");
	lcd_custom_char(0, heart);
	for (int i = 0; i < 8; i++) {
		LCD_Goto(5 + i,1);
		LCD_Char(0x00);
	}
	for(int j = 0 ; j<8; j++) {
		check[j]=get_key();
		if(check[j]=='1') {
			goto back_1;
		}
		if(check[j]!=succsec[j]) {
			ss = 0;
		}
		LCD_Goto(5 + j,1);
		LCD_Char('*');
		_delay_ms(250);
	}
	if(!ss) {
		LCD_Clear();
		LCD_String("Sai mat khau!");
		_delay_ms(1500);
		allow++;
		goto top;
	}
	LCD_Clear();
	LCD_String("Dung mat khau!");
	_delay_ms(1500);
	menu();
	back_1:
	_delay_ms(20);

}
//----------------------------------------------------------------------------
void MainLoop() {
	unsigned int temp,ADC_light,hours_,min_,hour_real,min_real,x,temp_sum,temp_sum_real,mode__,check__;
	byte hours,min,sec;
	read_info(&hours_,&min_);
	read_mode(&mode__);
	read_check(&check__);
	while(1) {
		read_light(&ADC_light);
		DS1307_GetTime(&hours,&min,&sec);
		hour_real=TwoDigits(hours);
		min_real=TwoDigits(min);
		temp=read_adc_channel();
		if((bit_is_clear(PINC,OK))) {
			_delay_ms(300);
			check_password();
			read_info(&hours_,&min_);
			read_mode(&mode__);
		}
		LCD_Home();
		temp_sum = hours_*60+min_;
		temp_sum_real = hour_real*60+min_real;
		if(hours_*60+min_<45) {
			hours_=24;
		}
		if(mode__==1) {
			read_check(&check__);
			if(hours_*60+min_<=0) {
				hours_=24;
			}
			if(hours_>=12) {
				x=-12;
				} else {
				x=12;
			}
			//====================================================================================================
			//truong hop mat dien ma qua thoi gian bat tat den thi khong dung cam bien dung thoi gian de dieu khien
			if(x==-12) { //truong hop thoi gian lon hon 12h
				//truong hop tat thiet bi hay bat den
				if((temp_sum+60)<=temp_sum_real||(temp_sum-(13*60))>=temp_sum_real) { // vi du hours=17h thi thiet bi se tat(bat den) tu [17,05]
					if(check__) {
						check__=0;

						eeprom_write_word(&check_,check__);
					}
				}
				if((temp_sum-60)>=temp_sum_real&&(temp_sum-(11*60))<=temp_sum_real) { // vi du hours=17h thi thoi gian bat thiet bi(tat den) la tu (05,17)
					if(!check__) {
						check__=1;
						eeprom_write_word(&check_,check__);
					}
				}
				//truong hop bat thiet bi hay tat den
				} else if(x==12) { // truong hop thoi gian be hon 12h
				if((temp_sum+60)<=temp_sum_real&&(temp_sum+(11*60))>=temp_sum_real) { //vi du hours=5 thi thiet bi se tat(bat den) tu [5,17]
					if(check__) {
						check__=0;
						eeprom_write_word(&check_,check__);
					}
				}
				if((temp_sum+(13*60))<=temp_sum_real||(temp_sum-60)>=temp_sum_real) { //vi du hours=5 thi thiet bi se bat(den tat) tu (17,5)
					if(!check__) {
						check__=1;
						eeprom_write_word(&check_,check__);
					}
				}
			}
			//====================================================================================================
			//truong hop dung ket hop giua anh sanh va thoi gian
			if((((temp_sum-45)<temp_sum_real)&&(temp_sum+45)>temp_sum_real)&&(ADC_light+50<=temp)) { // vi du hours = 17h thiet bi se bat(den tat) tu [17h-45',17h+45'] va anh sang phai be hon anh sang mac dinh
				if(check__) {
					check__=0;
					eeprom_write_word(&check_,check__);
				}
			}
			if(hours_>=12) {
				x=-12;
				} else {
				x=12;
			}
			temp_sum = (hours_+x)*60+min_;
			if((((temp_sum-45)<temp_sum_real)&&(temp_sum+45)>temp_sum_real)&&(ADC_light-50>temp)) { // vi du hours = 17h thi thiet bi se bat(den tat) tu [5h-45',5h+45'] va anh sang phai lon hon anh sang mac dinh
				if(!check__) {
					check__=1;
					eeprom_write_word(&check_,check__);
				}
			}
		}
		//====================================================================================================
		else {
			if(hours_*60+min_<=0) {
				hours_=24;
			}
			if(hours_>=12) {
				x=-12;
				} else {
				x=12;
			}
			if(x==-12) { //truong hop thoi gian lon hon 12h
				//truong hop tat thiet bi hay bat den
				if(temp_sum>temp_sum_real&&temp_sum-(12*60)<=temp_sum_real) { // vi du hours=17h thi thoi gian bat thiet bi(tat den) la tu (05,17)
					if(!check__) {
						check__=1;
						eeprom_write_word(&check_,check__);
					}
				}
				if(temp_sum<=temp_sum_real||temp_sum-(12*60)>temp_sum_real) { // vi du hours=17h thi thiet bi se tat(bat den) tu [17,05]
					if(check__) {
						check__=0;
						eeprom_write_word(&check_,check__);
					}
				}
				//truong hop bat thiet bi hay tat den
				} else if(x==12) { // truong hop thoi gian be hon 12h
				if(temp_sum+(12*60)<=temp_sum_real||temp_sum>temp_sum_real) { //vi du hours=5 thi thiet bi se bat(den tat) tu (17,5)
					if(!check__) {
						check__=1;
						eeprom_write_word(&check_,check__);
					}
				}
				if(temp_sum<=temp_sum_real&&temp_sum+(12*60)>temp_sum_real) { //vi du hours=5 thi thiet bi se tat(bat den) tu [5,17]
					if(check__) {
						check__=0;
						eeprom_write_word(&check_,check__);
					}
				}
			}

		}
		//======================================================================================================================
		if(check__) {
			SetBit(PORTD,RELAY);
			} else {
			ClearBit(PORTD,RELAY);
		}
		LCD_String("AS:");
		if(temp<1000) {
			LCD_String(" ");
		}
		LCD_Integer(temp);
		if(temp<100) {
			LCD_String(" ");
		}
		LCD_String("|");////
		LCD_TimeDate(); // put time & date on LCD
	}
}
// ---------------------------------------------------------------------------
// MAIN PROGRAM
int main(void)
{
	InitAVR(); // set port direction
	LCD_Init(); // initialize HD44780 LCD controller
	I2C_Init(); // set I2C clock frequency
	adc_init();
	LCD_String("Ready...");
	ShowDevices(); // show that I2C is working OK
	LCD_Clear();
	MainLoop(); // display time

}