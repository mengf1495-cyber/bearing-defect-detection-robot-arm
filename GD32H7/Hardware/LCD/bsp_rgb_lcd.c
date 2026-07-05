#include "bsp_rgb_lcd.h"
#include "driver_public.h"
#include "driver_gpio.h"
#include "driver_timer.h"
#include "driver_dma.h"
#include <stdarg.h>


//�û����ö�����
#define BACK_LIGHT_DUTY 80

TIMER_CH_DEF(LCD_RGB_BL,TIMER40,1,TIMER_CH_PWM_HIGH,D,11,AF_PP,GPIO_AF_0);

SET_DMA_AREA_SDRAM uint16_t ltdc_lcd_framebuf0[LCD_HEIGHT][LCD_WIDTH];

typde_rgb_lcd_printf_parameter_struct bsp_rgb_lcd_pritnf_parameter;


/* ����LCD��Ҫ���� */
typde_rgb_lcd_parameter_struct bsp_rgb_lcd_parameter;


static __IO uint8_t lcd_backlight_duty=0;

static void bsp_rgb_lcd_tli_gpio_config(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOH);
    rcu_periph_clock_enable(RCU_GPIOG);

    /* configure HSYNC(PE15), VSYNC(PA7), PCLK(PB3) */
    gpio_af_set(GPIOE, GPIO_AF_10, GPIO_PIN_15);
    gpio_af_set(GPIOA, GPIO_AF_14, GPIO_PIN_7);
    gpio_af_set(GPIOB, GPIO_AF_2, GPIO_PIN_3);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_15);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_7);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_3);

    /* configure LCD_R7(PG6), LCD_R6(PH12), LCD_R5(PH11), LCD_R4(PA5), LCD_R3(PH9),LCD_R2(PH8),
                 LCD_R1(PH3), LCD_R0(PH2) */
    gpio_af_set(GPIOC, GPIO_AF_14, GPIO_PIN_4);		//R7
    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_1);		//R6
    gpio_af_set(GPIOA, GPIO_AF_14, GPIO_PIN_9);		//R5
    gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_10);	//R4
    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_0);		//R3

    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_4);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_1);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_9);
    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_10);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_0);


    /* configure  LCD_G7(PD3), LCD_G6(PC7), LCD_G5(PC1), LCD_G4(PH15), LCD_G3(PH14), LCD_G2(PH13),LCD_G1(PB0), LCD_G0(PB1) */
    gpio_af_set(GPIOA, GPIO_AF_14, GPIO_PIN_6);//G2
    gpio_af_set(GPIOG, GPIO_AF_9, GPIO_PIN_10);//G3
    gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_4);//G4
    gpio_af_set(GPIOB, GPIO_AF_14, GPIO_PIN_11);//G5
    gpio_af_set(GPIOC, GPIO_AF_14, GPIO_PIN_7);//G6
    gpio_af_set(GPIOD, GPIO_AF_14, GPIO_PIN_3);//G7

    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_6);
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_10);
    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_4);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_11);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_7);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_3);

    /* configure LCD_B7(PB9), LCD_B6(PB8), LCD_B5(PB5), LCD_B4(PC11), LCD_B3(PG11),LCD_B2(PG10), LCD_B1(PG12), LCD_B0(PG14) */
    gpio_af_set(GPIOA, GPIO_AF_13, GPIO_PIN_8);//B3
    gpio_af_set(GPIOG, GPIO_AF_9, GPIO_PIN_12);//B4
    gpio_af_set(GPIOA, GPIO_AF_14, GPIO_PIN_3);//B5
    gpio_af_set(GPIOA, GPIO_AF_14, GPIO_PIN_15);//B6
    gpio_af_set(GPIOB, GPIO_AF_14, GPIO_PIN_9);//B7


    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_8);
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_12);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_3);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_15);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_9);
		


    /* configure LCD_DE(PF10) */
    gpio_af_set(GPIOE, GPIO_AF_14, GPIO_PIN_13);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_13);
    /* LCD PWM BackLight(PD11) */
    gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_11);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_11);
    gpio_bit_set(GPIOD, GPIO_PIN_11);															
											
}

static void bsp_rgb_lcd_tli_init(void)
{
    tli_parameter_struct tli_init_struct;
    tli_layer_parameter_struct tli_layer_init_struct;

    rcu_periph_clock_enable(RCU_TLI);
    
    bsp_rgb_lcd_tli_gpio_config();    

    rcu_pll_input_output_clock_range_config(IDX_PLL2, RCU_PLL2RNG_1M_2M, RCU_PLL2VCO_150M_420M);
    
    /* configure the PLL2 clock: CK_PLL2P/CK_PLL2Q/CK_PLL2R = HXTAL_VALUE / 25 * 150 / 3 */
    if(ERROR == rcu_pll2_config(25, 240, 3, 3, 2)) {
        while(1) {
        }
    }
    rcu_pll_clock_output_enable(RCU_PLL2R);
    
    rcu_tli_clock_div_config(RCU_PLL2R_DIV4);

    rcu_osci_on(RCU_PLL2_CK);

    if(ERROR == rcu_osci_stab_wait(RCU_PLL2_CK)) {
        while(1) {
        }
    }

    /* configure TLI parameter struct */
    tli_init_struct.signalpolarity_hs = TLI_HSYN_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_vs = TLI_VSYN_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_de = TLI_DE_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;
    /* LCD display timing configuration */
    tli_init_struct.synpsz_hpsz = HORIZONTAL_SYNCHRONOUS_PULSE - 1;
    tli_init_struct.synpsz_vpsz = VERTICAL_SYNCHRONOUS_PULSE - 1;
    tli_init_struct.backpsz_hbpsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1;
    tli_init_struct.backpsz_vbpsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1;
    tli_init_struct.activesz_hasz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH - 1;
    tli_init_struct.activesz_vasz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT - 1;
    tli_init_struct.totalsz_htsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH + HORIZONTAL_FRONT_PORCH - 1;
    tli_init_struct.totalsz_vtsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT + VERTICAL_FRONT_PORCH - 1;
    /* configure LCD background R,G,B values */
    tli_init_struct.backcolor_red = 0xFF;
    tli_init_struct.backcolor_green = 0xFF;
    tli_init_struct.backcolor_blue = 0xFF;
    tli_init(&tli_init_struct);


    /* TLI layer0 configuration */
    /* TLI window size configuration */
    tli_layer_init_struct.layer_window_leftpos = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH;
    tli_layer_init_struct.layer_window_rightpos = (ACTIVE_WIDTH + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1);
    tli_layer_init_struct.layer_window_toppos = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH;
    tli_layer_init_struct.layer_window_bottompos = (ACTIVE_HEIGHT + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1);
    /* TLI window pixel format configuration */
    tli_layer_init_struct.layer_ppf = LAYER_PPF_RGB565;
    /* TLI window specified alpha configuration */
    tli_layer_init_struct.layer_sa = 255;
    /* TLI layer default alpha R,G,B value configuration */
    tli_layer_init_struct.layer_default_blue = 0x00;
    tli_layer_init_struct.layer_default_green = 0x00;
    tli_layer_init_struct.layer_default_red = 0x00;
    tli_layer_init_struct.layer_default_alpha = 0x00;
    /* TLI window blend configuration */
    tli_layer_init_struct.layer_acf1 = LAYER_ACF1_SA;
    tli_layer_init_struct.layer_acf2 = LAYER_ACF2_SA;
    /* TLI layer frame buffer base address configuration */
    tli_layer_init_struct.layer_frame_bufaddr = (uint32_t)ltdc_lcd_framebuf0;
    tli_layer_init_struct.layer_frame_line_length = ((ACTIVE_WIDTH * 2) + 3);
    tli_layer_init_struct.layer_frame_buf_stride_offset = (ACTIVE_WIDTH * 2);
    tli_layer_init_struct.layer_frame_total_line_number = ACTIVE_HEIGHT;
    tli_layer_init(LAYER0, &tli_layer_init_struct);
    tli_dither_config(TLI_DITHER_ENABLE);
}

void bsp_rgb_lcd_backlight_on(void)
{    
    driver_timer_channel_init(&LCD_RGB_BL,10000000,100);//1M/1000 pwm ck=1K
       
    driver_timer_pwm_on(&LCD_RGB_BL);
	
    driver_timer_pwm_on(&LCD_RGB_BL);
    driver_timer_pwm_duty_set(&LCD_RGB_BL,lcd_backlight_duty);    
}

void bsp_rgb_lcd_backlight_off(void)
{   
    driver_timer_pwm_off(&LCD_RGB_BL);    
    driver_timer_pwm_duty_set(&LCD_RGB_BL,0);
}

void bsp_rgb_lcd_backlight_duty_set(uint8_t backlight_value)
{    
    lcd_backlight_duty=backlight_value;
    driver_timer_pwm_duty_set(&LCD_RGB_BL,lcd_backlight_duty);
}


static void bsp_rgb_lcd_port_init(void)
{    
    driver_timer_channel_init(&LCD_RGB_BL,10000000,100);//1M/1000 pwm ck=1K
       
    driver_timer_pwm_on(&LCD_RGB_BL);
    
    driver_dma_mem_init(&DMA_MEM);
    
    bsp_rgb_lcd_tli_init();    
}


/**
 * ˵��       ��ʼ��LCD
 *   @note    
 *
 * ����       ��
 * ����ֵ      ��
 */
uint32_t bsp_rgb_lcd_init(void)
{
    
    bsp_rgb_lcd_parameter.display_ram0 = ltdc_lcd_framebuf0[0];
    
    bsp_rgb_lcd_port_init();
    
    bsp_rgb_lcd_backlight_duty_set(BACK_LIGHT_DUTY);
    
//    bsp_rgb_lcd_backlight_on();          /* �������� */      
    
    delay_ms(10);        
    
    /* LCD�Ļ�����ɫ�ͱ���ɫ */
    bsp_rgb_lcd_parameter.g_point_color = WHITE;    /* ������ɫ */
    bsp_rgb_lcd_parameter.g_back_color  = BLACK;    /* ����ɫ */    
    
    delay_ms(1);     /* ��ʼ��FSMC��,����ȴ�һ��ʱ����ܿ�ʼ��ʼ�� */

	//!�˴�������Ļ�Ĵ�С
    bsp_rgb_lcd_display_dir(0); /* Ĭ��Ϊ���� */  
    
    bsp_rgb_lcd_clear(WHITE);

    tli_layer_enable(LAYER0);
    tli_reload_config(TLI_FRAME_BLANK_RELOAD_EN);
    tli_enable();  
		
    return DRV_SUCCESS;
}


/**
 * ˵��       ��ȡ��ĳ�����ɫֵ
 * ����       x,y:����
 * ����ֵ      �˵����ɫ(32λ��ɫ,�������LTDC)
 */
uint32_t bsp_rgb_lcd_read_point(uint16_t x, uint16_t y)
{
    
    if(x>bsp_rgb_lcd_parameter.width || y>bsp_rgb_lcd_parameter.height)
    {
        return 0;
    }
    
    if(bsp_rgb_lcd_parameter.display_ram0 != NULL)
    {
        return bsp_rgb_lcd_parameter.display_ram0[y*bsp_rgb_lcd_parameter.width+x];
    }        
    return 0; 
}

/**
 * ˵��       LCD������ʾ
 * ����       ��
 * ����ֵ      ��
 */
void bsp_rgb_lcd_display_on(void)
{
//    bsp_rgb_lcd_backlight_on();          /* �������� */      
    tli_enable();
}

/**
 * ˵��       LCD�ر���ʾ
 * ����       ��
 * ����ֵ      ��
 */
void bsp_rgb_lcd_display_off(void)
{
//    bsp_rgb_lcd_backlight_off();          /* �������� */      
    tli_disable();
}


/**
 * ˵��       ����
 * ����       x,y: ����
 * ����       color: �����ɫ(32λ��ɫ,�������LTDC)
 * ����ֵ      ��
 */
void bsp_rgb_lcd_draw_point(uint16_t x, uint16_t y, uint32_t color)
{
//    bsp_rgb_lcd_parameter.display_ram0
    if(x>bsp_rgb_lcd_parameter.width || y>bsp_rgb_lcd_parameter.height)
    {
        return;
    }
    
    if(bsp_rgb_lcd_parameter.display_ram0 != NULL)
    {    

        if(bsp_rgb_lcd_parameter.dir==0)
        {
            *(__IO uint16_t*)(((uint32_t)(bsp_rgb_lcd_parameter.display_ram0)) + 2*((bsp_rgb_lcd_parameter.width*y) + x)) = color; 
        }
        else
        {
            *(__IO uint16_t*)(((uint32_t)(bsp_rgb_lcd_parameter.display_ram0)) + 2*((bsp_rgb_lcd_parameter.height*x) + y)) = color;         
        }
    }        
}


/**
 * ˵��       ����LCD��ʾ����
 * ����       dir:0,����; 1,����
 * ����ֵ      ��
 */
void bsp_rgb_lcd_display_dir(uint8_t dir)
{
    bsp_rgb_lcd_parameter.dir = dir;   /* ����/���� */

    if (dir == 0)      /* ���� */ 
    {
        bsp_rgb_lcd_parameter.width = LCD_WIDTH;
        bsp_rgb_lcd_parameter.height = LCD_HEIGHT;  
    }
    else               /* ���� */ 
    {
        bsp_rgb_lcd_parameter.width = LCD_HEIGHT;
        bsp_rgb_lcd_parameter.height = LCD_WIDTH;
    }
}


/**
 * ˵��       ��������
 * ����       color: Ҫ��������ɫ
 * ����ֵ      ��
 */
void bsp_rgb_lcd_clear(uint16_t color)
{
    uint32_t index = 0;
    uint32_t totalpoint = bsp_rgb_lcd_parameter.width;
    bsp_rgb_lcd_parameter.g_back_color = color;
    
    bsp_rgb_lcd_parameter.g_point_color = ~color;

    totalpoint *= bsp_rgb_lcd_parameter.height;    /* �õ��ܵ��� */
    
    if(bsp_rgb_lcd_parameter.display_ram0 != NULL)
    { 
        for (index = 0; index < totalpoint; index++)
        {
            *(__IO uint16_t*)((uint32_t)(bsp_rgb_lcd_parameter.display_ram0)+2*index)=color;
        }
    }    
}

/**
 * ˵��       ��ָ����������䵥����ɫ
 * ����       (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex - sx + 1) * (ey - sy + 1)
 * ����       color: Ҫ������ɫ(32λ��ɫ,�������LTDC)
 * ����ֵ      ��
 */
void bsp_rgb_lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
{
    uint16_t height, width,x,y;

    if(sx>bsp_rgb_lcd_parameter.width || ex>bsp_rgb_lcd_parameter.width || sy>bsp_rgb_lcd_parameter.height || ey>bsp_rgb_lcd_parameter.height)
    {
        return;
    }
    width = ex - sx + 1;            /* �õ����Ŀ��� */
    height = ey - sy + 1;           /* �߶� */
    
    if(bsp_rgb_lcd_parameter.display_ram0 != NULL)
    { 
        if(bsp_rgb_lcd_parameter.dir==0)
        {
            for(y = sy; y < sy + height; y++){      
                for(x = sx; x < sx + width; x++){ 
                    *(__IO uint16_t*)( (uint32_t)(bsp_rgb_lcd_parameter.display_ram0) + 2*((bsp_rgb_lcd_parameter.width*y) + x) ) = color;
                }                
            }
        }
        else
        {
            for(y = sy; y < sy + height; y++){            
                for(x = sx; x < sx + width; x++){     
                    *(__IO uint16_t*)( (uint32_t)(bsp_rgb_lcd_parameter.display_ram0) + 2*((bsp_rgb_lcd_parameter.height*x) + y) ) = color;
                }                
            }        
        }
    }        
}

/**
 * ˵��       ��ָ�����������ָ����ɫ��
 * ����       (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex - sx + 1) * (ey - sy + 1)
 * ����       color: Ҫ������ɫ�����׵�ַ
 * ����ֵ      ��
 */
void bsp_rgb_lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint16_t height, width,x,y;
    
    if(sx>bsp_rgb_lcd_parameter.width || ex>bsp_rgb_lcd_parameter.width || sy>bsp_rgb_lcd_parameter.height || ey>bsp_rgb_lcd_parameter.height)
    {
        return;
    }    

    width = ex - sx + 1;            /* �õ����Ŀ��� */
    height = ey - sy + 1;           /* �߶� */
    
    if(bsp_rgb_lcd_parameter.display_ram0 != NULL)
    { 
        if(bsp_rgb_lcd_parameter.dir==0)
        {        
            for(y = sy; y < sy + height; y++){      
                for(x = sx; x < sx + width; x++){ 
                    *(__IO uint16_t*)( (uint32_t)(bsp_rgb_lcd_parameter.display_ram0) + 2*((bsp_rgb_lcd_parameter.width*y) + x) ) = *(color++);
                }                
            }
        }        
        else
        {
            for(y = sy; y < sy + height; y++){            
                for(x = sx; x < sx + width; x++){                       
                    *(__IO uint16_t*)( (uint32_t)(bsp_rgb_lcd_parameter.display_ram0) + 2*((bsp_rgb_lcd_parameter.height*x) + y) ) = *(color++);
                }                
            }        
        }        
    }  
}


/**
 * ˵��       ����
 * ����       x1,y1: �������
 * ����       x2,y2: �յ�����
 * ����       color: �ߵ���ɫ
 * ����ֵ      ��
 */
void bsp_rgb_lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;
    delta_x = x2 - x1;          /* ������������ */
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0)incx = 1;   /* ���õ������� */
    else if (delta_x == 0)incx = 0; /* ��ֱ�� */
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0; /* ˮƽ�� */
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y)distance = delta_x;  /* ѡȡ�������������� */
    else distance = delta_y;

    for (t = 0; t <= distance + 1; t++ )   /* ������� */
    {
        bsp_rgb_lcd_draw_point(row, col, color); /* ���� */
        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

/**
 * ˵��       ��ˮƽ��
 * ����       x0,y0: �������
 * ����       len  : �߳���
 * ����       color: ���ε���ɫ
 * ����ֵ      ��
 */
void bsp_rgb_lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color)
{
    if ((len == 0) || (x > bsp_rgb_lcd_parameter.width) || (y > bsp_rgb_lcd_parameter.height))return;

    bsp_rgb_lcd_fill(x, y, x + len - 1, y, color);
}

/**
 * ˵��       ������
 * ����       x1,y1: �������
 * ����       x2,y2: �յ�����
 * ����       color: ���ε���ɫ
 * ����ֵ      ��
 */
void bsp_rgb_lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    bsp_rgb_lcd_draw_line(x1, y1, x2, y1, color);
    bsp_rgb_lcd_draw_line(x1, y1, x1, y2, color);
    bsp_rgb_lcd_draw_line(x1, y2, x2, y2, color);
    bsp_rgb_lcd_draw_line(x2, y1, x2, y2, color);
}

/**
 * ˵��       ��Բ
 * ����       x,y  : Բ��������
 * ����       r    : �뾶
 * ����       color: Բ����ɫ
 * ����ֵ      ��
 */
void bsp_rgb_lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);       /* �ж��¸���λ�õı�־ */

    while (a <= b)
    {
        bsp_rgb_lcd_draw_point(x0 + a, y0 - b, color);  /* 5 */
        bsp_rgb_lcd_draw_point(x0 + b, y0 - a, color);  /* 0 */
        bsp_rgb_lcd_draw_point(x0 + b, y0 + a, color);  /* 4 */
        bsp_rgb_lcd_draw_point(x0 + a, y0 + b, color);  /* 6 */
        bsp_rgb_lcd_draw_point(x0 - a, y0 + b, color);  /* 1 */
        bsp_rgb_lcd_draw_point(x0 - b, y0 + a, color);
        bsp_rgb_lcd_draw_point(x0 - a, y0 - b, color);  /* 2 */
        bsp_rgb_lcd_draw_point(x0 - b, y0 - a, color);  /* 7 */
        a++;

        /* ʹ��Bresenham�㷨��Բ */
        if (di < 0)
        {
            di += 4 * a + 6;
        }
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

/**
 * ˵��       ���ʵ��Բ
 * ����       x0,y0: Բ��������
 * ����       r    : �뾶
 * ����       color: Բ����ɫ
 * ����ֵ      ��
 */
void bsp_rgb_lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
    uint32_t i;
    uint32_t imax = ((uint32_t)r * 707) / 1000 + 1;
    uint32_t sqmax = (uint32_t)r * (uint32_t)r + (uint32_t)r / 2;
    uint32_t xr = r;

    bsp_rgb_lcd_draw_hline(x - r, y, 2 * r, color);

    for (i = 1; i <= imax; i++)
    {
        if ((i * i + xr * xr) > sqmax)
        {
            /* draw lines from outside */
            if (xr > imax)
            {
                bsp_rgb_lcd_draw_hline (x - i + 1, y + xr, 2 * (i - 1), color);
                bsp_rgb_lcd_draw_hline (x - i + 1, y - xr, 2 * (i - 1), color);
            }

            xr--;
        }

        /* draw lines from inside (center) */
        bsp_rgb_lcd_draw_hline(x - xr, y + i, 2 * xr, color);
        bsp_rgb_lcd_draw_hline(x - xr, y - i, 2 * xr, color);
    }
}

/**
 * ˵��       ��ָ��λ����ʾһ���ַ�
 * ����       x,y  : ����
 * ����       chr  : Ҫ��ʾ���ַ�:" "--->"~"
 * ����       size : �����С FONT_ASCII_12_6/FONT_ASCII_16_8/FONT_ASCII_24_12/FONT_ASCII_32_16
 * ����       mode : ���ӷ�ʽ(1); �ǵ��ӷ�ʽ(0);
 * ����ֵ      ��
 */
void bsp_rgb_lcd_show_char(uint16_t x, uint16_t y, char chr, FONT_ASCII size, uint8_t mode, uint16_t color,uint16_t back_color)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = 0;
    uint8_t *pfont = 0;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* �õ�����һ���ַ���Ӧ������ռ���ֽ��� */
    chr = chr - ' ';    /* �õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩ */

    switch (size)
    {
        case FONT_ASCII_12_6:
            pfont = (uint8_t *)ascii_12_6[chr];  /* ����1206���� */
            break;

        case FONT_ASCII_16_8:
            pfont = (uint8_t *)ascii_16_8[chr];  /* ����1608���� */
            break;

        case FONT_ASCII_24_12:
            pfont = (uint8_t *)ascii_24_12[chr];  /* ����2412���� */
            break;

        case FONT_ASCII_32_16:
            pfont = (uint8_t *)ascii_32_16[chr];  /* ����3216���� */
            break;

        default:
            return ;
    }

    for (t = 0; t < csize; t++)
    {
        temp = pfont[t];    /* ��ȡ�ַ��ĵ������� */

        for (t1 = 0; t1 < 8; t1++)   /* һ���ֽ�8���� */
        {
            if (temp & 0x80)        /* ��Ч��,��Ҫ��ʾ */
            {
                bsp_rgb_lcd_draw_point(x, y, color);        /* �������,Ҫ��ʾ����� */
            }
            else if (mode == 0)     /* ��Ч��,����ʾ */
            {
                bsp_rgb_lcd_draw_point(x, y, back_color); /* ������ɫ,�൱������㲻��ʾ(ע�ⱳ��ɫ��ȫ�ֱ�������) */
            }

            temp <<= 1; /* ��λ, �Ա��ȡ��һ��λ��״̬ */
            y++;

            if (y >= bsp_rgb_lcd_parameter.height)return;  /* �������� */

            if ((y - y0) == size)   /* ��ʾ��һ����? */
            {
                y = y0; /* y���긴λ */
                x++;    /* x������� */

                if (x >= bsp_rgb_lcd_parameter.width)return;   /* x���곬������ */

                break;
            }
        }
    }
}

/**
 * ˵��       ƽ������, m^n
 * ����       m: ����
 * ����       n: ָ��
 * ����ֵ      m��n�η�
 */
static uint32_t bsp_rgb_lcd_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)result *= m;

    return result;
}

/**
 * ˵��       ��ʾlen������
 * ����       x,y : ��ʼ����
 * ����       num : ��ֵ(0 ~ 2^32)
 * ����       len : ��ʾ���ֵ�λ��
 * ����       size: ѡ������ FONT_ASCII_12_6/FONT_ASCII_16_8/FONT_ASCII_24_12/FONT_ASCII_32_16
 * ����ֵ      ��
 */
void bsp_rgb_lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, FONT_ASCII size, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)   /* ������ʾλ��ѭ�� */
    {
        temp = (num / bsp_rgb_lcd_pow(10, len - t - 1)) % 10;   /* ��ȡ��Ӧλ������ */

        if (enshow == 0 && t < (len - 1))   /* û��ʹ����ʾ,�һ���λҪ��ʾ */
        {
            if (temp == 0)
            {
                bsp_rgb_lcd_show_char(x + (size / 2)*t, y, ' ', size, 0, color,bsp_rgb_lcd_parameter.g_back_color);/* ��ʾ�ո�,ռλ */
                continue;   /* �����¸�һλ */
            }
            else
            {
                enshow = 1; /* ʹ����ʾ */
            }

        }

        bsp_rgb_lcd_show_char(x + (size / 2)*t, y, temp + '0', size, 0, color,bsp_rgb_lcd_parameter.g_back_color); /* ��ʾ�ַ� */
    }
}

/**
 * ˵��       ��չ��ʾlen������(��λ��0Ҳ��ʾ)
 * ����       x,y : ��ʼ����
 * ����       num : ��ֵ(0 ~ 2^32)
 * ����       len : ��ʾ���ֵ�λ��
 * ����       size: ѡ������ FONT_ASCII_12_6/FONT_ASCII_16_8/FONT_ASCII_24_12/FONT_ASCII_32_16
 * ����       mode: ��ʾģʽ
 *              [7]:0,�����;1,���0.
 *              [6:1]:����
 *              [0]:0,�ǵ�����ʾ;1,������ʾ.
 *
 * ����ֵ      ��
 */
void bsp_rgb_lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, FONT_ASCII size, uint8_t mode, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)   /* ������ʾλ��ѭ�� */
    {
        temp = (num / bsp_rgb_lcd_pow(10, len - t - 1)) % 10;    /* ��ȡ��Ӧλ������ */

        if (enshow == 0 && t < (len - 1))   /* û��ʹ����ʾ,�һ���λҪ��ʾ */
        {
            if (temp == 0)
            {
                if (mode & 0X80)   /* ��λ��Ҫ���0 */
                {
                    bsp_rgb_lcd_show_char(x + (size / 2)*t, y, '0', size, mode & 0X01, color,bsp_rgb_lcd_parameter.g_back_color);  /* ��0ռλ */
                }
                else
                {
                    bsp_rgb_lcd_show_char(x + (size / 2)*t, y, ' ', size, mode & 0X01, color,bsp_rgb_lcd_parameter.g_back_color);  /* �ÿո�ռλ */
                }

                continue;
            }
            else
            {
                enshow = 1; /* ʹ����ʾ */
            }

        }

        bsp_rgb_lcd_show_char(x + (size / 2)*t, y, temp + '0', size, mode & 0X01, color,bsp_rgb_lcd_parameter.g_back_color);
    }
}

/**
 * ˵��       ��ʾ�ַ���
 * ����       x,y         : ��ʼ����
 * ����       width,height: �����С
 * ����       size        : ѡ������ FONT_ASCII_12_6/FONT_ASCII_16_8/FONT_ASCII_24_12/FONT_ASCII_32_16
 * ����       p           : �ַ����׵�ַ
 * ����ֵ      ��
 */
void bsp_rgb_lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, FONT_ASCII size, char *p, uint16_t color)
{
    uint8_t x0 = x;
    width += x;
    height += y;

    while( ((*p <= '~') && (*p >= ' ')) || (*p =='\r') || (*p =='\n') )   /* �ж��ǲ��ǷǷ��ַ�! */
    {
        if((*p =='\r'))
        {
            x = x0;
        }        
        else if((*p =='\n'))
        {
            y += size;        
        }
        else if (x >= width)
        {
            x = x0;
            y += size;
        }
        if (y >= height)break;  /* �˳� */

        bsp_rgb_lcd_show_char(x, y, *p, size, 0, color,bsp_rgb_lcd_parameter.g_back_color);
        x += size / 2;
        p++;
    }
}

/**
 * ˵��       LCD��ӡ���ڳ�ʼ��
 * ����       x,y         : ��ʼ����
 * ����       x_end,y_end:  ����������
 * ����       size        : ѡ������ FONT_ASCII_12_6/FONT_ASCII_16_8/FONT_ASCII_24_12/FONT_ASCII_32_16
 * ����       back_color  : ������ɫ
 * ����       point_color   ������ɫ
 * ����ֵ      ��
 */
void bsp_rgb_lcd_printf_init(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end,FONT_ASCII char_size,uint16_t back_color,uint16_t point_color)
{
    bsp_rgb_lcd_pritnf_parameter.x_start=x_start;
    bsp_rgb_lcd_pritnf_parameter.y_start=y_start;

    bsp_rgb_lcd_pritnf_parameter.x_end=x_end;
    bsp_rgb_lcd_pritnf_parameter.y_end=y_end;

    bsp_rgb_lcd_pritnf_parameter.x_now=x_start;
    bsp_rgb_lcd_pritnf_parameter.y_now=y_start;

    bsp_rgb_lcd_pritnf_parameter.size=char_size;

    bsp_rgb_lcd_pritnf_parameter.back_color=back_color;   
    bsp_rgb_lcd_pritnf_parameter.point_color=point_color; 

    bsp_rgb_lcd_fill(bsp_rgb_lcd_pritnf_parameter.x_start,bsp_rgb_lcd_pritnf_parameter.y_start,bsp_rgb_lcd_pritnf_parameter.x_end,bsp_rgb_lcd_pritnf_parameter.y_end,bsp_rgb_lcd_pritnf_parameter.back_color);    
}


/**
 * ˵��       LCD��ӡ
 * ����       ...��printf��ͬ�÷����Զ�����
 * ����ֵ      ��
 */
void bsp_rgb_lcd_printf(const char * sFormat, ...) 
{
    #define PRINTF_MAX_LEN 100
    char printf_buffer[PRINTF_MAX_LEN];
    char* p=printf_buffer;
    uint16_t len=0,count=0;
    va_list ParamList;
    va_start(ParamList, sFormat);     
    vsprintf(printf_buffer,sFormat, ParamList);     
    va_end(ParamList); 

    len=strlen(printf_buffer);    

    while( ( ((*p <= '~') && (*p >= ' ')) || (*p =='\r') || (*p =='\n') ) && (count<len) )   /* �ж��ǲ��ǷǷ��ַ�! */
    {
        if((*p =='\r'))
        {
            bsp_rgb_lcd_pritnf_parameter.x_now = bsp_rgb_lcd_pritnf_parameter.x_start;
        }        
        else if((*p =='\n'))
        {
            bsp_rgb_lcd_pritnf_parameter.y_now += bsp_rgb_lcd_pritnf_parameter.size; 
            bsp_rgb_lcd_fill(bsp_rgb_lcd_pritnf_parameter.x_now,bsp_rgb_lcd_pritnf_parameter.y_now,bsp_rgb_lcd_pritnf_parameter.x_end,bsp_rgb_lcd_pritnf_parameter.y_now+bsp_rgb_lcd_pritnf_parameter.size,bsp_rgb_lcd_pritnf_parameter.back_color);            
        }
        else if( (bsp_rgb_lcd_pritnf_parameter.x_now + bsp_rgb_lcd_pritnf_parameter.size/2) > bsp_rgb_lcd_pritnf_parameter.x_end)
        {
            bsp_rgb_lcd_pritnf_parameter.x_now = bsp_rgb_lcd_pritnf_parameter.x_start;
            bsp_rgb_lcd_pritnf_parameter.y_now += bsp_rgb_lcd_pritnf_parameter.size;
            bsp_rgb_lcd_fill(bsp_rgb_lcd_pritnf_parameter.x_now,bsp_rgb_lcd_pritnf_parameter.y_now,bsp_rgb_lcd_pritnf_parameter.x_end,bsp_rgb_lcd_pritnf_parameter.y_now+bsp_rgb_lcd_pritnf_parameter.size,bsp_rgb_lcd_pritnf_parameter.back_color);                        
        }
        else if ( (bsp_rgb_lcd_pritnf_parameter.y_now+bsp_rgb_lcd_pritnf_parameter.size) > bsp_rgb_lcd_pritnf_parameter.y_end)
        {
            bsp_rgb_lcd_pritnf_parameter.x_now = bsp_rgb_lcd_pritnf_parameter.x_start;
            bsp_rgb_lcd_pritnf_parameter.y_now = bsp_rgb_lcd_pritnf_parameter.y_start;            
            bsp_rgb_lcd_fill(bsp_rgb_lcd_pritnf_parameter.x_start,bsp_rgb_lcd_pritnf_parameter.y_start,bsp_rgb_lcd_pritnf_parameter.x_end,bsp_rgb_lcd_pritnf_parameter.y_end,bsp_rgb_lcd_pritnf_parameter.back_color);
        }

        if((*p !='\r')&&(*p !='\n'))
        {
            bsp_rgb_lcd_show_char(bsp_rgb_lcd_pritnf_parameter.x_now, bsp_rgb_lcd_pritnf_parameter.y_now, *p, bsp_rgb_lcd_pritnf_parameter.size, 0, bsp_rgb_lcd_pritnf_parameter.point_color,bsp_rgb_lcd_pritnf_parameter.back_color);
            bsp_rgb_lcd_pritnf_parameter.x_now += bsp_rgb_lcd_pritnf_parameter.size / 2;            
        }
        

        p++;
        count++;
    }

}


void bsp_rgb_lcd_show_log(void)
{
    //��ʾlogͼƬ
    // bsp_rgb_lcd_color_fill(100,0,273,99,(uint16_t*)gd_log_picture);
	bsp_rgb_lcd_color_fill(0 , 0 , 499 , 99 , (uint16_t*)gd_log_picture);
}