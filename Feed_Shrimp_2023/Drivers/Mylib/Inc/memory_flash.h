#ifndef __MEMORY_FLASH_
#define __MEMORY_FLASH_

#include "stm32l1xx_hal.h"

#define FLASH_ADDR_PAGE_126 ((uint32_t)0x0801F810)	//Page 126
#define FLASH_ADDR_PAGE_127 ((uint32_t)0x0801FC00)	//Page 127

#define FLASH_USER_START_ADDR   FLASH_ADDR_PAGE_126
#define FLASH_USER_END_ADDR     FLASH_ADDR_PAGE_127 + FLASH_PAGE_SIZE

void FLASH_WritePage(uint32_t startPage, uint32_t endPage,uint32_t check, uint32_t data1,uint32_t data2,uint32_t data3);

#endif
