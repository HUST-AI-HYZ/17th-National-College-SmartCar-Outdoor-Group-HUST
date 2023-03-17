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

#include "zf_driver_exti.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief       EXTI 中断使能
// @param       pin             选择 EXTI 引脚 (可选择范围由 zf_driver_gpio.h 内 gpio_pin_enum 枚举值确定)
// @return      void
// Sample usage:                exti_enable(A0);
//-------------------------------------------------------------------------------------------------------------------
void exti_enable (gpio_pin_enum pin)
{
    EXTI->IMR |= (0x00000001 << (pin & 0x1F));
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       EXTI 中断失能
// @param       pin             选择 EXTI 引脚 (可选择范围由 zf_driver_gpio.h 内 gpio_pin_enum 枚举值确定)
// @return      void
// Sample usage:                exti_disable(A0);
//-------------------------------------------------------------------------------------------------------------------
void exti_disable (gpio_pin_enum pin)
{
    EXTI->IMR &= ~(0x00000001 << (pin & 0x1F));
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       EXTI 中断初始化 <同一下标的引脚不能同时初始化为外部中断输入 例如 A0 和 B0 不能同时初始化为外部中断输入>
// @param       pin             选择 EXTI 引脚 (可选择范围由 zf_driver_gpio.h 内 gpio_pin_enum 枚举值确定)
// @param       trigger         选择触发的信号方式 [EXTI_TRIGGER_RISING/EXTI_TRIGGER_FALLING/EXTI_TRIGGER_BOTH]
// @return      void
// Sample usage:                exti_init(A0, EXTI_TRIGGER_RISING);
//-------------------------------------------------------------------------------------------------------------------
void exti_init (gpio_pin_enum pin, exti_trigger_enum trigger)
{
    uint32 register_temp = 0;
    gpio_init(pin, GPI, GPIO_HIGH, GPI_PULL_UP);                                // 初始化选中的引脚

    RCC->APB2ENR |= RCC_APB2ENR_SYSCFG(1);                                      // 先使能 SYSCFG
    EXTI->IMR &= ~(0x00000001 << (pin & 0x1f));                                 // 禁止 Line 的中断

    switch((pin & 0x1F) / 0x04)                                                 // 计算对应在哪个寄存器设置
    {
        case 0: register_temp = SYSCFG->EXTICR1;    break;                      // 0-3 Line
        case 1: register_temp = SYSCFG->EXTICR2;    break;                      // 4-7 Line
        case 2: register_temp = SYSCFG->EXTICR3;    break;                      // 8-11 Line
        case 3: register_temp = SYSCFG->EXTICR4;    break;                      // 12-15 Line
    }
    switch(pin%0x04)
    {
        case 0: 
            register_temp &= 0xFFFFFFF0;                                        // 清空 Line12/8/4/0 原有设置 防止写入触发异常
            register_temp |= ((0x000000f0 & pin) >> 5);                         // Line12/8/4/0 - [ 3: 0] 写入对应引脚组别号
            break;
        case 1: 
            register_temp &= 0xFFFFFF0F;                                        // 清空 Line13/9/5/1 原有设置 防止写入触发异常
            register_temp |= ((0x000000f0 & pin) >> 5) << 4;                    // Line13/9/5/1 - [ 7: 4] 写入对应引脚组别号
            break;
        case 2: 
            register_temp &= 0xFFFFF0FF;                                        // 清空 Line14/10/6/2 原有设置 防止写入触发异常
            register_temp |= ((0x000000f0 & pin) >> 5) << 8;                    // Line14/10/6/2 - [11: 8] 写入对应引脚组别号
            break;
        case 3: 
            register_temp &= 0xFFFF0FFF;                                        // 清空 Line15/11/7/3 原有设置 防止写入触发异常
            register_temp |= ((0x000000f0 & pin) >> 5) << 12;                   // Line15/11/7/3 - [15:12] 写入对应引脚组别号
            break;
    }
    switch((pin & 0x1F) / 0x04)                                                 // 计算对应在哪个寄存器设置
    {
        case 0: SYSCFG->EXTICR1 = register_temp;    break;                      // 0-3 Line
        case 1: SYSCFG->EXTICR2 = register_temp;    break;                      // 4-7 Line
        case 2: SYSCFG->EXTICR3 = register_temp;    break;                      // 8-11 Line
        case 3: SYSCFG->EXTICR4 = register_temp;    break;                      // 12-15 Line
    }

    switch(trigger)
    {
        case EXTI_TRIGGER_RISING:                                               // 上升沿触发模式
            EXTI->RTSR |= (0x00000001 << (pin & 0x1f));                         // 上升沿触发使能
            EXTI->FTSR &= ~(0x00000001 << (pin & 0x1f));                        // 下降沿触发禁止
            break;
        case EXTI_TRIGGER_FALLING:                                              // 下降沿触发模式
            EXTI->RTSR &= ~(0x00000001 << (pin & 0x1f));                        // 上升沿触发禁止
            EXTI->FTSR |= (0x00000001 << (pin & 0x1f));                         // 下降沿触发使能
            break;
        case EXTI_TRIGGER_BOTH:                                                 // 双边沿触发模式
            EXTI->RTSR |= (0x00000001 << (pin & 0x1f));                         // 上升沿触发使能
            EXTI->FTSR |= (0x00000001 << (pin & 0x1f));                         // 下降沿触发使能
            break;
    }
    EXTI->IMR |= (0x00000001 << (pin & 0x1f));                                  // 使能 Line 的中断

   if(1 > (pin & 0x1F))
       interrupt_enable(EXTI0_IRQn);                                            // 使能 Line0 的中断响应
   else if(2 > (pin & 0x1F))
       interrupt_enable(EXTI1_IRQn);                                            // 使能 Line1 的中断响应
   else if(3 > (pin & 0x1F))
       interrupt_enable(EXTI2_IRQn);                                            // 使能 Line2 的中断响应
   else if(4 > (pin & 0x1F))
       interrupt_enable(EXTI3_IRQn);                                            // 使能 Line3 的中断响应
   else if(5 > (pin & 0x1F))
       interrupt_enable(EXTI4_IRQn);                                            // 使能 Line4 的中断响应
   else if(10 > (pin & 0x1F))
       interrupt_enable(EXTI9_5_IRQn);                                          // 使能 Line5-9 的中断响应
   else
       interrupt_enable(EXTI15_10_IRQn);                                        // 使能 Line10-15 的中断响应
}
