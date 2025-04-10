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
/**
 * @brief  获取指定按键的LCD显示状态
 *
 * 该函数根据传入的按键编号，从按键数组中获取对应按键的LCD显示状态。
 * 按键编号从1开始，函数内部会将编号减1以匹配数组索引。
 *
 * @param  i 要获取状态的按键编号，取值范围为1到按键总数。
 * @retval 对应按键的LCD显示状态，返回值为表示状态的整数。
 */
int get_key_stat(int i)
{
	return key[i-1].Lcd_sta;
}
/**
 * @brief  设置指定按键的LCD显示状态
 *
 * 该函数根据传入的按键编号和要设置的状态值，将对应按键的LCD显示状态进行更新。
 * 按键编号从1开始，函数内部会将编号减1以匹配数组索引。
 *
 * @param  i 要设置状态的按键编号，取值范围为1到按键总数。
 * @param  value 要设置的LCD显示状态值，为布尔类型。
 * @retval 无，该函数不返回值，直接更新按键的LCD显示状态。
 */
void set_key_stat(int i,bool value)
{
	key[i-1].Lcd_sta = value;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart ->Instance == USART1)
	{
		HAL_UARTEx_ReceiveToIdle_IT(&huart1,uart1.data,100);
		uart1.data_size = Size;
		uart1.data_flag = 1;
		
	}
}
/**
 * @brief  按键事件处理函数，用于对按键的长按、双击和单击事件进行检测与处理
 *
 * 该函数会遍历4个按键，依据传入的控制参数判断每个按键是否开启了长按、双击或单击功能，
 * 并根据按键的当前状态（按下、松开、长按等）更新按键的判断状态和LCD显示状态。
 *
 * @param  Long_press 一个整数，每一位对应一个按键，用于指示该按键是否开启长按功能。
 *         例如，0x01 表示 B1 开启长按，0x10 表示 B2 开启长按，0x11 表示 B1 和 B2 同时开启长按。
 * @param  Double_click 一个整数，每一位对应一个按键，用于指示该按键是否开启双击功能。
 * @param  Single_click 一个整数，每一位对应一个按键，用于指示该按键是否开启单击功能。
 *
 * @retval 无，该函数不返回值，直接对按键状态和LCD显示状态进行更新。
 */
void key_Event_Handler(int Long_press,int Double_click,int Single_click)
{
	for(int i=0;i<4;i++)
	{
		int key_all_flag =0x01;
		key_all_flag<<=i;
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
				if(key[i].key_delaytim >= BUTTON_LONG_TIME && (Long_press & key_all_flag))
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
				if(Double_click&key_all_flag)
				{
					key[i].judge_sta = 4;
				}
				else if(Single_click&key_all_flag)
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
				if(Single_click&key_all_flag)
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
			key_Event_Handler(0x1,0x1,0xf);
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





