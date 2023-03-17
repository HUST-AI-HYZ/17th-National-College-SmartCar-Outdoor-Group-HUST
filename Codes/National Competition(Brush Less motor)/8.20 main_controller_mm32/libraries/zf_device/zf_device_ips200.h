/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_ips200
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
*                   // 双排排针 并口两寸屏 硬件引脚
*                   RD                  查看 zf_device_ips200.h 中 IPS200_RD_PIN_PARALLEL8     宏定义
*                   WR                  查看 zf_device_ips200.h 中 IPS200_WR_PIN_PARALLEL8     宏定义
*                   RS                  查看 zf_device_ips200.h 中 IPS200_RS_PIN_PARALLEL8     宏定义
*                   RST                 查看 zf_device_ips200.h 中 IPS200_RST_PIN_PARALLEL8    宏定义
*                   CS                  查看 zf_device_ips200.h 中 IPS200_CS_PIN_PARALLEL8     宏定义
*                   BL                  查看 zf_device_ips200.h 中 IPS200_BL_PIN_PARALLEL8     宏定义
*                   D0-D7               查看 zf_device_ips200.h 中 IPS200_Dx_PIN_PARALLEL8     宏定义
*                   VCC                 3.3V电源
*                   GND                 电源地
*
*                   // 单排排针 SPI 两寸屏 硬件引脚
*                   SCL                 查看 zf_device_ips200.h 中 IPS200_SCL_PIN_SPI  宏定义
*                   SDA                 查看 zf_device_ips200.h 中 IPS200_SDA_PIN_SPI  宏定义
*                   RST                 查看 zf_device_ips200.h 中 IPS200_RST_PIN_SPI  宏定义
*                   DC                  查看 zf_device_ips200.h 中 IPS200_DC_PIN_SPI   宏定义
*                   CS                  查看 zf_device_ips200.h 中 IPS200_CS_PIN_SPI   宏定义
*                   BLk                 查看 zf_device_ips200.h 中 IPS200_BLk_PIN_SPI  宏定义
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   最大分辨率 320 * 240
*                   ------------------------------------
********************************************************************************************************************/

#ifndef _zf_device_ips200_h_
#define _zf_device_ips200_h_

#include "zf_common_clock.h"
#include "zf_common_debug.h"
#include "zf_common_font.h"
#include "zf_common_function.h"

#include "zf_driver_delay.h"
#include "zf_driver_fsmc.h"
#include "zf_driver_gpio.h"
#include "zf_driver_soft_spi.h"
#include "zf_driver_spi.h"

#define IPS200_USE_SOFT_SPI             (0 )                                    // 默认使用硬件 SPI 方式驱动 建议使用硬件 SPI 方式驱动
#if IPS200_USE_SOFT_SPI                                                         // 这两段 颜色正常的才是正确的 颜色灰的就是没有用的
//====================================================软件 SPI 驱动====================================================
// 如果使用的是单排排针的两寸屏幕 SPI 驱动控制引脚 可以修改
#define IPS200_SOFT_SPI_DELAY           (1 )                                    // 软件 SPI 的时钟延时周期 数值越小 SPI 通信速率越快
#define IPS200_SCL_PIN                  (D4)                                    // 软件 SPI SCK 引脚
#define IPS200_SDA_PIN                  (D6)                                    // 软件 SPI MOSI 引脚
//====================================================软件 SPI 驱动====================================================
#else
//====================================================硬件 SPI 驱动====================================================
// 如果使用的是单排排针的两寸屏幕 SPI 驱动控制引脚 可以修改
#define IPS200_SPI_SPEED                (system_clock / 2)                      // 硬件 SPI 速率 这里设置为系统时钟二分频
#define IPS200_SPI                      (SPI_3           )                      // 硬件 SPI 号
#define IPS200_SCL_PIN_SPI              (SPI3_SCK_D4     )                      // 硬件 SPI SCK 引脚
#define IPS200_SDA_PIN_SPI              (SPI3_MOSI_D6    )                      // 硬件 SPI MOSI 引脚
//====================================================硬件 SPI 驱动====================================================
#endif
// 如果使用的是单排排针的两寸屏幕 SPI 驱动控制引脚 可以修改
#define IPS200_RST_PIN_SPI              (D13)                                   // 液晶复位引脚定义
#define IPS200_DC_PIN_SPI               (D5 )                                   // 液晶命令位引脚定义
#define IPS200_CS_PIN_SPI               (D7 )
#define IPS200_BLk_PIN_SPI              (D11)

// 如果使用的是双排排针的两寸屏幕 并口驱动控制引脚 可以修改
#define IPS200_RST_PIN_PARALLEL8        (D5 )
#define IPS200_BL_PIN_PARALLEL8         (D11)
//#define IPS200_RD_PIN_PARALLEL8         (D4 )
//#define IPS200_WR_PIN_PARALLEL8         (D6 )

// 如果使用的是双排排针的两寸屏幕 并口驱动控制引脚  FSMC固定引脚 不可更改
#define IPS200_RD_PIN_PARALLEL8         (D4 )
#define IPS200_WR_PIN_PARALLEL8         (D6 )
#define IPS200_RS_PIN_PARALLEL8         (D13)
#define IPS200_CS_PIN_PARALLEL8         (D7 )

// 如果使用的是双排排针的两寸屏幕 并口驱动数据引脚 FSMC固定引脚 不可更改
#define IPS200_D0_PIN_PARALLEL8         (E11)
#define IPS200_D1_PIN_PARALLEL8         (E12)
#define IPS200_D2_PIN_PARALLEL8         (E13)
#define IPS200_D3_PIN_PARALLEL8         (E14)
#define IPS200_D4_PIN_PARALLEL8         (E15)
#define IPS200_D5_PIN_PARALLEL8         (D8 )
#define IPS200_D6_PIN_PARALLEL8         (D9 )
#define IPS200_D7_PIN_PARALLEL8         (D10)

// 数据对应地址 不可更改
#define IPS200_DATA_ADD                 (0x60080000)
#define IPS200_CMD_ADD                  (0x60000000)

// 控制语句
#define IPS200_RD(x)        ((x) ? (gpio_high(IPS200_RD_PIN_PARALLEL8)) : (gpio_low(IPS200_RD_PIN_PARALLEL8)))
#define IPS200_WR(x)        ((x) ? (gpio_high(IPS200_WR_PIN_PARALLEL8)) : (gpio_low(IPS200_WR_PIN_PARALLEL8)))
#define IPS200_RST(x)       ((x) ? (gpio_high(ips_rst_pin)) : (gpio_low(ips_rst_pin)))
#define IPS200_BL(x)        ((x) ? (gpio_high(ips_bl_pin)) : (gpio_low(ips_bl_pin)))

#define IPS200_DC(x)        ((x) ? (gpio_high(IPS200_DC_PIN_SPI)) : (gpio_low(IPS200_DC_PIN_SPI)))
#define IPS200_CS(x)        ((x) ? (gpio_high(IPS200_CS_PIN_SPI)) : (gpio_low(IPS200_CS_PIN_SPI)))

typedef enum
{
    IPS200_TYPE_SPI,                                                            // SPI 驱动
    IPS200_TYPE_PARALLEL8,                                                      // 并口驱动
}ips200_type_enum;

typedef enum
{
    IPS200_PORTAIT                      = 0,                                    // 竖屏模式
    IPS200_PORTAIT_180                  = 1,                                    // 竖屏模式  旋转180
    IPS200_CROSSWISE                    = 2,                                    // 横屏模式
    IPS200_CROSSWISE_180                = 3,                                    // 横屏模式  旋转180
}ips200_dir_enum;

#define IPS200_DEFAULT_DISPLAY_DIR      (IPS200_PORTAIT)                        // 默认的显示方向
#define IPS200_DEFAULT_PENCOLOR         (RGB565_RED    )                        // 默认的画笔颜色
#define IPS200_DEFAULT_BGCOLOR          (RGB565_WHITE  )                        // 默认的背景颜色

void    ips200_clear                    (void);
void    ips200_full                     (const uint16 color);
void    ips200_set_dir                  (ips200_dir_enum dir);
void    ips200_set_color                (const uint16 pen, const uint16 bgcolor);
void    ips200_draw_point               (uint16 x, uint16 y, const uint16 color);
void    ips200_draw_line                (uint16 x_start, uint16 y_start, uint16 x_end, uint16 y_end, const uint16 color);

void    ips200_show_char                (uint16 x, uint16 y, const char dat);
void    ips200_show_string              (uint16 x, uint16 y, const char dat[]);
void    ips200_show_int                 (uint16 x, uint16 y, const int32 dat, uint8 num);
void    ips200_show_uint                (uint16 x, uint16 y, const uint32 dat, uint8 num);
void    ips200_show_float               (uint16 x, uint16 y, const float dat, uint8 num, uint8 pointnum);

void    ips200_show_binary_image        (uint16 x, uint16 y, const uint8 *image, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height);
void    ips200_show_gray_image          (uint16 x, uint16 y, const uint8 *image, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height, uint8 threshold);
void    ips200_show_rgb565_image        (uint16 x, uint16 y, const uint16 *image, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height, uint8 color_mode);

void    ips200_show_wave                (uint16 x, uint16 y, const uint16 *wave, uint16 width, uint16 value_max, uint16 dis_width, uint16 dis_value_max);
void    ips200_show_chinese             (uint16 x, uint16 y, uint8 size, const uint8 *chinese_buffer, uint8 number, const uint16 color);

#define ips200_displayimage7725(p, width, height)                                       (ips200_show_binary_image(0, 0, (p), (width), (height), (width), (height)))

#define ips200_displayimage032(p, width, height)                                        (ips200_show_gray_image(0, 0, (p), (width), (height), (width), (height), 0))
#define ips200_displayimage032_zoom(p, width, height, dis_width, dis_height)            (ips200_show_gray_image(0, 0, (p), (width), (height), (dis_width), (dis_height), 0))
#define ips200_displayimage032_zoom1(x, y, p, width, height, dis_width, dis_height)     (ips200_show_gray_image((x), (y), (p), (width), (height), (dis_width), (dis_height), 0))

#define ips200_displayimage8660(p, width, height)                                       (ips200_show_rgb565_image(0, 0, (p), (width), (height), (width), (height), 1))
#define ips200_displayimage8660_zoom(p, width, height, dis_width, dis_height)           (ips200_show_rgb565_image(0, 0, (p), (width), (height), (dis_width), (dis_height), 1))
#define ips200_displayimage8660_zoom1(x, y, p, width, height, dis_width, dis_height)    (ips200_show_rgb565_image((x), (y), (p), (width), (height), (dis_width), (dis_height), 1))

void    ips200_init                     (ips200_type_enum type_select);

#endif
