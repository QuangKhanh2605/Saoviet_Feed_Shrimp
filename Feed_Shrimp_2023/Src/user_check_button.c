#include "user_check_button.h"

uint16_t BT_enter=0, BT_esc=0, BT_up=0, BT_down=0;

uint32_t stampTime1=0;
uint32_t stampTime2=0;
uint32_t stampTime3=0;
uint32_t stampThreshold_Relay1=0;
uint32_t stampThreshold_Relay2=0;
uint32_t *ptrStamp;

uint32_t load_flash=1;

uint16_t check_hold_esc=0;

void Check_BT_ENTER(uint16_t *State,uint16_t *checkState, uint16_t *setupCount,uint32_t *time1, uint32_t *time2, uint32_t *time3,
                    uint32_t *threshold_Relay1_Uint, uint32_t *threshold_Relay2_Uint )
{
	if(HAL_GPIO_ReadPin(GPIO_BT_ENTER, PIN_BT_ENTER)==0)
	{
		BT_enter=1;
	}
	if(BT_enter==1 && HAL_GPIO_ReadPin(GPIO_BT_ENTER, PIN_BT_ENTER)==1)
	{
		BT_enter=0;
		USER_LCD_Change_Setup();
		if (*State==0) *State=1;
		else 				  *State=0;
		
		if(*State==1)
		{
			*checkState=1;
			*time1=stampTime1;
			*time2=stampTime2;
			*time3=stampTime3;
			*threshold_Relay1_Uint=stampThreshold_Relay1;
			*threshold_Relay2_Uint=stampThreshold_Relay2;
			FLASH_WritePage(FLASH_USER_START_ADDR, FLASH_USER_END_ADDR, load_flash, *time1, *time2, *time3, stampThreshold_Relay1, stampThreshold_Relay2);
			*setupCount=1;
		}
		else
		{
			stampTime1=*time1;
			stampTime2=*time2;
			stampTime3=*time3;
			stampThreshold_Relay1 = *threshold_Relay1_Uint;
			stampThreshold_Relay2 = *threshold_Relay2_Uint;
			*setupCount=2;
			ptrStamp=&stampTime1;
		}
	}
}

void Check_BT_ESC(uint16_t State, uint16_t *setupCount)
{
	if(HAL_GPIO_ReadPin(GPIO_BT_ESC, PIN_BT_ESC)==0)
	{
		BT_esc=1;
	}
	if(BT_esc==1 && HAL_GPIO_ReadPin(GPIO_BT_ESC, PIN_BT_ESC)==1)
	{
		BT_esc=0;
		if(check_hold_esc==0)
		{
			USER_LCD_Change_Setup();	
			if(*setupCount==6) *setupCount=1;
			else 						   (*setupCount)++;
			
			if(*setupCount==4 ) *setupCount=5;
			if(*setupCount==5 ) *setupCount=6;
				
			if(State==0 && *setupCount==1) *setupCount=2;
			if(*setupCount==2 ) ptrStamp=&stampTime1;
			if(*setupCount==3 ) ptrStamp=&stampTime2;
			//if(*setupCount==4 ) ptrStamp=&stampTime3;
			//if(*setupCount==5 ) ptrStamp=&stampThreshold_Relay1;
			if(*setupCount==6 ) ptrStamp=&stampThreshold_Relay2;
		}
		else check_hold_esc=0;
	}
}

void Check_BT_UP(uint16_t State, uint16_t *stateWarning_Relay3)
{
	if (State==0)
	{
		if(HAL_GPIO_ReadPin(GPIO_BT_UP, PIN_BT_UP)==0 && BT_up==0)
		{
			BT_up=1;
		}
		if(HAL_GPIO_ReadPin(GPIO_BT_UP, PIN_BT_UP)==1)
		{
			BT_up=0;
		}
	}

	if(HAL_GPIO_ReadPin(GPIO_BT_UP, PIN_BT_UP)==0 )
	{
		*stateWarning_Relay3=0;
	}
}

void Check_BT_DOWN(uint16_t State, uint16_t *stateWarning_Relay3)
{
	if (State==0)
	{
		if(HAL_GPIO_ReadPin(GPIO_BT_DOWN, PIN_BT_DOWN)==0 && BT_down==0)
		{
			BT_down=1;
		}
		if(HAL_GPIO_ReadPin(GPIO_BT_DOWN, PIN_BT_DOWN)==1)
		{
			BT_down=0;
		}
	}
	
	if(HAL_GPIO_ReadPin(GPIO_BT_DOWN, PIN_BT_DOWN)==0 )
	{
		*stateWarning_Relay3=0;
	}
}

void BT_Check_Up_Down(uint16_t setupCount)
{
	BT_Press_Click_Up(&BT_up, ptrStamp);
	BT_Press_Click_Down(&BT_down, ptrStamp);
			
	BT_Press_Hold_Up(GPIO_BT_UP , PIN_BT_UP, ptrStamp);
	BT_Press_Hold_Down(GPIO_BT_DOWN, PIN_BT_DOWN, ptrStamp);
	
	LCD_Change_State_Setup_T1_T2_T3(stampTime1, stampTime2, stampTime3, stampThreshold_Relay1, stampThreshold_Relay2);
	UintTime_To_CharTime_T1_T2_T3(stampTime1, stampTime2, stampTime3, stampThreshold_Relay1, stampThreshold_Relay2);
}

void BT_Esc_Exit_Setup(uint16_t *State, uint16_t *setupCount, float ACS_Value_Float,uint32_t time1, uint32_t time2, uint32_t time3, 
	                                       uint32_t threshold_Relay1_Uint, uint32_t threshold_Relay2_Uint)
{
	BT_Press_Hold_Esc(GPIO_BT_ESC, PIN_BT_ESC, State, BT_up, BT_down);
	if(*State==1)
	{
		Run_Begin(*State, setupCount, ACS_Value_Float, time1, time2, time3, threshold_Relay1_Uint, threshold_Relay2_Uint);
		check_hold_esc=1;
	}
}

void Run_Begin(uint16_t State, uint16_t *setupCount, float ACS_Value_Float, uint32_t time1, uint32_t time2,uint32_t time3,
                                     uint32_t threshold_Relay1_Uint, uint32_t threshold_Relay2_Uint)
{
	stampTime1=time1;
	stampTime2=time2;
	stampTime3=time3;
	stampThreshold_Relay1 = threshold_Relay1_Uint;
	stampThreshold_Relay2 = threshold_Relay2_Uint;
	if(State==1) *setupCount=1;
	if(State==0) *setupCount=2;
	ptrStamp=&stampTime1;
	
	LCD_Change_State_Setup_T1_T2_T3(stampTime1, stampTime2, stampTime3, stampThreshold_Relay1, stampThreshold_Relay2);
	UintTime_To_CharTime_T1_T2_T3(stampTime1, stampTime2, stampTime3, stampThreshold_Relay1, stampThreshold_Relay2);
	Float_To_Char_ACS(ACS_Value_Float);
}

