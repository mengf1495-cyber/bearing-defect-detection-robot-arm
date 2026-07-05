/*!
    \file    main.c
    \brief    
    
    \version  
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "gd32h759i_eval.h"
#include "gd_nn_interface.h"
#include "nn_model_configure.h"
#include "nn_model_benchmark.h"
#include "remote_io.h"
#include "robot_arm_time.h"
#include "driver_public.h"
#include "bsp_ov2640.h"
#include "bsp_oled.h"
#include "bsp_led.h"
#include "bsp_robot_arm.h"
#include "systick.h"

/*
 * NPN 激光传感器已迁移至 XMC7100。
 * GD32H7 通过 remote_io_get_sensor() 获取缓存状态（XMC7100 主动上报）。
 */

void System_Init(void)
{
    driver_init();

    robot_time_init();

    /* GD32<->XMC7100 UART3 communication (PD8/PD9, 115200) */
    remote_io_init();
}


static void app_mpu_config(void);
static void camera_enhance_gray(void);
static uint8_t camera_gray_valid(int16_t *mean_out, int16_t *range_out);

/* input and output code */
int8_t* input_data = (int8_t*)static_buffer_peak;  
int8_t* output_data_ptr[1];  
    
/* nn_report init macro */
#define nn_report_init(nn_report, clock, m_name, opera_name) \
    nn_report.core_clock = clock;\
    nn_report.model_name = m_name;\
    nn_report.operator_name = (char**)opera_name;\

/* model reporter instance, if need invoke two models, please create two nn_report*/
nn_model_report_struct nn_report;

/* gd model_paras_array_dict arr, if need invoke two models, please create two buf*/
nn_uint8 model_paras_array_info_buf[8];

/* gd model_paras_array addr and model_paras_data addr */
const nn_uint8* model_paras_array_and_data[2] = {model_paras_arr, model_paras_data};

/* benchmark struct instance */
#if defined (BENCHMARK)
    static  nn_benchmark benchmark;                         
#endif

/* model parameters struct */
static nn_model bearing_model; 

#define nn_model_struct_init(m_struct)\
    m_struct.user_input = input_data;\
    m_struct.user_input_size = INPUT_SIZE * INPUT_TYPE_SIZE;\
    m_struct.user_output = (void**)output_data_ptr;\
    m_struct.user_output_size = OUTPUT_SIZE * OUTPUT_TYPE_SIZE;\
    \
    m_struct.operators_cb_array = func_cb_arr;\
    m_struct.model_paras_array = (const nn_uint8*)model_paras_array_and_data;\
    m_struct.model_paras_array_dict = model_paras_array_info_buf;\
    m_struct.report_ptr = &nn_report; 

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

static void camera_enhance_gray(void)
{
    const float black_level = 10.0f;
    const float gain = 1.5f;
    const float gamma_val = 0.80f;

    for (int i = 0; i < INPUT_SIZE; i++) {
        float v = (float)(camera_gray[i] + 128);

        v = (v - black_level) * gain;
        if (v < 0.0f) v = 0.0f;
        if (v > 255.0f) v = 255.0f;

        v = 255.0f * powf(v / 255.0f, gamma_val);
        if (v < 0.0f) v = 0.0f;
        if (v > 255.0f) v = 255.0f;

        camera_gray[i] = (int8_t)((int16_t)(v + 0.5f) - 128);
    }
}
static uint8_t camera_gray_valid(int16_t *mean_out, int16_t *range_out)
{
    int16_t min_v = 127;
    int16_t max_v = -128;
    int32_t sum = 0;

    for (int i = 0; i < INPUT_SIZE; i++) {
        int16_t v = camera_gray[i];

        if (v < min_v) min_v = v;
        if (v > max_v) max_v = v;

        sum += v;
    }

    int16_t mean = (int16_t)(sum / INPUT_SIZE);
    int16_t range = (int16_t)(max_v - min_v);

    if (mean_out != NULL) {
        *mean_out = mean;
    }

    if (range_out != NULL) {
        *range_out = range;
    }

    /*
     * camera_gray 范围�?-128~127�?
     * range 太小说明画面对比度太低；
     * mean 太极端说明画面可能太�?太亮�?
     */
    if (range < 10) return 0;
    if (mean < -120) return 0;
    if (mean > 120) return 0;

    return 1;
}
int main(void)
{

    /* error_record */
    error_type ret_error = ok;

#if defined(GD32H7XX) || defined(GD32H77D) || defined(GD32H77E)
    /* Enable I-Cache */
    SCB_EnableICache();
    /* Enable D-Cache */
    SCB_EnableDCache();
#endif
    /* systick */
    systick_config();
    /* mpu_config */
    app_mpu_config();
    
    /* model init */
    nn_model_struct_init(bearing_model);
    ret_error = nn_model_init(&bearing_model);
    if(ret_error != ok) {
        printf("init error: %d\n", ret_error);
        return -1;
    }

    /* report init */
    nn_report_init(nn_report, SystemCoreClock, model_name, operator_name);

    /* MCU Benchmark with PC */
    #if defined (BENCHMARK)

        benchmark_state state = benchmark_ok;
        
        /* init benchmark from up software */
        state = nn_benchmark_init(&benchmark); 
            
        /* run benchmark, receive data from PC */
        state = nn_benchmark_start(&benchmark, &bearing_model);

        if(state != benchmark_ok) {
            return -1;
        }

    /* Camera + AI + OLED mode */
    #else

        ov2640_id_struct ov2640id = {0};
        uint8_t cam_status, id_status;
        uint8_t frame_count = 0, fps = 0;
        uint32_t tick_last = 0;

        /* uart init at 115200 for image streaming */
        gd_eval_com_init(EVAL_COM);

        /* initialize delay for camera/OLED drivers */
        delay_init();

        /* LED init */
        bsp_led_group_init();

        /* OLED init */
        bsp_oled_init();
        bsp_oled_clear();
        bsp_oled_printf(0, "FPS: 0");
        bsp_oled_printf(1, "WAIT");
        bsp_oled_printf(2, "RDY");
        bsp_oled_refresh();

        /* NPN sensor: now on XMC7100, read via remote_io_get_sensor() */

        /* Camera init */
        cam_status = bsp_ov2640_init();
        id_status = bsp_ov2640_id_read(&ov2640id);

        /* Motor: now on XMC7100, start via remote_io */
        remote_io_motor_on();

        /* Display XMC7100 comm status */
        if (!remote_io_is_ok()) {
            bsp_oled_printf(1, "XMC ERR!");
            bsp_oled_refresh();
        }

        /* Robot arm init */
        bsp_robot_arm_init();

        tick_last = driver_tick;

        while (1)
        {
            uint8_t ov_flag = dci_check_ready();

            if (ov_flag >= 3)
            {
                SCB_InvalidateDCache_by_Addr((uint32_t *)camera_buff, sizeof(camera_buff));

                /* RGB565 -> grayscale */
                camera_extract_gray();
                camera_enhance_gray();

                /* FPS */
                frame_count++;
                {
                    uint32_t tick_now = driver_tick;
                    if (tick_now - tick_last >= 1000) {
                        if (remote_io_is_ok()) {
                            bsp_oled_printf(0, "FPS:%u", (unsigned)frame_count);
                        } else {
                            bsp_oled_printf(0, "FPS:%u COMM_ERR", (unsigned)frame_count);
                        }
                        frame_count = 0;
                        tick_last = tick_now;
                    }
                }

                /* ── Sensor check FIRST: must run regardless of image quality ── */
                int8_t bearing_centered = (int8_t)remote_io_get_sensor();

                enum { PHASE_FIRST, PHASE_FLIPPING, PHASE_SECOND, PHASE_REJECT, PHASE_DONE };
                static uint8_t inspect_phase = PHASE_FIRST;

                /* Sensor debounce: require N consecutive clear frames before re-arm */
                static uint8_t sensor_clear_cnt = 0;
#define SENSOR_CLEAR_NEEDED  5

                /* Sensor lost debounce: require N consecutive lost frames before abort */
                static uint8_t sensor_lost_cnt = 0;
#define SENSOR_LOST_NEEDED   5

                /* Stop motor immediately when bearing detected (except DONE phase) */
                if (bearing_centered && inspect_phase != PHASE_DONE) {
                    remote_io_motor_off();
                }

                int8_t class_0 = 0, class_1 = 0;
                uint8_t ai_ran = 0;
                int16_t diff = 0;

                /* ── Image quality: only gates AI, NOT motor control ── */
                int16_t gray_mean, gray_range;
                uint8_t frame_ok = camera_gray_valid(&gray_mean, &gray_range);

                if (bearing_centered && frame_ok) {

                    sensor_lost_cnt = 0;  /* bearing seen, reset lost debounce */

                    uint8_t run_ai = 0;

                    switch (inspect_phase) {

                    case PHASE_DONE:
                        /* Inspection complete, motor running, bearing is leaving */
                        sensor_clear_cnt = 0;  /* reset debounce while bearing still present */
                        break;

                    case PHASE_FIRST:
                        /* First side inspection */
                        run_ai = 1;
                        break;

                    case PHASE_FLIPPING:
                        /* Wait for flip to finish */
                        if (!bsp_robot_arm_is_busy()) {
                            /* Flip complete → switch to second side */
                            inspect_phase = PHASE_SECOND;
                        }
                        break;

                    case PHASE_SECOND:
                        /* Second side inspection */
                        run_ai = 1;
                        break;

                    case PHASE_REJECT:
                        /* Wait for BAD sequence to finish */
                        if (!bsp_robot_arm_is_busy()) {
                            bsp_robot_arm_reset();
                            remote_io_motor_on();
                            inspect_phase = PHASE_DONE;
                        }
                        break;

                    }

                    if (run_ai) {
                        /* Run AI on current frame */
                        memcpy(input_data, camera_gray, INPUT_SIZE);
                        ret_error = nn_model_invoke(&bearing_model);

                        if (ret_error == ok) {
                            class_0 = ((int8_t*)output_data_ptr[0])[0];
                            class_1 = ((int8_t*)output_data_ptr[0])[1];
                            diff = (int16_t)class_0 - (int16_t)class_1;
                            ai_ran = 1;

                            if (diff >= 250) {
                                /* DEFECT → start BAD sequence */
                                if (inspect_phase == PHASE_FIRST) {
                                    bsp_oled_printf(1, "DEFECT S1       ");
                                } else {
                                    bsp_oled_printf(1, "DEFECT S2       ");
                                }
                                bsp_robot_arm_push_defect();
                                inspect_phase = PHASE_REJECT;
                            } else {
                                /* NORMAL */
                                if (inspect_phase == PHASE_FIRST) {
                                    /* First side OK → flip */
                                    bsp_oled_printf(1, "NORMAL S1       ");
                                    bsp_robot_arm_start_flip();
                                    inspect_phase = PHASE_FLIPPING;
                                } else {
                                    /* Both sides OK → restart motor, wait for bearing to leave */
                                    bsp_oled_printf(1, "NORMAL S2       ");
                                    bsp_oled_printf(2, "PASS            ");
                                    remote_io_motor_on();
                                    inspect_phase = PHASE_DONE;
                                }
                            }
                        }
                    }

                    /* OLED line-2 status for non-PASS cases */
                    switch (inspect_phase) {
                    case PHASE_FLIPPING: {
                        uint8_t step = bsp_robot_arm_get_step();
                        bsp_oled_printf(2, "FLIP %u/9       ", (unsigned)step);
                        break;
                    }
                    case PHASE_REJECT: {
                        uint8_t step = bsp_robot_arm_get_step();
                        bsp_oled_printf(2, "REJECT %u/9     ", (unsigned)step);
                        break;
                    }
                    case PHASE_SECOND:
                        bsp_oled_printf(2, "WAIT S2         ");
                        break;
                    case PHASE_FIRST:
                    case PHASE_DONE:
                        /* "PASS" already set, or idle state */
                        break;
                    }

                } else if (bearing_centered && !frame_ok) {
                    /* Bearing detected but frame bad — motor already stopped, skip AI */
                    sensor_lost_cnt = 0;  /* bearing still seen */
                    switch (inspect_phase) {
                    case PHASE_FLIPPING: {
                        uint8_t step = bsp_robot_arm_get_step();
                        bsp_oled_printf(2, "FLIP %u/9       ", (unsigned)step);
                        break;
                    }
                    case PHASE_REJECT: {
                        uint8_t step = bsp_robot_arm_get_step();
                        bsp_oled_printf(2, "REJECT %u/9     ", (unsigned)step);
                        break;
                    }
                    case PHASE_SECOND:
                        bsp_oled_printf(2, "WAIT S2         ");
                        break;
                    case PHASE_FIRST:
                        bsp_oled_printf(2, "BADIMG          ");
                        break;
                    case PHASE_DONE:
                        sensor_clear_cnt = 0;  /* reset debounce while bearing still present */
                        break;
                    }

                    /* Still check arm completion on bad frames */
                    if (inspect_phase == PHASE_FLIPPING && !bsp_robot_arm_is_busy()) {
                        inspect_phase = PHASE_SECOND;
                    }
                    if (inspect_phase == PHASE_REJECT && !bsp_robot_arm_is_busy()) {
                        bsp_robot_arm_reset();
                        remote_io_motor_on();
                        inspect_phase = PHASE_DONE;
                    }

                } else {
                    /* Bearing NOT detected */
                    if (inspect_phase == PHASE_DONE) {
                        /* Inspection complete, bearing leaving naturally
                         * Debounce: require SENSOR_CLEAR_NEEDED consecutive clear frames
                         * before re-arming, to prevent sensor bounce from
                         * re-triggering inspection on the same bearing. */
                        sensor_clear_cnt++;
                        if (sensor_clear_cnt >= SENSOR_CLEAR_NEEDED && !bsp_robot_arm_is_busy()) {
                            inspect_phase = PHASE_FIRST;
                            sensor_clear_cnt = 0;
                        }
                        bsp_oled_printf(1, "WAIT            ");
                        bsp_oled_printf(2, "CLR %u/%u       ",
                                        (unsigned)sensor_clear_cnt,
                                        (unsigned)SENSOR_CLEAR_NEEDED);
                    } else if (inspect_phase == PHASE_FIRST) {
                        /* Idle or bearing removed before AI ran: restart motor */
                        sensor_clear_cnt = 0;
                        if (!bsp_robot_arm_is_busy()) {
                            remote_io_motor_on();
                        }
                        bsp_oled_printf(1, "WAIT            ");
                        bsp_oled_printf(2, "RUN             ");
                    } else {
                        /* Mid-inspection (FLIPPING/SECOND/REJECT): keep motor stopped */
                        remote_io_motor_off();
                        sensor_lost_cnt++;
                        if (sensor_lost_cnt >= SENSOR_LOST_NEEDED) {
                            /* Bearing genuinely lost — abort inspection */
                            if (!bsp_robot_arm_is_busy()) {
                                bsp_robot_arm_reset();
                                remote_io_motor_on();
                                inspect_phase = PHASE_FIRST;
                                sensor_clear_cnt = 0;
                                sensor_lost_cnt = 0;
                                bsp_oled_printf(1, "WAIT            ");
                                bsp_oled_printf(2, "RUN             ");
                            } else {
                                bsp_oled_printf(1, "WAIT            ");
                                bsp_oled_printf(2, "LOST            ");
                            }
                        } else {
                            /* Debouncing: sensor may recover after FLIP vibration */
                            bsp_oled_printf(1, "WAIT            ");
                            bsp_oled_printf(2, "LOST %u/%u      ",
                                            (unsigned)sensor_lost_cnt,
                                            (unsigned)SENSOR_LOST_NEEDED);
                        }
                    }
                }
                bsp_oled_printf(3, "diff:%d C0:%d C1:%d", diff, class_0, class_1);
                bsp_oled_refresh();

                /* Robot arm: advance action sequence step */
                bsp_robot_arm_process();

                /* UART send: live frame */
                usart_data_transmit(EVAL_COM, 0xFF);
                while (RESET == usart_flag_get(EVAL_COM, USART_FLAG_TBE));
                usart_data_transmit(EVAL_COM, 0xAA);
                while (RESET == usart_flag_get(EVAL_COM, USART_FLAG_TBE));
                for (int i = 0; i < INPUT_SIZE; i++) {
                    usart_data_transmit(EVAL_COM, (uint8_t)(camera_gray[i] + 128));
                    while (RESET == usart_flag_get(EVAL_COM, USART_FLAG_TBE));
                }
                usart_data_transmit(EVAL_COM, ai_ran);
                while (RESET == usart_flag_get(EVAL_COM, USART_FLAG_TBE));
                usart_data_transmit(EVAL_COM, (uint8_t)(class_0 + 128));
                while (RESET == usart_flag_get(EVAL_COM, USART_FLAG_TBE));
                usart_data_transmit(EVAL_COM, (uint8_t)(class_1 + 128));
                while (RESET == usart_flag_get(EVAL_COM, USART_FLAG_TBE));

                dci_restart();
            }

            /* Re-arm capture if CAP was auto-cleared */
            if (!(DCI_CTL & DCI_CTL_CAP)) {
                dci_restart();
            }
        }


    #endif

    while(1) {
    
    }

}

static void app_mpu_config(void)
{
    mpu_region_init_struct mpu_init_struct;
    mpu_region_struct_para_init(&mpu_init_struct);

    ARM_MPU_Disable();
    ARM_MPU_SetRegion(0, 0);

    mpu_init_struct.region_base_address  = 0x0;
    mpu_init_struct.region_size          = MPU_REGION_SIZE_4GB;
    mpu_init_struct.access_permission    = MPU_AP_NO_ACCESS;
    mpu_init_struct.access_bufferable    = MPU_ACCESS_NON_BUFFERABLE;
    mpu_init_struct.access_cacheable     = MPU_ACCESS_NON_CACHEABLE;
    mpu_init_struct.access_shareable     = MPU_ACCESS_SHAREABLE;
    mpu_init_struct.region_number        = MPU_REGION_NUMBER0;
    mpu_init_struct.subregion_disable    = 0x87;
    mpu_init_struct.instruction_exec     = MPU_INSTRUCTION_EXEC_NOT_PERMIT;
    mpu_init_struct.tex_type             = MPU_TEX_TYPE0;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();

    ARM_MPU_Enable(MPU_MODE_PRIV_DEFAULT);
}

#if defined(__GNUC__) && !defined(__clang__)/* For GNU GCC compiler */
/* retarget the C library printf function to the USART, in Eclipse GCC environment */
int __io_putchar(int ch)
{
    usart_data_transmit(EVAL_COM, (uint8_t) ch );
    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_TBE));
    return ch;
}
#else
/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_TBE));

    return ch;
}
#endif /* defined(__GNUC__) && !defined(__clang__) */
