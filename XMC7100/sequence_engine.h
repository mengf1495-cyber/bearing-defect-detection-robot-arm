/*!
    \file    sequence_engine.h
    \brief   动作序列引擎 — 移植自 GD32 项目 app_sequence.h

    通过 UART 发送 #xxxPxxxxTxxxx! 格式的舵机指令到总线舵机控制器。
    每步间隔 50ms，确保舵机有足够的过渡时间。

    \version 2025-07-04
*/

#ifndef SEQUENCE_ENGINE_H
#define SEQUENCE_ENGINE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SEQ_NONE = 0,
    SEQ_FLIP,
    SEQ_BAD
} seq_type_t;

/* 启动 FLIP 序列（9步翻面） */
uint8_t sequence_engine_start(seq_type_t type);

/* 推进序列（在主循环中调用，检查时间是否到期并发送下一步） */
/* 返回 1 = 序列结束，0 = 还在执行 */
uint8_t sequence_engine_run(void);

/* 立即停止当前序列 */
void sequence_engine_stop(void);

/* 查询状态 */
uint8_t sequence_engine_is_busy(void);
uint8_t sequence_engine_get_step(void);   /* 1-based */
seq_type_t sequence_engine_get_type(void);

#ifdef __cplusplus
}
#endif

#endif /* SEQUENCE_ENGINE_H */
