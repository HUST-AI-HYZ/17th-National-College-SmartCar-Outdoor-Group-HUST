/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_bluetooth_ch9141
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ790875685)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
* @note             接线定义：
*                   ------------------------------------
*                   模块管脚            单片机管脚
*                   RX                  查看 zf_device_bluetooth_ch9141.h 中 CH9141_RX_PINx 宏定义
*                   TX                  查看 zf_device_bluetooth_ch9141.h 中 CH9141_TX_PINx 宏定义
*                   RTS                 查看 zf_device_bluetooth_ch9141.h 中 CH9141_RTS_PINx 宏定义
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   其余引脚悬空
*                   ------------------------------------
********************************************************************************************************************/
#ifndef _zf_device_bluetooth_ch9141_h_
#define _zf_device_bluetooth_ch9141_h_

#include "zf_common_clock.h"
#include "zf_common_debug.h"
#include "zf_common_fifo.h"

#include "zf_driver_gpio.h"
#include "zf_driver_uart.h"
#include "zf_driver_delay.h"

#define BLUETOOTH_CH9141_INDEX                  (UART_6)                        // 蓝牙模块对应使用的串口号
#define BLUETOOTH_CH9141_BUAD_RATE              (115200)                        // 蓝牙模块对应使用的串口波特率
#define BLUETOOTH_CH9141_TX_PIN                 (UART6_RX_C7)                   // 蓝牙模块对应模块的 TX 要接到单片机的 RX
#define BLUETOOTH_CH9141_RX_PIN                 (UART6_TX_C6)                   // 蓝牙模块对应模块的 RX 要接到单片机的 TX
#define BLUETOOTH_CH9141_RTS_PIN                (C13)                           // 蓝牙模块对应模块的 RTS 引脚

#define BLUETOOTH_CH9141_BUFFER_SIZE            (64)
#define BLUETOOTH_CH9141_TIMEOUT_COUNT          (500)

void        bluetooth_ch9141_uart_callback      (void);

uint32      bluetooth_ch9141_send_byte          (const uint8 data);
uint32      bluetooth_ch9141_send_buff          (const uint8 *buff, uint32 len);
uint32      bluetooth_ch9141_send_string        (const char *str);
void        bluetooth_ch9141_send_image         (const uint8 *image_addr, uint32 image_size);

uint32      bluetooth_ch9141_read_buff          (uint8 *buff, uint32 len);

uint8       bluetooth_ch9141_init               (void);

#endif
