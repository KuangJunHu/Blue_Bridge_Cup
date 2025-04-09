#include "User.h"
#include "i2c_hal.h"
#include "tim.h"
#include "usart.h"

keys key[4]={0};
uart uart1={'\0',0,0};
void Set_FreCycle(int FreCycle)
{
	float set_fre = 80000000/80.0/FreCycle;
	float duty,cnt,arr;
	cnt = __HAL_TIM_GetCompare(&htim17,TIM_CHANNEL_1);
	arr = __HAL_TIM_GetAutoreload(&htim17)+1;
	duty = cnt/arr*set_fre;
	__HAL_TIM_SetAutoreload(&htim17,set_fre-1);
	__HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,(int)duty);
}

void Set_DutyCycle(int dutycycle)
{
	int tim_arr = __HAL_TIM_GetAutoreload(&htim17)+1;
	float set_compare = tim_arr * dutycycle /100.0;
	__HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,(int)set_compare);
}

int Read_IC(TIM_HandleTypeDef *time)
{
	//htim2为R40  	htim3为R39
	int cap = __HAL_TIM_GetCompare(time,TIM_CHANNEL_1)+1;
	return 20000000/cap;
}
double Read_ADC(ADC_HandleTypeDef *adc)
{
	//adc1为R38  	adc2为R37
	HAL_ADC_Start(adc);
	double value = HAL_ADC_GetValue(adc) * 3.3 / 4095 ;
	HAL_ADC_Stop(adc);
	return value;
}
int get_key_stat(int i){return key[i].Lcd_sta;}
void set_key_stat(int i,bool value){key[i].Lcd_sta = value;}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart ->Instance == USART1)
	{
		HAL_UARTEx_ReceiveToIdle_IT(&huart1,uart1.data,100);
		uart1.data_size = Size;
		uart1.data_flag = 1;
		
	}
}
int time_cnt1 = 0,time_cnt2 = 0,time_cnt3 = 0;
void key_Event_Handler(bool Long_press,bool Double_click,bool Single_click)
{
	for(int i=0;i<4;i++)
	{
		key[0].key_sta = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
		key[1].key_sta = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
		key[2].key_sta = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
		key[3].key_sta = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
		switch (key[i].judge_sta)
		{
		case 0: 
			if (key[i].key_sta == GPIO_PIN_RESET)
			{
				key[i].key_delaytim++;
				key[i].judge_sta = 1; 
			}
			break;
		case 1: 
			key[i].key_delaytim++;
			if (key[i].key_sta == GPIO_PIN_RESET) 
			{
				if(key[i].key_delaytim >= BUTTON_LONG_TIME && Long_press)
				{ 
					key[i].judge_sta = 2;
				}
			}
			else
			{
				key[i].judge_sta = 3; 
			}
			break;
		case 2:
			if(key[i].key_sta == GPIO_PIN_SET)
			{
				key[i].key_delaytim = 0;
				key[i].Lcd_sta = 3; 
				key[i].judge_sta = 0;
			}
			break;
		case 3:
			if(key[i].key_sta == GPIO_PIN_SET)
			{
				key[i].key_delaytim++;
				if(Double_click)
				{
					key[i].judge_sta = 4;
				}
				else if(Single_click)
				{
					key[i].key_delaytim = 0;
					key[i].Lcd_sta = 1;
					key[i].judge_sta = 0;
				}
				else
				{
					key[i].key_delaytim = 0;
					key[i].Lcd_sta = 0; 
					key[i].judge_sta = 0;
				}
			}
			break;
		case 4:
			if(key[i].key_sta == GPIO_PIN_SET)
			{
				key[i].key_delaytim++;
				if(Single_click)
				{
					if(key[i].key_delaytim >= BUTTON_DOUBLE_TIME)
					{
						key[i].key_delaytim = 0;
						key[i].Lcd_sta = 1;
						key[i].judge_sta = 0; 						
					} 
				}
				else if(key[i].key_delaytim >= BUTTON_DOUBLE_TIME)
				{
					key[i].key_delaytim = 0;
					key[i].judge_sta = 0; 						
				}
			}
			else
			{
				key[i].judge_sta = 5; 
			}
			break;
		case 5:
			if(key[i].key_sta == GPIO_PIN_SET)
			{
				key[i].key_delaytim = 0;
				key[i].Lcd_sta = 2;
				key[i].judge_sta = 0;
			}
			break;
		}
	}
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)
	{
			key_Event_Handler(1,1,1);
			HAL_TIM_Base_Start_IT(&htim6);
	}
}

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





