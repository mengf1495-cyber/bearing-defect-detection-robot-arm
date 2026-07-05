#ifndef BSP_RGB_LCD_H
#define BSP_RGB_LCD_H
#include "driver_public.h"
#include "bsp_lcd_font.h"
#include "gd_log.h"


#define LCD_WIDTH 800
#define LCD_HEIGHT 480
#define LCD_FB_BYTE_PER_PIXEL 1


#define HORIZONTAL_SYNCHRONOUS_PULSE 10
#define HORIZONTAL_BACK_PORCH 150
#define ACTIVE_WIDTH LCD_WIDTH
#define HORIZONTAL_FRONT_PORCH 15

#define VERTICAL_SYNCHRONOUS_PULSE 10
#define VERTICAL_BACK_PORCH 140
#define ACTIVE_HEIGHT LCD_HEIGHT
#define VERTICAL_FRONT_PORCH 40


/* LCD�����ṹ�� */
typedef struct __typde_rgb_lcd_parameter_struct
{
	uint16_t width;     /* LCD ���� */
	uint16_t height;    /* LCD �߶� */
	uint8_t  dir;       /* ���������������ƣ�0��������1������ */

	/* LCD�Ļ�����ɫ�ͱ���ɫ */
	__IO uint32_t g_point_color;    /* ������ɫ */
	__IO uint32_t g_back_color;    /* ����ɫ */

	uint16_t* display_ram0;    /* �Դ��ַ */
	uint16_t* display_ram1;    /* �Դ��ַ */
}typde_rgb_lcd_parameter_struct;


/* LCD prinft�����ṹ�� */
typedef struct __typde_rgb_lcd_printf_parameter_struct
{
	uint16_t x_start;
	uint16_t y_start;
	uint16_t x_end;
	uint16_t  y_end;
	uint16_t x_now;
	uint16_t y_now;

	FONT_ASCII size;

	/* LCD�Ļ�����ɫ�ͱ���ɫ */
	__IO uint32_t point_color;    /* ������ɫ */
	__IO uint32_t back_color;    /* ����ɫ */
}typde_rgb_lcd_printf_parameter_struct;
/******************************************************************************************/
/* LCD���� */
extern typde_rgb_lcd_parameter_struct bsp_rgb_lcd_parameter; /* ����LCD��Ҫ���� */

extern typde_rgb_lcd_printf_parameter_struct bsp_rgb_lcd_pritnf_parameter;

/* ���û�����ɫ */
#define WHITE           0xFFFF      /* ��ɫ */
#define BLACK           0x0000      /* ��ɫ */
#define RED             0xF800      /* ��ɫ */
#define GREEN           0x07E0      /* ��ɫ */
#define BLUE            0x001F      /* ��ɫ */ 
#define MAGENTA         0XF81F      /* Ʒ��ɫ/�Ϻ�ɫ = BLUE + RED */
#define YELLOW          0XFFE0      /* ��ɫ = GREEN + RED */
#define CYAN            0X07FF      /* ��ɫ = GREEN + BLUE */  

/* �ǳ�����ɫ */
#define BROWN           0XBC40      /* ��ɫ */
#define BRRED           0XFC07      /* �غ�ɫ */
#define GRAY            0X8430      /* ��ɫ */ 
#define DARKBLUE        0X01CF      /* ����ɫ */
#define LIGHTBLUE       0X7D7C      /* ǳ��ɫ */ 
#define GRAYBLUE        0X5458      /* ����ɫ */ 
#define LIGHTGREEN      0X841F      /* ǳ��ɫ */  
#define LGRAY           0XC618      /* ǳ��ɫ(PANNEL),���屳��ɫ */ 
#define LGRAYBLUE       0XA651      /* ǳ����ɫ(�м����ɫ) */ 
#define LBBLUE          0X2B12      /* ǳ����ɫ(ѡ����Ŀ�ķ�ɫ) */ 

/******************************************************************************************/


/******************************************************************************************/
/* �������� */

void bsp_rgb_lcd_backlight_off(void);
void bsp_rgb_lcd_backlight_on(void);
void bsp_rgb_lcd_backlight_duty_set(uint8_t backlight_value);


void bsp_rgb_lcd_wr_data(volatile uint16_t data);            /* LCDд���� */
void bsp_rgb_lcd_wr_regno(volatile uint16_t regno);          /* LCDд�Ĵ������/��ַ */
void bsp_rgb_lcd_write_reg(uint16_t regno , uint16_t data);   /* LCDд�Ĵ�����ֵ */


uint32_t bsp_rgb_lcd_init(void);                        /* ��ʼ��LCD */
void bsp_rgb_lcd_display_on(void);                  /* ����ʾ */
void bsp_rgb_lcd_display_off(void);                 /* ����ʾ */
void bsp_rgb_lcd_scan_dir(uint8_t dir);             /* ������ɨ�跽�� */
void bsp_rgb_lcd_display_dir(uint8_t dir);          /* ������Ļ��ʾ���� */
void bsp_rgb_lcd_ssd_backlight_set(uint8_t pwm);    /* SSD1963 ������� */

void bsp_rgb_lcd_set_cursor(uint16_t x , uint16_t y);    /* ���ù�� */
uint32_t bsp_rgb_lcd_read_point(uint16_t x , uint16_t y);/* ����(32λ��ɫ,����LTDC)  */
void bsp_rgb_lcd_draw_point(uint16_t x , uint16_t y , uint32_t color);/* ����(32λ��ɫ,����LTDC) */

void bsp_rgb_lcd_clear(uint16_t color);     /* LCD���� */
void bsp_rgb_lcd_fill_circle(uint16_t x , uint16_t y , uint16_t r , uint16_t color);                   /* ���ʵ��Բ */
void bsp_rgb_lcd_draw_circle(uint16_t x0 , uint16_t y0 , uint8_t r , uint16_t color);                  /* ��Բ */
void bsp_rgb_lcd_draw_hline(uint16_t x , uint16_t y , uint16_t len , uint16_t color);                  /* ��ˮƽ�� */
void bsp_rgb_lcd_set_window(uint16_t sx , uint16_t sy , uint16_t width , uint16_t height);             /* ���ô��� */
void bsp_rgb_lcd_fill(uint16_t sx , uint16_t sy , uint16_t ex , uint16_t ey , uint32_t color);          /* ��ɫ������(32λ��ɫ,����LTDC) */
void bsp_rgb_lcd_color_fill(uint16_t sx , uint16_t sy , uint16_t ex , uint16_t ey , uint16_t* color);   /* ��ɫ������ */
void bsp_rgb_lcd_draw_line(uint16_t x1 , uint16_t y1 , uint16_t x2 , uint16_t y2 , uint16_t color);     /* ��ֱ�� */
void bsp_rgb_lcd_draw_rectangle(uint16_t x1 , uint16_t y1 , uint16_t x2 , uint16_t y2 , uint16_t color);/* ������ */


void bsp_rgb_lcd_show_char(uint16_t x , uint16_t y , char chr , FONT_ASCII size , uint8_t mode , uint16_t color , uint16_t back_color);
void bsp_rgb_lcd_show_num(uint16_t x , uint16_t y , uint32_t num , uint8_t len , FONT_ASCII size , uint16_t color);
void bsp_rgb_lcd_show_xnum(uint16_t x , uint16_t y , uint32_t num , uint8_t len , FONT_ASCII size , uint8_t mode , uint16_t color);
void bsp_rgb_lcd_show_string(uint16_t x , uint16_t y , uint16_t width , uint16_t height , FONT_ASCII size , char* p , uint16_t color);


void bsp_rgb_lcd_printf_init(uint16_t x_start , uint16_t y_start , uint16_t x_end , uint16_t y_end , FONT_ASCII char_size , uint16_t back_color , uint16_t point_color);
void bsp_rgb_lcd_printf(const char* sFormat , ...);
void bsp_rgb_lcd_show_log(void);

void bsp_rgb_lcd_sram_mode_updata(void);


#endif /* BSP_OLED_H */
