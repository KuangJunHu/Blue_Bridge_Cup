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
#include "lcd.h"
#include "interrupt.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern unsigned char rx[10];
extern unsigned char RX_Buff;
extern int idx;
extern int sta;
extern int error_sta;
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
char mima[3]={'@','@','@'};
char mima_true[3]={'1','2','3'};
int mima_sta[3]={1,1,1};
int screen=1;
int error=0;
void rx_mima()
{
	if(idx!=0)
	{
		if(memcmp(rx,mima_true,3)==0 && rx[3]=='-')
		{
			memcpy(mima_true,rx+4,4);
			LED_Disp(0x01);
		}
		idx=0;
		memset(rx,0,sizeof(rx));
	}
}
void KEY()
{
	if(get_key_stat(0) && mima_sta[0])
	{
		mima_sta[0]=0;
		mima[0]='0';
		set_key_stat(0,0);
	}
	else if(get_key_stat(0))
	{
		set_key_stat(0,0);
		if(mima[0]<'9'){mima[0]+=1 ;}
		else		   {mima[0]='0';}

	}
	if(get_key_stat(1) && mima_sta[1])
	{
		mima_sta[1]=0;
		mima[1]='0';
		set_key_stat(1,0);
	}
	else if(get_key_stat(1))
	{
		set_key_stat(1,0);
		if(mima[1]<'9'){mima[1]+=1 ;}
		else		   {mima[1]='0';}

	}
	if(get_key_stat(2) && mima_sta[2])
	{
		mima_sta[2]=0;
		mima[2]='0';
		set_key_stat(2,0);
	}
	else if(get_key_stat(2))
	{
		set_key_stat(2,0);
		if(mima[2]<'9'){mima[2]+=1 ;}
		else		   {mima[2]='0';}
	}
	if(get_key_stat(3))
	{
		set_key_stat(3,0);
		if(memcmp(mima_true,mima,3)==0)
		{	
			error=0;
			screen=0;
			sta=1;
			LED_Disp(0x01);
			mima_sta[0]=1;mima_sta[1]=1;mima_sta[1]=1;
			__HAL_TIM_PRESCALER(&htim2,39);
			__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,100);
		}
		else
		{
			error++;
			error_sta=1;
			memcpy(mima,"@@@",3);
			mima_sta[0]=1;mima_sta[1]=1;mima_sta[1]=1;
		}
	}
}
void PSD()
{
	unsigned char str[20];
	LCD_DisplayStringLine(Line1,(u8 *)"       PSD ");
	sprintf((char *)str,"    B1:%c   ",mima[0]);
	LCD_DisplayStringLine(Line3,str);
	sprintf((char *)str,"    B2:%c   ",mima[1]);
	LCD_DisplayStringLine(Line4,str);	
	sprintf((char *)str,"    B3:%c   ",mima[2]);
	LCD_DisplayStringLine(Line5,str);
}
void STA()
{
	int frq=2000;
	int duty=10;
	unsigned char str[20];
	LCD_DisplayStringLine(Line1,(u8 *)"       STA ");
	sprintf((char *)str,"    F:%d   ",frq);
	LCD_DisplayStringLine(Line3,str);
	sprintf((char *)str,"    D:%d%%    ",duty);
	LCD_DisplayStringLine(Line4,str);
	LCD_ClearLine(Line5);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetTextColor(White);
	LCD_SetBackColor(Black);
	HAL_UART_Receive_IT(&huart1,&RX_Buff,1);
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	LED_Disp(0x00);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  rx_mima();
	  KEY();
	  HAL_UART_Transmit(&huart1,mima_true,sizeof(mima_true),0xffff);
	  switch(screen)
	  {
		  case 0:
			STA();break;
		  case 1:
			PSD();break;
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
