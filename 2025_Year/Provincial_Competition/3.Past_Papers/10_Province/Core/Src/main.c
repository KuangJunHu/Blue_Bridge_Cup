/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "interrupt.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "i2c_hal.h"
#include "MY_IIC.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char *Stats[3]={"Upper","Lower","Normal"};
double Max_V=2.4,Min_V=1.2;
int Upp_LED=1,Low_LED=2;
int sreen=0;
int select;
int select_adc;
void LED_Judge(int LED_Value)
{
	static int LED_Num;
	LED_Num=0x01<<(LED_Value-1);
	LED_Disp(LED_Num);
}
void ADC_Cap()
{
	if(getADC(&hadc2)>Max_V)
	{
		select_adc=0;
	}
	else if(getADC(&hadc2)<Min_V)
	{
		select_adc=1;
	}
	else
	{
		select_adc=2;
	}
	switch(select_adc)
	{
		case 0:
			LED_Judge(Upp_LED);
		break;
		case 1:
			LED_Judge(Low_LED);
		break;
		case 2:
			LED_Judge(0x00);
		break;
	}
}
void Key()
{
	if(get_key_stat(0))
	{
		sreen++;
		sreen%=2;
	}
	if(sreen)
	{
		if(get_key_stat(1))
		{
			select++;
			select%=4;
		}
		if(get_key_stat(2))
		{
			switch(select)
			{
				case 0:
				{
					if(Max_V>=3.2)
					{
						Max_V=3.3;
					}
					else
					{
						Max_V+=0.3;
					}
				}break;
				case 1:
				{
					if(Max_V-0.1>=Min_V)
					{
						Min_V+=0.3;
					}
					else
					{
						Min_V=Max_V;
					}
				}break;
				case 2:
				{
					if(Upp_LED>=8)
					{
						Upp_LED=8;
					}
					 else if((Upp_LED+1)!=Low_LED)
					{
						Upp_LED++;
					}
					else
					{
						Upp_LED+=2;
					}
					if(Upp_LED==9)
					{
						Upp_LED=7;
					}
				}break;
				case 3:
				{
					if(Low_LED>=8)
					{
						Low_LED=8;
					}
					 else if((Low_LED+1)!=Upp_LED)
					{
						Low_LED++;
					}
					else
					{
						Low_LED+=2;
					}
					if(Low_LED==9)
					{
						Low_LED=7;
					}
				}break;
			}
		}
		if(get_key_stat(3))
		{
			switch(select)
			{
				case 0:
				{
					if(Max_V<=Min_V+0.1)
					{
						Max_V=Min_V;
					}
					else
					{
						Max_V-=0.3;
					}
				}break;
				case 1:
				{
					if(0.1<=Min_V)
					{
						Min_V-=0.3;
					}
					else
					{
						Min_V=0.0;
					}
				}break;
				case 2:
				{
					if(Upp_LED<=1)
					{
						Upp_LED=1;
					}
					 else if((Upp_LED-1)!=Low_LED)
					{
						Upp_LED--;
					}
					else
					{
						Upp_LED-=2;
					}
					if(Upp_LED==-0)
					{
						Upp_LED=2;
					}
				}break;
				case 3:
				{
					if(Low_LED<=1)
					{
						Low_LED=1;
					}
					 else if((Low_LED-1)!=Upp_LED)
					{
						Low_LED--;
					}
					else
					{
						Low_LED-=2;
					}
					if(Low_LED==0)
					{
						Low_LED=2;
					}
				}break;
			}
		}
	}
	set_key_stat(0,0);
	set_key_stat(1,0);
	set_key_stat(2,0);
	set_key_stat(3,0);
}
void Main()
{
	unsigned char str[25];
	LCD_DisplayStringLine(Line1,(u8 *)"       Main    ");
	sprintf((char *)str,"   Volt:%.2fV        ",getADC(&hadc2));
	LCD_DisplayStringLine(Line3,str);
	sprintf((char *)str,"   Status:%s         ",Stats[select_adc]);
	LCD_DisplayStringLine(Line4,str);
	LCD_ClearLine(Line5);
	LCD_ClearLine(Line6);
	select=0;
}
void Setting()
{
	unsigned char str[25];
	LCD_DisplayStringLine(Line1,(u8 *)"       Setting ");
	if(select==0)
	{
		LCD_SetBackColor(Green);
		sprintf((char *)str,"   Max Volr:%.1fV    ",Max_V);
		LCD_DisplayStringLine(Line3,str);
		LCD_SetBackColor(Black);
	}
	else
	{
		sprintf((char *)str,"   Max Volr:%.1fV    ",Max_V);
		LCD_DisplayStringLine(Line3,str);
	}
	if(select==1)
	{
		LCD_SetBackColor(Green);
		sprintf((char *)str,"   Min Volr:%.1fV    ",Min_V);
		LCD_DisplayStringLine(Line4,str);
		LCD_SetBackColor(Black);
	}
	else
	{
		sprintf((char *)str,"   Min Volr:%.1fV    ",Min_V);
		LCD_DisplayStringLine(Line4,str);
	}
	if(select==2)
	{
		LCD_SetBackColor(Green);
		sprintf((char *)str,"   Upper:LD%d        ",Upp_LED);
		LCD_DisplayStringLine(Line5,str);
		LCD_SetBackColor(Black);
	}
	else
	{
		sprintf((char *)str,"   Upper:LD%d        ",Upp_LED);
		LCD_DisplayStringLine(Line5,str);
	}
	if(select==3)
	{
		LCD_SetBackColor(Green); 
		sprintf((char *)str,"   Lower:LD%d        ",Low_LED);
		LCD_DisplayStringLine(Line6,str);
		LCD_SetBackColor(Black);
	}
	else
	{
		sprintf((char *)str,"   Lower:LD%d        ",Low_LED);
		LCD_DisplayStringLine(Line6,str);
	}

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	unsigned char val=0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC2_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  I2CInit();
  LCD_Init();
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(Blue2);
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);
	LED_Disp(0x00);
	//x24c02_write(0,0);
	val=x24c02_read(0);
	x24c02_write(0, ++val);
	if(val==1)
	{
		x24c02_write(1,(char)Max_V);HAL_Delay(5);
		x24c02_write(2,(char)Min_V);HAL_Delay(5);
		x24c02_write(3,Upp_LED);HAL_Delay(5);
		x24c02_write(4,Low_LED);HAL_Delay(5);
	}
	Max_V=x24c02_read(1);HAL_Delay(5);
	Min_V=x24c02_read(2);HAL_Delay(5);
	Upp_LED=x24c02_read(3);HAL_Delay(5);
	Low_LED=x24c02_read(4);HAL_Delay(5);

	unsigned char buf[20];
		sprintf(buf, "      PWR CNTR:%d    ", val);
		LCD_DisplayStringLine(Line8, (uint8_t *)(buf));

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	x24c02_write(1,Max_V);HAL_Delay(5);
	x24c02_write(2,Min_V);HAL_Delay(5);
	x24c02_write(3,Upp_LED);HAL_Delay(5);
	x24c02_write(4,Low_LED);HAL_Delay(5);
	ADC_Cap();
	Key();
	  switch(sreen)
	  {
		  case 0:
			Main();break;
		  case 1:
			Setting();break;
	  }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
