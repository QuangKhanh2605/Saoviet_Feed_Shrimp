//#include "stm32f4xx_hal.h"
#include "user_util.h"
#include "main.h"
#ifndef __GLCD192x64H__
#define __GLCD192x64H__
    /* Define Size */
#define LCD128X64_WIDTH   128
#define LCD128X64_HIGHT    64
    /* Define Command */
#define DISPLAY_SET_Y           0x40                /* Set colum 0->63 */
#define DISPLAY_SET_X           0xB8                /* Set row 0->7 */
#define DISPLAY_START_LINE      0xC0                /* Display start line or page scroll */
#define DISPLAY_ON_CMD          0x3E                /* ON/OFF display LCD (last bit 1: ON, 0: OFF) */
#define ON                      0x01
#define OFF                     0x00
#define DISPLAY_STATUS_NOT_OK   0x30
#define DISPLAY_STATUS_BUSY     0x80                /* Read Status Ks0108 (MSB : 1 - busy,  0 - None) */

    /* Define PORT, PIN Data GLCD */
#define LCD128X64_PORT_D0       MCU_LCD_D0_GPIO_Port
#define LCD128X64_PORT_D1       MCU_LCD_D1_GPIO_Port
#define LCD128X64_PORT_D2       MCU_LCD_D2_GPIO_Port
#define LCD128X64_PORT_D3       MCU_LCD_D3_GPIO_Port
#define LCD128X64_PORT_D4       MCU_LCD_D4_GPIO_Port
#define LCD128X64_PORT_D5       MCU_LCD_D5_GPIO_Port
#define LCD128X64_PORT_D6       MCU_LCD_D6_GPIO_Port
#define LCD128X64_PORT_D7       MCU_LCD_D7_GPIO_Port

#define LCD128X64_PIN_D0        MCU_LCD_D0_Pin 
#define LCD128X64_PIN_D1        MCU_LCD_D1_Pin
#define LCD128X64_PIN_D2        MCU_LCD_D2_Pin
#define LCD128X64_PIN_D3        MCU_LCD_D3_Pin
#define LCD128X64_PIN_D4        MCU_LCD_D4_Pin
#define LCD128X64_PIN_D5        MCU_LCD_D5_Pin
#define LCD128X64_PIN_D6        MCU_LCD_D6_Pin
#define LCD128X64_PIN_D7        MCU_LCD_D7_Pin

    /* Define PORT, PIN Control GLCD */
#define LCD128X64_PORT_RS      MCU_LCD_RS_GPIO_Port 
#define LCD128X64_PORT_RW      MCU_LCD_RW_GPIO_Port
#define LCD128X64_PORT_EN      MCU_LCD_EN_GPIO_Port
#define LCD128X64_PORT_CS1     MCU_LCD_PSB_GPIO_Port    /* LCD_CS1_GPIO_Port */
#define LCD128X64_PORT_CS2     MCU_LCD_RST_GPIO_Port    /* LCD_CS2_GPIO_Port */
//#define LCD128X64_PORT_CS3     LCD_CS3_GPIO_Port
#define LCD128X64_PORT_RST     MCU_LCD_RST_GPIO_Port

#define LCD128X64_PIN_RS       MCU_LCD_RS_Pin
#define LCD128X64_PIN_RW       MCU_LCD_RW_Pin
#define LCD128X64_PIN_EN       MCU_LCD_EN_Pin
#define LCD128X64_PIN_CS1      MCU_LCD_PSB_Pin          /* LCD_CS1_Pin */
#define LCD128X64_PIN_CS2      MCU_LCD_RST_Pin          /* LCD_CS2_Pin */
//#define LCD128X64_PIN_CS3      LCD_CS3_Pin

#define LCD128X64_PIN_RST      MCU_LCD_RST_Pin


#define FONT5x7                 1
#define FONT_BATTERY            2
#define FONT16x16               2
#define FONT_HIGHT              8
#define FONT_WIDTH              6

/* --------------------------- Define function ------------------------------------ */
/* Init GLCD */
void GLCD_Initialize(void);
    /* Write byte to LCD */
void GLCD_WriteByteData(uint8_t dataToWrite);
    /* Break point to row and column */
void GLCD_GoTo(uint8_t Page, uint8_t column);
    /* Ghi mot ki tu len GLCD */
void GLCD_WriteCharacter(uint8_t Char, uint8_t Font_Size);
    /* Ghi mot chuoi ki tu len GLCD */
void GLCD_WriteString(char* String, uint8_t Page, uint8_t Column, uint8_t Font_Size);
    /* Xoa man hinh GLCD */
void GLCD_ClearScreen(void);
    /* Gui lenh toi GLCD */
void GLCD_WriteCommand(uint8_t commandToWrite, uint8_t controller);
    /* Doc trang thai GLCD (trang thai cua chip KS tuong ung) */
uint8_t GLCD_ReadStatus(uint8_t controller);
    /* On bit */
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
    /* Clear bit */
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
    /* Delay GLCD write or Read */
void GLCD_Delay(void);
    /* Select driver chip CS1/2/3 */
void GLCD_EnableController(uint8_t controller);
    /* Disable chip GLCD */
void GLCD_DisableController(uint8_t controller);
    /* Out Level Voltage D0->D7 */
void GLCD_OutData_Cmd(uint8_t Data);
    /* Out Level Voltage D7->D0 */
void GLCD_OutData(uint8_t Data);
    /* Select Pin Data to Output */
void GLCD_InitDataOut(void);
    /* Select Pin data to Input */
void GLCD_InitDataIn(void);
    /* Clear area of GLCD */
void GLCD_ClearArea(uint8_t Page1, uint8_t Column1, uint8_t Page2, uint8_t Column2);
    /* Draw Rect */
void GLCD_DrawRect(uint8_t Row1, uint8_t Row2, uint8_t Column1, uint8_t Column2, uint8_t Color);
    /* Draw Line */
void LCD128X64_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t Color);
    /* On/off pixel */
void GLCD_DrawPixel(uint8_t x, uint8_t y, uint8_t Status);

uint8_t GLCD_Read_DataPin(void);
    /* Doc du lieu tren cac chan data */
uint8_t GLCD_ReadByteData(uint8_t Page, uint8_t Column);

//void GLCD_Write_Integer(uint16_t Data, uint8_t Page, uint8_t Column);
void GLCD_Write_Float_Zero(uint16_t Data, uint8_t length, uint8_t point, uint8_t Page, uint8_t Column);
void GLCD_Write_Float_Not_Zero(uint16_t Data, uint8_t point, uint8_t Page, uint8_t Column);
void GLCD_Display_uint32(uint32_t Data, uint8_t point, uint8_t Page, uint8_t Column);
void GLCD_Write_Integer_Zero(uint8_t Data, uint8_t Page, uint8_t Column);
void GLCD_Write_Integer_Not_Zero(uint8_t Data, uint8_t Page, uint8_t Column);

#endif
