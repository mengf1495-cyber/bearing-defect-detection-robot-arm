#ifndef ROBOT_ARM_GPIO_H
#define ROBOT_ARM_GPIO_H
#include "gd32h7xx.h"
void robot_gpio_output_pp(uint32_t port, rcu_periph_enum rcu, uint32_t pin, FlagStatus initial);
void robot_gpio_output_od(uint32_t port, rcu_periph_enum rcu, uint32_t pin, FlagStatus initial);
void robot_gpio_input_pullup(uint32_t port, rcu_periph_enum rcu, uint32_t pin);
void robot_gpio_input_floating(uint32_t port, rcu_periph_enum rcu, uint32_t pin);
#endif
