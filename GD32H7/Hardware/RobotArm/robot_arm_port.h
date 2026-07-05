#ifndef ROBOT_ARM_PORT_H
#define ROBOT_ARM_PORT_H
#include <stdint.h>
void robot_arm_hw_init(void);
void robot_arm_background_1ms(void);
uint8_t robot_sound_read(void);
uint8_t robot_touch_read(void);
uint8_t robot_ir_read(void);
void robot_led_toggle(void);
#endif
