#ifndef __USER_SIM_
#define __USER_SIM_

#include "uart_sim.h"
#include "math.h"

void Receive_SMS_Setup(char *sim_rx,uint32_t *time1,uint32_t *time2,uint32_t *time3);
void Char_To_Uint(char *char_time,uint32_t *uint_time, uint16_t length);

#endif

