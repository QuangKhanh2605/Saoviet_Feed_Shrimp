#ifndef __CHECK_BUTTON_H
#define __CHECK_BUTTON_H

#include "stm32l1xx_it.h"
#include "stm32l1xx_hal.h"

#define MAX_TIME 9999

void HAL_SYSTICK_Callback(void);

void BT_Press_Click_Up(uint16_t *BT_up, uint32_t *ptr_stamp);
void BT_Press_Click_Down(uint16_t *BT_down, uint32_t *ptr_stamp);

void BT_Press_Hold_Up( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pinx, uint32_t *ptr_stamp);
void BT_Press_Hold_Down( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pinx, uint32_t *ptr_stamp);

void BT_Press_Hold_Esc( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pinx, uint16_t *State, uint16_t BT_up, uint16_t BT_down);

#endif
