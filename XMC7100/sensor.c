/*!
    \file    sensor.c
    \brief   NPN 激光传感器读取实现

    引脚映射（从 GD32H7 移植）:
    - 传感器 → PB0 (输入, 上拉)

    NPN 传感器输出逻辑:
    - 轴承遮挡 → 传感器输出低电平 → PB0 读取为 0 → 返回 1
    - 无轴承   → 传感器输出高电平 → PB0 读取为 1 → 返回 0

    \version 2025-07-04
*/

#include "sensor.h"
#include "hal_platform.h"

/* XMC7100 引脚定义 — 根据实际接线修改 */
#define SENSOR_PORT  0   /* PB0 → 替换为 XMC7100 实际端口 */
#define SENSOR_PIN   0   /* 替换为 XMC7100 实际引脚 */

void sensor_init(void)
{
    hal_gpio_input_pullup_init(SENSOR_PORT, SENSOR_PIN);
}

uint8_t sensor_is_triggered(void)
{
    /* NPN: LOW = triggered */
    return (hal_gpio_read(SENSOR_PORT, SENSOR_PIN) == 0) ? 1 : 0;
}
