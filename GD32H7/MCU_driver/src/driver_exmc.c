#include "driver_exmc.h"

/*!
* 说明     emxc gpio 初始化
* 输入[1]  无
* 返回值   无
*/
static void driver_exmc_nor_sram_gpio_init(void)
{
   /* EXMC clock enable */
    rcu_periph_clock_enable(RCU_EXMC);

    /* EXMC enable */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOG);

    /* configure GPIO D[0-15] */
    gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 |
                GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 |
                  GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 |
                            GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15);

    gpio_af_set(GPIOE, GPIO_AF_12, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
                GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
                  GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
                            GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
}

static void driver_exmc_sdram_gpio_init(void) 
{
   /* EXMC clock enable */
   gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5); 

    rcu_periph_clock_enable(RCU_EXMC);
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);		
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_GPIOH);

    /* D10(PB2) pin configuration */
    gpio_af_set(GPIOB, GPIO_AF_3, GPIO_PIN_2);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_2);
		
    /* D12(PC0) pin configuration */
    gpio_af_set(GPIOC, GPIO_AF_1, GPIO_PIN_0);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_0);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_0);

    /* D2(PD0),D3(PD1),D13(PD8),D14(PD9),D15(PD10),D0(PD14),D1(PD15) pin configuration */
    gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 |
                GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_8 | GPIO_PIN_9 |
                  GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_8 | GPIO_PIN_9 |
                            GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15);

    /* NBL0(PE0),NBL1(PE1),D4(PE7),D5(PE8),D6(PE9),D7(PE10),D8(PE11),D9(PE12),D10(PE13),D11(PE14) pin configuration */
    gpio_af_set(GPIOE, GPIO_AF_12, GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_7  | GPIO_PIN_8 |
                GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12  |
                GPIO_PIN_14);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_7  | GPIO_PIN_8 |
                  GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12  |
                  GPIO_PIN_14);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_7  | GPIO_PIN_8 |
                            GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12  |
                            GPIO_PIN_14);

    /* A0(PF0),A1(PF1),A2(PF2),A3(PF3),A4(PF4),A5(PF5),NRAS(PF11),A6(PF12),A7(PF13),A8(PF14),A9(PF15) pin configuration */
    gpio_af_set(GPIOF, GPIO_AF_12, GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  |
                GPIO_PIN_4  | GPIO_PIN_5  | GPIO_PIN_11 | GPIO_PIN_12 |
                GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  |
                  GPIO_PIN_4  | GPIO_PIN_5  | GPIO_PIN_11 | GPIO_PIN_12 |
                  GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  |
                            GPIO_PIN_4  | GPIO_PIN_5  | GPIO_PIN_11 | GPIO_PIN_12 |
                            GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

    /* A10(PG0),A11(PG1),A12(PG2),BA0/A14(PG4),BA1/A15(PG5),SDCLK(PG8),NCAS(PG15) pin configuration */
    gpio_af_set(GPIOG, GPIO_AF_12, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 |
                GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15);
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 |
                  GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 |
                            GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15);
    /* SDNWE(PH5) pin configuration */
    gpio_af_set(GPIOH, GPIO_AF_12, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5);
    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_2|GPIO_PIN_3| GPIO_PIN_5);
    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5);
}

/*!
* 说明     emxc LCD模式通用gpio初始化
* 输入[1]  norsram_region: @EXMC_BANK0_NORSRAM_REGION0/EXMC_BANK0_NORSRAM_REGION1/EXMC_BANK0_NORSRAM_REGION2/EXMC_BANK0_NORSRAM_REGION3
* 返回值   无
*/
static void driver_exmc_lcd_16bit_gpio_init(void)
{
   /* EXMC clock enable */
    rcu_periph_clock_enable(RCU_EXMC);

    /* EXMC enable */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOG);

    /* configure GPIO D[0-15] */
    gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 |
                GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 |
                  GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 |
                            GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15);

    gpio_af_set(GPIOE, GPIO_AF_12, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
                GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
                  GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
                            GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
                            
    /* configure NOE NWE */
    gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_4 | GPIO_PIN_5);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_4 | GPIO_PIN_5);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_4 | GPIO_PIN_5);                            
}



/*!
* 说明     emxc nor/sram模式初始化
* 输入[1]  norsram_region: @EXMC_BANK0_NORSRAM_REGION0/EXMC_BANK0_NORSRAM_REGION1/EXMC_BANK0_NORSRAM_REGION2/EXMC_BANK0_NORSRAM_REGION3
* 返回值   无
*/
void driver_exmc_norsram_init(uint32_t norsram_region)
{    
    #define NOR_SRAM_READ_DATA_SETTIME_NS 30
    #define NOR_SRAM_READ_ADDR_SETTIME_NS 10

    #define NOR_SRAM_WRITE_DATA_SETTIME_NS 30
    #define NOR_SRAM_WRITE_ADDR_SETTIME_NS 10
    
    exmc_norsram_parameter_struct nor_init_struct;
    exmc_norsram_timing_parameter_struct nor_timing_read_init_struct;
    exmc_norsram_timing_parameter_struct nor_timing_write_init_struct;

    /* EXMC clock enable */
    rcu_periph_clock_enable(RCU_EXMC);

    driver_exmc_nor_sram_gpio_init();
        
    nor_init_struct.read_write_timing = &nor_timing_read_init_struct;       
    nor_init_struct.write_timing = &nor_timing_write_init_struct;
    
    exmc_norsram_struct_para_init(&nor_init_struct);
    
    SystemCoreClockUpdate();
        
    /* config timing parameter */
    nor_timing_read_init_struct.asyn_access_mode = EXMC_ACCESS_MODE_A;
    nor_timing_read_init_struct.syn_data_latency = EXMC_DATALAT_2_CLK;
    nor_timing_read_init_struct.syn_clk_division = EXMC_SYN_CLOCK_RATIO_2_CLK;
    nor_timing_read_init_struct.bus_latency = 1;
    nor_timing_read_init_struct.asyn_data_setuptime = NOR_SRAM_READ_DATA_SETTIME_NS/(float)(1000000000.0f/SystemCoreClock);
    nor_timing_read_init_struct.asyn_address_holdtime = 1;
    nor_timing_read_init_struct.asyn_address_setuptime = NOR_SRAM_READ_ADDR_SETTIME_NS/(float)(1000000000.0f/SystemCoreClock);
    
    /* config timing parameter */
    nor_timing_write_init_struct.asyn_access_mode = EXMC_ACCESS_MODE_A;
    nor_timing_write_init_struct.syn_data_latency = EXMC_DATALAT_2_CLK;
    nor_timing_write_init_struct.syn_clk_division = EXMC_SYN_CLOCK_RATIO_2_CLK;
    nor_timing_write_init_struct.bus_latency = 1;
    nor_timing_write_init_struct.asyn_data_setuptime = NOR_SRAM_WRITE_DATA_SETTIME_NS/(float)(1000000000.0f/SystemCoreClock);
    nor_timing_write_init_struct.asyn_address_holdtime = 1;
    nor_timing_write_init_struct.asyn_address_setuptime =NOR_SRAM_WRITE_ADDR_SETTIME_NS/(float)(1000000000.0f/SystemCoreClock); 

    /* config EXMC bus parameters */
    nor_init_struct.norsram_region = norsram_region;
    nor_init_struct.write_mode = EXMC_ASYN_WRITE;
    nor_init_struct.extended_mode = ENABLE;
    nor_init_struct.asyn_wait = DISABLE;
    nor_init_struct.nwait_signal = DISABLE;
    nor_init_struct.memory_write = ENABLE;
    nor_init_struct.nwait_config = EXMC_NWAIT_CONFIG_BEFORE;
    nor_init_struct.nwait_polarity = EXMC_NWAIT_POLARITY_LOW;
    nor_init_struct.burst_mode = DISABLE;
    nor_init_struct.databus_width = EXMC_NOR_DATABUS_WIDTH_16B;
    nor_init_struct.memory_type = EXMC_MEMORY_TYPE_SRAM;
    nor_init_struct.address_data_mux = DISABLE;

    exmc_norsram_init(&nor_init_struct);

    /* enable the EXMC bank0 NORSRAM */
    exmc_norsram_enable(norsram_region);   
}


/*!
* 说明     emxc LCD模式初始化
* 输入[1]  norsram_region: @EXMC_BANK0_NORSRAM_REGION0/EXMC_BANK0_NORSRAM_REGION1/EXMC_BANK0_NORSRAM_REGION2/EXMC_BANK0_NORSRAM_REGION3
* 返回值   无
*/
void driver_exmc_lcd_init(uint32_t norsram_region)
{
    
    #define LCD_READ_DATA_SETTIME_NS 50
    #define LCD_READ_ADDR_SETTIME_NS 10

    #define LCD_WRITE_DATA_SETTIME_NS 20
    #define LCD_WRITE_ADDR_SETTIME_NS 10
    
    uint32_t ahb_clk=0;
    exmc_norsram_parameter_struct nor_init_struct;
    exmc_norsram_timing_parameter_struct nor_timing_read_init_struct;
    exmc_norsram_timing_parameter_struct nor_timing_write_init_struct;
    
    mpu_region_init_struct mpu_init_struct;
    mpu_region_struct_para_init(&mpu_init_struct);
    
    mpu_config(EXMC_BANK0_NORSRAM_REGIONx_ADDR(norsram_region),MPU_REGION_SIZE_64MB,MPU_AP_FULL_ACCESS,MPU_ACCESS_BUFFERABLE,MPU_ACCESS_NON_CACHEABLE,MPU_ACCESS_NON_SHAREABLE,MPU_INSTRUCTION_EXEC_NOT_PERMIT,MPU_TEX_TYPE0);    

    /* EXMC clock enable */
    rcu_periph_clock_enable(RCU_EXMC);
    
    driver_exmc_lcd_16bit_gpio_init();    
    
    nor_init_struct.read_write_timing = &nor_timing_read_init_struct;       
    nor_init_struct.write_timing = &nor_timing_write_init_struct;        
    exmc_norsram_struct_para_init(&nor_init_struct);    
    nor_init_struct.read_write_timing = &nor_timing_read_init_struct;       
    nor_init_struct.write_timing = &nor_timing_write_init_struct;      
    
    SystemCoreClockUpdate();
    ahb_clk=rcu_clock_freq_get(CK_AHB);
    /* config timing parameter */
    nor_timing_read_init_struct.asyn_access_mode = EXMC_ACCESS_MODE_A;
    nor_timing_read_init_struct.syn_data_latency = EXMC_DATALAT_2_CLK;
    nor_timing_read_init_struct.syn_clk_division = EXMC_SYN_CLOCK_RATIO_2_CLK;
    nor_timing_read_init_struct.bus_latency = 1;
    nor_timing_read_init_struct.asyn_data_setuptime = LCD_READ_DATA_SETTIME_NS/(float)(1000000000.0f/ahb_clk);
    nor_timing_read_init_struct.asyn_address_holdtime = 1;
    nor_timing_read_init_struct.asyn_address_setuptime = LCD_READ_ADDR_SETTIME_NS/(float)(1000000000.0f/ahb_clk);
    
    /* config timing parameter */
    nor_timing_write_init_struct.asyn_access_mode = EXMC_ACCESS_MODE_A;
    nor_timing_write_init_struct.syn_data_latency = EXMC_DATALAT_2_CLK;
    nor_timing_write_init_struct.syn_clk_division = EXMC_SYN_CLOCK_RATIO_2_CLK;
    nor_timing_write_init_struct.bus_latency = 1;
    nor_timing_write_init_struct.asyn_data_setuptime = LCD_WRITE_DATA_SETTIME_NS/(float)(1000000000.0f/ahb_clk);
    nor_timing_write_init_struct.asyn_address_holdtime = 1;
    nor_timing_write_init_struct.asyn_address_setuptime =LCD_WRITE_ADDR_SETTIME_NS/(float)(1000000000.0f/ahb_clk);

    /* config EXMC bus parameters */
    nor_init_struct.norsram_region = norsram_region;
    nor_init_struct.write_mode = EXMC_ASYN_WRITE;
    nor_init_struct.extended_mode = ENABLE;
    nor_init_struct.asyn_wait = DISABLE;
    nor_init_struct.nwait_signal = DISABLE;
    nor_init_struct.memory_write = ENABLE;
    nor_init_struct.nwait_config = EXMC_NWAIT_CONFIG_BEFORE;
    nor_init_struct.nwait_polarity = EXMC_NWAIT_POLARITY_LOW;
    nor_init_struct.burst_mode = DISABLE;
    nor_init_struct.databus_width = EXMC_NOR_DATABUS_WIDTH_16B;
    nor_init_struct.memory_type = EXMC_MEMORY_TYPE_SRAM;
    nor_init_struct.address_data_mux = DISABLE;

    exmc_norsram_init(&nor_init_struct);

    /* enable the EXMC bank0 NORSRAM */
    exmc_norsram_enable(norsram_region);  
}

/*!
    \brief      software delay
    \param[in]  count: count value
    \param[out] none
    \retval     none
*/
static void _delay(uint32_t count)
{
    __IO uint32_t index = 0;

    for(index = (100 * count); index != 0; index--) {
    }
}

/*!
    \brief      sdram peripheral initialize
    \param[in]  sdram_device: specified SDRAM device
                EXMC_SDRAM_DEVICE0      
                EXMC_SDRAM_DEVICE1    
    \param[out] none
    \retval     none
*/
Drv_Err driver_exmc_sdram_init(uint32_t sdram_device)
{
    
    /* define mode register content */
    /* burst length */
    #define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000U)
    #define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001U)
    #define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002U)
    #define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0003U)

    /* burst type */
    #define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000U)
    #define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008U)

    /* CAS latency */
    #define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020U)
    #define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030U)

    /* write mode */
    #define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000U)
    #define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200U)

    #define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000U)

    #define SDRAM_TIMEOUT                            ((uint32_t)0x0000FFFFU)

    exmc_sdram_parameter_struct sdram_init_struct;
    exmc_sdram_timing_parameter_struct sdram_timing_init_struct;
    exmc_sdram_command_parameter_struct sdram_command_init_struct;
    exmc_sdram_struct_para_init(&sdram_init_struct);

    uint32_t command_content = 0, bank_select;
    uint32_t timeout = SDRAM_TIMEOUT;
        
   /* EXMC clock enable */
    rcu_periph_clock_enable(RCU_EXMC);
    
    driver_exmc_sdram_gpio_init();
 
    /* specify which SDRAM to read and write */
    if(EXMC_SDRAM_DEVICE0 == sdram_device) {
        bank_select = EXMC_SDRAM_DEVICE0_SELECT;
    } else {
        bank_select = EXMC_SDRAM_DEVICE1_SELECT;
    }

    /* EXMC SDRAM device initialization sequence --------------------------------*/
    /* step 1 : configure SDRAM timing registers --------------------------------*/
    /* LMRD: 2 clock cycles */
    sdram_timing_init_struct.load_mode_register_delay = 2;
    /* XSRD: min = 67ns */
    sdram_timing_init_struct.exit_selfrefresh_delay = 12;
    /* RASD: min=42ns , max=120k (ns) */
    sdram_timing_init_struct.row_address_select_delay = 8;
    /* ARFD: min=60ns */
    sdram_timing_init_struct.auto_refresh_delay = 11;
    /* WRD:  min=1 Clock cycles +6ns */
    sdram_timing_init_struct.write_recovery_delay = 2;
    /* RPD:  min=18ns */
    sdram_timing_init_struct.row_precharge_delay = 4;
    /* RCD:  min=18ns */
    sdram_timing_init_struct.row_to_column_delay = 4;

    /* step 2 : configure SDRAM control registers ---------------------------------*/
    sdram_init_struct.sdram_device = sdram_device;
    sdram_init_struct.column_address_width = EXMC_SDRAM_COW_ADDRESS_9;
    sdram_init_struct.row_address_width = EXMC_SDRAM_ROW_ADDRESS_13;
    sdram_init_struct.data_width = EXMC_SDRAM_DATABUS_WIDTH_16B;
    sdram_init_struct.internal_bank_number = EXMC_SDRAM_4_INTER_BANK;
    sdram_init_struct.cas_latency = EXMC_SDCLK_PERIODS_3_CK_EXMC;
    sdram_init_struct.write_protection = DISABLE;
    sdram_init_struct.sdclock_config = EXMC_SDCLK_PERIODS_2_CK_EXMC;
    sdram_init_struct.burst_read_switch = ENABLE;//DISABLE;//ENABLE;
    sdram_init_struct.pipeline_read_delay = EXMC_PIPELINE_DELAY_1_CK_EXMC;
    sdram_init_struct.timing = &sdram_timing_init_struct;
    /* EXMC SDRAM bank initialization */
    exmc_sdram_init(&sdram_init_struct);

    /* step 3 : configure CKE high command---------------------------------------*/
    sdram_command_init_struct.command = EXMC_SDRAM_CLOCK_ENABLE;
    sdram_command_init_struct.bank_select = bank_select;
    sdram_command_init_struct.auto_refresh_number = EXMC_SDRAM_AUTO_REFLESH_1_SDCLK;
    sdram_command_init_struct.mode_register_content = 0;
    /* wait until the SDRAM controller is ready */
    while((exmc_flag_get(sdram_device, EXMC_SDRAM_FLAG_NREADY) != RESET) && (timeout > 0)) {
        timeout--;
    }
    
    if(timeout==0)
    {
        return DRV_ERROR;
    }
    /* send the command */
    exmc_sdram_command_config(&sdram_command_init_struct);

    /* step 4 : insert 10ms delay----------------------------------------------*/
    _delay(100);

    /* step 5 : configure precharge all command----------------------------------*/
    sdram_command_init_struct.command = EXMC_SDRAM_PRECHARGE_ALL;
    sdram_command_init_struct.bank_select = bank_select;
    sdram_command_init_struct.auto_refresh_number = EXMC_SDRAM_AUTO_REFLESH_1_SDCLK;
    sdram_command_init_struct.mode_register_content = 0;
    /* wait until the SDRAM controller is ready */
    timeout = SDRAM_TIMEOUT;
    while((exmc_flag_get(sdram_device, EXMC_SDRAM_FLAG_NREADY) != RESET) && (timeout > 0)) {
        timeout--;
    }
    if(timeout==0)
    {
        return DRV_ERROR;
    }    
    /* send the command */
    exmc_sdram_command_config(&sdram_command_init_struct);

    /* step 6 : configure Auto-Refresh command-----------------------------------*/
    sdram_command_init_struct.command = EXMC_SDRAM_AUTO_REFRESH;
    sdram_command_init_struct.bank_select = bank_select;
    sdram_command_init_struct.auto_refresh_number = EXMC_SDRAM_AUTO_REFLESH_8_SDCLK;
    sdram_command_init_struct.mode_register_content = 0;
    /* wait until the SDRAM controller is ready */
    timeout = SDRAM_TIMEOUT;
    while((exmc_flag_get(sdram_device, EXMC_SDRAM_FLAG_NREADY) != RESET) && (timeout > 0)) {
        timeout--;
    }
    if(timeout==0)
    {
        return DRV_ERROR;
    }    
    /* send the command */
    exmc_sdram_command_config(&sdram_command_init_struct);

    /* step 7 : configure load mode register command-----------------------------*/
    /* program mode register */
    command_content = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1        |
                      SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
                      SDRAM_MODEREG_CAS_LATENCY_3           |
                      SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                      SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

    sdram_command_init_struct.command = EXMC_SDRAM_LOAD_MODE_REGISTER;
    sdram_command_init_struct.bank_select = bank_select;
    sdram_command_init_struct.auto_refresh_number = EXMC_SDRAM_AUTO_REFLESH_1_SDCLK;
    sdram_command_init_struct.mode_register_content = command_content;

    /* wait until the SDRAM controller is ready */
    timeout = SDRAM_TIMEOUT;
    while((exmc_flag_get(sdram_device, EXMC_SDRAM_FLAG_NREADY) != RESET) && (timeout > 0)) {
        timeout--;
    }
    if(timeout==0)
    {
        return DRV_ERROR;
    }    
    /* send the command */
    exmc_sdram_command_config(&sdram_command_init_struct);

    /* step 8 : set the auto-refresh rate counter--------------------------------*/
    /* 64ms, 8192-cycle refresh, 64ms/8192=7.81us */
    /* SDCLK_Freq = SYS_Freq/2 */
    /* (7.81 us * SDCLK_Freq) - 20 */
    exmc_sdram_refresh_count_set(1542);

    /* wait until the SDRAM controller is ready */
    timeout = SDRAM_TIMEOUT;
    while((exmc_flag_get(sdram_device, EXMC_SDRAM_FLAG_NREADY) != RESET) && (timeout > 0)) {
        timeout--;
    }
    if(timeout==0)
    {
        return DRV_ERROR;
    } 
    
    _delay(100);

    return DRV_SUCCESS;    
}


/*!
    \brief      fill the buffer with specified value
    \param[in]  pbuffer: pointer to the buffer to fill data
    \param[in]  buffer_lengh: size of the buffer to fill
    \param[in]  start_value: first data to fill in the buffer
    \param[out] none
    \retval     none
*/
void fill_buffer(uint8_t *pbuffer, uint16_t buffer_lengh, uint16_t start_value)
{
    uint16_t index = 0;

    /* fill the buffer with specified values */
    for(index = 0; index < buffer_lengh; index++) {
        pbuffer[index] = (uint8_t)(index + start_value);
    }
}

/*!
    \brief      write a byte buffer(data is 8 bits) to the EXMC SDRAM memory
    \param[in]  sdram_device: specify which a SDRAM memory block is written
    \param[in]  pbuffer: pointer to buffer
    \param[in]  write_addr: SDRAM memory internal address from which the data will be written
    \param[in]  byte_count_to_write: number of bytes to write
    \param[out] none
    \retval     none
*/
void sdram_writebuffer_8(uint32_t sdram_device, uint8_t *pbuffer, uint32_t write_addr, uint32_t byte_count_to_write)
{
    uint32_t temp_addr;

    /* select the base address according to EXMC_Bank */
    if(sdram_device == EXMC_SDRAM_DEVICE0) {
        temp_addr = SDRAM_DEVICE0_ADDR;
    } else {
        temp_addr = SDRAM_DEVICE1_ADDR;
    }

    /* while there is data to write */
    for(; byte_count_to_write != 0; byte_count_to_write--) {
        /* transfer data to the memory */
        *(uint8_t *)(temp_addr + write_addr) = *pbuffer++;

        /* increment the address */
        write_addr += 1;
    }
}

/*!
    \brief      read a block of 8-bit data from the EXMC SDRAM memory
    \param[in]  sdram_device: specify which a SDRAM memory block is written
    \param[in]  pbuffer: pointer to buffer
    \param[in]  read_addr: SDRAM memory internal address to read from
    \param[in]  byte_count_to_read: number of bytes to read
    \param[out] none
    \retval     none
*/
void sdram_readbuffer_8(uint32_t sdram_device, uint8_t *pbuffer, uint32_t read_addr, uint32_t byte_count_to_read)
{
    uint32_t temp_addr;

    /* select the base address according to EXMC_Bank */
    if(sdram_device == EXMC_SDRAM_DEVICE0) {
        temp_addr = SDRAM_DEVICE0_ADDR;
    } else {
        temp_addr = SDRAM_DEVICE1_ADDR;
    }

    /* while there is data to read */
    for(; byte_count_to_read != 0; byte_count_to_read--) {
        /* read a byte from the memory */
        *pbuffer++ = *(uint8_t *)(temp_addr + read_addr);

        /* increment the address */
        read_addr += 1;
    }
}
