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

#include "zf_driver_gpio.h"

GPIO_Type *gpio_group[8] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH};

//-------------------------------------------------------------------------------------------------------------------
// @brief       gpio 输出设置
// @param       pin         选择的引脚 (可选择范围由 zf_driver_gpio.h 内 gpio_pin_enum 枚举值确定)
// @param       dat         0：低电平 1：高电平
// @return      void
// Sample usage:            gpio_set_level(D5, 1);                              // D5 输出高电平
//-------------------------------------------------------------------------------------------------------------------
void gpio_set_level (gpio_pin_enum pin, const uint8 dat)
{
    if(dat)
        gpio_high(pin);                                                         // 输出高电平
    else
        gpio_low(pin);                                                          // 输出低电平
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       gpio 电平获取
// @param       pin         选择的引脚 (可选择范围由 zf_driver_gpio.h 内 gpio_pin_enum 枚举值确定)
// Sample usage:            uint8 status = gpio_get_level(D5);                  // 获取 D5 引脚电平
//-------------------------------------------------------------------------------------------------------------------
uint8 gpio_get_level (gpio_pin_enum pin)
{
    if(gpio_group[(pin & 0xE0) >> 5]->IDR & (((uint16)0x0001) << (pin & 0x1F))) // 读取引脚值
        return 1;
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       gpio 翻转电平
// @param       pin         选择的引脚 (可选择范围由 zf_driver_gpio.h 内 gpio_pin_enum 枚举值确定)
// @return      void
// Sample usage:            gpio_toggle_level(D5);                              // 翻转 D5 电平
//-------------------------------------------------------------------------------------------------------------------
void gpio_toggle_level (gpio_pin_enum pin)
{
    if(gpio_group[(pin & 0xE0) >> 5]->IDR & (((uint16)0x0001) << (pin & 0x1F)))
        gpio_group[(pin & 0xE0) >> 5]->BRR |= (((uint16)0x0001) << (pin & 0x1F));   // 输出低电平
    else
        gpio_group[(pin & 0xE0) >> 5]->BSRR |= (((uint16)0x0001) << (pin & 0x1F));  // 输出高电平
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       gpio 方向设置
// @param       pin         选择的引脚 (可选择范围由 zf_driver_gpio.h 内 gpio_pin_enum 枚举值确定)
// @param       dir         引脚的方向   输出：GPO   输入：GPI
// @param       mode        引脚的模式 (可选择范围由 zf_driver_gpio.h 内 gpio_mode_enum 枚举值确定)
// @return      void
// Sample usage:            gpio_set_dir(D5, GPI, GPI_PULL_UP);                 // 设置 D5 为上拉输入
//-------------------------------------------------------------------------------------------------------------------
void gpio_set_dir (gpio_pin_enum pin, gpio_dir_enum dir, gpio_mode_enum mode)
{
    uint32 temp = 0;
    uint8 io_group = ((pin & 0xE0) >> 5);                                       // 提取IO分组
    uint8 io_pin = (pin & 0x1F);                                                // 提取IO引脚下标
    if(0x08 > io_pin)                                                           // 低8位引脚
    {
        temp = gpio_group[io_group]->CRL;
        temp &= ~(0x0000000f << (io_pin * 4));                                  // 清空模式
        temp |= (((uint32)dir | (uint32)(mode & 0x0C)) << (io_pin * 4));        // 写入对应模式
        gpio_group[io_group]->CRL = temp;
    }
    else                                                                        // 高8位引脚
    {
        temp = gpio_group[io_group]->CRH;
        temp &= ~(0x0000000f << ((io_pin - 8) * 4));                            // 清空模式
        temp |= (((uint32)dir | (uint32)(mode & 0x0C)) << ((io_pin - 8) * 4));  // 写入对应模式
        gpio_group[io_group]->CRH = temp;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       gpio 初始化
// @param       pin         选择的引脚 (可选择范围由 zf_driver_gpio.h 内 gpio_pin_enum 枚举值确定)
// @param       mode        引脚的方向 [GPI/GPIO]
// @param       dat         引脚初始化时设置的电平状态，输出时有效 0：低电平 1：高电平
// @param       mode        引脚的模式 (可选择范围由 zf_driver_gpio.h 内 gpio_mode_enum 枚举值确定)
// @return      void
// Sample usage:            gpio_init(D1, GPI, GPIO_HIGH, GPI_PULL_UP);
//-------------------------------------------------------------------------------------------------------------------
void gpio_init (gpio_pin_enum pin, gpio_dir_enum dir, const uint8 dat, gpio_mode_enum mode)
{
    uint32 temp = 0;
    uint8 io_group = ((pin & 0xE0) >> 5);                                       // 提取IO分组
    uint8 io_pin = (pin & 0x1F);                                                // 提取IO引脚下标

    RCC->AHB1ENR |= (0x00000001 << io_group);                                   // 使能 GPIOx 时钟

    if(dat || mode == GPI_PULL_UP)
        gpio_high(pin);                                                         // 初始化电平设置高
    else
        gpio_low(pin);                                                          // 初始化电平设置低

    if(0x08 > io_pin)                                                           // 低8位引脚
    {
        temp = gpio_group[io_group]->AFRL;
        temp &= ~(0x0000000F << (io_pin * 4));                                  // 清空 AF 复用
        temp |= ((uint32)15 << (io_pin * 4));                                   // AF 复用设置 15 保留
        gpio_group[io_group]->AFRL = temp;

        temp = gpio_group[io_group]->CRL;
        temp &= ~(0x0000000F << (io_pin * 4));                                  // 清空模式
        temp |= (((uint32)dir | (uint32)(mode & 0x0C)) << (io_pin * 4));        // 写入对应模式
        gpio_group[io_group]->CRL = temp;
    }
    else                                                                        // 高8位引脚
    {
        temp = gpio_group[io_group]->AFRH;
        temp &= ~(0x0000000F << ((io_pin - 8) * 4));                            // 清空 AF 复用
        temp |= ((uint32)15 << ((io_pin - 8) * 4));                             // AF 复用设置 15 保留
        gpio_group[io_group]->AFRH = temp;

        temp = gpio_group[io_group]->CRH;
        temp &= ~(0x0000000F << ((io_pin - 8) * 4));                            // 清空模式
        temp |= (((uint32)dir | (uint32)(mode & 0x0C)) << ((io_pin - 8) * 4));  // 写入对应模式
        gpio_group[io_group]->CRH = temp;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       gpio 复用功能初始化 一般是内部调用
// @param       pin         选择的引脚 (可选择范围由 zf_driver_gpio.h 内 gpio_pin_enum 枚举值确定)
// @param       mode        引脚的方向 [GPI/GPIO]
// @param       af          引脚的功能选择 (可选择范围由 zf_driver_gpio.h 内 gpio_af_enum 枚举值确定)
// @param       mode        引脚的模式 (可选择范围由 zf_driver_gpio.h 内 gpio_mode_enum 枚举值确定)
// @return      void
// Sample usage:            afio_init(D5, GPO, GPIO_AF0, GPO_AF_PUSH_PULL);
//-------------------------------------------------------------------------------------------------------------------
void afio_init (gpio_pin_enum pin, gpio_dir_enum dir, gpio_af_enum af, gpio_mode_enum mode)
{
    uint32 temp = 0;
    uint8 io_group = ((pin & 0xE0) >> 5);                                       // 提取IO分组
    uint8 io_pin = (pin & 0x1F);                                                // 提取IO引脚下标

    RCC->AHB1ENR |= (0x00000001 << io_group);                                   // 使能 GPIOx 时钟

    if(0x08 > io_pin)                                                           // 低8位引脚
    {
        temp = gpio_group[io_group]->AFRL;
        temp &= ~(0x0000000F << (io_pin * 4));                                  // 清空 AF 复用
        temp |= ((uint32)af << (io_pin * 4));                                   // AF 复用设置对应功能
        gpio_group[io_group]->AFRL = temp;

        temp = gpio_group[io_group]->CRL;
        temp &= ~(0x0000000F << (io_pin * 4));                                  // 清空模式
        temp |= (((uint32)dir | (uint32)(mode & 0x0C)) << (io_pin * 4));        // 写入对应模式
        gpio_group[io_group]->CRL = temp;
    }
    else                                                                        // 高8位引脚
    {
        temp = gpio_group[io_group]->AFRH;
        temp &= ~(0x0000000F << ((io_pin - 8) * 4));                            // 清空 AF 复用
        temp |= ((uint32)af << ((io_pin - 8) * 4));                             // AF 复用设置对应功能
        gpio_group[io_group]->AFRH = temp;

        temp = gpio_group[io_group]->CRH;
        temp &= ~(0x0000000F << ((io_pin - 8) * 4));                            // 清空模式
        temp |= (((uint32)dir | (uint32)(mode & 0x0C)) << ((io_pin - 8) * 4));  // 写入对应模式
        gpio_group[io_group]->CRH = temp;
    }
}
