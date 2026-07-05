/*!
    \file    servo_bus.c
    \brief   舵机总线通信实现 — UART1 → 外部总线舵机控制器

    协议:
    - MCU 通过 UART1 发送 #xxxPxxxxTxxxx! 格式的动作指令
    - 舵机控制器收到后解析并生成 PWM 信号驱动舵机
    - 停止命令: $DST!\r\n

    引脚映射（从 GD32H7 移植）:
    - UART1_TX → 舵机控制器 RXD
    - UART1_RX → 舵机控制器 TXD

    \version 2025-07-04
*/

#include "servo_bus.h"
#include "hal_platform.h"

/* XMC7100 UART 句柄和引脚 — 根据实际接线修改 */
#define SERVO_UART      ((hal_uart_handle_t)1)  /* UART1 */
#define SERVO_TX_PORT   0   /* 替换为 XMC7100 实际端口 */
#define SERVO_TX_PIN    0   /* 替换为 XMC7100 实际引脚 */
#define SERVO_RX_PORT   0   /* 替换为 XMC7100 实际端口 */
#define SERVO_RX_PIN    0   /* 替换为 XMC7100 实际引脚 */

void servo_bus_init(uint32_t baud)
{
    hal_uart_init(SERVO_UART, baud,
                  SERVO_TX_PORT, SERVO_TX_PIN,
                  SERVO_RX_PORT, SERVO_RX_PIN);
}

void servo_bus_send_byte(char c)
{
    hal_uart_send_byte(SERVO_UART, c);
}

void servo_bus_send_str(const char *s)
{
    hal_uart_send_str(SERVO_UART, s);
}
