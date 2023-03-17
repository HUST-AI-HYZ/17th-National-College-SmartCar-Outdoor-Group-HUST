/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_ips114
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
*                   SCL                  查看 zf_device_ips114.h 中 IPS114_SCL_PIN 宏定义
*                   SDA                  查看 zf_device_ips114.h 中 IPS114_SDA_PIN 宏定义
*                   RST                  查看 zf_device_ips114.h 中 IPS114_RST_PIN 宏定义
*                   DC                   查看 zf_device_ips114.h 中 IPS114_DC_PIN 宏定义
*                   CS                   查看 zf_device_ips114.h 中 IPS114_CS_PIN 宏定义
*                   BLK                  查看 zf_device_ips114.h 中 IPS114_BLK_PIN 宏定义
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   最大分辨率 135 * 240
*                   ------------------------------------
********************************************************************************************************************/

#ifndef _zf_device_ips114_h_
#define _zf_device_ips114_h_

#include "zf_common_clock.h"
#include "zf_common_debug.h"
#include "zf_common_font.h"
#include "zf_common_function.h"

#include "zf_driver_delay.h"
#include "zf_driver_soft_spi.h"
#include "zf_driver_spi.h"

#define IPS114_USE_SOFT_SPI             (0)                                     // 默认使用硬件 SPI 方式驱动 建议使用硬件 SPI 方式驱动
#if IPS114_USE_SOFT_SPI                                                         // 这两段 颜色正常的才是正确的 颜色灰的就是没有用的
//====================================================软件 SPI 驱动====================================================
#define IPS114_SOFT_SPI_DELAY           (1 )                                    // 软件 SPI 的时钟延时周期 数值越小 SPI 通信速率越快
#define IPS114_SCL_PIN                  (D4)                                    // 软件 SPI SCK 引脚
#define IPS114_SDA_PIN                  (D6)                                    // 软件 SPI MOSI 引脚
//====================================================软件 SPI 驱动====================================================
#else
//====================================================硬件 SPI 驱动====================================================
#define IPS114_SPI_SPEED                (system_clock / 2)                      // 硬件 SPI 速率 这里设置为系统时钟二分频
#define IPS114_SPI                      (SPI_3)                                 // 硬件 SPI 号
#define IPS114_SCL_PIN                  (SPI3_SCK_D4)                           // 硬件 SPI SCK 引脚
#define IPS114_SDA_PIN                  (SPI3_MOSI_D6)                          // 硬件 SPI MOSI 引脚
//====================================================硬件 SPI 驱动====================================================
#endif

#define IPS114_RST_PIN                  (D13)                                   // 液晶复位引脚定义
#define IPS114_DC_PIN                   (D5 )                                   // 液晶命令位引脚定义
#define IPS114_CS_PIN                   (D7 )                                   // CS 片选引脚
#define IPS114_BLK_PIN                  (D11)                                   // 液晶背光引脚定义

#define IPS114_DC(x)                    ((x) ? (gpio_high(IPS114_DC_PIN)) : (gpio_low(IPS114_DC_PIN)))
#define IPS114_RST(x)                   ((x) ? (gpio_high(IPS114_RST_PIN)) : (gpio_low(IPS114_RST_PIN)))
#define IPS114_CS(x)                    ((x) ? (gpio_high(IPS114_CS_PIN)) : (gpio_low(IPS114_CS_PIN)))
#define IPS114_BLK(x)                   ((x) ? (gpio_high(IPS114_BLK_PIN)) : (gpio_low(IPS114_BLK_PIN)))

typedef enum
{
    IPS114_PORTAIT                      = 0,                                    // 竖屏模式
    IPS114_PORTAIT_180                  = 1,                                    // 竖屏模式  旋转180
    IPS114_CROSSWISE                    = 2,                                    // 横屏模式
    IPS114_CROSSWISE_180                = 3,                                    // 横屏模式  旋转180
}ips114_dir_enum;

#define IPS114_DEFAULT_DISPLAY_DIR      (IPS114_CROSSWISE_180)                  // 默认的显示方向
#define IPS114_DEFAULT_PENCOLOR         (RGB565_RED)                            // 默认的画笔颜色
#define IPS114_DEFAULT_BGCOLOR          (RGB565_WHITE)                          // 默认的背景颜色

void    ips114_clear                    (void);
void    ips114_full                     (const uint16 color);
void    ips114_set_dir                  (ips114_dir_enum dir);
void    ips114_set_color                (const uint16 pen, const  uint16 bgcolor);
void    ips114_draw_point               (uint16 x, uint16 y, const uint16 color);
void    ips114_draw_line                (uint16 x_start, uint16 y_start, uint16 x_end, uint16 y_end, const uint16 color);

void    ips114_show_char                (uint16 x, uint16 y, const char dat);
void    ips114_show_string              (uint16 x, uint16 y, const char dat[]);
void    ips114_show_int                 (uint16 x,uint16 y, const int32 dat, uint8 num);
void    ips114_show_uint                (uint16 x,uint16 y, const uint32 dat, uint8 num);
void    ips114_show_float               (uint16 x,uint16 y, const float dat, uint8 num, uint8 pointnum);

void    ips114_show_binary_image        (uint16 x, uint16 y, const uint8 *image, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height);
void    ips114_show_gray_image          (uint16 x, uint16 y, const uint8 *image, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height, uint8 threshold);
void    ips114_show_rgb565_image        (uint16 x, uint16 y, const uint16 *image, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height, uint8 color_mode);

void    ips114_show_wave                (uint16 x, uint16 y, const uint16 *wave, uint16 width, uint16 value_max, uint16 dis_width, uint16 dis_value_max);
void    ips114_show_chinese             (uint16 x, uint16 y, uint8 size, const uint8 *chinese_buffer, uint8 number, const uint16 color);

#define ips114_displayimage7725(p, width, height)                                       (ips114_show_binary_image(0, 0, (p), (width), (height), (width), (height)))

#define ips114_displayimage032(p, width, height)                                        (ips114_show_gray_image(0, 0, (p), (width), (height), (width), (height), 0))
#define ips114_displayimage032_zoom(p, width, height, dis_width, dis_height)            (ips114_show_gray_image(0, 0, (p), (width), (height), (dis_width), (dis_height), 0))
#define ips114_displayimage032_zoom1(x, y, p, width, height, dis_width, dis_height)     (ips114_show_gray_image((x), (y), (p), (width), (height), (dis_width), (dis_height), 0))

#define ips114_displayimage8660(p, width, height)                                       (ips114_show_rgb565_image(0, 0, (p), (width), (height), (width), (height), 1))
#define ips114_displayimage8660_zoom(p, width, height, dis_width, dis_height)           (ips114_show_rgb565_image(0, 0, (p), (width), (height), (dis_width), (dis_height), 1))
#define ips114_displayimage8660_zoom1(x, y, p, width, height, dis_width, dis_height)    (ips114_show_rgb565_image((x), (y), (p), (width), (height), (dis_width), (dis_height), 1))

void    ips114_init                     (void);

#endif
