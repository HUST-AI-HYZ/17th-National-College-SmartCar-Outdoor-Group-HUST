/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_driver_pwm
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#include "zf_driver_gpio.h"
#include "zf_driver_timer.h"
#include "zf_driver_pwm.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief       PWM 更新占空比
// @param       pin             选择 PWM 引脚
// @param       duty            设置占空比
// @return      void
// Sample usage:                pwm_set_duty(TIM5_PWMCH1_A0, PWM_DUTY_MAX / 4);
//-------------------------------------------------------------------------------------------------------------------
void pwm_set_duty (pwm_channel_enum pin, const uint32 duty)
{
    // 如果是这一行报错 那你得去看看最大占空比是限定的多少 占空比写入错误
    zf_assert(PWM_DUTY_MAX >= duty);                                            // 占空比写入错误

    TIM1_Type *tim_index;
    switch((pin & 0xF0000) >> 16)
    {
        case 0: tim_index = ((TIM1_Type *)TIM1_BASE);   break;
        case 1: tim_index = ((TIM1_Type *)TIM2_BASE);   break;
        case 2: tim_index = ((TIM1_Type *)TIM3_BASE);   break;
        case 3: tim_index = ((TIM1_Type *)TIM4_BASE);   break;
        case 4: tim_index = ((TIM1_Type *)TIM5_BASE);   break;
        case 7: tim_index = ((TIM1_Type *)TIM8_BASE);   break;
        default: return;
    }
    uint16 period_temp = tim_index->ARR;                                        // 获取自动重装载值
    uint16 match_temp = (uint16)(period_temp * ((float)duty / PWM_DUTY_MAX));   // 计算占空比
    if(match_temp == period_temp)   match_temp +=1;

    switch(pin&0x0F000)                                                         // 提取通道值
    {
        case 0x01000:
            tim_index->CCR[0] = match_temp;                                     // 装载比较值
            break;
        case 0x02000:
            tim_index->CCR[1] = match_temp;                                     // 装载比较值
            break;
        case 0x03000:
            tim_index->CCR[2] = match_temp;                                     // 装载比较值
            break;
        case 0x04000:
            tim_index->CCR[3] = match_temp;                                     // 装载比较值
            break;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       PWM 初始化
// @param       pin             选择 PWM 引脚
// @param       freq            设置频率 同个模块只有最后一次设置生效
// @param       duty            设置占空比
// @return      void
// Sample usage:                pwm_init(TIM5_PWMCH1_A0, 17000, PWM_DUTY_MAX / 2);
//-------------------------------------------------------------------------------------------------------------------
void pwm_init (pwm_channel_enum pin, const uint32 freq, const uint32 duty)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 就去查看你在什么地方调用这个函数 检查你的传入参数
    // 这里是检查是否有重复使用定时器
    // 比如初始化了 TIM1_PIT 然后又初始化成 TIM1_PWM 这种用法是不允许的
    zf_assert(timer_funciton_check((timer_index_enum)((pin & 0xF0000) >> 16), TIMER_FUNCTION_PWM));
    // 如果是这一行报错 那你得去看看最大占空比是限定的多少 占空比写入错误
    zf_assert(PWM_DUTY_MAX >= duty);

    TIM1_Type *tim_index;
    uint16 freq_div;                                                            // 计算预分频
    uint16 period_temp;                                                         // 计算自动重装载值
    if(SYSTEM_CLOCK_120M < system_clock)
    {
        freq_div = ((system_clock / 2 / freq) >> 15);                           // 计算预分频
        period_temp = (system_clock / 2 / freq / (freq_div + 1));               // 计算自动重装载值
    }
    else
    {
        freq_div = ((system_clock / freq) >> 15);                               // 计算预分频
        period_temp = (system_clock / freq / (freq_div + 1));                   // 计算自动重装载值
    }
    uint16 match_temp = (uint16)(period_temp *((float)duty / PWM_DUTY_MAX));    // 计算占空比

    timer_clock_enable((timer_index_enum)((pin & 0xF0000) >> 16));
    afio_init((gpio_pin_enum)(pin & 0x000FF), GPO, (gpio_af_enum)((pin & 0x00F00) >> 8), GPO_AF_PUSH_PULL); // 初始化引脚

    switch((pin & 0xF0000) >> 16)
    {
        case 0: tim_index = ((TIM1_Type *)TIM1_BASE);   break;
        case 1: tim_index = ((TIM1_Type *)TIM2_BASE);   break;
        case 2: tim_index = ((TIM1_Type *)TIM3_BASE);   break;
        case 3: tim_index = ((TIM1_Type *)TIM4_BASE);   break;
        case 4: tim_index = ((TIM1_Type *)TIM5_BASE);   break;
        case 7: tim_index = ((TIM1_Type *)TIM8_BASE);   break;
        default: return;
    }

    tim_index->ARR = period_temp;                                               // 装载自动重装载值
    tim_index->PSC = freq_div;                                                  // 装载预分频
    tim_index->CR1 = (0x00000001 << 7);                                         // 允许自动重装载值的预装载
    tim_index->BDTR = (0x00000001 << 15);                                       // PWM 输出使能

    switch(pin & 0x0F000)
    {
        case 0x01000:
            tim_index->CCMR1 |= (0x00000006 << 4);                              // OC1M [6:4] 110 PWM 模式 1
            tim_index->CCMR1 |= (0x00000001 << 3);                              // 允许输出比较值的预装载
            tim_index->CCER |= (0x000000001);                                   // 使能通道 1
            tim_index->CCR[0] = match_temp;                                     // 装载比较值
            break;
        case 0x02000:
            tim_index->CCMR1 |= (0x00000006 << 12);                             // OC2M [14:12] 110 PWM 模式 1
            tim_index->CCMR1 |= (0x00000001 << 11);                             // 允许输出比较值的预装载
            tim_index->CCER |= (0x000000001 << 4);                              // 使能通道 2
            tim_index->CCR[1] = match_temp;                                     // 装载比较值
            break;
        case 0x03000:
            tim_index->CCMR2 |= (0x00000006 << 4);                              // OC1M [6:4] 110 PWM 模式 1
            tim_index->CCMR2 |= (0x00000001 << 3);                              // 允许输出比较值的预装载
            tim_index->CCER |= (0x000000001 << 8);                              // 使能通道 2
            tim_index->CCR[2] = match_temp;                                     // 装载比较值
            break;
        case 0x04000:
            tim_index->CCMR2 |= (0x00000006 << 12);                             // OC1M [6:4] 110 PWM 模式 1
            tim_index->CCMR2 |= (0x00000001 << 11);                             // 允许输出比较值的预装载
            tim_index->CCER |= (0x000000001 << 12);                             // 使能通道 2
            tim_index->CCR[3] = match_temp;                                     // 装载比较值
            break;
    }

    tim_index->CR1 |= (0x000000001);                                            // 使能定时器
}
