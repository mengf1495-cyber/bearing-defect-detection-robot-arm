#include "bsp_ov2640.h"
#include "bsp_ov2640_init_table.h"
#include "bsp_software_i2c.h"

DMA_DEF(DCI_DMA, DMA1, DMA_CH7, Circulation_Disable, DMA_REQUEST_DCI);

#define DCI_WIDTH       OV2640_SENSOR_WIDTH
#define DCI_HEIGHT      OV2640_SENSOR_HEIGHT
#define DMA_NUM         (DCI_BUFF_SIZE)

__attribute__((aligned(32))) volatile uint32_t camera_buff[DCI_BUFF_SIZE];
__attribute__((aligned(32))) int8_t camera_gray[OV2640_AI_WIDTH * OV2640_AI_HEIGHT];

void camera_extract_gray(void)
{
    uint8_t *raw = (uint8_t *)camera_buff;

    const uint16_t dst_w = OV2640_AI_WIDTH;
    const uint16_t dst_h = OV2640_AI_HEIGHT;

    /*
     * 实测可用参数：
     * stride = 800 bytes
     * phase  = 0
     *
     * camera_buff = 153600 bytes
     * 153600 / 800 = 192 lines
     * 所以有效解释尺寸先按 400 x 192。
     */
    const uint16_t stride = 800;
    const uint8_t phase = 0;

    const uint16_t view_w = 400;
    const uint16_t view_h = 192;

    /*
     * 为了保持比例，不要把 400x192 直接压成 96x96。
     * 先取中心 192x192 正方形，再缩放到 96x96。
     */
    const uint16_t crop_size = 192;
    const uint16_t src_x0 = (view_w - crop_size) / 2;   // 104
    const uint16_t src_y0 = 0;

    for (uint16_t dy = 0; dy < dst_h; dy++) {
        uint16_t sy_start = src_y0 + dy * crop_size / dst_h;
        uint16_t sy_end   = src_y0 + (dy + 1) * crop_size / dst_h;

        for (uint16_t dx = 0; dx < dst_w; dx++) {
            uint16_t sx_start = src_x0 + dx * crop_size / dst_w;
            uint16_t sx_end   = src_x0 + (dx + 1) * crop_size / dst_w;

            uint32_t sum = 0;
            uint32_t n = 0;

            for (uint16_t sy = sy_start; sy < sy_end; sy++) {
                for (uint16_t sx = sx_start; sx < sx_end; sx++) {
                    uint32_t addr = sy * stride + sx * 2 + phase;
                    uint8_t v = raw[addr];

                    sum += v;
                    n++;
                }
            }

            uint8_t avg = (uint8_t)(sum / n);
            camera_gray[dy * dst_w + dx] = (int8_t)avg - 128;
        }
    }
}

static void ov2640_dci_config(void)
{
    dci_parameter_struct dci_struct;

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_DCI);

    gpio_af_set(GPIOE, GPIO_AF_13, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6);
    gpio_af_set(GPIOB, GPIO_AF_13, GPIO_PIN_6 | GPIO_PIN_8);
    gpio_af_set(GPIOA, GPIO_AF_13, GPIO_PIN_4 | GPIO_PIN_10);
    gpio_af_set(GPIOC, GPIO_AF_13, GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_9);
    gpio_af_set(GPIOG, GPIO_AF_13, GPIO_PIN_7 | GPIO_PIN_9);

    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6 | GPIO_PIN_8);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_6 | GPIO_PIN_8);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4 | GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_4 | GPIO_PIN_10);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_9);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_9);
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7 | GPIO_PIN_9);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_7 | GPIO_PIN_9);

    dci_deinit();
    dci_struct.capture_mode     = DCI_CAPTURE_MODE_SNAPSHOT;
    dci_struct.clock_polarity   = DCI_CK_POLARITY_RISING;
    dci_struct.hsync_polarity   = DCI_HSYNC_POLARITY_LOW;
    dci_struct.vsync_polarity   = DCI_VSYNC_POLARITY_HIGH;
    dci_struct.frame_rate       = DCI_FRAME_RATE_ALL;
    dci_struct.interface_format = DCI_INTERFACE_FORMAT_8BITS;
    dci_init(&dci_struct);
}

uint8_t dci_check_ready(void)
{
    if (dma_flag_get(DCI_DMA.dmax, DCI_DMA.dma_chx, DMA_FLAG_FTF) != RESET) {
        dma_flag_clear(DCI_DMA.dmax, DCI_DMA.dma_chx, DMA_FLAG_FTF);
        return 3;
    }
    return 0;
}

void dci_restart(void)
{
    driver_dma_start(&DCI_DMA, (uint8_t *)camera_buff, DMA_NUM);
    DCI_CTL |= (DCI_CTL_EVSEN | DCI_CTL_CAP);
}

static void bsp_ov2640_sccb_config(void)
{
    bsp_software_i2c_init(&CAMERA_IIC);
}

uint8_t bsp_ov2640_reg_write(uint8_t reg, uint8_t data)
{
    return bsp_software_i2c_mem_write_byte(&CAMERA_IIC, OV2640_DEVICE_ADDRESS, reg, data);
}

uint8_t bsp_ov2640_reg_read(uint8_t reg, uint8_t *data)
{
    bsp_software_i2c_write_byte(&CAMERA_IIC, OV2640_DEVICE_ADDRESS, reg);
    return bsp_software_i2c_read_data(&CAMERA_IIC, OV2640_DEVICE_ADDRESS, data);
}

static uint8_t bsp_ov2640_write_table(const uint8_t (*table)[2])
{
    uint16_t i = 0;
    while (!((table[i][0] == OV2640_REG_END) && (table[i][1] == OV2640_VAL_END))) {
        if (table[i][0] == OV2640_REG_DELAY) {
            delay_ms(table[i][1]);
        } else {
            if (bsp_ov2640_reg_write(table[i][0], table[i][1]) != 0)
                return 0xFF;
        }
        i++;
    }
    return 0;
}

uint8_t bsp_ov2640_id_read(ov2640_id_struct *ov2640id)
{
    uint8_t temp;
    if (ov2640id == NULL) return 0xFF;
    if (bsp_ov2640_reg_write(OV2640_REG_BANK_SEL, OV2640_BANK_SENSOR) != 0) return 0xFF;
    if (bsp_ov2640_reg_read(OV2640_SENSOR_MIDH, &temp) != 0) return 0xFF;
    ov2640id->Manufacturer_ID1 = temp;
    if (bsp_ov2640_reg_read(OV2640_SENSOR_MIDL, &temp) != 0) return 0xFF;
    ov2640id->Manufacturer_ID2 = temp;
    if (bsp_ov2640_reg_read(OV2640_SENSOR_VER, &temp) != 0) return 0xFF;
    ov2640id->Version = temp;
    if (bsp_ov2640_reg_read(OV2640_SENSOR_PID, &temp) != 0) return 0xFF;
    ov2640id->PID = temp;
    return 0;
}

uint8_t bsp_ov2640_init(void)
{
    bsp_ov2640_sccb_config();
    ov2640_dci_config();

    if (bsp_ov2640_reg_write(OV2640_REG_BANK_SEL, OV2640_BANK_SENSOR) != 0)
        return 0xFF;
    if (bsp_ov2640_reg_write(OV2640_SENSOR_COM7, OV2640_SENSOR_RESET) != 0)
        return 0xFF;
    delay_ms(10);

    if (bsp_ov2640_write_table(ov2640_rgb565_qvga_init) != 0)
        return 0xFF;
    delay_ms(50);

    driver_dma_com_init(&DCI_DMA, ((uint32_t)&DCI_DATA), (uint32_t)camera_buff, DMA_Width_32BIT, DMA_PERIPH_TO_MEMORY);
    driver_dma_start(&DCI_DMA, (uint8_t *)camera_buff, DMA_NUM);

    dci_enable();
    DCI_CTL |= (DCI_CTL_EVSEN | DCI_CTL_CAP);

    return 0;
}
