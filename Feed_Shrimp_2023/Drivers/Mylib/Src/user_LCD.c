#include "user_LCD.h"

const uint16_t ACSII_value_number=48;

void Variable_To_Char_Time(char *time, uint16_t stamp)
{
	if(stamp<10)
	{
		time[0]='0';
		time[1]=stamp+ACSII_value_number;
	}
	else
	{
		time[0]=stamp/10+ACSII_value_number;
		time[1]=stamp%10+ACSII_value_number;
	}
	time[2]=NULL;
}

void Variable_To_Char(char time[], uint32_t stamp)
{
	uint16_t lengthStamp=1;
	uint32_t Division=10;
	while(stamp/Division>=1)
	{
		Division=Division*10;
		(lengthStamp)++;
	}
	
	for(int j=lengthStamp-1;j>=0;j--)
	{
	time[j]=stamp%10+ACSII_value_number;
	stamp=stamp/10;
	}
	time[lengthStamp]=' ';
	time[lengthStamp++]=' ';
}

void float_To_Char(char time[], float stamp)
{
	uint32_t test=stamp*100;
	uint16_t lengthStamp=1;
	uint32_t Division=10;
	while(test/Division>=1)
	{
		Division=Division*10;
		(lengthStamp)++;
	}
	for(int j=lengthStamp-1;j>=0;j--)
	{
	time[j]=test%10+ACSII_value_number;
	test=test/10;
	}
	if(lengthStamp>2)
	{
	lengthStamp++;
	time[lengthStamp-1]=time[lengthStamp-2];
	time[lengthStamp-2]=time[lengthStamp-3];
	time[lengthStamp-3]='.';
	}
	else if(lengthStamp==1)
	{
	lengthStamp=4;
	time[lengthStamp-1]=time[lengthStamp-4];
	time[lengthStamp-2]='0';
	time[lengthStamp-3]='.';
	time[0]='0';
	}
	else if(lengthStamp==2)
	{
	lengthStamp=4;
	time[lengthStamp-1]=time[lengthStamp-3];
	time[lengthStamp-2]=time[lengthStamp-4];
	time[lengthStamp-3]='.';
	time[0]='0';
	}
	time[lengthStamp]=' ';
}

void Variable_To_Char_Length(char time[], uint16_t stamp, uint16_t *lengthStamp)
{
	uint16_t Division=10;
	while(stamp/Division>=1)
	{
		Division=Division*10;
		(*lengthStamp)++;
	}
		for(int j=*lengthStamp-1;j>=0;j--)
		{
		time[j]=stamp%10+ACSII_value_number;
		stamp=stamp/10;
		}
}

void LCD_Run_Time(char time[],char hh[], char mm[], char ss[])
{
	time[2]=time[5]=':';
	time[0]=hh[0]; time[1]=hh[1];
	time[3]=mm[0]; time[4]=mm[1];
	time[6]=ss[0]; time[7]=ss[1];
	time[8]=NULL;
}

void LCD_Running_X1(CLCD_Name* LCD, uint16_t hh, uint16_t mm, uint16_t ss)
{
	CLCD_SetCursor(LCD,0,0);
	CLCD_WriteString(LCD,"Running ");
	char h[3],m[3],s[3];
	char time[8];
	
	Variable_To_Char_Time(h, hh);
	Variable_To_Char_Time(m, mm);
	Variable_To_Char_Time(s, ss);
	LCD_Run_Time(time, h, m, s);
	CLCD_SetCursor(LCD,8,0);
	CLCD_WriteString(LCD,time);

}

void LCD_Running_X2(CLCD_Name* LCD, uint16_t t1, uint16_t t2, uint16_t t3)
{

	CLCD_SetCursor(LCD,0,1);
	CLCD_WriteString(LCD,"                ");
}

void LCD_Setup_X1(CLCD_Name* LCD, uint16_t hh, uint16_t mm, uint16_t ss, uint16_t setupCount)
{
	CLCD_SetCursor(LCD,0,0);
	if(setupCount==1) 			CLCD_WriteString(LCD,"SET_T1 ");
	else if(setupCount==2) CLCD_WriteString(LCD,"SET_T2 ");
	else			  	CLCD_WriteString(LCD,"SET_T3 ");
	
	char h[3],m[3],s[3];
	char time[8];
	
	Variable_To_Char_Time(h, hh);
	Variable_To_Char_Time(m, mm);
	Variable_To_Char_Time(s, ss);
	LCD_Run_Time(time, h, m, s);
	CLCD_SetCursor(LCD,8,0);
	CLCD_WriteString(LCD,time);
}

void LCD_Setup_X2(CLCD_Name* LCD, uint16_t time, uint16_t setupCount)
{
	char LCD_send[16];
	uint16_t lengthStamp=1;
	
	CLCD_SetCursor(LCD,0,1);
	if(setupCount==1)       CLCD_WriteString(LCD,"T1:");
	else if(setupCount==2)  CLCD_WriteString(LCD,"T2:");
	if(setupCount==3)       CLCD_WriteString(LCD,"T3:");	
	
	Variable_To_Char_Length(LCD_send, time, &lengthStamp);
	CLCD_SetCursor(LCD,3,1);
	CLCD_WriteString(LCD,LCD_send);
	
	CLCD_SetCursor(LCD,3+lengthStamp,1);
	if(setupCount==1)       CLCD_WriteString(LCD," Giay           ");
	else if(setupCount==2)  CLCD_WriteString(LCD," Phut           ");
	if(setupCount==3)       CLCD_WriteString(LCD," Giay           ");	
}



