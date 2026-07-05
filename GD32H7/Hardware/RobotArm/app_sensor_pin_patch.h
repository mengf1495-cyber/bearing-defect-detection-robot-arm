/* Replace the three STM32 GPIO_ReadInputDataBit macros in app_sensor.h with these. */
#include "robot_arm_port.h"
#define shengyin_Read() robot_sound_read()
#define chumo_READ()    robot_touch_read()
#define hongwai_READ()  robot_ir_read()
