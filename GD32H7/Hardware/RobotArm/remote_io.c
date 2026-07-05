/*!
    \file    remote_io.c
    \brief   GD32H7 <-> XMC7100 UART3 communication layer

    命令格式: $CMD!\r\n
    响应格式: $RESPONSE!\r\n 或 $RESPONSE:PARAM!\r\n
    XMC7100 主动上报: $DONE!, $SENSOR:1!, $SENSOR:0!

    \version 2025-07-04
*/

#include "remote_io.h"
#include "gd32h7xx.h"
#include "robot_arm_board.h"
#include "driver_public.h"
#include <string.h>

/* ── 常量 ── */
#define RX_BUF_SIZE         128
#define FRAME_BUF_SIZE       64
#define CMD_TIMEOUT_MS      500
#define MAX_RETRIES            3
#define PING_INTERVAL_MS    1000
#define PING_TIMEOUT_MS     3000

/* ── UART3 专用接收缓冲区 ── */
static char     _rx_buf[RX_BUF_SIZE];
static uint8_t  _rx_index;
static uint8_t  _rx_mode;       /* 0=idle, 1=$-mode */
static char     _frame_buf[FRAME_BUF_SIZE];
static volatile uint8_t _frame_ready;

/* ── 响应类型 ── */
typedef enum {
    RESP_NONE = 0,
    RESP_OK,
    RESP_BUSY,
    RESP_PONG,
    RESP_FLIP_START,
    RESP_BAD_START,
    RESP_STOPPED,
    RESP_DONE,
} resp_type_t;

/* ── 缓存状态 ── */
static uint8_t  _sensor_state;       /* 0=clear, 1=bearing detected */
static uint8_t  _busy_state;         /* 0=idle, 1=sequence running */
static uint8_t  _step_state;         /* 0=idle, 1-9=current step */
static char     _seq_type_char;      /* 'I'=idle, 'F'=flip, 'B'=bad */
static uint8_t  _comm_fault;         /* 0=OK, 1=communication lost */
static uint8_t  _initialized;

/* ── 命令/响应同步 ── */
static resp_type_t _resp_type;
static uint32_t    _last_ping_ms;
static uint32_t    _last_pong_ms;

/* ── 前向声明 ── */
static void _send_frame(const char *cmd);
static void _parse_frame(const char *frame);
static uint8_t _wait_response(resp_type_t expected1, resp_type_t expected2, uint32_t timeout_ms);

/* ──────────────────────────────────────────────
 * UART3 硬件操作
 * ────────────────────────────────────────────── */

static void _rx_reset(void)
{
    _rx_index = 0;
    _rx_mode = 0;
    memset(_rx_buf, 0, RX_BUF_SIZE);
}

/*!
    \brief      UART3 RX 中断处理 — 由 gd32h7xx_it.c 的 UART3_IRQHandler 调用
*/
void remote_io_uart3_irq(void)
{
    if (RESET == usart_interrupt_flag_get(ROBOT_UART3_PERIPH, USART_INT_FLAG_RBNE)) {
        return;
    }

    char c = (char)usart_data_receive(ROBOT_UART3_PERIPH);

    /* 如果上一帧还未被主循环取走，丢弃新数据 */
    if (_frame_ready) {
        return;
    }

    /* 空闲状态：等待 '$' 起始符 */
    if (_rx_index == 0) {
        if (c == '$') {
            _rx_mode = 1;
        } else {
            return;  /* 忽略帧起始符以外的字符 */
        }
    }

    /* 存入缓冲区 */
    if (_rx_index < RX_BUF_SIZE - 1) {
        _rx_buf[_rx_index++] = c;
    } else {
        _rx_reset();
        return;
    }

    /* 检测帧结束 '!' */
    if (_rx_mode == 1 && c == '!') {
        _rx_buf[_rx_index] = '\0';
        /* 复制到帧缓冲区供主循环处理 */
        strncpy(_frame_buf, _rx_buf, FRAME_BUF_SIZE - 1);
        _frame_buf[FRAME_BUF_SIZE - 1] = '\0';
        _frame_ready = 1;
        _rx_reset();
    }
}

/*!
    \brief      通过 UART3 发送一个字节
*/
static void _send_byte(char c)
{
    while (RESET == usart_flag_get(ROBOT_UART3_PERIPH, USART_FLAG_TBE)) {
        /* wait for TX buffer empty */
    }
    usart_data_transmit(ROBOT_UART3_PERIPH, (uint8_t)c);
}

/*!
    \brief      通过 UART3 发送字符串
*/
static void _send_str(const char *s)
{
    while (s && *s) {
        _send_byte(*s++);
    }
}

/*!
    \brief      发送完整帧: "$CMD!\r\n"
*/
static void _send_frame(const char *cmd)
{
    _send_byte('$');
    _send_str(cmd);
    _send_byte('!');
    _send_byte('\r');
    _send_byte('\n');
}

/* ──────────────────────────────────────────────
 * 帧解析
 * ────────────────────────────────────────────── */

static void _parse_frame(const char *frame)
{
    if (!frame || frame[0] != '$') return;

    /* 跳过 '$' */
    frame++;

    /* ── 简单响应（无参数）── */
    if (strncmp(frame, "OK!", 3) == 0) {
        _resp_type = RESP_OK;
        return;
    }
    if (strncmp(frame, "BUSY!", 5) == 0) {
        _resp_type = RESP_BUSY;
        return;
    }
    if (strncmp(frame, "PONG!", 5) == 0) {
        _resp_type = RESP_PONG;
        _comm_fault = 0;
        _last_pong_ms = driver_tick;
        return;
    }
    if (strncmp(frame, "FLIP_START!", 11) == 0) {
        _resp_type = RESP_FLIP_START;
        _busy_state = 1;
        _seq_type_char = 'F';
        _step_state = 1;
        return;
    }
    if (strncmp(frame, "BAD_START!", 10) == 0) {
        _resp_type = RESP_BAD_START;
        _busy_state = 1;
        _seq_type_char = 'B';
        _step_state = 1;
        return;
    }
    if (strncmp(frame, "STOPPED!", 8) == 0) {
        _resp_type = RESP_STOPPED;
        _busy_state = 0;
        _step_state = 0;
        _seq_type_char = 'I';
        return;
    }

    /* ── 主动上报 ── */
    /* $DONE! */
    if (strncmp(frame, "DONE!", 5) == 0) {
        _resp_type = RESP_DONE;
        _busy_state = 0;
        _step_state = 0;
        _seq_type_char = 'I';
        return;
    }

    /* $SENSOR:1! 或 $SENSOR:0! */
    if (strncmp(frame, "SENSOR:", 7) == 0) {
        if (frame[7] == '1') {
            _sensor_state = 1;
        } else {
            _sensor_state = 0;
        }
        return;
    }

    /* $STATUS:X,N!  — X={I,F,B}, N=step */
    if (strncmp(frame, "STATUS:", 7) == 0) {
        _seq_type_char = frame[7];
        if (frame[8] == ',' && frame[9] >= '0' && frame[9] <= '9') {
            _step_state = (uint8_t)(frame[9] - '0');
        }
        if (_seq_type_char == 'I') {
            _busy_state = 0;
            _step_state = 0;
        } else {
            _busy_state = 1;
        }
        return;
    }
}

/* ──────────────────────────────────────────────
 * 命令发送与响应等待
 * ────────────────────────────────────────────── */

/*!
    \brief      发送命令并等待响应
    \param[in]  cmd         命令字符串（不含 $ 和 !）
    \param[in]  expected1   期望的响应类型
    \param[in]  expected2   备选响应类型（RESP_NONE 表示不关心）
    \param[in]  timeout_ms  超时（毫秒）
    \retval     1 = 收到期望响应, 0 = 超时或收到其他响应
*/
static uint8_t _wait_response(resp_type_t expected1, resp_type_t expected2,
                              uint32_t timeout_ms)
{
    uint32_t deadline = driver_tick + timeout_ms;

    _resp_type = RESP_NONE;

    while (driver_tick < deadline) {
        /* 让主循环有机会处理 UART3 接收数据 */
        remote_io_process();

        if (_resp_type == expected1 || _resp_type == expected2) {
            return 1;
        }

        /* 收到 BUSY 也算有效响应（命令被拒绝）*/
        if (_resp_type == RESP_BUSY && expected2 == RESP_BUSY) {
            return 1;
        }

        if (_resp_type != RESP_NONE && _resp_type != expected1
            && _resp_type != expected2) {
            /* 收到意外响应，继续等待 */
        }
    }

    /* 超时 */
    _resp_type = RESP_NONE;
    return 0;
}

/*!
    \brief      带重试的命令发送
    \param[in]  cmd       命令字符串
    \param[in]  exp1      期望响应1
    \param[in]  exp2      期望响应2
    \retval     1 = 成功, 0 = 失败
*/
static uint8_t _send_command(const char *cmd, resp_type_t exp1, resp_type_t exp2)
{
    if (!_initialized) return 0;

    for (uint8_t retry = 0; retry < MAX_RETRIES; retry++) {
        _send_frame(cmd);

        if (_wait_response(exp1, exp2, CMD_TIMEOUT_MS)) {
            return 1;
        }
    }

    /* 所有重试均失败 */
    _comm_fault = 1;
    return 0;
}

/* ──────────────────────────────────────────────
 * 公共 API
 * ────────────────────────────────────────────── */

void remote_io_init(void)
{
    /* 使用 uart3_init 初始化 UART3 硬件（PD8 TX, PD9 RX, 115200） */
    extern void uart3_init(uint32_t baud);
    uart3_init(115200);

    _rx_reset();
    _frame_ready = 0;

    _sensor_state   = 0;
    _busy_state     = 0;
    _step_state     = 0;
    _seq_type_char  = 'I';
    _comm_fault     = 0;
    _resp_type      = RESP_NONE;

    _initialized = 1;

    /* 初始同步：发送 PING，等待 XMC7100 响应 */
    _last_ping_ms = driver_tick;
    _last_pong_ms = 0;

    for (uint8_t retry = 0; retry < 10; retry++) {
        _send_frame("PING");
        if (_wait_response(RESP_PONG, RESP_NONE, 500)) {
            _comm_fault = 0;
            /* 同步完成：发送 RESET 建立已知状态 */
            _send_command("RESET", RESP_STOPPED, RESP_NONE);
            return;
        }
    }

    /* XMC7100 无响应 — 通信故障 */
    _comm_fault = 1;
}

void remote_io_process(void)
{
    if (!_initialized) return;

    /* ── 处理接收到的完整帧 ── */
    if (_frame_ready) {
        _parse_frame(_frame_buf);
        _frame_ready = 0;
    }

    /* ── 心跳：每 PING_INTERVAL_MS 发送 PING ── */
    if (driver_tick - _last_ping_ms >= PING_INTERVAL_MS) {
        _last_ping_ms = driver_tick;
        _send_frame("PING");
    }

    /* ── 心跳超时检测 ── */
    if (_last_pong_ms > 0 && (driver_tick - _last_pong_ms) > PING_TIMEOUT_MS) {
        _comm_fault = 1;
    }
}

/* ── 电机控制 ── */

void remote_io_motor_on(void)
{
    if (_comm_fault) return;
    _send_command("MOTOR_ON", RESP_OK, RESP_NONE);
}

void remote_io_motor_off(void)
{
    if (_comm_fault) return;
    _send_command("MOTOR_OFF", RESP_OK, RESP_NONE);
}

/* ── 机械臂动作 ── */

uint8_t remote_io_start_flip(void)
{
    if (_comm_fault) return 0;
    return _send_command("FLIP", RESP_FLIP_START, RESP_BUSY);
}

uint8_t remote_io_push_defect(void)
{
    if (_comm_fault) return 0;
    return _send_command("BAD", RESP_BAD_START, RESP_BUSY);
}

void remote_io_reset(void)
{
    if (_comm_fault) {
        /* 通信故障时尝试恢复 */
        _send_frame("RESET");
        return;
    }
    _send_command("RESET", RESP_STOPPED, RESP_NONE);
}

/* ── 状态查询（返回缓存值）── */

uint8_t remote_io_is_busy(void)
{
    return _busy_state;
}

uint8_t remote_io_get_step(void)
{
    return _step_state;
}

uint8_t remote_io_get_sensor(void)
{
    return _sensor_state;
}

char remote_io_get_seq_type(void)
{
    return _seq_type_char;
}

/* ── 通信健康 ── */

uint8_t remote_io_ping(void)
{
    if (!_initialized) return 0;

    _resp_type = RESP_NONE;
    _send_frame("PING");

    if (_wait_response(RESP_PONG, RESP_NONE, 500)) {
        _comm_fault = 0;
        return 1;
    }

    _comm_fault = 1;
    return 0;
}

uint8_t remote_io_is_ok(void)
{
    return _initialized && !_comm_fault;
}
