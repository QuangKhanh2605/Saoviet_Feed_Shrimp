#include "LCD_Object.h"

/*
	@brief  Hien thi len LCD
	@param  LCD la ten LCD muon hien thi
	@param  Name_Object_Run, Name_Object_stop la 2 Struct Object can hien thi
	@retval None
*/
void LCD_Display_Running_OR_Setup(CLCD_Name* LCD, LCD_Object_Display *Name_Object_Run,
                                                  LCD_Object_Display *Name_Object_Stop)
{
	if(Name_Object_Run->state==1 )
	{
	LCD_Send_Data(LCD, Name_Object_Run);
	Name_Object_Run->state=0;
	Name_Object_Stop->state=1;
	}
}

/*
	@brief  Hien thi len LCD
	@param  LCD la ten LCD muon hien thi 
	@param  setupCount, trang thai chuong trinh 
	@param  Name_Object1, Name_Object2,, Name_Object3, Name_Object4, Name_Object5, Name_Object6
          la Struct Object muon hien thi len LCD
	@retval None
*/
void LCD_Display_Esc(CLCD_Name* LCD,uint16_t setupCount,
																LCD_Object_Display *Name_Object1,
                                LCD_Object_Display *Name_Object2,
                                LCD_Object_Display *Name_Object3,
																LCD_Object_Display *Name_Object4,
                                LCD_Object_Display *Name_Object5,
                                LCD_Object_Display *Name_Object6)
{
	if(Name_Object1->state==1 && setupCount==1)
	{
	LCD_Send_Data(LCD, Name_Object1);
	Name_Object1->state=0;
	}
	
	if(Name_Object2->state==1 && setupCount==2)
	{
	LCD_Send_Data(LCD, Name_Object2);
	Name_Object2->state=0;
	}
	
	if(Name_Object3->state==1 && setupCount==3)
	{
	LCD_Send_Data(LCD, Name_Object3);
	Name_Object3->state=0;
	}
	
	if(Name_Object4->state==1 && setupCount==4)
	{
	LCD_Send_Data(LCD, Name_Object4);
	Name_Object4->state=0;
	}
	
	if(Name_Object5->state==1 && setupCount==5)
	{
	LCD_Send_Data(LCD, Name_Object5);
	Name_Object5->state=0;
	}
	
	if(Name_Object6->state==1 && setupCount==6)
	{
	LCD_Send_Data(LCD, Name_Object6);
	Name_Object6->state=0;
	}
}

/*
	@brief  Hien thi len LCD
	@param  LCD la ten LCD muon hien thi 
	@param  setupCount, trang thai chuong trinh 
	@param  Name_Object1, Name_Object2,, Name_Object3, Name_Object4, Name_Object5, Name_Object6
          la Struct Object muon hien thi len LCD
	@retval None
*/
void LCD_Display_ACS_Time1_Time2_Time3(CLCD_Name* LCD,uint16_t setupCount,
																       LCD_Object_Display *Name_Object1,
                                       LCD_Object_Display *Name_Object2,
                                       LCD_Object_Display *Name_Object3,
																       LCD_Object_Display *Name_Object4,
                                       LCD_Object_Display *Name_Object5,
                                       LCD_Object_Display *Name_Object6)
{
	if(setupCount==1 && Name_Object1->state==1)
	{
		LCD_Send_Data(LCD, Name_Object1);
		Name_Object1->state=0;
	}
	
	if(setupCount==2 && Name_Object2->state==1)
	{
		LCD_Send_Data(LCD, Name_Object2);
		Name_Object2->state=0;
	}
	
	if(setupCount==3 && Name_Object3->state==1)
	{
		LCD_Send_Data(LCD, Name_Object3);
		Name_Object3->state=0;
	}
	
	if(setupCount==4 && Name_Object4->state==1)
	{
		LCD_Send_Data(LCD, Name_Object4);
		Name_Object4->state=0;
	}
	
	if(setupCount==5 && Name_Object5->state==1)
	{
		LCD_Send_Data(LCD, Name_Object5);
		Name_Object5->state=0;
	}
	
	if(setupCount==6 && Name_Object6->state==1)
	{
		LCD_Send_Data(LCD, Name_Object6);
		Name_Object6->state=0;
	}
}

/*
	@brief  Hien thi len LCD
	@param  LCD la ten LCD muon hien thi 
	@param  Name_Object1, Name_Object2,, Name_Object3,la Struct Object muon hien thi len LCD
	@retval None
*/
void LCD_Display_Time(CLCD_Name* LCD,LCD_Object_Display *Name_Object1,
                                     LCD_Object_Display *Name_Object2,
                                     LCD_Object_Display *Name_Object3)
{
	if(Name_Object1->state==1)
	{
	LCD_Send_Data(LCD, Name_Object1);
	Name_Object1->state=0;
	}
	if(Name_Object2->state==1)
	{
	LCD_Send_Data(LCD, Name_Object2);
	Name_Object2->state=0;
	}
	if(Name_Object3->state==1)
	{
	LCD_Send_Data(LCD, Name_Object3);
	Name_Object3->state=0;
	}
}

void LCD_Send_Data(CLCD_Name* LCD, LCD_Object_Display *Name_Object)
{
	CLCD_SetCursor(LCD, Name_Object->col, Name_Object->rol);
	CLCD_WriteString(LCD, Name_Object->Object);
}
