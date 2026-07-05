#include "robot_arm_buzzer.h"
#include "robot_arm_board.h"
#include "robot_arm_gpio.h"
#include "y_delay.h"
#include "robot_arm_time.h"
void beep_init(void){robot_gpio_output_pp(ROBOT_BUZZER_PORT,ROBOT_BUZZER_RCU,ROBOT_BUZZER_PIN,RESET);}
void beep_on(int times,int gap_ms,int half){int i,j;if(half<50)half=500;for(i=0;i<times;i++){for(j=0;j<100000/half;j++){gpio_bit_set(ROBOT_BUZZER_PORT,ROBOT_BUZZER_PIN);delay_us((uint32_t)half);gpio_bit_reset(ROBOT_BUZZER_PORT,ROBOT_BUZZER_PIN);delay_us((uint32_t)half);}delay_ms((uint32_t)gap_ms);}}
