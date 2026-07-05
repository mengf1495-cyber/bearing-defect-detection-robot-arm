#include "app_uart.h"
#include <string.h>

/*
 * app_sequence 模块已迁移至 XMC7100。
 * 以下 weak 桩函数用于维持旧固件（app_uart / robot_arm_entry）的编译兼容。
 * GD32H7 主程序不再调用这些函数。
 */
__attribute__((weak)) uint8_t app_sequence_start_flip(void) { return 0; }
__attribute__((weak)) uint8_t app_sequence_start_bad(void)  { return 0; }
__attribute__((weak)) void    app_sequence_stop(void)        { }
__attribute__((weak)) uint8_t app_sequence_is_busy(void)     { return 0; }
__attribute__((weak)) void    app_sequence_run(void)         { }
__attribute__((weak)) uint8_t app_sequence_get_step(void)    { return 0; }
__attribute__((weak)) uint8_t app_sequence_get_type(void)    { return 0; }

/**
 * @函数描述: uart串口相关设备控制初始化
 * @return {*}
 */
void app_uart_init(void)
{
	uart1_init(115200); /* 连接总线设备串口 */
	uart2_init(115200); /* 连接总线设备串口 */
	uart3_init(115200); /* 连接总线设备串口 */
	uart4_init(115200); /* 连接总线设备串口 */
  uart5_init(115200); /* 连接总线设备串口 */
}

/**
 * @函数描述: 循环检测串口接收到的指令
 * @return {*}
 */
void app_uart_run(void)
{
    if (!uart_get_ok)
    {
        return;
    }

    /*
     * 翻面动作：
     * 串口发送 $FLIP!
     */
    if (strcmp(uart_receive_buf, "$FLIP!") == 0)
    {
        if (app_sequence_start_flip())
        {
            uart1_send_str("$FLIP_START!\r\n");
        }
        else
        {
            uart1_send_str("$BUSY!\r\n");
        }
    }

    /*
     * 缺陷件放下：
     * 串口发送 $BAD!
     */
    else if (strcmp(uart_receive_buf, "$BAD!") == 0)
    {
        if (app_sequence_start_bad())
        {
            uart1_send_str("$BAD_START!\r\n");
        }
        else
        {
            uart1_send_str("$BUSY!\r\n");
        }
    }

    /*
     * 停止后续动作。
     */
    else if (strcmp(uart_receive_buf, "$STOP!") == 0)
    {
        app_sequence_stop();
        uart1_send_str("$STOPPED!\r\n");
    }

    /*
     * 原有串口功能
     */
    else
    {
        if (uart_mode == 1)
        {
            parse_cmd(uart_receive_buf);
        }
        else if (uart_mode == 2)
        {
            parse_action(uart_receive_buf);
        }
        else if (uart_mode == 3)
        {
            parse_action(uart_receive_buf);
        }
        else if (uart_mode == 4)
        {
            save_action(uart_receive_buf);
        }
    }

    uart_get_ok = 0;
    uart_mode = 0;
}