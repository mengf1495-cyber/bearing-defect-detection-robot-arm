#include "robot_arm_soft_i2c.h"
#include "robot_arm_board.h"
#include "robot_arm_gpio.h"
#include "robot_arm_time.h"
#include "y_delay.h"

static inline void i2c_delay(void) { delay_us(ROBOT_I2C_HALF_PERIOD_US); }
static inline void scl_high(void) { gpio_bit_set(ROBOT_I2C_SCL_PORT, ROBOT_I2C_SCL_PIN); }
static inline void scl_low(void)  { gpio_bit_reset(ROBOT_I2C_SCL_PORT, ROBOT_I2C_SCL_PIN); }
static inline void sda_high(void) { gpio_bit_set(ROBOT_I2C_SDA_PORT, ROBOT_I2C_SDA_PIN); }
static inline void sda_low(void)  { gpio_bit_reset(ROBOT_I2C_SDA_PORT, ROBOT_I2C_SDA_PIN); }
static inline uint8_t sda_read(void) { return gpio_input_bit_get(ROBOT_I2C_SDA_PORT, ROBOT_I2C_SDA_PIN) ? 1U : 0U; }

void soft_i2c_gpio_init(void)
{
    robot_gpio_output_od(ROBOT_I2C_SCL_PORT, ROBOT_I2C_SCL_RCU, ROBOT_I2C_SCL_PIN, SET);
    robot_gpio_output_od(ROBOT_I2C_SDA_PORT, ROBOT_I2C_SDA_RCU, ROBOT_I2C_SDA_PIN, SET);
    scl_high(); sda_high();
}

void i2c_start(void)
{
    sda_high(); scl_high(); i2c_delay();
    sda_low();  i2c_delay();
    scl_low();
}

void i2c_stop(void)
{
    sda_low();  i2c_delay();
    scl_high(); i2c_delay();
    sda_high(); i2c_delay();
}

uint8_t i2c_wait_ack(void)
{
    uint32_t timeout = 200U;
    sda_high();
    scl_high();
    i2c_delay();
    while (sda_read()) {
        if (timeout-- == 0U) {
            scl_low();
            i2c_stop();
            return 1U;
        }
        delay_us(1U);
    }
    scl_low();
    return 0U;
}

void i2c_ack(void)
{
    scl_low(); sda_low(); i2c_delay();
    scl_high(); i2c_delay();
    scl_low(); sda_high();
}

void i2c_nack(void)
{
    scl_low(); sda_high(); i2c_delay();
    scl_high(); i2c_delay();
    scl_low();
}

void i2c_write_byte(uint8_t txd)
{
    uint8_t i;
    scl_low();
    for (i = 0U; i < 8U; ++i) {
        if (txd & 0x80U) sda_high(); else sda_low();
        txd <<= 1;
        i2c_delay(); scl_high(); i2c_delay(); scl_low();
    }
    sda_high();
}

uint8_t i2c_read_byte(uint8_t ack)
{
    uint8_t i, value = 0U;
    sda_high();
    for (i = 0U; i < 8U; ++i) {
        value <<= 1;
        scl_high(); i2c_delay();
        if (sda_read()) value |= 1U;
        scl_low(); i2c_delay();
    }
    if (ack) i2c_ack(); else i2c_nack();
    return value;
}
