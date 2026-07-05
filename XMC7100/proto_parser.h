/*!
    \file    proto_parser.h
    \brief   $...! 帧格式命令解析器

    用法：
    1. proto_parser_init() 初始化
    2. proto_parser_register_handler() 注册回调
    3. 主循环中调用 proto_parser_process() 喂入 UART RX 字节
    4. 收到完整帧时回调 handler(cmd_string, ctx)

    \version 2025-07-04
*/

#ifndef PROTO_PARSER_H
#define PROTO_PARSER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 命令处理回调: cmd = "PING", "MOTOR_ON", "FLIP" 等（不含 $ 和 !） */
typedef void (*proto_cmd_handler_t)(const char *cmd, void *ctx);

/* 初始化 */
void proto_parser_init(void);

/* 注册命令处理器 */
void proto_parser_register_handler(proto_cmd_handler_t handler, void *ctx);

/* 喂入一个接收字节 */
void proto_parser_feed(char c);

/* 处理 UART 接收缓冲区中的所有字节（在主循环中调用） */
void proto_parser_process(void *uart_handle);

#ifdef __cplusplus
}
#endif

#endif /* PROTO_PARSER_H */
