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
#include "string.h"
#include "stdio.h"
#include "interrupt.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern double frq;
extern struct keys key[4];
extern int x,y;
extern int HZ_sta;
extern int q,w,MH,R;
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
double getADC(ADC_HandleTypeDef *hadc)
{
	unsigned int value = 0;
	HAL_ADC_Start(hadc);
	value = HAL_ADC_GetValue(hadc);
	return value*3.3/4096;
}
void LED_Disp(int dsLED)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_All,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,dsLED<<8,GPIO_PIN_RESET);//
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}
int B4_DATA()
{
	static int i=1;
	if(get_key_stat(3)==3)
	{
		set_key_stat(3,0);
		i=0;
	}
	if(get_key_stat(3)==1)
	{
		set_key_stat(3,0);
		i=1;
	}
	return i;
}
int Duty()
{
	double value=getADC(&hadc2);
	static int duty;
	if(B4_DATA())
	{
		if(value<=1)		{duty=10;}
		else if(value>=3)	{duty=85;}
		else
		{
			duty=37.5*value-27.5;
		}
	}
	return duty;
}

char  B2_DATA()
{
	if(get_key_stat(1))
	{
		set_key_stat(1,0);
		if(!x)
		{
			HZ_sta++;
			x=1;
		}
	}
	if(HZ_sta%2){	return 'H';}
	else   		{	return 'L';}
}
int B2_PARA()
{
	static int i=0;
	if(get_key_stat(1))
	{
		set_key_stat(1,0);
		i++;
	}
	if(i%2)return 0;
	else   return 1;
}

int K=1;

void B3_PARA()
{
	if(B2_PARA())
	{
		if(get_key_stat(2))
		{
			set_key_stat(2,0);
			if(R<10){R++;q=1;w=0;}
			else R=1;
			
		}
		else if(get_key_stat(3))
		{
			set_key_stat(3,0);
			if(R>1){R--;}
			else{R=10;}
		}
	}
	else
	{
		if(get_key_stat(2))
		{
			set_key_stat(2,0);
			if(K<10){K++;}
			else K=1;
			
		}
		else if(get_key_stat(3))
		{
			set_key_stat(3,0);
			if(K>1){K--;}
			else{K=10;}
		}
	}
}

void DATA()
{
	unsigned char str[15];
	char a;
	LCD_DisplayStringLine(Line1,(u8 *)"        DATA");
	B2_DATA();
	sprintf((char *)str,"     M=%c    ",B2_DATA());
	LCD_DisplayStringLine(Line3,str);
	sprintf((char *)str,"     P=%d%%   ",Duty());
	LCD_DisplayStringLine(Line4,str);
	double v=frq*R* 6.28/100/K;
	sprintf((char *)str,"     V=%.1f     ",v);
	LCD_DisplayStringLine(Line5,str);
}
void PARA()
{
	unsigned char str[15];
	int s=1;
	LCD_DisplayStringLine(Line1,(u8 *)"        PARA");
	B3_PARA();
	sprintf((char *)str,"     R=%d  ",R);
	LCD_DisplayStringLine(Line3,str);
	sprintf((char *)str,"     K=%d  ",K);
	LCD_DisplayStringLine(Line4,str);
}
void RECD()
{
	unsigned char str[15];
	int s=1;
	LCD_DisplayStringLine(Line1,(u8 *)"        RECD");
	sprintf((char *)str,"     N=%d",HZ_sta);
	LCD_DisplayStringLine(Line3,str);
	int v1=frq*MH* 6.28/100/K;
	sprintf((char *)str,"     MH=%d",v1);
	LCD_DisplayStringLine(Line4,str);
	int v2=frq*MH* 6.28/100/K;
	sprintf((char *)str,"     ML=%d",v2);
	LCD_DisplayStringLine(Line5,str);
}
int Sreen()
{
	static int sreen=0;
	if(get_key_stat(0)==1)
	{
		LCD_Clear(Black);
		set_key_stat(0,0);
		sreen++;
		sreen%=3;
	}
	return sreen;
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
  MX_ADC2_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	LCD_Init();
  	LCD_Clear(Black);
	LCD_SetTextColor(White);
	LCD_SetBackColor(Black);
	HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_2);
	LED_Disp(0x00);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  switch(Sreen())
	  {
		  case 0:
			  DATA();break;
		  case 1:
			  PARA();break;
		  case 2:
			  RECD();break;
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
