#ifndef __USER_RELAY_LED_
#define __USER_RELAY_LED_	

#include "stm32l1xx_hal.h"
#include "Check_Button.h"

#define TIME_lED_STATUS_TOGGLE   2000
#define TIME_WAIT_RELAY3_WARNING 1000
#define ACS_VALUE_WARING         0.2

#define PIN_RELAY1 GPIO_PIN_0 
#define PIN_RELAY2 GPIO_PIN_1
#define PIN_RELAY3 GPIO_PIN_2

#define PIN_LED_NC       GPIO_PIN_2
#define PIN_LED_STATUS   GPIO_PIN_3
#define PIN_LED_RUN      GPIO_PIN_4
#define PIN_LED_WARING   GPIO_PIN_5

#define GPIO_RELAY GPIOB

#define GPIO_LED_NC       GPIOD
#define GPIO_LED_STATUS   GPIOB
#define GPIO_LED_RUN      GPIOB
#define GPIO_LED_WARING   GPIOB

void LED_Status_Run(uint16_t State, uint16_t countState);
void Waring(uint16_t *State, uint16_t *stateWaring, uint16_t *countState, uint32_t ACS_Value_Uint, 
                uint32_t threshol_Relay1_Uint, uint32_t threshol_Relay2_Uint);
void Relay3_ACS(uint16_t *State, uint16_t *stateWaring, uint16_t *countState, uint32_t ACS_Value_Uint, 
                uint32_t threshol_Relay1_Uint, uint32_t threshol_Relay2_Uint);
void Toggle_LED_Waring(void);
void Set_Relay1(void);
void Set_Relay2(void);
void Set_Relay3(void);

void Reset_Relay1(void);
void Reset_Relay2(void);
void Reset_Relay3(void);

void Set_LED_Waring(void);
void Set_LED_Run(void);
void Set_LED_Status(void);
void Set_LED_NC(void);

void Reset_LED_Waring(void);
void Reset_LED_Run(void);
void Reset_LED_Status(void);
void Reset_LED_NC(void);


#endif
