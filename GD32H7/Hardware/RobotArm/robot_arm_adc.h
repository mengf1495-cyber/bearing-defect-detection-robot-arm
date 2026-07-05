#ifndef ROBOT_ARM_ADC_H
#define ROBOT_ARM_ADC_H
#include <stdint.h>
void ADC_Voltage_Init(void); uint16_t ADC_Get_RawValue(void); float ADC_Calc_InputVoltage(void);
#endif
