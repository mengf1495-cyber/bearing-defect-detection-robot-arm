#ifndef _Y_LED_H_
#define _Y_LED_H_
#include "robot_arm_port.h"
#include "robot_arm_buzzer.h"
static inline void led_init(void){}
#define LED_TOGGLE() robot_led_toggle()
#endif
