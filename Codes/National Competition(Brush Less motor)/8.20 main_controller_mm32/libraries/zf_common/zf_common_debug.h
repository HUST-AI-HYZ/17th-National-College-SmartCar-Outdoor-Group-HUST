/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_common_debug
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#ifndef _zf_common_debug_h_
#define _zf_common_debug_h_

#include "zf_common_typedef.h"

// 如果修改串口并开启了 debug UART 的中断接收 需要同步更换 debug_interrupr_handler 函数到对应的中断服务函数
#define DEBUG_UART_INDEX            (UART_1)                                    // 指定 debug uart 所使用的的串口
#define DEBUG_UART_BAUDRATE         (115200)                                    // 指定 debug uart 所使用的的串口波特率
#define DEBUG_UART_TX_PIN           (UART1_TX_A9 )                              // 指定 debug uart 所使用的的串口引脚
#define DEBUG_UART_RX_PIN           (UART1_RX_A10)                              // 指定 debug uart 所使用的的串口引脚

typedef struct
{
    uint16 type_index;

    uint16 display_x_max;
    uint16 display_y_max;

    uint8 font_x_size;
    uint8 font_y_size;

    void (*output_uart)             (const char *str);
    void (*output_screen)           (uint16 x, uint16 y, const char *str);
    void (*output_screen_clear)     (void);
}debug_output_struct;

#define zf_assert(x)                (debug_assert_handler((x), __FILE__, __LINE__))     // 断言 一般用于参数判断 zf_assert(0) 就断言报错
#define zf_log(x, str)              (debug_log_handler((x), (str), __FILE__, __LINE__)) // 调试信息输出 用来做一些报错或者警告之类的输出

#define DEBUG_UART_USE_INTERRUPT    (0)                                         // 是否启用 debug uart 接收中断
#if DEBUG_UART_USE_INTERRUPT                                                    // 如果启用 debug uart 接收中断
#define DEBUG_RING_BUFFER_LEN       (64)                                        // 定义环形缓冲区大小 默认 64byte

extern uint8  debug_ring_buffer[DEBUG_RING_BUFFER_LEN];                         // 环形缓冲区
extern uint32 debug_ring_index;                                                 // 缓冲区下标

void        debug_interrupr_handler (void);
uint32      debug_read_ring_buffer  (uint8 *data);
#endif

void        debug_assert_enable         (void);
void        debug_assert_disable        (void);
void        debug_assert_handler        (uint8 pass, char *file, int line);
void        debug_log_handler           (uint8 pass, char *str, char *file, int line);
void        debug_output_struct_init    (debug_output_struct *info);
void        debug_output_init           (debug_output_struct *info);
void        debug_init                  (void);

#endif
