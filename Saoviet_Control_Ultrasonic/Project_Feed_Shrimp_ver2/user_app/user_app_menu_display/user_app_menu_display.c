#include "user_app_menu_display.h"
#include "user_app_lcd.h"
#include "user_internal_mem.h"
#include "user_app_feed_shrimp.h"
/*=============== Function static ================*/
static uint8_t fevent_menu_entry(uint8_t event);
static uint8_t fevent_menu_display_alter(uint8_t event);
/*================== Struct ===================*/
sEvent_struct       sEventAppMenu[]=
{
  {_EVENT_MENU_ENTRY,               1, 5, 5,                fevent_menu_entry},
  {_EVENT_MENU_DISPLAY_ALTER,       1, 5, 20,               fevent_menu_display_alter},
};

Struct_Menu_State               sMenuState={0,0,0};
Struct_Menu_Stamp               sMenuStamp={0};
/*================== Function Handle ===================*/
static uint8_t fevent_menu_entry(uint8_t event)
{
    if(sParamFeed.Power == _ON_FEED)
    {
        DLCD_Main_1_Entry();
        sMenuState.Screen = _MAIN_1;
    }
    else
    {
        DLCD_Main_2_Entry();
        sMenuState.Screen = _MAIN_2;
    }
    return 1;
}

static uint8_t fevent_menu_display_alter(uint8_t event)
{
    switch(sMenuState.Screen)
    {
        case _MAIN_1:
            Main_1_Time_Alter();
            if(sMenuState.Main_1 == _MAIN_1_LEVEL)
            {
                  Main_1_Level_Alter();
            }
            break;
            
        case _MAIN_2:
            Main_2_Time_Alter();
            if(sMenuState.Main_2 == _MAIN_2_RUN)
            {
                 Main_2_Run_Alter();
            }
            else if(sMenuState.Main_2 == _MAIN_2_FREE)
            {
                 Main_2_Free_Alter();
            }
            else
            {
                 Main_2_Cycle_Alter();
            }
            break;

        default:
          break;
    }
    fevent_enable(sEventAppMenu, event);
    return 1;
}

 
/*====================== Function Use Button Click =======================*/
void User_Button_Enter_Click(void)
{
    Handle_Menu_Button(_BT_ENTER_CLICK);
}

void User_Button_Exit_Click(void)
{
    Handle_Menu_Button(_BT_EXIT_CLICK);
}

void User_Button_Up_Click(void)
{
    Handle_Menu_Button(_BT_UP_CLICK);
}

void User_Button_Down_Click(void)
{
    Handle_Menu_Button(_BT_DOWN_CLICK);
}
/*====================== Function Use Button Hold Cycle =======================*/
void User_Button_Enter_Hold_Cycle(void)
{
    Handle_Menu_Button(_BT_ENTER_HOLD_CYCLE);
}

void User_Button_Exit_Hold_Cycle(void)
{
    Handle_Menu_Button(_BT_EXIT_HOLD_CYCLE);
}

void User_Button_Up_Hold_Cycle(void)
{
    Handle_Menu_Button(_BT_UP_HOLD_CYCLE);
}

void User_Button_Down_Hold_Cycle(void)
{
    Handle_Menu_Button(_BT_DOWN_HOLD_CYCLE);
}
/*====================== Function Use Button Hold Once =======================*/
void User_Button_Enter_Hold_Once(void)
{
    Handle_Menu_Button(_BT_ENTER_HOLD_ONCE);
}

void User_Button_Exit_Hold_Once(void)
{
    Handle_Menu_Button(_BT_EXIT_HOLD_ONCE);
}

void User_Button_Up_Hold_Once(void)
{
    Handle_Menu_Button(_BT_UP_HOLD_ONCE);
}

void User_Button_Down_Hold_Once(void)
{
    Handle_Menu_Button(_BT_DOWN_HOLD_ONCE);
}
/*========================= Function Handle Menu =======================*/
void    BT_Menu_Main_1(uint8_t KindButton)
{
    switch(KindButton)
    {
        case _BT_ENTER_CLICK:
          sMenuState.Screen = _MAIN_2;
          DLCD_Main_2_Entry();
          Menu_ResetStamp();
          sParamFeed.Power = _OFF_FEED;
          Menu_ResetState();
          break;
          
        case _BT_ENTER_HOLD_CYCLE:
          break;
          
        case _BT_EXIT_CLICK:
          switch(sMenuState.Main_1)
          {
            case _MAIN_1_LEVEL:
                sMenuState.Main_1 = _MAIN_1_RUN;
                oLCD_C_Main_1[_LCD_C_MAIN_1_RUN].Status = 1;
                oLCD_U_Main_1[_LCD_U_MAIN_1_RUN].Status = 1;
                DLCD_Main_1();
                break;

            case _MAIN_1_RUN:
                sMenuState.Main_1 = _MAIN_1_FREE;
                oLCD_C_Main_1[_LCD_C_MAIN_1_FREE].Status = 1;
                oLCD_U_Main_1[_LCD_U_MAIN_1_FREE].Status = 1;
                DLCD_Main_1();
                break;
                
            case _MAIN_1_FREE:
                sMenuState.Main_1 = _MAIN_1_CYCLE;
                oLCD_C_Main_1[_LCD_C_MAIN_1_CYCLE].Status = 1;
                oLCD_U_Main_1[_LCD_U_MAIN_1_CYCLE].Status = 1;
                DLCD_Main_1();
                break;
                
            case _MAIN_1_CYCLE:
                sMenuState.Main_1 = _MAIN_1_LEVEL;
                oLCD_C_Main_1[_LCD_C_MAIN_1_LEVEL].Status = 1;
//                oLCD_U_Main_1[_LCD_U_MAIN_1_LEVEL].Status = 1;
                DLCD_Main_1();
                break;
            
              default:
                break;
          }
          break;
        case _BT_EXIT_HOLD_CYCLE:
          break;
          
          
        case _BT_UP_CLICK:
        case _BT_UP_HOLD_CYCLE:
          if(sMenuState.Main_1 == _MAIN_1_LEVEL) 
          {
            if(sParamFeed.Level < LEVEL_MAX) sParamFeed.Level++;
          }
          break; 
          
        case _BT_DOWN_CLICK:
        case _BT_DOWN_HOLD_CYCLE:
          if(sMenuState.Main_1 == _MAIN_1_LEVEL) 
          {
            if(sParamFeed.Level > LEVEL_MIN) sParamFeed.Level--;
          }
          break; 
    
        case _BT_ENTER_HOLD_ONCE:
        case _BT_EXIT_HOLD_ONCE:
        case _BT_UP_HOLD_ONCE:
        case _BT_DOWN_HOLD_ONCE:
          break;
        
        default:
          break;
    }
}

void    BT_Menu_Main_2(uint8_t KindButton)
{
    switch(KindButton)
    {
        case _BT_ENTER_CLICK:
          sMenuState.Screen = _MAIN_1;
          DLCD_Main_1_Entry();
          Save_TimeFeed(sMenuStamp.TimeRun, sMenuStamp.TimeFree, sMenuStamp.TimeCycle);
          Menu_ResetStamp();
          sParamFeed.Power = _ON_FEED;
          Menu_ResetState();
          break;
          
        case _BT_ENTER_HOLD_CYCLE:
          break;
          
        case _BT_EXIT_CLICK:
          switch(sMenuState.Main_2)
          {
            case _MAIN_2_RUN:
                sMenuState.Main_2 = _MAIN_2_FREE;
                oLCD_C_Main_2[_LCD_C_MAIN_2_FREE].Status = 1;
                oLCD_U_Main_2[_LCD_U_MAIN_2_FREE].Status = 1;
                DLCD_Main_2();
                break;
                
            case _MAIN_2_FREE:
                sMenuState.Main_2 = _MAIN_2_CYCLE;
                oLCD_C_Main_2[_LCD_C_MAIN_2_CYCLE].Status = 1;
                oLCD_U_Main_2[_LCD_U_MAIN_2_CYCLE].Status = 1;
                DLCD_Main_2();
                break;
                
            case _MAIN_2_CYCLE:
                sMenuState.Main_2 = _MAIN_2_RUN;
                oLCD_C_Main_2[_LCD_C_MAIN_2_RUN].Status = 1;
                oLCD_U_Main_2[_LCD_U_MAIN_2_RUN].Status = 1;
                DLCD_Main_2();
                break;
            
              default:
                break;
          }
            break;
        case _BT_EXIT_HOLD_CYCLE:
          break;
          
          
        case _BT_UP_CLICK:
        case _BT_UP_HOLD_CYCLE:
          switch(sMenuState.Main_2)
          {
              case _MAIN_2_FREE:
                if(sMenuStamp.TimeFree < FREE_MAX) sMenuStamp.TimeFree++;
                break;
                
              case _MAIN_2_RUN:
                if(sMenuStamp.TimeRun < RUN_MAX) sMenuStamp.TimeRun++;
                break;
                
              case _MAIN_2_CYCLE:
                if(sMenuStamp.TimeCycle < CYCLE_MAX) sMenuStamp.TimeCycle++;
                break;
            
              default:
                break;
          }
          break;
          
        case _BT_DOWN_CLICK:
        case _BT_DOWN_HOLD_CYCLE:
          switch(sMenuState.Main_2)
          {
              case _MAIN_2_FREE:
                if(sMenuStamp.TimeFree > FREE_MIN) sMenuStamp.TimeFree--;
                break;
                
              case _MAIN_2_RUN:
                if(sMenuStamp.TimeRun > RUN_MIN) sMenuStamp.TimeRun--;
                break;
                
              case _MAIN_2_CYCLE:
                if(sMenuStamp.TimeCycle > CYCLE_MIN) sMenuStamp.TimeCycle--;
                break;
            
              default:
                break;
          }
          break; 
    
        case _BT_ENTER_HOLD_ONCE:
        case _BT_EXIT_HOLD_ONCE:
          sMenuState.Screen = _MAIN_1;
          DLCD_Main_1_Entry();
          Menu_ResetStamp();
          sParamFeed.Power = _ON_FEED;
          Menu_ResetState();
          break;
          
        case _BT_UP_HOLD_ONCE:
        case _BT_DOWN_HOLD_ONCE:
          break;
        
        default:
          break;
    }
}


/*========================  Function Handle Menu =======================*/
void    Handle_Menu_Button(uint8_t KindButton)
{
    switch(sMenuState.Screen)
    {
        case _MAIN_1:
          BT_Menu_Main_1(KindButton);
          break;
          
        case _MAIN_2:
          BT_Menu_Main_2(KindButton);
          break;
          
        default:
          break;
    }
}
/*=========================  Function Handle LCD =======================*/

/*============================ Function Handle =========================*/
void Menu_ResetStamp(void)
{
    sMenuStamp.TimeFree  = sParamFeed.TimeFree;
    sMenuStamp.TimeRun   = sParamFeed.TimeRun;
    sMenuStamp.TimeCycle = sParamFeed.TimeCycle;
}

void        Menu_ResetState(void)
{
    sMenuState.Main_1 = 0;
    sMenuState.Main_2 = 0;
}
/*==========================Function Handle=========================*/
void Init_AppMenuDisplay(void)
{

}

uint8_t AppMenu_Task(void)
{
    uint8_t i = 0;
    uint8_t Result =  false;
    
    for(i = 0; i < _EVENT_MENU_END; i++)
    {
        if(sEventAppMenu[i].e_status == 1)
        {
            Result = true; 
            
            if((sEventAppMenu[i].e_systick == 0) ||
               ((HAL_GetTick() - sEventAppMenu[i].e_systick) >= sEventAppMenu[i].e_period))
            {
                sEventAppMenu[i].e_status = 0; //Disable event
                sEventAppMenu[i].e_systick= HAL_GetTick();
                sEventAppMenu[i].e_function_handler(i);
            }
        }
    }
    
    
    return Result;
}

