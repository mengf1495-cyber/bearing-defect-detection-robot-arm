/*!
    \file    proto_responder.h
    \brief   响应构建器 — 发送 $RESPONSE! 帧到 GD32H7
    \version 2025-07-04
*/

#ifndef PROTO_RESPONDER_H
#define PROTO_RESPONDER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 初始化响应器，绑定 UART 句柄 */
void proto_responder_init(void *uart_handle);

/* 发送响应帧: $<response>!\r\n */
void proto_responder_send(const char *response);

#ifdef __cplusplus
}
#endif

#endif /* PROTO_RESPONDER_H */
