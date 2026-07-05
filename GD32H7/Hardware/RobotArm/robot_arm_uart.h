#ifndef ROBOT_ARM_UART_H
#define ROBOT_ARM_UART_H
#include <stdint.h>
#define UART_BUF_SIZE 256
extern char uart_receive_buf[UART_BUF_SIZE];
extern volatile uint16_t uart_get_ok;
extern volatile char uart_mode;
void uart1_init(uint32_t baud); void uart2_init(uint32_t baud); void uart3_init(uint32_t baud); void uart4_init(uint32_t baud); void uart5_init(uint32_t baud);
void uart1_send_byte(char c); void uart2_send_byte(char c); void uart3_send_byte(char c); void uart4_send_byte(char c); void uart5_send_byte(char c);
void uart1_send_str(char *s); void uart2_send_str(char *s); void uart3_send_str(char *s); void uart4_send_str(char *s); void uart5_send_str(char *s);
void robot_uart1_irq(void); void robot_uart2_irq(void); void robot_uart3_irq(void);
#endif
