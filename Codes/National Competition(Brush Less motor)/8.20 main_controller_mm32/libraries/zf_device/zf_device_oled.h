/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_oled
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
* @note             接线定义：
*                   ------------------------------------
*                   模块管脚             单片机管脚
*                   D0                  查看 zf_device_oled.h 中 OLED_D0_PIN 宏定义
*                   D1                  查看 zf_device_oled.h 中 OLED_D1_PIN 宏定义
*                   RES                 查看 zf_device_oled.h 中 OLED_RES_PIN 宏定义
*                   DC                  查看 zf_device_oled.h 中 OLED_DC_PIN 宏定义
*                   CS                  查看 zf_device_oled.h 中 OLED_CS_PIN 宏定义
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   ------------------------------------
********************************************************************************************************************/

#ifndef _zf_device_oled_h_
#define _zf_device_oled_h_

#include "zf_common_clock.h"
#include "zf_common_debug.h"
#include "zf_common_font.h"
#include "zf_common_function.h"

#include "zf_driver_delay.h"
#include "zf_driver_soft_spi.h"
#include "zf_driver_spi.h"

#define OLED_USE_SOFT_SPI               (1 )                                    // 默认使用硬件 SPI 方式驱动 建议使用硬件 SPI 方式驱动
#if OLED_USE_SOFT_SPI                                                           // 这两段 颜色正常的才是正确的 颜色灰的就是没有用的
//====================================================软件 SPI 驱动====================================================
#define OLED_SOFT_SPI_DELAY             (1 )                                    // 软件 SPI 的时钟延时周期 数值越小 SPI 通信速率越快
#define OLED_D0_PIN                     (D4)                                    // 软件 SPI SCK 引脚
#define OLED_D1_PIN                     (D6)                                    // 软件 SPI MOSI 引脚
//====================================================软件 SPI 驱动====================================================
#else
//====================================================硬件 SPI 驱动====================================================
#define OLED_SPI_SPEED                  (system_clock / 2)                      // 硬件 SPI 速率
#define OLED_SPI                        (SPI_3)                                 // 硬件 SPI 号
#define OLED_D0_PIN                     (SPI3_SCK_D4 )                          // 硬件 SPI SCK 引脚
#define OLED_D1_PIN                     (SPI3_MOSI_D6)                          // 硬件 SPI MOSI 引脚
//====================================================硬件 SPI 驱动====================================================
#endif

#define OLED_RES_PIN                    (D13)                                   // 液晶复位引脚定义
#define OLED_DC_PIN                     (D5 )                                   // 液晶命令位引脚定义
#define OLED_CS_PIN                     (D7 )                                   // CS 片选引脚

#define OLED_RES(x)                     ((x) ? (gpio_high(OLED_RES_PIN)) : (gpio_low(OLED_RES_PIN)))
#define OLED_DC(x)                      ((x) ? (gpio_high(OLED_DC_PIN)) : (gpio_low(OLED_DC_PIN)))
#define OLED_CS(x)                      ((x) ? (gpio_high(OLED_CS_PIN)) : (gpio_low(OLED_CS_PIN)))

typedef enum
{
    OLED_CROSSWISE                      = 0,                                    // 横屏模式
    OLED_CROSSWISE_180                  = 1,                                    // 横屏模式  旋转180
}oled_dir_enum;

typedef enum
{
    OLED_6x8_FONT                       = 0,                                    // 6x8 字体
    OLED_8x16_FONT                      = 1,                                    // 8x16 字体
}oled_font_size_enum;

#define OLED_BRIGHTNESS                 (0x7f)                                  // 设置OLED亮度 越大越亮 范围0-0XFF
#define OLED_DEFAULT_DISPLAY_DIR        (OLED_CROSSWISE)                        // 默认的显示方向
#define OLED_DEFAULT_DISPLAY_FONT       (OLED_6x8_FONT )                        // 默认的字体模式
#define OLED_X_MAX                      (128)
#define OLED_Y_MAX                      (64 )

void    oled_clear                      (void);
void    oled_full                       (const uint8 color);
void    oled_set_dir                    (oled_dir_enum dir);
void    oled_set_font                   (oled_font_size_enum font);
void    oled_draw_point                 (uint16 x, uint16 y, const uint8 color);

void    oled_show_string                (uint16 x, uint16 y, const char ch[]);
void    oled_show_int                   (uint16 x, uint16 y, const int32 dat, uint8 num);
void    oled_show_uint                  (uint16 x, uint16 y, const uint32 dat, uint8 num);
void    oled_show_float                 (uint16 x, uint16 y, const float dat, uint8 num, uint8 pointnum);

void    oled_show_binary_image          (uint16 x, uint16 y, const uint8 *image, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height);
void    oled_show_gray_image            (uint16 x, uint16 y, const uint8 *image, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height, uint8 threshold);

void    oled_show_wave                  (uint16 x, uint16 y, const uint16 *image, uint16 width, uint16 value_max, uint16 dis_width, uint16 dis_value_max);
void    oled_show_chinese               (uint16 x, uint16 y, uint8 size, const uint8 *chinese_buffer, uint8 number);

#define oled_displayimage7725(p,width,height)                                   (oled_show_binary_image(0, 0, (p), (width), (height), 128, 64))

#define oled_displayimage032(p,width,height,x)                                  (oled_show_gray_image(0, 0, (p), (width), (height), 128, 64, (x)))
#define oled_displayimage032_zoom(p,width,height,dis_width,dis_height,x)        (oled_show_gray_image(0, 0, (p), (width), (height), (dis_width,) (dis_height), (x)))

void    oled_init                       (void);

#endif
