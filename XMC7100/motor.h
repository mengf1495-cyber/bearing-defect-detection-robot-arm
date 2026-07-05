/*!
    \file    motor.h
    \brief   传送带直流电机控制 (PWMA/AIN1/AIN2)
    \version 2025-07-04
*/

#ifndef MOTOR_H
#define MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* 初始化电机 GPIO (PWMA=PA0, AIN1=PH13, AIN2=PH15) */
void motor_init(void);

/* 启动电机正转: PWMA=H, AIN1=H, AIN2=L */
void motor_on(void);

/* 停止电机: 全部低电平 */
void motor_off(void);

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_H */
