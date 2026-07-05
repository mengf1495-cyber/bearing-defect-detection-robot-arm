#include "driver_timer.h"
#include "bsp_rgb_pwm.h"

/* 定义注册RGB对应PWM通道 */
TIMER_CH_DEF(RGB_TIMER_R,TIMER0,0,TIMER_CH_PWM_LOW,A,8,AF_PP,GPIO_AF_1);
TIMER_CH_DEF(RGB_TIMER_G,TIMER0,1,TIMER_CH_PWM_LOW,A,9,AF_PP,GPIO_AF_1);
TIMER_CH_DEF(RGB_TIMER_B,TIMER0,2,TIMER_CH_PWM_LOW,A,10,AF_PP,GPIO_AF_1);

/*!
* 说明     RGB初始化
* 输入[1]  counter_frequency 计数器频率
* 输入[2]  period 周期值
* 返回值   无
*/
void bsp_rgb_init(uint32_t counter_frequency,uint16_t period)
{
    driver_timer_multi_channel_init(&RGB_TIMER_R,&RGB_TIMER_G,&RGB_TIMER_B,NULL,counter_frequency,period);
    driver_timer_pwm_on(&RGB_TIMER_R);
    driver_timer_pwm_on(&RGB_TIMER_G);
    driver_timer_pwm_on(&RGB_TIMER_B);    
}


/*!
* 说明     RGB设置
* 输入[1]  r_duty_value 周期指（频率）
* 输入[2]  g_duty_value 占空比（0--100）
* 输入[3]  b_duty_value 占空比（0--100）
* 返回值   无
*/
void bsp_rgb_duty_set(uint8_t r_duty_value, uint8_t g_duty_value, uint8_t b_duty_value)
{
    driver_timer_pwm_duty_set(&RGB_TIMER_R,r_duty_value);
    driver_timer_pwm_duty_set(&RGB_TIMER_G,g_duty_value); 
    driver_timer_pwm_duty_set(&RGB_TIMER_B,b_duty_value);
}
