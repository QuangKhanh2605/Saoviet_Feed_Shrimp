#include "user_sim.h"

void Receive_SMS_Setup(char *sim_rx, uint32_t *time1, uint32_t *time2, uint32_t *time3)
{
	uint16_t length=strlen(sim_rx);
	uint16_t length_time1=0;
	uint16_t length_time2=0;
	uint16_t length_time3=0;
	
	char char_time1[16];
	char char_time2[16];
	char char_time3[16];
	
	for(int i=0; i<length;i++)
	{
		if(sim_rx[i]=='T' || sim_rx[i]=='t')
		{
			if(sim_rx[i+2]=='=' || sim_rx[i+2]==':')
			{
				if(sim_rx[i+1]=='1')
				{
					for(int j=i+3;j<length;j++)
					{
						if(sim_rx[j] < '0' || sim_rx[j] > '9')
						{
							break;
						}
						char_time1[length_time1]=sim_rx[j];
						length_time1++;
					}
				}
				
				if(sim_rx[i+1]=='2')
				{
					for(int j=i+3;j<length;j++)
					{
						if(sim_rx[j] < '0' || sim_rx[j] > '9')
						{
							break;
						}
						char_time2[length_time2]=sim_rx[j];
						length_time2++;
					}
				}
				
				if(sim_rx[i+1]=='3')
				{
					for(int j=i+3;j<length;j++)
					{
						if(sim_rx[j] < '0' || sim_rx[j] > '9')
						{
							break;
						}
						char_time3[length_time3]=sim_rx[j];
						length_time3++;
					}
				}
			}
		}
	}
	Char_To_Uint(char_time1, time1, length_time1);
	Char_To_Uint(char_time2, time2, length_time2);
	Char_To_Uint(char_time3, time3, length_time3);
}

void Char_To_Uint(char *char_time,uint32_t *uint_time, uint16_t length)
{
	uint32_t time=0;
	for(int i=0;i<length;i++)
	{
		time=time+(char_time[i]-48)*pow(10,length-i-1);
	}
	if(time <= maxTime && length!=0) *uint_time=time;
}

