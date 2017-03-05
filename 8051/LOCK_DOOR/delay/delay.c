/**
  ******************************************************************************
  * Ten Tep		:		  delay.c
  * Tac Gia		:	  	Nguyen Quy Nhat
  * Cong Ty		:			MinhHaGroup
  *	Website 	:			BanLinhKien.Vn
  * Phien Ban	:			V1.0.0
  * Ngay			:    	31-07-2012
  * Tom Tat   :     Dinh nghia cac ham tao tre.
  *           
  *
  ******************************************************************************
  * Chu Y		:
  *							 
  ******************************************************************************
  */
#include "delay.h"
void delay_us(unsigned int time)
{
   unsigned int i;
   for(i=0;i<time;i++)
   {
      ;// khong lam gi ca
   }
}
//-----------void delay_ms(unsigned char x)------------

void delay_ms(unsigned time)
{
	unsigned int i,j;
	for(i=0;i<time;i++)
	for(j=0;j<125;j++)
	{
      ;// khong lam gi ca
	}
}

