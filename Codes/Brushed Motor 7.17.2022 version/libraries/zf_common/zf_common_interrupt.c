/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_common_interrupt
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#include "zf_common_interrupt.h"

static uint32 interrupt_nest_count = 0;

//-------------------------------------------------------------------------------------------------------------------
// @brief       全局中断使能
// @param       void
// @return      void
// Sample usage:                interrupt_global_enable();
//-------------------------------------------------------------------------------------------------------------------
void interrupt_global_enable (void)
{
    if(interrupt_nest_count)
        interrupt_nest_count --;
    if(0 == interrupt_nest_count)
        __ASM volatile("cpsie i");
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       全局中断屏蔽
// @param       void
// @return      void
// Sample usage:                interrupt_global_disable();
//-------------------------------------------------------------------------------------------------------------------
void interrupt_global_disable (void)
{
    if(!interrupt_nest_count)
        __ASM volatile("cpsid i");
    interrupt_nest_count ++;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       指定中断使能
// @param       irqn            指定中断号 可查看 isr.c 对应中断服务函数的标注
// @return      void
// Sample usage:                interrupt_enable(UART1_IRQn);
//-------------------------------------------------------------------------------------------------------------------
void interrupt_enable (IRQn_Type irqn)
{
    NVIC_EnableIRQ(irqn);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       指定中断屏蔽
// @param       irqn            指定中断号 可查看 isr.c 对应中断服务函数的标注
// @return      void
// Sample usage:                interrupt_disable(UART1_IRQn);
//-------------------------------------------------------------------------------------------------------------------
void interrupt_disable (IRQn_Type irqn)
{
    NVIC_DisableIRQ(irqn);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       指定中断设置优先级
// @param       irqn            指定中断号 可查看 isr.c 对应中断服务函数的标注
// @param       priority        中断优先级 0-7 越低越高
// @return      void
// Sample usage:                interrupt_set_priority(UART1_IRQn, 0);
//-------------------------------------------------------------------------------------------------------------------
void interrupt_set_priority (IRQn_Type irqn, uint8 priority)
{
    NVIC_SetPriority(irqn, priority);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       中断组初始化 clock_init 内部调用
// @param       void
// @return      void
// Sample usage:                interrupt_init();
//-------------------------------------------------------------------------------------------------------------------
void interrupt_init (void)
{
    NVIC_SetPriorityGrouping(4);
}
