#ifndef __ROBOT_ARM_FLASH_H
#define __ROBOT_ARM_FLASH_H

#include <stdint.h>
#include "robot_arm_board.h"

void spi_flash_init(void);

uint16_t spi_flash_read_id(void);

uint8_t spi_write_read(uint8_t data);

void spi_flash_read(
    uint8_t *buffer,
    uint32_t address,
    uint16_t length
);

void spi_flash_write_no_check(
    uint8_t *buffer,
    uint32_t address,
    uint16_t length
);

void spi_flash_erase_sector(uint32_t sector);

void spi_flash_wait_busy(void);

#endif