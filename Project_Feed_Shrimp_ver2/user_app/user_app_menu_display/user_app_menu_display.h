

#ifndef USER_APP_MENU_DISPLAY_H__
#define USER_APP_MENU_DISPLAY_H__

#define USING_APP_MENU_DISPLAY


#include "event_driven.h"
#include "user_util.h"


#define TIME_BACK_MAIN_1        60000


typedef enum
{
    _EVENT_MENU_ENTRY,
    _EVENT_MENU_DISPLAY_ALTER,

    _EVENT_MENU_END,
}eKindEventMenu;

typedef enum
{
    _BT_ENTER_CLICK,
    _BT_ENTER_HOLD_CYCLE,
    _BT_EXIT_CLICK,
    _BT_EXIT_HOLD_CYCLE,
    _BT_UP_CLICK,
    _BT_UP_HOLD_CYCLE,
    _BT_DOWN_CLICK,
    _BT_DOWN_HOLD_CYCLE,
    
    _BT_ENTER_HOLD_ONCE,
    _BT_EXIT_HOLD_ONCE,
    _BT_UP_HOLD_ONCE,
    _BT_DOWN_HOLD_ONCE,
}eKindButtonMenu;

typedef struct
{
    uint8_t Screen;
    uint8_t Main_1;
    uint8_t Main_2;
}Struct_Menu_State;

typedef struct
{
    uint16_t TimeRun;
    uint16_t TimeFree;
    uint16_t TimeCycle;
}Struct_Menu_Stamp;



typedef enum
{
    _MAIN_1,
    _MAIN_2,
}eKindMain1State;


typedef enum
{
    _MAIN_1_LEVEL,
    _MAIN_1_RUN,
    _MAIN_1_FREE,
    _MAIN_1_CYCLE,
}eKindMain_1;

typedef enum
{
    _MAIN_2_RUN,
    _MAIN_2_FREE,
    _MAIN_2_CYCLE,
}eKindMain_2;


extern sEvent_struct        sEventAppMenu[];
extern Struct_Menu_State               sMenuState;
extern Struct_Menu_Stamp               sMenuStamp;
/*================= Function Handle ================*/
uint8_t     AppMenu_Task(void);
void        Init_AppMenuDisplay(void);
void        Menu_ResetStamp(void);
void        Menu_ResetState(void);
void        Save_Password(void);
void        Init_Password(void);
uint8_t     Check_Password(void);
void        Stamp_Menu_Exit(void);


void        User_Button_Enter_Click(void);
void        User_Button_Exit_Click(void);
void        User_Button_Up_Click(void);
void        User_Button_Down_Click(void);

void        User_Button_Enter_Hold_Cycle(void);
void        User_Button_Exit_Hold_Cycle(void);
void        User_Button_Up_Hold_Cycle(void);
void        User_Button_Down_Hold_Cycle(void);

void        User_Button_Enter_Hold_Once(void);
void        User_Button_Exit_Hold_Once(void);
void        User_Button_Up_Hold_Once(void);
void        User_Button_Down_Hold_Once(void);

void        Handle_Menu_Button(uint8_t KindButton);
void        BT_Menu_Main_1(uint8_t KindButton);
void        BT_Menu_Main_2(uint8_t KindButton);




#endif
