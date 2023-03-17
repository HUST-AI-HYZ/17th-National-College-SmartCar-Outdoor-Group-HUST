/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_driver_encoder
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#ifndef _zf_driver_encoder_h_
#define _zf_driver_encoder_h_

#include "hal_device_registers.h"

#include "zf_common_clock.h"
#include "zf_common_debug.h"

typedef enum
{
    // Advanced Timer 16 bits TIM1
    TIM1_ENCOEDER_CH1_A8        = 0x1108,                                       // 0x1[CH1] 1[AF1] 08[A8]
    TIM1_ENCOEDER_CH1_E9        = 0x1189,                                       // 0x1[CH1] 1[AF1] 89[E9]
    // Advanced Timer 16 bits TIM1
    TIM1_ENCOEDER_CH2_A9        = 0x2109,                                       // 0x2[CH2] 1[AF1] 09[A9]
    TIM1_ENCOEDER_CH2_E11       = 0x218B,                                       // 0x2[CH2] 1[AF1] 8B[E11]

    // Advanced Timer 16 bits TIM8
    TIM8_ENCOEDER_CH1_C6        = 0x1346,                                       // 0x1[CH1] 3[AF3] 46[C6]
    // Advanced Timer 16 bits TIM8
    TIM8_ENCOEDER_CH2_C7        = 0x2347,                                       // 0x2[CH2] 3[AF3] 47[C7]

    // General Timer 32 bits TIM2
    TIM2_ENCOEDER_CH1_A0        = 0x1100,                                       // 0x1[CH1] 1[AF1] 00[A0]
    TIM2_ENCOEDER_CH1_A5        = 0x1105,                                       // 0x1[CH1] 1[AF1] 05[A5]
    TIM2_ENCOEDER_CH1_A15       = 0x110F,                                       // 0x1[CH1] 1[AF1] 0F[A15]
    // General Timer 32 bits TIM2
    TIM2_ENCOEDER_CH2_A1        = 0x2101,                                       // 0x2[CH2] 1[AF1] 01[A1]
    TIM2_ENCOEDER_CH2_B3        = 0x2123,                                       // 0x2[CH2] 1[AF1] 23[B3]

    // General Timer 32 bits TIM5
    TIM5_ENCOEDER_CH1_A0        = 0x1200,                                       // 0x1[CH1] 2[AF2] 00[A0]
    // General Timer 32 bits TIM5
    TIM5_ENCOEDER_CH2_A1        = 0x2201,                                       // 0x2[CH2] 2[AF2] 01[A1]

    // General Timer 16 bits TIM3
    TIM3_ENCOEDER_CH1_A6        = 0x1206,                                       // 0x1[CH1] 2[AF2] 06[A6]
    TIM3_ENCOEDER_CH1_B4        = 0x1224,                                       // 0x1[CH1] 2[AF2] 24[B4]
    TIM3_ENCOEDER_CH1_C6        = 0x1246,                                       // 0x1[CH1] 2[AF2] 46[C6]
    // General Timer 16 bits TIM3
    TIM3_ENCOEDER_CH2_A7        = 0x2207,                                       // 0x2[CH2] 2[AF2] 07[A7]
    TIM3_ENCOEDER_CH2_B5        = 0x2225,                                       // 0x2[CH2] 2[AF2] 25[B5]
    TIM3_ENCOEDER_CH2_C7        = 0x2247,                                       // 0x2[CH2] 2[AF2] 47[C7]

    // General Timer 16 bits TIM4
    TIM4_ENCOEDER_CH1_B6        = 0x1226,                                       // 0x1[CH1] 2[AF2] 26[B6]
    // General Timer 16 bits TIM4
    TIM4_ENCOEDER_CH2_B7        = 0x2227,                                       // 0x2[CH2] 2[AF2] 27[B7]
    TIM4_ENCOEDER_CH2_D13       = 0x226D,                                       // 0x2[CH2] 2[AF2] 6D[D13]
}encoder_channel_enum;

typedef enum
{
    TIM1_ENCOEDER = 0,
    TIM2_ENCOEDER,
    TIM3_ENCOEDER,
    TIM4_ENCOEDER,
    TIM5_ENCOEDER,
    TIM8_ENCOEDER = 7,
}encoder_index_enum;

int16   encoder_get_count       (encoder_index_enum encoder_n);
void    encoder_clear_count     (encoder_index_enum encoder_n);

void    encoder_init_quad       (encoder_index_enum encoder_n, encoder_channel_enum ch1_pin, encoder_channel_enum ch2_pin);
void    encoder_init_dir        (encoder_index_enum encoder_n, encoder_channel_enum ch1_pin, encoder_channel_enum ch2_pin);

#endif
