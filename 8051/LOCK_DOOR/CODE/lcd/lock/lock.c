#include "lock.h"

 void reverse(unsigned char s[],unsigned int n)
 {
    unsigned char i, j;
     unsigned char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
		 if(n<=9){
		 s[i+1]=s[i];
		 s[i]='0';
		 }
 }


 /* itoa:  convert n to characters in s */
 void itoa(unsigned int n, unsigned char s[])
 {
     unsigned char i, sign;
 
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s,sign);
 }
void buzzers(unsigned char x){
	unsigned char i;
	for(i=0;i<x;i++){
	BUZZER=1;
		delay_ms(300);
	BUZZER=0;
	delay_ms(400);		
	}
}
void getinput(unsigned int len,unsigned char x){
	unsigned char i,key;
	i=0;
	while(len!=0){
		check:
		while(!(key=get_key()));
		BUZZER=1;
		delay_ms(100);
		BUZZER=0;
		if(key=='C'){
			i--;
			len++;
			LCD_Gotoxy(2,13-len);
			LCD_PutChar(0x00);
			goto check;
		}
		
		input[i]=key;
		LCD_Gotoxy(2,13-len);
		LCD_PutChar(key);
		if(x==0){
			delay_ms(100);
		LCD_Gotoxy(2,13-len);
			LCD_PutChar('*');
		}
		len--;
		i++;
	}
}
bit check(unsigned char *first, unsigned char *second, unsigned char len){
	unsigned char i=0;
	for(i=0;i<len;i++){
		if(first[i]!=second[i])
			return 0;
	}
	return 1;
}
void lcd_custom_char(unsigned char add, unsigned char *k)
{
	unsigned char i;
   // cg ram address starts from 0x40 upto next 64 bytes
     if(add<8)                                 //If valid address
	 {
         LCD_SendCommand(0x40+(add*8));               //Write to CGRAM address location
         for(i=0;i<8;i++)
            LCD_PutChar(k[i]);                   //Write the character pattern to CGRAM location
     }
	 LCD_SendCommand(0x80);                           //shift back to DDRAM location 0
}
void store_code(){
	unsigned char i;
	for(i=0;i<8;i++)
		pass[i]=input[i];
}
void newcode(){
	EEP_24CXX_Init();
if(pass){
	LCD_Clear();
	LCD_Puts("New Password:");
	getinput(8,1);
  EEP_24CXX_WriteS(0x04,input);
	LCD_Clear();
	LCD_PutChar(0x03);
	LCD_Puts(" Save Password");
}
}
void changecode(){
	unsigned char i;
	EEP_24CXX_Init();
	Retype:
LCD_Clear();
	LCD_Puts("Old Password:");
	for(i=0;i<8;i++){
	LCD_Gotoxy(2,5+i);
	LCD_PutChar(0x00);
	}
	getinput(8,1);
	if(check(input,"12345678",8)==1){
	LCD_Clear();
	LCD_Puts("New Password:");
	getinput(8,1);
	store_code();
	 EEP_24CXX_WriteS(0x04,pass);
	LCD_Clear();
	LCD_PutChar(0x03);
	LCD_Puts(" Save Password");
	}else{
	goto Retype;
	}
}

bit menu(){
	
	Home:
	LCD_Clear();
	LCD_Gotoxy(1,1);
	LCD_Puts("Th.Bi");
	LCD_Gotoxy(2,2);
	LCD_Puts("1");
		LCD_Gotoxy(1,7);
	LCD_Puts("Setting");
	LCD_Gotoxy(2,10);
	LCD_Puts("2");
	///
	switch(get_key()){
		case '1':
			LCD_Clear();
			LCD_Gotoxy(1,1);
			LCD_Puts("ON");
			LCD_Gotoxy(2,1);
			LCD_Puts("1");
			LCD_Gotoxy(1,8);
			LCD_Puts("OFF");
			LCD_Gotoxy(2,8);
			LCD_Puts("0");
			
			while(1){
			switch(get_key()){
				case '0':
					Tb_A=0;
					break;
				case '1':
					Tb_A=1;
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
			LCD_Clear();
			LCD_Puts("Change Password");
			LCD_Gotoxy(2,1);
			LCD_Puts("1");
		switch(get_key()){
			case '1':
				changecode();
				break;
			default:
				break;
		}
			break;
		case 'C':
			return 1;
		break;
		default:
			break;
	}
	return 0;
}