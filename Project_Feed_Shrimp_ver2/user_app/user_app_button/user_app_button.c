#include "user_app_button.h"
#include "main.h"
/*==================Function Static====================*/
static uint8_t fevent_button_entry(uint8_t event);
static uint8_t fevent_button_click_enter(uint8_t event);
static uint8_t fevent_button_hold_enter(uint8_t event);
static uint8_t fevent_button_click_up(uint8_t event);
static uint8_t fevent_button_hold_up(uint8_t event);
static uint8_t fevent_button_click_down(uint8_t event);
static uint8_t fevent_button_hold_down(uint8_t event);
static uint8_t fevent_button_click_exit(uint8_t event);
static uint8_t fevent_button_hold_exit(uint8_t event);
/*======================Struct=========================*/
sEvent_struct               sEventAppButton[]=
{
  {_EVENT_BUTTON_ENTRY,                 1, 5, 5,            fevent_button_entry},
  
  {_EVENT_BUTTON_CLICK_ENTER,           1, 5, 5,            fevent_button_click_enter},
  {_EVENT_BUTTON_HOLD_ENTER,            0, 5, 5,            fevent_button_hold_enter},
  
  {_EVENT_BUTTON_CLICK_UP,              1, 5, 5,            fevent_button_click_up},
  {_EVENT_BUTTON_HOLD_UP,               0, 5, 5,            fevent_button_hold_up},
  
  {_EVENT_BUTTON_CLICK_DOWN,            1, 5, 5,            fevent_button_click_down},
  {_EVENT_BUTTON_HOLD_DOWN,             0, 5, 5,            fevent_button_hold_down},
  
  {_EVENT_BUTTON_CLICK_EXIT,            1, 5, 5,            fevent_button_click_exit},
  {_EVENT_BUTTON_HOLD_EXIT,             0, 5, 5,            fevent_button_hold_exit},
};

static GPIO_TypeDef*    BUTTON_PORT[NUMBER_BUTTON] = {BUTTON_ENTER_PORT, BUTTON_UP_PORT, 
                                                      BUTTON_DOWN_PORT, BUTTON_EXIT_PORT};
static const uint16_t   BUTTON_PIN[NUMBER_BUTTON]  = {BUTTON_ENTER_PIN, BUTTON_UP_PIN, 
                                                      BUTTON_DOWN_PIN, BUTTON_EXIT_PIN};
/*=====================Function Handle=================*/
static uint8_t fevent_button_entry(uint8_t event)
{
    return 1;
}

static uint8_t fevent_button_click_enter(uint8_t event)
{
    static uint8_t state_current = 0;
    static uint8_t state_before  = BUTTON_OFF;
    
    state_current = HAL_GPIO_ReadPin(BUTTON_PORT[BT_ENTER], BUTTON_PIN[BT_ENTER]);
    if(state_current == BUTTON_ON)
    {   
        if(sEventAppButton[_EVENT_BUTTON_HOLD_ENTER].e_status == 0)
        {
            sEventAppButton[_EVENT_BUTTON_HOLD_ENTER].e_period = TIME_HOLD_BUTTON_ENTER;
            fevent_enable(sEventAppButton, _EVENT_BUTTON_HOLD_ENTER);
        }
    }
    else
    {
        if(state_before == BUTTON_ON)  
        {
            if(sEventAppButton[_EVENT_BUTTON_HOLD_ENTER].e_period != TIME_HOLD_DELAY)
            {
                User_BT_Enter_Click();
            }
            else    sEventAppButton[_EVENT_BUTTON_HOLD_ENTER].e_period = TIME_HOLD_BUTTON_ENTER;
        }
        if(sEventAppButton[_EVENT_BUTTON_HOLD_ENTER].e_status == 1)
        fevent_disable(sEventAppButton, _EVENT_BUTTON_HOLD_ENTER);
    }
  
    state_before = state_current;
    fevent_enable(sEventAppButton, event);
    return 1;
}

static uint8_t fevent_button_hold_enter(uint8_t event)
{
    if(HAL_GPIO_ReadPin(BUTTON_PORT[BT_ENTER], BUTTON_PIN[BT_ENTER]) == BUTTON_ON)
    {
        if(sEventAppButton[_EVENT_BUTTON_HOLD_ENTER].e_period == TIME_HOLD_BUTTON_ENTER)
        {
            User_BT_Enter_Hold_Once();
        }
        else
        {
            User_BT_Enter_Hold_Cycle();
        }
        fevent_disable(sEventAppButton, _EVENT_BUTTON_CLICK_ENTER);
        sEventAppButton[_EVENT_BUTTON_HOLD_ENTER].e_period = TIME_HOLD_DELAY;
        fevent_enable(sEventAppButton, event);
    }
    else
    {
        fevent_enable(sEventAppButton, _EVENT_BUTTON_CLICK_ENTER);
    }
    return 1;
}

static uint8_t fevent_button_click_up(uint8_t event)
{
    static uint8_t state_current = 0;
    static uint8_t state_before  = BUTTON_OFF;
    
    state_current = HAL_GPIO_ReadPin(BUTTON_PORT[BT_UP], BUTTON_PIN[BT_UP]);
    if(state_current == BUTTON_ON)
    {    
        if(sEventAppButton[_EVENT_BUTTON_HOLD_UP].e_status == 0)
        {
            sEventAppButton[_EVENT_BUTTON_HOLD_UP].e_period = TIME_HOLD_BUTTON_UP;
            fevent_enable(sEventAppButton, _EVENT_BUTTON_HOLD_UP);
        }
    }
    else
    {
        if(state_before == BUTTON_ON)  
        {
            if(sEventAppButton[_EVENT_BUTTON_HOLD_UP].e_period != TIME_HOLD_DELAY)
            {
                User_BT_Up_Click();
            }
            else sEventAppButton[_EVENT_BUTTON_HOLD_UP].e_period = TIME_HOLD_BUTTON_UP;
        }
        if(sEventAppButton[_EVENT_BUTTON_HOLD_UP].e_status == 1)
        fevent_disable(sEventAppButton, _EVENT_BUTTON_HOLD_UP);
    }
  
    state_before = state_current;
    fevent_enable(sEventAppButton, event);
    return 1;
}

static uint8_t fevent_button_hold_up(uint8_t event)
{
    if(HAL_GPIO_ReadPin(BUTTON_PORT[BT_UP], BUTTON_PIN[BT_UP]) == BUTTON_ON)
    {
        if(sEventAppButton[_EVENT_BUTTON_HOLD_UP].e_period == TIME_HOLD_BUTTON_UP)
        {
            User_BT_Up_Hold_Once();
        }
        else
        {
            User_BT_Up_Hold_Cycle();
        }
        fevent_disable(sEventAppButton, _EVENT_BUTTON_CLICK_UP);
        sEventAppButton[_EVENT_BUTTON_HOLD_UP].e_period = TIME_HOLD_DELAY;
        fevent_enable(sEventAppButton, event);
    }
    else
    {
        fevent_enable(sEventAppButton, _EVENT_BUTTON_CLICK_UP);
    }
    return 1;
}

static uint8_t fevent_button_click_down(uint8_t event)
{
    static uint8_t state_current = 0;
    static uint8_t state_before  = BUTTON_OFF;
    
    state_current = HAL_GPIO_ReadPin(BUTTON_PORT[BT_DOWN], BUTTON_PIN[BT_DOWN]);
    if(state_current == BUTTON_ON)
    {
        if(sEventAppButton[_EVENT_BUTTON_HOLD_DOWN].e_status == 0)
        {
            sEventAppButton[_EVENT_BUTTON_HOLD_DOWN].e_period = TIME_HOLD_BUTTON_DOWN;
            fevent_enable(sEventAppButton, _EVENT_BUTTON_HOLD_DOWN);
        }
    }
    else
    {
        if(state_before == BUTTON_ON)  
        {
            if(sEventAppButton[_EVENT_BUTTON_HOLD_DOWN].e_period != TIME_HOLD_DELAY)
            {
                User_BT_Down_Click();
            }
            else sEventAppButton[_EVENT_BUTTON_HOLD_DOWN].e_period = TIME_HOLD_BUTTON_DOWN;
        }
        
        if(sEventAppButton[_EVENT_BUTTON_HOLD_DOWN].e_status == 1)
        fevent_disable(sEventAppButton, _EVENT_BUTTON_HOLD_DOWN);
    }
  
    state_before = state_current;
    fevent_enable(sEventAppButton, event);
    return 1;
}

static uint8_t fevent_button_hold_down(uint8_t event)
{
    if(HAL_GPIO_ReadPin(BUTTON_PORT[BT_DOWN], BUTTON_PIN[BT_DOWN]) == BUTTON_ON)
    {
        if(sEventAppButton[_EVENT_BUTTON_HOLD_DOWN].e_period == TIME_HOLD_BUTTON_DOWN)
        {
            User_BT_Down_Hold_Once();
        }
        else
        {
            User_BT_Down_Hold_Cycle();
        }
        fevent_disable(sEventAppButton, _EVENT_BUTTON_CLICK_DOWN);
        sEventAppButton[_EVENT_BUTTON_HOLD_DOWN].e_period = TIME_HOLD_DELAY;
        fevent_enable(sEventAppButton, event);
    }
    else
    {
        fevent_enable(sEventAppButton, _EVENT_BUTTON_CLICK_DOWN);
    }

    return 1;
}

static uint8_t fevent_button_click_exit(uint8_t event)
{
    static uint8_t state_current = 0;
    static uint8_t state_before  = BUTTON_OFF;
    
    state_current = HAL_GPIO_ReadPin(BUTTON_PORT[BT_EXIT], BUTTON_PIN[BT_EXIT]);
    if(state_current == BUTTON_ON)
    {
        if(sEventAppButton[_EVENT_BUTTON_HOLD_EXIT].e_status == 0)
        {
            sEventAppButton[_EVENT_BUTTON_HOLD_EXIT].e_period = TIME_HOLD_BUTTON_EXIT;
            fevent_enable(sEventAppButton, _EVENT_BUTTON_HOLD_EXIT);
        }
    }
    else
    {
        if(state_before == BUTTON_ON)  
        {
            if(sEventAppButton[_EVENT_BUTTON_HOLD_EXIT].e_period != TIME_HOLD_DELAY)
            {
                User_BT_Exit_Click();
            }
            else sEventAppButton[_EVENT_BUTTON_HOLD_EXIT].e_period = TIME_HOLD_BUTTON_EXIT;
        }
      
        if(sEventAppButton[_EVENT_BUTTON_HOLD_EXIT].e_status == 1)
        fevent_disable(sEventAppButton, _EVENT_BUTTON_HOLD_EXIT);
    }
    
    state_before = state_current;
    fevent_enable(sEventAppButton, event);
    return 1;
}

static uint8_t fevent_button_hold_exit(uint8_t event)
{
    if(HAL_GPIO_ReadPin(BUTTON_PORT[BT_EXIT], BUTTON_PIN[BT_EXIT]) == BUTTON_ON)
    {
        if(sEventAppButton[_EVENT_BUTTON_HOLD_EXIT].e_period == TIME_HOLD_BUTTON_EXIT)
        {
            User_BT_Exit_Hold_Once();
        }
        else
        {
            User_BT_Exit_Hold_Cycle();
        }
        fevent_disable(sEventAppButton, _EVENT_BUTTON_CLICK_EXIT);
        sEventAppButton[_EVENT_BUTTON_HOLD_EXIT].e_period = TIME_HOLD_DELAY;
        fevent_enable(sEventAppButton, event);
    }
    else
    {
        fevent_enable(sEventAppButton, _EVENT_BUTTON_CLICK_EXIT);
    }
    return 1;
}

/*=========================Handle=======================*/

uint8_t AppButton_Task(void)
{
    uint8_t i = 0;
    uint8_t Result =  false;
    
    for(i = 0; i < _EVENT_BUTTON_END; i++)
    {
        if(sEventAppButton[i].e_status == 1)
        {
            Result = true; 
            
            if((sEventAppButton[i].e_systick == 0) ||
               ((HAL_GetTick() - sEventAppButton[i].e_systick) >= sEventAppButton[i].e_period))
            {
                sEventAppButton[i].e_status = 0; //Disable event
                sEventAppButton[i].e_systick= HAL_GetTick();
                sEventAppButton[i].e_function_handler(i);
            }
        }
    }
    
    return Result;
}

