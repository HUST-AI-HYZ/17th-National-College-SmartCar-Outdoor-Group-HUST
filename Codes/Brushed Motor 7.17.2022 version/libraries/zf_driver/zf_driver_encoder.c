/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_driver_encoder
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
#include "zf_driver_encoder.h"

static uint8 encoder_dir_pin[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

//-------------------------------------------------------------------------------------------------------------------
// @brief       ENCODER 接口获取编码器计数
// @param       encoder_n       ENCODER 模块号 参照 zf_driver_encoder.h 内 encoder_index_enum 枚举体定义
// @return      int16           编码器数值
// Sample usage:                encoder_get_count(TIM3_ENCOEDER);
//-------------------------------------------------------------------------------------------------------------------
int16 encoder_get_count (encoder_index_enum encoder_n)
{
    switch(encoder_n)
    {
        case TIM1_ENCOEDER:
            if(0xFF == encoder_dir_pin[encoder_n])                              // 正交解码模式
                return (int16)TIM1->CNT / 4;                                    // 返回 int16 的计数值 4096 / 4 = 1024
            else                                                                // 否则是方向编码器
                if(gpio_get_level((gpio_pin_enum)encoder_dir_pin[encoder_n]))   // 方向引脚高电平
                    return (uint16)TIM1->CNT;                                   // 返回正数值
                else                                                            // 方向引脚低电平
                    return -((uint16)TIM1->CNT);                                // 返回负数值
        case TIM2_ENCOEDER:
            if(0xFF == encoder_dir_pin[encoder_n])                              // 正交解码模式
                return (int16)TIM2->CNT / 4;                                    // 返回 int16 的计数值 4096 / 4 = 1024
            else                                                                // 否则是方向编码器
                if(gpio_get_level((gpio_pin_enum)encoder_dir_pin[encoder_n]))   // 方向引脚高电平
                    return (uint16)TIM2->CNT;                                   // 返回正数值
                else                                                            // 方向引脚低电平
                    return -((uint16)TIM2->CNT);                                // 返回负数值
        case TIM3_ENCOEDER:
            if(0xFF == encoder_dir_pin[encoder_n])                              // 正交解码模式
                return (int16)TIM3->CNT / 4;                                    // 返回 int16 的计数值 4096 / 4 = 1024
            else                                                                // 否则是方向编码器
                if(gpio_get_level((gpio_pin_enum)encoder_dir_pin[encoder_n]))   // 方向引脚高电平
                    return (uint16)TIM3->CNT;                                   // 返回正数值
                else                                                            // 方向引脚低电平
                    return -((uint16)TIM3->CNT);                                // 返回负数值
        case TIM4_ENCOEDER:
            if(0xFF == encoder_dir_pin[encoder_n])                              // 正交解码模式
                return (int16)TIM4->CNT / 4;                                    // 返回 int16 的计数值 4096 / 4 = 1024
            else                                                                // 否则是方向编码器
                if(gpio_get_level((gpio_pin_enum)encoder_dir_pin[encoder_n]))   // 方向引脚高电平
                    return (uint16)TIM4->CNT;                                   // 返回正数值
                else                                                            // 方向引脚低电平
                    return -((uint16)TIM4->CNT);                                // 返回负数值
        case TIM5_ENCOEDER:
            if(0xFF == encoder_dir_pin[encoder_n])                              // 正交解码模式
                return (int16)TIM5->CNT / 4;                                    // 返回 int16 的计数值 4096 / 4 = 1024
            else                                                                // 否则是方向编码器
                if(gpio_get_level((gpio_pin_enum)encoder_dir_pin[encoder_n]))   // 方向引脚高电平
                    return (uint16)TIM5->CNT;                                   // 返回正数值
                else                                                            // 方向引脚低电平
                    return -((uint16)TIM5->CNT);                                // 返回负数值
        case TIM8_ENCOEDER:
            if(0xFF == encoder_dir_pin[encoder_n])                              // 正交解码模式
                return (int16)TIM8->CNT / 4;                                    // 返回 int16 的计数值 4096 / 4 = 1024
            else                                                                // 否则是方向编码器
                if(gpio_get_level((gpio_pin_enum)encoder_dir_pin[encoder_n]))   // 方向引脚高电平
                    return (uint16)TIM8->CNT;                                   // 返回正数值
                else                                                            // 方向引脚低电平
                    return -((uint16)TIM8->CNT);                                // 返回负数值
        default: return 0;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ENCODER 清空编码器计数
// @param       encoder_n       ENCODER 模块号 参照 zf_driver_encoder.h 内 encoder_index_enum 枚举体定义
// @return      void
// Sample usage:                encoder_clear_count(TIM3_ENCOEDER);
//-------------------------------------------------------------------------------------------------------------------
void encoder_clear_count (encoder_index_enum encoder_n)
{
    switch(encoder_n)
    {
        case TIM1_ENCOEDER: TIM1->EGR |= 0x01;    return;                       // 复位计数值
        case TIM2_ENCOEDER: TIM2->EGR |= 0x01;    return;                       // 复位计数值
        case TIM3_ENCOEDER: TIM3->EGR |= 0x01;    return;                       // 复位计数值
        case TIM4_ENCOEDER: TIM4->EGR |= 0x01;    return;                       // 复位计数值
        case TIM5_ENCOEDER: TIM5->EGR |= 0x01;    return;                       // 复位计数值
        case TIM8_ENCOEDER: TIM8->EGR |= 0x01;    return;                       // 复位计数值
        default: return;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ENCODER 接口初始化 正交编码器使用
// @param       encoder_n       ENCODER 模块号 参照 zf_driver_encoder.h 内 encoder_index_enum 枚举体定义
// @param       ch1_pin         ENCODER 通道 1 参照 zf_driver_encoder.h 内 encoder_channel_enum 枚举体定义
// @param       ch2_pin         ENCODER 通道 2 参照 zf_driver_encoder.h 内 encoder_channel_enum 枚举体定义
// @return      void
// Sample usage:                encoder_init_quad(TIM3_ENCOEDER, TIM3_ENCOEDER_CH1_B4, TIM3_ENCOEDER_CH2_B5);
//-------------------------------------------------------------------------------------------------------------------
void encoder_init_quad (encoder_index_enum encoder_n, encoder_channel_enum ch1_pin, encoder_channel_enum ch2_pin)
{
    uint32 register_temp = 0;
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 就去查看你在什么地方调用这个函数 检查你的传入参数
    // 这里是检查是否有重复使用定时器
    // 比如初始化了 TIM1_PWM 然后又初始化成 TIM1_ENCODER 这种用法是不允许的
    zf_assert(timer_funciton_check((timer_index_enum)encoder_n, TIMER_FUNCTION_ENCODER));

    timer_clock_enable((timer_index_enum)encoder_n);
    TIM1_Type *tim_index;
    afio_init((gpio_pin_enum)(ch1_pin & 0xFF), GPI, (gpio_af_enum)((ch1_pin & 0xF00) >> 8), GPI_FLOATING_IN);
    afio_init((gpio_pin_enum)(ch2_pin & 0xFF), GPI, (gpio_af_enum)((ch2_pin & 0xF00) >> 8), GPI_FLOATING_IN);

    switch(encoder_n)
    {
        case TIM1_ENCOEDER: tim_index = ((TIM1_Type *)TIM1_BASE);   break;
        case TIM2_ENCOEDER: tim_index = ((TIM1_Type *)TIM2_BASE);   break;
        case TIM3_ENCOEDER: tim_index = ((TIM1_Type *)TIM3_BASE);   break;
        case TIM4_ENCOEDER: tim_index = ((TIM1_Type *)TIM4_BASE);   break;
        case TIM5_ENCOEDER: tim_index = ((TIM1_Type *)TIM5_BASE);   break;
        case TIM8_ENCOEDER: tim_index = ((TIM1_Type *)TIM8_BASE);   break;
        default: return;
    }

    tim_index->ARR = 0xFFFF;                                                    // 装载自动重装载值
    tim_index->PSC = 0;                                                         // 装载预分频

    register_temp = tim_index->CCMR1;
    register_temp |= (0x00000001);                                              // 输入捕获 IC1 映射到 TI1
    register_temp |= (0x00000001 << 8);                                         // 输入捕获 IC2 映射到 TI2
    tim_index->CCMR1 = register_temp;

    tim_index->SMCR |= (0x00000003);                                            // 从模式 编码器模式 3 选择

    register_temp = tim_index->CR1;
    register_temp |= (0x00000001 << 1);                                         // 禁止UEV事件的产生
    register_temp |= (0x00000001);                                              // 使能定时器
    tim_index->CR1 = register_temp;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ENCODER 接口初始化 带方向编码器使用
// @param       encoder_n       ENCODER 模块号 参照 zf_driver_encoder.h 内 encoder_index_enum 枚举体定义
// @param       ch1_pin         ENCODER 通道 1 参照 zf_driver_encoder.h 内 encoder_channel_enum 枚举体定义
// @param       ch2_pin         ENCODER 通道 2 参照 zf_driver_encoder.h 内 encoder_channel_enum 枚举体定义
// @return      void
// Sample usage:                encoder_init_dir(TIM3_ENCOEDER, TIM3_ENCOEDER_CH1_B4, TIM3_ENCOEDER_CH2_B5);
// Note:                        ch1_pin 将作为脉冲输入计数 ch2_pin 将作为计数方向控制引脚
//                              此方式下 在出现反复正反转的情况下 计数方向将不是完全可靠的
//                              只有读取时刻的 ch2_pin 的电平决定整个计数值的正负符号
//                              同时 ch1_pin 只能选择 CH1 通道的引脚
//-------------------------------------------------------------------------------------------------------------------
void encoder_init_dir (encoder_index_enum encoder_n, encoder_channel_enum ch1_pin, encoder_channel_enum ch2_pin)
{
    uint32 register_temp = 0;
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 就去查看你在什么地方调用这个函数 检查你的传入参数
    // 这里是检查是否有重复使用定时器
    // 比如初始化了 TIM1_PWM 然后又初始化成 TIM1_ENCODER 这种用法是不允许的
    zf_assert(timer_funciton_check((timer_index_enum)encoder_n, TIMER_FUNCTION_ENCODER));

    timer_clock_enable((timer_index_enum)encoder_n);
    TIM1_Type *tim_index;
    afio_init((gpio_pin_enum)(ch1_pin & 0xFF), GPI, (gpio_af_enum)((ch1_pin & 0xF00) >> 8), GPI_FLOATING_IN);
    gpio_init((gpio_pin_enum)(ch2_pin & 0xFF), GPI, GPIO_LOW, GPI_FLOATING_IN);
    encoder_dir_pin[encoder_n] = (ch2_pin & 0xFF);

    switch(encoder_n)
    {
        case TIM1_ENCOEDER: tim_index = ((TIM1_Type *)TIM1_BASE);   break;
        case TIM2_ENCOEDER: tim_index = ((TIM1_Type *)TIM2_BASE);   break;
        case TIM3_ENCOEDER: tim_index = ((TIM1_Type *)TIM3_BASE);   break;
        case TIM4_ENCOEDER: tim_index = ((TIM1_Type *)TIM4_BASE);   break;
        case TIM5_ENCOEDER: tim_index = ((TIM1_Type *)TIM5_BASE);   break;
        case TIM8_ENCOEDER: tim_index = ((TIM1_Type *)TIM8_BASE);   break;
        default: return;
    }

    tim_index->ARR = 0xFFFF;                                                    // 装载自动重装载值
    tim_index->PSC = 0;                                                         // 装载预分频

    tim_index->CCMR1 |= (0x00000001);                                           // 输入捕获 IC1 映射到 TI1

    register_temp = tim_index->SMCR;
    register_temp |= (0x00000005 << 4);                                         // 滤波后的定时器输入 1 TI1FP1
    register_temp |= (0x00000007);                                              // 从模式 外部时钟1模式 选择
    tim_index->SMCR = register_temp;

    register_temp = tim_index->CR1;
    register_temp |= (0x00000001 << 1);                                         // 禁止UEV事件的产生
    register_temp |= (0x00000001);                                              // 使能定时器
    tim_index->CR1 = register_temp;
}
