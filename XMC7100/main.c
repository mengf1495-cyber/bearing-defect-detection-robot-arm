/*!
    \file    main.c
    \brief   XMC7100 电机与机械臂控制器 — 主程序

    角色：
    - 接收 GD32H7 通过 UART3 发送的命令帧 ($XXX!)
    - 控制传送带电机 (PWMA/AIN1/AIN2 GPIO)
    - 读取 NPN 激光传感器（PB0, 低电平有效）
    - 通过 UART1 发送舵机动作指令到外部总线舵机控制器
    - 执行 9 步 FLIP/9 步 BAD 动作序列

    状态机: INIT -> IDLE -> FLIPPING/REJECTING -> SAFE_MODE

    \version 2025-07-04
*/

#include "hal_platform.h"
#include "proto_parser.h"
#include "proto_responder.h"
#include "sequence_engine.h"
#include "motor.h"
#include "sensor.h"
#include "servo_bus.h"

/* ── 外设句柄 ── */
#define UART_GD32    ((hal_uart_handle_t)0)   /* UART3 → GD32H7 */
#define UART_SERVO   ((hal_uart_handle_t)1)   /* UART1 → 舵机控制器 */

/* ── 系统状态 ── */
typedef enum {
    STATE_INIT,
    STATE_IDLE,
    STATE_FLIPPING,
    STATE_REJECTING,
    STATE_SAFE_MODE
} sys_state_t;

static sys_state_t _state = STATE_INIT;

/* ── 心跳 ── */
static uint32_t _last_ping_ms = 0;
#define PING_TIMEOUT_MS  3000  /* 3秒无PING → 安全模式 */

/* ── 传感器边缘检测 ── */
static uint8_t _last_sensor = 0;
static uint8_t _sensor_debounce_cnt = 0;
#define SENSOR_DEBOUNCE_MS  5

/* ── 前向声明 ── */
static void _cmd_handler(const char *cmd, void *ctx);
static void _enter_safe_mode(void);

/* ──────────────────────────────────────────────
 * 主函数
 * ────────────────────────────────────────────── */

int main(void)
{
    /* 1. 初始化硬件 */
    hal_timer_init();

    /* 电机: PWMA=PA0, AIN1=PH13, AIN2=PH15 */
    motor_init();

    /* 传感器: PB0, 输入上拉 */
    sensor_init();

    /* UART1: 舵机控制器 (115200) */
    servo_bus_init(115200);

    /* UART3: GD32H7 通信 (115200) — 具体引脚在 hal_uart_init 中配置 */
    /* TX→GD32 PD9(RX), RX←GD32 PD8(TX) */
    hal_uart_init(UART_GD32, 115200,
                  /* TX_PORT */ 0, /* TX_PIN */ 0,   /* 替换为实际引脚 */
                  /* RX_PORT */ 0, /* RX_PIN */ 0);  /* 替换为实际引脚 */

    /* 2. 初始化协议层 */
    proto_parser_init();
    proto_responder_init(UART_GD32);
    proto_parser_register_handler(_cmd_handler, NULL);

    /* 3. 进入 IDLE */
    _state = STATE_IDLE;
    _last_ping_ms = hal_timer_millis();
    _last_sensor = sensor_is_triggered();

    /* 4. 主循环 */
    while (1)
    {
        /* ── Feed watchdog ── */
        /* TODO: 使能 XMC7100 内部看门狗 */

        /* ── 处理接收到的命令 ── */
        proto_parser_process(UART_GD32);

        /* ── 传感器边缘检测（去抖后主动上报）── */
        {
            uint8_t raw = sensor_is_triggered();

            if (raw != _last_sensor) {
                _sensor_debounce_cnt++;
                if (_sensor_debounce_cnt >= SENSOR_DEBOUNCE_MS) {
                    _last_sensor = raw;
                    _sensor_debounce_cnt = 0;

                    if (raw) {
                        proto_responder_send("SENSOR:1");
                    } else {
                        proto_responder_send("SENSOR:0");
                    }
                }
            } else {
                _sensor_debounce_cnt = 0;
            }
        }

        /* ── 动作序列推进 ── */
        switch (_state) {
        case STATE_FLIPPING:
        case STATE_REJECTING:
            if (sequence_engine_run()) {
                /* 序列执行完成 */
                proto_responder_send("DONE");
                _state = STATE_IDLE;
            }
            break;

        case STATE_SAFE_MODE:
            /* 等待恢复通信 */
            break;

        default:
            break;
        }

        /* ── 心跳超时检测 ── */
        if (_state != STATE_SAFE_MODE && _state != STATE_INIT) {
            uint32_t now = hal_timer_millis();
            if (now - _last_ping_ms > PING_TIMEOUT_MS) {
                _enter_safe_mode();
            }
        }
    }

    return 0;
}

/* ──────────────────────────────────────────────
 * 命令处理
 * ────────────────────────────────────────────── */

static void _cmd_handler(const char *cmd, void *ctx)
{
    (void)ctx;

    if (!cmd) return;

    /* ── PING 心跳 ── */
    if (str_cmp(cmd, "PING")) {
        _last_ping_ms = hal_timer_millis();

        if (_state == STATE_SAFE_MODE) {
            /* 恢复通信 */
            _state = STATE_IDLE;
        }

        proto_responder_send("PONG");
        return;
    }

    /* ── 安全模式下忽略其他命令 ── */
    if (_state == STATE_SAFE_MODE) {
        return;
    }

    /* ── MOTOR_ON ── */
    if (str_cmp(cmd, "MOTOR_ON")) {
        motor_on();
        proto_responder_send("OK");
        return;
    }

    /* ── MOTOR_OFF ── */
    if (str_cmp(cmd, "MOTOR_OFF")) {
        motor_off();
        proto_responder_send("OK");
        return;
    }

    /* ── FLIP ── */
    if (str_cmp(cmd, "FLIP")) {
        if (sequence_engine_is_busy()) {
            proto_responder_send("BUSY");
            return;
        }

        /* 翻面时电机必须停止 */
        motor_off();

        if (sequence_engine_start(SEQ_FLIP)) {
            _state = STATE_FLIPPING;
            proto_responder_send("FLIP_START");
        } else {
            proto_responder_send("ERR");
        }
        return;
    }

    /* ── BAD ── */
    if (str_cmp(cmd, "BAD")) {
        if (sequence_engine_is_busy()) {
            proto_responder_send("BUSY");
            return;
        }

        /* 剔除时电机必须停止 */
        motor_off();

        if (sequence_engine_start(SEQ_BAD)) {
            _state = STATE_REJECTING;
            proto_responder_send("BAD_START");
        } else {
            proto_responder_send("ERR");
        }
        return;
    }

    /* ── RESET ── */
    if (str_cmp(cmd, "RESET")) {
        sequence_engine_stop();
        motor_off();
        servo_bus_send_str("$DST!\r\n");  /* 停止所有舵机 */
        _state = STATE_IDLE;
        proto_responder_send("STOPPED");
        return;
    }

    /* ── STATUS? ── */
    if (str_cmp(cmd, "STATUS?")) {
        char status_str[16];
        char type_char = 'I';
        uint8_t step = 0;

        if (sequence_engine_is_busy()) {
            step = sequence_engine_get_step();
            if (_state == STATE_FLIPPING) {
                type_char = 'F';
            } else if (_state == STATE_REJECTING) {
                type_char = 'B';
            }
        }

        /* 构建 "$STATUS:X,N!" */
        /* proto_responder_send_formatted 会添加 $ 和 ! */
        int len = snprintf(status_str, sizeof(status_str),
                          "STATUS:%c,%u", type_char, (unsigned)step);
        (void)len;
        proto_responder_send(status_str);
        return;
    }
}

/* ──────────────────────────────────────────────
 * 安全模式
 * ────────────────────────────────────────────── */

static void _enter_safe_mode(void)
{
    _state = STATE_SAFE_MODE;

    /* 停止所有运动 */
    sequence_engine_stop();
    motor_off();
    servo_bus_send_str("$DST!\r\n");
}

/* ──────────────────────────────────────────────
 * 字符串比较辅助
 * ────────────────────────────────────────────── */

#include <string.h>

int str_cmp(const char *a, const char *b)
{
    if (!a || !b) return 0;
    while (*a && *b && *a == *b) { a++; b++; }
    return (*a == *b) ? 1 : 0;
}
