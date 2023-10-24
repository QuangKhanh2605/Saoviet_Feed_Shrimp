
#ifndef USER_APP_LCD__
#define USER_APP_LCD__

#define USING_APP_LCD

#include "event_driven.h"
#include "user_util.h"

#include "CLCD.h"

#define MCU_LCD_RS_PORT         MCU_RS_GPIO_Port
#define MCU_LCD_EN_PORT         MCU_EN_GPIO_Port
#define MCU_LCD_D7_PORT         LCD_D7_GPIO_Port
#define MCU_LCD_D6_PORT         LCD_D6_GPIO_Port
#define MCU_LCD_D5_PORT         LCD_D5_GPIO_Port
#define MCU_LCD_D4_PORT         LCD_D4_GPIO_Port

#define MCU_LCD_RS_PIN          MCU_RS_Pin
#define MCU_LCD_EN_PIN          MCU_EN_Pin
#define MCU_LCD_D7_PIN          LCD_D7_Pin
#define MCU_LCD_D6_PIN          LCD_D6_Pin
#define MCU_LCD_D5_PIN          LCD_D5_Pin
#define MCU_LCD_D4_PIN          LCD_D4_Pin


typedef enum
{
    _EVENT_LCD_ENTRY,
    _EVENT_LCD_GET_TIME_RTC,
    
    _EVENT_LCD_END,
}eKindEventLcd;

typedef struct
{
    uint8_t  e_name;
    uint8_t Status;
    uint8_t Rol;
	uint8_t Col;
    char *Object;
}LCD_Char_Display;

typedef struct
{
    uint8_t e_name;
    uint8_t  Status;
    uint8_t  Rol;
    uint8_t  Col;
    uint16_t *Object;
}LCD_Uint_Display;

typedef struct
{
    uint8_t e_name;
    uint8_t Status;
    uint8_t Rol;
    uint8_t Col;
    uint8_t Object;
}LCD_Uint_Icon;


typedef enum
{
    _LCD_C_MAIN_1_RUNNING,
    _LCD_C_MAIN_1_LEVEL,
    _LCD_C_MAIN_1_RUN,
    _LCD_C_MAIN_1_FREE,
    _LCD_C_MAIN_1_CYCLE,

    _LCD_C_MAIN_1_END,
}eKindLcd_C_Main_1;

typedef enum
{
    _LCD_U_MAIN_1_HOUR,
    _LCD_U_MAIN_1_MINUTES,
    _LCD_U_MAIN_1_SECOND,
    _LCD_U_MAIN_1_LEVEL,
    _LCD_U_MAIN_1_RUN,
    _LCD_U_MAIN_1_FREE,
    _LCD_U_MAIN_1_CYCLE,
    
    _LCD_U_MAIN_1_END,
}eKindLcd_U_Main_1;



typedef enum
{
    _LCD_C_MAIN_2_SETUP,
    _LCD_C_MAIN_2_RUN,
    _LCD_C_MAIN_2_FREE,
    _LCD_C_MAIN_2_CYCLE,

    _LCD_C_MAIN_2_END,
}eKindLcd_C_Main_2;

typedef enum
{
    _LCD_U_MAIN_2_HOUR,
    _LCD_U_MAIN_2_MINUTES,
    _LCD_U_MAIN_2_SECOND,
    _LCD_U_MAIN_2_RUN,
    _LCD_U_MAIN_2_FREE,
    _LCD_U_MAIN_2_CYCLE,
    
    _LCD_U_MAIN_2_END,
}eKindLcd_U_Main_2;

typedef struct
{
    uint16_t Hour;
    uint16_t Minutes;
    uint16_t Second;
}Struct_TimeRun;


extern sEvent_struct       sEventAppVibSensor[];
extern LCD_Uint_Display                    oLCD_U_Main_1[];
extern LCD_Char_Display                    oLCD_C_Main_1[];
extern LCD_Uint_Display                    oLCD_U_Main_2[];
extern LCD_Char_Display                    oLCD_C_Main_2[];

/*================= Function Handle ================*/
uint8_t     AppLcd_Task(void);
void        Init_AppLcd(void);

void        Init_LCD16x2(void);
void        Set_Time(uint32_t Time);

void        LCD_Display_Uint(uint32_t Var, uint8_t rol, uint8_t col);
void        LCD_Display_Uint_Time(uint32_t Var, uint8_t rol, uint8_t col);

void        DLCD_Main_1(void);
void        DLCD_Main_1_Entry(void);

void        DLCD_Main_2(void);
void        DLCD_Main_2_Entry(void);

void        Main_1_Time_Alter(void);
void        Main_1_Level_Alter(void);

void        Main_2_Time_Alter(void);
void        Main_2_Run_Alter(void);
void        Main_2_Free_Alter(void);
void        Main_2_Cycle_Alter(void);

#endif

