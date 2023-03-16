/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "CLCD.h"
#include "user_LCD.h"
#include "check_Button.h"
#include "user_Relay_Led.h"
#include "user_LCD_object.h"
#include "user_check_button.h"
#include "uart_sim.h"
#include "ACS712.h"

//#include "user_sim.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */



//#define On_Off_Sim GPIO_PIN_13
//#define Pin_PWKEY  GPIO_PIN_0
//#define Pin_RESET  GPIO_PIN_5
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint32_t runTime=0;
uint32_t time1=10; //so giay cho an
uint32_t time2=10;	//so phut giua 2 chu ky ban
uint32_t time3=5; //thoi gian giua 2 moto vang va chinh luong 

uint16_t State=1;
uint16_t checkState=0;
uint16_t countState=0;
uint16_t setupCount=1;
uint16_t check_Power_OFF=0;

//gio phut giay
uint16_t hh, mm, ss;

CLCD_Name LCD;

UART_BUFFER rx_uart2;

float ACS_Value=0;

//UART_BUFFER rx_uart1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Delay_1ms(void);
void Delay_s(int time);
void Check_BT_Callback(void);
void Check_Test(void);
void Set_Time(uint16_t *hh, uint16_t *mm, uint16_t *ss);
void Run_Feed_Shrimp(void);
void BT_Check_Up_Down(void);
void Display_Time(void);
void Setup_SIM(void);
void Read_Flash(void);
void Check_SMS_Receive(void);
/* USER CODE END 0 */
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	//rx_uart1.huart=&huart1;
	rx_uart2.huart=&huart2;
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
  MX_ADC_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_UART_Receive_IT(&huart2,&rx_uart2.buffer,1);
	//HAL_ADC_Start_IT(&hadc);
	HAL_ADC_Start(&hadc);
	//HAL_UART_Receive_IT(&huart1,&rx_uart1.buffer,1);
	
	CLCD_4BIT_Init(&LCD, 16,2, GPIOB, GPIO_PIN_15,GPIOC, GPIO_PIN_6,
                             GPIOC, GPIO_PIN_7,GPIOC, GPIO_PIN_8,
                             GPIOC, GPIO_PIN_9,GPIOA, GPIO_PIN_8);
	
	//Setup_SIM();
	Read_Flash();
	
	CLCD_SetCursor(&LCD, 0,0);
	CLCD_WriteString(&LCD, "        00:00:00");
	
	//Receive_SMS_Setup("+CMT: +84966674796,23/03/14,09:34:14+28 SETUP T1=  100  t3:199", &time1, &time2, &time3);
	Run_Begin(&setupCount, ACS_Value ,time1, time2, time3);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		//Check_SMS_Receive();
		
		Display_Time();
		Check_BT_Callback();
		ACS_712(&hadc, &ACS_Value);
		if(State==0 )
		{
			Check_Test();
			BT_Esc_Exit_Setup(&State, &setupCount, ACS_Value,&time1, &time2, &time3);
			USER_LCD_Display_Running_OR_Setup(State);
			if(setupCount!=4) BT_Check_Up_Down();
			USER_LCD_Display_Setup(&LCD, setupCount);
		}
		
		if(State==1) 
		{
			Run_Feed_Shrimp();
			USER_LCD_Display_Running_OR_Setup(State);
			USER_LCD_Display_Running(&LCD, setupCount, ACS_Value);
			Check_Test();
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_10B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc.Init.LowPowerAutoWait = ADC_AUTOWAIT_DISABLE;
  hadc.Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_DISABLE;
  hadc.Init.ChannelsBank = ADC_CHANNELS_BANK_A;
  hadc.Init.ContinuousConvMode = ENABLE;
  hadc.Init.NbrOfConversion = 1;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_14;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_384CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 16000;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_15 
                          |GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB0 PB1 PB2 PB15 
                           PB3 PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_15 
                          |GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PC6 PC7 PC8 PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PC10 PC11 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PD2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */



//void Check_SMS_Receive(void)
//{
//	if(Wait_SMS_Receive(&rx_uart1,&rx_uart3,"AT+RESET")==1) 
//	{
//		Reset_Relay_Led(GPIOC, RELAY1, GPIOC, RELAY2,GPIOC, LED5);
//		Reset_Relay_Led(GPIOA, RELAY3, GPIOA, RELAY4,GPIOC, LED6);
//		runTime=0;
//		countState=0;
//		Delete_Buffer(&rx_uart3);
//	}
//	if(Wait_SMS_Receive(&rx_uart1,&rx_uart3,"SETUP")==1) 
//	{
//		Receive_SMS_Setup(rx_uart3.sim_rx, &time1, &time2, &time3);
//		Reset_Relay_Led(GPIOC, RELAY1, GPIOC, RELAY2,GPIOC, LED5);
//		Reset_Relay_Led(GPIOA, RELAY3, GPIOA, RELAY4,GPIOC, LED6);
//		runTime=0;
//		countState=0;
//		State=1;
//		FLASH_WritePage(FLASH_USER_START_ADDR, FLASH_USER_END_ADDR, 1, time1, time2, time3);
//		Run_Begin(&setupCount, time1, time2, time3);
//		Delete_Buffer(&rx_uart3);
//	}
//}


void Run_Feed_Shrimp(void)
{
	if(countState==0)
	{
		//Set_Relay_Led(GPIOC, RELAY1, GPIOC, RELAY2,GPIOC, LED5);
		Set_Relay1();
		countState++;
		runTime=0;
	}
	if(countState==1 && runTime>=time3 )
	{
		//Set_Relay_Led(GPIOA, RELAY3, GPIOA, RELAY4,GPIOC, LED6);
		Set_Relay2();
		countState++;
		runTime=0;
	}
	if(countState==2 && runTime>=time1 )
	{
		//Reset_Relay_Led(GPIOA, RELAY3, GPIOA, RELAY4,GPIOC, LED6);
		Reset_Relay2();
		countState++;
		runTime=0;
	}
	if(countState==3 && runTime>=time3 )
	{
		//Reset_Relay_Led(GPIOC, RELAY1, GPIOC, RELAY2,GPIOC, LED5);
		Reset_Relay1();
		countState++;
		runTime=0;
	}
	if(countState==4 && runTime>=time2)
	{
		runTime=0;
		countState=0;
	}
}

void Check_BT_Callback(void)
{
	Check_BT_ENTER(&State, &checkState, &setupCount, &time1, &time2, &time3);
	Check_BT_ESC(State, &setupCount);
	Check_BT_UP(State);
	Check_BT_DOWN(State);
		
	//Check_Test();
}

void Set_Time(uint16_t *hh, uint16_t *mm, uint16_t *ss)
{
	const uint16_t MINUTES_OF_THE_HOUR=60;
	const uint16_t SECOND_OF_THE_HOUR=60;
	const uint32_t SECOND_OF_THE_DAY=86400;
	
	*hh=runTime/(MINUTES_OF_THE_HOUR*SECOND_OF_THE_HOUR);
	*mm=(runTime-(*hh)*MINUTES_OF_THE_HOUR*SECOND_OF_THE_HOUR)/SECOND_OF_THE_HOUR;
	*ss=(runTime-(*hh)*MINUTES_OF_THE_HOUR*SECOND_OF_THE_HOUR-(*mm)*SECOND_OF_THE_HOUR);
	
	if(runTime>=SECOND_OF_THE_DAY-1)
	{
	runTime=0; *hh=0; *mm=0; *ss=0;
	}
}

void Check_Test(void)
{
	if(checkState==1)
	{
//		Reset_Relay_Led(GPIOC, RELAY1, GPIOC, RELAY2,GPIOC, LED5);
//		Reset_Relay_Led(GPIOA, RELAY3, GPIOA, RELAY4,GPIOC, LED6);
		Reset_Relay1();
		Reset_Relay2();
		countState=0;
		runTime=0;
		checkState=0;
	}
	if(State==0)
	{
//		Reset_Relay_Led(GPIOC, RELAY1, GPIOC, RELAY2,GPIOC, LED5);
//		Reset_Relay_Led(GPIOA, RELAY3, GPIOA, RELAY4,GPIOC, LED6);
		Reset_Relay1();
		Reset_Relay2();
		countState=0;
		runTime=0;
	}
}

void Display_Time(void)
{
	Set_Time(&hh, &mm, &ss);
	LCD_Change_State_Time_HH_MM_SS(hh, mm, ss);
	UintTime_To_CharTime_HH_MM_SS(hh,  mm, ss);
	USER_LCD_Display_Time(&LCD);
}

//void Setup_SIM(void)
//{
//	float PWM_runtime_LCD=1.32;
//	CLCD_SetCursor(&LCD, 2,0);
//	CLCD_WriteString(&LCD, "Loading SIM");	
//	while(runTime<=22)
//	{
//	Setup_On_Off_Sim(GPIOB, On_Off_Sim,
//                   GPIOB, Pin_PWKEY,
//									 GPIOC, Pin_RESET, runTime);
//	CLCD_SetCursor(&LCD, runTime/PWM_runtime_LCD,1);
//	CLCD_WriteString(&LCD, ".");
//	}
//	if(runTime<=25)
//	{
//	CLCD_SetCursor(&LCD, 2,0);
//	CLCD_WriteString(&LCD, " Config SIM");
//	}
//	Delete_Buffer(&rx_uart3);
//	Config_Uart_Sim(&rx_uart1,&rx_uart3);
//	Config_SMS_Receive(&rx_uart1, &rx_uart3);
//}

void Read_Flash(void)
{
	check_Power_OFF=FLASH_ReadData32(FLASH_USER_START_ADDR);
	
	if(check_Power_OFF == 1)
	{
		time1=FLASH_ReadData32(FLASH_USER_START_ADDR + 4);
		time2=FLASH_ReadData32(FLASH_USER_START_ADDR + 8);
		time3=FLASH_ReadData32(FLASH_USER_START_ADDR + 12);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  UNUSED(htim);
	runTime++;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
	if(huart->Instance == huart2.Instance)
	{
		rx_uart2.sim_rx[(rx_uart2.countBuffer)++]= rx_uart2.buffer;
		HAL_UART_Receive_IT(&huart2,&rx_uart2.buffer,1);
		
	}
	
//	if(huart->Instance == huart1.Instance)
//	{
//		rx_uart1.sim_rx[(rx_uart1.countBuffer)++]= rx_uart1.buffer;
//		HAL_UART_Receive_IT(&huart1,&rx_uart1.buffer,1);
//	}
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);
  /* NOTE : This function should not be modified. When the callback is needed,
            function HAL_ADC_ConvCpltCallback must be implemented in the user file.
   */
}

void Delay_1ms(void)
{
	__HAL_TIM_SetCounter(&htim2,0);
	while (__HAL_TIM_GetCounter(&htim2)<1000);
}
void Delay_s(int time)
{
	int i=0;
	for(i=0;i<time;i++)
	{
		Delay_1ms();
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
