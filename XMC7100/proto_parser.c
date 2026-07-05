/*!
    \file    proto_parser.c
    \brief   $...! 帧格式命令解析器实现
*/

#include "proto_parser.h"
#include "hal_platform.h"
#include <string.h>

#define RX_BUF_SIZE  128
#define CMD_BUF_SIZE  64

static char _rx_buf[RX_BUF_SIZE];
static uint8_t _rx_index;
static uint8_t _rx_mode;  /* 0=idle, 1=$-mode */

static char _cmd_buf[CMD_BUF_SIZE];
static volatile uint8_t _cmd_ready;

static proto_cmd_handler_t _handler = NULL;
static void *_ctx = NULL;

void proto_parser_init(void)
{
    _rx_index = 0;
    _rx_mode = 0;
    _cmd_ready = 0;
    memset(_rx_buf, 0, RX_BUF_SIZE);
    memset(_cmd_buf, 0, CMD_BUF_SIZE);
}

void proto_parser_register_handler(proto_cmd_handler_t handler, void *ctx)
{
    _handler = handler;
    _ctx = ctx;
}

void proto_parser_feed(char c)
{
    /* 如果上一帧还未被主循环取走，丢弃 */
    if (_cmd_ready) return;

    /* 空闲状态：等待 '$' */
    if (_rx_index == 0) {
        if (c == '$') {
            _rx_mode = 1;
            /* 不存储 $，直接进入数据模式 */
            return;
        }
        /* 忽略 $ 以外的字符 */
        return;
    }

    /* 数据模式 */
    if (_rx_index < RX_BUF_SIZE - 1) {
        _rx_buf[_rx_index++] = c;
    } else {
        /* 缓冲区溢出，重置 */
        _rx_index = 0;
        _rx_mode = 0;
        return;
    }

    /* 帧结束: '!' */
    if (_rx_mode == 1 && c == '!') {
        /* 复制到命令缓冲区（去掉末尾的 '!'）*/
        uint8_t len = _rx_index - 1;  /* 不包含 '!' */
        if (len >= CMD_BUF_SIZE) len = CMD_BUF_SIZE - 1;
        memcpy(_cmd_buf, _rx_buf, len);
        _cmd_buf[len] = '\0';

        /* 去掉可能尾随的 \r\n */
        if (len > 0 && _cmd_buf[len - 1] == '\r') _cmd_buf[--len] = '\0';
        if (len > 0 && _cmd_buf[len - 1] == '\n') _cmd_buf[--len] = '\0';

        _cmd_ready = 1;

        /* 重置接收状态 */
        _rx_index = 0;
        _rx_mode = 0;
    }
}

void proto_parser_process(void *uart_handle)
{
    /* 从 UART RX 缓冲区读取所有可用字节 */
    while (hal_uart_rx_available(uart_handle) > 0) {
        int byte = hal_uart_recv_byte(uart_handle);
        if (byte >= 0) {
            proto_parser_feed((char)byte);
        }
    }

    /* 处理完整帧 */
    if (_cmd_ready && _handler) {
        _handler(_cmd_buf, _ctx);
        _cmd_ready = 0;
    }
}
