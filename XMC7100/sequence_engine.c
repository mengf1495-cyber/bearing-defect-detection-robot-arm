/*!
    \file    sequence_engine.c
    \brief   动作序列引擎 — 移植自 app_sequence.c

    每个序列包含 9 组动作，每组包含 6 个舵机指令。
    每组通过 UART 发送到总线舵机控制器，每条指令之间延时 3ms。
    每组之间等待 ACTION_WAIT_MS (50ms)。

    \version 2025-07-04
*/

#include "sequence_engine.h"
#include "hal_platform.h"
#include "servo_bus.h"

/* 每组动作之间等待 50ms */
#define ACTION_WAIT_MS    50U

/*
 * ==========================
 * FLIP: 9 组动作（轴承翻面）
 * ==========================
 */
static const char *const flip_action_table[] =
{
    /* G0001 */
    "#000P1493T1500!"
    "#001P2138T0600!"
    "#002P2357T0600!"
    "#003P1143T1500!"
    "#004P0859T1500!"
    "#005P0857T1500!",

    /* G0002 */
    "#000P1493T1500!"
    "#001P2138T0600!"
    "#002P2357T0600!"
    "#003P0974T0500!"
    "#004P0859T1500!"
    "#005P0857T1500!",

    /* G0003 */
    "#000P1493T1500!"
    "#001P2138T1500!"
    "#002P2357T1500!"
    "#003P0974T1500!"
    "#004P0859T1500!"
    "#005P1480T0600!",

    /* G0004 */
    "#000P1493T1500!"
    "#001P1929T0500!"
    "#002P2121T0500!"
    "#003P0974T1500!"
    "#004P0859T1500!"
    "#005P1480T1500!",

    /* G0005 */
    "#000P1493T1500!"
    "#001P1929T1500!"
    "#002P2121T1500!"
    "#003P0974T1500!"
    "#004P2160T1500!"
    "#005P1480T1500!",

    /* G0006 */
    "#000P1493T1500!"
    "#001P2138T0500!"
    "#002P2357T0500!"
    "#003P1010T0500!"
    "#004P2160T1500!"
    "#005P1480T1500!",

    /* G0007 */
    "#000P1493T1500!"
    "#001P2138T0600!"
    "#002P2357T0600!"
    "#003P1200T1500!"
    "#004P2160T1500!"
    "#005P0857T0600!",

    /* G0008 */
    "#000P1493T1500!"
    "#001P2138T0600!"
    "#002P2357T0600!"
    "#003P1200T0600!"
    "#004P2160T1500!"
    "#005P0857T1500!",

    /* G0009 */
    "#000P1493T1500!"
    "#001P2138T0600!"
    "#002P2357T0600!"
    "#003P1200T1500!"
    "#004P0859T0600!"
    "#005P0857T1500!",

    /* G0010 */
    "#000P1493T1500!"
    "#001P2138T0600!"
    "#002P2357T0600!"
    "#003P1143T0400!"
    "#004P0859T0600!"
    "#005P0857T1500!"
};

#define FLIP_ACTION_COUNT \
    ((uint8_t)(sizeof(flip_action_table) / sizeof(flip_action_table[0])))

/*
 * ==========================
 * BAD: 9 组动作（缺陷剔除）
 * ==========================
 */
static const char *const bad_action_table[] =
{
    /* G0001 */
    "#000P1493T1500!"
    "#001P2138T0600!"
    "#002P2357T0600!"
    "#003P1143T1500!"
    "#004P0859T1500!"
    "#005P0857T1500!",

    /* G0002 */
    "#000P1493T1500!"
    "#001P2138T0600!"
    "#002P2357T0600!"
    "#003P0974T0500!"
    "#004P0859T1500!"
    "#005P0857T1500!",

    /* G0003 */
    "#000P1493T1500!"
    "#001P2138T1500!"
    "#002P2357T1500!"
    "#003P0974T1500!"
    "#004P0859T1500!"
    "#005P1480T0600!",

    /* G0004 */
    "#000P1493T1500!"
    "#001P1929T0500!"
    "#002P2121T0500!"
    "#003P0974T1500!"
    "#004P0859T1500!"
    "#005P1480T1500!",

    /* G0005 */
    "#000P0786T1500!"
    "#001P1929T0500!"
    "#002P2121T0500!"
    "#003P0974T1500!"
    "#004P0859T1500!"
    "#005P1480T1500!",

    /* G0006 */
    "#000P0786T1500!"
    "#001P1316T0800!"
    "#002P2121T0500!"
    "#003P1071T0800!"
    "#004P0859T1500!"
    "#005P1480T1500!",

    /* G0007 */
    "#000P0786T1500!"
    "#001P1286T0600!"
    "#002P2121T0600!"
    "#003P1071T1500!"
    "#004P0859T1500!"
    "#005P1214T0500!",

    /* G0008 */
    "#000P0786T1500!"
    "#001P2138T0700!"
    "#002P2357T0700!"
    "#003P1143T0600!"
    "#004P0859T0600!"
    "#005P0857T0500!",

    /* G0009 */
    "#000P1493T0800!"
    "#001P2138T0600!"
    "#002P2357T0600!"
    "#003P1286T1500!"
    "#004P0859T0600!"
    "#005P0857T1500!"
};

#define BAD_ACTION_COUNT \
    ((uint8_t)(sizeof(bad_action_table) / sizeof(bad_action_table[0])))

/* ── 引擎内部状态 ── */
static seq_type_t  _seq_type   = SEQ_NONE;
static uint8_t     _seq_index  = 0;
static uint8_t     _seq_busy   = 0;
static uint32_t    _seq_step_start_ms = 0;

/* ── 辅助函数 ── */

static uint8_t _get_action_count(void)
{
    if (_seq_type == SEQ_FLIP) return FLIP_ACTION_COUNT;
    if (_seq_type == SEQ_BAD)  return BAD_ACTION_COUNT;
    return 0;
}

static const char *_get_action(void)
{
    if (_seq_type == SEQ_FLIP && _seq_index < FLIP_ACTION_COUNT) {
        return flip_action_table[_seq_index];
    }
    if (_seq_type == SEQ_BAD && _seq_index < BAD_ACTION_COUNT) {
        return bad_action_table[_seq_index];
    }
    return NULL;
}

/* 执行当前动作组：通过 UART 发送所有舵机指令 */
static void _execute_current(void)
{
    const char *action = _get_action();

    if (!action) {
        /* 序列结束 */
        _seq_busy = 0;
        _seq_type = SEQ_NONE;
        _seq_index = 0;
        return;
    }

    /*
     * 逐字节发送动作字符串到总线舵机控制器。
     * 每个 '!' 后延时 3ms，确保舵机控制器有充足时间解析每条指令。
     */
    while (*action) {
        servo_bus_send_byte(*action);
        if (*action == '!') {
            hal_timer_delay_ms(3);
        }
        action++;
    }

    _seq_step_start_ms = hal_timer_millis();
}

/* ── 公共 API ── */

uint8_t sequence_engine_start(seq_type_t type)
{
    if (_seq_busy) return 0;
    if (type != SEQ_FLIP && type != SEQ_BAD) return 0;

    _seq_type  = type;
    _seq_index = 0;
    _seq_busy  = 1;

    _execute_current();
    return 1;
}

/* 返回 1 = 序列结束 */
uint8_t sequence_engine_run(void)
{
    uint8_t action_count;

    if (!_seq_busy) return 0;

    /* 检查时间是否到 */
    if ((uint32_t)(hal_timer_millis() - _seq_step_start_ms) < ACTION_WAIT_MS) {
        return 0;
    }

    action_count = _get_action_count();
    _seq_index++;

    if (_seq_index >= action_count) {
        /* 所有步骤执行完毕 */
        _seq_busy = 0;
        _seq_type = SEQ_NONE;
        _seq_index = 0;
        return 1;  /* 序列结束 */
    }

    /* 执行下一步 */
    _execute_current();
    return 0;
}

void sequence_engine_stop(void)
{
    _seq_busy  = 0;
    _seq_type  = SEQ_NONE;
    _seq_index = 0;
}

uint8_t sequence_engine_is_busy(void)  { return _seq_busy; }

uint8_t sequence_engine_get_step(void)
{
    if (!_seq_busy) return 0;
    return (uint8_t)(_seq_index + 1);
}

seq_type_t sequence_engine_get_type(void) { return _seq_type; }
