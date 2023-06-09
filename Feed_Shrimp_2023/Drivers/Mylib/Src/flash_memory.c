#include "flash_memory.h"

//uint32_t startpage = FLASH_USER_START_ADDR_TIME;

/*
	@brief  Ghi du lieu vao Flash (6 Byte)
	@param  StartPage page flash bat dau viet
	@param  endPage page flash ket thuc viet
	@param  data1, data2, data3, data4, data5 du lieu can ghi vao Flash
	@retval None
*/
void FLASH_WritePage(uint32_t startPage, uint32_t endPage,uint32_t check, uint32_t data1, uint32_t data2, uint32_t data3,
	                   uint32_t data4, uint32_t data5)
{
  HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef EraseInit;
	EraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInit.PageAddress = startPage;
	EraseInit.NbPages = (endPage - startPage)/FLASH_PAGE_SIZE;
	uint32_t PageError = 0;
	HAL_FLASHEx_Erase(&EraseInit, &PageError);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startPage , check);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startPage + 4, data1); //4 byte dau tien
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startPage + 8, data2); // 4byte tiep theo
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startPage + 12, data3); // 4byte tiep theo
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startPage + 16, data4); // 4byte tiep theo
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startPage + 20, data5); // 4byte tiep theo
  HAL_FLASH_Lock();
}

/*
	@brief  Ghi du lieu vao Flash (5 Byte)
	@param  StartPage page flash bat dau viet
	@param  endPage page flash ket thuc viet
	@param  data1, data2, data3, data4, data5 du lieu can ghi vao Flash
	@retval None
*/
void FLASH_Write_Program(uint32_t startPage, uint32_t endPage,uint16_t check, uint32_t data1, uint32_t data2, uint32_t data3)
{
  HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef EraseInit;
	EraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInit.PageAddress = startPage;
	EraseInit.NbPages = (endPage - startPage)/FLASH_PAGE_SIZE;
	uint32_t PageError = 0;
	HAL_FLASHEx_Erase(&EraseInit, &PageError);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startPage , check);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startPage + 4, data1); //4 byte dau tien
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startPage + 8, data2); // 4byte tiep theo
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startPage + 12, data3); // 4byte tiep theo
  HAL_FLASH_Lock();
}

/*
	@brief  Doc mot byte tu Flash
	@param  addr dia chi byte can doc
*/
uint32_t FLASH_ReadData32(uint32_t addr)
{
	uint32_t data = *(__IO uint32_t *)(addr);
	return data;
}


