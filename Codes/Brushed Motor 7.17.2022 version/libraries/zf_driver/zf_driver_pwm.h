/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_driver_pwm
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#ifndef _zf_driver_pwm_h_
#define _zf_driver_pwm_h_

#include "hal_device_registers.h"

#include "zf_common_clock.h"
#include "zf_common_debug.h"

#define PWM_DUTY_MAX        (50000)

typedef enum                                                                    // 枚举 PWM 通道    此枚举定义不允许用户修改
{
    // Advanced Timer 16 bits TIM1
    TIM1_PWM_CH1_A8          = 0x01108,                                         // 0x 0[TIM1] 1[CH1] 1[AF1] 08[A8]
    TIM1_PWM_CH1_E9          = 0x01189,                                         // 0x 0[TIM1] 1[CH1] 1[AF1] 89[E9]

    TIM1_PWM_CH2_A9          = 0x02109,                                         // 0x 0[TIM1] 2[CH2] 1[AF1] 09[A9]
    TIM1_PWM_CH2_E11         = 0x0218B,                                         // 0x 0[TIM1] 2[CH2] 1[AF1] 8B[E11]

    TIM1_PWM_CH3_A10         = 0x0310A,                                         // 0x 0[TIM1] 3[CH3] 1[AF1] 0A[A10]
    TIM1_PWM_CH3_E13         = 0x0318D,                                         // 0x 0[TIM1] 3[CH2] 1[AF1] 8D[E13]

    TIM1_PWM_CH4_A11         = 0x0410B,                                         // 0x 0[TIM1] 4[CH4] 1[AF1] 0B[A11]
    TIM1_PWM_CH4_E14         = 0x0418E,                                         // 0x 0[TIM1] 4[CH4] 1[AF1] 8E[E14]

    // General Timer 32 bits TIM2
    TIM2_PWM_CH1_A0          = 0x11100,                                         // 0x 1[TIM2] 1[CH1] 1[AF1] 00[A0]
    TIM2_PWM_CH1_A5          = 0x11105,                                         // 0x 1[TIM2] 1[CH1] 1[AF1] 05[A5]
    TIM2_PWM_CH1_A15         = 0x1110F,                                         // 0x 1[TIM2] 1[CH1] 1[AF1] 0F[A15]

    TIM2_PWM_CH2_A1          = 0x12101,                                         // 0x 1[TIM2] 2[CH2] 1[AF1] 01[A1]
    TIM2_PWM_CH2_B3          = 0x12123,                                         // 0x 1[TIM2] 2[CH2] 1[AF1] 23[B3]

    TIM2_PWM_CH3_A2          = 0x13102,                                         // 0x 1[TIM2] 3[CH3] 1[AF1] 02[A2]
    TIM2_PWM_CH3_B10         = 0x1312A,                                         // 0x 1[TIM2] 3[CH3] 1[AF1] 2A[B10]

    TIM2_PWM_CH4_A3          = 0x14103,                                         // 0x 1[TIM2] 4[CH4] 1[AF1] 03[A3]
    TIM2_PWM_CH4_B11         = 0x1412B,                                         // 0x 1[TIM2] 4[CH4] 1[AF1] 2B[B11]

    // General Timer 16 bits TIM3
    TIM3_PWM_CH1_A6          = 0x21206,                                         // 0x 2[TIM3] 1[CH1] 2[AF2] 06[A6]
    TIM3_PWM_CH1_B4          = 0x21224,                                         // 0x 2[TIM3] 1[CH1] 2[AF2] 24[B4]
    TIM3_PWM_CH1_C6          = 0x21246,                                         // 0x 2[TIM3] 1[CH1] 2[AF2] 46[C6]

    TIM3_PWM_CH2_A7          = 0x22207,                                         // 0x 2[TIM3] 2[CH2] 2[AF2] 07[A7]
    TIM3_PWM_CH2_B5          = 0x22225,                                         // 0x 2[TIM3] 2[CH2] 2[AF2] 25[B5]
    TIM3_PWM_CH2_C7          = 0x22247,                                         // 0x 2[TIM3] 2[CH2] 2[AF2] 47[C7]

    TIM3_PWM_CH3_B0          = 0x23220,                                         // 0x 2[TIM3] 3[CH3] 2[AF2] 20[B0]
    TIM3_PWM_CH3_C8          = 0x23248,                                         // 0x 2[TIM3] 3[CH3] 2[AF2] 48[C8]

    TIM3_PWM_CH4_B1          = 0x24221,                                         // 0x 2[TIM3] 4[CH4] 2[AF2] 21[B1]
    TIM3_PWM_CH4_C9          = 0x24249,                                         // 0x 2[TIM3] 4[CH4] 2[AF2] 49[C9]

    // General Timer 16 bits TIM4
    TIM4_PWM_CH1_B6          = 0x31226,                                         // 0x 2[TIM3] 1[CH1] 2[AF2] 26[B6]
    TIM4_PWM_CH1_D12         = 0x3126C,                                         // 0x 2[TIM3] 1[CH1] 2[AF2] 6C[D12]

    TIM4_PWM_CH2_B7          = 0x32227,                                         // 0x 2[TIM3] 2[CH2] 2[AF2] 27[B7]
    TIM4_PWM_CH2_D13         = 0x3226D,                                         // 0x 2[TIM3] 2[CH2] 2[AF2] 6D[D13]

    TIM4_PWM_CH3_B8          = 0x33228,                                         // 0x 2[TIM3] 3[CH3] 2[AF2] 28[B8]
    TIM4_PWM_CH3_D14         = 0x3326E,                                         // 0x 2[TIM3] 3[CH3] 2[AF2] 6E[D14]

    TIM4_PWM_CH4_B9          = 0x34229,                                         // 0x 2[TIM3] 4[CH4] 2[AF2] 29[B9]
    TIM4_PWM_CH4_D15         = 0x3426F,                                         // 0x 2[TIM3] 4[CH4] 2[AF2] 6F[D15]

    // General Timer 32 bits TIM5
    TIM5_PWM_CH1_A0          = 0x41200,                                         // 0x 4[TIM5] 1[CH1] 2[AF2] 00[A0]

    TIM5_PWM_CH2_A1          = 0x42201,                                         // 0x 4[TIM5] 2[CH2] 2[AF2] 01[A1]

    TIM5_PWM_CH3_A2          = 0x43202,                                         // 0x 4[TIM5] 3[CH3] 2[AF2] 02[A2]

    TIM5_PWM_CH4_A3          = 0x44203,                                         // 0x 4[TIM5] 4[CH4] 2[AF2] 03[A3]

    // Advanced Timer 16 bits TIM8
    TIM8_PWM_CH1_C6          = 0x71346,                                         // 0x 7[TIM8] 1[CH1] 3[AF3] 46[C6]

    TIM8_PWM_CH2_C7          = 0x72347,                                         // 0x 7[TIM8] 2[CH2] 3[AF3] 47[C7]

    TIM8_PWM_CH3_C8          = 0x73348,                                         // 0x 7[TIM8] 3[CH3] 3[AF3] 48[C8]

    TIM8_PWM_CH4_C9          = 0x74349,                                         // 0x 7[TIM8] 4[CH4] 3[AF3] 49[C9]
}pwm_channel_enum;

void    pwm_set_duty    (pwm_channel_enum pin, const uint32 duty);
void    pwm_init        (pwm_channel_enum pin, const uint32 freq, const uint32 duty);

#endif
