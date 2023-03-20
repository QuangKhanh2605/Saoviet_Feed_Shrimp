#include "uart_debug.h"

uint16_t check_State=1;
uint16_t check_countState=0;


void debug_uart(UART_HandleTypeDef *uart, uint16_t State, uint16_t countState)
{
	if(State == 0 && check_State==0)
	{
		HAL_UART_Transmit(uart, (uint8_t *)("\nSetup"), (uint16_t)strlen("\nSetup"),1000);
		HAL_UART_Transmit(uart,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
		
		HAL_UART_Transmit(uart, (uint8_t *)("Relay2_OFF"), (uint16_t)strlen("Relay2_OFF"),1000);
		HAL_UART_Transmit(uart,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
		
		HAL_UART_Transmit(uart, (uint8_t *)("Relay1_OFF"), (uint16_t)strlen("Relay1_OFF"),1000);
		HAL_UART_Transmit(uart,(uint8_t *)"\r\n\n",(uint16_t)strlen("\r\n\n"),1000);
		check_State = 1;
	}
	if(State == 1 && check_State==1)
	{
		HAL_UART_Transmit(uart, (uint8_t *)("Running"), (uint16_t)strlen("Running"),1000);
		HAL_UART_Transmit(uart,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
		check_State = 0;
		check_countState=1;
	}
	if(State == 1)
	{
		if(countState == 1 && check_countState == 1)
		{		
			HAL_UART_Transmit(uart, (uint8_t *)("Relay1_ON"), (uint16_t)strlen("Relay1_ON"),1000);
			HAL_UART_Transmit(uart,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
			check_countState = 2;
		}
		if(countState == 2 && check_countState == 2)
		{
			HAL_UART_Transmit(uart, (uint8_t *)("Relay2_ON"), (uint16_t)strlen("Relay2_ON"),1000);
			HAL_UART_Transmit(uart,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
			check_countState = 3;
		}
		if(countState == 3 && check_countState == 3)
		{
			HAL_UART_Transmit(uart, (uint8_t *)("Relay2_OFF"), (uint16_t)strlen("Relay2_OFF"),1000);
			HAL_UART_Transmit(uart,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
			check_countState = 4;
		}
		if(countState == 4 && check_countState == 4)
		{
			HAL_UART_Transmit(uart, (uint8_t *)("Relay1_OFF"), (uint16_t)strlen("Relay1_OFF"),1000);
			HAL_UART_Transmit(uart,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
			HAL_UART_Transmit(uart, (uint8_t *)("Free Time"), (uint16_t)strlen("Free Time"),1000);
			HAL_UART_Transmit(uart,(uint8_t *)"\r\n\n",(uint16_t)strlen("\r\n\n"),1000);
			check_countState = 0;
		}
		if(countState == 0 && check_countState == 0)
		{
			check_countState = 1;
		}
	}
}

