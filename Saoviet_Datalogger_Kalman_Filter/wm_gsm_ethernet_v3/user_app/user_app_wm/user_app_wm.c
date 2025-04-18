

#include "user_app_wm.h"

#include "user_string.h"
#include "string.h"
#include "user_json.h"

#include "user_adc.h"
#include "user_timer.h"
#include "user_time.h"

#include "user_internal_mem.h"
#include "user_uart.h"

#include "user_app_comm.h"
#include "user_modbus_rtu.h"
#include "user_define.h"
#include "user_convert_variable.h"
/*================ Define =================*/
static uint8_t     _Cb_Entry_Wm (uint8_t event);
static uint8_t     _Cb_Log_TSVH (uint8_t event);
static uint8_t     _Cb_ext_irq (uint8_t event);
static uint8_t     _Cb_Dir_Change (uint8_t event);

static uint8_t     _Cb_Check_Mode (uint8_t event);
static uint8_t     _Cb_Meas_Pressure (uint8_t event);
static uint8_t     _Cb_Scan_Alarm (uint8_t event);
static uint8_t     _Cb_Control_Led1 (uint8_t event);
static uint8_t     _Cb_Read_RS485_Modbus(uint8_t event);
static uint8_t     _Cb_Read_RS485_2_Modbus(uint8_t event); 

static uint8_t     _Cb_Power_Up_12V(uint8_t event);

static uint8_t     _Cb_Get_Contact_Input(uint8_t event); 
static uint8_t     _Cb_Update_Meter_Data(uint8_t event);  


/*================ Struct =================*/
sEvent_struct sEventAppWM [] =
{
    { _EVENT_ENTRY_WM, 		    0, 0, 500, 	    _Cb_Entry_Wm }, 
        
    { _EVENT_LOG_TSVH, 		    0, 0, 500, 	    _Cb_Log_TSVH },
    { _EVENT_EXT_IRQ,           0, 0, 100, 	    _Cb_ext_irq },               //Ngat de 100ms chong nhieu
    { _EVENT_DIR_CHANGE,		0, 0, 0, 	    _Cb_Dir_Change },            //Chong nhieu 100ms -> Ton PIN    
    
    { _EVENT_CHECK_MODE,		1, 0, 1000,     _Cb_Check_Mode },           
    { _EVENT_MEAS_PRESSURE,		0, 0, 3000,     _Cb_Meas_Pressure }, 
    { _EVENT_SCAN_ALARM,	    1, 0, 1000,     _Cb_Scan_Alarm }, 
    { _EVENT_CONTROL_LED1,		1, 0, 200,      _Cb_Control_Led1 }, 
    
    { _EVENT_RS485_MODBUS,		0, 0, 500,      _Cb_Read_RS485_Modbus }, 
    { _EVENT_RS485_2_MODBUS,    1, 0, 500,      _Cb_Read_RS485_2_Modbus }, 
    
    { _EVENT_POWER_UP_12V,      0, 0, 100,      _Cb_Power_Up_12V },
    
    { _EVENT_GET_CONTACT_IN,    0, 0, 1000,     _Cb_Get_Contact_Input },
    
    { _EVENT_UPDATE_METER,		1, 0, 1000,     _Cb_Update_Meter_Data }, 
};
            
    
Struct_Battery_Status    sBattery;
Struct_Battery_Status    sVout;
Struct_Battery_Status    sPressure;
Struct_Pulse			 sPulse[MAX_CHANNEL];

struct_ThresholdConfig   sMeterThreshold =
{
    .FlowHigh = 0xFFFF,
    .FlowLow = 0,
    .PeakHigh = 0xFFFF,
    .PeakLow = 0,
    .LowBatery = 10,
    .LevelHigh = 4000,
    .LevelLow  = 10,
};

sLastRecordInfor    sLastPackInfor = { {0}, 0};

static uint8_t aMARK_ALARM[10];
static uint32_t aLandMarkAlarm_u32[10];

//uint32_t *pMarkSoftResetMCU = (uint32_t *) (&sModem.MarkResetChip_u8);
//uint32_t *pPulseLPTIM = (uint32_t *) (&sPulse.Number_u32);


sAppWmVariable sWmVar = 
{
    .nChannel_u8 = MAX_CHANNEL,
};


static UTIL_TIMER_Object_t TimerLevel;
static void OnTimerLevelPowerOn(void *context);

static UTIL_TIMER_Object_t TimerRs485_2;
static void OnTimerRs485_2(void *context);


static GPIO_TypeDef*  LED_PORT[1] = {LED_GPIO_Port};
static const uint16_t LED_PIN[1] = {LED_Pin};

static UTIL_TIMER_Object_t Timer12VPowerUp;
static void OnTimer12VPowerUp(void *context);

extern sData sUart485;
extern sData sUart485_2;

static uint8_t PressureKey_u8 = 0xFF;

/*================ Struct =================*/


void AppWm_Init (void)
{
    //Init xung trong flash
#if ( defined (STM32L433xx) || defined (STM32L452xx) || defined (STM32L496xx) )  && (defined (BOARD_QN_V3_2) || defined (BOARD_QN_V4_0) )
    AppWm_Init_Pulse_Infor();
    AppWm_Button_Default_Device();
#endif
    AppWm_Init_Thresh_Measure();
    
    //Set timer power on level power
    UTIL_TIMER_Create(&TimerLevel, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnTimerLevelPowerOn, NULL);
    UTIL_TIMER_SetPeriod(&TimerLevel, 10000);
    
    //Set timer power on level power
    UTIL_TIMER_Create(&TimerRs485_2, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnTimerRs485_2, NULL);
    
    //Set timer power on level power
    UTIL_TIMER_Create(&Timer12VPowerUp, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnTimer12VPowerUp, NULL);
    UTIL_TIMER_SetPeriod(&Timer12VPowerUp, 10000);
    
#ifdef USING_AT_CONFIG
    CheckList_AT_CONFIG[_SET_DEFAULT_PULSE].CallBack = AppWm_SER_Set_Default_Pulse;
    
    CheckList_AT_CONFIG[_SET_FLOW_THRESH].CallBack = AppWm_SER_Set_Thresh_Flow;
    CheckList_AT_CONFIG[_QUERY_FLOW_THRESH].CallBack = AppWm_SER_Get_Thresh_Flow;
    
    CheckList_AT_CONFIG[_SET_QUAN_THRESH].CallBack = AppWm_SER_Set_Thresh_Quan;
    CheckList_AT_CONFIG[_QUERY_QUAN_THRESH].CallBack = AppWm_SER_Get_Thresh_Quan;
    
    CheckList_AT_CONFIG[_SET_BAT_THRESH].CallBack = AppWm_SER_Set_Thresh_Bat;
    CheckList_AT_CONFIG[_QUERY_BAT_THRESH].CallBack = AppWm_SER_Get_Thresh_Bat;
    
    CheckList_AT_CONFIG[_SET_PULSE_VALUE].CallBack = AppWm_SER_Set_Pulse;
    CheckList_AT_CONFIG[_QUERY_PULSE_VALUE].CallBack = AppWm_SER_Get_Pulse;
    
    CheckList_AT_CONFIG[_SET_LEVEL_THRESH].CallBack = AppWm_SER_Set_Level_Thresh;
    CheckList_AT_CONFIG[_QUERY_LEVEL_THRESH].CallBack = AppWm_SER_Get_Level_Thresh;
    
    CheckList_AT_CONFIG[_SET_NUM_CHANNEL].CallBack = AppWm_SER_Set_Num_Channel;
    CheckList_AT_CONFIG[_QUERY_NUM_CHANNEL].CallBack = AppWm_SER_Get_Num_Channel;
    
    CheckList_AT_CONFIG[_SET_MODBUS_INFOR].CallBack = AppWm_SER_Set_Modbus_Infor;
    CheckList_AT_CONFIG[_QUERY_MODBUS_INFOR].CallBack = AppWm_SER_Get_Modbus_Infor;
    
    CheckList_AT_CONFIG[_SET_PRESS_INFOR].CallBack = AppWm_SER_Set_Pressure_Infor;
    CheckList_AT_CONFIG[_QUERY_PRESS_INFOR].CallBack = AppWm_SER_Get_Pressure_Infor;
#endif
    
    AppWm_Init_WM_Dig_Infor();
    
    AppWm_Init_Default_Pressure();
    AppWm_Init_Press_Infor();
}


uint8_t AppWm_Task(void)
{
	uint8_t i = 0;
	uint8_t Result = 0;

	for (i = 0; i < _EVENT_END_WM; i++)
	{
		if (sEventAppWM[i].e_status == 1)
		{
            if ( (i != _EVENT_CHECK_MODE) 
                  && (i != _EVENT_SCAN_ALARM) 
                  && (i != _EVENT_CONTROL_LED1)
                  && (i != _EVENT_UPDATE_METER) 
                  && (i != _EVENT_GET_CONTACT_IN) )
                Result = 1;

			if ((sEventAppWM[i].e_systick == 0) ||
					((HAL_GetTick() - sEventAppWM[i].e_systick)  >=  sEventAppWM[i].e_period))
			{
                sEventAppWM[i].e_status = 0;  //Disable event
				sEventAppWM[i].e_systick = HAL_GetTick();
				sEventAppWM[i].e_function_handler(i);
			}
		}
	}
    
	return Result;
}



static uint8_t _Cb_Entry_Wm (uint8_t event)
{
    fevent_active(sEventAppWM, _EVENT_MEAS_PRESSURE); 
//    fevent_active(sEventAppWM, _EVENT_RS485_MODBUS);
    fevent_active(sEventAppWM, _EVENT_RS485_2_MODBUS);
    fevent_active(sEventAppWM, _EVENT_GET_CONTACT_IN); 
    
	return 1;
}


static uint8_t _Cb_Log_TSVH (uint8_t event)
{
	//Log Data meter to flash
    UTIL_Printf_Str(DBLEVEL_M, "u_app_wm:log tsvh data\r\n");
    //
    Get_RTC();

	AppWm_Get_VBat();
    AppWm_Get_VOut();
    
    APP_LOG(TS_OFF, DBLEVEL_M, "u_app_wm: Vpin in: %d mV \r\n", sBattery.mVol_u32);
    APP_LOG(TS_OFF, DBLEVEL_M, "u_app_wm: Vpin out: %d mV \r\n", sVout.mVol_u32);
    
    #ifdef LPTIM_ENCODER_MODE 
        LPTIM_Encoder_Get_Pulse(&hlptim1, &sPulse[0]);
    #else   
        LPTIM_Counter_Get_Pulse(&hlptim1, &sPulse[0]);
        AppWm_Plus_5Pulse(&sPulse[0]);
    #endif
        
    #ifdef LPTIM2_ENCODER_MODE 
        LPTIM_Encoder_Get_Pulse(&hlptim2, &sPulse[1]);
    #else   
        LPTIM_Counter_Get_Pulse(&hlptim2, &sPulse[1]);
        AppWm_Plus_5Pulse(&sPulse[1]);
    #endif
      
    TIM_Counter_Get_Pulse(&htim1, &sPulse[2]);  
    TIM_Counter_Get_Pulse(&htim2, &sPulse[3]);
        
#ifdef ENABLE_TEST_HARDWARE
    APP_LOG(TS_OFF, DBLEVEL_M, "u_app_wm: pulse: %d, %d, %d, %d \r\n", 
                                    sPulse.Number_u32, sPulse[1].Number_u32, sPulse[2].Number_u32, sPulse[3].Number_u32 );
#endif
    
    //Luu lai xung
    AppWm_Save_Pulse();
    
    if (sRTC.year < 20)
    {
    	return 0;
    }
    
    //Get Last Packet Infor
    AppWm_Get_Last_Pulse();  
    //Packet TSVH and push to mem
//    AppWm_Log_Data_TSVH();  
//    AppWm_Packet_Modbus();
    
    AppSensor_Log_Data_TSVH();

	return 1;
}


static uint8_t _Cb_ext_irq(uint8_t event)
{
    UTIL_Printf_Str(DBLEVEL_M, "u_app_wm: irq send intan\r\n");

	sWmVar.rIntan_u8 = true;
	//Active event Measure Press
	fevent_active(sEventAppWM, _EVENT_ENTRY_WM);
    // uu tien khoi dong ethernet truoc
    #ifdef USING_APP_ETHERNET    
        AppEth_Restart_If_PSM();
    #endif
    
    #ifdef USING_APP_SIM
        AppSim_Restart_If_PSM(); 
        
        if (sWmVar.pRq_Instant != NULL)
            sWmVar.pRq_Instant();
    #endif

    return 1;
}


static uint8_t _Cb_Dir_Change(uint8_t event)
{
    char aData[48] = {0}; 
    
    sprintf(aData, "u_app_wm: dir change: %d, %d\r\n", sPulse[0].Direct_u8, sPulse[1].Direct_u8);
    
    if (sWmVar.pPack_Alarm_Str != NULL)
        sWmVar.pPack_Alarm_Str(aData);
    
    //Neu nhu bi nhieu vao day nhieu qua thi nen bo qua?
    aMARK_ALARM[_ALARM_DIR_CHANGE] = TRUE;
    //Dong goi ban tin Event
    AppWm_Log_Data_Event ();
   
    return 1;
}


static uint8_t _Cb_Check_Mode(uint8_t event)
{
    //Check PIN VBAT_Detect to decide _MODE_ONLINE or _MODE_RUNNING
    #ifdef USING_APP_SIM
    if ((*sAppSimVar.pModeConnNow_u8 == MODE_CONNECT_DATA_MAIN) 
        || (*sAppSimVar.pModeConnNow_u8 == MODE_CONNECT_DATA_BACKUP))
    {
        if (sAppSimVar.StepPowerOn_u8 == 0)
            AppWm_Check_Mode_Power();    
    }
    #endif

    fevent_enable(sEventAppWM, event);

    return 1;
}


static uint8_t _Cb_Meas_Pressure(uint8_t event)
{       
    if ( (PressureKey_u8 != event) && (PressureKey_u8 != 0xFF) )
    {
        sEventAppWM[event].e_period = 1000;
        fevent_enable(sEventAppWM, event);
        
        return 1;
    }
       
    PressureKey_u8 = event;
    uint8_t Status = AppWm_Meas_Pressure_Process(&sEventAppWM[event].e_period);
               
    if (Status == true)
    {       
        PressureKey_u8 = 0xFF;
        
        for (uint8_t i = 0; i < MAX_CHANNEL; i++)
        {
        #ifdef ENABLE_TEST_HARDWARE
            APP_LOG(TS_OFF, DBLEVEL_M, "u_app_wm: pressure vol %d: %d mV \r\n", i, sWmVar.aPRESSURE[i].mVol_u32);
        #endif

        #ifdef PRESSURE_DECODE
            APP_LOG(TS_OFF, DBLEVEL_M, "u_app_wm: pressure chan.%d: %d mbar \r\n", i, sWmVar.aPRESSURE[i].Val_i32);
        #else
            APP_LOG(TS_OFF, DBLEVEL_M, "u_app_wm: pressure chan.%d: %d mV \r\n", i, sWmVar.aPRESSURE[i].Val_i32);
        #endif
        }    
                    
        //neu co req Intan -> send intan | Log TSVH de gui len server
        if (sWmVar.rIntan_u8 == TRUE)
        {
            sWmVar.rIntan_u8 = false;
        #ifdef USING_APP_SIM
            sMQTT.aMARK_MESS_PENDING[DATA_INTAN] = TRUE;
        #endif
        } else
        {
            fevent_active(sEventAppWM, _EVENT_LOG_TSVH);
        }
        
    #ifdef USING_APP_SIM
        if (sWmVar.pCtrl_Sim != NULL)
            sWmVar.pCtrl_Sim(false);
    #endif
    } else
    {     
    #ifdef USING_APP_SIM
        if (sWmVar.pCtrl_Sim != NULL)
            sWmVar.pCtrl_Sim(true);
    #endif
        
        fevent_enable(sEventAppWM, event);
    }

    return 1;
}




static uint8_t _Cb_Scan_Alarm(uint8_t event)
{   
    static uint8_t cCheck = 0;
    
    Get_RTC();
    
    AppWm_Get_VBat();
    AppWm_Get_VOut();
        
    #ifdef LPTIM_ENCODER_MODE 
        LPTIM_Encoder_Get_Pulse(&hlptim1, &sPulse[0]);
    #else   
        LPTIM_Counter_Get_Pulse(&hlptim1, &sPulse[0]);
        AppWm_Plus_5Pulse(&sPulse[0]);
    #endif
    
    #ifdef LPTIM2_ENCODER_MODE 
        LPTIM_Encoder_Get_Pulse(&hlptim2, &sPulse[1]);
    #else   
        LPTIM_Counter_Get_Pulse(&hlptim2, &sPulse[1]);
        AppWm_Plus_5Pulse(&sPulse[1]);
    #endif
        
    TIM_Counter_Get_Pulse(&htim1, &sPulse[2]);  
    TIM_Counter_Get_Pulse(&htim2, &sPulse[3]);
    
    //tinh toan luu luong
    AppWm_Cacul_Flow(0);
    AppWm_Cacul_Flow(1);
    AppWm_Cacul_Flow(2);
    AppWm_Cacul_Flow(3);
    //
    if (cCheck++ >= 5)
    {
        cCheck = 0;
        if (sRTC.year > 20)
        {
            // Cacul and check alarm: Vbat, Threshold flow, peak, Dir change
            if (AppWm_Scan_Alarm() == 1)   
            {
                //Dong goi Event Mess
                AppWm_Log_Data_Event ();
            }
        }
    }
            
    fevent_enable(sEventAppWM, event);
        
	return 1;
}


void AppWm_Cacul_Flow (uint8_t chann)
{
    static ST_TIME_FORMAT LandmarkTime_u32[MAX_CHANNEL] = {0};
    static uint32_t LastPulse_u32[MAX_CHANNEL] = {0};
    static uint32_t DeltaTime[MAX_CHANNEL] = {0};
    static int32_t DeltaPulse[MAX_CHANNEL] = {0};
    static uint32_t PlusTime[MAX_CHANNEL] = {0};
    double TempDouble = 0;
    
    uint32_t LastTimeSecond = 0, CurrTimeSecond = 0; 
    uint8_t rCacul = false, newpulse = false;
    
    if (chann >= MAX_CHANNEL)
        return;
    
    LastTimeSecond = HW_RTC_GetCalendarValue_Second(LandmarkTime_u32[chann], 0);
    CurrTimeSecond = HW_RTC_GetCalendarValue_Second(sRTC, 0);
            
    if (sPulse[chann].Number_u32 != LastPulse_u32[chann])
    {
        DeltaPulse[chann] = AppWm_Cacul_Quantity(sPulse[chann].Number_u32, LastPulse_u32[chann]);
        LastPulse_u32[chann] = sPulse[chann].Number_u32;
        
        rCacul = true;
        newpulse = true;
        PlusTime[chann] = 0;
    } else
    {
        //qua thoi gian duration xung truoc moi cho giam dan
        if ( (CurrTimeSecond > LastTimeSecond)
            && ( (CurrTimeSecond - LastTimeSecond) > (DeltaTime[chann] + PlusTime[chann]) ) )
        {
            rCacul = true;
            PlusTime[chann] += 10;   //them 10s sau moi check
        } 
    }
    
    if ( (rCacul == true) 
         && (LandmarkTime_u32[chann].year != 0) 
         && (CurrTimeSecond > LastTimeSecond) )
    {
        //tinh ra luu luong
        TempDouble = DeltaPulse[chann] * 3600 * Convert_Scale(sPulse[chann].Factor_u8 + 2) / (CurrTimeSecond - LastTimeSecond);  
        sPulse[chann].Flow_i32 = (int32_t) TempDouble ;
    }
    
    //
    if (newpulse == true)
    {
        DeltaTime[chann] = CurrTimeSecond - LastTimeSecond;
        UTIL_MEM_cpy(&LandmarkTime_u32[chann], &sRTC, sizeof(ST_TIME_FORMAT));
    }
}



/*
    Func: Set Mode Control Led
        - Mode: + _LED_MODE_ONLINE_INIT     : Nhay deu 1s
                + _LED_MODE_CONNECT_SERVER  : Nhay Duty: 430ms off, 70ms on
                + _LED_MODE_UPDATE_FW       : Nhay deu 100ms
                + _LED_MODE_POWER_SAVE      : Off led
                + _LED_MODE_TEST_PULSE      : Nhay theo xung doc vao
*/
uint8_t AppWm_Get_Mode_Led (void)
{
    uint8_t Result = 0;
    
    #ifdef USING_APP_SIM
        if ((sModemVar.ModeConnNow_u8 != MODE_CONNECT_DATA_MAIN) 
            && (sModemVar.ModeConnNow_u8 != MODE_CONNECT_DATA_BACKUP))  
        {
            Result = _LED_MODE_UPDATE_FW;
        } else if (sWmVar.rTestsPulse_u8 == FALSE)
        {
            if ( (sSimCommVar.State_u8 == _SIM_CONN_MQTT)
            #ifdef USING_APP_ETHERNET
                || (sAppEthVar.Status_u8 == _ETH_MQTT_CONNECTED) 
            #endif
                    )
                Result = _LED_MODE_CONNECT_SERVER;
            else
                Result = _LED_MODE_ONLINE_INIT;
        } else
            Result = _LED_MODE_TEST_PULSE;
    #else
        if (sWmVar.rTestsPulse_u8 == FALSE) 
        {
            Result = _LED_MODE_ONLINE_INIT;
        } else
            Result = _LED_MODE_TEST_PULSE;  
    #endif
        
    return Result;
}

/*
    Func: CB Event Control Led
*/
static uint8_t _Cb_Control_Led1(uint8_t event)
{
    uint8_t ModeControl = 0;
    static uint8_t CountToggle = 0;
    static uint8_t MarkFirst = 0;
    static uint8_t Status = 0;
    static uint32_t LastPulseControl = 0;
    static uint32_t LastTimeCheckPulse = 0;
    
    if ( (sWmVar.rDefault_u8 == true) && (CountToggle <= 10) )
    {
        ModeControl = _LED_MODE_DEFAULT_DEV;
    } else
    {
        ModeControl = AppWm_Get_Mode_Led();
    }
    
    //Handler in step control one
    switch ( ModeControl )
    {
        case _LED_MODE_DEFAULT_DEV: 
            CountToggle++;
            sEventAppWM[event].e_period = 100;
            LED_Toggle(_LED_STATUS);
            break;
        case _LED_MODE_ONLINE_INIT:
            sEventAppWM[event].e_period = 1000;
            LED_Toggle(_LED_STATUS);
            break;
        case _LED_MODE_CONNECT_SERVER:
            if (sEventAppWM[event].e_period != 430)
            {
                sEventAppWM[event].e_period = 430;
                LED_Off (_LED_STATUS);
            } else
            {
                sEventAppWM[event].e_period = 70;
                LED_On (_LED_STATUS);
            }
            break;
        case _LED_MODE_UPDATE_FW:
            sEventAppWM[event].e_period = 100;
            LED_Toggle(_LED_STATUS);
            break;      
        case _LED_MODE_POWER_SAVE:
            sEventAppWM[event].e_period = 1000;
            LED_Off (_LED_STATUS);
            break;
        case _LED_MODE_TEST_PULSE:
            sEventAppWM[event].e_period = 200;
            if (MarkFirst == 0)
            {
                MarkFirst = 1;
                LastPulseControl = sPulse[0].Number_u32;
                LastTimeCheckPulse = HAL_GetTick();
                LED_On (_LED_STATUS);
                
                return 1;
            }
            
            #ifdef LPTIM_ENCODER_MODE 
                LPTIM_Encoder_Get_Pulse(&hlptim1, &sPulse[0]);
            #else   
                LPTIM_Counter_Get_Pulse(&hlptim1, &sPulse[0]);
                AppWm_Plus_5Pulse(&sPulse[0]);
            #endif
                
            if (Cal_Time(LastTimeCheckPulse, HAL_GetTick()) <= 60000)
            {
                //Neu co xung nuoc thay doi -> Nhay led
                if (LastPulseControl != sPulse[0].Number_u32)
                {
                    LastPulseControl = sPulse[0].Number_u32;
                    
                    if (Status == FALSE)
                        Status = TRUE;
                } 
                
                if (Status == TRUE)
                {
                    LED_Off (_LED_STATUS);
                    Status = PENDING;
                } else if (Status == PENDING)   
                {
                    LED_On (_LED_STATUS);
                    Status = FALSE;
                }
            }
        default:
            break;
    }
    
    fevent_enable(sEventAppWM, event);
       
    return 1;
}


uint32_t Clo_Du = 0;
uint32_t pH_Water = 0;
uint32_t NTU = 0 ;
uint32_t Salinity = 0;
uint32_t Temperature = 0;

static uint8_t _Cb_Read_RS485_Modbus (uint8_t event)
{
    static uint8_t retry = 0;
    static uint8_t step = 0;
    uint8_t isfinish_u8 = FALSE;
    sData ModContent;
    
    if (retry < 2)
    {
        switch (step)
        {
            case 0:
                V12_IN_OFF; 
                V_PIN_ON;  //On power: cap nguon cho level
                RS485_ON;
                
                //Init uart 485
                RS485_Stop_RX_Mode();
                WM_DIG_Init_Uart(&uart_rs485, sWmDigVar.sModbInfor[0].MType_u8);
                RS485_Init_RX_Mode();
                
                UTIL_TIMER_SetPeriod(&TimerLevel, 2000);
                UTIL_TIMER_Start (&TimerLevel);
                step++;
                break;
            case 1:  //Send request frame modrtu 
                RS485_Modbus_Read_Value(sWmDigVar.sModbInfor[0].SlaveId_u8,
                                        sListWmDigital[sWmDigVar.sModbInfor[0].MType_u8].Addr_u32, 
                                        sListWmDigital[sWmDigVar.sModbInfor[0].MType_u8].MaxRegis_u8);
                
                UTIL_TIMER_SetPeriod(&TimerLevel, 1000);
                UTIL_TIMER_Start (&TimerLevel);
                step++;
                break;
            case 2: //Check ACK
                retry++;
                if ((Rs485Status_u8 == TRUE) 
                    && (RS485_Modbus_Check_Format(sWmDigVar.sModbInfor[0].SlaveId_u8, 
                                                  sListWmDigital[sWmDigVar.sModbInfor[0].MType_u8].MaxRegis_u8, 
                                                  &sUart485, &ModContent) == true))
                {                   
                    //Dong goi ban tin tsvh cho modbus
                    APP_LOG(TS_OFF, DBLEVEL_M, "u_app_wm: modbus data: ");
                    
                    for (uint16_t i = 0; i < ModContent.Length_u16; i++)
                    {
                        APP_LOG(TS_OFF, DBLEVEL_M, "%x ", *(ModContent.Data_a8 + i) );  
                    }
                    APP_LOG(TS_OFF, DBLEVEL_M, " \r\n");
                    
                    retry = 0;
                    
                    //decode
                    sListWmDigital[sWmDigVar.sModbInfor[0].MType_u8].fDecode(&ModContent);
                    //get data to channel modbus de hien thị
                    WM_DIG_Get_Data(0, sWmDigVar.sModbInfor[0].MType_u8);
                    
                    isfinish_u8 = TRUE;
                    
                    
                      Clo_Du = sUart485.Data_a8[5]<<8 | sUart485.Data_a8[6];
                      Clo_Du = (Clo_Du << 16) | (sUart485.Data_a8[3]<<8 | sUart485.Data_a8[4]);
                      
                      pH_Water = sUart485.Data_a8[13]<<8 | sUart485.Data_a8[14];
                      pH_Water = (pH_Water << 16) | (sUart485.Data_a8[11]<<8 | sUart485.Data_a8[12]);
                      
                      NTU = sUart485.Data_a8[21]<<8 | sUart485.Data_a8[22];
                      NTU = (NTU << 16) | (sUart485.Data_a8[19]<<8 | sUart485.Data_a8[20]);
                      
                      Salinity = sUart485.Data_a8[29]<<8 | sUart485.Data_a8[30];
                      Salinity = (Salinity << 16) | (sUart485.Data_a8[27]<<8 | sUart485.Data_a8[28]);
                      
                      Temperature = sUart485.Data_a8[37]<<8 | sUart485.Data_a8[38];
                      Temperature = (Temperature << 16) | (sUart485.Data_a8[35]<<8 | sUart485.Data_a8[36]);
                      
                      Handle_Data_CM44(Clo_Du, pH_Water, NTU, Salinity, Temperature);
                       
                      sHandleRs485.CountDisconnectRS485_1 = 0;
                    break;
                }
                
                UTIL_TIMER_SetPeriod(&TimerLevel, 500);
                UTIL_TIMER_Start (&TimerLevel);

                step = 1;    //chi quay lai buoc hoi lenh
                break;
            default:
                step = 0;
                break;
        }
    } else
    {
        retry = 0;
        Rs485Status_u8 = FALSE;
        isfinish_u8 = TRUE;
        
        if (sWmDigVar.sModbDevData[0].Status_u8 == true) {
            if (sWmVar.pPack_Alarm_Str != NULL)
                sWmVar.pPack_Alarm_Str ("u_app_wm: read modbus fail!\r\n");
        } else {
            UTIL_Printf_Str(DBLEVEL_M, "u_app_wm: read modbus fail!\r\n" );
        }
        
        sWmDigVar.sModbDevData[0].Status_u8 = false;
        
        sHandleRs485.CountDisconnectRS485_1++;
        if(sHandleRs485.CountDisconnectRS485_1 > 3)
            sHandleRs485.CountDisconnectRS485_1 =3;
        
        if(sHandleRs485.CountDisconnectRS485_1 >=3 && sHandleRs485.CountDisconnectRS485_2 >=3)
        {
            sDataSensorMeasure.sClo_Du.Value   = 0;
            sDataSensorMeasure.spH_Water.Value = 0;
            sDataSensorMeasure.sNTU.Value      = 0;
            sDataSensorMeasure.sSalinity.Value = 0;
            sDataSensorMeasure.sTemperature.Value = 0;
        }
    }
    
    if (isfinish_u8 == TRUE)
    {
        step = 0;
        
        RS485_Stop_RX_Mode();
        
        if (sModemInfor.ModePower_u8 == _POWER_MODE_SAVE)
        {
            V_PIN_OFF;   //off Power
            RS485_OFF;   //Off 485 de go to sleep mode
        } else {
            sEventAppWM[event].e_period = 1000;
            fevent_enable(sEventAppWM, event);
        }
    }
    
    return 1;
}




static uint8_t _Cb_Read_RS485_2_Modbus (uint8_t event)
{
    static uint8_t retry = 0;
    static uint8_t step = 0;
    uint8_t isfinish_u8 = FALSE;
    sData ModContent;
    
    if (retry < 2)
    {
        switch (step)
        {
            case 0:
                V12_IN_OFF; 
                V_PIN_ON;  //On power: cap nguon cho level
                RS485_ON;
                
                //Init uart 485
                RS485_2_Stop_RX_Mode();
                WM_DIG_Init_Uart(&uart_rs485_2, sWmDigVar.sModbInfor[1].MType_u8);
                RS485_2_Init_RX_Mode();
                
                UTIL_TIMER_SetPeriod(&TimerRs485_2, 2000);
                UTIL_TIMER_Start (&TimerRs485_2);
                step++;
                break;
            case 1:  //Send request frame modrtu 
                RS485_2_Modbus_Read_Value(sWmDigVar.sModbInfor[1].SlaveId_u8,
                                            sListWmDigital[sWmDigVar.sModbInfor[1].MType_u8].Addr_u32,
                                            sListWmDigital[sWmDigVar.sModbInfor[1].MType_u8].MaxRegis_u8);
                
                UTIL_TIMER_SetPeriod(&TimerRs485_2, 500);
                UTIL_TIMER_Start (&TimerRs485_2);
                step++;
                break;
            case 2: //Check ACK
                retry++;
                if ((Rs485_2Status_u8 == TRUE) 
                    && (RS485_Modbus_Check_Format(sWmDigVar.sModbInfor[1].SlaveId_u8, 
                                                  sListWmDigital[sWmDigVar.sModbInfor[1].MType_u8].MaxRegis_u8, 
                                                  &sUart485_2, &ModContent) == true))
                {                   
                    //Dong goi ban tin tsvh cho modbus
                    APP_LOG(TS_OFF, DBLEVEL_M, "u_app_wm: modbus 2 data: ");
                    
                    for (uint16_t i = 0; i < ModContent.Length_u16; i++)
                    {
                        APP_LOG(TS_OFF, DBLEVEL_M, "%x ", *(ModContent.Data_a8 + i) );  
                    }
                    APP_LOG(TS_OFF, DBLEVEL_M, " \r\n");
                    
                    retry = 0;
                    
                    //decode
                    sListWmDigital[sWmDigVar.sModbInfor[1].MType_u8].fDecode(&ModContent);
                    //get data to channel modbus de hien thị
                    WM_DIG_Get_Data(1, sWmDigVar.sModbInfor[1].MType_u8);
                    
                    isfinish_u8 = TRUE;
                    
                      Clo_Du = sUart485_2.Data_a8[5]<<8 | sUart485_2.Data_a8[6];
                      Clo_Du = (Clo_Du << 16) | (sUart485_2.Data_a8[3]<<8 | sUart485_2.Data_a8[4]);
                      
                      pH_Water = sUart485_2.Data_a8[13]<<8 | sUart485_2.Data_a8[14];
                      pH_Water = (pH_Water << 16) | (sUart485_2.Data_a8[11]<<8 | sUart485_2.Data_a8[12]);
                      
                      NTU = sUart485_2.Data_a8[21]<<8 | sUart485_2.Data_a8[22];
                      NTU = (NTU << 16) | (sUart485_2.Data_a8[19]<<8 | sUart485_2.Data_a8[20]);
                      
                      Salinity = sUart485_2.Data_a8[29]<<8 | sUart485_2.Data_a8[30];
                      Salinity = (Salinity << 16) | (sUart485_2.Data_a8[27]<<8 | sUart485_2.Data_a8[28]);
                      
                      Temperature = sUart485_2.Data_a8[37]<<8 | sUart485_2.Data_a8[38];
                      Temperature = (Temperature << 16) | (sUart485_2.Data_a8[35]<<8 | sUart485_2.Data_a8[36]);
                      
                      Handle_Data_CM44(Clo_Du, pH_Water, NTU, Salinity, Temperature);
                      sHandleRs485.CountDisconnectRS485_2 = 0;
                      
                    break;
                }
                
                UTIL_TIMER_SetPeriod(&TimerRs485_2, 500);
                UTIL_TIMER_Start (&TimerRs485_2);

                step = 1;    //chi quay lai buoc hoi lenh
                break;
            default:
                step = 0;
                break;
        }
    } else
    {
        retry = 0;
        Rs485_2Status_u8 = FALSE;
        isfinish_u8 = TRUE;
        
        if (sWmDigVar.sModbDevData[1].Status_u8 == true) {
            if (sWmVar.pPack_Alarm_Str != NULL)
                sWmVar.pPack_Alarm_Str ("u_app_wm: read modbus 2 fail!\r\n");
        } else {
            UTIL_Printf_Str(DBLEVEL_M, "u_app_wm: read modbus 2 fail!\r\n" );
        }
        
        sWmDigVar.sModbDevData[1].Status_u8 = false; 
        
        sHandleRs485.CountDisconnectRS485_2++;
        if(sHandleRs485.CountDisconnectRS485_2 > 3)
            sHandleRs485.CountDisconnectRS485_2 =3;
        
        if(sHandleRs485.CountDisconnectRS485_1 >=3 && sHandleRs485.CountDisconnectRS485_2 >=3)
        {
            
            sDataSensorMeasure.sClo_Du.Value   = 0;
            sDataSensorMeasure.spH_Water.Value = 0;
            sDataSensorMeasure.sNTU.Value      = 0;
            sDataSensorMeasure.sSalinity.Value = 0;
            sDataSensorMeasure.sTemperature.Value = 0;
        }
    }
    
    if (isfinish_u8 == TRUE)
    {
        step = 0;
        
        RS485_2_Stop_RX_Mode();
        
        if (sModemInfor.ModePower_u8 == _POWER_MODE_SAVE)
        {
            V_PIN_OFF;   //off Power
            RS485_OFF;   //Off 485 de go to sleep mode
        } else {
            sEventAppWM[event].e_period = 10000;
            fevent_enable(sEventAppWM, event);
        }
    }
    
    return 1;
}



static uint8_t _Cb_Power_Up_12V(uint8_t event)
{
    UTIL_TIMER_Start (&Timer12VPowerUp);

    return 1;
}

/*
    Func: Callback event read input contact
*/

static uint8_t _Cb_Get_Contact_Input(uint8_t event)
{
//    sWmVar.aINPUT_VAL[0] = (uint8_t ) CONTACT_IN1;
//    sWmVar.aINPUT_VAL[1] = (uint8_t ) CONTACT_IN2;
         
    fevent_enable(sEventAppWM, event);
    
    return 1;
}


static uint8_t _Cb_Update_Meter_Data(uint8_t event)
{
    if (sModemInfor.ModePower_u8 == _POWER_MODE_ONLINE)
    {
        //dien ap pin va 12v: press tu update trong scan alarm
        
        //update pressure
        if ( (PressureKey_u8 != event) && (PressureKey_u8 != 0xFF) )
        {
            sEventAppWM[event].e_period = 1000;
            fevent_enable(sEventAppWM, event);
            
            return 1;
        }
           
        PressureKey_u8 = event;
        uint8_t Status = AppWm_Meas_Pressure_Process(&sEventAppWM[event].e_period);
                   
        if (Status == true)
        {       
            PressureKey_u8 = 0xFF; 
            sEventAppWM[event].e_period = 2000;
        }
    }
    
    fevent_enable(sEventAppWM, event);
    
    return 1;
}




/*================ Function Handler =================*/

uint16_t AppWm_Get_VBat(void)
{
    //Get batterry
    sBattery.mVol_u32 = ADC_Get_Value (ADC_CHANNEL_6);
    sBattery.mVol_u32 *= 2;   //Phan ap chia 2
    sBattery.Level_u16 = ADC_Convert_Vol_To_Percent (sBattery.mVol_u32, VDD_BAT, VDD_MIN); 

	return sBattery.Level_u16;
}

uint16_t AppWm_Get_VOut(void)
{
    sVout.mVol_u32 = ADC_Get_Value(ADC_CHANNEL_9);
    sVout.mVol_u32 = sVout.mVol_u32 * 247 / 47;
    sVout.Level_u16 = ADC_Convert_Vol_To_Percent (sVout.mVol_u32, VDD_OUT_MAX, VDD_OUT_MIN); 
   
	return sVout.Level_u16;
}


int32_t AppWm_Pressure_ADC_Vol(uint32_t Channel)
{
    static uint16_t aPressmVol[10] = {0x00};
    static uint8_t Index = 0;
    
    uint8_t j;
    uint16_t minData;
    uint16_t tempVol;
    
    int32_t reVal = -1;
    
    //Get mV ADC
    tempVol = ADC_Get_Value(Channel);
    aPressmVol[Index++] = (uint16_t)tempVol;
    
    //If 10 sample -> analys
    if (Index >= 10)
    {
        /* arrange */
        for(Index = 0; Index < 5; Index++)
        {
            for(j = Index+1; j <= (9 - Index); j++)
            {
                if (aPressmVol[j] < aPressmVol[Index])
                {
                    minData = aPressmVol[Index];
                    aPressmVol[Index] = aPressmVol[j];
                    aPressmVol[j] = minData;
                }
                if (aPressmVol[9-j] > aPressmVol[9-Index])
                {
                    minData = aPressmVol[9-Index];
                    aPressmVol[9-Index] = aPressmVol[9-j];
                    aPressmVol[9-j] = minData;
                }
               
            }
        }
        //Get average from index 3 -> 7
        tempVol = 0;
        for(Index = 3; Index < 7; Index++)
        {
            tempVol += aPressmVol[Index];
        }
        
        reVal = (int32_t)(tempVol / 4);
        
        //Reinit index and buff
        Index = 0;
        UTIL_MEM_set( aPressmVol, 0, sizeof(aPressmVol) );
    } 

    return reVal;
}

/*
    Func: chuyen doi mV sang bar
        + Mode 0: cho nguon dong: 4mA -> 20mA tuong ung 0 - 10 bar
        + Mode 1: Cho nguon ap: 0 -> 10V tuong ung 0 -> 10 bar
*/
uint32_t AppWm_Convert_mV_To_Press_Unit (uint32_t mVol, uint8_t mode)
{
    uint32_t Result = 0;
    uint32_t VolReal = 0, ResolutionBar = 0;
    uint16_t VolHigh = 0;
    uint16_t VolLow = 0;
    uint8_t IsOverFlow = false, IsLessFlow = false;
    
    ResolutionBar = (PRESSURE_HIGH_BAR - PRESSURE_LOW_BAR) * 1000; //scale 1000 
    
    switch (mode)
    {
        case 0:  //nguon dong: 4mA -> 20mA
            VolReal = mVol;
            
            VolLow = PRESSURE_LOW_CURR * PRESSURE_RESISTOR;
            VolHigh = PRESSURE_HIGH_CURR * PRESSURE_RESISTOR;

            //Noi suy tuyen tinh
            if ( (VolReal > VolLow) && (VolReal <= VolHigh) )
            {
                Result = ((VolReal - VolLow) * ResolutionBar) / (VolHigh - VolLow);    
            } else if (VolReal <= VolLow) 
            {
                IsLessFlow = true;
            } else if (VolReal > VolHigh) 
            {
                IsOverFlow = true;
            }
            break;
        case 1:  //nguon ap 0 -> 10V
            VolReal = mVol * PRESSURE_DIVIDE_VOL;
        
            if (VolReal <= PRESSURE_HIGH_VOL2)
            {
                Result = VolReal;
            } else
            {
                IsOverFlow = true;
            }
            break;
        case 2:
            VolReal = mVol * PRESSURE_DIVIDE_VOL;
        
            if ((VolReal >= PRESSURE_LOW_VOL3) && (VolReal <= PRESSURE_HIGH_VOL3))
            {
                Result = ((VolReal - PRESSURE_LOW_VOL3) * ResolutionBar) / (PRESSURE_HIGH_VOL3 - PRESSURE_LOW_VOL3) ;  //scale 1000;
            } else if (VolReal < PRESSURE_LOW_VOL3)
            {
                IsLessFlow = true;
            } else
            {
                IsOverFlow = true;
            }
            break;
        case 3:   //0 ->10V tuong ung 0 -> 5 bar
            VolReal = mVol * PRESSURE_DIVIDE_VOL;
        
            if (VolReal <= PRESSURE_HIGH_VOL2)
            {
                Result = VolReal / 2;
            } else
            {
                IsOverFlow = true;
            }
            break;
        default:
            break;
    }
    
    if (IsLessFlow == true)
    {
        Result = 0;
        UTIL_Printf_Str(DBLEVEL_M, "u_app_wm: not insert pressure!\r\n" );
    }
    
    if (IsOverFlow == true)
    {
        Result = 0xFF * ResolutionBar;      /* Over flowing */
        UTIL_Printf_Str(DBLEVEL_M, "u_app_wm: over flowing!\r\n" );     
    }
    
    return Result;
}



void AppWm_Get_Last_Pulse (void)
{
#ifdef LPTIM_ENCODER_MODE
	sLastPackInfor.Pulse_u32 = sPulse[0].Number_u32 / 4;
#else
	sLastPackInfor.Pulse_u32 = sPulse[0].Number_u32;
#endif

    //Last Time
    sLastPackInfor.sTime.year   = sRTC.year;
    sLastPackInfor.sTime.month  = sRTC.month;
    sLastPackInfor.sTime.date   = sRTC.date;
    
    sLastPackInfor.sTime.hour   = sRTC.hour;
    sLastPackInfor.sTime.min    = sRTC.min;
    sLastPackInfor.sTime.sec    = sRTC.sec;
}



//neu khoi dong lai: cong them 5 xung ban dau cua che do counter
void AppWm_Plus_5Pulse (Struct_Pulse *sPulse)
{    
    if (sPulse->IsOver5Pulse_u8 == false)
    {
        if (sPulse->Number_u32 != sPulse->NumberInit_u32)
        {
            sPulse->Number_u32  += 5;
            sPulse->IsOver5Pulse_u8 = true;
        }
    }
}

/*
    Func:  Check Mode tranfer data: ONLINE OR SAVE_POWER
        - VoutmV: Voltage PIN out (2).
        - If: Vout > 10,500 mV -> ONLINE
    Return: 1: If have Change mode
            0: If no change mode
*/
uint8_t AppWm_Check_Change_Mode (uint8_t *Mode, uint32_t VoutmV)
{
    if (*Mode == _POWER_MODE_ONLINE)
    {
        if (VoutmV <= 10100)
        {
            if (sWmVar.pPack_Alarm_Str != NULL)
                sWmVar.pPack_Alarm_Str ("u_app_wm: change to save mode!\r\n");

            *Mode = _POWER_MODE_SAVE;
            return TRUE;
        }
    } else
    {
        if (VoutmV > 10900)
        {
            if (sWmVar.pPack_Alarm_Str != NULL)
                sWmVar.pPack_Alarm_Str ("u_app_wm: change to run mode!\r\n");
            
            *Mode = _POWER_MODE_ONLINE;
            return TRUE;
        }
    }
    
    return FALSE;
}



void AppWm_Check_Mode_Power (void)   
{
    if (AppWm_Analys_Vout() == true)
    {
        if (*sWmVar.pHardware_u8 == __HARD_ONLINE_PSM)
        {
            //do dien ap pin ngoai -> chuyen mode
            if (AppWm_Check_Change_Mode(sWmVar.pModePower_u8, sVout.mVol_u32) == TRUE)
            {
                //Print to debug
                APP_LOG(TS_OFF, DBLEVEL_M, "u_app_wm: Vpin out: %d \r\n", sVout.mVol_u32);
                
            #ifdef USING_APP_COMM
                AppComm_Set_Next_TxTimer();
            #endif
                
            #ifdef USING_APP_ETHERNET    
                AppEth_Restart_If_PSM();
            #endif
                
            #ifdef USING_APP_SIM
                AppSim_Restart_If_PSM(); 
            #endif
            }
        }
            
        if (sWmVar.IrqPowUp_u8 == waiting) 
            sWmVar.IrqPowUp_u8 = false;
    }
}


uint8_t AppWm_Analys_Vout (void)
{
    static uint32_t TotalVout = 0;    
    static uint8_t IndexSample = 0;
    
    if (sWmVar.IrqPowUp_u8 == pending) 
    {
        sWmVar.IrqPowUp_u8 = waiting;
        IndexSample = 0;
        TotalVout = 0;
    }
    
    if (IndexSample < 10)
    {
        AppWm_Get_VOut();
        
        TotalVout += sVout.mVol_u32;
        IndexSample++;      
    } else
    {
        sVout.mVol_u32 = TotalVout / 10;
        
        IndexSample = 0;
        TotalVout = 0;
        
        return true;
    }
    
    return false; 
}

/*
    Func: Tat het nguon tieu thu nang luong cua app temh
        + Off 12v
        + Off Led 1 2 3
*/

void AppWm_Off_Energy_Consumption (void)
{
    LED_Off (_LED_STATUS);
    
    V_PIN_OFF;   //off Power
    RS485_OFF;   //Off 485 de go to sleep mode
    
    //turnoff lcd display
    sLCD.Ready_u8 = false;
    HAL_GPIO_WritePin (LCD_ON_OFF_GPIO_Port, LCD_ON_OFF_Pin, GPIO_PIN_RESET);   
}




/*
    - Function tinh toan va check alarm
        + Co the doc ra tu record truoc va lay lai duoc chi so o phia truoc + thoi gian: Sau do tính
        + Neu nhu thiet bi duoc cam moi hoan toan: 
                *Index Record  = 0
                *Page cuoi cung se bi xoa roi. Nen se check ra firstbyte = 0 va length = 0.
                * Khi do check alarm se khong bao alarm
*/

uint8_t AppWm_Scan_Alarm (void)
{
    uint8_t         i = 0;
    
    //so sanh voi cac threshold va Set flag mark alarm. 
    if (sPulse[0].Quantity_i32 > sMeterThreshold.PeakHigh) 
    {
        if (AppWm_Check_Replay_Alarm( _ALARM_PEAK_HIGH ) == TRUE)
            if (sWmVar.pPack_Alarm_Str != NULL)
                sWmVar.pPack_Alarm_Str ("u_app_wm: high quantity!\r\n");
    }
    //Peak Low
    if (sPulse[0].Quantity_i32 < sMeterThreshold.PeakLow)  
    {
        if (AppWm_Check_Replay_Alarm( _ALARM_PEAK_LOW ) == TRUE)
            if (sWmVar.pPack_Alarm_Str != NULL)
                sWmVar.pPack_Alarm_Str ("u_app_wm: low quantity!\r\n");
    } 
    //Flow high
    if (sPulse[0].Flow_i32 > sMeterThreshold.FlowHigh) 
    {
        if (AppWm_Check_Replay_Alarm( _ALARM_FLOW_HIGH ) == TRUE)
            if (sWmVar.pPack_Alarm_Str != NULL)
                sWmVar.pPack_Alarm_Str ("u_app_wm: high flow!\r\n");
    } 
    //Flow low
    if (sPulse[0].Flow_i32 < sMeterThreshold.FlowLow)  //   
    {
        if (AppWm_Check_Replay_Alarm( _ALARM_FLOW_LOW ) == TRUE)
            if (sWmVar.pPack_Alarm_Str != NULL)
                sWmVar.pPack_Alarm_Str ("u_app_wm: low flow!\r\n");
    }
    //Check Vbat threshold
    if (sBattery.Level_u16 < sMeterThreshold.LowBatery) 
    {
        if (AppWm_Check_Replay_Alarm(_ALARM_VBAT_LOW) == TRUE)
            if (sWmVar.pPack_Alarm_Str != NULL)
                sWmVar.pPack_Alarm_Str ("u_app_wm: low battery!\r\n");
    } 
    
    //Check chan Detect Cut  
    sWmVar.DTCVal_u8 = HAL_GPIO_ReadPin(PULSE1_DTC_GPIO_Port, PULSE1_DTC_Pin);
    if (sWmVar.DTCVal_u8 == 1) 
    {
        if (AppWm_Check_Replay_Alarm( _ALARM_DETECT_CUT ) == TRUE)
            if (sWmVar.pPack_Alarm_Str != NULL)
                sWmVar.pPack_Alarm_Str ("u_app_wm: DTC!\r\n");
    }
    
    //Check cac mark xem return 1 hay 0
    for (i = 0; i < _ALARM_END;i++)
        if (aMARK_ALARM[i] == 1)
            return 1;
    
    return 0;
}


/*
    Func: handler when check Alarm true
        + Neu truoc do da canh bao -> Sau MAX_ALARM_CONTINUOUS lan ms dc canh bao lai
        + Neu truoc do chua co canh bao: Mark alarm
*/
uint8_t AppWm_Check_Replay_Alarm (uint8_t Index)
{
    if (Check_Time_Out(aLandMarkAlarm_u32[Index], MAX_PERIOD_ALARM) == true)
    {
        aLandMarkAlarm_u32[Index] = RtCountSystick_u32;
        aMARK_ALARM[Index] = TRUE;
        
        return true;
    }
    
    return false;
}


uint8_t AppWm_Packet_TSVH (sData *pData)
{
#ifdef PAYLOAD_HEX_FORMAT
    uint16_t PosIndex = 0, LengthPacket = 0;
    uint8_t TempCrc = 0;
#endif
    uint16_t i = 0;
    
    //Dong goi tung ban tin cac kenh
    for (i = 0; i < MAX_CHANNEL; i++)
    {
    #ifdef PAYLOAD_HEX_FORMAT
        PosIndex = pData->Length_u16;
        pData->Length_u16 += 2;
        LengthPacket = AppWm_Packet_TSVH_Channel(pData, i);
       
        *(pData->Data_a8 + PosIndex) = i + 1;
        *(pData->Data_a8 + PosIndex + 1) = LengthPacket;
    #else
        AppWm_Packet_TSVH_Channel(pData, i);
    #endif
    }

#ifdef PAYLOAD_HEX_FORMAT
    // caculator crc
    pData->Length_u16++;
	for (i = 0; i < (pData->Length_u16 - 1); i++)
		TempCrc ^= pData->Data_a8[i];

    pData->Data_a8[pData->Length_u16-1] = TempCrc;
#endif
    
    return true;
}



double AppWm_Linear_Interpolation(double InVal,
                                   double InMin, double InMax,
                                   double OutMin, double OutMax)
{
    if ( (InMin > InMax) || (OutMin > OutMax) )
        return -1000;
    
    //3 truong hop
    //TH1: k lap 
    if (InVal < InMin) 
        return -1000;
    //
    if (InVal > InMax)
        return 0xFFFF *1000;
    
    return ( OutMin + ((InVal - InMin) * (OutMax - OutMin) / (InMax - InMin)) );	
}


uint8_t AppWm_Packet_TSVH_Channel (sData *pData, uint8_t channel)
{
#ifdef PAYLOAD_HEX_FORMAT
    uint16_t FistPos = pData->Length_u16;
    uint32_t TempPulse = 0;
    
#ifdef USING_APP_COMM   
    uint16_t i = 0;
    uint8_t TempCrc = 0;
    uint8_t TempVal = 0;     
    uint8_t strChanel[] = {"0001"};
    
    //----------sTime--------------------
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_TIME_DEVICE, &sRTC, 6, 0xAA);
    
    //Channel ID
    strChanel[3] = 0x31 + channel;
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_SERI_SENSOR, &strChanel, 4, 0xAA);
    
    //----------chi so xung--------------------
#if (TSVH_OPERA_OR_FLASH == TSVH_MULTI)
    #ifdef LPTIM_ENCODER_MODE 
        TempPulse = sPulse[channel].Number_u32 / 4 + sPulse[channel].Start_u32;
    #else    
        TempPulse = sPulse[channel].Number_u32 + sPulse[channel].Start_u32;
    #endif
    
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_WM_PULSE, &TempPulse, 4, sPulse[channel].Factor_u8);

    //----------luu luong --------------------
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_WM_DIG_FLOW, &sPulse[channel].Flow_i32, 4, 0xFE);   
#else
    #ifdef LPTIM_ENCODER_MODE 
        TempPulse = sPulse[channel].Number_u32 / 4;
    #else    
        TempPulse = sPulse[channel].Number_u32;
    #endif
        
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_WM_PULSE, &TempPulse, 4, 0x00);

    //----------luu luong --------------------
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_WM_FLOW, &TempVal, 2, 0x00);   
#endif
    
    //----------Ap suat--------------------
#ifdef PRESSURE_DECODE
    if (sWmVar.aPRESSURE[channel].sLinearInter.Type_u8 == __AN_PRESS) {
        SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_WM_PRESSURE, &sWmVar.aPRESSURE[channel].Val_i32, 2, 0xFD);    
    } else {
        SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_WM_LEVEL_VAL_SENSOR, &sWmVar.aPRESSURE[channel].Val_i32, 2, 0xFD);    
    } 
#else
    if (sWmVar.aPRESSURE[channel].sLinearInter.Type_u8 == __AN_PRESS) {
        SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_WM_PRESSURE, &sWmVar.aPRESSURE[channel].Val_i32, 2, 0);    
    } else {
        SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_WM_LEVEL_VAL_SENSOR, &sWmVar.aPRESSURE[channel].Val_i32, 2, 0);    
    }
#endif
    //----------Dien ap Pin--------------------
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_DEV_VOL1, &sBattery.mVol_u32, 2, 0xFD);
    
    //----------Cuong do song--------------------
#ifdef USING_APP_SIM
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_RSSI_1, &sSimCommInfor.RSSI_u8, 1, 0x00);
#endif
    
    //----------Dien ap Pin--------------------
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_DEV_VOL2, &sVout.mVol_u32, 2, 0xFD);    
    
    //----------Tan suat--------------------
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_FREQ_SEND, &sModemInfor.sFrequence.UnitMin_u16, 2, 0x00);
   
    //----------trang thai tiep diem --------------------
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_CONTACT_STATUS, &sWmVar.aINPUT_VAL[channel], 1, 0xAA);
       
    // caculator crc
    pData->Length_u16++;
	for (i = FistPos; i < (pData->Length_u16 - 1); i++)
		TempCrc ^= pData->Data_a8[i];

    pData->Data_a8[pData->Length_u16-1] = TempCrc;
#endif  // USING_APP_COMM  
    return  (pData->Length_u16- FistPos);
#else
    uint8_t aTIME_DATA[40] = {0};
    sData   sTimeData = {aTIME_DATA, 0};
    uint8_t aID_CHANEL[50] = {0};
    uint8_t LenIDChanel = 0;
    int32_t RealPress_u32 = 0;
    sData *pModemID = sWmVar.pGet_ID();
    uint32_t TempPulse = 0;
        
    UTIL_MEM_cpy(aID_CHANEL, pModemID->Data_a8, pModemID->Length_u16 );
    LenIDChanel = pModemID->Length_u16;
    aID_CHANEL[LenIDChanel++] = '0';
    aID_CHANEL[LenIDChanel++] = '0';
    aID_CHANEL[LenIDChanel++] = '0';
    aID_CHANEL[LenIDChanel++] = 0x31 + channel;

    Json_Add_Root(pData, NULL, 0, 0);
    Json_Add_Object_String(pData, (uint8_t *) "deviceID", strlen("deviceID"),
                                          aID_CHANEL , LenIDChanel, 1);
    
    String_Packet_Stime(&sTimeData, sRTC);
    Json_Add_Object_String(pData, (uint8_t *) "sentAt", strlen("sentAt"),
                                          sTimeData.Data_a8, sTimeData.Length_u16, 1);
    
    //Chuyen doi Pressure ra gia tri thuc te
    RealPress_u32 = AppWm_Linear_Interpolation(sWmVar.aPRESSURE[channel].Val_i32, 
                                               sWmVar.aPRESSURE[channel].sLinearInter.InMin_u16, 
                                               sWmVar.aPRESSURE[channel].sLinearInter.InMax_u16,
                                               sWmVar.aPRESSURE[channel].sLinearInter.OutMin_u16  * 1000, 
                                               sWmVar.aPRESSURE[channel].sLinearInter.OutMax_u16 * 1000 );
    
    Json_Add_Object_Number(pData, (uint8_t *) "presureVal", strlen("presureVal"), 
                                              RealPress_u32, 3, 1);

    #ifdef LPTIM_ENCODER_MODE 
        TempPulse = sPulse[channel].Number_u32 / 4;
    #else    
        TempPulse = sPulse[channel].Number_u32;
    #endif
     
    Json_Add_Object_Number(pData, (uint8_t *) "pulse", strlen("pulse"), 
                                               TempPulse, 0, 1);
        
    Json_Add_Object_Number(pData, (uint8_t *) "intbattery", strlen("intbattery"), 
                                           sBattery.mVol_u32, 3, 1);
    Json_Add_Object_Number(pData, (uint8_t *) "exbattery", strlen("exbattery"), 
                                           sVout.mVol_u32, 3, 1);       
    Json_Add_Object_Number(pData, (uint8_t *) "rssi", strlen("rssi"), 
                                           0 - sSimCommInfor.RSSI_u8, 0, 1);
    Json_Add_Object_Number(pData, (uint8_t *) "freqsend", strlen("freqsend"), 
                                           sModemInfor.sFrequence.UnitMin_u16, 0, 1);     
        
    return  (pData->Length_u16);
#endif
}


                    
                    
                    
uint8_t AppWm_Packet_Event (uint8_t *pData)
{
    uint8_t     length = 0;
    uint16_t	i = 0;
    uint8_t     TempCrc = 0;
    
    //----------------------- start send data below ---------------------
    pData[length++] = 0x01;   // sTime
    pData[length++] = 0x06;
    pData[length++] = sRTC.year;
    pData[length++] = sRTC.month;
    pData[length++] = sRTC.date;
    pData[length++] = sRTC.hour;
    pData[length++] = sRTC.min;
    pData[length++] = sRTC.sec;
    
    //Chăc chan vao day da co 1 alarm
    for (i = 0; i < _ALARM_END;i++)
    {
        if (aMARK_ALARM[i] == 1)
        {
            //Obis
            pData[length++] = i;
            //Length +Data + scale
            switch (i)
            {
                case _ALARM_FLOW_LOW:
                case _ALARM_FLOW_HIGH:
                    pData[length++] = 4;  //length
                    pData[length++] = (sPulse[0].Flow_i32 >> 24) & 0xFF;  //
                    pData[length++] = (sPulse[0].Flow_i32 >> 16) & 0xFF; //
                    pData[length++] = (sPulse[0].Flow_i32 >> 8) & 0xFF;  //
                    pData[length++] = sPulse[0].Flow_i32 & 0xFF;  // doi vi pulse/hour
                    pData[length++] = 0x00;
                    break;
                case _ALARM_PEAK_LOW:
                case _ALARM_PEAK_HIGH:
                    pData[length++] = 4;  //length
                    pData[length++] = (sPulse[0].Quantity_i32 >> 24) & 0xFF;   //
                    pData[length++] = (sPulse[0].Quantity_i32 >> 16) & 0xFF;   //
                    pData[length++] = (sPulse[0].Quantity_i32 >> 8) & 0xFF;    //
                    pData[length++] = sPulse[0].Quantity_i32 & 0xFF;           // doi vi pulse
                    pData[length++] = 0x00;
                    break;
                case _ALARM_VBAT_LOW:
                    pData[length++] = 2;  //length
                    pData[length++] = (sBattery.Level_u16 >> 8) & 0xFF;  //
                    pData[length++] = sBattery.Level_u16 & 0xFF;         // doi vi %
                    pData[length++] = 0x00;
                    break;
                case _ALARM_DIR_CHANGE:
                    pData[length++] = 1;     //length
                    pData[length++] = sPulse[0].Direct_u8; 
                    pData[length++] = 0x00;
                    break;
                case _ALARM_DETECT_CUT:
                    pData[length++] = 1;     //length
                    pData[length++] = sWmVar.DTCVal_u8; 
                    pData[length++] = 0x00;
                    break;
                default:
                    break;   
            }
        }
        
        //Reset Alarm
        aMARK_ALARM[i] = 0;
    }  
            
    // caculator crc
    length++;
	for (i = 0; i < (length - 1); i++)
		TempCrc ^= pData[i];

    pData[length-1] = TempCrc;
    
    return length;
}


int32_t AppWm_Cacul_Quantity (uint32_t PulseCur, uint32_t PulseOld)
{
    int32_t Qual = 0;
    
    if (PulseCur >=  PulseOld)
	{
		if ((PulseCur - PulseOld) < 0xFFFFFFAF)   //khong the quay nguoc dc 0x50 xung trong 10p va khong the quay tien 0xFFAF
			Qual = (PulseCur - PulseOld);  //binh thuong
		else
			Qual = - (0xFFFFFFFF - PulseCur + PulseOld + 1); //quay nguoc
	} else
	{
		if ((PulseOld - PulseCur) > 0x50)   //khong the quay nguoc dc 0x50 xung trong 10p
			Qual = (0xFFFFFFFF - PulseOld + PulseCur + 1);  //binh thuong
        else
			Qual = - (PulseOld - PulseCur);  // quay ngược
	}
    
    return Qual;
}





/*
    Func: Log Data Event
        + Packet Event Data
        + Save to Flash or ExMem
*/

void AppWm_Log_Data_Event (void)
{
//    uint8_t     aMessData[64] = {0};
//    uint8_t     Length = 0;
//    
//    if (sRTC.year <= 20)
//        return;
//    
//    Length = AppWm_Packet_Event (&aMessData[0]);
//
//#ifdef USING_APP_MEM
//    AppMem_Write_Data(sAppMem.RecMemType_u8, _MEM_DATA_EVENT_A, 0, 
//                      &aMessData[0], Length, sRecEvent.SizeRecord_u16);
//#else
//    #ifdef USING_APP_SIM
//        AppSim_Get_Data_From_Flash(DATA_TSVH_OPERA, aMessData, Length);
//    #endif
//#endif
}

/*
    Func: Log Data TSVH
        + Packet Data TSVH
        + Save to Flash or ExMem
*/

void AppWm_Log_Data_TSVH (void)
{
    uint8_t aMessData[512] = {0};
    sData sTempTSVH = {aMessData, 0};
    
    if (sRTC.year <= 20)
        return;
    
#ifdef PAYLOAD_HEX_FORMAT
    AppWm_Packet_TSVH (&sTempTSVH);
    
#ifdef USING_APP_MEM
    AppMem_Write_Data(sAppMem.RecMemType_u8, _MEM_DATA_TSVH_A, 0, 
                      sTempTSVH.Data_a8, sTempTSVH.Length_u16, sRecTSVH.SizeRecord_u16);
#else
    #ifdef USING_APP_SIM
        AppSim_Get_Data_From_Flash(DATA_TSVH_OPERA, sTempTSVH.Data_a8, sTempTSVH.Length_u16);
    #endif
#endif   //USING_APP_MEM
        
#else
    //Dong goi tung ban tin cac kenh
    for (uint8_t i = 0; i < MAX_CHANNEL; i++)
    {
        Reset_Buff(&sTempTSVH);
        AppWm_Packet_TSVH_Channel(&sTempTSVH, i);
        
    #ifdef USING_APP_MEM
        AppMem_Write_Data(sAppMem.RecMemType_u8, _MEM_DATA_TSVH_A, 0, 
                      sTempTSVH.Data_a8, sTempTSVH.Length_u16, sRecTSVH.SizeRecord_u16);
    #else
        #ifdef USING_APP_SIM
            AppSim_Get_Data_From_Flash(DATA_TSVH_OPERA, sTempTSVH.Data_a8, sTempTSVH.Length_u16);
        #endif
    #endif   //USING_APP_MEM
    }
#endif  //PAYLOAD_HEX_FORMAT
}



void AppWm_Init_Thresh_Measure (void)
{
#ifdef USING_INTERNAL_MEM
    uint8_t 	temp = 0xFF;
	uint8_t		Buff_temp[16] = {0};
      
	// Doc gia tri cau hinh threshold. Dung de check cac alarm
    temp = *(__IO uint8_t*) ADDR_THRESH_MEAS;    //2 byte
	if (temp != FLASH_BYTE_EMPTY)   //  Doc cau hinh so lan retry send cua 1 ban tin
    {
        OnchipFlashReadData(ADDR_THRESH_MEAS, &Buff_temp[0], 16);
        //
        sMeterThreshold.FlowHigh    = (Buff_temp[2] << 8) | Buff_temp[3];
        sMeterThreshold.FlowLow     = (Buff_temp[4] << 8) | Buff_temp[5];
        
        sMeterThreshold.PeakHigh    = (Buff_temp[6] << 8) | Buff_temp[7];
        sMeterThreshold.PeakLow     = (Buff_temp[8] << 8) | Buff_temp[9];
        
        sMeterThreshold.LowBatery   = Buff_temp[10];
        
        sMeterThreshold.LevelHigh   = (Buff_temp[11] << 8) | Buff_temp[12];
        sMeterThreshold.LevelLow    = (Buff_temp[13] << 8) | Buff_temp[14];
    } else
    {
        AppWm_Save_Thresh_Measure();
    }    
#endif
}


void AppWm_Save_Thresh_Measure (void)
{
#ifdef USING_INTERNAL_MEM
    uint8_t aTEMP_THRESH[24] = {0};
    
    aTEMP_THRESH[0] = BYTE_TEMP_FIRST;
    aTEMP_THRESH[1] = 13;

    aTEMP_THRESH[2] = (sMeterThreshold.FlowHigh >> 8) & 0xFF;
    aTEMP_THRESH[3] = sMeterThreshold.FlowHigh & 0xFF;
    
    aTEMP_THRESH[4] = (sMeterThreshold.FlowLow >> 8) & 0xFF;
    aTEMP_THRESH[5] = sMeterThreshold.FlowLow & 0xFF;
     
    aTEMP_THRESH[6] = (sMeterThreshold.PeakHigh >> 8) & 0xFF;  
    aTEMP_THRESH[7] = sMeterThreshold.PeakHigh & 0xFF;
    
    aTEMP_THRESH[8] = (sMeterThreshold.PeakLow >> 8) & 0xFF;
    aTEMP_THRESH[9] = sMeterThreshold.PeakLow & 0xFF;
    
    aTEMP_THRESH[10] = sMeterThreshold.LowBatery;
    
    aTEMP_THRESH[11] = (sMeterThreshold.LevelHigh >> 8) & 0xFF;
    aTEMP_THRESH[12] = sMeterThreshold.LevelHigh & 0xFF;
    
    aTEMP_THRESH[13] = (sMeterThreshold.LevelLow >> 8) & 0xFF;
    aTEMP_THRESH[14] = sMeterThreshold.LevelLow & 0xFF;

    OnchipFlashPageErase(ADDR_THRESH_MEAS);
    OnchipFlashWriteData(ADDR_THRESH_MEAS, &aTEMP_THRESH[0], 16);
#endif
}


void AppWm_Init_Level_Calib (void)
{
#ifdef USING_INTERNAL_MEM
//    uint8_t		aTEMP[64] = {0};
//    uint8_t 	temp = 0xFF;
//
//    temp = *(__IO uint8_t*) ADDR_LEVEL_CALIB;    //2 byte
//	if (temp == BYTE_TEMP_FIRST)  
//    {
//        OnchipFlashReadData(ADDR_LEVEL_CALIB, &aTEMP[0], 64);
//        sWmVar.ZeroPointCalib_i16 = (int16_t) ( (aTEMP[2] << 8) | aTEMP[3] );
//        
//        sWmVar.sCalibTable.Mode_u8 = aTEMP[4];
//        sWmVar.sCalibTable.MaxPoint_u8 = aTEMP[5];
//        
//        if (sWmVar.sCalibTable.MaxPoint_u8 > MAX_POINT_CALIB)
//            sWmVar.sCalibTable.MaxPoint_u8 = MAX_POINT_CALIB;
//                
//        for (uint8_t i = 0; i < sWmVar.sCalibTable.MaxPoint_u8; i++)
//        {
//            sWmVar.sCalibTable.aTABLE_VAL[i][0] = (int16_t) ( (aTEMP[6+i*4] << 8) | aTEMP[7+i*4] );
//            sWmVar.sCalibTable.aTABLE_VAL[i][1] = (int16_t) ( (aTEMP[8+i*4] << 8) | aTEMP[9+i*4] );
//        }
//    } else
//    {
//        AppWm_Save_Level_Calib();
//    }
#endif
}

void AppWm_Save_Level_Calib (void)
{
#ifdef USING_INTERNAL_MEM
//    uint8_t aTEMP[64] = {0};
//    
//    aTEMP[0] = BYTE_TEMP_FIRST;
//    aTEMP[1] = 2;
//    
//    aTEMP[2] = (sWmVar.ZeroPointCalib_i16 >> 8) & 0xFF;
//    aTEMP[3] = sWmVar.ZeroPointCalib_i16 & 0xFF;
//    
//    aTEMP[4] = sWmVar.sCalibTable.Mode_u8;     
//    aTEMP[5] = sWmVar.sCalibTable.MaxPoint_u8;            //11 diem
//    
//    for (uint8_t i = 0; i < MAX_POINT_CALIB; i++)
//    {
//        aTEMP[6+i*4] = (sWmVar.sCalibTable.aTABLE_VAL[i][0] >> 8) & 0xFF;
//        aTEMP[7+i*4] = (sWmVar.sCalibTable.aTABLE_VAL[i][0] & 0xFF);
//        aTEMP[8+i*4] = (sWmVar.sCalibTable.aTABLE_VAL[i][1] >> 8) & 0xFF;
//        aTEMP[9+i*4] = (sWmVar.sCalibTable.aTABLE_VAL[i][1] & 0xFF);
//    }
//    
//    OnchipFlashPageErase(ADDR_LEVEL_CALIB);
//    OnchipFlashWriteData(ADDR_LEVEL_CALIB, &aTEMP[0], 64);
#endif
}

/*
    Func: Doc xung luu trong flash
        + Doc byte danh dau (4 byte 0xAA)
        + Doc xung neu c� danh dau ( 4 byte) x 4 kenh

    Ex: 0xAA 0x08 0xAA 0xAA 0xAA 0xAA 01 02 03 04 ...
*/


void AppWm_Init_Pulse_Infor (void)
{
//	//Read Mark Soft Reset: add 0x2000FC40
//	__asm("ldr r0, = 0x2000F840");
//	__asm("ldr r2, [r0, 0]");
//
//	__asm("ldr r0, = pMarkSoftResetMCU");
//	__asm("ldr r1, [r0, 0]");
//	__asm("str r2, [r1]");
//
//	PrintDebug(&uart_debug, (uint8_t*) "== Mark Reset: ", 15, 1000);
//
//	UTIL_Print_Number(sModem.MarkResetChip_u8);
//
//	PrintDebug(&uart_debug, (uint8_t*) "\r\n", 2, 1000);
//
//	if (sModem.MarkResetChip_u8 != 0xA5A5A5A5)
//	{
//		//
//		Modem_Alarm_Emergency((uint8_t*) "Modem: Hard reset MCU\r\n", 23);
//		sPulse.Number_u32 = 0;
//        sMQTT.aMARK_MESS_PENDING[SEND_HARD_RS_MCU] = TRUE;
//	} else
//	{
//		Modem_Log_Debug_String((uint8_t*) "Modem: Get Pulse by RAM\r\n", 25);
//		//Get Pulse from RAM
//		__asm("ldr r0, = 0x2000F84A");
//		__asm("ldr r2, [r0, 0]");
//
//		__asm("ldr r0, = pPulseLPTIM");
//		__asm("ldr r1, [r0, 0]");
//		__asm("str r2, [r1]");
//
//		//Xoa danh dau add 0x2000FC40
//		sModem.MarkResetChip_u8 = 0;
//
//		__asm("ldr r0, = pMarkSoftResetMCU");
//		__asm("ldr r1, [r0, 0]");
//		__asm("ldr r2, [r1, 0]");
//
//		__asm("ldr r0, = 0x2000F840");
//		__asm("str r2, [r0]");
//	}

#if defined (BOARD_QN_V3_2) || defined (BOARD_QN_V4_0)
    //Doc ra tu flash
    uint8_t	aTEMP[128] = {0};
    uint8_t TempU8 = 0xFF;
    uint8_t Retry = 3, IsGetFromFlash = false, i = 0;
    uint16_t Pos = 0;
    
    while ((Retry--) > 0)
    {
        TempU8 = *(__IO uint8_t*) ADDR_METER_NUMBER;    
        if (TempU8 == BYTE_TEMP_FIRST)  
        {
            UTIL_MEM_set(aTEMP, 0, sizeof(aTEMP));
            OnchipFlashReadData(ADDR_METER_NUMBER + 2, &aTEMP[0], 128);
            
            //Watermeter number channel
            sWmVar.nChannel_u8 = aTEMP[4];
            //Mode Pressure  //Danh ra 8 vi tri cho cau hinh Press Input: (5 6 7 8) (9 10 11 12)
            for (i = 0; i < MAX_CHANNEL; i++)  
                sWmVar.aPRESSURE[i].Mode_u8 = aTEMP[5 + i];
            
            if ( (aTEMP[0] == BYTE_TEMP_FIRST)
                && (aTEMP[1] == BYTE_TEMP_FIRST)
                && (aTEMP[2] == BYTE_TEMP_FIRST)
                    && (aTEMP[3] == BYTE_TEMP_FIRST) )
            {
                //Xung 4 kenh
                Pos = 13;
                sPulse[0].Number_u32 = aTEMP[Pos] << 24 | aTEMP[Pos + 1] << 16 | aTEMP[Pos + 2] << 8 | aTEMP[Pos + 3];
                Pos += 4;
                sPulse[0].Start_u32 = aTEMP[Pos] << 24 | aTEMP[Pos + 1] << 16 | aTEMP[Pos + 2] << 8 | aTEMP[Pos + 3];
                Pos += 4;
                sPulse[0].Factor_u8 = aTEMP[Pos];
                Pos += 1;
                
                sPulse[1].Number_u32 = aTEMP[Pos] << 24 | aTEMP[Pos + 1] << 16 | aTEMP[Pos + 2] << 8 | aTEMP[Pos + 3];
                Pos += 4;
                sPulse[1].Start_u32 = aTEMP[Pos] << 24 | aTEMP[Pos + 1] << 16 | aTEMP[Pos + 2] << 8 | aTEMP[Pos + 3];
                Pos += 4;
                sPulse[1].Factor_u8 = aTEMP[Pos];
                Pos += 1;
                
                sPulse[2].Number_u32 = aTEMP[Pos] << 24 | aTEMP[Pos + 1] << 16 | aTEMP[Pos + 2] << 8 | aTEMP[Pos + 3];
                Pos += 4;
                sPulse[2].Start_u32 = aTEMP[Pos] << 24 | aTEMP[Pos + 1] << 16 | aTEMP[Pos + 2] << 8 | aTEMP[Pos + 3];
                Pos += 4;
                sPulse[2].Factor_u8 = aTEMP[Pos];
                Pos += 1;
                
                sPulse[3].Number_u32 = aTEMP[Pos] << 24 | aTEMP[Pos + 1] << 16 | aTEMP[Pos + 2] << 8 | aTEMP[Pos + 3];
                Pos += 4;
                sPulse[3].Start_u32 = aTEMP[Pos] << 24 | aTEMP[Pos + 1] << 16 | aTEMP[Pos + 2] << 8 | aTEMP[Pos + 3];
                Pos += 4;
                sPulse[3].Factor_u8 = aTEMP[Pos];
                Pos += 1;
                
                IsGetFromFlash = true;
                
                break;
            }
        }
    }
    
    if (IsGetFromFlash == false)
    {
        //hard reset mcu
        if (sWmVar.pPack_Alarm_Str != NULL)
            sWmVar.pPack_Alarm_Str ("u_app_wm: init pulse hrs!\r\n");   
        
    #ifdef BUTTON_DEFAULT_DEV
        AppWm_Save_Pulse();
    #endif
    } else
    {
        //Lay data tu flash
        if (sWmVar.pPack_Alarm_Str != NULL)
            sWmVar.pPack_Alarm_Str ("u_app_wm: init pulse flash!\r\n");  
        
    #ifndef BUTTON_DEFAULT_DEV
        //Xoa co danh dau 
        AppWm_UnMark_Init_Pulse();
    #endif
    }
    
	sPulse[0].NumberInit_u32 = sPulse[0].Number_u32;   
    sPulse[1].NumberInit_u32 = sPulse[1].Number_u32;   
    sPulse[2].NumberInit_u32 = sPulse[2].Number_u32;  
    sPulse[3].NumberInit_u32 = sPulse[3].Number_u32;   
    
    if ( (sPulse[0].Factor_u8 < 0xFC) && (sPulse[0].Factor_u8 > 5) )
        sPulse[0].Factor_u8 = 0;
    
    if ( (sPulse[1].Factor_u8 < 0xFC) && (sPulse[1].Factor_u8 > 5) )
        sPulse[1].Factor_u8 = 0;
    
    if ( (sPulse[2].Factor_u8 < 0xFC) && (sPulse[2].Factor_u8 > 5) )
        sPulse[2].Factor_u8 = 0;
    
    if ( (sPulse[3].Factor_u8 < 0xFC) && (sPulse[3].Factor_u8 > 5) )
        sPulse[3].Factor_u8 = 0;
#endif  
}

uint8_t AppWm_Save_Pulse (void)
{
#if defined (BOARD_QN_V3_2) || defined (BOARD_QN_V4_0)
    uint8_t aTEMP[128] = {0};
    uint8_t aTEMP_READ[128] = {0};
    uint8_t Retry = 5, IsWriteOK = true, Result = false;
    uint16_t Count = 0;
    
    aTEMP[Count++] = BYTE_TEMP_FIRST;
    aTEMP[Count++] = 20;

    aTEMP[Count++] = BYTE_TEMP_FIRST;
    aTEMP[Count++] = BYTE_TEMP_FIRST;
    aTEMP[Count++] = BYTE_TEMP_FIRST;
    aTEMP[Count++] = BYTE_TEMP_FIRST;
    
    aTEMP[Count++] = sWmVar.nChannel_u8;
    
    for (uint8_t i = 0; i < 8; i++)
    {
        if (i < MAX_CHANNEL)
            aTEMP[Count++] = sWmVar.aPRESSURE[i].Mode_u8;
        else
            aTEMP[Count++] = 0x00;
    }
    
    aTEMP[Count++] = (sPulse[0].Number_u32 >> 24) & 0xFF;
    aTEMP[Count++] = (sPulse[0].Number_u32 >> 16) & 0xFF;
    aTEMP[Count++] = (sPulse[0].Number_u32 >> 8) & 0xFF;
    aTEMP[Count++] = sPulse[0].Number_u32 & 0xFF;
    
    aTEMP[Count++] = (sPulse[0].Start_u32 >> 24) & 0xFF;
    aTEMP[Count++] = (sPulse[0].Start_u32 >> 16) & 0xFF;
    aTEMP[Count++] = (sPulse[0].Start_u32 >> 8) & 0xFF;
    aTEMP[Count++] = sPulse[0].Start_u32 & 0xFF;
    
    aTEMP[Count++] = sPulse[0].Factor_u8 & 0xFF;
    
    //
    aTEMP[Count++] = (sPulse[1].Number_u32 >> 24) & 0xFF;
    aTEMP[Count++] = (sPulse[1].Number_u32 >> 16) & 0xFF;
    aTEMP[Count++] = (sPulse[1].Number_u32 >> 8) & 0xFF;
    aTEMP[Count++] = sPulse[1].Number_u32 & 0xFF;
    
    aTEMP[Count++] = (sPulse[1].Start_u32 >> 24) & 0xFF;
    aTEMP[Count++] = (sPulse[1].Start_u32 >> 16) & 0xFF;
    aTEMP[Count++] = (sPulse[1].Start_u32 >> 8) & 0xFF;
    aTEMP[Count++] = sPulse[1].Start_u32 & 0xFF;

    aTEMP[Count++] = sPulse[1].Factor_u8 & 0xFF;
    
    //
    aTEMP[Count++] = (sPulse[2].Number_u32 >> 24) & 0xFF;
    aTEMP[Count++] = (sPulse[2].Number_u32 >> 16) & 0xFF;
    aTEMP[Count++] = (sPulse[2].Number_u32 >> 8) & 0xFF;
    aTEMP[Count++] = sPulse[2].Number_u32 & 0xFF;
    
    aTEMP[Count++] = (sPulse[2].Start_u32 >> 24) & 0xFF;
    aTEMP[Count++] = (sPulse[2].Start_u32 >> 16) & 0xFF;
    aTEMP[Count++] = (sPulse[2].Start_u32 >> 8) & 0xFF;
    aTEMP[Count++] = sPulse[2].Start_u32 & 0xFF;

    aTEMP[Count++] = sPulse[2].Factor_u8 & 0xFF;
    
    //
    aTEMP[Count++] = (sPulse[3].Number_u32 >> 24) & 0xFF;
    aTEMP[Count++] = (sPulse[3].Number_u32 >> 16) & 0xFF;
    aTEMP[Count++] = (sPulse[3].Number_u32 >> 8) & 0xFF;
    aTEMP[Count++] = sPulse[3].Number_u32 & 0xFF;
    
    aTEMP[Count++] = (sPulse[3].Start_u32 >> 24) & 0xFF;
    aTEMP[Count++] = (sPulse[3].Start_u32 >> 16) & 0xFF;
    aTEMP[Count++] = (sPulse[3].Start_u32 >> 8) & 0xFF;
    aTEMP[Count++] = sPulse[3].Start_u32 & 0xFF;
    
    aTEMP[Count++] = sPulse[3].Factor_u8 & 0xFF;
    
    while (Retry-- > 0)
    {
        Erase_Firmware(ADDR_METER_NUMBER, 1);
        OnchipFlashWriteData(ADDR_METER_NUMBER, &aTEMP[0], 128);
    
        UTIL_MEM_set(aTEMP_READ, 0, sizeof(aTEMP_READ));
        OnchipFlashReadData(ADDR_METER_NUMBER, &aTEMP_READ[0], 128);
        
        for (uint8_t i = 0; i < 128; i++)
        {
            if (aTEMP_READ[i] != aTEMP[i])
            {
                IsWriteOK = false;
                break;
            }
        }
                        
        if (IsWriteOK == true)
        {
            Result = true;
            break;
        }
    }
        
    if (Result == false)
    {
        if (sWmVar.pPack_Alarm_Str != NULL)
            sWmVar.pPack_Alarm_Str ("u_app_wm: save pulse error!\r\n");  
    }
                            
    return Result;
#else
    return true;
#endif  
}



uint8_t AppWm_UnMark_Init_Pulse (void)
{
#if defined (BOARD_QN_V3_2) || defined (BOARD_QN_V4_0)
    uint8_t aTEMP[128] = {0};
    uint8_t aTEMP_READ[128] = {0};
    uint8_t Retry = 5, IsWriteOK = true, Result = false;
    uint16_t Count = 0;
    
    aTEMP[Count++] = BYTE_TEMP_FIRST;
    aTEMP[Count++] = 20;

#ifdef BUTTON_DEFAULT_DEV
    aTEMP[Count++] = BYTE_TEMP_FIRST;
    aTEMP[Count++] = BYTE_TEMP_FIRST;
    aTEMP[Count++] = BYTE_TEMP_FIRST;
    aTEMP[Count++] = BYTE_TEMP_FIRST;
#else    
    aTEMP[Count++] = 0x00;
    aTEMP[Count++] = 0x00;
    aTEMP[Count++] = 0x00;
    aTEMP[Count++] = 0x00;
#endif
    
    aTEMP[6] = sWmVar.nChannel_u8;
    
    for (uint8_t i = 0; i < 8; i++)
    {
        if (i < MAX_CHANNEL)
            aTEMP[Count++] = sWmVar.aPRESSURE[i].Mode_u8;
        else
            aTEMP[Count++] = 0x00;
    }
    
    aTEMP[Count++] = (sPulse[0].Number_u32 >> 24) & 0xFF;
    aTEMP[Count++] = (sPulse[0].Number_u32 >> 16) & 0xFF;
    aTEMP[Count++] = (sPulse[0].Number_u32 >> 8) & 0xFF;
    aTEMP[Count++] = sPulse[0].Number_u32 & 0xFF;
    
    aTEMP[Count++] = (sPulse[0].Start_u32 >> 24) & 0xFF;
    aTEMP[Count++] = (sPulse[0].Start_u32 >> 16) & 0xFF;
    aTEMP[Count++] = (sPulse[0].Start_u32 >> 8) & 0xFF;
    aTEMP[Count++] = sPulse[0].Start_u32 & 0xFF;
    
    aTEMP[Count++] = sPulse[0].Factor_u8 & 0xFF;
    
    //
    aTEMP[Count++] = (sPulse[1].Number_u32 >> 24) & 0xFF;
    aTEMP[Count++] = (sPulse[1].Number_u32 >> 16) & 0xFF;
    aTEMP[Count++] = (sPulse[1].Number_u32 >> 8) & 0xFF;
    aTEMP[Count++] = sPulse[1].Number_u32 & 0xFF;
    
    aTEMP[Count++] = (sPulse[1].Start_u32 >> 24) & 0xFF;
    aTEMP[Count++] = (sPulse[1].Start_u32 >> 16) & 0xFF;
    aTEMP[Count++] = (sPulse[1].Start_u32 >> 8) & 0xFF;
    aTEMP[Count++] = sPulse[1].Start_u32 & 0xFF;

    aTEMP[Count++] = sPulse[1].Factor_u8 & 0xFF;
    
    //
    aTEMP[Count++] = (sPulse[2].Number_u32 >> 24) & 0xFF;
    aTEMP[Count++] = (sPulse[2].Number_u32 >> 16) & 0xFF;
    aTEMP[Count++] = (sPulse[2].Number_u32 >> 8) & 0xFF;
    aTEMP[Count++] = sPulse[2].Number_u32 & 0xFF;
    
    aTEMP[Count++] = (sPulse[2].Start_u32 >> 24) & 0xFF;
    aTEMP[Count++] = (sPulse[2].Start_u32 >> 16) & 0xFF;
    aTEMP[Count++] = (sPulse[2].Start_u32 >> 8) & 0xFF;
    aTEMP[Count++] = sPulse[2].Start_u32 & 0xFF;

    aTEMP[Count++] = sPulse[2].Factor_u8 & 0xFF;
    
    //
    aTEMP[Count++] = (sPulse[3].Number_u32 >> 24) & 0xFF;
    aTEMP[Count++] = (sPulse[3].Number_u32 >> 16) & 0xFF;
    aTEMP[Count++] = (sPulse[3].Number_u32 >> 8) & 0xFF;
    aTEMP[Count++] = sPulse[3].Number_u32 & 0xFF;
    
    aTEMP[Count++] = (sPulse[3].Start_u32 >> 24) & 0xFF;
    aTEMP[Count++] = (sPulse[3].Start_u32 >> 16) & 0xFF;
    aTEMP[Count++] = (sPulse[3].Start_u32 >> 8) & 0xFF;
    aTEMP[Count++] = sPulse[3].Start_u32 & 0xFF;
    
    aTEMP[Count++] = sPulse[3].Factor_u8 & 0xFF;
    
    while (Retry-- > 0)
    {
        Erase_Firmware(ADDR_METER_NUMBER, 1);
        OnchipFlashWriteData(ADDR_METER_NUMBER, &aTEMP[0], 128);
    
        UTIL_MEM_set(aTEMP_READ, 0, sizeof(aTEMP_READ));
        OnchipFlashReadData(ADDR_METER_NUMBER, &aTEMP_READ[0], 128);
        
        for (uint8_t i = 0; i < 128; i++)
        {
            if (aTEMP_READ[i] != aTEMP[i])
            {
                IsWriteOK = false;
                break;
            }
        }
                        
        if (IsWriteOK == true)
        {
            Result = true;
            break;
        }
    }
        
    if (Result == false)
    {
        UTIL_Printf_Str(DBLEVEL_M, "u_app_wm: clear mark pulse error!\r\n");
    }
                            
    return Result;
#else
    return true;
#endif  
}


static void OnTimerLevelPowerOn(void *context)
{
//	fevent_active(sEventAppWM, _EVENT_RS485_MODBUS);
}

static void OnTimerRs485_2(void *context)
{
	fevent_active(sEventAppWM, _EVENT_RS485_2_MODBUS);
}



static void OnTimer12VPowerUp(void *context)
{
    sWmVar.IrqPowUp_u8 = pending;
    fevent_active(sEventAppWM, _EVENT_CHECK_MODE);
}


void LED_Toggle (Led_TypeDef Led)
{
    HAL_GPIO_TogglePin(LED_PORT[Led], LED_PIN[Led]);
}


void LED_On (Led_TypeDef Led)
{
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
}



void LED_Off (Led_TypeDef Led)
{
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);
}


/*
    Dat fun nay sau Init Pulse from flash
*/
void AppWm_Button_Default_Device (void)
{
    uint32_t DetectButton_u32 = 0;
     //Doc chan PA4 de xem khoi tao xung ve 0?
    if (BUTTON_DEFAULT == true)
    {
        while (DetectButton_u32++ < MAX_BUTTON_DEF_DEV )       //1M
        {
            if (BUTTON_DEFAULT == false)
                break;
        }
    }

    //Neu DetectButton_u32 >  : Nut nhan duoc giu trong khoang do.
    if (DetectButton_u32 >= MAX_BUTTON_DEF_DEV)
    {
        AppWm_Default_Pulse();
    }
}


void AppWm_Default_Pulse (void)
{
    if (sWmVar.pPack_Alarm_Str != NULL)
            sWmVar.pPack_Alarm_Str ("u_app_wm: default device!\r\n");  
        
    sWmVar.rDefault_u8 = true;
        
    sPulse[0].Number_u32 = 0;
    sPulse[1].Number_u32 = 0;
    sPulse[2].Number_u32 = 0;
    sPulse[3].Number_u32 = 0;
        
    sPulse[0].Start_u32 = 0;   
    sPulse[1].Start_u32 = 0;   
    sPulse[2].Start_u32 = 0;  
    sPulse[3].Start_u32 = 0; 
    
    sPulse[0].NumberInit_u32 = sPulse[0].Number_u32;   
    sPulse[1].NumberInit_u32 = sPulse[1].Number_u32;   
    sPulse[2].NumberInit_u32 = sPulse[2].Number_u32;  
    sPulse[3].NumberInit_u32 = sPulse[3].Number_u32; 
    
    AppWm_Save_Pulse();

#ifdef USING_APP_MEM
    sRecTSVH.IndexSend_u16 = 0;
    sRecTSVH.IndexSave_u16 = 0;
    sRecEvent.IndexSend_u16 = 0;
    sRecEvent.IndexSave_u16 = 0;
    sRecLog.IndexSend_u16 = 0;
    sRecLog.IndexSave_u16 = 0;
    sRecGPS.IndexSend_u16 = 0;
    sRecGPS.IndexSave_u16 = 0;
    
    AppMem_Save_Index_Rec();
#endif
}

/*Func Callback serial*/
#ifdef USING_AT_CONFIG

void AppWm_SER_Set_Default_Pulse(sData *strRecei, uint16_t Pos)
{
    sPulse[0].Number_u32 = 0;
    sPulse[1].Number_u32 = 0;
    sPulse[2].Number_u32 = 0;
    sPulse[3].Number_u32 = 0;
    
    sWmVar.pRespond_Str(PortConfig, "\r\nreset pulse ok!", 0);
}

void AppWm_SER_Set_Thresh_Flow(sData *str_Receiv, uint16_t Pos)
{
    uint16_t    Posfix = Pos;
    uint16_t    TemLowThresh = 0;
    uint16_t    TemHighThresh = 0;
    
    TemLowThresh = (uint16_t) UTIL_Get_Num_From_Str (str_Receiv, &Posfix);
    TemHighThresh = (uint16_t) UTIL_Get_Num_From_Str (str_Receiv, &Posfix);
    
    if ( (TemLowThresh != 0xFFFF) && (TemHighThresh != 0xFFFF) )
    {
        sMeterThreshold.FlowLow  = TemLowThresh;
        sMeterThreshold.FlowHigh = TemHighThresh;
        
        AppWm_Save_Thresh_Measure();
        
        sWmVar.pRespond_Str(PortConfig, "\r\nOK\r\n", 0);
    }
    
    sWmVar.pRespond_Str(PortConfig, "\r\nOVER VALUE U16\r\n", 0);
}

void AppWm_SER_Get_Thresh_Flow(sData *str_Receiv, uint16_t Pos)
{
    char aData[32] = {0};
    
    sprintf(aData, "\r\nflowthresh: %d,%d\r\n", sMeterThreshold.FlowLow, sMeterThreshold.FlowHigh);
    
    sWmVar.pRespond_Str(PortConfig, aData, 0);
}


void AppWm_SER_Set_Thresh_Quan(sData *str_Receiv, uint16_t Pos)
{
    uint16_t    Posfix = Pos;
    uint16_t    TemLowThresh = 0;
    uint16_t    TemHighThresh = 0;
    
    TemLowThresh = (uint16_t) UTIL_Get_Num_From_Str (str_Receiv, &Posfix);
    TemHighThresh = (uint16_t) UTIL_Get_Num_From_Str (str_Receiv, &Posfix);
    
    if ( (TemLowThresh != 0xFFFF) && (TemHighThresh != 0xFFFF) )
    {
        sMeterThreshold.PeakLow  = TemLowThresh;
        sMeterThreshold.PeakHigh = TemHighThresh;
        
        AppWm_Save_Thresh_Measure();
        
        sWmVar.pRespond_Str(PortConfig, "\r\nOK\r\n", 0);
    }
    
    sWmVar.pRespond_Str(PortConfig, "\r\nOVER VALUE U16\r\n", 0);
}

void AppWm_SER_Get_Thresh_Quan(sData *str_Receiv, uint16_t Pos)
{
    char aData[32] = {0};
    
    sprintf(aData, "\r\nQuanthresh: %d,%d\r\n", sMeterThreshold.PeakLow, sMeterThreshold.PeakHigh);
    
    sWmVar.pRespond_Str(PortConfig, aData, 0);
}


void AppWm_SER_Set_Thresh_Bat(sData *str_Receiv, uint16_t Pos)
{
	uint16_t    Posfix = Pos;
    uint16_t    ValTemp = 0;
	  
    ValTemp = (uint16_t) UTIL_Get_Num_From_Str(str_Receiv, &Posfix);
    
    if (ValTemp != 0xFFFF) 
    {
        sMeterThreshold.LowBatery  = ValTemp;
        AppWm_Save_Thresh_Measure();
        
        sWmVar.pRespond_Str(PortConfig, "\r\nOK\r\n", 0);
    }
    
    sWmVar.pRespond_Str(PortConfig, "\r\nOVER VALUE U16\r\n", 0);
}

void AppWm_SER_Get_Thresh_Bat(sData *str_Receiv, uint16_t Pos)
{
    char aData[32] = {0};
    
    sprintf(aData, "\r\nBatthresh: %d\r\n", sMeterThreshold.LowBatery);
    
    sWmVar.pRespond_Str(PortConfig, aData, 0);
}
    

void AppWm_SER_Set_Pulse(sData *str_Receiv, uint16_t Pos)
{
    uint16_t Posfix = Pos;
    uint32_t aVAL_TEMP[10] = {0};
    uint8_t Index = 0;
	//
	while (Posfix < str_Receiv->Length_u16)
	{
        aVAL_TEMP[Index++] = UTIL_Get_Num_From_Str(str_Receiv, &Posfix);
        
        if (Index >= 10)
            break;
	}
    
#ifdef LPTIM_ENCODER_MODE 
    sPulse[0].Number_u32   = aVAL_TEMP[0] * 4;
#else   
    sPulse[0].Number_u32   = aVAL_TEMP[0];
#endif
    
#ifdef LPTIM2_ENCODER_MODE 
    sPulse[1].Number_u32  = aVAL_TEMP[1] * 4;
#else   
    sPulse[1].Number_u32  = aVAL_TEMP[1];
#endif
        
    sPulse[2].Number_u32      = aVAL_TEMP[2];
    sPulse[3].Number_u32      = aVAL_TEMP[3];
        
    sPulse[0].IsOver5Pulse_u8 = true;
    sPulse[1].IsOver5Pulse_u8 = true; 
    
    sWmVar.pRespond_Str(PortConfig, "\r\nOK\r\n", 0);
}


void AppWm_SER_Get_Pulse(sData *str_Receiv, uint16_t Pos)
{
    char aData[64] = {0};
    uint32_t aVAL_TEMP[4] = {0};
    
#ifdef LPTIM_ENCODER_MODE 
    aVAL_TEMP[0] = sPulse[0].Number_u32 / 4;
#else   
    aVAL_TEMP[0] = sPulse[0].Number_u32;
#endif
    
#ifdef LPTIM2_ENCODER_MODE 
    aVAL_TEMP[1] = sPulse[1].Number_u32 / 4;
#else   
    aVAL_TEMP[1] = sPulse[1].Number_u32;
#endif

    aVAL_TEMP[2] = sPulse[2].Number_u32;
    aVAL_TEMP[3] = sPulse[3].Number_u32;
    
    sprintf(aData, "\r\npulse: %d, %d, %d, %d\r\n", aVAL_TEMP[0], aVAL_TEMP[1],
                                                aVAL_TEMP[2], aVAL_TEMP[3]);
    
    sWmVar.pRespond_Str(PortConfig, aData, 0);
}
    

void AppWm_SER_Set_Level_Calib(sData *str_Receiv, uint16_t Pos)
{
//    uint8_t Sign = 0;
//    int16_t Value = 0;
//    uint16_t TempVal = 0;
//        
//    if (*(strRecei->Data_a8 + Pos++) == '-')
//        Sign = 1;
//    
//    TempVal = (uint16_t) UTIL_Get_Num_From_Str(strRecei, &Pos);
//        
//    if (TempVal != 0xFFFF)
//    {
//        if (Sign == 1)
//            Value = 0 - TempVal;
//        
//        sWmVar.pRespond_Str(PortConfig, "\r\nOK\r\n", 0);
//    } else
//        sWmVar.pRespond_Str(PortConfig, "\r\nERROR", 0);    
}

void AppWm_SER_Get_Level_Calib(sData *str_Receiv, uint16_t Pos)
{

}

void AppWm_SER_Set_Level_Thresh(sData *str_Receiv, uint16_t Pos)
{
    uint16_t    Posfix = Pos;
    uint16_t    TemLowThresh = 0;
    uint16_t    TemHighThresh = 0;
    //
    TemLowThresh = (uint16_t) UTIL_Get_Num_From_Str(str_Receiv, &Posfix);
    TemHighThresh = (uint16_t) UTIL_Get_Num_From_Str(str_Receiv, &Posfix);
    
    if ( (TemLowThresh != 0xFFFF) && (TemHighThresh != 0xFFFF) )
    {
        sMeterThreshold.LevelLow  = TemLowThresh;
        sMeterThreshold.LevelHigh = TemHighThresh;
        
        AppWm_Save_Thresh_Measure();
        
        sWmVar.pRespond_Str(PortConfig, "\r\nOK\r\n", 0);
    }
    
    sWmVar.pRespond_Str(PortConfig, "\r\nERROR", 0);   
}

void AppWm_SER_Get_Level_Thresh(sData *str_Receiv, uint16_t Pos)
{
    char aData[32] = {0};
    
    sprintf(aData, "\r\nLevelthresh: %d,%d\r\n", sMeterThreshold.LevelLow, sMeterThreshold.LevelHigh);
    
    sWmVar.pRespond_Str(PortConfig, aData, 0);
}


void AppWm_SER_Set_Num_Channel(sData *str_Receiv, uint16_t Pos)
{
    uint16_t Temp = (uint16_t) UTIL_Get_Num_From_Str(str_Receiv, &Pos);
    
    if (Temp != 0xFFFF)
    {
        sWmVar.nChannel_u8 = Temp;
        AppWm_UnMark_Init_Pulse();
        
        sWmVar.pRespond_Str(PortConfig, "\r\nOK\r\n", 0);
    }
    
    sWmVar.pRespond_Str(PortConfig, "\r\nERROR", 0);  
}


void AppWm_SER_Get_Num_Channel(sData *str_Receiv, uint16_t Pos)
{
    char aData[16] = {0};
    
    sprintf(aData, "\r\n%d\r\n", sWmVar.nChannel_u8);
    
    sWmVar.pRespond_Str(PortConfig, aData, 0);
}



void AppWm_SER_Set_Modbus_Infor(sData *pData, uint16_t Pos)
{
    uint8_t Index = 0;
    uint8_t Mtype = 0xFF;
    uint8_t SlaveId = 0xFF;
    uint8_t Total = (uint8_t) UTIL_Get_Num_From_Str(pData, &Pos);
    
    if (Total <= MAX_SLAVE_MODBUS) 
    {
        sWmDigVar.nModbus_u8 = Total;
        
        while (Index < Total)
        {
            Mtype = (uint8_t) UTIL_Get_Num_From_Str(pData, &Pos);
            SlaveId = (uint8_t) UTIL_Get_Num_From_Str(pData, &Pos);
            
            if ((Mtype != 0xFF) && (SlaveId != 0xFF))
            {
                sWmDigVar.sModbInfor[Index].MType_u8 = Mtype;
                sWmDigVar.sModbInfor[Index].SlaveId_u8 = SlaveId;
            } else
            {
                break;
            }
            
            Index++;
        }
        
        //Luu lai
        AppWm_Save_WM_Dig_Infor();
        
        sWmVar.pRespond_Str(PortConfig, "\r\nOK\r\n", 0);
    } else
    {
        sWmVar.pRespond_Str(PortConfig, "\r\nERROR", 0); 
    }
}


void AppWm_SER_Get_Modbus_Infor(sData *str_Receiv, uint16_t Pos)
{
    char aData[128] = {0};
    
    sprintf(aData, "%d", sWmDigVar.nModbus_u8 );
    
    for (uint8_t i = 0; i < sWmDigVar.nModbus_u8; i++)
    {
        sprintf(aData+strlen(aData), ",%d,%d", sWmDigVar.sModbInfor[i].MType_u8, sWmDigVar.sModbInfor[i].SlaveId_u8);
    }
    
    sWmVar.pRespond_Str(PortConfig, aData, 0);
}


void AppWm_SER_Set_Pressure_Infor(sData *str_Receiv, uint16_t Pos)
{
    uint16_t Posfix = Pos;
    uint8_t Channel = 0xFF;
    uint16_t MinIn = 0, MaxIn = 0, MinOut = 0, MaxOut = 0, factor = 0, InUnit = 0, OutUnit = 0, type = 0;
	  
    Channel = (uint16_t) UTIL_Get_Num_From_Str(str_Receiv, &Posfix);
    MinIn = (uint16_t) UTIL_Get_Num_From_Str(str_Receiv, &Posfix);
    MaxIn = (uint16_t) UTIL_Get_Num_From_Str(str_Receiv, &Posfix);
    InUnit = (uint16_t) UTIL_Get_Num_From_Str(str_Receiv, &Posfix);
    MinOut = (uint16_t) UTIL_Get_Num_From_Str(str_Receiv, &Posfix);
    MaxOut = (uint16_t) UTIL_Get_Num_From_Str(str_Receiv, &Posfix);
    OutUnit = (uint16_t) UTIL_Get_Num_From_Str(str_Receiv, &Posfix);
    factor = (uint16_t) UTIL_Get_Num_From_Str(str_Receiv, &Posfix); 
    type = (uint16_t) UTIL_Get_Num_From_Str(str_Receiv, &Posfix); 
    
    if (AppWm_Set_Pressure_Infor(Channel, MinIn, MaxIn, MinOut, InUnit, MaxOut, factor, OutUnit, type) == true)
    {
        sWmVar.pRespond_Str(PortConfig, "OK", 0);
    } else
    {
        sWmVar.pRespond_Str(PortConfig, "ERROR", 0);  
    }
}

uint8_t AppWm_Set_Pressure_Infor (uint8_t channel, uint16_t MinIn, uint16_t MaxIn, uint8_t InUnit,
                                  uint16_t MinOut, uint16_t MaxOut, uint16_t Factor, uint8_t OutUnit, uint8_t type)
{
    if ( (channel <= MAX_CHANNEL )
        && (MinIn != 0xFFFF) && (MaxIn != 0xFFFF) && (InUnit < 6)
        && (MinOut != 0xFFFF) && (MaxOut != 0xFFFF) 
        && (OutUnit < 6) )
    {
        if (channel == 0)
        {
            //set tat ca nhu nhau
            for (uint8_t i = 0; i < MAX_CHANNEL; i++)
            {
                sWmVar.aPRESSURE[i].sLinearInter.InMin_u16 = MinIn;
                sWmVar.aPRESSURE[i].sLinearInter.InMax_u16 = MaxIn;
                sWmVar.aPRESSURE[i].sLinearInter.OutMin_u16 = MinOut;
                sWmVar.aPRESSURE[i].sLinearInter.OutMax_u16 = MaxOut;
                sWmVar.aPRESSURE[i].sLinearInter.Factor_u16 = Factor;
                sWmVar.aPRESSURE[i].sLinearInter.InUnit_u8 = InUnit;
                sWmVar.aPRESSURE[i].sLinearInter.OutUnit_u8 = OutUnit;
                sWmVar.aPRESSURE[i].sLinearInter.Type_u8 = type;
            }
        } else
        {
            sWmVar.aPRESSURE[channel - 1].sLinearInter.InMin_u16 = MinIn;
            sWmVar.aPRESSURE[channel - 1].sLinearInter.InMax_u16 = MaxIn;
            sWmVar.aPRESSURE[channel - 1].sLinearInter.OutMin_u16 = MinOut;
            sWmVar.aPRESSURE[channel - 1].sLinearInter.OutMax_u16 = MaxOut;
            sWmVar.aPRESSURE[channel - 1].sLinearInter.Factor_u16 = Factor;
            sWmVar.aPRESSURE[channel - 1].sLinearInter.InUnit_u8 = InUnit;
            sWmVar.aPRESSURE[channel - 1].sLinearInter.OutUnit_u8 = OutUnit;
            sWmVar.aPRESSURE[channel - 1].sLinearInter.Type_u8 = type;
        }
        
        AppWm_Save_Press_Infor();
        
        return true;
    }
    
    return false;
}


void AppWm_SER_Get_Pressure_Infor(sData *str_Receiv, uint16_t Pos)
{
    char aData[1024] = {0};
    char sAnalogName[2][10] = {"pressure", "level"};
    for (uint8_t i = 0; i < MAX_CHANNEL; i++)
    {
        
        sprintf(aData + strlen(aData), "%d: %d - %d <-> %d - %d; hs: %d; Unit: %d; Type: %s\r\n", i + 1,
                                       sWmVar.aPRESSURE[i].sLinearInter.InMin_u16, sWmVar.aPRESSURE[i].sLinearInter.InMax_u16,
                                       sWmVar.aPRESSURE[i].sLinearInter.OutMin_u16, sWmVar.aPRESSURE[i].sLinearInter.OutMax_u16,
                                       sWmVar.aPRESSURE[i].sLinearInter.Factor_u16, sWmVar.aPRESSURE[i].sLinearInter.OutUnit_u8,
                                       sAnalogName[sWmVar.aPRESSURE[i].sLinearInter.Type_u8]);
    }
    
    sWmVar.pRespond_Str(PortConfig, aData, 0); 
}


#endif

    
void AppWm_Packet_Modbus (void)
{
    uint8_t aTEMP_PAYLOAD[512] = {0};
    sData pData = {aTEMP_PAYLOAD, 0};
    uint16_t i = 0, posindex = 0, length = 0;
    uint8_t TempCrc = 0;// caculator crc
    uint8_t count = 0;
    
    if (sRTC.year <= 20)
        return;

    for (i = 0; i < sWmDigVar.nModbus_u8; i++)
    {
        if (sWmDigVar.sModbDevData[i].Status_u8 == true)
        {
            posindex = pData.Length_u16;
            pData.Length_u16 += 2;
            
            length = AppWm_Packet_Mod_Channel(&pData, i);
            
            *(pData.Data_a8 + posindex) = count + 1;
            *(pData.Data_a8 + posindex + 1) = length;
        
            count++;
        }
        
        //gop 4 ban tin voi nhau. hoac cuoi cung
        if ( (count > 0) &&
             ( (count >= 4) ||  ( (i + 1) >= sWmDigVar.nModbus_u8 ) ) )
        {
            count = 0;
      
            pData.Length_u16++;
            for (i = 0; i < (pData.Length_u16 - 1); i++)
                TempCrc ^= pData.Data_a8[i];

            pData.Data_a8[pData.Length_u16-1] = TempCrc;
            
        #ifdef USING_APP_MEM
            AppMem_Write_Data(sAppMem.RecMemType_u8, _MEM_DATA_TSVH_A, 0, 
                              pData.Data_a8, pData.Length_u16, sRecTSVH.SizeRecord_u16);
        #else
            #ifdef USING_APP_SIM
                AppSim_Get_Data_From_Flash(DATA_TSVH_OPERA, pData.Data_a8, pData.Length_u16);
            #endif
        #endif
        }
    }
}



uint8_t AppWm_Packet_Mod_Channel (sData *pData, uint8_t chann)
{
#ifdef PAYLOAD_HEX_FORMAT
    uint16_t lastlength = pData->Length_u16, i = 0;
    uint8_t TempCrc = 0;
    uint8_t strChanel[4] = {"0005"};
   
    strChanel[3] = chann + MAX_CHANNEL + 0x31;
    
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_TIME_DEVICE, &sRTC, 6, 0xAA);
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_SERI_SENSOR, &strChanel, 4, 0xAA);
       
    //----------Dien ap Pin--------------------
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_DEV_VOL1, &sBattery.mVol_u32, 2, 0xFD);
    //----------Dien ap Pin--------------------
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_DEV_VOL2, &sVout.mVol_u32, 2, 0xFD);   
        //----------Cuong do song--------------------
#ifdef USING_APP_SIM
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_RSSI_1, &sSimCommInfor.RSSI_u8, 1, 0x00);
#endif
    //----------Tan suat--------------------
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_FREQ_SEND, &sModemInfor.sFrequence.UnitMin_u16, 2, 0x00);
   
    /*
        Them data phan wm digital
    */
    WM_DIG_Packet_Mess(pData, chann);

    // caculator crc
    pData->Length_u16++;
	for (i = 0; i < (pData->Length_u16 - 1); i++)
		TempCrc ^= pData->Data_a8[i];

    pData->Data_a8[pData->Length_u16-1] = TempCrc;
    
    return (pData->Length_u16 - lastlength);
#else
    uint8_t aTIME_DATA[40] = {0};
    sData   sTimeData = {aTIME_DATA, 0};
    uint8_t aID_CHANEL[50] = {0};
    uint8_t LenIDChanel = 0;
    sData *pModemID = sWmVar.pGet_ID();
        
    UTIL_MEM_cpy(aID_CHANEL, pModemID->Data_a8, pModemID->Length_u16 );
    LenIDChanel = pModemID->Length_u16;
    aID_CHANEL[LenIDChanel++] = '0';
    aID_CHANEL[LenIDChanel++] = '0';
    aID_CHANEL[LenIDChanel++] = '0';
    aID_CHANEL[LenIDChanel++] = 0x31 + MAX_CHANNEL;

    Json_Add_Root(pData, NULL, 0, 0);
    Json_Add_Object_String(pData, (uint8_t *) "deviceID", strlen("deviceID"),
                                          aID_CHANEL , LenIDChanel, 1);
    
    String_Packet_Stime(&sTimeData, sRTC);
    Json_Add_Object_String(pData, (uint8_t *) "sentAt", strlen("sentAt"),
                                          sTimeData.Data_a8, sTimeData.Length_u16, 1);

    Json_Add_Object_Number(pData, (uint8_t *) "intbattery", strlen("intbattery"), 
                                           sBattery.mVol_u32, 3, 1);
    Json_Add_Object_Number(pData, (uint8_t *) "exbattery", strlen("exbattery"), 
                                           sVout.mVol_u32, 3, 1);       
    Json_Add_Object_Number(pData, (uint8_t *) "rssi", strlen("rssi"), 
                                           sSimCommInfor.RSSI_u8, 0, 1);
    Json_Add_Object_Number(pData, (uint8_t *) "freqsend", strlen("freqsend"), 
                                           sModemInfor.sFrequence.UnitMin_u16, 0, 1);   
    
    /*
        Them data phan wm digital
    */
        
    WM_DIG_Packet_Mess(pData, chann);
    
    return true;
#endif
}




void AppWm_Init_WM_Dig_Infor (void)
{
#ifdef USING_INTERNAL_MEM
    uint8_t	aTEMP[64] = {0};
    uint8_t temp = FLASH_BYTE_EMPTY;
    uint8_t Index = 0;

    temp = *(__IO uint8_t*) ADDR_MODBUS_INFOR;    //2 byte
	if (temp == BYTE_TEMP_FIRST)  
    {
        OnchipFlashReadData(ADDR_MODBUS_INFOR, &aTEMP[0], 64);
        
        sWmDigVar.nModbus_u8 = aTEMP[2];
        
        if (sWmDigVar.nModbus_u8 > MAX_SLAVE_MODBUS)
        {
            sWmDigVar.nModbus_u8 = MAX_SLAVE_MODBUS;
        }
        
        while (Index < sWmDigVar.nModbus_u8)
        {
            sWmDigVar.sModbInfor[Index].MType_u8 = aTEMP[3 + Index*2]; 
            sWmDigVar.sModbInfor[Index].SlaveId_u8 = aTEMP[4 + Index*2]; 
            
            if (sWmDigVar.sModbInfor[Index].MType_u8 >= __MET_UNKNOWN)
                sWmDigVar.sModbInfor[Index].MType_u8 = __MET_UNKNOWN;
            
            Index++;
        }        
    } else
    {
        AppWm_Save_WM_Dig_Infor();
    }
#endif
}

void AppWm_Save_WM_Dig_Infor (void)
{
#ifdef USING_INTERNAL_MEM
    uint8_t aTEMP[64] = {0};
    
    if (sWmDigVar.nModbus_u8 > MAX_SLAVE_MODBUS)
    {
        sWmDigVar.nModbus_u8 = MAX_SLAVE_MODBUS;
    }
    
    aTEMP[0] = BYTE_TEMP_FIRST;
    aTEMP[1] = MAX_SLAVE_MODBUS * 2 + 1;
    aTEMP[2] = sWmDigVar.nModbus_u8;
    
    for (uint8_t i = 0; i < sWmDigVar.nModbus_u8; i++)
    {
        aTEMP[3 + i*2] = sWmDigVar.sModbInfor[i].MType_u8;
        aTEMP[4 + i*2] = sWmDigVar.sModbInfor[i].SlaveId_u8;
    }
    
    OnchipFlashPageErase(ADDR_MODBUS_INFOR);
    OnchipFlashWriteData(ADDR_MODBUS_INFOR, &aTEMP[0], 64);
#endif
}



void AppWm_Init_Default_Pressure (void)
{
    for (uint8_t i = 0; i < MAX_CHANNEL; i++)
    {
        sWmVar.aPRESSURE[i].sLinearInter.Factor_u16 = 100;
        sWmVar.aPRESSURE[i].sLinearInter.InMin_u16 = 4;
        sWmVar.aPRESSURE[i].sLinearInter.InMax_u16 = 20;
        sWmVar.aPRESSURE[i].sLinearInter.InUnit_u8 = _UNIT_MILIAM;
        sWmVar.aPRESSURE[i].sLinearInter.OutMin_u16 = 0;
        sWmVar.aPRESSURE[i].sLinearInter.OutMax_u16 = 10;
        sWmVar.aPRESSURE[i].sLinearInter.OutUnit_u8 = _UNIT_BAR;
    }
}


void AppWm_Init_Press_Infor (void)
{
#ifdef USING_INTERNAL_MEM
    uint8_t	aTEMP[128] = {0};
    uint8_t temp = FLASH_BYTE_EMPTY;
    uint16_t Pos = 0;
    
    temp = *(__IO uint8_t*) ADDR_PRESS_INFOR;   
	if (temp == BYTE_TEMP_FIRST)  
    {
        OnchipFlashReadData(ADDR_PRESS_INFOR, &aTEMP[0], 128);  //6*(14) = 84 byte
        
        Pos = 2;
        
        for (uint8_t i = 0; i < MAX_CHANNEL; i++)
        {
            sWmVar.aPRESSURE[i].sLinearInter.InMin_u16 = aTEMP[Pos] << 8 | aTEMP[Pos + 1];
            Pos += 2;
            sWmVar.aPRESSURE[i].sLinearInter.InMax_u16 = aTEMP[Pos] << 8 | aTEMP[Pos + 1];
            Pos += 2;
            sWmVar.aPRESSURE[i].sLinearInter.OutMin_u16 = aTEMP[Pos] << 8 | aTEMP[Pos + 1];
            Pos += 2;
            sWmVar.aPRESSURE[i].sLinearInter.OutMax_u16 = aTEMP[Pos] << 8 | aTEMP[Pos + 1];
            Pos += 2;
            sWmVar.aPRESSURE[i].sLinearInter.Factor_u16 = aTEMP[Pos] << 8 | aTEMP[Pos + 1];
            Pos += 2;
            sWmVar.aPRESSURE[i].sLinearInter.InUnit_u8 = aTEMP[Pos];
            Pos += 1;
            sWmVar.aPRESSURE[i].sLinearInter.OutUnit_u8 = aTEMP[Pos];
            Pos += 1;
            sWmVar.aPRESSURE[i].Calib_i16 = aTEMP[Pos] << 8 | aTEMP[Pos + 1];
            Pos += 2;
            sWmVar.aPRESSURE[i].sLinearInter.Type_u8 = MIN(aTEMP[Pos], 1);
            Pos += 1;
        }
    } else
    {
        AppWm_Save_Press_Infor();
    }
#endif
}

void AppWm_Save_Press_Infor (void)
{
#ifdef USING_INTERNAL_MEM
    uint8_t aTEMP[128] = {0};
    uint8_t Count = 0;
    
    aTEMP[Count++] = BYTE_TEMP_FIRST;
    aTEMP[Count++] = MAX_CHANNEL * 11;
    
    for (uint8_t i = 0; i < MAX_CHANNEL; i++)
    {
        aTEMP[Count++] = (uint8_t) (sWmVar.aPRESSURE[i].sLinearInter.InMin_u16 >> 8);
        aTEMP[Count++] = (uint8_t) (sWmVar.aPRESSURE[i].sLinearInter.InMin_u16);
        aTEMP[Count++] = (uint8_t) (sWmVar.aPRESSURE[i].sLinearInter.InMax_u16 >> 8);
        aTEMP[Count++] = (uint8_t) (sWmVar.aPRESSURE[i].sLinearInter.InMax_u16);
        aTEMP[Count++] = (uint8_t) (sWmVar.aPRESSURE[i].sLinearInter.OutMin_u16 >> 8);
        aTEMP[Count++] = (uint8_t) (sWmVar.aPRESSURE[i].sLinearInter.OutMin_u16);
        aTEMP[Count++] = (uint8_t) (sWmVar.aPRESSURE[i].sLinearInter.OutMax_u16 >> 8);
        aTEMP[Count++] = (uint8_t) (sWmVar.aPRESSURE[i].sLinearInter.OutMax_u16);
        aTEMP[Count++] = (uint8_t) (sWmVar.aPRESSURE[i].sLinearInter.Factor_u16 >> 8);
        aTEMP[Count++] = (uint8_t) (sWmVar.aPRESSURE[i].sLinearInter.Factor_u16);
        aTEMP[Count++] = (uint8_t) (sWmVar.aPRESSURE[i].sLinearInter.InUnit_u8);
        aTEMP[Count++] = (uint8_t) (sWmVar.aPRESSURE[i].sLinearInter.OutUnit_u8);
        aTEMP[Count++] = (uint8_t) (sWmVar.aPRESSURE[i].Calib_i16 >> 8);
        aTEMP[Count++] = (uint8_t) (sWmVar.aPRESSURE[i].Calib_i16);
        aTEMP[Count++] = (uint8_t) (sWmVar.aPRESSURE[i].sLinearInter.Type_u8);
    } 

    OnchipFlashPageErase(ADDR_PRESS_INFOR);
    OnchipFlashWriteData(ADDR_PRESS_INFOR, &aTEMP[0], 128);
#endif
}


/*
    Func: Measure pressure process
        + On Power during 3s.   
        + Read ADC each channel
            ++ each channel get 10 sample and cacul average
            ...
        + Off Power and finish    
*/
uint8_t AppWm_Meas_Pressure_Process (uint32_t *delaynext)
{   
    static uint8_t Step_u8 = 0;
    static uint8_t Index = 0;
     
    uint32_t Channel = 0;
    uint8_t Result = PENDING;
    int32_t mVolTemp = 0;
    
    switch(Step_u8)
    {
        case 0:
            /* On 12V power */
//            if (sVout.mVol_u32 >= 10200)
//                V12_IN_ON;
//            else /* on Boot power */ 
                V_PIN_ON;
            
            Step_u8++;
            *delaynext = 3000; //delay 3s truoc khi vao do
            break;
        case 1:  //6 kenh
            if (Index < MAX_CHANNEL)
            {
                switch (Index)
                {
                    case 0: Channel = ADC_CHANNEL_10; break;
                    case 1: Channel = ADC_CHANNEL_11; break;
                    case 2: Channel = ADC_CHANNEL_12; break;
                    case 3: Channel = ADC_CHANNEL_13; break;
                    case 4: Channel = ADC_CHANNEL_14; break;
                    case 5: Channel = ADC_CHANNEL_15; break;
                    default: Channel = ADC_CHANNEL_10; break;
                }
                
                mVolTemp = AppWm_Pressure_ADC_Vol (Channel);
                //Neu gia tri tra ve != -1: do ket thuc
                if (mVolTemp != -1)
                {
                    sWmVar.aPRESSURE[Index].VolAdc_u32 = mVolTemp;
                    sWmVar.aPRESSURE[Index].mVol_i32 = mVolTemp - sWmVar.aPRESSURE[Index].Calib_i16;
                    //convert gia tri mVol sang gia tri don vi ap suat
                #ifdef PRESSURE_DECODE
                    //Chuyen doi Pressure ra gia tri thuc te
                    sWmVar.aPRESSURE[Index].Val_i32 = (int32_t) AppWm_Linear_Interpolation(sWmVar.aPRESSURE[Index].mVol_i32 * 100, 
                                                                                         sWmVar.aPRESSURE[Index].sLinearInter.InMin_u16 * sWmVar.aPRESSURE[Index].sLinearInter.Factor_u16, 
                                                                                         sWmVar.aPRESSURE[Index].sLinearInter.InMax_u16 * sWmVar.aPRESSURE[Index].sLinearInter.Factor_u16,
                                                                                         sWmVar.aPRESSURE[Index].sLinearInter.OutMin_u16 * 1000, 
                                                                                         sWmVar.aPRESSURE[Index].sLinearInter.OutMax_u16 * 1000 );
                #else
                    sWmVar.aPRESSURE[Index].Val_i32 = sWmVar.aPRESSURE[Index].mVol_u32;
                #endif
                    
                    Index++;
                }
            } else
            {
                Index = 0;
                Step_u8++;
            }
            //enable 5m read again.
            *delaynext = 5;
            break;
        case 2: 
            if (sModemInfor.ModePower_u8 == _POWER_MODE_SAVE)
            {
                /* Off power for sensor */
                V12_IN_OFF;
                V_PIN_OFF;  
            }
            
            Step_u8 = 0; 
            Result = true;
            break;
        default:
            break;
    }   
    
    return Result;
}




uint16_t AppWm_Packet_Setting (uint8_t *aData, uint8_t chann)
{
    sData pData = {aData, 0};
    uint8_t aSERI_SENSOR[4] = {"0001"};
    uint8_t TempOne = 1;
    aSERI_SENSOR[3] = chann + 0x31;
        
    SV_Protocol_Packet_Data(pData.Data_a8, &pData.Length_u16, OBIS_SERI_SENSOR, &aSERI_SENSOR, 4, 0xAA);
    SV_Protocol_Packet_Data(pData.Data_a8, &pData.Length_u16, OBIS_SETT_PULSE_FACTOR, &TempOne, 1, sPulse[chann].Factor_u8);
    SV_Protocol_Packet_Data(pData.Data_a8, &pData.Length_u16, OBIS_SETT_PULSE_START, &sPulse[chann].Start_u32, 4, sPulse[chann].Factor_u8);
    
    SV_Protocol_Packet_Data(pData.Data_a8, &pData.Length_u16, OBIS_SETT_LI_IN_MIN, &sWmVar.aPRESSURE[chann].sLinearInter.InMin_u16, 2, 0);
    SV_Protocol_Packet_Data(pData.Data_a8, &pData.Length_u16, OBIS_SETT_LI_IN_MAX, &sWmVar.aPRESSURE[chann].sLinearInter.InMax_u16, 2, 0);
    SV_Protocol_Packet_Data(pData.Data_a8, &pData.Length_u16, OBIS_SETT_LI_IN_UNIT, &sWmVar.aPRESSURE[chann].sLinearInter.InUnit_u8, 1, 0xAA);
    
    SV_Protocol_Packet_Data(pData.Data_a8, &pData.Length_u16, OBIS_SETT_LI_OUT_MIN, &sWmVar.aPRESSURE[chann].sLinearInter.OutMin_u16, 2, 0);
    SV_Protocol_Packet_Data(pData.Data_a8, &pData.Length_u16, OBIS_SETT_LI_OUT_MAX, &sWmVar.aPRESSURE[chann].sLinearInter.OutMax_u16, 2, 0);
    SV_Protocol_Packet_Data(pData.Data_a8, &pData.Length_u16, OBIS_SETT_LI_OUT_UNIT, &sWmVar.aPRESSURE[chann].sLinearInter.OutUnit_u8, 1, 0xAA);
    
    SV_Protocol_Packet_Data(pData.Data_a8, &pData.Length_u16, OBIS_SETT_PRESS_FACTOR, &sWmVar.aPRESSURE[chann].sLinearInter.Factor_u16, 2, 0);
    SV_Protocol_Packet_Data(pData.Data_a8, &pData.Length_u16, OBIS_SETT_PRESS_TYPE, &sWmVar.aPRESSURE[chann].sLinearInter.Type_u8, 1, 0xAA);
    
    return pData.Length_u16;
}

/*
    @brief  Log Data TSVH
*/
void AppSensor_Log_Data_TSVH (void)
{
  if(sRTC.year > 20)
  {
    uint8_t     aMessData[256] = {0};
    uint8_t     Length = 0;
    
//    if (sRTC.year <= 20)
//        return;
    
    Length = SensorRS485_Packet_TSVH (&aMessData[0]);
//#ifdef USING_APP_MEM
//    AppMem_Write_Data(_MEM_DATA_TSVH_A, &aMessData[0], Length, sRecTSVH.SizeRecord_u16);
//#endif
        
#ifdef USING_APP_MEM
    AppMem_Write_Data(sAppMem.RecMemType_u8, _MEM_DATA_TSVH_A, 0, 
                      &aMessData[0], Length, sRecTSVH.SizeRecord_u16);
#endif
  }
}
/*
    @brief  Packet TSVH
*/
uint8_t SensorRS485_Packet_TSVH (uint8_t *pData)
{
    uint16_t    length = 0;
    uint16_t	i = 0;
    uint8_t     TempCrc = 0;  
    
    //----------sTime--------------------
    SV_Protocol_Packet_Data(pData, &length, OBIS_TIME_DEVICE, &sRTC, 6, 0xAA);
    
//    if(sStateCtrlOxy.StateSensorOxy == _OXY_CONNECT)
//    {
//        //----------  Temperature ------
//        AppCtrlOxy_Packet_Data(pData, &length, OBIS_ENVI_OXY_TEMPERATURE, &sParamMeasure.Temp, 2, 0xFE);
//    }
//    
//    if(sParamCtrlPH.StateSensorPH == _OXY_CONNECT)
//    {
//        //----------  pH_Water ------
//        AppCtrlOxy_Packet_Data(pData, &length, OBIS_ENVI_PH_WATER, &sParamCtrlPH.pH_Measure, 2, 0xFE);
//    }
//    
//    if(sStateCtrlOxy.StateSensorOxy == _OXY_CONNECT)
//    {
//        //----------  Oxy mg/L ---------
//        AppCtrlOxy_Packet_Data(pData, &length, OBIS_ENVI_OXY_MG_L, &sParamMeasure.Oxy_Mg_L, 2, 0xFE);
//        
//        //----------  Salinity ------
//        AppCtrlOxy_Packet_Data(pData, &length, OBIS_ENVI_SALINITY, &sParamMeasure.Salinity, 2, 0xFE);
//
//        //----------  Oxy Percent ------
//        AppCtrlOxy_Packet_Data(pData, &length, OBIS_ENVI_OXY_PERCENT, &sParamMeasure.Oxy_Percent, 2, 0xFE);
//    }
//    
//    //---------- Oxy State ---------
//    AppCtrlOxy_Packet_Data(pData, &length, OBIS_ENVI_STATE_MACHINE, &sStateCtrlOxy.StateMachine, 1, 0xAA);
//    AppCtrlOxy_Packet_Data(pData, &length, OBIS_STATE_OXY_1, &sStateCtrlOxy.aStateOxy[0], 1, 0xAA);
//    AppCtrlOxy_Packet_Data(pData, &length, OBIS_STATE_OXY_2, &sStateCtrlOxy.aStateOxy[1], 1, 0xAA);
//    AppCtrlOxy_Packet_Data(pData, &length, OBIS_STATE_OXY_3, &sStateCtrlOxy.aStateOxy[2], 1, 0xAA);
//    AppCtrlOxy_Packet_Data(pData, &length, OBIS_STATE_OXY_4, &sStateCtrlOxy.aStateOxy[3], 1, 0xAA);
//    
//    AppCtrlOxy_Packet_Data(pData, &length, OBIS_STATE_PUMP_PH, &sParamCtrlPH.StateCtrl, 1, 0xAA);
       
        //----------  Clo_Du ------
        if(sHandleRs485.State_Recv_Clo == 1)
            SensorRS485_Packet_Data(pData, &length, OBIS_ENVI_CLO_DU, &sDataSensorMeasure.sClo_Du_Filter.Value, 2, sDataSensorMeasure.sClo_Du_Filter.Scale);
        
        //----------  pH ------
        if(sHandleRs485.State_Recv_pH == 1)
            SensorRS485_Packet_Data(pData, &length, OBIS_ENVI_PH_WATER, &sDataSensorMeasure.spH_Water_Filter.Value, 2, sDataSensorMeasure.spH_Water_Filter.Scale);
        
        //----------  Do Duc------
        if(sHandleRs485.State_Recv_NTU == 1)
            SensorRS485_Packet_Data(pData, &length, OBIS_ENVI_NTU, &sDataSensorMeasure.sNTU_Filter.Value, 2, sDataSensorMeasure.sNTU_Filter.Scale);
        
        //----------  SALINITY %------
        if(sHandleRs485.State_Recv_Salinity == 1)
            SensorRS485_Packet_Data(pData, &length, OBIS_ENVI_SALINITY_UNIT, &sDataSensorMeasure.sSalinity_Filter.Value, 2, sDataSensorMeasure.sSalinity_Filter.Scale);
        
        //----------  Temperature ------
        if(sHandleRs485.State_Recv_Temperature == 1)
            SensorRS485_Packet_Data(pData, &length, OBIS_ENVI_OXY_TEMPERATURE, &sDataSensorMeasure.sTemperature_Filter.Value, 2, sDataSensorMeasure.sTemperature_Filter.Scale);
        
        //----------  EC ------
        if(sHandleRs485.State_Recv_EC == 1)
            SensorRS485_Packet_Data(pData, &length, OBIS_ENVI_EC, &sDataSensorMeasure.sEC_Filter.Value, 2, sDataSensorMeasure.sEC_Filter.Scale);
    
    //----------Tan suat--------------------
    SV_Protocol_Packet_Data(pData, &length, OBIS_RSSI_1, &sSimCommInfor.RSSI_u8, 1, 0x00);
    SV_Protocol_Packet_Data(pData, &length, OBIS_FREQ_SEND, &sModemInfor.sFrequence.DurOnline_u32 , 2, 0x00);
    // caculator crc
    length++;
	for (i = 0; i < (length - 1); i++)
		TempCrc ^= pData[i];

    pData[length-1] = TempCrc;
    
    return length;
}

/*
    @brief  Packet Data
*/
void SensorRS485_Packet_Data (uint8_t *pTarget, uint16_t *LenTarget, uint8_t Obis, 
                             void *pData, uint8_t LenData, uint8_t Scale)
{
    uint16_t Pos = *LenTarget;
//    uint16_t i= 0;
//    uint8_t *TempU8;
    
    if (LenData == 0)
        return;
    
    pTarget[Pos++] = Obis;
    pTarget[Pos++] = LenData;
            
    switch (Obis)
    {
        case OBIS_ENVI_STATE_MACHINE:
        case OBIS_ENVI_OXY_MG_L:
        case OBIS_ENVI_OXY_PERCENT:
        case OBIS_ENVI_OXY_TEMPERATURE:
        case OBIS_ENVI_EC:
        case OBIS_ENVI_SALINITY:
//        case OBIS_ENVI_ID_SERVER:
        case OBIS_ENVI_PH_WATER:
//        case OBIS_STATE_OXY_1:
//        case OBIS_STATE_OXY_2:
//        case OBIS_STATE_OXY_3:
//        case OBIS_STATE_OXY_4:
//        case OBIS_STATE_PUMP_PH:
        case OBIS_ENVI_SALINITY_UNIT:
        case OBIS_ENVI_CLO_DU:
        case OBIS_ENVI_NTU:
            AppComm_Sub_Packet_Integer(pTarget, &Pos, pData, LenData);
            //
            if (Scale != 0xAA)
                pTarget[Pos++] = Scale;
            break;
            
//        case OBIS_ENVI_STATE_OXY:
//            TempU8 = (uint8_t *) pData;
//            for (i = 0; i < LenData; i++)
//                pTarget[Pos++] = *(TempU8 + i);
//            break; 
            
        default:
            break;
    }
    
    *LenTarget = Pos ;
}

void AppComm_Sub_Packet_Integer (uint8_t *pTarget, uint16_t *LenTarget, void *Data, uint8_t LenData)
{
    uint16_t Pos = *LenTarget;
    
    uint8_t  *TempU8;
    uint16_t *TempU16;
    uint32_t *TempU32;
    uint64_t *TempU64;
    
    switch (LenData)
    {
        case 1:    
            TempU8 = (uint8_t *) Data;
            pTarget[Pos++] = *TempU8;
            break;
        case 2:
            TempU16 = (uint16_t *) Data;
            pTarget[Pos++] = ((*TempU16) >> 8) & 0xFF;
            pTarget[Pos++] =  (*TempU16) & 0xFF;
            break;
        case 4:
            TempU32 = (uint32_t *) Data;
            pTarget[Pos++] = ((*TempU32) >> 24) & 0xFF;
            pTarget[Pos++] = ((*TempU32) >> 16) & 0xFF;
            pTarget[Pos++] = ((*TempU32) >> 8) & 0xFF;
            pTarget[Pos++] =  (*TempU32) & 0xFF;
            break;
        case 8:
            TempU64 = (uint64_t *) Data;
            pTarget[Pos++] = ((*TempU64) >> 56) & 0xFF;
            pTarget[Pos++] = ((*TempU64) >> 48) & 0xFF;
            pTarget[Pos++] = ((*TempU64) >> 40) & 0xFF;
            pTarget[Pos++] = ((*TempU64) >> 32) & 0xFF;
            pTarget[Pos++] = ((*TempU64) >> 24) & 0xFF;
            pTarget[Pos++] = ((*TempU64) >> 16) & 0xFF;
            pTarget[Pos++] = ((*TempU64) >> 8) & 0xFF;
            pTarget[Pos++] =  (*TempU64) & 0xFF;
            break;
        default:
            break;
    }
    
    *LenTarget = Pos ;
}

void Handle_Data_CM44(uint32_t u32_Clo_Du, uint32_t u32_pH_Water, uint32_t u32_NTU, uint32_t u32_Salinity, uint32_t u32_Temp)
{
    float f_Clo_Du = 0;
    float f_pH_Water = 0;
    float f_NTU = 0 ;
    float f_Salinity = 0;
    float f_Temperature = 0;
    uint32_t stamp_div = 0;
  
    Convert_uint32Hex_To_Float(u32_Clo_Du, &f_Clo_Du);
    Convert_uint32Hex_To_Float(u32_pH_Water, &f_pH_Water);
    Convert_uint32Hex_To_Float(u32_NTU, &f_NTU);
    Convert_uint32Hex_To_Float(u32_Salinity, &f_Salinity);
    Convert_uint32Hex_To_Float(u32_Temp, &f_Temperature);
    

    if(f_Clo_Du >= 0)    
    {
      stamp_div = (uint32_t)(f_Clo_Du*1000)%10;
      if(stamp_div >= 5) f_Clo_Du += 0.01;
    }
    else                
    {
      stamp_div = (uint32_t)((0-f_Clo_Du)*1000)%10;
      if(stamp_div >= 5) f_Clo_Du -= 0.01;
    }
    
    if(f_pH_Water >= 0)    
    {
      stamp_div = (uint32_t)(f_pH_Water*1000)%10;
      if(stamp_div >= 5) f_pH_Water += 0.01;
    }
    else                
    {
      stamp_div = (uint32_t)((0-f_pH_Water)*1000)%10;
      if(stamp_div >= 5) f_pH_Water -= 0.01;
    }

    
    if(f_NTU >= 0)    
    {
      stamp_div = (uint32_t)(f_NTU*100)%10;
      if(stamp_div >= 5) f_NTU += 0.1;
    }
    else                
    {
      stamp_div = (uint32_t)((0-f_NTU)*100)%10;
      if(stamp_div >= 5) f_NTU -= 0.1;
    }
    
    if(f_Salinity >= 0)    
    {
      stamp_div = (uint32_t)(f_Salinity*1000)%10;
      if(stamp_div >= 5) f_Salinity += 0.01;
    }
    else                
    {
      stamp_div = (uint32_t)((0-f_Salinity)*1000)%10;
      if(stamp_div >= 5) f_Salinity -= 0.01;
    }
    
    if(f_Temperature >= 0)    
    {
      stamp_div = (uint32_t)(f_Temperature*1000)%10;
      if(stamp_div >= 5) f_Temperature += 0.01;
    }
    else                
    {
      stamp_div = (uint32_t)((0-f_Temperature)*1000)%10;
      if(stamp_div >= 5) f_Temperature -= 0.01;
    }
    
    sDataSensorMeasure.sClo_Du.Value = (uint16_t)(f_Clo_Du*100);
    sDataSensorMeasure.spH_Water.Value = (uint16_t)(f_pH_Water*100);
    sDataSensorMeasure.sNTU.Value = (uint16_t)(f_NTU*10);
    sDataSensorMeasure.sSalinity.Value = (uint16_t)(f_Salinity*100);
    sDataSensorMeasure.sTemperature.Value = (uint16_t)(f_Temperature*100);
}





