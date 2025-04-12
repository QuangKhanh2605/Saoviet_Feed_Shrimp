
#include "user_app_relay.h"
#include "main.h"
#include "user_app_feed_shrimp.h"
/*============== Function static ===============*/
static uint8_t fevent_relay_entry(uint8_t event);
static uint8_t fevent_control_led_status(uint8_t event);
static uint8_t fevent_control_led_run(uint8_t event);
/*=================== struct ==================*/
sEvent_struct               sEventAppRelay[] = 
{
  {_EVENT_RELAY_ENTRY,              1, 0, 2000,                 fevent_relay_entry},

  {_EVENT_CONTROL_LED_STATUS,       1, 5, TIME_LED_STATUS,      fevent_control_led_status},
  {_EVENT_CONTROL_LED_RUN,          1, 5, 5,                    fevent_control_led_run},

};

Struct_StatusRelay          sStatusRelay={OFF_RELAY};                
static GPIO_TypeDef*    RELAY_PORT[NUMBER_RELAY] = {MCU_RL1_GPIO_Port, MCU_RL2_GPIO_Port, MCU_RL3_GPIO_Port};

static uint16_t         RELAY_PIN[NUMBER_RELAY] = {MCU_RL1_Pin, MCU_RL2_Pin, MCU_RL3_Pin};


/*================= Function Handle ==============*/
static uint8_t fevent_relay_entry(uint8_t event)
{
    fevent_active(sEventAppRelay, _EVENT_CONTROL_LED_STATUS);
    return 1;
}

static uint8_t fevent_control_led_status(uint8_t event)
{
/*-----------------Dieu khien led Status----------------*/
    if(sEventAppRelay[_EVENT_CONTROL_LED_STATUS].e_period == TIME_LED_STATUS)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
        sEventAppRelay[_EVENT_CONTROL_LED_STATUS].e_period = 40;
    }
    else
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
        sEventAppRelay[_EVENT_CONTROL_LED_STATUS].e_period = TIME_LED_STATUS;
    }
    
    fevent_enable(sEventAppRelay, event);
    return 1;
}

static uint8_t fevent_control_led_run(uint8_t event)
{
    if(sParamFeed.State > 0 && sParamFeed.State < 4)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
    }
    fevent_enable(sEventAppRelay, event);
    return 1;
}

/*========== Function Handle ============*/

/*
    @brief  Dieu khien Relay 
    @param  Relay: Loai relay
    @param  State: Trang thai dong hoac mo
    @param  StateRespond: Co phan hoi ve PcBox hay k
    @param  RelayDebug: Co hien thi Debug hay k 
    @param  RelayCtrl: Co dieu khien relay qua tap lenh k 
*/
void ControlRelay(uint8_t Relay, uint8_t State, uint8_t RelayDebug)
{
  if(State == OFF_RELAY || State == ON_RELAY)
  {
    switch(Relay)
    {
          case RELAY_1: 
            OnOff_Relay(RELAY_1, State);
            sStatusRelay.RL1 = State;
            break;

          case RELAY_2: 
            OnOff_Relay(RELAY_2, State);
            sStatusRelay.RL2 = State;
            break;
            
          case RELAY_3: 
            OnOff_Relay(RELAY_3, State);
            sStatusRelay.RL3 = State;
            break;
            
          case RELAY_4: 
            OnOff_Relay(RELAY_4, State);
            sStatusRelay.RL4 = State;
            break;
            
          case RELAY_5: 
            OnOff_Relay(RELAY_5, State);
            sStatusRelay.RL5 = State;
            break;
            
          case RELAY_6: 
            OnOff_Relay(RELAY_6, State);
            sStatusRelay.RL6 = State;
            break;
            
          case RELAY_7: 
            OnOff_Relay(RELAY_7, State);
            sStatusRelay.RL7 = State;
            break;
            
          case RELAY_8: 
            OnOff_Relay(RELAY_8, State);
            sStatusRelay.RL8 = State;
            break;
            
          case RELAY_9: 
            OnOff_Relay(RELAY_9, State);
            sStatusRelay.RL9 = State;
            break;
            
         default:
            break;
    }
    Relay_Debug(RelayDebug, Relay, State);
  }
}

/*
    @brief  ON/OFF Relay
    @param  Relay: Loai relay
    @param  State: Trang thai ON hoac OFF
*/
void OnOff_Relay(Relay_TypeDef Relay, uint8_t State)
{
    if(State == ON_RELAY)
    {
        if(Relay == RELAY_1)
            HAL_GPIO_WritePin(RELAY_PORT[Relay], RELAY_PIN[Relay], GPIO_PIN_OFF_RELAY);
        else
            HAL_GPIO_WritePin(RELAY_PORT[Relay], RELAY_PIN[Relay], GPIO_PIN_ON_RELAY);
          
    }
    else
    {
        if(Relay == RELAY_1)
            HAL_GPIO_WritePin(RELAY_PORT[Relay], RELAY_PIN[Relay], GPIO_PIN_ON_RELAY);
        else
            HAL_GPIO_WritePin(RELAY_PORT[Relay], RELAY_PIN[Relay], GPIO_PIN_OFF_RELAY);
    }
}


void Init_AppRelay(void)
{
  
}



/*
    @brief  Hien thi debug bat tat relay
    @param  State_Debug: Co thuc hien hien thi debug khong
    @param  KindRelay: Loai relay 
    @param  Status: Trang thai relay hien tai
*/
void Relay_Debug(uint8_t State_Debug, uint8_t KindRelay, uint8_t Status)
{
    if(State_Debug == _RL_DEBUG)
    {
#ifdef USING_APP_RELAY_DEBUG
    if(Status == ON_RELAY)
    {
      UTIL_Printf(DBLEVEL_M, (uint8_t*)"user_app_relay: ON Relay: ", sizeof("user_app_relay: ON Relay: "));
    }
    else
    {
      UTIL_Printf(DBLEVEL_M, (uint8_t*)"user_app_relay: OFF Relay: ", sizeof("user_app_relay: OFF Relay: "));
    }
    
    switch(KindRelay)
    {
        case RELAY_1:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"RELAY_1 ", sizeof("RELAY_1 ")); 
           break;
           
        case RELAY_2:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"RELAY_2 ", sizeof("RELAY_2 ")); 
           break;
           
        case RELAY_3:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"RELAY_3 ", sizeof("RELAY_3 ")); 
           break;
           
        case RELAY_4:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"RELAY_4 ", sizeof("RELAY_4 ")); 
           break;
           
        case RELAY_5:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"RELAY_5 ", sizeof("RELAY_5 ")); 
           break;
           
        case RELAY_6:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"RELAY_6 ", sizeof("RELAY_6 ")); 
           break;
           
        case RELAY_7:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"RELAY_7 ", sizeof("RELAY_7 ")); 
           break;
           
        case RELAY_8:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"RELAY_8 ", sizeof("RELAY_8 ")); 
           break;
           
        case RELAY_9:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"RELAY_9 ", sizeof("RELAY_9 ")); 
           break;

        default:
           break;
    }
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")); 
#endif
    }
}

uint8_t AppRelay_Task(void)
{
    uint8_t i = 0;
    uint8_t Result = false;
    for( i = 0; i < _EVENT_RELAY_END; i++)
    {
        if(sEventAppRelay[i].e_status == 1)
        {
            Result = true;
            if((sEventAppRelay[i].e_systick == 0) ||
                ((HAL_GetTick() - sEventAppRelay[i].e_systick) >= sEventAppRelay[i].e_period))
            {
                sEventAppRelay[i].e_status = 0; //Disable event
                sEventAppRelay[i].e_systick= HAL_GetTick();
                sEventAppRelay[i].e_function_handler(i);
            }
        }
    }
    
    return Result;
}

