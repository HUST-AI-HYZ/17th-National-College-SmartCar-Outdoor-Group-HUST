/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_absolute_encoder
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
*                   // 硬件 SPI 引脚
*                   SCLK                查看 zf_device_absolute_encoder.h 中 ABSOLUTE_ENCODER_SCLK_PIN 宏定义
*                   MOSI                查看 zf_device_absolute_encoder.h 中 ABSOLUTE_ENCODER_MOSI_PIN 宏定义
*                   MISO                查看 zf_device_absolute_encoder.h 中 ABSOLUTE_ENCODER_MISO_PIN 宏定义
*                   CS                  查看 zf_device_absolute_encoder.h 中 ABSOLUTE_ENCODER_CS_PIN 宏定义
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   ------------------------------------
********************************************************************************************************************/

#ifndef _zf_device_absolute_encoder_h_
#define _zf_device_absolute_encoder_h_

#include "zf_common_clock.h"
#include "zf_common_debug.h"
#include "zf_common_function.h"

#include "zf_driver_delay.h"
#include "zf_driver_soft_spi.h"
#include "zf_driver_spi.h"

#define ABSOLUTE_ENCODER_USE_SOFT_SPI       (0)                                 // 默认使用硬件 SPI 方式驱动
#if ABSOLUTE_ENCODER_USE_SOFT_SPI                                               // 这两段 颜色正常的才是正确的 颜色灰的就是没有用的
//====================================================软件 SPI 驱动====================================================
#define ABSOLUTE_ENCODER_SOFT_SPI_DELAY     (1)                                 // 软件 SPI 的时钟延时周期 数值越小 SPI 通信速率越快
#define ABSOLUTE_ENCODER_SCLK_PIN           (B13)                               // 硬件 SPI SCK 引脚
#define ABSOLUTE_ENCODER_MOSI_PIN           (B15)                               // 硬件 SPI MOSI 引脚
#define ABSOLUTE_ENCODER_MISO_PIN           (B14)                               // 硬件 SPI MISO 引脚
//====================================================软件 SPI 驱动====================================================
#else
//====================================================硬件 SPI 驱动====================================================
#define ABSOLUTE_ENCODER_SPI_SPEED          (system_clock / 6)                  // 硬件 SPI 速率
#define ABSOLUTE_ENCODER_SPI                (SPI_2)                             // 硬件 SPI 号
#define ABSOLUTE_ENCODER_SCLK_PIN           (SPI2_SCK_B13)                      // 硬件 SPI SCK 引脚
#define ABSOLUTE_ENCODER_MOSI_PIN           (SPI2_MOSI_B15)                     // 硬件 SPI MOSI 引脚
#define ABSOLUTE_ENCODER_MISO_PIN           (SPI2_MISO_B14)                     // 硬件 SPI MISO 引脚
//====================================================硬件 SPI 驱动====================================================
#endif

#define ABSOLUTE_ENCODER_CS_PIN             (B12)
#define ABSOLUTE_ENCODER_CSN(x)             ((x) ? (gpio_high(ABSOLUTE_ENCODER_CS_PIN)): (gpio_low(ABSOLUTE_ENCODER_CS_PIN)))

#define ABSOLUTE_ENCODER_TIMEOUT_COUNT      (100)
#define ABSOLUTE_ENCODER_DEFAULT_ZERO       (0)

//====================================================角度传感器参数====================================================
#define ABS_ENCODER_SPI_W                   (0x80)
#define ABS_ENCODER_SPI_R                   (0x40)

#define ZERO_L_REG                          (0x00)
#define ZERO_H_REG                          (0x01)
#define DIR_REG                             (0X09)
//====================================================角度传感器参数====================================================

int16       absolute_encoder_get_location   (void);
int16       absolute_encoder_get_offset     (void);
uint8       absolute_encoder_init           (void);

#endif
