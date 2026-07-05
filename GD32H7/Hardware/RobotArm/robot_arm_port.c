#include "robot_arm_port.h"
#include "robot_arm_board.h"
#include "robot_arm_gpio.h"
#include "robot_arm_time.h"
#include "robot_arm_soft_i2c.h"
#include "robot_arm_uart.h"
#include "robot_arm_flash.h"
#include "robot_arm_adc.h"
#include "robot_arm_ultrasonic.h"
#include "robot_arm_buzzer.h"
#include "robot_arm_ps2.h"
void robot_arm_hw_init(void){robot_time_init();robot_gpio_output_pp(ROBOT_LED_PORT,ROBOT_LED_RCU,ROBOT_LED_PIN,RESET);robot_gpio_input_pullup(ROBOT_SOUND_PORT,ROBOT_SOUND_RCU,ROBOT_SOUND_PIN);robot_gpio_input_pullup(ROBOT_TOUCH_PORT,ROBOT_TOUCH_RCU,ROBOT_TOUCH_PIN);robot_gpio_input_pullup(ROBOT_IR_PORT,ROBOT_IR_RCU,ROBOT_IR_PIN);beep_init();soft_i2c_gpio_init();usb_ps2_Init();uart1_init(115200);uart2_init(115200);spi_flash_init();ADC_Voltage_Init();ultrasonic_sensor_init();}
void robot_arm_background_1ms(void){robot_time_tick_1ms();}
uint8_t robot_sound_read(void){return gpio_input_bit_get(ROBOT_SOUND_PORT,ROBOT_SOUND_PIN)?1U:0U;}
uint8_t robot_touch_read(void){return gpio_input_bit_get(ROBOT_TOUCH_PORT,ROBOT_TOUCH_PIN)?1U:0U;}
uint8_t robot_ir_read(void){return gpio_input_bit_get(ROBOT_IR_PORT,ROBOT_IR_PIN)?1U:0U;}
void robot_led_toggle(void){gpio_bit_toggle(ROBOT_LED_PORT,ROBOT_LED_PIN);}
