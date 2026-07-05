// robot_arm_time.c
#include "robot_arm_time.h"
#include "gd32h7xx.h"

/* AI 项目已验证的 1ms 计数器 (driver_tic_inc 在 SysTick 中递增) */
extern volatile uint32_t driver_tick;

static volatile uint32_t g_robot_ms = 0;

void robot_time_init(void)
{
    SystemCoreClockUpdate();
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0U;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void robot_time_tick_1ms(void)
{
    ++g_robot_ms;
}

uint32_t robot_millis(void)
{
    return driver_tick;  /* 复用已验证的计数器 */
}

uint32_t robot_micros(void)      // ? ???(????????)
{
    return DWT->CYCCNT / (SystemCoreClock / 1000000UL);
}

void robot_delay_us(uint32_t us)  // ? ???
{
    const uint32_t cycles = (SystemCoreClock / 1000000UL) * us;
    const uint32_t start = DWT->CYCCNT;
    while ((uint32_t)(DWT->CYCCNT - start) < cycles) {
    }
}

void robot_delay_ms(uint32_t ms)  // ? ???
{
    while (ms--) {
        robot_delay_us(1000U);   // ? ???????
    }
}