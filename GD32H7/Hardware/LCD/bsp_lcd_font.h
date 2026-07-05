#ifndef __BSP_LCD_FONT_H
#define __BSP_LCD_FONT_H

/* 字体大小枚举 */
typedef enum {FONT_ASCII_12_6 = 12,FONT_ASCII_16_8=16,FONT_ASCII_24_12=24,FONT_ASCII_32_16=32} FONT_ASCII;

/* 12*6 ASCII字符集点阵 */
extern const unsigned char ascii_12_6[95][12];

/* 16*8 ASCII字符集点阵 */
extern const unsigned char ascii_16_8[95][16];;

/* 24*12 ASICII字符集点阵 */
extern const unsigned char ascii_24_12[95][36];

/* 32*16 ASCII字符集点阵 */
extern const unsigned char ascii_32_16[95][64];



#endif





















