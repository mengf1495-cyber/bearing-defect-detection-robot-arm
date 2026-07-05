#include "bsp_led.h"
#include "driver_gpio.h"


/* LED����ע�� */
LED_DEF(CAM_LED1,E,2,RESET);
LED_DEF(CAM_LED2,E,5,RESET);
const void* LED_INIT_GROUP[]={&CAM_LED1,&CAM_LED2};

const uint8_t LED_SIZE=sizeof(LED_INIT_GROUP)/sizeof(void*);


/*!
* ˵��     LED��ʼ������
* ����[1]  LEDx��LED�ṹ��ָ�� @LED0/LED1
* ����ֵ   ��
*/
void bsp_led_init(typdef_gpio_general *LEDx)
{
    driver_gpio_general_init(LEDx);
}
/*!
* ˵��     ��ʼ������ע���LED
* ����     ��
* ����ֵ   ��
*/
void bsp_led_group_init(void)
{
    uint8_t i;
    for(i=0;i<LED_SIZE;i++)
    {
        bsp_led_init(((typdef_gpio_general *)LED_INIT_GROUP[i]));    
    }
}


/*!
* ˵��     ����LED����
* ����[1]  LEDx��LED�ṹ��ָ�� @LED0/LED1
* ����ֵ   ��
*/
void bsp_led_on(typdef_gpio_general *LEDx)
{
    driver_gpio_pin_write(LEDx,(bit_status)!(LEDx->default_state));
}

/*!
* ˵��     Ϩ��LED����
* ����[1]  LEDx��LED�ṹ��ָ�� @LED0/LED1
* ����ֵ   ��
*/
void bsp_led_off(typdef_gpio_general *LEDx)
{
    driver_gpio_pin_write(LEDx,LEDx->default_state);
}

/*!
* ˵��     ��תLED����
* ����[1]  LEDx��LED�ṹ��ָ�� @LED0/LED1
* ����ֵ   ��
*/
void bsp_led_toggle(typdef_gpio_general *LEDx)
{
    driver_gpio_pin_toggle(LEDx);
}
