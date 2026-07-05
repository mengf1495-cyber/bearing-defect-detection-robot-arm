#include "robot_arm_entry.h"
#include "robot_arm_port.h"
#include "robot_arm_time.h"
#include "y_delay.h"

/*
 * app_sequence 模块已迁移至 XMC7100。
 * 以下 weak 桩函数用于维持旧固件编译兼容。
 */
__attribute__((weak)) void app_sequence_run(void) {}
__attribute__((weak)) uint8_t app_sequence_is_busy(void) { return 0; }

/* Link the legacy application files to enable these real implementations. */
__attribute__((weak)) void app_ps2_run(void){}
__attribute__((weak)) void app_uart_run(void){}
__attribute__((weak)) void app_action_run(void){}
__attribute__((weak)) void app_sensor_run(void){}
__attribute__((weak)) void app_ps2_init(void){}
__attribute__((weak)) void app_sensor_init(void){}
__attribute__((weak)) void parameter_init(void){}
__attribute__((weak)) void setup_kinematics(float a,float b,float c,float d,void*e)
{
    (void)a;
    (void)b;
    (void)c;
    (void)d;
    (void)e;
}

void RobotArm_SystemInit(void)
{
    robot_arm_hw_init();
    app_ps2_init();
    parameter_init();
    app_sensor_init();
}

void RobotArm_Loop(void)
{
    static uint32_t led_ms = 0U;

    /*
     * ????????????
     */
    app_sequence_run();

    /*
     * ?? USART1 ??????????
     * ?????????,????????????
     */
    app_uart_run();

    /*
     * ?????????,?????????
     */
    if (!app_sequence_is_busy())
    {
        app_ps2_run();
        app_action_run();
        app_sensor_run();
    }

    if ((uint32_t)(millis() - led_ms) >= 1000U)
    {
        led_ms = millis();
        robot_led_toggle();
    }
}