/*!
 * 文件名称： bsp_rgb_pwm.h
 * 描    述： rgb灯珠驱动文件
 * 版本：     2023-12-03, V1.0
*/

 

#ifndef BSP_RGB_PWM_H
#define BSP_RGB_PWM_H

#include "driver_public.h"
#include "driver_timer.h"

#define  proport          1000000 

TIMER_CH_DEF_EXTERN(RGB_TIMER_R);
TIMER_CH_DEF_EXTERN(RGB_TIMER_G);
TIMER_CH_DEF_EXTERN(RGB_TIMER_B);

void bsp_rgb_init(uint32_t counter_frequency,uint16_t period);
void bsp_rgb_duty_set(uint8_t r_duty_value, uint8_t g_duty_value, uint8_t b_duty_value);
#endif /* BSP_RGB_PWM_H*/
