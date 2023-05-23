#include "ACS712.h"

int16_t dropACS=16;
static int16_t dropACS_countState=0;
uint16_t countAvg=0;

uint32_t ADC_ACS_Avg=0;
float ADC_ACS=0;

uint32_t offsetVoltage_Avg=0;
float offsetVoltage=0;

/*
	@brief  Tinh gia tri dong do duoc thong qua gia tri ADC cua IC ACS712 va cua nguon
	@param  ACS_Value_Float Gia tri dong dang Float sau khi tinh toan
	@param  ACS_Value_Uint Gia tri dong dang Uint sau khi tinh toan
	@param  ADC_VCC_Value Gia tri ADC cua nguon 
	@param  ADC_ACS_Value Gia tri ADC cua IC ACS712
	@retval None
*/
void ACS_712(float *ACS_Value_Float, uint32_t *ACS_Value_Uint, uint16_t ADC_VCC_Value, uint16_t ADC_ACS_Value)
{
	ADC_ACS_Avg = ADC_ACS_Avg + ADC_ACS_Value*2 + dropACS+ dropACS_countState;
	offsetVoltage_Avg = offsetVoltage_Avg + ADC_VCC_Value;
	countAvg++;

	if(countAvg == 25)
	{
		offsetVoltage_Avg = offsetVoltage_Avg/countAvg;
		offsetVoltage = ((float)offsetVoltage_Avg/ADC_12BIT)*VCC;
		
		ADC_ACS_Avg = ADC_ACS_Avg/countAvg;
		ADC_ACS = ((float)ADC_ACS_Avg/ADC_12BIT)*VCC;
		
		*ACS_Value_Float = (ADC_ACS - offsetVoltage)/SENSITIVITY_30A;
		*ACS_Value_Uint = *ACS_Value_Float * pow(10,LENGTH_MOD_FLOAT);

		countAvg=0;
		offsetVoltage=0;
		ADC_ACS=0;
		offsetVoltage_Avg=0;
		ADC_ACS_Avg=0;
	}
}

/*
	@brief  Hieu chuan gia tri dong do duoc theo trang thai chuong trinh keo Relay
	@param  countState trang thai chuong trinh dang chay
	@retval None
*/
void Calib_CountState(uint16_t countState)
{
	if(countState == 1) dropACS_countState=2;
	else if(countState == 2) dropACS_countState=4;
	else dropACS_countState = 0;
}
