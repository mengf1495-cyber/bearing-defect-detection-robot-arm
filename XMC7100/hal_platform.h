/*!
    \file    hal_platform.h
    \brief   XMC7100 硬件抽象层 — 移植到其他 MCU 时只需修改本文件的实现

    所有硬件操作均通过这12个函数完成，实现时替换为具体 MCU 的 HAL 函数。

    对于 XMC7100: 使用 Infineon XMC HAL (XMC_GPIO, XMC_UART_CH, SYSTIMER)
    对于 STM32:   使用 STM32 HAL (HAL_GPIO, HAL_UART, HAL_GetTick)
    对于 GD32:    使用 GD32 StdPeriph (gpio_*, usart_*, driver_tick)

    \version 2025-07-04
*/

#ifndef HAL_PLATFORM_H
#define HAL_PLATFORM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ──────────────── GPIO ──────────────── */

/* 配置引脚为推挽输出，初始低电平 */
void hal_gpio_output_init(uint32_t port, uint32_t pin);

/* 配置引脚为输入，内部上拉 */
void hal_gpio_input_pullup_init(uint32_t port, uint32_t pin);

/* 输出高电平 */
void hal_gpio_set(uint32_t port, uint32_t pin);

/* 输出低电平 */
void hal_gpio_reset(uint32_t port, uint32_t pin);

/* 读取引脚状态，返回 1=高, 0=低 */
uint8_t hal_gpio_read(uint32_t port, uint32_t pin);

/* ──────────────── UART ──────────────── */

/* 指定 UART 外设的句柄类型（平台相关） */
typedef void* hal_uart_handle_t;

/* 初始化 UART: 波特率、8N1、TX/RX、使能 RX 中断 */
void hal_uart_init(hal_uart_handle_t uart, uint32_t baud,
                   uint32_t tx_port, uint32_t tx_pin,
                   uint32_t rx_port, uint32_t rx_pin);

/* 阻塞发送一个字节 */
void hal_uart_send_byte(hal_uart_handle_t uart, char c);

/* 阻塞发送字符串 */
void hal_uart_send_str(hal_uart_handle_t uart, const char *s);

/* 非阻塞接收一个字节，返回 -1 表示 RX 缓冲区为空 */
int hal_uart_recv_byte(hal_uart_handle_t uart);

/* 返回 RX 缓冲区可读字节数 */
uint16_t hal_uart_rx_available(hal_uart_handle_t uart);

/* ──────────────── Timer ──────────────── */

/* 初始化定时器（用于 millis/delay） */
void hal_timer_init(void);

/* 返回从启动到现在的毫秒数 */
uint32_t hal_timer_millis(void);

/* 阻塞延迟（毫秒） */
void hal_timer_delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* HAL_PLATFORM_H */
