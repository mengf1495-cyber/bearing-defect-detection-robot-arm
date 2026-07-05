#ifndef BSP_OV2640_H
#define BSP_OV2640_H

#include "driver_public.h"

#ifdef __cplusplus
extern "C" {
#endif

/* OV2640 sensor output: 320x240 RGB565 */
#define OV2640_SENSOR_WIDTH            320U
#define OV2640_SENSOR_HEIGHT           240U

/* AI model input: 96x96 grayscale */
#define OV2640_AI_WIDTH                96U
#define OV2640_AI_HEIGHT               96U

/* Center crop from 320x240 sensor frame */
#define CROP_SIZE                     200U
#define CROP_X0                       ((OV2640_SENSOR_WIDTH  - CROP_SIZE) / 2U)
#define CROP_Y0                       ((OV2640_SENSOR_HEIGHT - CROP_SIZE) / 2U)

/* DCI buffer: 32-bit words, each holds 2 RGB565 pixels */
#define DCI_BUFF_SIZE                  ((OV2640_SENSOR_WIDTH * OV2640_SENSOR_HEIGHT) / 2U)
extern volatile uint32_t camera_buff[DCI_BUFF_SIZE];

/* Grayscale buffer: downsampled to 96x96, directly usable as AI input */
extern int8_t camera_gray[OV2640_AI_WIDTH * OV2640_AI_HEIGHT];

void camera_extract_gray(void);

/* ATK-MC2640 / OV2640 SCCB address */
#define OV2640_DEVICE_WRITE_ADDRESS    0x60
#define OV2640_DEVICE_READ_ADDRESS     0x61
#define OV2640_DEVICE_ADDRESS          (OV2640_DEVICE_WRITE_ADDRESS >> 1)

typedef struct
{
    uint8_t Manufacturer_ID1;
    uint8_t Manufacturer_ID2;
    uint8_t Version;
    uint8_t PID;
} ov2640_id_struct;

#define OV2640_REG_BANK_SEL            0xFF
#define OV2640_BANK_DSP                0x00
#define OV2640_BANK_SENSOR             0x01
#define OV2640_SENSOR_PID              0x0A
#define OV2640_SENSOR_VER              0x0B
#define OV2640_SENSOR_MIDH             0x1C
#define OV2640_SENSOR_MIDL             0x1D
#define OV2640_SENSOR_COM7             0x12
#define OV2640_SENSOR_RESET            0x80

#ifndef OV2640_USE_EXTERNAL_XCLK
#define OV2640_USE_EXTERNAL_XCLK       0
#endif

uint8_t bsp_ov2640_init(void);
uint8_t bsp_ov2640_id_read(ov2640_id_struct *ov2640id);
uint8_t bsp_ov2640_reg_write(uint8_t reg, uint8_t data);
uint8_t bsp_ov2640_reg_read(uint8_t reg, uint8_t *data);

void dci_restart(void);
uint8_t dci_check_ready(void);

#ifdef __cplusplus
}
#endif

#endif
