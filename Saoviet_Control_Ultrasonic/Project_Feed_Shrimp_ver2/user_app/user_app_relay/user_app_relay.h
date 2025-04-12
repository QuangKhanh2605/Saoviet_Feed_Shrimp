

#ifndef USER_APP_RELAY_H_
#define USER_APP_RELAY_H_

#define USING_APP_RELAY

#define USING_APP_RELAY_DEBUG

#include "event_driven.h"
#include "user_util.h"

#define TIME_LED_STATUS        2000
#define NUMBER_RELAY           3
#define GPIO_PIN_ON_RELAY      GPIO_PIN_SET
#define GPIO_PIN_OFF_RELAY     GPIO_PIN_RESET

typedef enum
{
    _EVENT_RELAY_ENTRY,
    _EVENT_CONTROL_LED_STATUS,
    _EVENT_CONTROL_LED_RUN,

    
    _EVENT_RELAY_END,
}eKindEventRelay;

typedef enum
{
    _RL_UNDEBUG = 0,
    _RL_DEBUG,
}eKindRelayDebug;


typedef struct
{
    uint8_t RL1;
    uint8_t RL2;
    uint8_t RL3;
    uint8_t RL4;
    uint8_t RL5;
    uint8_t RL6;
    uint8_t RL7;
    uint8_t RL8;
    uint8_t RL9;
    uint8_t RL10;
}Struct_StatusRelay;

typedef enum
{
    RELAY_1 = 0,
    RELAY_2,
    RELAY_3,
    RELAY_4,
    RELAY_5,
    RELAY_6,
    RELAY_7,
    RELAY_8,
    RELAY_9,
}Relay_TypeDef;


typedef enum
{
    OFF_RELAY=0,
    ON_RELAY,
}eNumStateRelay;
extern sEvent_struct        sEventAppRelay[];

extern Struct_StatusRelay         sStatusRelay;
/*=============== Function handle ================*/

uint8_t     AppRelay_Task(void);
void        Init_AppRelay(void);

void        OnOff_Relay(Relay_TypeDef Relay, uint8_t State);
void        Relay_Debug(uint8_t State_Debug, uint8_t KindRelay, uint8_t Status);
void        ControlRelay(uint8_t Relay, uint8_t State, uint8_t RelayDebug);

#endif
