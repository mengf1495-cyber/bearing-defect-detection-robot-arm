#include "robot_arm_adc.h"
#include "robot_arm_board.h"
#include "gd32h7xx.h"
void ADC_Voltage_Init(void){rcu_periph_clock_enable(ROBOT_ADC_RCU);rcu_periph_clock_enable(ROBOT_ADC_RCU_PERIPH);gpio_mode_set(ROBOT_ADC_PORT,GPIO_MODE_ANALOG,GPIO_PUPD_NONE,ROBOT_ADC_PIN);adc_deinit(ROBOT_ADC_PERIPH);adc_resolution_config(ROBOT_ADC_PERIPH,ADC_RESOLUTION_12B);adc_data_alignment_config(ROBOT_ADC_PERIPH,ADC_DATAALIGN_RIGHT);adc_channel_length_config(ROBOT_ADC_PERIPH,ADC_REGULAR_CHANNEL,1);adc_regular_channel_config(ROBOT_ADC_PERIPH,0,ROBOT_ADC_CHANNEL,810U);adc_external_trigger_config(ROBOT_ADC_PERIPH,ADC_REGULAR_CHANNEL,EXTERNAL_TRIGGER_DISABLE);adc_enable(ROBOT_ADC_PERIPH);}
uint16_t ADC_Get_RawValue(void){adc_flag_clear(ROBOT_ADC_PERIPH,ADC_FLAG_EOC);adc_software_trigger_enable(ROBOT_ADC_PERIPH,ADC_REGULAR_CHANNEL);while(RESET==adc_flag_get(ROBOT_ADC_PERIPH,ADC_FLAG_EOC)){}return (uint16_t)adc_regular_data_read(ROBOT_ADC_PERIPH);}
float ADC_Calc_InputVoltage(void){uint32_t i,s=0;for(i=0;i<16;i++)s+=ADC_Get_RawValue();return ((float)s/16.0f/4095.0f)*ROBOT_ADC_VREF*ROBOT_ADC_DIVIDER_RATIO;}
