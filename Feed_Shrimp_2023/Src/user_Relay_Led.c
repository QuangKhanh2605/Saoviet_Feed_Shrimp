#include "user_Relay_Led.h"

uint32_t check_led_status=0;

uint32_t check_ACS_value=0;

uint32_t warning_run_after_500_ms=0;
uint32_t check_warning_run_after_500_ms=0;

uint32_t warning_relay3_after_500_ms=0;
uint32_t check_warning_relay3_after_500_ms=0;

void Warning_Relay2(uint16_t *State, uint16_t *stateWarning_Relay3, uint16_t *countState, uint32_t ACS_Value_Uint, 
                uint32_t threshol_Relay2_Uint)
{
	if(*State == 1 && *countState == 2)
	{
		if(warning_relay3_after_500_ms==0)
		{
			check_warning_relay3_after_500_ms=HAL_GetTick();
			warning_relay3_after_500_ms=2;
		}
		
		if(HAL_GetTick()-check_warning_relay3_after_500_ms>TIME_WAIT_WARNING && warning_relay3_after_500_ms==2)
		{
			warning_relay3_after_500_ms=1;
		}
	}
	else
	{
		warning_relay3_after_500_ms=0;
	}
	
	if(warning_relay3_after_500_ms==1)
	{
		if(ACS_Value_Uint < threshol_Relay2_Uint )
		{
			*stateWarning_Relay3=1;
		}
		else
		{
			*stateWarning_Relay3=0;
		}
	}
}

void Warning_Relay1(uint16_t *State, uint16_t *stateWarning, uint16_t *countState, uint32_t ACS_Value_Uint, 
                uint32_t threshol_Relay1_Uint)
{
	if(*State == 1 && *countState <= 3 )
	{
		if(warning_run_after_500_ms==0)
		{
			check_warning_run_after_500_ms=HAL_GetTick();
			warning_run_after_500_ms=2;
		}
		
		if(HAL_GetTick()-check_warning_run_after_500_ms>TIME_WAIT_WARNING && warning_run_after_500_ms==2)
		{
			warning_run_after_500_ms=1;
		}
	}
	else
	{
		warning_run_after_500_ms=0;
	}

	if(warning_run_after_500_ms==1)
	{
		if(ACS_Value_Uint < threshol_Relay1_Uint)
		{
			*stateWarning=1;
		}
		else
		{
			*stateWarning=0;
		}
	}
	else
	{
		*stateWarning=0;
	}
}

void LED_Status_Run(uint16_t State, uint16_t countState)
{
	if(HAL_GetTick() -check_led_status>TIME_lED_STATUS_TOGGLE)
	{
		check_led_status=HAL_GetTick();
		Set_LED_Status();
	}
	if(HAL_GetTick()-check_led_status>TIME_TOGGLE && HAL_GPIO_ReadPin(GPIO_LED_STATUS, PIN_LED_STATUS)==1)
	{
		Reset_LED_Status();
	}
	
	if(State == 1 && countState < 4)
	{
		Set_LED_Run();
	}
	else
	{
		Reset_LED_Run();
	}
}

void Toggle_LED_Warning(void)
{
	if(HAL_GetTick()-check_ACS_value>TIME_TOGGLE)
	{
		check_ACS_value=HAL_GetTick();
		HAL_GPIO_TogglePin(GPIO_LED_WARNING, PIN_LED_WARNING);
	}
}

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

void Set_LED_Warning(void)
{
	HAL_GPIO_WritePin(GPIO_LED_WARNING, PIN_LED_WARNING, GPIO_PIN_SET);
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

void Reset_LED_Warning(void)
{
	HAL_GPIO_WritePin(GPIO_LED_WARNING, PIN_LED_WARNING, GPIO_PIN_RESET);
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


