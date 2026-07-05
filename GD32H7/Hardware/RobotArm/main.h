#ifndef _MAIN_H_
#define _MAIN_H_
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "gd32h7xx.h"
typedef uint8_t u8; typedef uint16_t u16; typedef uint32_t u32; typedef int8_t s8; typedef int16_t s16; typedef int32_t s32; typedef uint8_t bool;
#include "robot_arm_time.h"
#include "robot_arm_port.h"
#include "robot_arm_uart.h"
/* servo PWM 已迁移至 XMC7100 */
#include "robot_arm_flash.h"
#include "robot_arm_ultrasonic.h"
#include "robot_arm_adc.h"
#include "robot_arm_soft_i2c.h"
#include "robot_arm_ps2.h"
#include "robot_arm_buzzer.h"
/* Keep these algorithm/application headers from your STM32 project unchanged. */
#include "y_kinematics/y_kinematics.h"
#include "y_global/y_global.h"
#include "us01/us01.h"
#include "oled/y_oled.h"
#include "LTR381/LTR381.h"
#include "app_ps2.h"
#include "app_uart.h"
#include "app_sensor.h"
#include "resource/resource.h"
#endif
