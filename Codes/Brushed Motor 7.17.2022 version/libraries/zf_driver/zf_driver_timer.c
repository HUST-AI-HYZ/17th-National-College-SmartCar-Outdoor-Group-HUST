/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_driver_timer
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

static timer_function_enum timer_function_state[8] = 
{
    TIMER_FUNCTION_INIT, TIMER_FUNCTION_INIT,
    TIMER_FUNCTION_INIT, TIMER_FUNCTION_INIT,
    TIMER_FUNCTION_INIT, TIMER_FUNCTION_INIT,
    TIMER_FUNCTION_INIT, TIMER_FUNCTION_INIT
};

static timer_mode_enum timer_mode_state[8] = 
{
    TIMER_SYSTEM_CLOCK, TIMER_SYSTEM_CLOCK,
    TIMER_SYSTEM_CLOCK, TIMER_SYSTEM_CLOCK,
    TIMER_SYSTEM_CLOCK, TIMER_SYSTEM_CLOCK,
    TIMER_SYSTEM_CLOCK, TIMER_SYSTEM_CLOCK
};

//-------------------------------------------------------------------------------------------------------------------
// @brief       TIMER 外设确认功能状态 库内部调用
// @param       index           TIMER 外设模块号
// @param       mode            需要确的功能模块
// @return      uint8           1-可以使用 0-不可以使用
// Sample usage:                zf_assert(timer_funciton_check(TIM_1, TIMER_FUNCTION_PWM);
//-------------------------------------------------------------------------------------------------------------------
uint8 timer_funciton_check (timer_index_enum index,timer_function_enum mode)
{
    if(TIMER_FUNCTION_INIT == timer_function_state[index])
        timer_function_state[index] = mode;
    else if(timer_function_state[index] == mode)
        return 1;
    else
        return 0;
    return 1;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       TIMER 时钟使能 库内部调用
// @param       index           TIMER 外设模块号
// @return      void
// Sample usage:                timer_clock_enable(TIM_1);
//-------------------------------------------------------------------------------------------------------------------
void timer_clock_enable (timer_index_enum index)
{
    switch(index)
    {
        case TIM_1:
            RCC->APB2ENR |= 0x00000001;                                         // 使能时钟
            break;
        case TIM_2:
            RCC->APB1ENR |= 0x00000001;                                         // 使能时钟
            break;
        case TIM_3:
            RCC->APB1ENR |= 0x00000001 << 1;                                    // 使能时钟
            break;
        case TIM_4:
            RCC->APB1ENR |= 0x00000001 << 2;                                    // 使能时钟
            break;
        case TIM_5:
            RCC->APB1ENR |= 0x00000001 << 3;                                    // 使能时钟
            break;
        case TIM_6:
            RCC->APB1ENR |= 0x00000001 << 4;                                    // 使能时钟
            break;
        case TIM_7:
            RCC->APB1ENR |= 0x00000001 << 5;                                    // 使能时钟
            break;
        case TIM_8:
            RCC->APB2ENR |= 0x00000001 << 1;                                    // 使能时钟
            break;
        default:
            break;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       TIMER 计时器初始化
// @param       index           TIMER 外设模块号
// @param       mode            计时方式
// @return      void
// Sample usage:                timer_init(TIM_1, TIMER_US);
//-------------------------------------------------------------------------------------------------------------------
void timer_init (timer_index_enum index, timer_mode_enum mode)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 就去查看你在什么地方调用这个函数 检查你的传入参数
    // 这里是检查是否有重复使用定时器
    // 比如初始化了 TIM1_PWM 然后又初始化成 TIM_1 这种用法是不允许的
    zf_assert(timer_funciton_check(index, TIMER_FUNCTION_TIMER));
    timer_function_state[index] = TIMER_FUNCTION_TIMER;

    timer_clock_enable(index);                                                  // 使能时钟
    TIM1_Type *tim_index;
    switch(index)
    {
        case TIM_1: tim_index = ((TIM1_Type *)TIM1_BASE);   break;
        case TIM_2: tim_index = ((TIM1_Type *)TIM2_BASE);   break;
        case TIM_3: tim_index = ((TIM1_Type *)TIM3_BASE);   break;
        case TIM_4: tim_index = ((TIM1_Type *)TIM4_BASE);   break;
        case TIM_5: tim_index = ((TIM1_Type *)TIM5_BASE);   break;
        case TIM_6: tim_index = ((TIM1_Type *)TIM6_BASE);   break;
        case TIM_7: tim_index = ((TIM1_Type *)TIM7_BASE);   break;
        case TIM_8: tim_index = ((TIM1_Type *)TIM8_BASE);   break;
        default: return;
    }

    timer_mode_state[index] = mode;
    switch(mode)
    {
        case TIMER_US:
            tim_index->ARR |= 0x0000FFFF;                                       // 装载自动重装载值
            if(SYSTEM_CLOCK_120M < system_clock)
            {
                tim_index->PSC = system_clock / 2/ 1000000 - 1;                 // 装载预分频
            }
            else
            {
                tim_index->PSC = system_clock / 1000000 - 1;                    // 装载预分频
            }
            break;
        case TIMER_MS:
            tim_index->ARR |= 0x0000FFFF;                                       // 装载自动重装载值
            if(SYSTEM_CLOCK_120M < system_clock)
            {
                tim_index->PSC = (system_clock / 2 / 1000) / 2 - 1;             // 装载预分频
            }
            else
            {
                tim_index->PSC = (system_clock / 1000) / 2 - 1;                 // 装载预分频
            }
            break;
        default:
            tim_index->ARR |= 0x0000FFFF;                                       // 装载自动重装载值
            tim_index->PSC = 0;                                                 // 装载预分频
            break;
    }

    tim_index->EGR |= 0x01;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       TIMER 开始计时
// @param       index           TIMER 外设模块号
// @return      void
// Sample usage:                timer_start(TIM_1);
//-------------------------------------------------------------------------------------------------------------------
void timer_start (timer_index_enum index)
{
    TIM1_Type *tim_index;
    switch(index)
    {
        case TIM_1: tim_index = ((TIM1_Type *)TIM1_BASE);   break;
        case TIM_2: tim_index = ((TIM1_Type *)TIM2_BASE);   break;
        case TIM_3: tim_index = ((TIM1_Type *)TIM3_BASE);   break;
        case TIM_4: tim_index = ((TIM1_Type *)TIM4_BASE);   break;
        case TIM_5: tim_index = ((TIM1_Type *)TIM5_BASE);   break;
        case TIM_6: tim_index = ((TIM1_Type *)TIM6_BASE);   break;
        case TIM_7: tim_index = ((TIM1_Type *)TIM7_BASE);   break;
        case TIM_8: tim_index = ((TIM1_Type *)TIM8_BASE);   break;
        default: return;
    }
    tim_index->CR1 |= (0x00000001);                                             // 使能定时器
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       TIMER 停止计时
// @param       index           TIMER 外设模块号
// @return      void
// Sample usage:                timer_stop(TIM_1);
//-------------------------------------------------------------------------------------------------------------------
void timer_stop (timer_index_enum index)
{
    TIM1_Type *tim_index;
    switch(index)
    {
        case TIM_1: tim_index = ((TIM1_Type *)TIM1_BASE);   break;
        case TIM_2: tim_index = ((TIM1_Type *)TIM2_BASE);   break;
        case TIM_3: tim_index = ((TIM1_Type *)TIM3_BASE);   break;
        case TIM_4: tim_index = ((TIM1_Type *)TIM4_BASE);   break;
        case TIM_5: tim_index = ((TIM1_Type *)TIM5_BASE);   break;
        case TIM_6: tim_index = ((TIM1_Type *)TIM6_BASE);   break;
        case TIM_7: tim_index = ((TIM1_Type *)TIM7_BASE);   break;
        case TIM_8: tim_index = ((TIM1_Type *)TIM8_BASE);   break;
        default: return;
    }
    tim_index->CR1 &= (0xFFFFFFFE);                                             // 停止定时器
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       TIMER 获取计时器计数
// @param       index           TIMER 外设模块号
// @return      uint16          计数值
// Sample usage:                timer_get(TIM_1);
//-------------------------------------------------------------------------------------------------------------------
uint16 timer_get (timer_index_enum index)
{
    TIM1_Type *tim_index;
    switch(index)
    {
        case TIM_1: tim_index = ((TIM1_Type *)TIM1_BASE);   break;
        case TIM_2: tim_index = ((TIM1_Type *)TIM2_BASE);   break;
        case TIM_3: tim_index = ((TIM1_Type *)TIM3_BASE);   break;
        case TIM_4: tim_index = ((TIM1_Type *)TIM4_BASE);   break;
        case TIM_5: tim_index = ((TIM1_Type *)TIM5_BASE);   break;
        case TIM_6: tim_index = ((TIM1_Type *)TIM6_BASE);   break;
        case TIM_7: tim_index = ((TIM1_Type *)TIM7_BASE);   break;
        case TIM_8: tim_index = ((TIM1_Type *)TIM8_BASE);   break;
        default: return 0;
    }
    if(TIMER_MS == timer_mode_state[index])
        return (uint16)tim_index->CNT / 2;
    return (uint16)tim_index->CNT;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       TIMER 清除计时器计数
// @param       index           TIMER 外设模块号
// @return      void
// Sample usage:                timer_clear(TIM_1);
//-------------------------------------------------------------------------------------------------------------------
void timer_clear (timer_index_enum index)
{
    switch(index)
    {
        case TIM_1: TIM1->EGR |= 0x01;  break;
        case TIM_2: TIM2->EGR |= 0x01;  break;
        case TIM_3: TIM3->EGR |= 0x01;  break;
        case TIM_4: TIM4->EGR |= 0x01;  break;
        case TIM_5: TIM5->EGR |= 0x01;  break;
        case TIM_6: TIM6->EGR |= 0x01;  break;
        case TIM_7: TIM7->EGR |= 0x01;  break;
        case TIM_8: TIM8->EGR |= 0x01;  break;
        default:    break;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       TIMER ETR 外部时钟输入功能初始化 32类单片机用于驱动摄像头 库内部调用
// @param       pin             ETR 引脚
// @param       sign            信号类型 上升沿或者下降沿
// @return      void
// Sample usage:                timer_etr_init(OV7725_PCLK_PIN, TIM_ETR_FALLING);
//-------------------------------------------------------------------------------------------------------------------
void timer_etr_init (timer_etr_pin_emun pin, timer_etr_mode_emun sign)
{
    timer_index_enum index = (timer_index_enum)((pin & 0xF000) >> 12);

    zf_assert(timer_funciton_check(index, TIMER_FUNCTION_CAMERA));
    timer_clock_enable(index);

    TIM1_Type *tim_index;
    switch(index)
    {
        case TIM_1: tim_index = ((TIM1_Type *)TIM1_BASE);   break;
        case TIM_2: tim_index = ((TIM1_Type *)TIM2_BASE);   break;
        case TIM_3: tim_index = ((TIM1_Type *)TIM3_BASE);   break;
        case TIM_4: tim_index = ((TIM1_Type *)TIM4_BASE);   break;
        case TIM_5: tim_index = ((TIM1_Type *)TIM5_BASE);   break;
        case TIM_6: tim_index = ((TIM1_Type *)TIM6_BASE);   break;
        case TIM_7: tim_index = ((TIM1_Type *)TIM7_BASE);   break;
        case TIM_8: tim_index = ((TIM1_Type *)TIM8_BASE);   break;
        default: return;
    }

    afio_init((gpio_pin_enum)(pin & 0x00FF), GPI, (gpio_af_enum)((pin & 0x0F00) >> 8), GPI_FLOATING_IN);    // 提取对应IO索引 AF功能编码

    tim_index->ARR |= 0x0000FFFF;                                               // 装载自动重装载值
    tim_index->PSC = 0;                                                         // 装载预分频
    tim_index->EGR |= 0x01;

    uint32 temp = tim_index->SMCR;
    temp &= 0xFFFF0088;
    temp |= 0x00000074;
    tim_index->SMCR = temp;
    if(TIM_ETR_FALLING == sign)
        tim_index->SMCR |= 0x00008000;
    tim_index->DIER |= 0x00004000;
    tim_index->CR1 |= (0x00000001);                                             // 使能定时器
}

