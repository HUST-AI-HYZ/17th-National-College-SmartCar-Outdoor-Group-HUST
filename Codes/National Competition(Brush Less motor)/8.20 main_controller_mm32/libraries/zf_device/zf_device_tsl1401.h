/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_tsl1401
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
* @note             接线定义：
*                   ------------------------------------
*                   模块管脚            单片机管脚
*                   CLK                 查看 zf_device_tsl1401.h 中 TSL1401_CLK_PIN 宏定义
*                   SI                  查看 zf_device_tsl1401.h 中 TSL1401_SI_PIN 宏定义
*                   AO[x]               查看 zf_device_tsl1401.h 中 TSL1401_AO_PINx 宏定义
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   ------------------------------------
********************************************************************************************************************/

#ifndef _zf_device_tsl1401_h_
#define _zf_device_tsl1401_h_

#include "zf_common_clock.h"
#include "zf_common_debug.h"

#include "zf_driver_adc.h"
#include "zf_driver_delay.h"
#include "zf_driver_gpio.h"
#include "zf_driver_pit.h"
#include "zf_driver_uart.h"

#define TSL1401_AO_PIN          (ADC2_CH8_B0)                                   // 对应第一个 TSL1401 的 AO 引脚
#define TSL1401_AO_PIN2         (ADC2_CH9_B1)                                   // 对应第二个 TSL1401 的 AO 引脚

// TSL1401 的控制引脚定义 多个 TSL1401 建议将控制引脚直接并联
#define TSL1401_CLK_PIN         (B2)                                            // TSL1401 的 CLK 引脚定义
#define TSL1401_SI_PIN          (B3)                                            // TSL1401 的 SI 引脚定义
#define TSL1401_CLK(x)          ((x) ? (gpio_high(TSL1401_CLK_PIN)) : (gpio_low(TSL1401_CLK_PIN)))
#define TSL1401_SI(x)           ((x) ? (gpio_high(TSL1401_SI_PIN)) : (gpio_low(TSL1401_SI_PIN)))

// TSL1401 的周期采集部分定义 使用到哪个 PIT 就要把  放在哪个 PIT 的中断服务函数下
#define TSL1401_EXPOSURE_TIME   (10 )                                           // 定义 TSL1401 曝光时间 单位 MS
#define TSL1401_PIT_INDEX       (TIM8_PIT)                                      // 使用周期中断
#define TSL1401_AD_RESOLUTION   (ADC_8BIT)                                      // ADC 精度 8bit
#define TSL1401_DATA_LEN        (128 )                                          // TSL1401 数据长度

extern uint16 tsl1401_data[2][TSL1401_DATA_LEN];                                // TSL1401 数据存放数组
extern uint8 tsl1401_finish_flag;                                               // TSL1401 数据采集完成标志

void tsl1401_collect_pit_handler    (void);
void tsl1401_send_data              (uart_index_enum uart_n, uint8 index);
void tsl1401_init                   (void);

#endif
