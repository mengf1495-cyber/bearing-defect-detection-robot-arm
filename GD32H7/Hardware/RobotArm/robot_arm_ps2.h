#ifndef ROBOT_ARM_PS2_H
#define ROBOT_ARM_PS2_H
#include <stdint.h>
extern uint8_t ps2_buf[9];
uint8_t usb_ps2_Init(void);
void ps2_write_read(void);
uint8_t ps2_is_connected(void);
#endif
