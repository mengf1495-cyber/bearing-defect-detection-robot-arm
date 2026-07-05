#include "bsp_oled.h"
#include "bsp_software_i2c.h"
#include "bsp_lcd_font.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

SOFT_I2C_SCL_DEF(OLED_IIC, A, 7);
SOFT_I2C_SDA_DEF(OLED_IIC, A, 9);
SOFT_I2C_DEF(OLED_IIC, 400000);

uint8_t oled_framebuf[OLED_PAGES][OLED_WIDTH];

static uint8_t reverse_byte(uint8_t b)
{
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

static void oled_write_cmd(uint8_t cmd)
{
    bsp_software_i2c_start(&OLED_IIC);
    bsp_software_i2c_send_byte(&OLED_IIC, OLED_I2C_ADDR << 1);
    bsp_software_i2c_wait_ack(&OLED_IIC);
	
    bsp_software_i2c_send_byte(&OLED_IIC, 0x00);
    bsp_software_i2c_wait_ack(&OLED_IIC);
    bsp_software_i2c_send_byte(&OLED_IIC, cmd);
    bsp_software_i2c_wait_ack(&OLED_IIC);
    bsp_software_i2c_stop(&OLED_IIC);
}

void bsp_oled_init(void)
{
    bsp_software_i2c_init(&OLED_IIC);

    oled_write_cmd(0xAE);
    oled_write_cmd(0xD5); oled_write_cmd(0x80);
    oled_write_cmd(0xA8); oled_write_cmd(0x3F);
    oled_write_cmd(0xD3); oled_write_cmd(0x00);
    oled_write_cmd(0x40);
    oled_write_cmd(0x8D); oled_write_cmd(0x14);
    oled_write_cmd(0x20); oled_write_cmd(0x00);
    oled_write_cmd(0xA1);
    oled_write_cmd(0xC8);
    oled_write_cmd(0xDA); oled_write_cmd(0x12);
    oled_write_cmd(0x81); oled_write_cmd(0xCF);
    oled_write_cmd(0xD9); oled_write_cmd(0xF1);
    oled_write_cmd(0xDB); oled_write_cmd(0x40);
    oled_write_cmd(0xA4);
    oled_write_cmd(0xA6);
    oled_write_cmd(0xAF);

    bsp_oled_clear();
    bsp_oled_refresh();
}

void bsp_oled_clear(void)
{
    memset(oled_framebuf, 0x00, sizeof(oled_framebuf));
}

void bsp_oled_refresh(void)
{
    uint8_t i, j;

    for (i = 0; i < OLED_PAGES; i++) {
        oled_write_cmd(0xB0 + i);
        oled_write_cmd(0x00);
        oled_write_cmd(0x10);

        bsp_software_i2c_start(&OLED_IIC);
        bsp_software_i2c_send_byte(&OLED_IIC, OLED_I2C_ADDR << 1);
        bsp_software_i2c_wait_ack(&OLED_IIC);
        bsp_software_i2c_send_byte(&OLED_IIC, 0x40);
        bsp_software_i2c_wait_ack(&OLED_IIC);

        for (j = 0; j < OLED_WIDTH; j++) {
            bsp_software_i2c_send_byte(&OLED_IIC, oled_framebuf[i][j]);
            bsp_software_i2c_wait_ack(&OLED_IIC);
        }

        bsp_software_i2c_stop(&OLED_IIC);
    }
}

void bsp_oled_draw_pixel(uint8_t x, uint8_t y, uint8_t color)
{
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT)
        return;

    if (color)
        oled_framebuf[y / 8][x] |= (1 << (y % 8));
    else
        oled_framebuf[y / 8][x] &= ~(1 << (y % 8));
}

void bsp_oled_draw_char(uint8_t x, uint8_t y, char ch, uint8_t color)
{
    uint8_t col, byte_idx;
    uint8_t page_start = y / 8;
    uint8_t bit_shift = y % 8;

    if (ch < ' ' || ch > '~')
        ch = ' ';

    const uint8_t *font = ascii_16_8[ch - ' '];

    if (bit_shift == 0 && color) {
        /* Aligned to page boundary, fast path: direct byte copy with bit-reverse */
        for (col = 0; col < 8; col++) {
            byte_idx = col * 2;
            oled_framebuf[page_start][x + col]     = reverse_byte(font[byte_idx]);
            oled_framebuf[page_start + 1][x + col] = reverse_byte(font[byte_idx + 1]);
        }
    } else {
        /* Slow path: pixel-by-pixel when not aligned or inverting */
        uint8_t row;
        for (col = 0; col < 8; col++) {
            byte_idx = col * 2;
            for (row = 0; row < 8; row++) {
                uint8_t bit = (font[byte_idx] >> (7 - row)) & 1;
                uint8_t px = (color) ? bit : !bit;
                bsp_oled_draw_pixel(x + col, y + row, px);
            }
            for (row = 0; row < 8; row++) {
                uint8_t bit = (font[byte_idx + 1] >> (7 - row)) & 1;
                uint8_t px = (color) ? bit : !bit;
                bsp_oled_draw_pixel(x + col, y + 8 + row, px);
            }
        }
    }
}

void bsp_oled_draw_string(uint8_t x, uint8_t y, const char *str, uint8_t color)
{
    while (*str) {
        bsp_oled_draw_char(x, y, *str, color);
        x += OLED_FONT_W;
        if (x + OLED_FONT_W > OLED_WIDTH)
            break;
        str++;
    }
}

void bsp_oled_set_cursor(uint8_t row, uint8_t col)
{
    (void)row;
    (void)col;
}

void bsp_oled_printf(uint8_t row, const char *fmt, ...)
{
    char buf[32];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    bsp_oled_draw_string(0, row * OLED_FONT_H, buf, 1);
}
