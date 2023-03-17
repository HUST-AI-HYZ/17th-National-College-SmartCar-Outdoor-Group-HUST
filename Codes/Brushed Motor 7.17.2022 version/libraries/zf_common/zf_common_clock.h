/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_common_clock
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#ifndef _zf_common_clock_h_
#define _zf_common_clock_h_

#include "hal_device_registers.h"

#include "zf_common_typedef.h"

#define BOARD_XTAL_FREQ         (8000000)                                       // 晶振频率 如果自己用的不是这个频率就修改这里 UM 定义范围为 4-24Mhz
#define XTAL_STARTUP_TIMEOUT    (0x0F00 )                                       // 晶振就绪等待超时时长

// 推荐最高使用到 120MHz
// 虽然内核可以超频突破 120MHz
// 但总线最多只能到 120MHz 
// 当内核超频超过 120MHz 时总线必须为内核二分频
// 所以推荐最高使用到 120MHz 就好

// 收手吧 它只是个默频 96MHz 的 M3
// 它已经承受了太多他不该承受的了
typedef enum
{
    SYSTEM_CLOCK_XTAL       = BOARD_XTAL_FREQ,                                  // 使用晶振频率作为时钟频率
    SYSTEM_CLOCK_24M        = 24000000,                                         // 24Mhz
    SYSTEM_CLOCK_48M        = 48000000,                                         // 48Mhz
    SYSTEM_CLOCK_72M        = 72000000,                                         // 72Mhz
    SYSTEM_CLOCK_96M        = 96000000,                                         // 96Mhz
    SYSTEM_CLOCK_120M       = 120000000,                                        // 120Mhz
}system_clock_enum;

extern uint32 system_clock;                                                     // 全局变量 系统时钟信息

void clock_init (uint32 clock);                                                 // 核心时钟初始化

#endif
