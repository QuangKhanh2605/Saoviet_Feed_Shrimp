#ifndef __ACS712_
#define __ACS712_

#include "stm32l1xx_hal.h"
#include "check_Button.h"
#include "math.h"
#include "Convert_Variable.h"

void ACS_712(float *ACS_Value_Float, uint32_t *ACS_Value_Uint, uint16_t ADC_VCC_Value, uint16_t ADC_ACS_Value);

#endif
