/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_driver_exti
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#ifndef _zf_driver_exti_h_
#define _zf_driver_exti_h_

#include "hal_device_registers.h"

#include "zf_common_interrupt.h"

#include "zf_driver_gpio.h"

typedef enum{                                                                   // 枚举 EXTI 触发信号  此枚举定义不允许用户修改
    EXTI_TRIGGER_RISING,                                                        // 上升沿触发模式
    EXTI_TRIGGER_FALLING,                                                       // 下降沿触发模式
    EXTI_TRIGGER_BOTH,                                                          // 双边沿触发模式
}exti_trigger_enum;

void    exti_enable   (gpio_pin_enum pin);
void    exti_disable  (gpio_pin_enum pin);
void    exti_init     (gpio_pin_enum pin, exti_trigger_enum trigger);

#endif
