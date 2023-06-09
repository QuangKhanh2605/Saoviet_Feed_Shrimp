#include "user_LCD_object.h"

uint32_t check_time1=0;
uint32_t check_time2=0;
uint32_t check_time3=0;
uint32_t check_thresholdRelay1=0;
uint32_t check_thresholdRelay2=0;
uint16_t check_Warning=0;

float thresholdRelay1_Float;
float thresholdRelay2_Float;
float check_ACS=0;

uint32_t check_SS=0;
uint16_t check_MM=0;
uint16_t check_HH=0;

char HH[3],MM[3],SS[3];

char stamp_time1[9];
char stamp_time2[9];
char stamp_time3[9];
char stamp_thresholdRelay1[9];
char stamp_thresholdRelay2[9];
char char_ACS[9];

LCD_Object_Display LCD_Running={"Running ",0,0,0};
LCD_Object_Display LCD_Setup={"Cai dat ",0,0,0};
LCD_Object_Display LCD_Warning={"   Cap nguon!   ",0,0,0};
LCD_Object_Display LCD_Time={"00:00:00",8,0,0};

LCD_Object_Display LCD_SetupT1={"Ban :       Giay",0,1,1};
LCD_Object_Display LCD_SetupT2={"Nghi:       Phut",0,1,1};
LCD_Object_Display LCD_SetupT3={"Time3:      Giay",0,1,1};

LCD_Object_Display LCD_ThresholRelay1_Unit={"Vcc:        Ampe",0,1,1};
LCD_Object_Display LCD_ThresholRelay2_Unit={"Cam :       Ampe",0,1,1};

LCD_Object_Display LCD_Time_HH={HH,8,0,1};
LCD_Object_Display LCD_Time_MM={MM,11,0,1};
LCD_Object_Display LCD_Time_SS={SS,14,0,1};

LCD_Object_Display LCD_Time1={stamp_time1,6,1,1};
LCD_Object_Display LCD_Time2={stamp_time2,6,1,1};
LCD_Object_Display LCD_Time3={stamp_time3,6,1,1};

LCD_Object_Display LCD_ThresholRelay1={stamp_thresholdRelay1,6,1,1};
LCD_Object_Display LCD_ThresholRelay2={stamp_thresholdRelay2,6,1,1};

LCD_Object_Display LCD_ACS_Value={char_ACS,6,1,1};
LCD_Object_Display LCD_ACS_Uint={"Dong:       Ampe",0,1,1};

void LCD_Change_State_Setup_T1_T2_T3(uint32_t stampTime1, uint32_t stampTime2, uint32_t stampTime3,
                                     uint32_t stampThresholdRelay1, uint32_t stampThresholdRelay2)
{
	if (stampTime1!=check_time1)
	{
		check_time1=stampTime1;
		LCD_Time1.state=1;
	}
	
	if (stampTime2!=check_time2)
	{
		check_time2=stampTime2;
		LCD_Time2.state=1;
	}
	
	if (stampTime3!=check_time3)
	{
		check_time3=stampTime3;
		LCD_Time3.state=1;
	}
	
	if (stampThresholdRelay1 != check_thresholdRelay1)
	{
		check_thresholdRelay1= stampThresholdRelay1;
		LCD_ThresholRelay1.state=1;
	}
	
	if (stampThresholdRelay2 != check_thresholdRelay2)
	{
		check_thresholdRelay2= stampThresholdRelay2;
		LCD_ThresholRelay2.state=1;
	}
}

void LCD_Change_State_Time_HH_MM_SS(uint16_t hh, uint16_t mm, uint32_t ss)
{
	if(check_SS!=ss)
	{
		check_SS=ss;
		LCD_Time_SS.state=1;
	}
	if(check_MM!=mm)
	{
		check_MM=mm;
		LCD_Time_MM.state=1;
	}
	if(check_HH!=hh)
	{
		check_HH=hh;
		LCD_Time_HH.state=1;
	}
}

void LCD_Change_State_ACS(float ACS_Value_Float)
{
	if (ACS_Value_Float!=check_ACS)
	{
		check_ACS=ACS_Value_Float;
		LCD_ACS_Value.state=1;
	}
}

void UintTime_To_CharTime_HH_MM_SS(uint16_t hh, uint16_t mm, uint32_t ss)
{
	Uint_To_Char_Time(HH, hh);
	Uint_To_Char_Time(MM, mm);
	Uint_To_Char_Time(SS, ss);
}

void UintTime_To_CharTime_T1_T2_T3(uint32_t stampTime1, uint32_t stampTime2, uint32_t stampTime3,
                                   uint32_t stampThresholdRelay1, uint32_t stampThresholdRelay2)
{
	Uint_To_Char(stamp_time1, stampTime1);
	Uint_To_Char(stamp_time2, stampTime2);
	Uint_To_Char(stamp_time3, stampTime3);
	
	Uint_To_Float_Mod(stampThresholdRelay1, &thresholdRelay1_Float);
	Uint_To_Float_Mod(stampThresholdRelay2, &thresholdRelay2_Float);
	
	Float_To_Char(stamp_thresholdRelay1, thresholdRelay1_Float);
	Float_To_Char(stamp_thresholdRelay2, thresholdRelay2_Float);
}

void Float_To_Char_ACS(float ACS_Value_Float)
{
	Float_To_Char(LCD_ACS_Value.Object,ACS_Value_Float);
	
	if( AC_DC == "AC" && LCD_ACS_Value.Object[0] == '-')
	{
		for(int i=0;i<9;i++)
		{
		LCD_ACS_Value.Object[i]=LCD_ACS_Value.Object[i+1];
		}
	}
}

void USER_LCD_Display_Time(CLCD_Name* LCD)
{
	LCD_Display_Time(LCD, &LCD_Time_HH, &LCD_Time_MM, &LCD_Time_SS);
}

void USER_LCD_Display_Running_OR_Setup(uint16_t State)
{
	if(State==0)
	{
		if(LCD_Running.state==0) LCD_Setup.state=1;
	}
	if(State==1)
	{
		if(LCD_Setup.state==0) LCD_Running.state=1;
	}
}

void USER_LCD_Display_Running(CLCD_Name* LCD, uint16_t setupCount, float ACS_Value_Float)
{
	LCD_Display_Running_OR_Setup(LCD, &LCD_Running, &LCD_Setup);
}

void USER_LCD_Display_Setup(CLCD_Name* LCD, uint16_t setupCount)
{
	LCD_Display_Running_OR_Setup(LCD, &LCD_Setup, &LCD_Running);
}

void USER_LCD_Display_X(CLCD_Name* LCD, uint16_t setupCount, float ACS_Value_Float)
{
	LCD_Change_State_ACS(ACS_Value_Float);
	Float_To_Char_ACS(ACS_Value_Float);
	LCD_Display_Esc(LCD, setupCount ,&LCD_ACS_Uint ,&LCD_SetupT1, &LCD_SetupT2, &LCD_SetupT3,
                                   &LCD_ThresholRelay1_Unit, &LCD_ThresholRelay2_Unit);
	LCD_Display_ACS_Time1_Time2_Time3(LCD, setupCount ,&LCD_ACS_Value ,&LCD_Time1, &LCD_Time2, &LCD_Time3,
                                                     &LCD_ThresholRelay1, &LCD_ThresholRelay2);
}

void USER_LCD_Change_Setup(void)
{
	LCD_SetupT1.state=1;
	LCD_SetupT2.state=1;
	LCD_SetupT3.state=1;
	LCD_ThresholRelay1_Unit.state=1;
	LCD_ThresholRelay2_Unit.state=1;
	LCD_ACS_Uint.state=1;
	
	LCD_Time1.state=1;
	LCD_Time2.state=1;
	LCD_Time3.state=1;
	LCD_ThresholRelay1.state=1;
	LCD_ThresholRelay2.state=1;
	LCD_ACS_Value.state=1;
}

void USER_LCD_Display_Warning(CLCD_Name* LCD, uint16_t stateWarning)
{
	if(check_Warning == 0 && stateWarning == 1)
	{
		LCD_Send_Data(LCD, &LCD_Warning);
		check_Warning=1;
	}
		
	if(check_Warning == 1 && stateWarning == 0)
	{
		LCD_Send_Data(LCD, &LCD_Time);
		LCD_Running.state=1;
		check_Warning=0;
	}
}


