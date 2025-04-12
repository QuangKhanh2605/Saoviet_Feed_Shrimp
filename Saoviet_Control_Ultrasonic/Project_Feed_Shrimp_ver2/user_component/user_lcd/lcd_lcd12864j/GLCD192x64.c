#include "GLCD192x64.h"
//#include "cmsis_os.h"
#include "string.h"
#include "main.h"
uint8_t screen_x = 0; /* Number culumn 0->LCD128X64_WIDTH */
uint8_t screen_y = 0; /* ROW */
uint8_t TimeOnLcd = 0;
static const char font5x8[] =
{
    0x00, 0x00, 0x00, 0x00, 0x00,   /* (SPACE) */
    0x00, 0x00, 0x5F, 0x00, 0x00,   /* ! */
    0x00, 0x07, 0x00, 0x07, 0x00,   /* " */
    0x14, 0x7F, 0x14, 0x7F, 0x14,   /* # */
    0x24, 0x2A, 0x7F, 0x2A, 0x12,   /* $ */
    0x23, 0x13, 0x08, 0x64, 0x62,   /* % */
    0x36, 0x49, 0x55, 0x22, 0x50,   /* & */
    0x00, 0x05, 0x03, 0x00, 0x00,   /* ' */
    0x00, 0x1C, 0x22, 0x41, 0x00,   /* ( */
    0x00, 0x41, 0x22, 0x1C, 0x00,   /* ) */
    0x08, 0x2A, 0x1C, 0x2A, 0x08,   /* * */
    0x08, 0x08, 0x3E, 0x08, 0x08,   /* + */
    0x00, 0x50, 0x30, 0x00, 0x00,   /* , */
    0x08, 0x08, 0x08, 0x08, 0x08,   /* - */
    0x00, 0x30, 0x30, 0x00, 0x00,   /* . */
    0x20, 0x10, 0x08, 0x04, 0x02,   /* / */
    0x3E, 0x51, 0x49, 0x45, 0x3E,   /* 0 */
    0x00, 0x42, 0x7F, 0x40, 0x00,   /* 1 */
    0x42, 0x61, 0x51, 0x49, 0x46,   /* 2 */
    0x21, 0x41, 0x45, 0x4B, 0x31,   /* 3 */
    0x18, 0x14, 0x12, 0x7F, 0x10,   /* 4 */
    0x27, 0x45, 0x45, 0x45, 0x39,   /* 5 */
    0x3C, 0x4A, 0x49, 0x49, 0x30,   /* 6 */
    0x01, 0x71, 0x09, 0x05, 0x03,   /* 7 */
    0x36, 0x49, 0x49, 0x49, 0x36,   /* 8 */
    0x06, 0x49, 0x49, 0x29, 0x1E,   /* 9 */
    0x00, 0x36, 0x36, 0x00, 0x00,   /* : */
    0x00, 0x56, 0x36, 0x00, 0x00,   /* ; */
    0x00, 0x08, 0x14, 0x22, 0x41,   /* < */
    0x14, 0x14, 0x14, 0x14, 0x14,   /* = */
    0x41, 0x22, 0x14, 0x08, 0x00,   /* > */
    0x02, 0x01, 0x51, 0x09, 0x06,   /* ? */
    0x32, 0x49, 0x79, 0x41, 0x3E,   /* @ */
    0x7E, 0x11, 0x11, 0x11, 0x7E,   /* A */
    0x7F, 0x49, 0x49, 0x49, 0x36,   /* B */
    0x3E, 0x41, 0x41, 0x41, 0x22,   /* C */
    0x7F, 0x41, 0x41, 0x22, 0x1C,   /* D */
    0x7F, 0x49, 0x49, 0x49, 0x41,   /* E */
    0x7F, 0x09, 0x09, 0x01, 0x01,   /* F */
    0x3E, 0x41, 0x41, 0x51, 0x32,   /* G */
    0x7F, 0x08, 0x08, 0x08, 0x7F,   /* H */
    0x00, 0x41, 0x7F, 0x41, 0x00,   /* I */
    0x20, 0x40, 0x41, 0x3F, 0x01,   /* J */
    0x7F, 0x08, 0x14, 0x22, 0x41,   /* K */
    0x7F, 0x40, 0x40, 0x40, 0x40,   /* L */
    0x7F, 0x02, 0x04, 0x02, 0x7F,   /* M */
    0x7F, 0x04, 0x08, 0x10, 0x7F,   /* N */
    0x3E, 0x41, 0x41, 0x41, 0x3E,   /* O */
    0x7F, 0x09, 0x09, 0x09, 0x06,   /* P */
    0x3E, 0x41, 0x51, 0x21, 0x5E,   /* Q */
    0x7F, 0x09, 0x19, 0x29, 0x46,   /* R */
    0x46, 0x49, 0x49, 0x49, 0x31,   /* S */
    0x01, 0x01, 0x7F, 0x01, 0x01,   /* T */
    0x3F, 0x40, 0x40, 0x40, 0x3F,   /* U */
    0x1F, 0x20, 0x40, 0x20, 0x1F,   /* V */
    0x7F, 0x20, 0x18, 0x20, 0x7F,   /* W */
    0x63, 0x14, 0x08, 0x14, 0x63,   /* X */
    0x03, 0x04, 0x78, 0x04, 0x03,   /* Y */
    0x61, 0x51, 0x49, 0x45, 0x43,   /* Z */
    0x00, 0x00, 0x7F, 0x41, 0x41,   /* [ */
    0x02, 0x04, 0x08, 0x10, 0x20,   /* "\" */
    0x41, 0x41, 0x7F, 0x00, 0x00,   /* ] */
    0x04, 0x02, 0x01, 0x02, 0x04,   /* ^ */
    0x40, 0x40, 0x40, 0x40, 0x40,   /* _ */
    0x00, 0x01, 0x02, 0x04, 0x00,   /* ` */
    0x20, 0x54, 0x54, 0x54, 0x78,   /* a */
    0x7F, 0x48, 0x44, 0x44, 0x38,   /* b */
    0x38, 0x44, 0x44, 0x44, 0x20,   /* c */
    0x38, 0x44, 0x44, 0x48, 0x7F,   /* d */
    0x38, 0x54, 0x54, 0x54, 0x18,   /* e */
    0x08, 0x7E, 0x09, 0x01, 0x02,   /* f */
    0x08, 0x14, 0x54, 0x54, 0x3C,   /* g */
    0x7F, 0x08, 0x04, 0x04, 0x78,   /* h */
    0x00, 0x44, 0x7D, 0x40, 0x00,   /* i */
    0x20, 0x40, 0x44, 0x3D, 0x00,   /* j */
    0x00, 0x7F, 0x10, 0x28, 0x44,   /* k */
    0x00, 0x41, 0x7F, 0x40, 0x00,   /* l */
    0x7C, 0x04, 0x18, 0x04, 0x78,   /* m */
    0x7C, 0x08, 0x04, 0x04, 0x78,   /* n */
    0x38, 0x44, 0x44, 0x44, 0x38,   /* o */
    0x7C, 0x14, 0x14, 0x14, 0x08,   /* p */
    0x08, 0x14, 0x14, 0x18, 0x7C,   /* q */
    0x7C, 0x08, 0x04, 0x04, 0x08,   /* r */
    0x48, 0x54, 0x54, 0x54, 0x20,   /* s */
    0x04, 0x3F, 0x44, 0x40, 0x20,   /* t */
    0x3C, 0x40, 0x40, 0x20, 0x7C,   /* u */
    0x1C, 0x20, 0x40, 0x20, 0x1C,   /* v */
    0x3C, 0x40, 0x30, 0x40, 0x3C,   /* w */
    0x44, 0x28, 0x10, 0x28, 0x44,   /* x */
    0x0C, 0x50, 0x50, 0x50, 0x3C,   /* y */
    0x44, 0x64, 0x54, 0x4C, 0x44,   /* z */
    0x00, 0x08, 0x36, 0x41, 0x00,   /* { */
    0x00, 0x00, 0x7F, 0x00, 0x00,   /* | */
    0x00, 0x41, 0x36, 0x08, 0x00,   /* } */
    0x08, 0x08, 0x2A, 0x1C, 0x08,   /* -> */
    0x08, 0x1C, 0x2A, 0x08, 0x08,   /* <- */
    0x60, 0x70, 0x78, 0x7C, 0x7E,   /* Connect */
    0x1C, 0x36, 0x2A, 0x36, 0x1C,    /* Disconnect */
    0x30, 0x3E, 0x3F, 0x3E, 0x30, /* BELL SYMBOL */
    0x07, 0x08, 0x7F, 0x09, 0x07, /* He so goc pha */
    0x44, 0x44, 0x44, 0x44, 0x7C, /* Not PIN - 0    84 */ 
    0x7C, 0x44, 0x44, 0x44, 0x7C, /* 25% PIN */
    0x7C, 0x7C, 0x44, 0x44, 0x7C, /* 50% PIN */
    0x7C, 0x7C, 0x7C, 0x44, 0x7C, /* 75% PIN */
    0x7C, 0x7C, 0x7C, 0x7C, 0x7C,  /* 100% PIN */
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x40, 0x60, 0x70, 0x78, 0x7C,   /* Interner Connect */
    0x45, 0x62, 0x75, 0x78, 0x7C,   /* Interner DisConnect */
    0x00, 0x00, 0x02, 0x05, 0x02,    // /* Do C*/ 0x8C
    0x40, 0x5C, 0x4A, 0x5C, 0x40,   // Oxy Auto   
    0x5E, 0x44, 0x48, 0x44, 0x5E,   //Oxy Manual
    0x40, 0x54, 0x48, 0x54, 0x40,   //Oxy Off
    0x1C, 0x2A, 0x6B, 0x2A, 0x1C,   //Connect Sensor Oxy
    0x18, 0x2C, 0x68, 0x2C, 0x18    //Disconnect Sensor Oxy
};

void GLCD_Initialize(void)
{
    uint8_t i;
    HAL_GPIO_WritePin(LCD128X64_PORT_RST, LCD128X64_PIN_RST, GPIO_PIN_SET);     /* |RST */
    for(i = 0; i < LCD128X64_WIDTH/LCD128X64_HIGHT; i++)    /* 3 Ks0108 */
    {
        GLCD_WriteCommand((DISPLAY_ON_CMD | ON), i);  /* ON CS1,2,3 */
    }
    GLCD_Delay();
    TimeOnLcd = 1;
}

void GLCD_WriteCommand(uint8_t commandToWrite, uint8_t controller)
{
    /* Set cac chan Data la Output, Write Command */
    GLCD_InitDataOut();
    /* reset ca RS va RW */
    GPIO_ResetBits(LCD128X64_PORT_RS, LCD128X64_PIN_RS);    /* RS = 0 */
    GPIO_ResetBits(LCD128X64_PORT_RW, LCD128X64_PIN_RW);    /* RW = 0 */
    GLCD_EnableController(controller);  /* Enable CSx */
    /* Out data D0->D7 */
    GLCD_OutData_Cmd(commandToWrite);
    GPIO_SetBits(LCD128X64_PORT_EN, LCD128X64_PIN_EN); /* Enable GLCD */
    GLCD_Delay();
    GPIO_ResetBits(LCD128X64_PORT_EN, LCD128X64_PIN_EN); /* Disable GLCD */
    GLCD_DisableController(controller); /* Disable CSx */
}
/* Kiem tra trang thai cua GLCD,
    return = 1, GLCD dang ban
    return = 0, GLCD dang ranh
*/
uint8_t status = 0;
uint8_t GLCD_ReadStatus(uint8_t controller)
{
    /* Set cac chan Data la Input, doc bit busy */
    GLCD_InitDataIn();
    /* Tac dong vao cac chan dk de doc trang thai GLCD */
    GPIO_SetBits(LCD128X64_PORT_RW, LCD128X64_PIN_RW);   /* RW = 1 */
    GPIO_ResetBits(LCD128X64_PORT_RS, LCD128X64_PIN_RS); /* Rs = 0 */
    GLCD_EnableController(controller);  /* Lua chon chip de tac dong CS1,2,3 */
    GPIO_SetBits(LCD128X64_PORT_EN, LCD128X64_PIN_EN); /* GLCD bat dau hoat dong */
    GLCD_Delay(); /* Cho doc trang thai tu cac chan D0->D7 */
    status = GLCD_Read_DataPin();
    GPIO_ResetBits(LCD128X64_PORT_EN, LCD128X64_PIN_EN);    /* Disable GLCD */
    GLCD_DisableController(controller); /* Disable CHIP */
    return status;
}

void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_SET);
}
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_RESET);
}
void GLCD_Delay(void)
{
   /* uint16_t Time_out = 0;
   while(Time_out<5)
       Time_out++; */
}
void GLCD_EnableController(uint8_t controller)
{
    switch(controller)
    {
        case 0: /* CS1 */
                GPIO_SetBits(LCD128X64_PORT_CS1,LCD128X64_PIN_CS1);
                GPIO_ResetBits(LCD128X64_PORT_CS2,LCD128X64_PIN_CS2);
            break;
        case 1: /* CS2 */
                GPIO_SetBits(LCD128X64_PORT_CS2,LCD128X64_PIN_CS2);
                GPIO_ResetBits(LCD128X64_PORT_CS1,LCD128X64_PIN_CS1);
            break;
        case 2: /* CS3 */
          /*
            if(LCD128X64_WIDTH == 192)
                GPIO_ResetBits(LCD128X64_PORT_CS3,LCD128X64_PIN_CS3);
          */
            break;
        default:
            break;
    }
}
void GLCD_DisableController(uint8_t controller)
{
    switch(controller)
    {
        case 0: /* CS1 */
                GPIO_ResetBits(LCD128X64_PORT_CS1,LCD128X64_PIN_CS1);
            break;
        case 1: /* CS2 */
                GPIO_ResetBits(LCD128X64_PORT_CS2,LCD128X64_PIN_CS2);
            break;
        case 2: /* CS3 */
          /*
            if(LCD128X64_WIDTH == 192)
                GPIO_SetBits(LCD128X64_PORT_CS3,LCD128X64_PIN_CS3);
          */
            break;
        default:
            break;
    }
}

/* Xuat byte data tu MCU -> GLCD (D7->D0) */
void GLCD_OutData(uint8_t Data)
{
    if(Data&0x01)
    {
        GPIO_SetBits(LCD128X64_PORT_D0,LCD128X64_PIN_D0);
    }
    else
    {
        GPIO_ResetBits(LCD128X64_PORT_D0,LCD128X64_PIN_D0);
    }
    if(Data&0x02)
    {
        GPIO_SetBits(LCD128X64_PORT_D1,LCD128X64_PIN_D1);
    }
    else
    {
        GPIO_ResetBits(LCD128X64_PORT_D1,LCD128X64_PIN_D1);
    }
    if(Data&0x04)
    {
        GPIO_SetBits(LCD128X64_PORT_D2,LCD128X64_PIN_D2);
    }
    else
    {
        GPIO_ResetBits(LCD128X64_PORT_D2,LCD128X64_PIN_D2);
    }
    if(Data&0x08)
    {
        GPIO_SetBits(LCD128X64_PORT_D3,LCD128X64_PIN_D3);
    }
    else
    {
        GPIO_ResetBits(LCD128X64_PORT_D3,LCD128X64_PIN_D3);
    }
    if(Data&0x10)
    {
        GPIO_SetBits(LCD128X64_PORT_D4,LCD128X64_PIN_D4);
    }
    else
    {
        GPIO_ResetBits(LCD128X64_PORT_D4,LCD128X64_PIN_D4);
    }
    if(Data&0x20)
    {
        GPIO_SetBits(LCD128X64_PORT_D5,LCD128X64_PIN_D5);
    }
    else
    {
        GPIO_ResetBits(LCD128X64_PORT_D5,LCD128X64_PIN_D5);
    }
    if(Data&0x40)
    {
        GPIO_SetBits(LCD128X64_PORT_D6,LCD128X64_PIN_D6);
    }
    else
    {
        GPIO_ResetBits(LCD128X64_PORT_D6,LCD128X64_PIN_D6);
    }
    if(Data&0x80)
    {
        GPIO_SetBits(LCD128X64_PORT_D7,LCD128X64_PIN_D7);
    }
    else
    {
        GPIO_ResetBits(LCD128X64_PORT_D7,LCD128X64_PIN_D7);
    }
}

/* Xuat byte data tu MCU -> GLCD   (D0-D7) */
void GLCD_OutData_Cmd(uint8_t Data)
{
    if(Data&0x01)
        GPIO_SetBits(LCD128X64_PORT_D0,LCD128X64_PIN_D0);
    else
        GPIO_ResetBits(LCD128X64_PORT_D0,LCD128X64_PIN_D0);
    if(Data&0x02)
        GPIO_SetBits(LCD128X64_PORT_D1,LCD128X64_PIN_D1);
    else
        GPIO_ResetBits(LCD128X64_PORT_D1,LCD128X64_PIN_D1);
    if(Data&0x04)
        GPIO_SetBits(LCD128X64_PORT_D2,LCD128X64_PIN_D2);
    else
        GPIO_ResetBits(LCD128X64_PORT_D2,LCD128X64_PIN_D2);
    if(Data&0x08)
        GPIO_SetBits(LCD128X64_PORT_D3,LCD128X64_PIN_D3);
    else
        GPIO_ResetBits(LCD128X64_PORT_D3,LCD128X64_PIN_D3);
    if(Data&0x10)
        GPIO_SetBits(LCD128X64_PORT_D4,LCD128X64_PIN_D4);
    else
        GPIO_ResetBits(LCD128X64_PORT_D4,LCD128X64_PIN_D4);
    if(Data&0x20)
        GPIO_SetBits(LCD128X64_PORT_D5,LCD128X64_PIN_D5);
    else
        GPIO_ResetBits(LCD128X64_PORT_D5,LCD128X64_PIN_D5);
    if(Data&0x40)
        GPIO_SetBits(LCD128X64_PORT_D6,LCD128X64_PIN_D6);
    else
        GPIO_ResetBits(LCD128X64_PORT_D6,LCD128X64_PIN_D6);
    if(Data&0x80)
        GPIO_SetBits(LCD128X64_PORT_D7,LCD128X64_PIN_D7);
    else
        GPIO_ResetBits(LCD128X64_PORT_D7,LCD128X64_PIN_D7);
}

/* Write byte Data to GLCD */
void GLCD_WriteByteData(uint8_t dataToWrite)
{
    /* Set cac chan Data la Output, Write Command */
    GLCD_InitDataOut();
    GPIO_ResetBits(LCD128X64_PORT_RW, LCD128X64_PIN_RW);    /* RW = 0 */
    GPIO_SetBits(LCD128X64_PORT_RS, LCD128X64_PIN_RS);      /* RS = 1 */
    /* Out Data D0->D7 */
    GLCD_OutData(dataToWrite);
    GLCD_EnableController(screen_y / 64);
    GPIO_SetBits(LCD128X64_PORT_EN, LCD128X64_PIN_EN);
    GLCD_Delay();
    GPIO_ResetBits(LCD128X64_PORT_EN, LCD128X64_PIN_EN);
    GLCD_DisableController(screen_y / 64);
    screen_y++;
    if(screen_y>127)
    {
        screen_y = 0;
    }
}
/* Write String to LCD */
/* Auto break next page if String > max */
void GLCD_GoTo(uint8_t Page, uint8_t column)
{
    uint8_t i;
    uint8_t Temp_Page = 0;
    uint8_t Temp_Column = 0;
    /* Dao lai vi tri Page va column */
    Temp_Page = Page;//7-Page;
    Temp_Column = column;/* 127 - column */
    screen_x = Temp_Page;
    screen_y = Temp_Column;

    for(i = 0; i < LCD128X64_WIDTH/64; i++) /* Set ve goc x,0 */
    {
        GLCD_WriteCommand(DISPLAY_SET_X | Temp_Page,i); /* Set row */
        GLCD_WriteCommand(DISPLAY_SET_Y | 0,i);         /* Set column */
        GLCD_WriteCommand(DISPLAY_START_LINE | 0,i);
    }
    GLCD_WriteCommand((DISPLAY_SET_X | Temp_Page), (Temp_Column / 64));         /* Set row */
    GLCD_WriteCommand((DISPLAY_SET_Y | ((Temp_Column)%64)), (Temp_Column/64));  /* Set column */
}

void GLCD_WriteCharacter(uint8_t Char, uint8_t Font_Size)
{
    int8_t i = 0;
    uint8_t index_char = Char - 0x20;
    if(Font_Size == FONT5x7)
    {
        GLCD_WriteByteData(0x00);
        for(i=0;i<=4;i++)
        {
            GLCD_WriteByteData(font5x8[5*index_char+i]);
        }
    }
    else if (Font_Size == FONT_BATTERY) /* Font_Size = 2 */
    {
        GLCD_WriteByteData(0x00);
        GLCD_WriteByteData(0x7C);
        for (i = 0; i <= 4; i++)
        {
            GLCD_WriteByteData(font5x8[5 * index_char + i]);
        }
        GLCD_WriteByteData(0x38);
    }
}

/* Display String, start addr: x,y */
void GLCD_WriteString(char* String, uint8_t Page, uint8_t Column, uint8_t Font_Size)
{
    int8_t Length_String = 0;
    if(Page < 8)
    {
        //Column += Length_String*6;
        GLCD_GoTo(Page,Column);
        while(Length_String<strlen(String))
        {
            if(Font_Size == 1)
            {
                //GLCD_WriteCharacter(*(String+Length_String-1),Font_Size);
                GLCD_WriteCharacter(*(String+Length_String),Font_Size);
            }
            else
            {
                /* Clear khu vuc can hien thi */
                // GLCD_ClearArea(Row,Row+FONT_WIDTH,Column,Column+FONT_HIGHT);
                /* Write Data */
                GLCD_WriteCharacter(*String++, Font_Size);
            }
            Length_String++;
        }
    }
    /* Check error on/off contactor */
    if(GLCD_ReadStatus(0)&DISPLAY_STATUS_NOT_OK)
    {
        if(TimeOnLcd == 1)
        {
            TimeOnLcd = 0;
            GLCD_Initialize();
        }
    }
    if(GLCD_ReadStatus(1)&DISPLAY_STATUS_NOT_OK)
    {
        if(TimeOnLcd == 1)
        {
            TimeOnLcd = 0;
            GLCD_Initialize();
        }
    }
}

void GLCD_ClearScreen(void)
{
    uint8_t i,j = 0;
    for(i=0;i<LCD128X64_HIGHT/8;i++)
    {
        GLCD_GoTo(i,0);
        for(j=0;j<LCD128X64_WIDTH;j++)
        {
            GLCD_WriteByteData(0x00);
        }
    }
}

void GLCD_InitDataOut(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = LCD128X64_PIN_D0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD128X64_PORT_D0, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LCD128X64_PIN_D1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD128X64_PORT_D1, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LCD128X64_PIN_D2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD128X64_PORT_D2, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LCD128X64_PIN_D3;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD128X64_PORT_D3, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LCD128X64_PIN_D4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD128X64_PORT_D4, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LCD128X64_PIN_D5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD128X64_PORT_D5, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LCD128X64_PIN_D6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD128X64_PORT_D6, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LCD128X64_PIN_D7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD128X64_PORT_D7, &GPIO_InitStruct);
}

void GLCD_InitDataIn(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = LCD128X64_PIN_D0;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(LCD128X64_PORT_D0, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LCD128X64_PIN_D1;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(LCD128X64_PORT_D1, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LCD128X64_PIN_D2;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(LCD128X64_PORT_D2, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LCD128X64_PIN_D3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(LCD128X64_PORT_D3, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LCD128X64_PIN_D4;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(LCD128X64_PORT_D4, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LCD128X64_PIN_D5;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(LCD128X64_PORT_D5, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LCD128X64_PIN_D6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(LCD128X64_PORT_D6, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LCD128X64_PIN_D7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(LCD128X64_PORT_D7, &GPIO_InitStruct);
}

/*
    Display Float Value
    Point: chu so sau dau phay, neu ko co -> integer
*/
void GLCD_Write_Float_Not_Zero(uint16_t Data, uint8_t point, uint8_t Page, uint8_t Column)
{
    uint8_t Length_Char = 0;
    uint8_t aData_Temp[4] = {0};
    uint16_t Temp_Value = 0;

    aData_Temp[0] = Data/1000;
    Temp_Value = Data - aData_Temp[0]*1000;
    aData_Temp[1] = Temp_Value/100;
    Temp_Value = Data - aData_Temp[0]*1000 - aData_Temp[1]*100;
    aData_Temp[2] = Temp_Value/10;
    aData_Temp[3]  = Data%10;

    GLCD_GoTo(Page, Column);
    if((aData_Temp[0] == 0) && (point != 3U))          /* 0 */
    {
        aData_Temp[0] = 0x20;       /* " " */
    }
    else
    {
        aData_Temp[0]+=0x30;
        Length_Char++;
    }
    if ((aData_Temp[1] == 0) && (Length_Char == 0) && (point != 2U))    /* 0 */
    {
        aData_Temp[1] = 0x20;   /* " " */
    }
    else
    {
        aData_Temp[1]+=0x30;
        Length_Char++;
    }
    if((aData_Temp[2] == 0)&&(Length_Char == 0)&&(point != 1U))
    {
        aData_Temp[2] = 0x20;     /* " " */
    }
    else
    {
        aData_Temp[2]+=0x30;
    }
    /* Print */
    GLCD_WriteCharacter(aData_Temp[0], FONT5x7); /* "." */
    if (3U == point)
    {
        GLCD_WriteCharacter(0x2E, FONT5x7); /* "." */
    }
    GLCD_WriteCharacter(aData_Temp[1], FONT5x7);
    if (2U == point)
    {
        GLCD_WriteCharacter(0x2E, FONT5x7); /* "." */
    }
    GLCD_WriteCharacter(aData_Temp[2], FONT5x7);
    if(1U == point)
    {
        GLCD_WriteCharacter(0x2E, FONT5x7); /* "." */
    }
    GLCD_WriteCharacter(aData_Temp[3]+0x30, FONT5x7);
}

/* This function used to display uint32_t */
void GLCD_Display_uint32(uint32_t Data, uint8_t point, uint8_t Page, uint8_t Column)
{
    int8_t index;
    uint32_t tempData;
    uint8_t arrData[11] = {0U};
    tempData = Data;
    for(index = 0U; index < 11U; index++)
    {
        if((tempData != 0U) && (index != point))
        {
            arrData[index] = (uint8_t)((tempData % 10U) + 0x30U);
            tempData = tempData / 10U;
        }
        else if(index == point)
        {
            arrData[index] = 0x2EU; /* . */
        }
        else if((point + 1) >= index)
        {
            arrData[index] = 0x30U;
        }
        else
        {
            arrData[index] = 0x20U;
        }
    }
    GLCD_GoTo(Page, Column);
    /* Display on the screen */
   for(index = 11; index > 0U; index--)
   {
       GLCD_WriteCharacter(arrData[index-1], FONT5x7);
   }
}




void GLCD_Write_Float_Zero(uint16_t Data, uint8_t length, uint8_t point, uint8_t Page, uint8_t Column)
{
    uint8_t aData_Temp[4] = {0};
    int8_t i = 0;
    uint16_t Temp_Value = 0;
    aData_Temp[0] = Data/1000;
    Temp_Value = Data - aData_Temp[0]*1000;
    aData_Temp[1] = Temp_Value/100;
    Temp_Value = Data - aData_Temp[0]*1000 - aData_Temp[1]*100;
    aData_Temp[2] = Temp_Value/10;
    aData_Temp[3]  = Data%10;

    GLCD_GoTo(Page, Column);
    for(i=3; i>= 4-length;i--)
    {
        GLCD_WriteCharacter(aData_Temp[i]+0x30, FONT5x7);
    }
}
/* Display Integer (1 byte hex -> char) Value */
/*
void GLCD_Write_Integer(uint8_t* Data, uint8_t length, uint8_t Page, uint8_t Column)
{
    uint8_t i = 0;
    GLCD_GoTo(Page, Column);
    for(i=0;i<length;i++)
    {
        GLCD_WriteCharacter((*(Data+i)/10)+0x30, FONT5x7);
        GLCD_WriteCharacter((*(Data+i)%10)+0x30, FONT5x7);
    }
}
*/
void GLCD_ClearArea(uint8_t Page1, uint8_t Column1, uint8_t Page2, uint8_t Column2)
{
    uint8_t i,j = 0;
    for(i=Page1;i<=Page2;i++)
    {
        GLCD_GoTo(i,Column1);
        for(j=Column1;j<=Column2;j++)
        {
            GLCD_WriteByteData(0x00);
        }
    }
}
/* ON/OFF 1 diem anh tai vi tri toa do X,Y tren GLCD */
uint8_t Data = 0;
void GMCU_LCD_DrawPixel(uint8_t x, uint8_t y, uint8_t Status)
{
    uint8_t Page = x/8;
    /* Read Data from RAM GLCD */
    Data = GLCD_ReadByteData(Page, y);
    if(Status == 1) /* On */
    {
        Data = Data|(1<<(x%8));
    }
    else /* Off */
    {
        if((Data>>(x%8)&0x01) == 1)
        {
            Data = Data^(1<<(x%8));
        }
    }
    /* Write Data */
    GLCD_GoTo(Page,y);/* ko can */
    GLCD_WriteByteData(Data);
}

/* Draw Rect */
void GMCU_LCD_DrawRect(uint8_t Row1, uint8_t Row2, uint8_t Column1, uint8_t Column2, uint8_t Color)
{
    LCD128X64_DrawLine(Row1,Column1,Row2,Column1,Color);
    LCD128X64_DrawLine(Row1,Column2,Row2,Column2,Color);
    LCD128X64_DrawLine(Row1,Column1,Row1,Column2,Color);
    LCD128X64_DrawLine(Row2,Column1,Row2,Column2,Color);
}

/* Draw Line */
void LCD128X64_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t Color)
{
    int8_t dy,dx;
    signed char addx=1,addy=1;
    signed int P,diff;
    uint8_t i=0;

    if(x1>x2)
    {
        dx=x1-x2;
        addx=-1;
    }
    else dx=x2-x1;
    if(y1>y2)
    {
        dy=y1-y2;
        addy=-1;
    }
    else dy=y2-y1;

    if(dx>=dy)
    {
        dy*=2;
        P=dy-dx;
        diff=P-dx;
        for(;i<=dx;++i)
        {
            GMCU_LCD_DrawPixel(x1,y1,Color);
            if(P < 0)
            {
                P+=dy;
                x1+=addx;
            }
            else
            {
                P+=diff;
                x1+=addx;
                y1+=addy;
            }
        }
    }
    else
    {
        dx*=2;
        P=dx-dy;
        diff=P-dy;
        for(;i<=dy;++i)
        {
            GMCU_LCD_DrawPixel(x1,y1,Color);
            if(P<0)
            {
                P+=dx;
                y1+=addy;
            }
            else
            {
                P+=diff;
                x1+=addx;
                y1+=addy;
            }
        }
    }
}

/* Doc 1 byte (8bit) data tu RAM cua GLCD */
/* cs Select IC KS0108 */
uint8_t GLCD_ReadByteData(uint8_t Page, uint8_t Column)
{
    uint8_t data=0;
    /* Nhayden vi tri x,y */
    GLCD_GoTo(Page,Column);
    /* Set cac chan Data la Input, doc data tai vi tri x,y */
    GLCD_InitDataIn();
    /* Tac dong vao cac chan dk de doc trang thai GLCD */
    GPIO_SetBits(LCD128X64_PORT_RW, LCD128X64_PIN_RW);  /* RW = 1 */
    GPIO_SetBits(LCD128X64_PORT_RS, LCD128X64_PIN_RS);  /* Rs = 1 */
    GLCD_EnableController(Column/LCD128X64_HIGHT);      /* Lua chon chip de tac dong // CS1,2,3 */
    GPIO_SetBits(LCD128X64_PORT_EN, LCD128X64_PIN_EN);  /* GLCD bat dau hoat dong */
    GLCD_Delay();
    /* Doc trang thai tu chan D0-D7 */
    data = GLCD_Read_DataPin();     /* Doc du lieu tu LCD128X64 ra ngoai */
    GPIO_ResetBits(LCD128X64_PORT_EN, LCD128X64_PIN_EN);    /* GLCD bat dau hoat dong */
    GLCD_DisableController(Column/LCD128X64_HIGHT);

    /* Nhay den vi tri x,y */
    GLCD_GoTo(Page,Column);
    /* Set cac chan Data la Input, doc data tai vi tri x,y */
    GLCD_InitDataIn();
    /* Tac dong vao cac chan dk de doc trang thai GLCD */
    GPIO_SetBits(LCD128X64_PORT_RW, LCD128X64_PIN_RW);  /* RW = 1 */
    GPIO_SetBits(LCD128X64_PORT_RS, LCD128X64_PIN_RS);  /* Rs = 1 */
    GLCD_EnableController(Column/LCD128X64_HIGHT);      /* Lua chon chip de tac dong // CS1,2,3 */
    GPIO_SetBits(LCD128X64_PORT_EN, LCD128X64_PIN_EN);  /* GLCD bat dau hoat dong */
    GLCD_Delay();
    /* Doc trang thai tu chan D0-D7 */
    data = GLCD_Read_DataPin(); /* Doc du lieu tu LCD128X64 ra ngoai */
    GPIO_ResetBits(LCD128X64_PORT_EN, LCD128X64_PIN_EN);    /* GLCD bat dau hoat dong */
    GLCD_DisableController(Column/LCD128X64_HIGHT);
    return data;    /* Tra ve gia tri du lieu doc duocss */
}

uint8_t GLCD_Read_DataPin(void)
{
    uint8_t Data = 0;
    Data =  (HAL_GPIO_ReadPin(LCD128X64_PORT_D7,LCD128X64_PIN_D7)<<7)| \
            (HAL_GPIO_ReadPin(LCD128X64_PORT_D6,LCD128X64_PIN_D6)<<6)| \
            (HAL_GPIO_ReadPin(LCD128X64_PORT_D5,LCD128X64_PIN_D5)<<5)| \
            (HAL_GPIO_ReadPin(LCD128X64_PORT_D4,LCD128X64_PIN_D4)<<4)| \
            (HAL_GPIO_ReadPin(LCD128X64_PORT_D3,LCD128X64_PIN_D3)<<3)| \
            (HAL_GPIO_ReadPin(LCD128X64_PORT_D2,LCD128X64_PIN_D2)<<2)| \
            (HAL_GPIO_ReadPin(LCD128X64_PORT_D1,LCD128X64_PIN_D1)<<1)| \
            (HAL_GPIO_ReadPin(LCD128X64_PORT_D0,LCD128X64_PIN_D0)<<0);
    return Data;
}

void GLCD_Write_Integer_Zero(uint8_t Data, uint8_t Page, uint8_t Column)
{
    uint8_t aData_Temp[2] = {0};
    aData_Temp[0] = Data/10;
    aData_Temp[1]  = Data%10;
    GLCD_GoTo(Page, Column);
    GLCD_WriteCharacter(aData_Temp[1]+0x30, FONT5x7);
    GLCD_WriteCharacter(aData_Temp[0]+0x30, FONT5x7);
}

void GLCD_Write_Integer_Not_Zero(uint8_t Data, uint8_t Page, uint8_t Column)
{
    uint8_t aData_Temp[2] = {0};
    aData_Temp[0] = Data/10;
    aData_Temp[1]  = Data%10;
    if(aData_Temp[0] == 0)
    {
        GLCD_GoTo(Page, Column);
        GLCD_WriteCharacter(aData_Temp[1]+0x30, FONT5x7);
        /*
        GLCD_GoTo(Page, Column+12);
        GLCD_WriteCharacter(0x20, FONT5x7); // hien space
        GLCD_WriteCharacter(aData_Temp[1]+0x30, FONT5x7);
        */
    }
    else
    {
      GLCD_GoTo(Page, Column);
      GLCD_WriteCharacter(aData_Temp[0]+0x30, FONT5x7);
      GLCD_WriteCharacter(aData_Temp[1]+0x30, FONT5x7);
    }
}
