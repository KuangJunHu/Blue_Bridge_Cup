/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "i2c_hal.h"
#include "User.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
extern uart uart1;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
RTC_DateTypeDef Date;
RTC_TimeTypeDef Time;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char key_sta[20];
int LED = 0xff;
double A_Fre,A_T,B_Fre,B_T;
int PD = 1000,PH = 5000,PX = 0;
int NDA = 0,NDB = 0,NHA,NHB;
int flag_NDA = 0,flag_NDB = 0;
void Read_Fre()
{
	A_Fre = Read_IC(&IC_R40) + PX ;
	if(A_Fre<=400)			{A_Fre = 400;}
	else if(A_Fre>=20000)	{A_Fre = 20000;}
	B_Fre = Read_IC(&IC_R39) + PX;
	if(B_Fre<=400)			{B_Fre = 400;}
	else if(B_Fre>=20000)	{B_Fre = 20000;}
	A_T = 1000000/A_Fre;
	B_T = 1000000/B_Fre;
	if(A_Fre > PH && flag_NDA == 0){LED |= 0xfd;NDA++;flag_NDA =1 ;}
	else if	(A_Fre < PH)			{LED &= 0x02;flag_NDA =0;}
	if(B_Fre > PH && flag_NDB == 0){LED |= 0xfb;NDB++;flag_NDB =1 ;}
	else if (B_Fre < PH)			{LED &= 0x04;flag_NDB =0;}
}
void LCD_Data_Fre()
{
	char str[20];
	sprintf(str,"        DATA  ");
	LCD_DisplayStringLine(Line1,(u8 *)str);
	if(A_Fre >= 1000)	{sprintf(str,"     A=%.2fKHz    ",A_Fre/1000);}
	else if(A_Fre < 0)	{sprintf(str,"     A=NULL      ");}
	else 				{sprintf(str,"     A=%.0fHz    ",A_Fre);}
	LCD_DisplayStringLine(Line3,(u8 *)str);
	if(B_Fre >= 1000)	{sprintf(str,"     B=%.2fKHz    ",B_Fre/1000);}
	else if(B_Fre < 0)	{sprintf(str,"     B=NULL      ");}
	else 				{sprintf(str,"     B=%.0fHz    ",B_Fre);}
	LCD_DisplayStringLine(Line4,(u8 *)str);
	LCD_ClearLine(Line5);LCD_ClearLine(Line6);
}
void LCD_Data_T()
{
	char str[20];
	sprintf(str,"        DATA  ");
	LCD_DisplayStringLine(Line1,(u8 *)str);
	if(A_T >= 1000)		{sprintf(str,"     A=%.2fmS    ",A_T/1000);}
	else if(A_T < 0)	{sprintf(str,"     A=NULL      ");}
	else 				{sprintf(str,"     A=%.0fuS    ",A_T);}
	LCD_DisplayStringLine(Line3,(u8 *)str);
	if(B_T >= 1000)		{sprintf(str,"     B=%.2fmS    ",B_T/1000);}
	else if(B_T < 0)	{sprintf(str,"     B=NULL      ");}
	else 				{sprintf(str,"     B=%.0fuS    ",B_T);}
	LCD_DisplayStringLine(Line4,(u8 *)str);
}
void LCD_Para()
{
	char str[20];
	sprintf(str,"        PARA ");
	LCD_DisplayStringLine(Line1,(u8 *)str);
	sprintf(str,"     PD=%dHz    ",PD);
	LCD_DisplayStringLine(Line3,(u8 *)str);
	sprintf(str,"     PH=%dHz    ",PH);
	LCD_DisplayStringLine(Line4,(u8 *)str);
	sprintf(str,"     PX=%dHz    ",PX);
	LCD_DisplayStringLine(Line5,(u8 *)str);
}
void LCD_Reco()
{
	char str[20];
	sprintf(str,"        RECO ");
	LCD_DisplayStringLine(Line1,(u8 *)str);
	sprintf(str,"     NDA=%d      ",NDA);
	LCD_DisplayStringLine(Line3,(u8 *)str);
	sprintf(str,"     NDB=%d      ",NDB);
	LCD_DisplayStringLine(Line4,(u8 *)str);
	sprintf(str,"     NHA=%d      ",NHA);
	LCD_DisplayStringLine(Line5,(u8 *)str);
	sprintf(str,"     NHB=%d      ",NHB);
	LCD_DisplayStringLine(Line6,(u8 *)str);
}
void Uart_Proc()
{
	if(uart1.data_flag)
	{
		uart1.data_flag = 0;
		HAL_UART_Transmit(&huart1,uart1.data,uart1.data_size,0xffff); 
	}
}
void LED_Proc()
{
	GPIOC->ODR = (LED << 8);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}
int sreen = 0,Sel_F_T = 0,Sel_P = 0;
void Key_Proc()
{
	if(sreen ==1)
	{
		if(get_key_stat(0) == 1)
		{
			if(Sel_P == 0)		{PD+=100;if(PD>=1000)PD=1000;}
			else if(Sel_P == 1)	{PH+=100;if(PH>=10000)PH=10000;}
			else 				{PX+=100;if(PX>=1000)PX=1000;}
			set_key_stat(0,0);
		}
		else if(get_key_stat(1) == 1)
		{
			if(Sel_P == 0)		{PD-=100;if(PD<=100)PD=100;}
			else if(Sel_P == 1)	{PH-=100;if(PH<=1000)PH=1000;}
			else 				{PX-=100;if(PX<=-1000)PX=-1000;}
			set_key_stat(1,0);
		}
		else if(get_key_stat(2) == 1)
		{
			Sel_P++;Sel_P%=3;
			set_key_stat(2,0);
		}
	}
	if(get_key_stat(2) == 1)
	{
		if(sreen == 0){Sel_F_T++;Sel_F_T%=2;}
		set_key_stat(2,0);
	}
//	else if(get_key_stat(2) == 3)
//	{
//		if(sreen == 1)
//		{NDA = 0;NDB = 0;NHA = 0;NHB = 0;}
//	}
	if(get_key_stat(3) == 1)
	{
		sreen++;
		sreen%=3;
		set_key_stat(3,0);
	}

}

void LCD_Proc()
{
	switch(sreen)
	{
		case 0: 
			LED|=0xfe;
			if(Sel_F_T){LCD_Data_T();}
			else 		LCD_Data_Fre();	break;
		case 1: 
			LED|=0xff;
			LCD_Para();		break;
			
		case 2: LCD_Reco();		break;
		default:				break;
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
  MX_TIM6_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_TIM17_Init();
  MX_TIM16_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
	LED_Proc();
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	LCD_DisplayStringLine(Line0,(u8 *)"666");
	HAL_UARTEx_ReceiveToIdle_IT(&huart1,uart1.data,100);
	HAL_TIM_Base_Start_IT(&htim6);
	HAL_TIM_PWM_Start(&htim16,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);
	HAL_TIM_IC_Start(&htim2,TIM_CHANNEL_1);
	HAL_TIM_IC_Start(&htim3,TIM_CHANNEL_1);
	
	HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED);
	HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);
	HAL_ADC_Start(&hadc1);
	HAL_ADC_Start(&hadc2);
//	I2CInit();
//	x24c02_write(0x00,'1');
//	HAL_Delay(10);
//	char x= x24c02_read(0x00);
//	HAL_Delay(1000);
//	Set_FreCycle(100);
//	HAL_Delay(10);
//	Set_DutyCycle(30);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  Read_Fre();
	  LCD_Proc();
	  Key_Proc();
	  Uart_Proc();
	  LED_Proc();
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV3;
  RCC_OscInitStruct.PLL.PLLN = 20;
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
