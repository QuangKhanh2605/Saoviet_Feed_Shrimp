#include "Convert_Variable.h"


const uint16_t ACSII_value_number=48;

void Uint_To_Char_Time(char *time, uint16_t stamp)
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

void Uint_To_Char(char time[], uint32_t stamp)
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

void Float_To_Char(char time[], float stamp)
{
	uint16_t division=1;
	for(int j=1;j<=LENGTH_MOD_FLOAT; j++)
	{
		division = division*10;
	}
	
	uint16_t test=stamp*division;
	uint16_t mod=test%division;
	uint16_t quotient=test/division;
	uint16_t lengthMod=2;
	uint16_t lengthQoutient=1;
	uint32_t divisionMod=10;
	uint32_t divisionQoutient=10;
	char Cmod[3];
	char Cquotient[3];
	
	while(mod/divisionMod>=1)
	{
		divisionMod=divisionMod*10;
		(lengthMod)++;
	}
	while(quotient/divisionQoutient>=1)
	{
		divisionQoutient=divisionQoutient*10;
		(lengthQoutient)++;
	}
	
	for(int j=LENGTH_MOD_FLOAT-1;j>=0;j--)
	{
		Cmod[j]=mod%10+ACSII_value_number;
		mod=mod/10;
	}
	for(int j=lengthQoutient-1;j>=0;j--)
	{
		Cquotient[j]=quotient%10+ACSII_value_number;
		quotient=quotient/10;
	}	
	
	uint16_t count=0;
	if(stamp < 0 && (mod !=0 || quotient !=0))
	{
		time[count]='-';
		count++;
	}
	
	for(int j=0;j<lengthQoutient;j++)
	{
		time[count]=Cquotient[j];
		count++;
	}
	
	if(LENGTH_MOD_FLOAT!=0)
	{
		time[count]='.';
		count++;
	}
	
	for(int j=lengthMod; j<LENGTH_MOD_FLOAT;j++)
	{
		time[count]='0';
		count++;
	}
	
	for(int j=0;j<LENGTH_MOD_FLOAT;j++)
	{
		time[count]=Cmod[j];
		count++;
	}
	
	time[count]=' ';
}

void Uint_To_Char_Length(char time[], uint16_t stamp, uint16_t *lengthStamp)
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

