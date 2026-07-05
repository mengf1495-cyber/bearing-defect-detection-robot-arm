/*!
    \file    servo_bus.h
    \brief   舵机总线通信 — 通过 UART1 发送指令到外部总线舵机控制器
    \version 2025-07-04
*/

#ifndef SERVO_BUS_H
#define SERVO_BUS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 初始化舵机总线 UART (115200 8N1) */
void servo_bus_init(uint32_t baud);

/* 发送一个字节 */
void servo_bus_send_byte(char c);

/* 发送字符串 */
void servo_bus_send_str(const char *s);

#ifdef __cplusplus
}
#endif

#endif /* SERVO_BUS_H */
