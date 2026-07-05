#include "robot_arm_ultrasonic.h"
#include "robot_arm_board.h"
#include "robot_arm_gpio.h"
#include "robot_arm_time.h"
#include "y_delay.h"

void ultrasonic_sensor_init(void){robot_gpio_output_pp(ROBOT_US_TRIG_PORT,ROBOT_US_TRIG_RCU,ROBOT_US_TRIG_PIN,RESET);robot_gpio_input_floating(ROBOT_US_ECHO_PORT,ROBOT_US_ECHO_RCU,ROBOT_US_ECHO_PIN);}
float ultrasonic_distance_read(void){uint32_t t0,t1;gpio_bit_reset(ROBOT_US_TRIG_PORT,ROBOT_US_TRIG_PIN);delay_us(3);gpio_bit_set(ROBOT_US_TRIG_PORT,ROBOT_US_TRIG_PIN);delay_us(12);gpio_bit_reset(ROBOT_US_TRIG_PORT,ROBOT_US_TRIG_PIN);t0=micros();while(!gpio_input_bit_get(ROBOT_US_ECHO_PORT,ROBOT_US_ECHO_PIN)){if((uint32_t)(micros()-t0)>30000U)return -1.0f;}t0=micros();while(gpio_input_bit_get(ROBOT_US_ECHO_PORT,ROBOT_US_ECHO_PIN)){if((uint32_t)(micros()-t0)>30000U)return -1.0f;}t1=micros();return (float)(t1-t0)*0.017f;}
