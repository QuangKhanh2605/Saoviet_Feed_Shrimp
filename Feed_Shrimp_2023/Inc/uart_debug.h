#ifndef __UART_DEBUG_
#define __UART_DEBUG_

#include "stm32l1xx_hal.h"
#include "uart_sim.h"

void debug_uart(UART_HandleTypeDef *uart, uint16_t State, uint16_t countState);

#endif
