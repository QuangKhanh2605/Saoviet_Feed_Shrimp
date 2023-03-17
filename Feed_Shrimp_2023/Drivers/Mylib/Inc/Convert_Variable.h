#ifndef __CONVERT_VARIABLE_
#define __CONVERT_VARIABLE_

#include "stm32l1xx_hal.h"

#define LENGTH_MOD_FLOAT 1

void Uint_To_Char_Time(char time[], uint16_t stamp);
void Uint_To_Char_Length(char time[], uint16_t stamp, uint16_t *lengthStamp);
void Uint_To_Char(char time[], uint32_t stamp);
void Float_To_Char(char time[], float stamp);

#endif
