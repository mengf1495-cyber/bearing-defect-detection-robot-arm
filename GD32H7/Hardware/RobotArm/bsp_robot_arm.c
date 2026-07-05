/*!
    \file    bsp_robot_arm.c
    \brief   机械臂驱动统一入口 - 双芯片架构：GD32H7 通过 UART3 发送命令给 XMC7100

    架构变更 (2025-07-04):
    - GD32H7 (AI推理):  通过 remote_io 模块与 XMC7100 通信
    - XMC7100 (控制):   电机控制 + UART1→舵机控制器 + 动作序列引擎

    GD32H7 不再直接控制 UART1、电机GPIO、传感器GPIO。
    所有控制命令通过 remote_io 模块经 UART3 发送给 XMC7100。

    \version 2025-07-04
*/

#include "bsp_robot_arm.h"
#include "remote_io.h"

static uint8_t initialized = 0;

void bsp_robot_arm_init(void)
{
    if (initialized) return;

    /*
     * UART1（舵机控制器通信）和动作序列引擎已迁移至 XMC7100。
     * remote_io_init() 在 main.c 的 System_Init() 中调用，
     * 负责初始化 UART3 并与 XMC7100 建立通信。
     */
    initialized = 1;
}

void bsp_robot_arm_start_flip(void)
{
    if (!initialized) return;
    remote_io_start_flip();
}

void bsp_robot_arm_push_defect(void)
{
    if (!initialized) return;
    remote_io_push_defect();
}

void bsp_robot_arm_reset(void)
{
    if (!initialized) return;
    remote_io_reset();
}

void bsp_robot_arm_process(void)
{
    if (!initialized) return;
    remote_io_process();
}

uint8_t bsp_robot_arm_is_busy(void)
{
    if (!initialized) return 0;
    return remote_io_is_busy();
}

bsp_arm_seq_type_t bsp_robot_arm_get_seq_type(void)
{
    if (!initialized) return ARM_SEQ_NONE;

    char t = remote_io_get_seq_type();
    if (t == 'F') return ARM_SEQ_FLIP;
    if (t == 'B') return ARM_SEQ_BAD;
    return ARM_SEQ_NONE;
}

uint8_t bsp_robot_arm_get_step(void)
{
    if (!initialized) return 0;
    return remote_io_get_step();
}
