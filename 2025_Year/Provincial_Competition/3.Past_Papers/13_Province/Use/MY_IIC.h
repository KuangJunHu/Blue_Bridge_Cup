#ifndef _MY_IIC_H_
#define _MY_IIC_H_
#include "main.h"

//RES
void write_resistor(uint8_t value);
uint8_t read_resistor(void);

//EE
uint8_t x24c02_read(uint8_t address);
void x24c02_write(unsigned char address,unsigned char info);

#endif
