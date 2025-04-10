#ifndef _USER_H_
#define _USER_H_

#include "main.h"
#include "stdbool.h"

#define IC_R37 hadc2
#define IC_R38 hadc1
#define IC_R39 htim3
#define IC_R40 htim2
                  	
#define BUTTON_DOUBLE_TIME	(50)	
#define BUTTON_LONG_TIME  	(100)              		

typedef struct 
{
	unsigned char judge_sta;	
	bool key_sta;			
	unsigned char Lcd_sta; 	
	int key_delaytim;
}keys;

typedef struct
{
	unsigned char  data[100];
	int data_size;
	int data_flag;
}uart;


int get_key_stat(int );
void set_key_stat(int ,bool);

double Read_ADC(ADC_HandleTypeDef *adc);
int Read_IC(TIM_HandleTypeDef *time);

void Set_FreCycle(int FreCycle);
void Set_DutyCycle(int dutycycle);
//RES
void write_resistor(uint8_t value);
uint8_t read_resistor(void);

//EE
uint8_t x24c02_read(uint8_t address);
void x24c02_write(unsigned char address,unsigned char info);



#endif
