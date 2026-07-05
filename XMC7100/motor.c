/*!
    \file    motor.c
    \brief   传送带直流电机控制实现

    引脚映射（从 GD32H7 移植）:
    - PWMA  → PA0  (PWM 使能 — 简化用 GPIO 高/低控制)
    - AIN1  → PH13 (方向控制 1)
    - AIN2  → PH15 (方向控制 2)

    控制逻辑:
    - 正转: PWMA=H, AIN1=H, AIN2=L
    - 停止: PWMA=L, AIN1=L, AIN2=L

    \version 2025-07-04
*/

#include "motor.h"
#include "hal_platform.h"

/*
 * XMC7100 引脚定义 — 根据实际接线修改
 * 使用 XMC7100 GPIO 端口/引脚编号
 */
#define MOTOR_PWMA_PORT  0   /* PA0 → 替换为 XMC7100 实际端口 */
#define MOTOR_PWMA_PIN   0   /* 替换为 XMC7100 实际引脚 */

#define MOTOR_AIN1_PORT  0   /* PH13 → 替换为 XMC7100 实际端口 */
#define MOTOR_AIN1_PIN   0   /* 替换为 XMC7100 实际引脚 */

#define MOTOR_AIN2_PORT  0   /* PH15 → 替换为 XMC7100 实际端口 */
#define MOTOR_AIN2_PIN   0   /* 替换为 XMC7100 实际引脚 */

void motor_init(void)
{
    hal_gpio_output_init(MOTOR_PWMA_PORT, MOTOR_PWMA_PIN);
    hal_gpio_output_init(MOTOR_AIN1_PORT, MOTOR_AIN1_PIN);
    hal_gpio_output_init(MOTOR_AIN2_PORT, MOTOR_AIN2_PIN);
}

void motor_on(void)
{
    hal_gpio_set(MOTOR_PWMA_PORT, MOTOR_PWMA_PIN);
    hal_gpio_set(MOTOR_AIN1_PORT, MOTOR_AIN1_PIN);
    hal_gpio_reset(MOTOR_AIN2_PORT, MOTOR_AIN2_PIN);
}

void motor_off(void)
{
    hal_gpio_reset(MOTOR_PWMA_PORT, MOTOR_PWMA_PIN);
    hal_gpio_reset(MOTOR_AIN1_PORT, MOTOR_AIN1_PIN);
    hal_gpio_reset(MOTOR_AIN2_PORT, MOTOR_AIN2_PIN);
}
