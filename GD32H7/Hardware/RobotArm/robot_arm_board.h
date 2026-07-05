#ifndef ROBOT_ARM_BOARD_H
#define ROBOT_ARM_BOARD_H

#include "gd32h7xx.h"

/*
 * GD32H759IMK6 robot-arm default pin plan
 * --------------------------------------
 * This plan targets the 2x13 expansion header shown on the supplied interface
 * board / custom adapter board. It intentionally reuses several RGB-LCD pins.
 * Therefore the RGB LCD and its touch panel must be disabled while this robot
 * arm pin plan is active.
 *
 * All signals are 3.3 V GPIO unless explicitly stated otherwise.
 * Servo power must come from an external 5-6 V rail. Connect grounds together.
 */

#define ROBOT_ARM_TIMER_INPUT_HZ     300000000UL  /* SYSCLK=600MHz, AHB=300MHz, APB1=150MHz, timer x2 */
#define ROBOT_ARM_TIMER_TICK_HZ      1000000UL    /* 1 us timer base */
#define ROBOT_ARM_SERVO_FRAME_US     20000U
#define ROBOT_ARM_SERVO_COUNT        6U

/* Six GPIO software-PWM servo outputs. */
#define ROBOT_SERVO0_PORT GPIOE
#define ROBOT_SERVO0_PIN  GPIO_PIN_13
#define ROBOT_SERVO0_RCU  RCU_GPIOE
#define ROBOT_SERVO1_PORT GPIOE
#define ROBOT_SERVO1_PIN  GPIO_PIN_15
#define ROBOT_SERVO1_RCU  RCU_GPIOE
#define ROBOT_SERVO2_PORT GPIOB
#define ROBOT_SERVO2_PIN  GPIO_PIN_9
#define ROBOT_SERVO2_RCU  RCU_GPIOB
#define ROBOT_SERVO3_PORT GPIOG
#define ROBOT_SERVO3_PIN  GPIO_PIN_12
#define ROBOT_SERVO3_RCU  RCU_GPIOG
#define ROBOT_SERVO4_PORT GPIOD
#define ROBOT_SERVO4_PIN  GPIO_PIN_3
#define ROBOT_SERVO4_RCU  RCU_GPIOD
#define ROBOT_SERVO5_PORT GPIOH
#define ROBOT_SERVO5_PIN  GPIO_PIN_4
#define ROBOT_SERVO5_RCU  RCU_GPIOH

/* Shared software-I2C bus: OLED + LTR381 + US01 + optional TCS34725. */
#define ROBOT_I2C_SCL_PORT GPIOB
#define ROBOT_I2C_SCL_PIN  GPIO_PIN_1
#define ROBOT_I2C_SCL_RCU  RCU_GPIOB
#define ROBOT_I2C_SDA_PORT GPIOH
#define ROBOT_I2C_SDA_PIN  GPIO_PIN_10
#define ROBOT_I2C_SDA_RCU  RCU_GPIOH
#define ROBOT_I2C_HALF_PERIOD_US 5U

/* PS2 module uses an independent two-wire bit-bang bus. */
#define ROBOT_PS2_SCL_PORT GPIOA
#define ROBOT_PS2_SCL_PIN  GPIO_PIN_6
#define ROBOT_PS2_SCL_RCU  RCU_GPIOA
#define ROBOT_PS2_SDA_PORT GPIOG
#define ROBOT_PS2_SDA_PIN  GPIO_PIN_10
#define ROBOT_PS2_SDA_RCU  RCU_GPIOG
#define ROBOT_PS2_HALF_PERIOD_US 5U

/* External W25Q64 / GD25Q64 bit-bang SPI. This avoids AF and DMA conflicts. */
#define ROBOT_FLASH_CS_PORT   GPIOA
#define ROBOT_FLASH_CS_PIN    GPIO_PIN_8
#define ROBOT_FLASH_CS_RCU    RCU_GPIOA
#define ROBOT_FLASH_SCK_PORT  GPIOC
#define ROBOT_FLASH_SCK_PIN   GPIO_PIN_7
#define ROBOT_FLASH_SCK_RCU   RCU_GPIOC
#define ROBOT_FLASH_MISO_PORT GPIOC
#define ROBOT_FLASH_MISO_PIN  GPIO_PIN_4
#define ROBOT_FLASH_MISO_RCU  RCU_GPIOC
#define ROBOT_FLASH_MOSI_PORT GPIOA
#define ROBOT_FLASH_MOSI_PIN  GPIO_PIN_9
#define ROBOT_FLASH_MOSI_RCU  RCU_GPIOA

/* Status LED and buzzer. */
#define ROBOT_LED_PORT GPIOG
#define ROBOT_LED_PIN  GPIO_PIN_3
#define ROBOT_LED_RCU  RCU_GPIOG
#define ROBOT_BUZZER_PORT GPIOC
#define ROBOT_BUZZER_PIN  GPIO_PIN_13
#define ROBOT_BUZZER_RCU  RCU_GPIOC

/* Digital sensors. */
#define ROBOT_SOUND_PORT GPIOF
#define ROBOT_SOUND_PIN  GPIO_PIN_8
#define ROBOT_SOUND_RCU  RCU_GPIOF
#define ROBOT_TOUCH_PORT GPIOF
#define ROBOT_TOUCH_PIN  GPIO_PIN_9
#define ROBOT_TOUCH_RCU  RCU_GPIOF
#define ROBOT_IR_PORT GPIOF
#define ROBOT_IR_PIN  GPIO_PIN_10
#define ROBOT_IR_RCU  RCU_GPIOF

/* Conventional ultrasonic sensor (HC-SR04-style). */
#define ROBOT_US_TRIG_PORT GPIOB
#define ROBOT_US_TRIG_PIN  GPIO_PIN_12
#define ROBOT_US_TRIG_RCU  RCU_GPIOB
#define ROBOT_US_ECHO_PORT GPIOH
#define ROBOT_US_ECHO_PIN  GPIO_PIN_6
#define ROBOT_US_ECHO_RCU  RCU_GPIOH

/* Battery ADC. PA7 is exposed on the custom header. Verify divider ratio on your carrier. */
#define ROBOT_ADC_PORT GPIOA
#define ROBOT_ADC_PIN  GPIO_PIN_7
#define ROBOT_ADC_RCU  RCU_GPIOA
#define ROBOT_ADC_PERIPH ADC0
#define ROBOT_ADC_RCU_PERIPH RCU_ADC0
#define ROBOT_ADC_CHANNEL ADC_CHANNEL_7
#define ROBOT_ADC_VREF 3.3f
#define ROBOT_ADC_DIVIDER_RATIO 4.0f

/* UART connectors on the supplied carrier. */
#define ROBOT_UART1_PERIPH USART1     /* PA2 TX, PA3 RX - board connector 串口1 */
#define ROBOT_UART1_RCU    RCU_USART1
#define ROBOT_UART1_TX_PORT GPIOA
#define ROBOT_UART1_TX_PIN  GPIO_PIN_2
#define ROBOT_UART1_RX_PORT GPIOA
#define ROBOT_UART1_RX_PIN  GPIO_PIN_3
#define ROBOT_UART1_GPIO_RCU RCU_GPIOA
#define ROBOT_UART1_AF GPIO_AF_7

#define ROBOT_UART2_PERIPH USART2     /* PB10 TX, PB11 RX - board connector 串口2 */
#define ROBOT_UART2_RCU    RCU_USART2
#define ROBOT_UART2_TX_PORT GPIOB
#define ROBOT_UART2_TX_PIN  GPIO_PIN_10
#define ROBOT_UART2_RX_PORT GPIOB
#define ROBOT_UART2_RX_PIN  GPIO_PIN_11
#define ROBOT_UART2_GPIO_RCU RCU_GPIOB
#define ROBOT_UART2_AF GPIO_AF_7

/* UART3: GD32H7 <-> XMC7100 inter-MCU communication */
#define ROBOT_UART3_PERIPH     USART3      /* PD8 TX, PD9 RX */
#define ROBOT_UART3_RCU        RCU_USART3
#define ROBOT_UART3_TX_PORT    GPIOD
#define ROBOT_UART3_TX_PIN     GPIO_PIN_8
#define ROBOT_UART3_RX_PORT    GPIOD
#define ROBOT_UART3_RX_PIN     GPIO_PIN_9
#define ROBOT_UART3_GPIO_RCU   RCU_GPIOD
#define ROBOT_UART3_AF         GPIO_AF_7

#endif
