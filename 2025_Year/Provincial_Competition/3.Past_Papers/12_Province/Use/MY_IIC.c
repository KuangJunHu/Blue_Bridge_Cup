#include "i2c_hal.h"
#include "MY_IIC.h"
#include "main.h"


void write_resistor(uint8_t value)
{   
	I2CStart();
	I2CSendByte(0x5E);  
	I2CWaitAck();
	
	I2CSendByte(value);  
	I2CWaitAck();
	I2CStop();
}

uint8_t read_resistor(void)
{   
	uint8_t value; 
	I2CStart();
	I2CSendByte(0x5F);  
	I2CWaitAck();
	
	value = I2CReceiveByte();
	I2CSendNotAck();
	I2CStop();

	return value;
}

uint8_t x24c02_read(uint8_t address)
{
	unsigned char val;
	
	I2CStart(); 
	I2CSendByte(0xa0);
	I2CWaitAck(); 
	
	I2CSendByte(address);
	I2CWaitAck(); 
	
	I2CStart();
	I2CSendByte(0xa1); 
	I2CWaitAck();
	val = I2CReceiveByte(); 
	I2CWaitAck();
	I2CStop();
	
	return(val);
}

void x24c02_write(unsigned char address,unsigned char info)
{
	I2CStart(); 
	I2CSendByte(0xa0); 
	I2CWaitAck(); 
	
	I2CSendByte(address);	
	I2CWaitAck(); 
	I2CSendByte(info); 
	I2CWaitAck(); 
	I2CStop();
}