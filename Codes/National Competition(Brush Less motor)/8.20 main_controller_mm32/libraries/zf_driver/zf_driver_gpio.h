/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_driver_gpio
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#ifndef _zf_driver_gpio_h_
#define _zf_driver_gpio_h_

#include "hal_device_registers.h"

#include "zf_common_typedef.h"

typedef enum                                                                    // 枚举 GPIO 引脚  此枚举定义不允许用户修改
{
    A0 = 32* 0, A1 ,    A2 ,    A3 ,    A4 ,    A5 ,    A6 ,    A7 ,
    A8 ,        A9 ,    A10,    A11,    A12,    A13,    A14,    A15,

    B0 = 32* 1, B1 ,    B2 ,    B3 ,    B4 ,    B5 ,    B6 ,    B7 ,
    B8 ,        B9 ,    B10,    B11,    B12,    B13,    B14,    B15,

    C0 = 32* 2, C1 ,    C2 ,    C3 ,    C4 ,    C5 ,    C6 ,    C7 ,
    C8 ,        C9 ,    C10,    C11,    C12,    C13,    C14,    C15,

    D0 = 32* 3, D1 ,    D2 ,    D3 ,    D4 ,    D5 ,    D6 ,    D7 ,
    D8 ,        D9 ,    D10,    D11,    D12,    D13,    D14,    D15,

    E0 = 32* 4, E1 ,    E2 ,    E3 ,    E4 ,    E5 ,    E6 ,    E7 ,
    E8 ,        E9 ,    E10,    E11,    E12,    E13,    E14,    E15,

    F0 = 32* 5, F1 ,    F2 ,    F3 ,    F4 ,    F5 ,    F6 ,    F7 ,
    F8 ,        F9 ,    F10,    F11,    F12,    F13,    F14,    F15,

    G0 = 32* 6, G1 ,    G2 ,    G3 ,    G4 ,    G5 ,    G6 ,    G7 ,
    G8 ,        G9 ,    G10,    G11,    G12,    G13,    G14,    G15,

    H0 = 32* 7, H1 ,    H2 ,    H3 ,
}gpio_pin_enum;

typedef enum                                                                    // 枚举端口方向   此枚举定义不允许用户修改
{
    GPI = 0x00,                                                                 // 定义管脚输入
    GPO = 0x01,                                                                 // 定义管脚输出
}gpio_dir_enum;

typedef enum                                                                    // 枚举端口模式   此枚举定义不允许用户修改
{
    GPI_ANAOG_IN        = 0x00,                                                 // 0x0[输入] 0[定义管脚模拟输入]
    GPI_FLOATING_IN     = 0x04,                                                 // 0x0[输入] 4[定义管脚浮空输入]
    GPI_PULL_DOWN       = 0x08,                                                 // 0x0[输入] 8[定义管脚下拉输入]
    GPI_PULL_UP         = 0x09,                                                 // 0x0[输入] 9[定义管脚上拉输入]

    GPO_PUSH_PULL       = 0x10,                                                 // 0x1[输出] 0[定义管脚推挽输出]
    GPO_OPEN_DTAIN      = 0x14,                                                 // 0x1[输出] 4[定义管脚开漏输出]
    GPO_AF_PUSH_PULL    = 0x18,                                                 // 0x1[输出] 8[定义管脚复用推挽输出]
    GPO_AF_OPEN_DTAIN   = 0x1C,                                                 // 0x1[输出] C[定义管脚复用开漏输出]
}gpio_mode_enum;

typedef enum                                                                    // 枚举端口复用   此枚举定义不允许用户修改
{
    GPIO_AF0            = 0x00,                                                 // 引脚复用功能选项  0
    GPIO_AF1            = 0x01,                                                 // 引脚复用功能选项  1
    GPIO_AF2            = 0x02,                                                 // 引脚复用功能选项  2
    GPIO_AF3            = 0x03,                                                 // 引脚复用功能选项  3
    GPIO_AF4            = 0x04,                                                 // 引脚复用功能选项  4
    GPIO_AF5            = 0x05,                                                 // 引脚复用功能选项  5
    GPIO_AF6            = 0x06,                                                 // 引脚复用功能选项  6
    GPIO_AF7            = 0x07,                                                 // 引脚复用功能选项  7
    GPIO_AF8            = 0x08,                                                 // 引脚复用功能选项  8
    GPIO_AF9            = 0x09,                                                 // 引脚复用功能选项  9
    GPIO_AF10           = 0x0A,                                                 // 引脚复用功能选项 10
    GPIO_AF11           = 0x0B,                                                 // 引脚复用功能选项 11
    GPIO_AF12           = 0x0C,                                                 // 引脚复用功能选项 12
    GPIO_AF13           = 0x0D,                                                 // 引脚复用功能选项 13
    GPIO_AF14           = 0x0E,                                                 // 引脚复用功能选项 14
    GPIO_AF15           = 0x0F,                                                 // 引脚复用功能选项 15
}gpio_af_enum;

typedef enum                                                                    // 枚举端口电平   此枚举定义不允许用户修改
{
    GPIO_LOW            = 0x00,                                                 // 定义管脚初始化电平为低
    GPIO_HIGH           = 0x01,                                                 // 定义管脚初始化电平为高
}gpio_level_enum;

extern GPIO_Type *gpio_group[8];

#define     gpio_idr_addr(x)            (0x40040008 + (((x) & 0xE0) >> 5) * 0x400 + (((x) & 0x1F) / 8))
#define     gpio_odr_addr(x)            (0x4004000C + (((x) & 0xE0) >> 5) * 0x400 + (((x) & 0x1F) / 8))
#define     gpio_bsrr_addr(x)           (0x40040010 + (((x) & 0xE0) >> 5) * 0x400 + (((x) & 0x1F) / 8))
#define     gpio_brr_addr(x)            (0x40040014 + (((x) & 0xE0) >> 5) * 0x400 + (((x) & 0x1F) / 8))

#define     gpio_low(x)                 (gpio_group[((x) >> 5)]->BRR  |= ((uint16)0x0001 << ((x) & 0x1F)))  // GPIO复位
#define     gpio_high(x)                (gpio_group[((x) >> 5)]->BSRR  |= ((uint16)0x0001 << ((x) & 0x1F))) // GPIO置位

void        gpio_set_level              (gpio_pin_enum pin, const uint8 dat);
uint8       gpio_get_level              (gpio_pin_enum pin);
void        gpio_toggle_level           (gpio_pin_enum pin);
void        gpio_set_dir                (gpio_pin_enum pin, gpio_dir_enum dir, gpio_mode_enum mode);

void        gpio_init                   (gpio_pin_enum pin, gpio_dir_enum dir, const uint8 dat, gpio_mode_enum mode);
void        afio_init                   (gpio_pin_enum pin, gpio_dir_enum dir, gpio_af_enum af, gpio_mode_enum mode);

#ifdef COMPATIBLE_WITH_OLDER_VERSIONS                                           // 兼容旧版本开源库接口名称
#define     gpio_set(pin, dat)          (gpio_set_level((pin), (dat)))
#define     gpio_get(pin)               (gpio_get_level((pin)))
#define     gpio_dir(pin, dir, mode)    (gpio_set_dir((pin), (dir), (mode)))
#define     gpio_toggle(pin)            (gpio_toggle_level((pin)))
#endif

#endif
