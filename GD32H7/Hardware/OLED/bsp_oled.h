#ifndef BSP_OLED_H
#define BSP_OLED_H

#include "driver_public.h"

#define OLED_I2C_ADDR       0x3C

#define OLED_WIDTH          128
#define OLED_HEIGHT         64
#define OLED_PAGES          (OLED_HEIGHT / 8)

#define OLED_FONT_W          8
#define OLED_FONT_H         16
#define OLED_COLS            (OLED_WIDTH / OLED_FONT_W)
#define OLED_ROWS            (OLED_HEIGHT / OLED_FONT_H)

extern uint8_t oled_framebuf[OLED_PAGES][OLED_WIDTH];

void bsp_oled_init(void);
void bsp_oled_clear(void);
void bsp_oled_refresh(void);
void bsp_oled_draw_pixel(uint8_t x, uint8_t y, uint8_t color);
void bsp_oled_draw_char(uint8_t x, uint8_t y, char ch, uint8_t color);
void bsp_oled_draw_string(uint8_t x, uint8_t y, const char *str, uint8_t color);
void bsp_oled_set_cursor(uint8_t row, uint8_t col);
void bsp_oled_printf(uint8_t row, const char *fmt, ...);

#endif
