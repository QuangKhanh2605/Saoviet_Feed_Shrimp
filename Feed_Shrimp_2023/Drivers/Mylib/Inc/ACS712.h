#ifndef __ACS712_
#define __ACS712_

#include "stm32l1xx_hal.h"
#include "check_Button.h"
#include "math.h"
#include "Convert_Variable.h"

#define SENSITIVITY_5A   0.185
#define SENSITIVITY_20A  0.1
#define SENSITIVITY_30A  0.066

#define ADC_12BIT        4095
#define ADC_10BIT        1023

#define VCC              3.3

void ACS_712(float *ACS_Value_Float,uint32_t *ACS_Value_Uint, uint16_t ADC_VCC_Value, uint16_t ADC_ACS_Value);
void Calib_CountState(uint16_t countState);

#endif
