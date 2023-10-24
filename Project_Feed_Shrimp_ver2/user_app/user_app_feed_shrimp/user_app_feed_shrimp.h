
#ifndef USER_APP_FEED__
#define USER_APP_FEED__

#define USING_APP_FEED

#include "event_driven.h"
#include "user_util.h"

#include "CLCD.h"

#define CYCLE_MAX         9999
#define CYCLE_DEFAULT     5
#define CYCLE_MIN         0

#define RUN_MAX         9999
#define RUN_DEFAULT     10
#define RUN_MIN         1

#define FREE_MAX        9999
#define FREE_DEFAULT    1
#define FREE_MIN        1

#define CYCLE_DEFAULT   5

#define LEVEL_MAX       30
#define LEVEL_MIN       1

typedef enum
{
    _EVENT_FEED_ENTRY,
    _EVENT_FEED_GET_TIME_RTC,
    _EVENT_FEED_SHRIMP,
    _EVENT_FEED_SAVE_STATE,
    _EVENT_FEED_SAVE_LEVEL,
    _EVENT_FEED_LEVEL_MOTOR,
    
    _EVENT_FEED_END,
}eKindEventFeed;

typedef struct
{
    uint16_t Power;
    uint16_t Level;
    uint32_t Time;
    uint16_t TimeRun;
    uint16_t TimeFree;
    uint16_t TimeCycle;
    uint8_t  State;
    uint16_t StampLevel;
    uint8_t StampPower;
}Struct_Param_Feed;

typedef enum
{
    _OFF_FEED,
    _ON_FEED,
}eKindStateFeed;


extern sEvent_struct                    sEventAppFeed[];
extern Struct_Param_Feed                sParamFeed;
/*================= Function Handle ================*/
uint8_t     AppFeed_Task(void);
void        Init_AppFeed(void);
void        Save_TimeFeed(uint16_t TimeRun, uint16_t TimeFree, uint16_t TimeCycle);
void        Init_LevelFeed(void);
void        Save_LevelFeed(uint16_t Level);
void        Init_StateFeed(void);
void        Save_StateFeed(uint8_t State);

void        Control_MotorLevel(uint8_t Level);
#endif

