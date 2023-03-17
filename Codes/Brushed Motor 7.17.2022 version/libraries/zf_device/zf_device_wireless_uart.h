/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_wrieless_uart
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
*                   RX                  查看 zf_device_wrieless_uart.h 中 WRIELESS_UART_RX_PINx 宏定义
*                   TX                  查看 zf_device_wrieless_uart.h 中 WRIELESS_UART_TX_PINx 宏定义
*                   RTS                 查看 zf_device_wrieless_uart.h 中 WRIELESS_UART_RTS_PINx 宏定义
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   其余引脚悬空
*                   ------------------------------------
********************************************************************************************************************/

#ifndef _zf_device_wrieless_uart_h_
#define _zf_device_wrieless_uart_h_

#include "zf_common_clock.h"
#include "zf_common_debug.h"
#include "zf_common_fifo.h"

#include "zf_driver_delay.h"
#include "zf_driver_gpio.h"
#include "zf_driver_uart.h"

#define WRIELESS_UART_INDEX         (UART_3)                                    // 无线串口对应使用的串口号
#define WRIELESS_UART_BUAD_RATE     (9600)                                    // 无线串口对应使用的串口波特率
#define WRIELESS_UART_TX_PIN        (UART3_TX_D8)                               // 无线串口对应模块的 TX 要接到单片机的 RX
#define WRIELESS_UART_RX_PIN        (UART3_RX_D9)                               // 无线串口对应模块的 RX 要接到单片机的 TX
#define WRIELESS_UART_RTS_PIN       (B14)                                       // 无线串口对应模块的 RTS 引脚

// ------------------------------------ 自动波特率 ------------------------------------
// 注意事项1：无线转串口模块版本是V2.0以下的是无法开启自动波特率的。
// 注意事项2：开启自动波特率务必连接RTS引脚 否则会开启失败。
// 注意事项3：模块自动波特率失败的话 可以尝试断电重启

// 开启自动波特率务必阅读上面两条 注意事项
// 开启自动波特率务必阅读上面两条 注意事项
// 开启自动波特率务必阅读上面两条 注意事项

// 0：关闭自动波特率  
// 1：开启自动波特率 自动波特率的作用是修改 WIRELESS_UART_BAUD 之后不需要对模块进行配置 模块会自动设置为对应的波特率

#define WIRELESS_UART_AUTO_BAUD_RATE    (0)
// ------------------------------------ 自动波特率 ------------------------------------

#define WIRELESS_UART_BUFFER_SIZE       (64  )
#define WIRELESS_UART_TIMEOUT_COUNT     (0x64)

void        wireless_uart_callback          (void);

uint32      wireless_uart_send_byte         (const uint8 data);
uint32      wireless_uart_send_buff         (const uint8 *buff, uint32 len);
uint32      wireless_uart_send_string       (const char *str);
void        wireless_uart_send_image        (const uint8 *image_addr, uint32 image_size);

uint32      wireless_uart_read_buff         (uint8 *buff, uint32 len);

uint8       wireless_uart_init              (void);

#endif
