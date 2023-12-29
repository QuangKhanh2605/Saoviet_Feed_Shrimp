#include "user_app_lcd.h"
#include "CLCD.h"
#include "user_app_menu_display.h"
#include "user_app_feed_shrimp.h"
#include "main.h"
/*=============== Function static ================*/
static uint8_t fevent_lcd_entry(uint8_t event);
static uint8_t fevent_lcd_get_time_rtc(uint8_t event);

/*================== Struct ===================*/
sEvent_struct       sEventAppLcd[]=
{
  {_EVENT_LCD_ENTRY,            1, 5, 0,        fevent_lcd_entry},
  {_EVENT_LCD_GET_TIME_RTC,     1, 5, 250,      fevent_lcd_get_time_rtc},
};

CLCD_Name LCD;

Struct_TimeRun                  sTimeRun={0,0,0};
/*===== Object LCD =============== MAIN Screen 1 =================*/
LCD_Char_Display                    oLCD_C_Main_1[]=
{
  {_LCD_C_MAIN_1_RUNNING,       0,0,0,          (char*)"Running "},
  {_LCD_C_MAIN_1_LEVEL,         0,1,0,          (char*)"Level:          "},
  {_LCD_C_MAIN_1_RUN,           0,1,0,          (char*)"Ban  :      Giay"},
  {_LCD_C_MAIN_1_FREE,          0,1,0,          (char*)"Nghi :      Phut"},
  {_LCD_C_MAIN_1_CYCLE,         0,1,0,          (char*)"Cycle:      Giay"}
};

LCD_Uint_Display                    oLCD_U_Main_1[]=
{
  {_LCD_U_MAIN_1_HOUR,          0,0,8,          &sTimeRun.Hour},
  {_LCD_U_MAIN_1_MINUTES,       0,0,11,         &sTimeRun.Minutes},
  {_LCD_U_MAIN_1_SECOND,        0,0,14,         &sTimeRun.Second},
  {_LCD_U_MAIN_1_LEVEL,         0,1,7,          &sParamFeed.Level},
  {_LCD_U_MAIN_1_RUN,           0,1,7,          &sParamFeed.TimeRun},
  {_LCD_U_MAIN_1_FREE,          0,1,7,          &sParamFeed.TimeFree},
  {_LCD_U_MAIN_1_CYCLE,         0,1,7,          &sParamFeed.TimeCycle},
};

LCD_Char_Display                    oLCD_C_Main_2[]=
{
  {_LCD_C_MAIN_2_SETUP,         0,0,0,          (char*)"Cai dat "},
  {_LCD_C_MAIN_2_RUN,           0,1,0,          (char*)"Ban  :      Giay"},
  {_LCD_C_MAIN_2_FREE,          0,1,0,          (char*)"Nghi :      Phut"},
  {_LCD_C_MAIN_2_CYCLE,         0,1,0,          (char*)"Cycle:      Giay"},
};

LCD_Uint_Display                    oLCD_U_Main_2[]=
{
  {_LCD_U_MAIN_2_HOUR,          0,0,8,          (uint16_t*)&sTimeRun.Hour},
  {_LCD_U_MAIN_2_MINUTES,       0,0,11,         (uint16_t*)&sTimeRun.Minutes},
  {_LCD_U_MAIN_2_SECOND,        0,0,14,         (uint16_t*)&sTimeRun.Second},
  {_LCD_U_MAIN_2_RUN,           0,1,7,          &sMenuStamp.TimeRun},
  {_LCD_U_MAIN_2_FREE,          0,1,7,          &sMenuStamp.TimeFree},
  {_LCD_U_MAIN_2_CYCLE,         0,1,7,          &sMenuStamp.TimeCycle}
};
/*=============================== Function Handle ===============================*/


static uint8_t fevent_lcd_entry(uint8_t event)
{
    CLCD_SetCursor(&LCD, 0, 0);
    CLCD_WriteString(&LCD, (char*)"          :  :  ");
    return 1;
}
static uint8_t fevent_lcd_get_time_rtc(uint8_t event)
{
    return 1;
}
/*================================================================================*/

void        DLCD_Main_1(void)
{
    for(uint8_t i = 0; i < _LCD_C_MAIN_1_END; i++)
    {
        if(oLCD_C_Main_1[i].Status == 1)
        {
            CLCD_SetCursor(&LCD, oLCD_C_Main_1[i].Col,oLCD_C_Main_1[i].Rol);
            CLCD_WriteString(&LCD, oLCD_C_Main_1[i].Object);
            oLCD_C_Main_1[i].Status = 0;
        }
    }
    
    for(uint8_t i = 0; i < _LCD_U_MAIN_1_END; i++)
    {
        if(oLCD_U_Main_1[i].Status == 1)
        {
            if(i == _LCD_U_MAIN_1_HOUR ||
               i == _LCD_U_MAIN_1_MINUTES ||
               i == _LCD_U_MAIN_1_SECOND)
            {
                LCD_Display_Uint_Time((uint32_t)*oLCD_U_Main_1[i].Object, oLCD_U_Main_1[i].Col, oLCD_U_Main_1[i].Rol);
            }
            else
            {
                LCD_Display_Uint((uint32_t)*oLCD_U_Main_1[i].Object, oLCD_U_Main_1[i].Col, oLCD_U_Main_1[i].Rol);
            }
            oLCD_U_Main_1[i].Status = 0;
        }
    }
}

void        DLCD_Main_1_Entry(void)
{
    oLCD_C_Main_1[_LCD_C_MAIN_1_RUNNING].Status = 1;
    oLCD_C_Main_1[_LCD_C_MAIN_1_LEVEL].Status = 1;
    
    oLCD_U_Main_1[_LCD_U_MAIN_1_HOUR].Status = 1;
    oLCD_U_Main_1[_LCD_U_MAIN_1_MINUTES].Status = 1;
    oLCD_U_Main_1[_LCD_U_MAIN_1_SECOND].Status = 1;
    oLCD_U_Main_1[_LCD_U_MAIN_1_LEVEL].Status = 1;
    DLCD_Main_1();
}



void        DLCD_Main_2(void)
{
    for(uint8_t i = 0; i < _LCD_C_MAIN_2_END; i++)
    {
        if(oLCD_C_Main_2[i].Status == 1)
        {
            CLCD_SetCursor(&LCD, oLCD_C_Main_2[i].Col,oLCD_C_Main_2[i].Rol);
            CLCD_WriteString(&LCD, oLCD_C_Main_2[i].Object);
            oLCD_C_Main_2[i].Status = 0;
        }
    }
    
    for(uint8_t i = 0; i < _LCD_U_MAIN_2_END; i++)
    {
        if(oLCD_U_Main_2[i].Status == 1)
        {
            if(i == _LCD_U_MAIN_2_HOUR ||
               i == _LCD_U_MAIN_2_MINUTES ||
               i == _LCD_U_MAIN_2_SECOND)
            {
                LCD_Display_Uint_Time((uint32_t)*oLCD_U_Main_2[i].Object, oLCD_U_Main_2[i].Col, oLCD_U_Main_2[i].Rol);
            }
            else
            {
                LCD_Display_Uint((uint32_t)*oLCD_U_Main_2[i].Object, oLCD_U_Main_2[i].Col, oLCD_U_Main_2[i].Rol);
            }
            oLCD_U_Main_2[i].Status = 0;
        }
    }
}

void        DLCD_Main_2_Entry(void)
{
    oLCD_C_Main_2[_LCD_C_MAIN_2_SETUP].Status = 1;
    oLCD_C_Main_2[_LCD_C_MAIN_2_RUN].Status = 1;
    
    oLCD_U_Main_2[_LCD_U_MAIN_2_HOUR].Status = 1;
    oLCD_U_Main_2[_LCD_U_MAIN_2_MINUTES].Status = 1;
    oLCD_U_Main_2[_LCD_U_MAIN_2_SECOND].Status = 1;
    oLCD_U_Main_2[_LCD_U_MAIN_2_RUN].Status = 1;
    DLCD_Main_2();
}

void Main_1_Level_Alter(void)
{
    static uint16_t stamp = 0;
    
    if(stamp != *oLCD_U_Main_1[_LCD_U_MAIN_1_LEVEL].Object)
    {
        oLCD_U_Main_1[_LCD_U_MAIN_1_LEVEL].Status = 1;
        stamp = *oLCD_U_Main_1[_LCD_U_MAIN_1_LEVEL].Object;
    }
    
    DLCD_Main_1();
}

void Main_1_Time_Alter(void)
{
    static uint8_t stampHH = 0;
    static uint8_t stampMM = 0;
    static uint8_t stampSS = 0;
    
    if(stampHH != *oLCD_U_Main_1[_LCD_U_MAIN_1_HOUR].Object)
    {
        oLCD_U_Main_1[_LCD_U_MAIN_1_HOUR].Status = 1;
        stampHH = *oLCD_U_Main_1[_LCD_U_MAIN_1_HOUR].Object;
    }
    
    if(stampMM != *oLCD_U_Main_1[_LCD_U_MAIN_1_MINUTES].Object)
    {
        oLCD_U_Main_1[_LCD_U_MAIN_1_MINUTES].Status = 1;
        stampMM = *oLCD_U_Main_1[_LCD_U_MAIN_1_MINUTES].Object;
    }
    
    if(stampSS != *oLCD_U_Main_1[_LCD_U_MAIN_1_SECOND].Object)
    {
        oLCD_U_Main_1[_LCD_U_MAIN_1_SECOND].Status = 1;
        stampSS = *oLCD_U_Main_1[_LCD_U_MAIN_1_SECOND].Object;
    }
    
    DLCD_Main_1();
}

void Main_2_Time_Alter(void)
{
    static uint8_t stampHH = 0;
    static uint8_t stampMM = 0;
    static uint8_t stampSS = 0;
    
    if(stampHH != *oLCD_U_Main_2[_LCD_U_MAIN_2_HOUR].Object)
    {
        oLCD_U_Main_2[_LCD_U_MAIN_2_HOUR].Status = 1;
        stampHH = *oLCD_U_Main_2[_LCD_U_MAIN_2_HOUR].Object;
    }
    
    if(stampMM != *oLCD_U_Main_2[_LCD_U_MAIN_2_MINUTES].Object)
    {
        oLCD_U_Main_2[_LCD_U_MAIN_2_MINUTES].Status = 1;
        stampMM = *oLCD_U_Main_2[_LCD_U_MAIN_2_MINUTES].Object;
    }
    
    if(stampSS != *oLCD_U_Main_2[_LCD_U_MAIN_2_SECOND].Object)
    {
        oLCD_U_Main_2[_LCD_U_MAIN_2_SECOND].Status = 1;
        stampSS = *oLCD_U_Main_2[_LCD_U_MAIN_2_SECOND].Object;
    }
    
    DLCD_Main_2();
}

void Main_2_Run_Alter(void)
{
    static uint8_t stampRun = 0;
    
    if(stampRun != *oLCD_U_Main_2[_LCD_U_MAIN_2_RUN].Object)
    {
        oLCD_U_Main_2[_LCD_U_MAIN_2_RUN].Status = 1;
        stampRun = *oLCD_U_Main_2[_LCD_U_MAIN_2_RUN].Object;
    }
    
    DLCD_Main_2();
}

void Main_2_Free_Alter(void)
{
    static uint8_t stamp = 0;
    
    if(stamp != *oLCD_U_Main_2[_LCD_U_MAIN_2_FREE].Object)
    {
        oLCD_U_Main_2[_LCD_U_MAIN_2_FREE].Status = 1;
        stamp = *oLCD_U_Main_2[_LCD_U_MAIN_2_FREE].Object;
    }
    
    DLCD_Main_2();
}

void Main_2_Cycle_Alter(void)
{
    static uint8_t stamp = 0;
    
    if(stamp != *oLCD_U_Main_2[_LCD_U_MAIN_2_CYCLE].Object)
    {
        oLCD_U_Main_2[_LCD_U_MAIN_2_CYCLE].Status = 1;
        stamp = *oLCD_U_Main_2[_LCD_U_MAIN_2_CYCLE].Object;
    }
    
    DLCD_Main_2();
}

void Init_LCD16x2(void)
{
    CLCD_4BIT_Init(&LCD, 16,2, MCU_LCD_RS_PORT, MCU_LCD_RS_PIN,MCU_LCD_EN_PORT, MCU_LCD_EN_PIN,
                               MCU_LCD_D4_PORT, MCU_LCD_D4_PIN,MCU_LCD_D5_PORT, MCU_LCD_D5_PIN,
                               MCU_LCD_D6_PORT, MCU_LCD_D6_PIN,MCU_LCD_D7_PORT, MCU_LCD_D7_PIN);
}

void Set_Time(uint32_t Time)
{
	const uint16_t MINUTES_OF_THE_HOUR=60;
	const uint16_t SECOND_OF_THE_HOUR=60;
	const uint32_t SECOND_OF_THE_DAY=86400;
    
    uint32_t TimeRun = 0;

    while(TimeRun>SECOND_OF_THE_DAY)
    {
        TimeRun = TimeRun/SECOND_OF_THE_DAY;
    }
    
    TimeRun = Time%SECOND_OF_THE_DAY;

	sTimeRun.Hour=TimeRun/(MINUTES_OF_THE_HOUR*SECOND_OF_THE_HOUR);
	sTimeRun.Minutes=(TimeRun-(sTimeRun.Hour)*MINUTES_OF_THE_HOUR*SECOND_OF_THE_HOUR)/SECOND_OF_THE_HOUR;
	sTimeRun.Second=(TimeRun-(sTimeRun.Hour)*MINUTES_OF_THE_HOUR*SECOND_OF_THE_HOUR-(sTimeRun.Minutes)*SECOND_OF_THE_HOUR);
}

void LCD_Display_Uint(uint32_t Var, uint8_t rol, uint8_t col)
{
    uint8_t aData[9]={0};
	uint16_t lengthStamp=1;
	uint32_t Division=10;
	while(Var/Division>=1)
	{
		Division=Division*10;
		(lengthStamp)++;
	}
	
	for(int j=lengthStamp-1;j>=0;j--)
	{
	aData[j]=Var%10+0x30;
	Var=Var/10;
	}
	aData[lengthStamp]=0x20;
    aData[lengthStamp++]=0x20;
    CLCD_SetCursor(&LCD, rol, col);
    CLCD_WriteString(&LCD, (char*)aData);
}

void LCD_Display_Uint_Time(uint32_t Var, uint8_t rol, uint8_t col)
{
    uint8_t aData[2]={0};
    if(Var >= 10)
    {
        aData[0] = Var/10 + 0x30;
        aData[1] = Var%10 + 0x30;
    }
    else
    {
        aData[0] = 0x30;
        aData[1] = Var + 0x30;
    }

    CLCD_SetCursor(&LCD, rol, col);
    CLCD_WriteString(&LCD, (char*)aData);
}

void Init_AppLcd(void)
{
    Init_LCD16x2();
}

uint8_t AppLcd_Task(void)
{
    uint8_t i = 0;
    uint8_t Result =  false;
    
    for(i = 0; i < _EVENT_LCD_END; i++)
    {
        if(sEventAppLcd[i].e_status == 1)
        {
            Result = true; 
            
            if((sEventAppLcd[i].e_systick == 0) ||
               ((HAL_GetTick() - sEventAppLcd[i].e_systick) >= sEventAppLcd[i].e_period))
            {
                sEventAppLcd[i].e_status = 0; //Disable event
                sEventAppLcd[i].e_systick= HAL_GetTick();
                sEventAppLcd[i].e_function_handler(i);
            }
        }
    }
    
    
    return Result;
}


