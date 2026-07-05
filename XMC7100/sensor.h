/*!
    \file    sensor.h
    \brief   NPN 激光传感器读取 (PB0, LOW=轴承检测到)
    \version 2025-07-04
*/

#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 初始化传感器引脚 (输入, 内部上拉) */
void sensor_init(void);

/* 读取传感器状态: 1=轴承检测到, 0=无轴承 */
uint8_t sensor_is_triggered(void);

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_H */
