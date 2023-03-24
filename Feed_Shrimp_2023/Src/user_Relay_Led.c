#include "user_Relay_Led.h"

uint32_t check_led_status=0;

uint32_t check_ACS_value=0;
uint32_t ACS_value_after_500_ms=0;
uint32_t check_ACS_value_after_500_ms=0;

float stampACSvalue=0;
uint32_t Relay3_after_500_ms=0;
uint32_t check_Relay3_after_500_ms=0;

void Relay3_ACS(uint16_t *State, uint16_t *stateWaring, uint16_t *countState, uint32_t ACS_Value_Uint, 
                uint32_t threshol_Relay1_Uint, uint32_t threshol_Relay2_Uint)
{
	if(*State == 1 && *countState == 2)
	{
		if(Relay3_after_500_ms==0)
		{
			check_Relay3_after_500_ms=GET_SYSTICK_MS();
			Relay3_after_500_ms=2;
		}
		
		if(check_Relay3_after_500_ms>GET_SYSTICK_MS()) 
		{
			Relay3_after_500_ms=0;
			check_Relay3_after_500_ms=0;
		}
		if(GET_SYSTICK_MS()-check_Relay3_after_500_ms>TIME_WAIT_RELAY3_WARNING && Relay3_after_500_ms==2)
		{
			Relay3_after_500_ms=1;
		}
	}
	else
	{
		Relay3_after_500_ms=0;
	}
	
	if(Relay3_after_500_ms==1)
	{
		if(ACS_Value_Uint < threshol_Relay2_Uint )
		{
			*stateWaring=1;
		}
		else
		{
			*stateWaring=0;
		}
	}
	
	if(*countState > 2)
	{
		*stateWaring=0;
	}
}

void Waring(uint16_t *State, uint16_t *stateWaring, uint16_t *countState, uint32_t ACS_Value_Uint, 
                uint32_t threshol_Relay1_Uint, uint32_t threshol_Relay2_Uint)
{
	if(*State == 1 && *countState <= 1 )
	{
		if(ACS_value_after_500_ms==0)
		{
			check_ACS_value_after_500_ms=GET_SYSTICK_MS();
			ACS_value_after_500_ms=2;
		}
		
		if(check_ACS_value_after_500_ms>GET_SYSTICK_MS()) 
		{
			ACS_value_after_500_ms=0;
			check_ACS_value_after_500_ms=0;
		}
		
		if(GET_SYSTICK_MS()-check_ACS_value_after_500_ms>TIME_WAIT_RELAY3_WARNING && ACS_value_after_500_ms==2)
		{
			ACS_value_after_500_ms=1;
		}
	}
	else
	{
		ACS_value_after_500_ms=0;
	}

	if(ACS_value_after_500_ms==1)
	{
		if(ACS_Value_Uint < threshol_Relay1_Uint)
		{
			*stateWaring=1;
		}
		else
		{
			*stateWaring=0;
		}
	}
}

void LED_Status_Run(uint16_t State, uint16_t countState)
{
	if(check_led_status>GET_SYSTICK_MS()) check_led_status=0;
	
	if(GET_SYSTICK_MS() -check_led_status>TIME_lED_STATUS_TOGGLE)
	{
		check_led_status=GET_SYSTICK_MS();
		Set_LED_Status();
	}
	if(GET_SYSTICK_MS()-check_led_status>100 && HAL_GPIO_ReadPin(GPIO_LED_STATUS, PIN_LED_STATUS)==1)
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

void Toggle_LED_Waring(void)
{
	if(check_ACS_value>GET_SYSTICK_MS())  check_ACS_value=0;
			if(GET_SYSTICK_MS()-check_ACS_value>100)
			{
				check_ACS_value=GET_SYSTICK_MS();
				HAL_GPIO_TogglePin(GPIO_LED_WARING, PIN_LED_WARING);
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

void Set_LED_Waring(void)
{
	HAL_GPIO_WritePin(GPIO_LED_WARING, PIN_LED_WARING, GPIO_PIN_SET);
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
	HAL_GPIO_WritePin(GPIO_LED_WARING, PIN_LED_WARING, GPIO_PIN_RESET);
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


