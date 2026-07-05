#ifndef ROBOT_ARM_SOFT_I2C_H
#define ROBOT_ARM_SOFT_I2C_H
#include <stdint.h>
void soft_i2c_gpio_init(void);
void i2c_start(void);
void i2c_stop(void);
uint8_t i2c_wait_ack(void);
void i2c_ack(void);
void i2c_nack(void);
void i2c_write_byte(uint8_t txd);
uint8_t i2c_read_byte(uint8_t ack);
#endif
