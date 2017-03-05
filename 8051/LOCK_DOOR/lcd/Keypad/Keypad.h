// ***********************************************************
// Project:   Keypad Interfacing with 8051
// Author:    M.Saeed Yasin
// Module description:  Get input from keypad and display it on LCD
// ***********************************************************
#include <AT89X52.H>
#include "delay/delay.h"
#define TRUE 1
#define FALSE 0

char READ_SWITCHES(void);
char get_key(void);

//********************
// Define Pins
//********************
sbit RowA = P1^0;     //RowA
sbit RowB = P1^1;     //RowB
sbit RowC = P1^2;     //RowC
sbit RowD = P1^3;     //RowD

sbit C1   = P1^4;     //Column1
sbit C2   = P1^5;     //Column2
sbit C3   = P1^6;     //Column3
sbit C4   = P1^7;     //Column4




