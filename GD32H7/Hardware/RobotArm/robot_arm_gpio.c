#include "robot_arm_gpio.h"

void robot_gpio_output_pp(uint32_t port, rcu_periph_enum rcu, uint32_t pin, FlagStatus initial)
{
    rcu_periph_clock_enable(rcu);
    gpio_mode_set(port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin);
    gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, pin);
    gpio_bit_write(port, pin, initial == SET ? SET : RESET);
}

void robot_gpio_output_od(uint32_t port, rcu_periph_enum rcu, uint32_t pin, FlagStatus initial)
{
    rcu_periph_clock_enable(rcu);
    gpio_mode_set(port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, pin);
    gpio_output_options_set(port, GPIO_OTYPE_OD, GPIO_OSPEED_60MHZ, pin);
    gpio_bit_write(port, pin, initial == SET ? SET : RESET);
}

void robot_gpio_input_pullup(uint32_t port, rcu_periph_enum rcu, uint32_t pin)
{
    rcu_periph_clock_enable(rcu);
    gpio_mode_set(port, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, pin);
}

void robot_gpio_input_floating(uint32_t port, rcu_periph_enum rcu, uint32_t pin)
{
    rcu_periph_clock_enable(rcu);
    gpio_mode_set(port, GPIO_MODE_INPUT, GPIO_PUPD_NONE, pin);
}
