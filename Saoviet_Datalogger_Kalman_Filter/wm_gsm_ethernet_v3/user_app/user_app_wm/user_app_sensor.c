
#include "user_app_sensor.h"
#include "user_define.h"
#include "user_convert_variable.h"

/*============== Function static ===============*/
static uint8_t fevent_sensor_entry(uint8_t event);

static uint8_t fevent_sensor_state_ph(uint8_t event);
static uint8_t fevent_sensor_state_clo(uint8_t event);
static uint8_t fevent_sensor_state_ec(uint8_t event);
static uint8_t fevent_sensor_state_turbidity(uint8_t event);
/*=================== struct ==================*/
sEvent_struct               sEventAppSensor[] = 
{
  {_EVENT_SENSOR_ENTRY,              1, 5, 60000,                fevent_sensor_entry},
  
  {_EVENT_SENSOR_STATE_PH,           0, 5, 500,                  fevent_sensor_state_ph},
  {_EVENT_SENSOR_STATE_CLO,          0, 5, 500,                  fevent_sensor_state_clo},
  {_EVENT_SENSOR_STATE_EC,           0, 5, 500,                  fevent_sensor_state_ec},
  {_EVENT_SENSOR_STATE_TURBIDITY,    0, 5, 500,                  fevent_sensor_state_turbidity},
};

Struct_Offset_Measure   sOffsetMeasure = {0};

uint8_t DurationTimeWarningSensor = 0;

int16_t aSampling_STT_pH[NUMBER_SAMPLING_SS] = {0};
int16_t aSampling_VALUE_pH[NUMBER_SAMPLING_SS] = {0};

int16_t aSampling_STT_Clo[NUMBER_SAMPLING_SS] = {0};
int16_t aSampling_VALUE_Clo[NUMBER_SAMPLING_SS] = {0};

int16_t aSampling_STT_EC[NUMBER_SAMPLING_SS] = {0};
int16_t aSampling_VALUE_EC[NUMBER_SAMPLING_SS] = {0};

int16_t aSampling_STT_Turbidity[NUMBER_SAMPLING_SS] = {0};
int16_t aSampling_VALUE_Turbidity[NUMBER_SAMPLING_SS] = {0};

/*================= Function Handle ==============*/
static uint8_t fevent_sensor_entry(uint8_t event)
{
    fevent_enable(sEventAppSensor, _EVENT_SENSOR_STATE_PH);
    return 1;
}

static uint8_t fevent_sensor_state_ph(uint8_t event)
{
    static uint32_t gettick_state_slave = 0;
    static uint8_t StateSensor_Before = _SENSOR_CONNECT;
    uint8_t aData[2] = {0};

    if(sSensor_pH.State_Connect != StateSensor_Before)
    {
        gettick_state_slave = HAL_GetTick();
        
        if(sSensor_pH.State_Connect == _SENSOR_CONNECT)      
        {

        }
        else
        {
            aData[0] = 0x00;
            aData[1] = 0x00;
            Log_EventWarnig(OBIS_WARNING_SENSOR_CONNECT, 0x01, aData);
        }

        StateSensor_Before = sSensor_pH.State_Connect;
    }

    if(sSensor_pH.State_Connect == _SENSOR_DISCONNECT)
    {
        if(HAL_GetTick() - gettick_state_slave >= DurationTimeWarningSensor*60000)
        {
            gettick_state_slave = HAL_GetTick();
            aData[0] = 0x00;
            aData[1] = 0x00;
            Log_EventWarnig(OBIS_WARNING_SENSOR_CONNECT, 0x01, aData);
        }
    }

    fevent_enable(sEventAppSensor, _EVENT_SENSOR_STATE_CLO);
    return 1;
}

static uint8_t fevent_sensor_state_clo(uint8_t event)
{
    static uint32_t gettick_state_slave = 0;
    static uint8_t StateSensor_Before = _SENSOR_CONNECT;
    uint8_t aData[2] = {0};

    if(sSensor_Clo.State_Connect != StateSensor_Before)
    {
        gettick_state_slave = HAL_GetTick();
        
        if(sSensor_Clo.State_Connect == _SENSOR_CONNECT)      
        {

        }
        else
        {
            aData[0] = 0x02;
            aData[1] = 0x00;
            Log_EventWarnig(OBIS_WARNING_SENSOR_CONNECT, 0x01, aData);
        }

        StateSensor_Before = sSensor_Clo.State_Connect;
    }

    if(sSensor_Clo.State_Connect == _SENSOR_DISCONNECT)
    {
        if(HAL_GetTick() - gettick_state_slave >= DurationTimeWarningSensor*60000)
        {
            gettick_state_slave = HAL_GetTick();
            aData[0] = 0x02;
            aData[1] = 0x00;
            Log_EventWarnig(OBIS_WARNING_SENSOR_CONNECT, 0x01, aData);
        }
    }

    fevent_enable(sEventAppSensor, _EVENT_SENSOR_STATE_EC);
    return 1;
}

static uint8_t fevent_sensor_state_ec(uint8_t event)
{
    static uint32_t gettick_state_slave = 0;
    static uint8_t StateSensor_Before = _SENSOR_CONNECT;
    uint8_t aData[2] = {0};

    if(sSensor_EC.State_Connect != StateSensor_Before)
    {
        gettick_state_slave = HAL_GetTick();
        
        if(sSensor_EC.State_Connect == _SENSOR_CONNECT)      
        {

        }
        else
        {
            aData[0] = 0x04;
            aData[1] = 0x00;
            Log_EventWarnig(OBIS_WARNING_SENSOR_CONNECT, 0x01, aData);
        }

        StateSensor_Before = sSensor_EC.State_Connect;
    }

    if(sSensor_EC.State_Connect == _SENSOR_DISCONNECT)
    {
        if(HAL_GetTick() - gettick_state_slave >= DurationTimeWarningSensor*60000)
        {
            gettick_state_slave = HAL_GetTick();
            aData[0] = 0x04;
            aData[1] = 0x00;
            Log_EventWarnig(OBIS_WARNING_SENSOR_CONNECT, 0x01, aData);
        }
    }

    fevent_enable(sEventAppSensor, _EVENT_SENSOR_STATE_TURBIDITY);
    return 1;
}

static uint8_t fevent_sensor_state_turbidity(uint8_t event)
{
    static uint32_t gettick_state_slave = 0;
    static uint8_t StateSensor_Before = _SENSOR_CONNECT;
    uint8_t aData[2] = {0};

    if(sSensor_Turbidity.State_Connect != StateSensor_Before)
    {
        gettick_state_slave = HAL_GetTick();
        
        if(sSensor_Turbidity.State_Connect == _SENSOR_CONNECT)      
        {

        }
        else
        {
            aData[0] = 0x06;
            aData[1] = 0x00;
            Log_EventWarnig(OBIS_WARNING_SENSOR_CONNECT, 0x01, aData);
        }

        StateSensor_Before = sSensor_Turbidity.State_Connect;
    }

    if(sSensor_Turbidity.State_Connect == _SENSOR_DISCONNECT)
    {
        if(HAL_GetTick() - gettick_state_slave >= DurationTimeWarningSensor*60000)
        {
            gettick_state_slave = HAL_GetTick();
            aData[0] = 0x06;
            aData[1] = 0x00;
            Log_EventWarnig(OBIS_WARNING_SENSOR_CONNECT, 0x01, aData);
        }
    }

    fevent_enable(sEventAppSensor, _EVENT_SENSOR_STATE_PH);
    return 1;
}
/*=======================Handle Sensor======================*/
void Log_EventWarnig(uint8_t Obis, uint8_t LengthData, uint8_t *aDataWaring)
{
  Get_RTC();
  
  if(sRTC.year > 20)
  {
    uint8_t     aData[10]={0};
    uint16_t    length = 0;
    uint16_t	i = 0;
    uint8_t     TempCrc = 0;
    
    SV_Protocol_Packet_Data(aData, &length, OBIS_TIME_DEVICE, &sRTC, 6, 0xAA);
    aData[length++] = Obis;
    aData[length++] = LengthData;
    
    for(uint8_t i = 0; i < LengthData; i++)
        aData[length++] = *(aDataWaring+i);
    
    // caculator crc
    length++;
	for (i = 0; i < (length - 1); i++)
		TempCrc ^= aData[i];

    aData[length-1] = TempCrc;
  
#ifdef USING_APP_MEM
    AppMem_Write_Data(sAppMem.RecMemType_u8, _MEM_DATA_EVENT_A, 0, &aData[0], length, sRecEvent.SizeRecord_u16);
#endif
  }
}
/*==================Handle Define AT command=================*/
#ifdef USING_AT_CONFIG
void AT_CMD_Get_Time_Warning_Sensor(sData *str, uint16_t Pos)
{
    uint8_t aTemp[50] = "TimeWarningSensor: ";   //13 ki tu dau tien
    sData StrResp = {&aTemp[0], 19}; 

    Convert_Uint64_To_StringDec (&StrResp, (uint64_t) (DurationTimeWarningSensor), 0);
    Insert_String_To_String(StrResp.Data_a8, &StrResp.Length_u16, (uint8_t*)" min",0 , 4);

	Modem_Respond(PortConfig, StrResp.Data_a8, StrResp.Length_u16, 0);
}

void AT_CMD_Set_Time_Warning_Sensor (sData *str_Receiv, uint16_t Pos)
{
    uint32_t TempU32 = 0;
    if( str_Receiv->Data_a8[0] >= '0' && str_Receiv->Data_a8[0] <= '9')
    {
        uint8_t length = 0;
        for(uint8_t i = 0; i < str_Receiv->Length_u16; i++)
        {
            if( str_Receiv->Data_a8[i] < '0' || str_Receiv->Data_a8[i]>'9') break;
            else length++;
        }
        TempU32 = Convert_String_To_Dec(str_Receiv->Data_a8 , length);
        if(TempU32 <=255 && TempU32 >=1)
        {
            Save_TimeWarningSensor(TempU32);
            Modem_Respond(PortConfig, (uint8_t*)"OK", 2, 0);
        }
        else
        {
            Modem_Respond(PortConfig, (uint8_t*)"ERROR", 5, 0);
        }
    }
    else
    {
        Modem_Respond(PortConfig, (uint8_t*)"ERROR", 5, 0);
    }
}

void AT_CMD_Get_Offset_Clo (sData *str, uint16_t Pos)
{
    uint8_t aTemp[50] = "Offset Clo: ";   //13 ki tu dau tien
    sData StrResp = {&aTemp[0], 12}; 

    Convert_Point_Int_To_String_Scale (aTemp, &StrResp.Length_u16, (int)(sOffsetMeasure.sClo_Du.Value), sOffsetMeasure.sClo_Du.Scale);
    Insert_String_To_String(aTemp, &StrResp.Length_u16, (uint8_t*)" mg/L",0 , 5);

	Modem_Respond(PortConfig, StrResp.Data_a8, StrResp.Length_u16, 0);
}

void AT_CMD_Set_Offset_Clo (sData *str_Receiv, uint16_t Pos)
{
    uint32_t TempU32 = 0;
    Struct_SS_Value sMeasure;
    if( str_Receiv->Data_a8[0] >= '0' && str_Receiv->Data_a8[0] <= '9')
    {
        uint8_t length = 0;
        for(uint8_t i = 0; i < str_Receiv->Length_u16; i++)
        {
            if( str_Receiv->Data_a8[i] < '0' || str_Receiv->Data_a8[i]>'9') break;
            else length++;
        }
        TempU32 = Convert_String_To_Dec(str_Receiv->Data_a8 , length);
        if(TempU32 <= 32765)
        {
            sMeasure.Value = TempU32;
            Save_OffsetMeasure(_OFFSET_CLO_DU, sMeasure);
            Modem_Respond(PortConfig, (uint8_t*)"OK", 2, 0);
        }
        else
        {
            Modem_Respond(PortConfig, (uint8_t*)"ERROR", 5, 0);
        }
    }
    else
    {
        Modem_Respond(PortConfig, (uint8_t*)"ERROR", 5, 0);
    }
}

void AT_CMD_Get_Offset_pH (sData *str, uint16_t Pos)
{
    uint8_t aTemp[50] = "Offset pH: ";   //13 ki tu dau tien
    sData StrResp = {&aTemp[0], 11}; 

    Convert_Point_Int_To_String_Scale (aTemp, &StrResp.Length_u16, (int)(sOffsetMeasure.spH_Water.Value), sOffsetMeasure.spH_Water.Scale);
    Insert_String_To_String(aTemp, &StrResp.Length_u16, (uint8_t*)" pH",0 , 3);

	Modem_Respond(PortConfig, StrResp.Data_a8, StrResp.Length_u16, 0);
}

void AT_CMD_Set_Offset_pH (sData *str_Receiv, uint16_t Pos)
{
    uint32_t TempU32 = 0;
    Struct_SS_Value sMeasure;
    if( str_Receiv->Data_a8[0] >= '0' && str_Receiv->Data_a8[0] <= '9')
    {
        uint8_t length = 0;
        for(uint8_t i = 0; i < str_Receiv->Length_u16; i++)
        {
            if( str_Receiv->Data_a8[i] < '0' || str_Receiv->Data_a8[i]>'9') break;
            else length++;
        }
        TempU32 = Convert_String_To_Dec(str_Receiv->Data_a8 , length);
        if(TempU32 <= 32765)
        {
            sMeasure.Value = TempU32;
            Save_OffsetMeasure(_OFFSET_PH, sMeasure);
            Modem_Respond(PortConfig, (uint8_t*)"OK", 2, 0);
        }
        else
        {
            Modem_Respond(PortConfig, (uint8_t*)"ERROR", 5, 0);
        }
    }
    else
    {
        Modem_Respond(PortConfig, (uint8_t*)"ERROR", 5, 0);
    }
}
void AT_CMD_Get_Offset_NTU (sData *str, uint16_t Pos)
{
    uint8_t aTemp[50] = "Offset NTU: ";   //13 ki tu dau tien
    sData StrResp = {&aTemp[0], 12}; 

    Convert_Point_Int_To_String_Scale (aTemp, &StrResp.Length_u16, (int)(sOffsetMeasure.sNTU.Value), sOffsetMeasure.sNTU.Scale);
    Insert_String_To_String(aTemp, &StrResp.Length_u16, (uint8_t*)" NTU",0 , 4);

	Modem_Respond(PortConfig, StrResp.Data_a8, StrResp.Length_u16, 0);
}

void AT_CMD_Set_Offset_NTU (sData *str_Receiv, uint16_t Pos)
{
    uint32_t TempU32 = 0;
    Struct_SS_Value sMeasure;
    if( str_Receiv->Data_a8[0] >= '0' && str_Receiv->Data_a8[0] <= '9')
    {
        uint8_t length = 0;
        for(uint8_t i = 0; i < str_Receiv->Length_u16; i++)
        {
            if( str_Receiv->Data_a8[i] < '0' || str_Receiv->Data_a8[i]>'9') break;
            else length++;
        }
        TempU32 = Convert_String_To_Dec(str_Receiv->Data_a8 , length);
        if(TempU32 <= 32765)
        {
            sMeasure.Value = TempU32;
            Save_OffsetMeasure(_OFFSET_NTU, sMeasure);
            Modem_Respond(PortConfig, (uint8_t*)"OK", 2, 0);
        }
        else
        {
            Modem_Respond(PortConfig, (uint8_t*)"ERROR", 5, 0);
        }
    }
    else
    {
        Modem_Respond(PortConfig, (uint8_t*)"ERROR", 5, 0);
    }
}
#endif
/*===================Save and Init Offset Measure=================*/
void Save_OffsetMeasure(uint8_t KindOffset, Struct_SS_Value sMeasure)
{
#ifdef USING_INTERNAL_MEM
    uint8_t aData[20] = {0};
    uint8_t length = 0;

    aData[length++] = sOffsetMeasure.sClo_Du.Value >> 8;
    aData[length++] = sOffsetMeasure.sClo_Du.Value;
    
    aData[length++] = sOffsetMeasure.spH_Water.Value >> 8;
    aData[length++] = sOffsetMeasure.spH_Water.Value;
    
    aData[length++] = sOffsetMeasure.sNTU.Value >> 8;
    aData[length++] = sOffsetMeasure.sNTU.Value;
    
    aData[length++] = sOffsetMeasure.sSalinity.Value >> 8;
    aData[length++] = sOffsetMeasure.sSalinity.Value;
    
    aData[length++] = sOffsetMeasure.sTemperature.Value >> 8;
    aData[length++] = sOffsetMeasure.sTemperature.Value;
    
    aData[length++] = sOffsetMeasure.sEC.Value >> 8;
    aData[length++] = sOffsetMeasure.sEC.Value;
    
    switch(KindOffset)
    {
        case _OFFSET_CLO_DU:
          if(sMeasure.Value != 0)
              sOffsetMeasure.sClo_Du.Value = sMeasure.Value - sOffsetMeasure.sClo_Du_Sampling.Value;
          else
              sOffsetMeasure.sClo_Du.Value = 0;
          
          aData[0] = sOffsetMeasure.sClo_Du.Value >> 8;
          aData[1] = sOffsetMeasure.sClo_Du.Value;
          break;
          
        case _OFFSET_PH:
          if(sMeasure.Value != 0)
              sOffsetMeasure.spH_Water.Value = sMeasure.Value - sOffsetMeasure.spH_Water_Sampling.Value;
          else
              sOffsetMeasure.spH_Water.Value = 0;
          
          aData[2] = sOffsetMeasure.spH_Water.Value >> 8;
          aData[3] = sOffsetMeasure.spH_Water.Value;
          break;
          
        case _OFFSET_NTU:
          if(sMeasure.Value != 0)
              sOffsetMeasure.sNTU.Value = sMeasure.Value - sOffsetMeasure.sNTU_Sampling.Value;
          else
              sOffsetMeasure.sNTU.Value = 0;
          
          aData[4] = sOffsetMeasure.sNTU.Value >> 8;
          aData[5] = sOffsetMeasure.sNTU.Value;
          break;
          
        case _OFFSET_SALINITY:
          if(sMeasure.Value != 0)
              sOffsetMeasure.sSalinity.Value = sMeasure.Value - sOffsetMeasure.sSalinity_Sampling.Value;
          else
              sOffsetMeasure.sSalinity.Value = 0;
          
          aData[6] = sOffsetMeasure.sSalinity.Value >> 8;
          aData[7] = sOffsetMeasure.sSalinity.Value;
          break;
          
        case _OFFSET_TEMPERATURE:
          if(sMeasure.Value != 0)
              sOffsetMeasure.sTemperature.Value = sMeasure.Value - sOffsetMeasure.sTemperature_Sampling.Value;
          else
              sOffsetMeasure.sTemperature.Value = 0;
          
          aData[8] = sOffsetMeasure.sTemperature.Value >> 8;
          aData[9] = sOffsetMeasure.sTemperature.Value;
          break;
          
        case _OFFSET_EC:
          if(sMeasure.Value != 0)
              sOffsetMeasure.sEC.Value = sMeasure.Value - sOffsetMeasure.sEC_Sampling.Value;
          else
              sOffsetMeasure.sEC.Value = 0;
          
          aData[10] = sOffsetMeasure.sEC.Value >> 8;
          aData[11] = sOffsetMeasure.sEC.Value;
          break;
          
        default:
          break;
    }
    
    Save_Array(ADDR_OFFSET_MEASURE, aData, length);
#endif
}

void Init_OffsetMeasure(void)
{
#ifdef USING_INTERNAL_MEM
    if(*(__IO uint8_t*)(ADDR_OFFSET_MEASURE) != FLASH_BYTE_EMPTY)
    {
        sOffsetMeasure.sClo_Du.Value = *(__IO uint8_t*)(ADDR_OFFSET_MEASURE+2) <<8;
        sOffsetMeasure.sClo_Du.Value |= *(__IO uint8_t*)(ADDR_OFFSET_MEASURE+3);
        
        sOffsetMeasure.spH_Water.Value  = *(__IO uint8_t*)(ADDR_OFFSET_MEASURE+4) <<8;
        sOffsetMeasure.spH_Water.Value |= *(__IO uint8_t*)(ADDR_OFFSET_MEASURE+5);
        
        sOffsetMeasure.sNTU.Value = *(__IO uint8_t*)(ADDR_OFFSET_MEASURE+6) <<8;
        sOffsetMeasure.sNTU.Value |= *(__IO uint8_t*)(ADDR_OFFSET_MEASURE+7);
        
        sOffsetMeasure.sSalinity.Value = *(__IO uint8_t*)(ADDR_OFFSET_MEASURE+8) <<8;
        sOffsetMeasure.sSalinity.Value |= *(__IO uint8_t*)(ADDR_OFFSET_MEASURE+9);
        
        sOffsetMeasure.sTemperature.Value  = *(__IO uint8_t*)(ADDR_OFFSET_MEASURE+10) <<8;
        sOffsetMeasure.sTemperature.Value |= *(__IO uint8_t*)(ADDR_OFFSET_MEASURE+11);
        
        sOffsetMeasure.sEC.Value = *(__IO uint8_t*)(ADDR_OFFSET_MEASURE+12) <<8;
        sOffsetMeasure.sEC.Value |= *(__IO uint8_t*)(ADDR_OFFSET_MEASURE+13);
    }
    
    sOffsetMeasure.sClo_Du.Scale = DEFAULT_SCALE_CLO;
    sOffsetMeasure.sClo_Du_Sampling.Scale = DEFAULT_SCALE_CLO;
    
    sOffsetMeasure.spH_Water.Scale = DEFAULT_SCALE_PH;
    sOffsetMeasure.spH_Water_Sampling.Scale = DEFAULT_SCALE_PH;
    
    sOffsetMeasure.sNTU.Scale = DEFAULT_SCALE_NTU;
    sOffsetMeasure.sNTU_Sampling.Scale = DEFAULT_SCALE_NTU;
    
    sOffsetMeasure.sSalinity.Scale = DEFAULT_SCALE_SALINITY;
    sOffsetMeasure.sSalinity_Sampling.Scale = DEFAULT_SCALE_SALINITY;
    
    sOffsetMeasure.sTemperature.Scale = DEFAULT_SCALE_TEMPERATURE;
    sOffsetMeasure.sTemperature_Sampling.Scale = DEFAULT_SCALE_TEMPERATURE;
    
    sOffsetMeasure.sEC.Scale = DEFAULT_SCALE_EC;
    sOffsetMeasure.sEC_Sampling.Scale = DEFAULT_SCALE_EC;
#endif    
}

/*====================Save and Init Time Warning==================*/
void Save_TimeWarningSensor(uint8_t Duration)
{
#ifdef USING_INTERNAL_MEM
    uint8_t aData[8] = {0};
    uint8_t length = 0;
    
    DurationTimeWarningSensor = Duration;
    
    aData[length++] = DurationTimeWarningSensor;

    Save_Array(ADDR_TIME_WARNING_SENSOR, aData, length);
#endif
}

void Init_TimeWarningSensor(void)
{
#ifdef USING_INTERNAL_MEM
    if(*(__IO uint8_t*)(ADDR_TIME_WARNING_SENSOR) != FLASH_BYTE_EMPTY)
    {
        DurationTimeWarningSensor = *(__IO uint8_t*)(ADDR_TIME_WARNING_SENSOR+2);
    }
    else
    {
        DurationTimeWarningSensor = TIME_RESEND_WARNING;
    }
#endif    
}
/*======================Hanlde Sampling Sensor====================*/
void quickSort_Sampling(int16_t array_stt[],int16_t array_sampling[], uint8_t left, uint8_t right)
{
//	uint16_t pivot = array_sampling[(left+right)/2];
//	uint16_t low = left;
//    uint16_t high = right;
//	while (low < high)
//    {
//		while (array_sampling[low] < pivot) low++;
//        
//		while (array_sampling[high] > pivot) high--;
//        
//		if (low <= high)
//        {
//			int temp = 0;
//            temp = array_sampling[low];
//			array_sampling[low] = array_sampling[high];
//			array_sampling[high] = temp;
//            
//            temp = array_stt[low];
//			array_stt[low] = array_stt[high];
//			array_stt[high] = temp;
//            
//			low++;
//			high--;
//		}
//	}
//	if (low < right)
//		quickSort_Sampling_pH(array_stt, array_sampling, low, right);
//
//	if (left < high)
//		quickSort_Sampling_pH(array_stt, array_sampling, left, high);
  
/*---------------------- Sap xep noi bot --------------------*/
  
  for(uint8_t i = 0; i < NUMBER_SAMPLING_SS; i++)
  {
    for(uint8_t j = 0; j < NUMBER_SAMPLING_SS - 1; j++)
    {
        if(array_sampling[j] > array_sampling[j + 1])
        {
			int temp = 0;
            temp = array_sampling[j];
			array_sampling[j] = array_sampling[j+1];
			array_sampling[j+1] = temp;
            
            temp = array_stt[j];
			array_stt[j] = array_stt[j+1];
			array_stt[j+1] = temp;
        }
    }
  }
}

void quickSort_Sampling_pH(void)
{
    static uint8_t Handle_Once = 0;

    if(sSensor_pH.State_Connect == _SENSOR_DISCONNECT)
    {
        sDataSensorMeasure.spH_Water.Value = 0;
        Handle_Once = 0;
    }
    else
    {
        if(Handle_Once == 0)
        {
            Handle_Once = 1;
            for(uint8_t i = 0; i< NUMBER_SAMPLING_SS; i++)
            {
              aSampling_STT_pH[i] = i;
              
              if(aSampling_VALUE_pH[i] == 0)
                aSampling_VALUE_pH[i] = sSensor_pH.sPH_Value.Value;
            }
            
            quickSort_Sampling(aSampling_STT_pH, aSampling_VALUE_pH, 0, NUMBER_SAMPLING_SS - 1);
            sDataSensorMeasure.spH_Water.Value = aSampling_VALUE_pH[NUMBER_SAMPLING_SS/2];
        }
        else
        {
            for(uint8_t i = 0; i < NUMBER_SAMPLING_SS; i++)
            {
                if(aSampling_STT_pH[i] == NUMBER_SAMPLING_SS - 1)
                {
                    aSampling_STT_pH[i] = 0;
                    aSampling_VALUE_pH[i] = sSensor_pH.sPH_Value.Value;
                }
                else
                    aSampling_STT_pH[i] = aSampling_STT_pH[i] + 1;
            }

            quickSort_Sampling(aSampling_STT_pH, aSampling_VALUE_pH, 0, NUMBER_SAMPLING_SS - 1);
            sDataSensorMeasure.spH_Water.Value = aSampling_VALUE_pH[NUMBER_SAMPLING_SS/2];
        }
    }
}

void quickSort_Sampling_Clo(void)
{
    static uint8_t Handle_Once = 0;

    if(sSensor_Clo.State_Connect == _SENSOR_DISCONNECT)
    {
        sDataSensorMeasure.sClo_Du.Value = 0;
        Handle_Once = 0;
    }
    else
    {
        if(Handle_Once == 0)
        {
            Handle_Once = 1;
            for(uint8_t i = 0; i< NUMBER_SAMPLING_SS; i++)
            {
              aSampling_STT_Clo[i] = i;
              
              if(aSampling_VALUE_Clo[i] == 0)
                aSampling_VALUE_Clo[i] = sSensor_Clo.sClo_Value.Value;
            }
            
            quickSort_Sampling(aSampling_STT_Clo, aSampling_VALUE_Clo, 0, NUMBER_SAMPLING_SS - 1);
            sDataSensorMeasure.sClo_Du.Value = aSampling_VALUE_Clo[NUMBER_SAMPLING_SS/2];
        }
        else
        {
            for(uint8_t i = 0; i < NUMBER_SAMPLING_SS; i++)
            {
                if(aSampling_STT_Clo[i] == NUMBER_SAMPLING_SS - 1)
                {
                    aSampling_STT_Clo[i] = 0;
                    aSampling_VALUE_Clo[i] = sSensor_Clo.sClo_Value.Value;
                }
                else
                    aSampling_STT_Clo[i] = aSampling_STT_Clo[i] + 1;
            }

            quickSort_Sampling(aSampling_STT_Clo, aSampling_VALUE_Clo, 0, NUMBER_SAMPLING_SS - 1);
            sDataSensorMeasure.sClo_Du.Value = aSampling_VALUE_Clo[NUMBER_SAMPLING_SS/2];
        }
    }
}

void quickSort_Sampling_EC(void)
{
    static uint8_t Handle_Once = 0;

    if(sSensor_EC.State_Connect == _SENSOR_DISCONNECT)
    {
        sDataSensorMeasure.sEC.Value = 0;
        Handle_Once = 0;
    }
    else
    {
        if(Handle_Once == 0)
        {
            Handle_Once = 1;
            for(uint8_t i = 0; i< NUMBER_SAMPLING_SS; i++)
            {
              aSampling_STT_EC[i] = i;
              
              if(aSampling_VALUE_EC[i] == 0)
                aSampling_VALUE_EC[i] = sSensor_EC.sConductivity_Value.Value;
            }
            
            quickSort_Sampling(aSampling_STT_EC, aSampling_VALUE_EC, 0, NUMBER_SAMPLING_SS - 1);
            sDataSensorMeasure.sEC.Value = aSampling_VALUE_EC[NUMBER_SAMPLING_SS/2];
        }
        else
        {
            for(uint8_t i = 0; i < NUMBER_SAMPLING_SS; i++)
            {
                if(aSampling_STT_EC[i] == NUMBER_SAMPLING_SS - 1)
                {
                    aSampling_STT_EC[i] = 0;
                    aSampling_VALUE_EC[i] = sSensor_EC.sConductivity_Value.Value;
                }
                else
                    aSampling_STT_EC[i] = aSampling_STT_EC[i] + 1;
            }

            quickSort_Sampling(aSampling_STT_EC, aSampling_VALUE_EC, 0, NUMBER_SAMPLING_SS - 1);
            sDataSensorMeasure.sEC.Value = aSampling_VALUE_EC[NUMBER_SAMPLING_SS/2];
        }
    }
}

void quickSort_Sampling_Turbidity(void)
{
    static uint8_t Handle_Once = 0;

    if(sSensor_Turbidity.State_Connect == _SENSOR_DISCONNECT)
    {
        sDataSensorMeasure.sNTU.Value = 0;
        Handle_Once = 0;
    }
    else
    {
        if(Handle_Once == 0)
        {
            Handle_Once = 1;
            for(uint8_t i = 0; i< NUMBER_SAMPLING_SS; i++)
            {
              aSampling_STT_Turbidity[i] = i;
              
              if(aSampling_VALUE_Turbidity[i] == 0)
                aSampling_VALUE_Turbidity[i] = sSensor_Turbidity.sNTU_Value.Value;
            }
            
            quickSort_Sampling(aSampling_STT_Turbidity, aSampling_VALUE_Turbidity, 0, NUMBER_SAMPLING_SS - 1);
            sDataSensorMeasure.sNTU.Value = aSampling_VALUE_Turbidity[NUMBER_SAMPLING_SS/2];
        }
        else
        {
            for(uint8_t i = 0; i < NUMBER_SAMPLING_SS; i++)
            {
                if(aSampling_STT_Turbidity[i] == NUMBER_SAMPLING_SS - 1)
                {
                    aSampling_STT_Turbidity[i] = 0;
                    aSampling_VALUE_Turbidity[i] = sSensor_Turbidity.sNTU_Value.Value;
                }
                else
                    aSampling_STT_Turbidity[i] = aSampling_STT_Turbidity[i] + 1;
            }

            quickSort_Sampling(aSampling_STT_Turbidity, aSampling_VALUE_Turbidity, 0, NUMBER_SAMPLING_SS - 1);
            sDataSensorMeasure.sNTU.Value = aSampling_VALUE_Turbidity[NUMBER_SAMPLING_SS/2];
        }
    }
}
/*==========================Filter Measure=========================*/
int32_t Filter_Turbidity(int16_t var, uint8_t scale)
{
    //Kalman Filter
    static float x_est = 0.0;   // Uoc luong ban dau
    static float P = 1.0;       // Hiep phuong sai ban dau
    
    static float x_est_last = 0;
  
    float Q = 0.0000001;  // Nhieu mo hinh
    float R = 0.0001;   // Nhieu cam bien

    float x_pred, P_pred, K;
    
    float varFloat = 0;
    int32_t varInt32 = 0;
    
    if(var != 0)
    {
        varFloat = Handle_int32_To_Float_Scale(var, scale);
        
        //Thay doi nhanh du lieu
        if(x_est_last - varFloat > 0.2 || varFloat - x_est_last > 0.2)
        {
           Q *=1000; 
        }
        
        // Buoc du doan
        x_pred = x_est;
        P_pred = P + Q;

        // Tinh he so kalman
        K = P_pred / (P_pred + R);

        // Cap nhat gia tri
        x_est = x_pred + K * (varFloat - x_pred);
        P = (1 - K) * P_pred;
        
        varInt32 = Hanlde_Float_To_Int32_Scale_Round(x_est, scale);
    }
    else
    {
        P = 1;
        x_est = 0;
    }
    x_est_last = x_est;
    return varInt32;
}

int32_t Filter_CloDu(int16_t var, uint8_t scale)
{
    //Kalman_Filter
    static float x_est = 0.0;   // Uoc luong ban dau
    static float P = 1.0;       // Hiep phuong sai ban dau
    
    static float x_est_last = 0;
  
    float Q = 0.0000001;  // Nhieu mo hinh
    float R = 0.0001;   // Nhieu cam bien

    float x_pred, P_pred, K;
    
    float varFloat = 0;
    int32_t varInt32 = 0;
    
    if(var != 0)
    {
        varFloat = Handle_int32_To_Float_Scale(var, scale);
        
        //Thay doi nhanh du lieu
        if(x_est_last - varFloat > 0.2 || varFloat - x_est_last > 0.2)
        {
           Q *=1000; 
        }
        
        // Buoc du doan
        x_pred = x_est;
        P_pred = P + Q;

        // Tinh he so kalman
        K = P_pred / (P_pred + R);

        // Cap nhat gia tri
        x_est = x_pred + K * (varFloat - x_pred);
        P = (1 - K) * P_pred;
        
        varInt32 = Hanlde_Float_To_Int32_Scale_Round(x_est, scale);
    }
    else
    {
        P = 1;
        x_est = 0;
    }
    x_est_last = x_est;
    return varInt32;
}

int32_t Filter_pH(int16_t var, uint8_t scale)
{
    //Kalman Filter
    static float x_est = 0.0;   // Uoc luong ban dau
    static float P = 1.0;       // Hiep phuong sai ban dau
    
    static float x_est_last = 0;
  
    float Q = 0.0000001;  // Nhieu mo hinh
    float R = 0.0001;   // Nhieu cam bien

    float x_pred, P_pred, K;
    
    float varFloat = 0;
    int32_t varInt32 = 0;
    
    if(var != 0)
    {
        varFloat = Handle_int32_To_Float_Scale(var, scale);
        
        //Thay doi nhanh du lieu
        if(x_est_last - varFloat > 0.2 || varFloat - x_est_last > 0.2)
        {
           Q *=1000; 
        }
        
        // Buoc du doan
        x_pred = x_est;
        P_pred = P + Q;

        // Tinh he so kalman
        K = P_pred / (P_pred + R);

        // Cap nhat gia tri
        x_est = x_pred + K * (varFloat - x_pred);
        P = (1 - K) * P_pred;
        
        varInt32 = Hanlde_Float_To_Int32_Scale_Round(x_est, scale);
    }
    else
    {
        P = 1;
        x_est = 0;
    }
    x_est_last = x_est;
    return varInt32;
}

int32_t Filter_Salinity(int16_t var, uint8_t scale)
{
    return var;
}

int32_t Filter_Temperature(int16_t var, uint8_t scale)
{
    return var;
}

int32_t Filter_EC(int16_t var, uint8_t scale)
{
    return var;
}

/*=====================Handle Task and Init app===================*/
void Init_AppSensor(void)
{
    Init_TimeWarningSensor();
    Init_OffsetMeasure();
#ifdef USING_AT_CONFIG
    /* regis cb serial */
    CheckList_AT_CONFIG[_GET_FREQ_WARNING_SENSOR].CallBack = AT_CMD_Get_Time_Warning_Sensor;
    CheckList_AT_CONFIG[_SET_FREQ_WARNING_SENSOR].CallBack = AT_CMD_Set_Time_Warning_Sensor;
    
    CheckList_AT_CONFIG[_GET_OFFSET_CLO].CallBack = AT_CMD_Get_Offset_Clo;
    CheckList_AT_CONFIG[_SET_OFFSET_CLO].CallBack = AT_CMD_Set_Offset_Clo;
    
    CheckList_AT_CONFIG[_GET_OFFSET_PH].CallBack = AT_CMD_Get_Offset_pH;
    CheckList_AT_CONFIG[_SET_OFFSET_PH].CallBack = AT_CMD_Set_Offset_pH;
    
    CheckList_AT_CONFIG[_GET_OFFSET_NTU].CallBack = AT_CMD_Get_Offset_NTU;
    CheckList_AT_CONFIG[_SET_OFFSET_NTU].CallBack = AT_CMD_Set_Offset_NTU;
#endif
}

uint8_t AppSensor_Task(void)
{
    uint8_t i = 0;
    uint8_t Result = false;
    for( i = 0; i < _EVENT_SENSOR_END; i++)
    {
        if(sEventAppSensor[i].e_status == 1)
        {
            Result = true;
            if((sEventAppSensor[i].e_systick == 0) ||
                ((HAL_GetTick() - sEventAppSensor[i].e_systick) >= sEventAppSensor[i].e_period))
            {
                sEventAppSensor[i].e_status = 0; //Disable event
                sEventAppSensor[i].e_systick= HAL_GetTick();
                sEventAppSensor[i].e_function_handler(i);
            }
        }
    }
    
    return Result;
}

