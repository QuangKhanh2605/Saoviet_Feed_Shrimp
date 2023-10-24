/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MCU_RL1_Pin GPIO_PIN_0
#define MCU_RL1_GPIO_Port GPIOB
#define MCU_RL2_Pin GPIO_PIN_1
#define MCU_RL2_GPIO_Port GPIOB
#define MCU_RL3_Pin GPIO_PIN_2
#define MCU_RL3_GPIO_Port GPIOB
#define MCU_RS_Pin GPIO_PIN_15
#define MCU_RS_GPIO_Port GPIOB
#define MCU_EN_Pin GPIO_PIN_6
#define MCU_EN_GPIO_Port GPIOC
#define LCD_D4_Pin GPIO_PIN_7
#define LCD_D4_GPIO_Port GPIOC
#define LCD_D5_Pin GPIO_PIN_8
#define LCD_D5_GPIO_Port GPIOC
#define LCD_D6_Pin GPIO_PIN_9
#define LCD_D6_GPIO_Port GPIOC
#define LCD_D7_Pin GPIO_PIN_8
#define LCD_D7_GPIO_Port GPIOA
#define BT_EXIT_Pin GPIO_PIN_15
#define BT_EXIT_GPIO_Port GPIOA
#define BT_DOWN_Pin GPIO_PIN_10
#define BT_DOWN_GPIO_Port GPIOC
#define BT_UP_Pin GPIO_PIN_11
#define BT_UP_GPIO_Port GPIOC
#define BT_ENTER_Pin GPIO_PIN_12
#define BT_ENTER_GPIO_Port GPIOC
#define LED_NC_Pin GPIO_PIN_2
#define LED_NC_GPIO_Port GPIOD
#define LED_STATUS_Pin GPIO_PIN_3
#define LED_STATUS_GPIO_Port GPIOB
#define LED_RUN_Pin GPIO_PIN_4
#define LED_RUN_GPIO_Port GPIOB
#define LED_WARNING_Pin GPIO_PIN_5
#define LED_WARNING_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
