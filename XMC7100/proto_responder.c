/*!
    \file    proto_responder.c
    \brief   响应构建器实现
*/

#include "proto_responder.h"
#include "hal_platform.h"

static void *_uart = NULL;

void proto_responder_init(void *uart_handle)
{
    _uart = uart_handle;
}

void proto_responder_send(const char *response)
{
    if (!_uart || !response) return;

    hal_uart_send_byte(_uart, '$');
    hal_uart_send_str(_uart, response);
    hal_uart_send_byte(_uart, '!');
    hal_uart_send_byte(_uart, '\r');
    hal_uart_send_byte(_uart, '\n');
}
