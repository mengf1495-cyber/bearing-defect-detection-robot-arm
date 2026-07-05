#ifndef _Y_FLASH_H_
#define _Y_FLASH_H_

#include <stdint.h>
#include "robot_arm_flash.h"

/* ?????????????? */
/* ?? Flash ???????? */

/* ??????? */
#define w25x_init() \
    spi_flash_init()

#define w25x_read(buffer, address, length) \
    spi_flash_read( \
        (uint8_t *)(buffer), \
        (uint32_t)(address), \
        (uint16_t)(length) \
    )

#define w25x_write(buffer, address, length) \
    spi_flash_write_no_check( \
        (uint8_t *)(buffer), \
        (uint32_t)(address), \
        (uint16_t)(length) \
    )

#define w25x_writeS(buffer, address, length) \
    spi_flash_write_no_check( \
        (uint8_t *)(buffer), \
        (uint32_t)(address), \
        (uint16_t)(length) \
    )

#define w25x_erase_sector(sector) \
    spi_flash_erase_sector((uint32_t)(sector))

#endif