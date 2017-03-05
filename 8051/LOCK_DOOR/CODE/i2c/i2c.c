#include "i2c.h"
void I2C_Delay(unsigned int time)
{
	while(time)
	time--;
}
void I2C_Init()
{
	SCL=1;
	I2C_Delay(I2C_DELAY);
	SDA=1;
	I2C_Delay(I2C_DELAY);
}
void I2C_Start()
{
	SDA=1; 
  	SCL=1; 
  	I2C_Delay(I2C_DELAY);
  	SDA=0; 
  	I2C_Delay(I2C_DELAY);
  	SCL=0;
  	I2C_Delay(I2C_DELAY);
}
void I2C_Stop()
{	
	SDA=0;
  	SCL=1;
  	I2C_Delay(I2C_DELAY);
  	SDA=1;
  	I2C_Delay(I2C_DELAY);
  	SCL=0;
  	I2C_Delay(I2C_DELAY);
}
unsigned char I2C_CheckAck(void)
{
	unsigned char ack=0;
  	SDA=1;
  	SCL=1;
  	I2C_Delay(I2C_DELAY/2);
  	ack=SDA;
  	I2C_Delay(I2C_DELAY/2);
  	SCL=0;
  	I2C_Delay(I2C_DELAY);
  	if(ack==1) return 0; 
  	return 1;
}
void I2C_Write(unsigned char Data)
{
unsigned char i;
  	for(i=0;i<8;i++)
    {
   		SDA=((Data<<i)&0x80);
		SCL=1;
		I2C_Delay(I2C_DELAY);
		SCL=0;
		I2C_Delay(I2C_DELAY);
	}
}
unsigned char I2C_Read(void)
{
  	unsigned char I2C_data=0,i,temp;
  	for(i=0;i<8;i++)
    {
    	SDA=1;    
      	SCL=1;    
      	I2C_Delay(120);
      	temp=SDA;
      	I2C_Delay(120);
      	SCL=0;
      	if(temp==1)
       	{
         	I2C_data=I2C_data<<1;
         	I2C_data=I2C_data|0x01;
       	}
      	else
        	I2C_data=I2C_data<<1;
    }
    return I2C_data;
}