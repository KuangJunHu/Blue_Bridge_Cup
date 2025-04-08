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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "lcd.h"
#include "interrupt.h"
#include "MY_IIC.h"
#include "i2c_hal.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern unsigned char  rx[10];
extern unsigned char RX_Buff;
extern int idx;
extern struct Some_State LED;
extern struct Some_State LED1;
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
int Sreen=0;
int Store_num[2]={0,0};
float Store_Price[2]={1.0,1.0};
int Store_all[2]={10,10};
unsigned char Buy[20];
void Key()
{
	if(get_key_stat(0))
	{
		Sreen++;
		Sreen%=3;
	}
	if(Sreen==0)
	{
		if(get_key_stat(1))
		{
			Store_num[0]++;
			if(Store_num[0]>=Store_all[0])
			{
				Store_num[0]=Store_all[0];
			}
		}
		else if(get_key_stat(2))
		{
			Store_num[1]++;
			if(Store_num[1]>=Store_all[1])
			{
				Store_num[1]=Store_all[1];
			}
		}
		else if(get_key_stat(3))
		{
			LED.tim=0;
			LED.sta=1;
			float all_money=Store_num[0]*Store_Price[0]+\
							Store_num[0]*Store_Price[0];
			sprintf((char *)Buy,"X:%d,Y:%d,Z:%.1f",Store_num[0],\
					Store_num[1],all_money);
			HAL_UART_Transmit(&huart1,Buy,strlen(Buy),0xffff);
			Store_all[0]-=Store_num[0];
			Store_all[1]-=Store_num[1];
			x24c02_write(0,Store_all[0]);HAL_Delay(5);
			x24c02_write(1,Store_all[1]);HAL_Delay(5);
			Store_num[0]=Store_num[1]=0;
		}
	}
	else if(Sreen == 1)
	{
		if(get_key_stat(1))
		{
			if(Store_Price[0]>=1.95)
			{
				Store_Price[0]=1.0;
			}
			else
			{
				Store_Price[0]+=0.1;
			}
			x24c02_write(3,(int)(Store_Price[1]*10));
		}
		else if(get_key_stat(2))
		{
			if(Store_Price[1]>=1.95)
			{
				Store_Price[1]=1.0;
			}
			else
			{
				Store_Price[1]+=0.1;
			}
			x24c02_write(3,(int)(Store_Price[1]*10));
		}
	}
	else if(Sreen == 2)
	{
		if(get_key_stat(1))
		{
			Store_all[0]++;
			x24c02_write(0,Store_all[0]);
		}
		else if(get_key_stat(2))
		{
			Store_all[1]++;
			x24c02_write(1,Store_all[1]);
		}
	}
	set_key_stat(0,0);
	set_key_stat(1,0);
	set_key_stat(2,0);
	set_key_stat(3,0);
}
void Usb()
{
	if(idx!=0)
	{
		if(idx==1 && rx[0]=='?')
		{
			sprintf((char *)Buy,"X:%.1f,Y:%.1f",Store_Price[0],Store_Price[1]);
			HAL_UART_Transmit(&huart1,Buy,strlen(Buy),0xffff);
		}
		idx=0;
		memset(rx,0,strlen(rx));
	}
}
void LED_6()
{
	int led=0x00;
	if(LED.sta)
	{
		led|=0x01;
		LED_Disp(led);
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,30);
	}
	else if(LED.sta_re)
	{
		led=0x00;
		LED_Disp(led);
		LED.sta_re=0;
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,5);
	}

	if(Store_all[0]==0 && Store_all[1]==0)
	{
		LED1.sta=1;
	}
	else
	{
		LED1.tim=0;
		LED1.sta_re=0;
		LED1.sta=0;
		LED_Disp(0x00);
	}
	if(LED1.sta_re)
	{
		led|=0x02;
		LED_Disp(led);
	}
	else
	{
		led|=0x00;
		LED_Disp(led);
	}
}
void Shop()
{
	unsigned char str[20];
	LCD_DisplayStringLine(Line1,(u8 *)"        SHOP   ");
	sprintf((char *)str,"     X:%d   ",Store_num[0]);
	LCD_DisplayStringLine(Line3,str);
	sprintf((char *)str,"     Y:%d   ",Store_num[1]);
	LCD_DisplayStringLine(Line4,str);
}
void Price()
{
	unsigned char str[20];
	LCD_DisplayStringLine(Line1,(u8 *)"        PRICE  ");
	sprintf((char *)str,"     X:%.1f   ",Store_Price[0]);
	LCD_DisplayStringLine(Line3,str);
	sprintf((char *)str,"     Y:%.1f   ",Store_Price[1]);
	LCD_DisplayStringLine(Line4,str);
}
void Rep()
{
	unsigned char str[20];
	LCD_DisplayStringLine(Line1,(u8 *)"        REP   ");
	sprintf((char *)str,"     X:%d     ",Store_all[0]);
	LCD_DisplayStringLine(Line3,str);
	sprintf((char *)str,"     Y:%d     ",Store_all[1]);
	LCD_DisplayStringLine(Line4,str);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	int val=0;
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
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	I2CInit();
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetTextColor(White);
	LCD_SetBackColor(Black);
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	HAL_UART_Receive_IT(&huart1,&RX_Buff,1);
	LED_Disp(0x00);
	val=x24c02_read(5);HAL_Delay(5);
	x24c02_write(5,++val);HAL_Delay(5);
	if(val==1)
	{
		x24c02_write(0,Store_all[0]);HAL_Delay(5);
		x24c02_write(1,Store_all[1]);HAL_Delay(5);
		x24c02_write(2,(int)(Store_Price[0]*10));HAL_Delay(5);
		x24c02_write(3,(int)(Store_Price[1]*10));HAL_Delay(5);
	}
	Store_all[0]=x24c02_read(0);HAL_Delay(5);
	Store_all[1]=x24c02_read(1);HAL_Delay(5);
	Store_Price[0]=x24c02_read(2)/10;HAL_Delay(5);
	Store_Price[1]=x24c02_read(3)/10;HAL_Delay(5);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  LED_6();
	  Usb();
	  Key();
	  switch(Sreen)
	  {
		  case 0:
			  Shop();break;
		  case 1:
			  Price();break;
		  case 2:
			  Rep();break;
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
