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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "lcd.h"
#include "interrupt.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern unsigned char  rx[20];
extern unsigned char RX_Buff;
extern int idx;
extern struct Some_State LED;

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
float V_value[2]={3.0,1.0};
float V_value_old[2]={0,0};
int screen=1;
int led;

int led_2=1;
int led_value=0;
void Key()
{
	if(get_key_stat(0))
	{
		screen++;
		screen%=2;
		if(V_value[0]>=V_value[1]+0.99)
		{
			V_value_old[0]=V_value[0];
			V_value_old[1]=V_value[1];
			led&=0xfd;
		}
		else
		{
			V_value[0]=V_value_old[0];
			V_value[1]=V_value_old[1];	
			led|=0x02;
		}
		if(led_2)
		{
			led&=0xfd;
		}
		else
		{
			led|=0x02;
		}
	}
	if(!screen)
	{
		if(get_key_stat(1))
		{
			if(V_value[0]>=3.25)
			{
				V_value[0]=0.0;
			}
			else
			{
				V_value[0]+=0.1;
			}
		}
		else if(get_key_stat(2))
		{
			if(V_value[1]>=3.25)
			{
				V_value[1]=0.0;
			}
			else
			{
				V_value[1]+=0.1;
			}
		}
		if(V_value[0]>=V_value[1]+0.99)
		{
			led_2=1;
		}
		else
		{
			led_2=0;
		}
	}
	set_key_stat(0,0);
	set_key_stat(1,0);
	set_key_stat(2,0);
}
void adc_tim()
{
	float adc_value=getADC(&hadc2);
	if(adc_value>=V_value[1] && adc_value<=V_value[0])
	{
		LED.sta=1;
		led|=0x01;
	}
	else
	{
		led&=0xfe;
		LED.sta=0;
		LED.tim=0;
		LED.sta_re=0;
	}
	LED_Disp(led);
}
void Usb()
{
	if(idx)
	{
		HAL_Delay(20);
		if(idx==9)
		{
			V_value_old[0]=V_value[0];
			V_value_old[1]=V_value[1];
			sscanf((char *)rx,"\"%f,%f\"",&V_value[0],&V_value[1]);
			if((V_value[0]>=V_value[1]+0.99) && (rx[0]=='"')&&(rx[8]=='"'))
			{
				led&=0xf7;
			}
			else
			{
				V_value[0]=V_value_old[0];
				V_value[1]=V_value_old[1];
				led|=0x08;
			}
			
		}
		else
		{
			led|=0x08;
		}
		idx=0;
		memset(rx,0,strlen(rx));
	}
}
void Data()
{
	//int a=0;
	unsigned char str[20];
	float adc_value=getADC(&hadc2);
	LCD_DisplayStringLine(Line0,(u8 *)"      Data  ");
	sprintf((char *)str," V:%.2fV    ",adc_value);
	LCD_DisplayStringLine(Line2,str);
	sprintf((char *)str," T:%ds      ",LED.sta_re);
	LCD_DisplayStringLine(Line3,str);
}

void Para()
{
	unsigned char str[20];
	LCD_DisplayStringLine(Line0,(u8 *)"      Para  ");
	sprintf((char *)str," Vmax:%.1f   ",V_value[0]);
	LCD_DisplayStringLine(Line2,str);
	sprintf((char *)str," Vmin:%.1f   ",V_value[1]);
	LCD_DisplayStringLine(Line3,str);
	
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
  MX_ADC2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	LED_Disp(0x00);
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetTextColor(White);
	LCD_SetBackColor(Black);
	
	HAL_UART_Receive_IT(&huart1,&RX_Buff,1);
	
	HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);
	
	HAL_TIM_Base_Start_IT(&htim1);
	//HAL_TIM_IC_Start_IT(&htim1,TIM_CHANNEL_1);
	//HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  Usb();
	  adc_tim();
	  Key();
	  if(screen){Data();}
	  else		{Para();}
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
