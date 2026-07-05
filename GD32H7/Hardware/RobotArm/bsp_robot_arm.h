/*!
    \file    bsp_robot_arm.h
    \brief   机械臂驱动统一入口 - 双芯片架构：GD32H7(AI) + XMC7100(控制)

    架构 (2025-07-04):
    - GD32H7 通过 remote_io 模块经 UART3 发送命令给 XMC7100
    - XMC7100 执行电机控制、传感器读取和舵机动作序列
    - GD32H7 不再直接控制 GPIO、UART1 或 TIMER6 PWM

    \version 2025-07-04
*/

#ifndef BSP_ROBOT_ARM_H
#define BSP_ROBOT_ARM_H

#include "driver_public.h"

/* 机械臂动作序列类型 */
typedef enum {
    ARM_SEQ_NONE = 0,
    ARM_SEQ_FLIP = 1,   /* 翻面 (10步) */
    ARM_SEQ_BAD  = 2    /* 剔除 (8步) */
} bsp_arm_seq_type_t;

#ifdef __cplusplus
extern "C" {
#endif

/* 机械臂硬件初始化（GPIO、TIMER6、Servo、UART等） */
void bsp_robot_arm_init(void);

/* 启动翻面动作序列 (10步, ~16s) */
void bsp_robot_arm_start_flip(void);

/* 启动剔除动作序列 (8步, ~13s) */
void bsp_robot_arm_push_defect(void);

/* 机械臂复位：回到待机位置 */
void bsp_robot_arm_reset(void);

/* 机械臂后台处理：在main循环中每次调用，推进动作序列 */
void bsp_robot_arm_process(void);

/* 查询机械臂是否正在执行动作 */
uint8_t bsp_robot_arm_is_busy(void);

/* 获取当前动作序列类型 */
bsp_arm_seq_type_t bsp_robot_arm_get_seq_type(void);

/* 获取当前动作序列步数 (1-based, 0=空闲) */
uint8_t bsp_robot_arm_get_step(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_ROBOT_ARM_H */
