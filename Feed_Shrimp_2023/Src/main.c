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
#include "user_relay_led.h"
#include "user_check_button.h"
#include "uart_sim.h"
#include "ACS712.h"
#include "uart_debug.h"
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
DMA_HandleTypeDef hdma_adc;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint32_t runTime=0;
uint32_t time1=10; //so giay cho an
uint32_t time2=1;	//so phut giua 2 chu ky ban
uint32_t time3=5; //thoi gian giua 2 moto vang va chinh luong 
float threshold_Relay1_Float=0.0;
float threshold_Relay2_Float=0.0;

uint32_t threshold_Relay1_Uint=0;
uint32_t threshold_Relay2_Uint=0;

uint16_t State=1;
uint16_t stateWarning_Run=0;
uint16_t stateWarning_Relay3=0;
uint16_t checkState=0;
uint16_t countState=0;
uint16_t setupCount=1;
uint16_t check_Power_OFF=0;
uint16_t check_Power=0;
uint16_t check_Power_setup=0;

//gio phut giay
uint16_t hh, mm, ss;

CLCD_Name LCD;

UART_BUFFER rx_uart2;

float ACS_Value_Float=0;
uint32_t ACS_Value_Uint=0;
uint16_t ADC_stamp[2];

uint8_t check_RTC=0;
RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;
//UART_BUFFER rx_uart1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM3_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Check_BT_Callback(void);
void Check_Test(void);
void Set_Time(uint16_t *hh, uint16_t *mm, uint16_t *ss);
void Run_Feed_Shrimp(void);
void Display_Time(void);
void Setup_SIM(void);
void Read_Flash(void);
void Check_SMS_Receive(void);
void State_Warning(void);
void Time_RTC(void);
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
  MX_DMA_Init();
  MX_ADC_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
	//HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_UART_Receive_IT(&huart2,&rx_uart2.buffer,4);
	//HAL_ADC_Start_IT(&hadc);
	//HAL_ADC_Start(&hadc);
	//HAL_UART_Receive_IT(&huart1,&rx_uart1.buffer,1);
	
	CLCD_4BIT_Init(&LCD, 16,2, GPIOB, GPIO_PIN_15,GPIOC, GPIO_PIN_6,
                             GPIOC, GPIO_PIN_7,GPIOC, GPIO_PIN_8,
                             GPIOC, GPIO_PIN_9,GPIOA, GPIO_PIN_8);
	Read_Flash();
	
	CLCD_SetCursor(&LCD, 0,0);
	CLCD_WriteString(&LCD, "        00:00:00");
	
	Run_Begin(State,&setupCount, ACS_Value_Float ,time1, time2, time3, threshold_Relay1_Uint, threshold_Relay2_Uint);
	HAL_ADC_Start_DMA(&hadc, (uint32_t*) ADC_stamp,2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		//Check_SMS_Receive();
		Time_RTC();
		Display_Time();
		debug_uart(&huart2, State, countState);
		Check_BT_Callback();
		LED_Status_Run(State, countState);
		
		USER_LCD_Display_Warning(&LCD, stateWarning_Run);
		State_Warning();
		
		if(State==0 )
		{
			stateWarning_Run=0;
			if(setupCount!=4) BT_Check_Up_Down(setupCount);
			BT_Esc_Exit_Setup(&State, &setupCount, ACS_Value_Float,time1, time2, time3, threshold_Relay1_Uint, threshold_Relay2_Uint);
			USER_LCD_Display_Running_OR_Setup(State);
			USER_LCD_Display_Setup(&LCD, setupCount);
			USER_LCD_Display_X(&LCD, setupCount, ACS_Value_Float);
		}
		
		if(State==1 ) 
		{
			Run_Feed_Shrimp(); 
			USER_LCD_Display_Running_OR_Setup(State);
			USER_LCD_Display_Running(&LCD, setupCount, ACS_Value_Float);
			USER_LCD_Display_X(&LCD, setupCount, ACS_Value_Float);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc.Init.LowPowerAutoWait = ADC_AUTOWAIT_DISABLE;
  hadc.Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_DISABLE;
  hadc.Init.ChannelsBank = ADC_CHANNELS_BANK_A;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.NbrOfConversion = 2;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = ENABLE;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_15;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_384CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_14;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
	if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1)==0x2608)
	{
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x2608);
	}
  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 160;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

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
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
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

  /*Configure GPIO pins : PB0 PB1 PB2 PB3
                           PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
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

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void State_Warning()
{
	//Warning_Relay1(&State, &stateWarning_Run, &countState, ACS_Value_Uint, threshold_Relay1_Uint);
	Warning_Relay2(&State, &stateWarning_Relay3, &countState, ACS_Value_Uint, threshold_Relay2_Uint);
	
	if(stateWarning_Run == 1)
	{
		//Toggle_LED_Warning();
	}
	else
	{
		//Reset_LED_Warning();
	}
	
	if(stateWarning_Relay3 == 1)
	{
		Toggle_LED_Warning();
		Set_Relay3();
		//Set_LED_NC();
	}
	else
	{
		Reset_LED_Warning();
		Reset_Relay3();
		//Reset_LED_NC();
	}
}

void Run_Feed_Shrimp(void)
{
	if(countState==0)
	{
		Set_Relay1();
		countState++;
		runTime=0;
	}
	if(countState==1 && runTime>=time3 )
	{
		Set_Relay2();
		countState++;
		runTime=0;
	}
	if(countState==2 && runTime>=time1 )
	{
		Reset_Relay2();
		countState++;
		runTime=0;
	}
	if(countState==3 && runTime>=time3 )
	{
		Reset_Relay1();
		countState++;
		runTime=0;
	}
	if(countState==4 && runTime>=60*time2)
	{
		runTime=0;
		countState=0;
	}
}

void Check_BT_Callback(void)
{
	Check_BT_ENTER(&State, &checkState, &setupCount, &time1, &time2, &time3, &threshold_Relay1_Uint, &threshold_Relay2_Uint );
	Check_BT_ESC(State, &setupCount);
	Check_BT_UP(State, &stateWarning_Relay3);
	Check_BT_DOWN(State, &stateWarning_Relay3);
		
	Check_Test();
}

void Set_Time(uint16_t *hh, uint16_t *mm, uint16_t *ss)
{
	const uint16_t MINUTES_OF_THE_HOUR=60;
	const uint16_t SECOND_OF_THE_HOUR=60;
	const uint32_t SECOND_OF_THE_DAY=86400;
	
	*hh=runTime/(MINUTES_OF_THE_HOUR*SECOND_OF_THE_HOUR);
	*mm=(runTime-(*hh)*MINUTES_OF_THE_HOUR*SECOND_OF_THE_HOUR)/SECOND_OF_THE_HOUR;
	*ss=(runTime-(*hh)*MINUTES_OF_THE_HOUR*SECOND_OF_THE_HOUR-(*mm)*SECOND_OF_THE_HOUR);
	
	if(runTime>=SECOND_OF_THE_DAY)
	{
	runTime=0; *hh=0; *mm=0; *ss=0;
	}
}

void Check_Test(void)
{
	if(checkState==1)
	{
		Reset_Relay1();
		Reset_Relay2();
		Reset_Relay3();
		countState=0;
		runTime=0;
		checkState=0;
	}
	if(State==0)
	{
		Reset_Relay1();
		Reset_Relay2();
		Reset_Relay3();
		countState=0;
		runTime=0;
		if(check_Power_setup==0)
		{
			FLASH_Write_Program(FLASH_USER_START_ADDR_PROGRAM, FLASH_USER_END_ADDR_PROGRAM,1, State, countState, runTime);
			check_Power_setup=1;
		}
	}
}

void Display_Time(void)
{
	Set_Time(&hh, &mm, &ss);
	LCD_Change_State_Time_HH_MM_SS(hh, mm, ss);
	UintTime_To_CharTime_HH_MM_SS(hh,  mm, ss);
	USER_LCD_Display_Time(&LCD);
}

void Read_Flash(void)
{
	check_Power_OFF=FLASH_ReadData32(FLASH_USER_START_ADDR);
	
	if(check_Power_OFF == 1)
	{
		time1=FLASH_ReadData32(FLASH_USER_START_ADDR + 4);
		time2=FLASH_ReadData32(FLASH_USER_START_ADDR + 8);
		time3=FLASH_ReadData32(FLASH_USER_START_ADDR + 12);
		threshold_Relay1_Uint=FLASH_ReadData32(FLASH_USER_START_ADDR + 16);
		threshold_Relay2_Uint=FLASH_ReadData32(FLASH_USER_START_ADDR + 20);
	}
	else
	{
		threshold_Relay1_Uint = threshold_Relay1_Float*pow(10,LENGTH_MOD_FLOAT);
		threshold_Relay2_Uint = threshold_Relay2_Float*pow(10,LENGTH_MOD_FLOAT);
	}
	
	check_Power=FLASH_ReadData32(FLASH_USER_START_ADDR_PROGRAM );
	if(check_Power==1)
	{
		State=FLASH_ReadData32(FLASH_USER_START_ADDR_PROGRAM + 4);
		countState=FLASH_ReadData32(FLASH_USER_START_ADDR_PROGRAM + 8);
		runTime=FLASH_ReadData32(FLASH_USER_START_ADDR_PROGRAM + 12);
		if(countState==1)
		{
			Set_Relay1();
		}
		if(countState==2)
		{
			Set_Relay1();
			Set_Relay2();
		}
	}
}

void Time_RTC(void)
{
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	if(sTime.Seconds != check_RTC)
	{
		check_RTC = sTime.Seconds;
		if(State == 1)
		{
			runTime++;
		}
		
		if(State==0)
		{
			countState=0;
			runTime=0;
		}
		
		if(countState>0 && runTime>0)
		{
			FLASH_Write_Program(FLASH_USER_START_ADDR_PROGRAM, FLASH_USER_END_ADDR_PROGRAM,1, State, countState, runTime);
			check_Power_setup=0;
		}	
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  UNUSED(htim);

	if(htim->Instance == htim3.Instance)
	{
		HAL_ADC_Start_DMA(&hadc, (uint32_t*) ADC_stamp, 2);
		Calib_CountState(countState);
		ACS_712(&ACS_Value_Float, &ACS_Value_Uint, ADC_stamp[0], ADC_stamp[1]);
	}
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
