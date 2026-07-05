#ifndef DRIVER_EXMC_H
#define DRIVER_EXMC_H
#include "driver_public.h"

#define SDRAM_DEVICE0_ADDR                         ((uint32_t)0xC0000000U)
#define SDRAM_DEVICE1_ADDR                         ((uint32_t)0xD0000000U)

#define EXMC_BANK0_NORSRAM_REGIONx_ADDR(NEx)       ((uint32_t)0x60000000+64*1024*1024*NEx)

#define EXMC_BANK0_NORSRAM_REGION0_ADDR            EXMC_BANK0_NORSRAM_REGIONx_ADDR(0)
#define EXMC_BANK0_NORSRAM_REGION1_ADDR            EXMC_BANK0_NORSRAM_REGIONx_ADDR(1)
#define EXMC_BANK0_NORSRAM_REGION2_ADDR            EXMC_BANK0_NORSRAM_REGIONx_ADDR(2)
#define EXMC_BANK0_NORSRAM_REGION3_ADDR            EXMC_BANK0_NORSRAM_REGIONx_ADDR(3)

void driver_exmc_norsram_init(uint32_t norsram_region);
void driver_exmc_lcd_init(uint32_t norsram_region);
Drv_Err driver_exmc_sdram_init(uint32_t sdram_device);

    
void sdram_writebuffer_8(uint32_t sdram_device, uint8_t *pbuffer, uint32_t write_addr, uint32_t byte_count_to_write);
void sdram_readbuffer_8(uint32_t sdram_device, uint8_t *pbuffer, uint32_t read_addr, uint32_t byte_count_to_read);
void fill_buffer(uint8_t *pbuffer, uint16_t buffer_lengh, uint16_t start_value);


#endif /* GPIO_H*/
