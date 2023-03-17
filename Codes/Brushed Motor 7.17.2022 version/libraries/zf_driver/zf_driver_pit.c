/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_driver_pit
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#include "zf_driver_timer.h"
#include "zf_driver_pit.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief       PIT 中断使能
// @param       pit_n           PIT 外设模块号
// @return      void
// Sample usage:                pit_enable(TIM1_PIT);
//-------------------------------------------------------------------------------------------------------------------
void pit_enable (pit_index_enum pit_n)
{
    switch(pit_n)
    {
        case TIM1_PIT: TIM1->CR1 |= (0x00000001);  return;
        case TIM2_PIT: TIM2->CR1 |= (0x00000001);  return;
        case TIM3_PIT: TIM3->CR1 |= (0x00000001);  return;
        case TIM4_PIT: TIM4->CR1 |= (0x00000001);  return;
        case TIM5_PIT: TIM5->CR1 |= (0x00000001);  return;
        case TIM6_PIT: TIM6->CR1 |= (0x00000001);  return;
        case TIM7_PIT: TIM7->CR1 |= (0x00000001);  return;
        case TIM8_PIT: TIM8->CR1 |= (0x00000001);  return;
        default: return;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       PIT 中断禁止
// @param       pit_n           PIT 外设模块号
// @return      void
// Sample usage:                pit_disable(TIM1_PIT);
//-------------------------------------------------------------------------------------------------------------------
void pit_disable (pit_index_enum pit_n)
{
    switch(pit_n)
    {
        case TIM1_PIT: TIM1->CR1 &= ~(0x00000001); return;
        case TIM2_PIT: TIM2->CR1 &= ~(0x00000001); return;
        case TIM3_PIT: TIM3->CR1 &= ~(0x00000001); return;
        case TIM4_PIT: TIM4->CR1 &= ~(0x00000001); return;
        case TIM5_PIT: TIM5->CR1 &= ~(0x00000001); return;
        case TIM6_PIT: TIM6->CR1 &= ~(0x00000001); return;
        case TIM7_PIT: TIM7->CR1 &= ~(0x00000001); return;
        case TIM8_PIT: TIM8->CR1 &= ~(0x00000001); return;
        default: return;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       PIT 初始化 一般调用 pit_init_ms 或 pit_init_us
// @param       pit_n           PIT 外设模块号
// @param       period          PIT 周期 一般是芯片或者模块时钟频率计算
// @return      void
// Sample usage:                pit_init(TIM1_PIT, 120);
//-------------------------------------------------------------------------------------------------------------------
void pit_init (pit_index_enum pit_n, uint32 period)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 就去查看你在什么地方调用这个函数 检查你的传入参数
    // 这里是检查是否有重复使用定时器
    // 比如初始化了 TIM1_PWM 然后又初始化成 TIM1_PIT 这种用法是不允许的
    zf_assert(timer_funciton_check((timer_index_enum)pit_n, TIMER_FUNCTION_TIMER));
    // 如果是这一行报错 那我就得问问你为什么周期写的是 0
    zf_assert(period != 0);

    if(SYSTEM_CLOCK_120M < system_clock)
        period /= 2;
    uint16 freq_div = (period >> 15);                                           // 计算预分频
    uint16 period_temp = (period / (freq_div + 1));                             // 计算自动重装载值

    timer_clock_enable((timer_index_enum)pit_n);                                // 使能时钟
    TIM1_Type *tim_index;
    switch(pit_n)
    {
        case TIM1_PIT: tim_index = ((TIM1_Type *)TIM1_BASE);   break;
        case TIM2_PIT: tim_index = ((TIM1_Type *)TIM2_BASE);   break;
        case TIM3_PIT: tim_index = ((TIM1_Type *)TIM3_BASE);   break;
        case TIM4_PIT: tim_index = ((TIM1_Type *)TIM4_BASE);   break;
        case TIM5_PIT: tim_index = ((TIM1_Type *)TIM5_BASE);   break;
        case TIM6_PIT: tim_index = ((TIM1_Type *)TIM6_BASE);   break;
        case TIM7_PIT: tim_index = ((TIM1_Type *)TIM7_BASE);   break;
        case TIM8_PIT: tim_index = ((TIM1_Type *)TIM8_BASE);   break;
        default: return;
    }

    tim_index->ARR = period_temp;                                               // 装载自动重装载值
    tim_index->PSC = freq_div;                                                  // 装载预分频
    tim_index->DIER |= (0x00000001);                                            // 使能中断更新

    IRQn_Type irq_index[8] = {
        TIM1_UP_IRQn,   TIM2_IRQn,  TIM3_IRQn,  TIM4_IRQn,
        TIM5_IRQn,      TIM6_IRQn,  TIM7_IRQn,  TIM8_UP_IRQn };

    interrupt_enable(irq_index[(pit_n)]);

//    tim_index->EGR |= 0x01;
    tim_index->CR1 |= (0x00000001);                                             // 使能定时器
}
