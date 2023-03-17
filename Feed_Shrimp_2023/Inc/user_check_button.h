#ifndef __USER_CHECK_BUTTON_
#define __USER_CHECK_BUTTON_

#include "user_LCD_object.h"
#include "check_Button.h"
#include "memory_flash.h"

#define PIN_BT_ENTER GPIO_PIN_12
#define PIN_BT_UP    GPIO_PIN_11
#define PIN_BT_DOWN  GPIO_PIN_10
#define PIN_BT_ESC   GPIO_PIN_15

#define GPIO_BT_ENTER GPIOC
#define GPIO_BT_UP    GPIOC
#define GPIO_BT_DOWN  GPIOC
#define GPIO_BT_ESC   GPIOA

void Check_BT_ENTER(uint16_t *State,uint16_t *checkState, uint16_t *setupCount,uint32_t *time1, uint32_t *time2, uint32_t *time3);
void Check_BT_ESC(uint16_t State, uint16_t *setupCount);
void Check_BT_UP(uint16_t State);
void Check_BT_DOWN(uint16_t State);

uint32_t FLASH_ReadData32(uint32_t addr);
void FLASH_WritePage(uint32_t startPage, uint32_t endPage,uint32_t check, uint32_t data1,uint32_t data2,uint32_t data3);
void Run_Begin(uint16_t *setupCount, float ACS_value, uint32_t time1, uint32_t time2, uint32_t time3);
void BT_Esc_Exit_Setup(uint16_t *State, uint16_t *setupCount, float ACS_value,uint32_t *time1, uint32_t *time2, uint32_t *time3);



#endif
