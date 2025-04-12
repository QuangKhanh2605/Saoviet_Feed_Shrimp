#include "user_app_feed_shrimp.h"
#include "rtc.h"
#include "user_app_relay.h"
#include "user_internal_mem.h"
#include "user_app_menu_display.h"
#include "user_app_lcd.h"
#include "iwdg.h"
#include "tim.h"
/*=============== Function static ================*/
static uint8_t fevent_feed_entry(uint8_t event);
static uint8_t fevent_feed_get_time_rtc(uint8_t event);
static uint8_t fevent_feed_shrimp(uint8_t event);
static uint8_t fevent_feed_save_state(uint8_t event);
static uint8_t fevent_feed_save_level(uint8_t event);
static uint8_t fevent_feed_level_motor(uint8_t event);
/*================== Struct ===================*/
sEvent_struct       sEventAppFeed[]=
{
  {_EVENT_FEED_ENTRY,            1, 5, 0,        fevent_feed_entry},
  {_EVENT_FEED_GET_TIME_RTC,     1, 5, 250,      fevent_feed_get_time_rtc},
  {_EVENT_FEED_SHRIMP,           1, 5, 20,       fevent_feed_shrimp},
  {_EVENT_FEED_SAVE_STATE,       1, 5, 5,        fevent_feed_save_state},
  {_EVENT_FEED_SAVE_LEVEL,       1, 5, 5000,     fevent_feed_save_level},
  {_EVENT_FEED_LEVEL_MOTOR,      1, 5, 5,        fevent_feed_level_motor},
};

RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;

Struct_Param_Feed               sParamFeed={0};
/*===== Object LCD =============== MAIN Screen 1 =================*/

/*=============================== Function Handle ===============================*/

static uint8_t fevent_feed_entry(uint8_t event)
{
    return 1;
}
uint32_t Gettick = 0;
static uint8_t fevent_feed_get_time_rtc(uint8_t event)
{
    static uint8_t check_RTC= 0;
    
    HAL_IWDG_Refresh(&hiwdg);
    Gettick = HAL_GetTick();
    
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	if(sTime.Seconds != check_RTC)
	{
		check_RTC = sTime.Seconds;
		if(sParamFeed.Power == _ON_FEED)
		{
			sParamFeed.Time++;
            Set_Time(sParamFeed.Time);
		}
		
		if(sParamFeed.Power == _OFF_FEED)
		{
			sParamFeed.State=0;
			sParamFeed.Time=0;
            Set_Time(sParamFeed.Time);
		}
	}
    fevent_enable(sEventAppFeed, event);
    return 1;
}

static uint8_t fevent_feed_shrimp(uint8_t event)
{
    static uint32_t Time_Ultrasonic_On = 0;
    static uint32_t Time_Ultrasonic_Off = 0;
    static uint8_t State_Ultrasonic = 0;
    
    if(sParamFeed.Power == _ON_FEED)
    {
        if(sParamFeed.State==0)
        {
            OnOff_Relay(RELAY_1, ON_RELAY);
            sParamFeed.State++;
            sParamFeed.Time=0;
        }
        if(sParamFeed.State==1 && sParamFeed.Time>=sParamFeed.TimeRun )
        {
//            Control_MotorLevel(sParamFeed.Level);
//            OnOff_Relay(RELAY_3, ON_RELAY);
            OnOff_Relay(RELAY_1, OFF_RELAY);
            sParamFeed.State++;
            sParamFeed.Time=0;
            
            Time_Ultrasonic_Off = HAL_GetTick();
            Time_Ultrasonic_On = HAL_GetTick();
            State_Ultrasonic = 1;
            OnOff_Relay(RELAY_3, ON_RELAY);
        }
        
        if(sParamFeed.State==2)
        {
            if(sParamFeed.Time>=sParamFeed.TimeCycle)
            {
//                Off_MotorLevel();
                OnOff_Relay(RELAY_2, ON_RELAY);
                sParamFeed.State++;
                sParamFeed.Time=0;
                OnOff_Relay(RELAY_3, OFF_RELAY);
            }
            else
            {
                if(State_Ultrasonic == 1)
                {
                    if(HAL_GetTick() - Time_Ultrasonic_On > 8000)
                    {
                        State_Ultrasonic = 0;
                        Time_Ultrasonic_Off = HAL_GetTick();
                        OnOff_Relay(RELAY_3, OFF_RELAY);
                    }
                }
                else
                {
                    if(HAL_GetTick() - Time_Ultrasonic_Off > 2000)
                    {
                        State_Ultrasonic = 1;
                        Time_Ultrasonic_On = HAL_GetTick();
                        OnOff_Relay(RELAY_3, ON_RELAY);
                    }
                }
            }
        }
        if(sParamFeed.State==3 && sParamFeed.Time>=sParamFeed.TimeFree)
        {
            OnOff_Relay(RELAY_2, OFF_RELAY);
//            sParamFeed.State++;
            sParamFeed.State=0;
            sParamFeed.Time=0;
        }
//        if(sParamFeed.State==4 && sParamFeed.Time>=60*sParamFeed.TimeFree)
//        {
//            sParamFeed.Time=0;
//            sParamFeed.State=0;
//        }
    }
    else
    {
        OnOff_Relay(RELAY_1, OFF_RELAY);
        OnOff_Relay(RELAY_2, OFF_RELAY);
        OnOff_Relay(RELAY_3, OFF_RELAY);
//        Off_MotorLevel();
    }
    
    fevent_enable(sEventAppFeed, event);
    return 1;
}

static uint8_t fevent_feed_save_state(uint8_t event)
{
    if(sParamFeed.StampPower != sParamFeed.Power)
    {
        Save_StateFeed(sParamFeed.Power);
    }
    fevent_enable(sEventAppFeed, event);
    return 1;
}

static uint8_t fevent_feed_save_level(uint8_t event)
{
    if(sParamFeed.StampLevel != sParamFeed.Level)
    {
        Save_LevelFeed(sParamFeed.Level);
    }
    fevent_enable(sEventAppFeed, event);
    return 1;
}

static uint8_t fevent_feed_level_motor(uint8_t event)
{
    static uint16_t stampLevel = 0;
    
    if(stampLevel != sParamFeed.Level)
    {
        if(sParamFeed.State == 2)
        {
            Control_MotorLevel(sParamFeed.Level);
        }
    }
    
    stampLevel = sParamFeed.Level;
    
    fevent_enable(sEventAppFeed, event);
    return 1;
}

/*=========================================================*/
void Control_MotorLevel(uint8_t Level)
{
  __HAL_TIM_SetCompare (&htim2, TIM_CHANNEL_2, Level);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
}

void Off_MotorLevel(void)
{
    __HAL_TIM_SetCompare (&htim2, TIM_CHANNEL_2, 0);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
}


void Save_StateFeed(uint8_t State)
{
    uint8_t aData[2] = {0};
    uint8_t length = 0;
    
    sParamFeed.StampPower = State;
    
    aData[length++] = sParamFeed.Power;

    Save_Array(ADDR_STATE_FEED_SHRIMP, aData, length);
}

void Init_StateFeed(void)
{
    if(*(__IO uint8_t*)(ADDR_STATE_FEED_SHRIMP) == BYTE_TEMP_FIRST)
    {
        sParamFeed.Power = *(__IO uint8_t*)(ADDR_STATE_FEED_SHRIMP+2);
    }
    else
    {
        sParamFeed.Power = _ON_FEED;
    }
    
    sParamFeed.StampPower = sParamFeed.Power;
    Off_MotorLevel();
}


void    Save_TimeFeed(uint16_t TimeRun, uint16_t TimeFree, uint16_t TimeCycle)
{
    uint8_t aData[8] = {0};
    uint8_t length = 0;
    
    sParamFeed.TimeRun = TimeRun;
    sParamFeed.TimeFree = TimeFree;
    sParamFeed.TimeCycle = TimeCycle;
    
    aData[length++] = sParamFeed.TimeRun >>8;
    aData[length++] = sParamFeed.TimeRun;
    
    aData[length++] = sParamFeed.TimeFree >> 8;
    aData[length++] = sParamFeed.TimeFree;
    
    aData[length++] = sParamFeed.TimeCycle >> 8;
    aData[length++] = sParamFeed.TimeCycle;

    Save_Array(ADDR_TIME_FEED_SHRIMP, aData, length);
}

void    Save_LevelFeed(uint16_t Level)
{
    uint8_t aData[8] = {0};
    uint8_t length = 0;
    
    sParamFeed.StampLevel = Level;
    
    aData[length++] = sParamFeed.Level >>8;
    aData[length++] = sParamFeed.Level;

    Save_Array(ADDR_LEVEL_FEED_SHRIMP, aData, length);
}
    
void    Init_LevelFeed(void)
{
    if(*(__IO uint8_t*)(ADDR_LEVEL_FEED_SHRIMP) == BYTE_TEMP_FIRST)
    {
        sParamFeed.Level = *(__IO uint8_t*)(ADDR_LEVEL_FEED_SHRIMP+2);
        sParamFeed.Level = sParamFeed.Level << 8;
        sParamFeed.Level = *(__IO uint8_t*)(ADDR_LEVEL_FEED_SHRIMP+3);
    }
    else
    {
        sParamFeed.Level = LEVEL_MAX;
    }
    sParamFeed.StampLevel = sParamFeed.Level;
}

void Init_TimeFeed(void)
{
    if(*(__IO uint8_t*)(ADDR_TIME_FEED_SHRIMP) == BYTE_TEMP_FIRST)
    {
        sParamFeed.TimeRun = *(__IO uint8_t*)(ADDR_TIME_FEED_SHRIMP+2);
        sParamFeed.TimeRun = sParamFeed.TimeRun << 8;
        sParamFeed.TimeRun |= *(__IO uint8_t*)(ADDR_TIME_FEED_SHRIMP+3);
        
        sParamFeed.TimeFree  = *(__IO uint8_t*)(ADDR_TIME_FEED_SHRIMP+4) ;
        sParamFeed.TimeFree  = sParamFeed.TimeFree << 8;
        sParamFeed.TimeFree |= *(__IO uint8_t*)(ADDR_TIME_FEED_SHRIMP+5);
        
        sParamFeed.TimeCycle  = *(__IO uint8_t*)(ADDR_TIME_FEED_SHRIMP+6) ;
        sParamFeed.TimeCycle  = sParamFeed.TimeCycle << 8;
        sParamFeed.TimeCycle |= *(__IO uint8_t*)(ADDR_TIME_FEED_SHRIMP+7);
    }
    else
    {
        sParamFeed.TimeRun = RUN_DEFAULT;
        sParamFeed.TimeFree = FREE_DEFAULT;
        sParamFeed.TimeCycle = CYCLE_DEFAULT;
    }
    Menu_ResetStamp();
}

void Init_AppFeed(void)
{
    Init_TimeFeed();
    Init_LevelFeed();
    Init_StateFeed();
}

uint8_t AppFeed_Task(void)
{
    uint8_t i = 0;
    uint8_t Result =  false;
    
    for(i = 0; i < _EVENT_FEED_END; i++)
    {
        if(sEventAppFeed[i].e_status == 1)
        {
            Result = true; 
            
            if((sEventAppFeed[i].e_systick == 0) ||
               ((HAL_GetTick() - sEventAppFeed[i].e_systick) >= sEventAppFeed[i].e_period))
            {
                sEventAppFeed[i].e_status = 0; //Disable event
                sEventAppFeed[i].e_systick= HAL_GetTick();
                sEventAppFeed[i].e_function_handler(i);
            }
        }
    }
    
    
    return Result;
}


