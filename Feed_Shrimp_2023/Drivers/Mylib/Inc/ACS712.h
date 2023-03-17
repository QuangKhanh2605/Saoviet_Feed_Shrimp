#ifndef __ACS712_
#define __ACS712_

#include "stm32l1xx_hal.h"
#include "check_Button.h"
#include "math.h"

void ACS_712(ADC_HandleTypeDef* hadc, float *ACS_Value);

#endif
