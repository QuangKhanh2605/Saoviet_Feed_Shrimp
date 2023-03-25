#ifndef __MEMORY_FLASH_
#define __MEMORY_FLASH_

#include "stm32l1xx_hal.h"

#define FLASH_ADDR_PAGE_126 ((uint32_t)0x0801F810)	//Page 126
#define FLASH_ADDR_PAGE_127 ((uint32_t)0x0801FC00)	//Page 127

#define FLASH_ADDR_PAGE_124 ((uint32_t)0x08000000 + 1024*124)	
#define FLASH_ADDR_PAGE_125 ((uint32_t)0x08000000 + 1024*125)	

#define FLASH_USER_START_ADDR   FLASH_ADDR_PAGE_126
#define FLASH_USER_END_ADDR     FLASH_ADDR_PAGE_127 + FLASH_PAGE_SIZE

#define FLASH_USER_START_ADDR_PROGRAM   FLASH_ADDR_PAGE_124 
#define FLASH_USER_END_ADDR_PROGRAM     FLASH_ADDR_PAGE_125

void FLASH_WritePage(uint32_t startPage, uint32_t endPage,uint32_t check, uint32_t data1, uint32_t data2, uint32_t data3,
	                   uint32_t data4, uint32_t data5);

void FLASH_Write_Program(uint32_t startPage, uint32_t endPage,uint16_t check, uint32_t data1, uint32_t data2, uint32_t data3);

uint32_t FLASH_ReadData32(uint32_t addr);
#endif
