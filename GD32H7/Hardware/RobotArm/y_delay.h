#ifndef __Y_DELAY_H
#define __Y_DELAY_H

#include <stdint.h>
#include "robot_arm_time.h"

/*
 * STM32 ????????? GD32H759 ????
 */
#define delay_us(us) robot_delay_us((uint32_t)(us))
#define delay_ms(ms) robot_delay_ms((uint32_t)(ms))
#define mdelay(ms)   robot_delay_ms((uint32_t)(ms))
#define millis()     robot_millis()
#define micros()     robot_micros()
#endif