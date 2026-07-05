#include "robot_arm_ps2.h"
#include "robot_arm_board.h"
#include "robot_arm_gpio.h"
#include "robot_arm_time.h"
#include "y_delay.h"

uint8_t ps2_buf[9];
static inline void dly(void) { delay_us(ROBOT_PS2_HALF_PERIOD_US); }
static inline void scl_h(void){ gpio_bit_set(ROBOT_PS2_SCL_PORT, ROBOT_PS2_SCL_PIN); }
static inline void scl_l(void){ gpio_bit_reset(ROBOT_PS2_SCL_PORT, ROBOT_PS2_SCL_PIN); }
static inline void sda_h(void){ gpio_bit_set(ROBOT_PS2_SDA_PORT, ROBOT_PS2_SDA_PIN); }
static inline void sda_l(void){ gpio_bit_reset(ROBOT_PS2_SDA_PORT, ROBOT_PS2_SDA_PIN); }
static inline uint8_t sda_r(void){ return gpio_input_bit_get(ROBOT_PS2_SDA_PORT, ROBOT_PS2_SDA_PIN) ? 1U : 0U; }

static void start(void){ sda_h(); scl_h(); dly(); sda_l(); dly(); scl_l(); }
static void stop(void){ sda_l(); dly(); scl_h(); dly(); sda_h(); dly(); }
static void send_byte(uint8_t v){ uint8_t i; for(i=0;i<8;i++){ if(v&0x80U)sda_h();else sda_l(); dly(); scl_h(); dly(); scl_l(); v<<=1; } sda_h(); }
static uint8_t recv_byte(uint8_t ack){ uint8_t i,v=0; sda_h(); for(i=0;i<8;i++){ v<<=1; scl_h(); dly(); if(sda_r())v|=1U; scl_l(); dly(); } if(ack)sda_l();else sda_h(); scl_h(); dly(); scl_l(); sda_h(); return v; }
static uint8_t wait_ack(void){ uint16_t n=500; sda_h(); scl_h(); while(sda_r() && n--) delay_us(1); scl_l(); return n?0U:1U; }

uint8_t usb_ps2_Init(void)
{
    robot_gpio_output_od(ROBOT_PS2_SCL_PORT, ROBOT_PS2_SCL_RCU, ROBOT_PS2_SCL_PIN, SET);
    robot_gpio_output_od(ROBOT_PS2_SDA_PORT, ROBOT_PS2_SDA_RCU, ROBOT_PS2_SDA_PIN, SET);
    scl_h(); sda_h();
    return 1U;
}

void ps2_write_read(void)
{
    static const uint8_t cmd[9] = {0x01,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    uint8_t i;
    start();
    for(i=0;i<9;i++){
        send_byte(cmd[i]);
        (void)wait_ack();
        ps2_buf[i]=recv_byte(i<8U);
    }
    stop();
}

uint8_t ps2_is_connected(void)
{
    return (ps2_buf[0] == 0x01U) ? 1U : 0U;
}
