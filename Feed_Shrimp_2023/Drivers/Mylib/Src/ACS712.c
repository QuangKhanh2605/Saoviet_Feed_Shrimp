#include "ACS712.h"

uint16_t countAvg=0;
//uint16_t ADC_stamp[2];
uint16_t dropACS=14;
uint32_t ADC_12bit=4095;

uint32_t ADC_ACS_Avg=0;
float ADC_ACS=0;

uint32_t offsetVoltage_Avg=0;
float offsetVoltage=0;

float VCC=3.3;
//float sensitivity_5A=0.185;
//float sensitivity_20A=0.1;
float sensitivity_30A=0.066;

void ACS_712(float *ACS_Value_Float,uint32_t *ACS_Value_Uint, uint16_t ADC_VCC_Value, uint16_t ADC_ACS_Value)
{
	ADC_ACS_Avg = ADC_ACS_Avg + ADC_ACS_Value*2+dropACS;
	offsetVoltage_Avg = offsetVoltage_Avg + ADC_VCC_Value;
	countAvg++;
	
	if(countAvg == 50)
	{
		offsetVoltage_Avg = offsetVoltage_Avg/countAvg;
		offsetVoltage = ((float)offsetVoltage_Avg/ADC_12bit)*VCC;
		
		ADC_ACS_Avg = ADC_ACS_Avg/countAvg;
		ADC_ACS = ((float)ADC_ACS_Avg/ADC_12bit)*VCC;
		
		*ACS_Value_Float = (ADC_ACS - offsetVoltage)/sensitivity_30A;
		*ACS_Value_Uint = *ACS_Value_Float * pow(10,LENGTH_MOD_FLOAT);
		//Uint_To_Float(ACS_Value_Uint, ACS_Value);
		countAvg=0;
		offsetVoltage=0;
		ADC_ACS=0;
		offsetVoltage_Avg=0;
		ADC_ACS_Avg=0;
	}
}
