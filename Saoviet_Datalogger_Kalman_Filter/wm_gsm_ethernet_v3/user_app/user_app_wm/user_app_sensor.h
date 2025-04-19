

#ifndef USER_APP_SENSOR_H_
#define USER_APP_SENSOR_H_

#define USING_APP_SENSOR

#define USING_APP_SENSOR_DEBUG

#define TIME_RESEND_WARNING         5
#define NUMBER_SAMPLING_SS          10


#include "event_driven.h"
#include "user_util.h"
#include "user_app_rs485.h"

typedef enum
{
    _EVENT_SENSOR_ENTRY,
    _EVENT_SENSOR_STATE_PH,
    _EVENT_SENSOR_STATE_CLO,
    _EVENT_SENSOR_STATE_EC,
    _EVENT_SENSOR_STATE_TURBIDITY,
    
    _EVENT_SENSOR_END,
}eKindEventSensor;

typedef enum 
{
    _OFFSET_CLO_DU,
    _OFFSET_PH,
    _OFFSET_NTU,
    _OFFSET_SALINITY,
    _OFFSET_TEMPERATURE,
    _OFFSET_EC,
}eKindOffsetMeasure;

typedef struct
{
    Struct_SS_Value sClo_Du;
    Struct_SS_Value spH_Water;
    Struct_SS_Value sNTU;
    Struct_SS_Value sSalinity;
    Struct_SS_Value sTemperature;
    Struct_SS_Value sEC;
    
    Struct_SS_Value sClo_Du_Sampling;
    Struct_SS_Value spH_Water_Sampling;
    Struct_SS_Value sNTU_Sampling;
    Struct_SS_Value sSalinity_Sampling;
    Struct_SS_Value sTemperature_Sampling;
    Struct_SS_Value sEC_Sampling;
}Struct_Offset_Measure;

extern sEvent_struct        sEventAppSensor[];
extern Struct_Offset_Measure   sOffsetMeasure;
/*=============== Function handle ================*/

uint8_t     AppSensor_Task(void);
void        Init_AppSensor(void);

void Log_EventWarnig(uint8_t Obis, uint8_t LengthData, uint8_t *aDataWaring);
void Save_TimeWarningSensor(uint8_t Duration);
void Init_TimeWarningSensor(void);

void Save_OffsetMeasure(uint8_t KindOffset, Struct_SS_Value sMeasure);
void Init_OffsetMeasure(void);

void AT_CMD_Get_Time_Warning_Sensor(sData *str, uint16_t Pos);
void AT_CMD_Set_Time_Warning_Sensor (sData *str_Receiv, uint16_t Pos);

void AT_CMD_Get_Offset_Clo (sData *str, uint16_t Pos);
void AT_CMD_Set_Offset_Clo (sData *str, uint16_t Pos);

void AT_CMD_Get_Offset_pH (sData *str, uint16_t Pos);
void AT_CMD_Set_Offset_pH (sData *str, uint16_t Pos);

void AT_CMD_Get_Offset_NTU (sData *str, uint16_t Pos);
void AT_CMD_Set_Offset_NTU (sData *str, uint16_t Pos);

void quickSort_Sampling(int16_t array_stt[],int16_t array_sampling[], uint8_t left, uint8_t right);
void quickSort_Sampling_pH(void);
void quickSort_Sampling_Clo(void);
void quickSort_Sampling_EC(void);
void quickSort_Sampling_Turbidity(void);

int32_t Filter_Turbidity(int16_t var, uint8_t scale);
int32_t Filter_CloDu(int16_t var, uint8_t scale);
int32_t Filter_pH(int16_t var, uint8_t scale);
int32_t Filter_Salinity(int16_t var, uint8_t scale);
int32_t Filter_Temperature(int16_t var, uint8_t scale);
int32_t Filter_EC(int16_t var, uint8_t scale);
#endif
