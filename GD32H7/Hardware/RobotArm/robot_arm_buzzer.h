#ifndef ROBOT_ARM_BUZZER_H
#define ROBOT_ARM_BUZZER_H
#include <stdint.h>
void beep_init(void); void beep_on(int times,int gap_ms,int half_period_us);
#endif
