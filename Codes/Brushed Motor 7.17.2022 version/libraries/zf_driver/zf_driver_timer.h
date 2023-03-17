/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_driver_timer
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#ifndef _zf_driver_timer_h_
#define _zf_driver_timer_h_

#include "hal_device_registers.h"

#include "zf_common_clock.h"
#include "zf_common_debug.h"

typedef enum
{
    TIM_1,
    TIM_2,
    TIM_3,
    TIM_4,
    TIM_5,
    TIM_6,
    TIM_7,
    TIM_8,
}timer_index_enum;

typedef enum
{
    TIMER_SYSTEM_CLOCK,                                                         // 系统频率计时 最大 0xFFFF
    TIMER_US,                                                                   // 微秒计时 最大 0xFFFF
    TIMER_MS,                                                                   // 毫秒计时 最大 0xFFFF/2
}timer_mode_enum;

typedef enum
{
    TIMER_FUNCTION_INIT = 0,                                                    // 功能未初始化
    TIMER_FUNCTION_TIMER,                                                       // 用作 TIMER 计时
    TIMER_FUNCTION_PIT,                                                         // 用作 PIT
    TIMER_FUNCTION_PWM,                                                         // 用作 PWM
    TIMER_FUNCTION_ENCODER,                                                     // 用作 ENCODER
    TIMER_FUNCTION_CAMERA,                                                      // 用作 CAMERA
}timer_function_enum;

// 枚举 TIM_ETR 输入引脚  此枚举定义不允许用户修改
typedef enum                                                                    // 枚举 TIM_ETR 通道
{
    TIM1_ETR_A12            = 0x010C,                                           // 0x 0[TIM1] 1[AF1] 0C[A12]
    TIM1_ETR_E7             = 0x0187,                                           // 0x 0[TIM1] 1[AF1] 87[E7]

    TIM2_ETR_A0             = 0x1100,                                           // 0x 1[TIM2] 1[AF1] 00[A0]
    TIM2_ETR_A5             = 0x1105,                                           // 0x 1[TIM2] 1[AF1] 05[A5]
    TIM2_ETR_A15            = 0x110F,                                           // 0x 1[TIM2] 1[AF1] 0F[A15]

    TIM3_ETR_D2             = 0x2262,                                           // 0x 2[TIM3] 2[AF2] 62[D2]

    TIM4_ETR_E0             = 0x3280,                                           // 0x 3[TIM4] 2[AF2] 80[E0]

    TIM8_ETR_A0             = 0x7300,                                           // 0x 7[TIM8] 3[AF3] 00[A0]
}timer_etr_pin_emun;

typedef enum
{
    TIM_ETR_RISING,
    TIM_ETR_FALLING,
}timer_etr_mode_emun;

uint8       timer_funciton_check        (timer_index_enum index, timer_function_enum mode);
void        timer_clock_enable          (timer_index_enum index);

void        timer_init                  (timer_index_enum index, timer_mode_enum mode);
void        timer_start                 (timer_index_enum index);
void        timer_stop                  (timer_index_enum index);
uint16      timer_get                   (timer_index_enum index);
void        timer_clear                 (timer_index_enum index);

void        timer_etr_init              (timer_etr_pin_emun pin, timer_etr_mode_emun sign);

#endif
