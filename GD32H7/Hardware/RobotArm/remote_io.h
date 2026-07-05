/*!
    \file    remote_io.h
    \brief   GD32H7 <-> XMC7100 UART3 communication layer

    协议：
    - GD32H7 为 Master，XMC7100 为 Slave
    - 帧格式: $COMMAND[:PARAM]!
    - UART3 PD8(TX) / PD9(RX), 115200 8N1

    \version 2025-07-04
*/

#ifndef REMOTE_IO_H
#define REMOTE_IO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ── 初始化 ── */
void remote_io_init(void);

/* ── 主循环处理（解析接收到的响应帧，更新缓存状态）── */
void remote_io_process(void);

/* ── UART3 RX 中断处理 ── */
void remote_io_uart3_irq(void);

/* ── 电机控制 ── */
void    remote_io_motor_on(void);
void    remote_io_motor_off(void);

/* ── 机械臂动作 ── */
uint8_t remote_io_start_flip(void);
uint8_t remote_io_push_defect(void);
void    remote_io_reset(void);

/* ── 状态查询（返回缓存值，无 UART 往返延迟）── */
uint8_t remote_io_is_busy(void);
uint8_t remote_io_get_step(void);
char    remote_io_get_seq_type(void);   /* 'I'=idle, 'F'=flip, 'B'=bad */

/* ── 传感器状态（返回缓存值，XMC7100 主动上报更新）── */
uint8_t remote_io_get_sensor(void);

/* ── 通信健康检查 ── */
uint8_t remote_io_ping(void);
uint8_t remote_io_is_ok(void);

#ifdef __cplusplus
}
#endif

#endif /* REMOTE_IO_H */
