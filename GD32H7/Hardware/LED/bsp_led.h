#ifndef BSP_LED_H
#define BSP_LED_H

#include "driver_public.h"
#include "driver_gpio.h"

#define LED_DEF(name, port, pin,default_state)    \
    GPIO_DEF(name,port,pin,OUT_PP,default_state,NULL) 
    
extern const void* LED_INIT_GROUP[];  
extern const uint8_t LED_SIZE;
    
GPIO_DEF_EXTERN(CAM_LED1);
GPIO_DEF_EXTERN(CAM_LED2);
GPIO_DEF_EXTERN(CAM_LED3);
GPIO_DEF_EXTERN(CAM_LED4);

void bsp_led_init(  typdef_gpio_general *LEDx);
void bsp_led_group_init(void); 
void bsp_led_on(  typdef_gpio_general *LEDx);
void bsp_led_off(  typdef_gpio_general *LEDx);
void bsp_led_toggle(  typdef_gpio_general *LEDx);
#endif /* BSP_LED_H*/
