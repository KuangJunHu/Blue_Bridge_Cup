#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "interrupt.h"
#include "tim.h"
#include "lcd.h"
#include "usart.h"

struct keys key[4]={0};
struct Some_State LED={0,0,0};
struct Input_Capure TM2={0,0};
unsigned char  rx[20];
unsigned char RX_Buff;
int idx=0;

void key_Event_Handler(bool ,bool ,bool );
//0x01<<(LED_Value-1)


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM1)
    {
		key_Event_Handler(0,0,1);
		if(LED.sta)
		{
			LED.tim++;
			if(LED.tim>=100)
			{
				LED.tim=0;
				LED.sta_re++;
			}
		}
		HAL_TIM_Base_Start_IT(htim);
	}
}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM1)
	{
		static int ch1=0,ch2=0;
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
		{
			ch1 = HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1)+2;
			TM2.frq = (80000000.0/10)/ch1;
			TM2.duty = 100.0*ch2/ch1;
		}
		else if(htim ->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			ch2 = HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2)+2;
		}
	}
}
/*void LCD_Light_Back(uint8_t *str,uint8_t start,uint8_t end,uint8_t Line)
{
	for(int i = 0;i<=strlen((char *)str)-1;i++)
	{
		if(i<start || i>end)
		{
			LCD_DisplayChar(Line,(320 - (16 * i)),str[i]);
		}
	}
	LCD_SetBackColor(Green);
	for(;end+1>start;start++)
	{
		LCD_DisplayChar(Line,(320 - (16 * start)),str[start]);
	}
	LCD_SetBackColor(Black);	
}

void LCD_Text_Back(uint8_t *str,uint8_t start,uint8_t end,uint8_t Line)
{
	for(int i = 0;i<=strlen((char *)str)-1;i++)
	{
		if(i<start || i>end)
		{
			LCD_DisplayChar(Line,(320 - (16 * i)),str[i]);
		}
	}	
	LCD_SetTextColor(Red);
	for(;end+1>start;start++)
	{
		LCD_DisplayChar(Line,(320 - (16 * start)),str[start]);
	}
	LCD_SetTextColor(White);	
}*/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		rx[idx++]=RX_Buff;
		HAL_UART_Receive_IT(&huart1,&RX_Buff,1);
	}
}

double getADC(ADC_HandleTypeDef *hadc)
{
	unsigned int value = 0;
	HAL_ADC_Start(hadc);
	value = HAL_ADC_GetValue(hadc);
	return value*3.3/4096;
}

void LED_Disp(int LED_OFF)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_All,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,LED_OFF<<8,GPIO_PIN_RESET);//
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}
/**
	@brief 获取某个按键的LCD状态成员变量
	@param i 要获取的按键在key结构体数组中的索引
	@retval int类型，返回指定按键的LCD状态
*/
int get_key_stat(int i)
{
	return key[i].Lcd_sta;
}

/**
	@brief 设置某个按键的LCD状态成员变量
	@param i 要设置的按键在key结构体数组中的索引
	@param value 要设置的按键的LCD状态值
	@retval 无
*/
void set_key_stat(int i,bool value)
{
	key[i].Lcd_sta = value;
}


/**
	@brief 处理键盘事件，根据按键状态和延时时间判断事件类型和相应操作
	@param Long_press: bool类型，用于判断是否检测到长按事件
	@param Double_click: bool类型，用于判断是否检测到双击事件
	@param Single_click: bool类型，用于判断是否检测到单击事件
	@retval 无返回值 
*/ 
//解决了双击偶尔会出现的bug，和只设置长按，没有按到时间，而没有清零
void key_Event_Handler(bool Long_press,bool Double_click,bool Single_click)
{
	static int key_delaytim = 0;
	static int debounce = 1;
	
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
				key_delaytim++;
				if(Double_click)	{debounce = 2 ;}
				else 				{debounce = 1 ;}
				if(key_delaytim >= debounce)
				{
					key[i].judge_sta = 1; 
				}
			}
			break;
		case 1: 
			if (key[i].key_sta == GPIO_PIN_RESET) 
			{
				key_delaytim++;
				if(key_delaytim >= BUTTON_LONG_TIME && Long_press)
				{ 
					key[i].judge_sta = 2;
				}
			}
			else
			{
				key_delaytim = 0;
				key[i].judge_sta = 3; 
			}
			break;
		case 2:
			if(key[i].key_sta == GPIO_PIN_SET)
			{
				key_delaytim = 0;
				key[i].Lcd_sta = 3; 
				key[i].judge_sta = 0;
			}
			break;
		case 3:
			if(key[i].key_sta == GPIO_PIN_SET)
			{
				key_delaytim++;
				if(Double_click)
				{
					key[i].judge_sta = 4;
				}
				else if(Single_click)
				{
					key_delaytim = 0;
					key[i].Lcd_sta = 1;
					key[i].judge_sta = 0;
				}
				else
				{
					key_delaytim = 0;
					key[i].Lcd_sta = 0; 
					key[i].judge_sta = 0;
				}
			}
			break;
		case 4:
			if(key[i].key_sta == GPIO_PIN_SET)
			{
				key_delaytim++;
				if(Single_click)
				{
					if(key_delaytim >= BUTTON_DOUBLE_TIME)
					{
						key_delaytim = 0;
						key[i].Lcd_sta = 1;
						key[i].judge_sta = 0; 						
					} 
				}
				else if(key_delaytim >= BUTTON_DOUBLE_TIME)
				{
					key_delaytim = 0;
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
				key_delaytim = 0;
				key[i].Lcd_sta = 2;
				key[i].judge_sta = 0;
			}
			break;
		}
	}
}








