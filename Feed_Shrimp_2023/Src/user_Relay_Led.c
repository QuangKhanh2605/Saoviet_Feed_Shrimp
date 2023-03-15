#include "user_Relay_Led.h"

void Set_Relay1(void)
{
	HAL_GPIO_WritePin(GPIO_RELAY, PIN_RELAY1, GPIO_PIN_SET);
}
void Set_Relay2(void)
{
	HAL_GPIO_WritePin(GPIO_RELAY, PIN_RELAY2, GPIO_PIN_SET);
}
void Set_Relay3(void)
{
	HAL_GPIO_WritePin(GPIO_RELAY, PIN_RELAY3, GPIO_PIN_SET);
}

void Reset_Relay1(void)
{
	HAL_GPIO_WritePin(GPIO_RELAY, PIN_RELAY1, GPIO_PIN_RESET);
}
void Reset_Relay2(void)
{
	HAL_GPIO_WritePin(GPIO_RELAY, PIN_RELAY2, GPIO_PIN_RESET);
}
void Reset_Relay3(void)
{
	HAL_GPIO_WritePin(GPIO_RELAY, PIN_RELAY3, GPIO_PIN_RESET);
}

void Set_LED_Waring(void)
{
	HAL_GPIO_WritePin(GPIO_LED_WARING, PIN_LED_NC, GPIO_PIN_SET);
}
void Set_LED_Run(void)
{
	HAL_GPIO_WritePin(GPIO_LED_RUN, PIN_LED_RUN, GPIO_PIN_SET);
}
void Set_LED_Status(void)
{
	HAL_GPIO_WritePin(GPIO_LED_STATUS, PIN_LED_STATUS, GPIO_PIN_SET);
}
void Set_LED_NC(void)
{
	HAL_GPIO_WritePin(GPIO_LED_NC, PIN_LED_NC, GPIO_PIN_SET);
}

void Reset_LED_Waring(void)
{
	HAL_GPIO_WritePin(GPIO_LED_WARING, PIN_LED_NC, GPIO_PIN_RESET);
}
void Reset_LED_Run(void)
{
	HAL_GPIO_WritePin(GPIO_LED_RUN, PIN_LED_RUN, GPIO_PIN_RESET);
}
void Reset_LED_Status(void)
{
	HAL_GPIO_WritePin(GPIO_LED_STATUS, PIN_LED_STATUS, GPIO_PIN_RESET);
}
void Reset_LED_NC(void)
{
	HAL_GPIO_WritePin(GPIO_LED_NC, PIN_LED_NC, GPIO_PIN_RESET);
}

