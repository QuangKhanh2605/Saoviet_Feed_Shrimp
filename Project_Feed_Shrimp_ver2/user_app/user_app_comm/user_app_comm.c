#include "user_app_comm.h"
#include "user_app_lcd.h"
#include "user_app_relay.h"
#include "user_app_button.h"
#include "user_app_menu_display.h"
#include "user_app_feed_shrimp.h"
/*=========================== Func App Main ========================*/

void SysApp_Init (void)
{
    Init_AppLcd();
    Init_AppMenuDisplay();
    Init_AppFeed();
}

void SysApp_Setting (void)
{
    AppComm_Init();
}

/*
    Func: Init Queue trong main
*/
uint8_t TaskStatus_u8 = 0;
void Main_Task (void)
{
  SysApp_Init();
  SysApp_Setting();
  HAL_Delay(20);
  for(;;)
  {
#ifdef USING_APP_LCD
    AppLcd_Task();
#endif
    
#ifdef USING_APP_RELAY
    AppRelay_Task();
#endif
    
#ifdef USING_APP_BUTTON
    AppButton_Task();
#endif
        
#ifdef USING_APP_MENU_DISPLAY
    AppMenu_Task();
#endif
    
#ifdef USING_APP_FEED
    AppFeed_Task();
#endif
  }
}


/*============= Function App Main ===============*/

void AppComm_Init (void)
{

}




