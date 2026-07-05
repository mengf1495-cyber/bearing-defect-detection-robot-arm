#include "robot_arm_flash.h"
#include "robot_arm_board.h"
#include "robot_arm_gpio.h"
#include "robot_arm_time.h"
#define CS_L() gpio_bit_reset(ROBOT_FLASH_CS_PORT,ROBOT_FLASH_CS_PIN)
#define CS_H() gpio_bit_set(ROBOT_FLASH_CS_PORT,ROBOT_FLASH_CS_PIN)
#define CK_L() gpio_bit_reset(ROBOT_FLASH_SCK_PORT,ROBOT_FLASH_SCK_PIN)
#define CK_H() gpio_bit_set(ROBOT_FLASH_SCK_PORT,ROBOT_FLASH_SCK_PIN)
#define MO_L() gpio_bit_reset(ROBOT_FLASH_MOSI_PORT,ROBOT_FLASH_MOSI_PIN)
#define MO_H() gpio_bit_set(ROBOT_FLASH_MOSI_PORT,ROBOT_FLASH_MOSI_PIN)
static void wen(void){CS_L();spi_write_read(0x06);CS_H();}
uint8_t spi_write_read(uint8_t d){uint8_t i,r=0;for(i=0;i<8;i++){if(d&0x80)MO_H();else MO_L();d<<=1;CK_H();r=(uint8_t)((r<<1)|(gpio_input_bit_get(ROBOT_FLASH_MISO_PORT,ROBOT_FLASH_MISO_PIN)?1:0));CK_L();}return r;}
void spi_flash_init(void){robot_gpio_output_pp(ROBOT_FLASH_CS_PORT,ROBOT_FLASH_CS_RCU,ROBOT_FLASH_CS_PIN,SET);robot_gpio_output_pp(ROBOT_FLASH_SCK_PORT,ROBOT_FLASH_SCK_RCU,ROBOT_FLASH_SCK_PIN,RESET);robot_gpio_output_pp(ROBOT_FLASH_MOSI_PORT,ROBOT_FLASH_MOSI_RCU,ROBOT_FLASH_MOSI_PIN,RESET);robot_gpio_input_pullup(ROBOT_FLASH_MISO_PORT,ROBOT_FLASH_MISO_RCU,ROBOT_FLASH_MISO_PIN);CS_H();}
uint16_t spi_flash_read_id(void){uint16_t v;CS_L();spi_write_read(0x90);spi_write_read(0);spi_write_read(0);spi_write_read(0);v=(uint16_t)spi_write_read(0xff)<<8;v|=spi_write_read(0xff);CS_H();return v;}
void spi_flash_wait_busy(void){uint8_t s;do{CS_L();spi_write_read(0x05);s=spi_write_read(0xff);CS_H();}while(s&1);}
void spi_flash_read(uint8_t*b,uint32_t a,uint16_t n){CS_L();spi_write_read(0x03);spi_write_read(a>>16);spi_write_read(a>>8);spi_write_read(a);while(n--)*b++=spi_write_read(0xff);CS_H();}
static void page(uint8_t*b,uint32_t a,uint16_t n){wen();CS_L();spi_write_read(0x02);spi_write_read(a>>16);spi_write_read(a>>8);spi_write_read(a);while(n--){spi_write_read(*b++);}CS_H();spi_flash_wait_busy();}
void spi_flash_write_no_check(uint8_t*b,uint32_t a,uint16_t n){uint16_t m;while(n){m=(uint16_t)(256U-(a&255U));if(m>n)m=n;page(b,a,m);a+=m;b+=m;n-=m;}}
void spi_flash_erase_sector(uint32_t s){wen();CS_L();spi_write_read(0x20);s*=4096U;spi_write_read(s>>16);spi_write_read(s>>8);spi_write_read(s);CS_H();spi_flash_wait_busy();}
