#include "ACS712.h"

uint8_t countAvg=0;
float ADC_stamp=0;
uint32_t check=0;

void ACS_712(ADC_HandleTypeDef* hadc, float *ACS_Value)
{
	
	float offsetVoltage=2.5;
	float VCC=3.3;
	//float sensitivity_5A=0.185;
	//float sensitivity_20A=0.1;
	float sensitivity_30A=0.066;
	
	if(check>GET_SYSTICK_MS()) check=0;
	
	if(GET_SYSTICK_MS()-check>200 )
	{
		check=GET_SYSTICK_MS();
		ADC_stamp = ADC_stamp + HAL_ADC_GetValue(hadc)*2;
		countAvg++;
	}
	
	if(countAvg==5)
	{
		ADC_stamp=ADC_stamp/countAvg;
		ADC_stamp=(ADC_stamp/1023)*VCC;
		*ACS_Value = (ADC_stamp - 2.5)/sensitivity_30A;
		countAvg=0;
		ADC_stamp=0;
	}
}
