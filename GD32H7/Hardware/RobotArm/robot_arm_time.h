#ifndef __ROBOT_ARM_TIME_H
#define __ROBOT_ARM_TIME_H

#include <stdint.h>

/* ??? DWT ??????? Tick */
void robot_time_init(void);

/* SysTick ? 1 ms ???? */
void robot_time_tick_1ms(void);

/* ????????,?? ms */
uint32_t robot_millis(void);
uint32_t robot_micros(void); 
/* ???? */
void robot_delay_us(uint32_t us);
void robot_delay_ms(uint32_t ms);

#endif