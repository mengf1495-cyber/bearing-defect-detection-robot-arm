#include "robot_arm_uart.h"
#include "robot_arm_board.h"
#include "gd32h7xx.h"
#include <string.h>

char uart_receive_buf[UART_BUF_SIZE];
volatile uint16_t uart_get_ok = 0;
volatile char uart_mode = 0;

static uint16_t rxn = 0;

/*
 * ???????:
 * 0 = ??
 * 1 = $...! ????
 * 2 = #...! ??/????????
 * 4 = <...> ??????????
 */
static volatile char rx_mode = 0;

static void uart_hw_init(uint32_t usart_periph,
                         rcu_periph_enum usart_rcu,
                         uint32_t gpio_port,
                         rcu_periph_enum gpio_rcu,
                         uint32_t tx_pin,
                         uint32_t rx_pin,
                         uint32_t gpio_af,
                         uint32_t baud,
                         IRQn_Type irq)
{
    rcu_periph_clock_enable(gpio_rcu);
    rcu_periph_clock_enable(usart_rcu);

    gpio_af_set(gpio_port, gpio_af, tx_pin | rx_pin);
    gpio_mode_set(gpio_port, GPIO_MODE_AF, GPIO_PUPD_PULLUP, tx_pin | rx_pin);
    gpio_output_options_set(gpio_port, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, tx_pin | rx_pin);

    usart_deinit(usart_periph);
    usart_baudrate_set(usart_periph, baud);
    usart_word_length_set(usart_periph, USART_WL_8BIT);
    usart_stop_bit_set(usart_periph, USART_STB_1BIT);
    usart_parity_config(usart_periph, USART_PM_NONE);
    usart_receive_config(usart_periph, USART_RECEIVE_ENABLE);
    usart_transmit_config(usart_periph, USART_TRANSMIT_ENABLE);

    usart_interrupt_enable(usart_periph, USART_INT_RBNE);
    nvic_irq_enable(irq, 3, 0);

    usart_enable(usart_periph);
}

void uart1_init(uint32_t baud)
{
    uart_hw_init(ROBOT_UART1_PERIPH,
                 ROBOT_UART1_RCU,
                 ROBOT_UART1_TX_PORT,
                 ROBOT_UART1_GPIO_RCU,
                 ROBOT_UART1_TX_PIN,
                 ROBOT_UART1_RX_PIN,
                 ROBOT_UART1_AF,
                 baud,
                 USART1_IRQn);
}

void uart2_init(uint32_t baud)
{
    uart_hw_init(ROBOT_UART2_PERIPH,
                 ROBOT_UART2_RCU,
                 ROBOT_UART2_TX_PORT,
                 ROBOT_UART2_GPIO_RCU,
                 ROBOT_UART2_TX_PIN,
                 ROBOT_UART2_RX_PIN,
                 ROBOT_UART2_AF,
                 baud,
                 USART2_IRQn);
}

/*
 * ?? 3 ???? robot_arm_board.h ??? ROBOT_UART3_xxx ?,
 * ???????,???????
 * ????? OV2640/OpenMV ? UART3/4/5,???? board ???????
 */
void uart3_init(uint32_t baud)
{
    uart_hw_init(ROBOT_UART3_PERIPH,
                 ROBOT_UART3_RCU,
                 ROBOT_UART3_TX_PORT,
                 ROBOT_UART3_GPIO_RCU,
                 ROBOT_UART3_TX_PIN,
                 ROBOT_UART3_RX_PIN,
                 ROBOT_UART3_AF,
                 baud,
                 USART3_IRQn);
}

void uart4_init(uint32_t baud)
{
    (void)baud;
}

void uart5_init(uint32_t baud)
{
    (void)baud;
}

static void uart_send_byte_hw(uint32_t usart_periph, char c)
{
    while (RESET == usart_flag_get(usart_periph, USART_FLAG_TBE)) {
    }
    usart_data_transmit(usart_periph, (uint8_t)c);
}

void uart1_send_byte(char c)
{
    uart_send_byte_hw(ROBOT_UART1_PERIPH, c);
}

void uart2_send_byte(char c)
{
    uart_send_byte_hw(ROBOT_UART2_PERIPH, c);
}

void uart3_send_byte(char c)
{
    uart_send_byte_hw(ROBOT_UART3_PERIPH, c);
}

void uart4_send_byte(char c)
{
    uart1_send_byte(c);
}

void uart5_send_byte(char c)
{
    uart1_send_byte(c);
}

static void uart_send_str_hw(void (*send_byte)(char), char *str)
{
    while (str && *str) {
        send_byte(*str++);
    }
}

void uart1_send_str(char *str)
{
    uart_send_str_hw(uart1_send_byte, str);
}

void uart2_send_str(char *str)
{
    uart_send_str_hw(uart2_send_byte, str);
}

void uart3_send_str(char *str)
{
    uart_send_str_hw(uart3_send_byte, str);
}

void uart4_send_str(char *str)
{
    uart_send_str_hw(uart4_send_byte, str);
}

void uart5_send_str(char *str)
{
    uart_send_str_hw(uart5_send_byte, str);
}

static void uart_rx_reset(void)
{
    rxn = 0;
    rx_mode = 0;
    memset(uart_receive_buf, 0, UART_BUF_SIZE);
}

/*
 * ??????????
 *
 * ??:
 * $xxx!      ? uart_mode = 1 ? parse_cmd()
 * #xxx!      ? uart_mode = 2 ? parse_action()
 * <xxx>      ? uart_mode = 4 ? save_action()
 *
 * ??:
 * ???????????????? ! ,?? <...> ???? > ????
 */
static void uart_irq_handler(uint32_t usart_periph)
{
    char c;

    if (RESET == usart_interrupt_flag_get(usart_periph, USART_INT_FLAG_RBNE)) {
        return;
    }

    c = (char)usart_data_receive(usart_periph);

    /*
     * ?????????? app_uart_run() ??,
     * ????????,????????
     */
    if (uart_get_ok) {
        return;
    }

    /*
     * ?????
     */
    if (rxn == 0) {
        if (c == '$') {
            rx_mode = 1;
        } else if (c == '#') {
            rx_mode = 2;
        } else if (c == '<') {
            rx_mode = 4;
        } else {
            /*
             * ?????????
             * ????????? DEFECT ??,???? $DEFECT!
             */
            return;
        }
    }

    /*
     * ??????
     */
    if (rxn < UART_BUF_SIZE - 1) {
        uart_receive_buf[rxn++] = c;
    } else {
        /*
         * ?????,?????
         */
        uart_rx_reset();
        return;
    }

    /*
     * ?????:
     * $...! ? #...! ? ! ??;
     * <...> ? > ???
     */
    if (((rx_mode == 1 || rx_mode == 2) && c == '!') ||
        (rx_mode == 4 && c == '>')) {
        uart_receive_buf[rxn] = '\0';
        uart_mode = rx_mode;
        uart_get_ok = 1;

        rxn = 0;
        rx_mode = 0;
    }
}

void robot_uart1_irq(void)
{
    uart_irq_handler(ROBOT_UART1_PERIPH);
}

void robot_uart2_irq(void)
{
    uart_irq_handler(ROBOT_UART2_PERIPH);
}

void robot_uart3_irq(void)
{
    uart_irq_handler(ROBOT_UART3_PERIPH);
}