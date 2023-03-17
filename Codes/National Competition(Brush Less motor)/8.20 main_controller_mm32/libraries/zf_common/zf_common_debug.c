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

#include "zf_common_debug.h"
#include "zf_common_interrupt.h"

#include "zf_driver_uart.h"

static debug_output_struct debug_output_info;
static volatile uint8 zf_debug_init_flag = 0;
static volatile uint8 zf_debug_assert_enable = 1;

//-------------------------------------------------------------------------------------------------------------------
// @brief       debug 软延时函数 在 120MHz 下是一秒多的时间 各单片机需要根据各自时钟试验
// @param       pass        判断是否触发断言
// @param       *file       文件名
// @param       line        目标行数
// @return      void
//-------------------------------------------------------------------------------------------------------------------
static void debug_delay (void)
{
    vuint32 loop_1 = 0, loop_2 = 0;
    for(loop_1 = 0; loop_1 <= 0xFF; loop_1 ++)
        for(loop_2 = 0; loop_2 <= 0xFFFF; loop_2 ++)
            __NOP();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       debug 串口输出接口 此部分不允许用户更改
// @param       *str        需要输出的字符串
// @return      void
//-------------------------------------------------------------------------------------------------------------------
static void debug_uart_str_output (const char *str)
{
    uart_write_string(DEBUG_UART_INDEX, str);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       debug 输出接口 此部分不允许用户更改
// @param       *file       文件名
// @param       line        目标行数
// @return      void
//-------------------------------------------------------------------------------------------------------------------
static void debug_output (char *file, int line)
{
    char *file_str;
    vuint16 i = 0, j = 0;
    vint16 len_origin = 0;
    vint16 show_len = 0;
    len_origin = strlen(file);

    char output_buffer[256];
    char file_path_buffer[64];

    if(debug_output_info.type_index)
        debug_output_info.output_screen_clear();

    if(zf_debug_init_flag)
    {
        if(debug_output_info.type_index)
        {
            // 需要分行将文件的路径和行数输出
            // <不输出完整路径 只输出一级目录 例如 src/main.c>
            // 输出 line : xxxx
            debug_output_info.output_screen(0, 0, "Assert error");

            file_str = file;
            len_origin = strlen(file);
            show_len = (debug_output_info.display_x_max / debug_output_info.font_x_size);

            while(*file_str++ != '\0');

            // 只取一级目录 如果文件放在盘符根目录 或者 MDK 的工程根目录 就会直接输出当前目录
            for(j = 0; (j < 2) && (len_origin >= 0); len_origin --)          // 查找两个 '/'
            {
                file_str --;
                if((*file_str == '/') || (*file_str == 0x5C))
                    j ++;
            }

            // 文件路径保存到数组中
            if(len_origin >= 0)
            {
                file_str ++;
                sprintf(output_buffer, "file: %s", file_str);
            }
            else
            {
                if(0 == j)
                    sprintf(output_buffer, "file: mdk/%s", file_str);
                else
                    sprintf(output_buffer, "file: %s", file_str);
            }

            // 屏幕显示路径
            for(i = 0; i < ((strlen(output_buffer) / show_len) + 1); i ++)
            {
                for(j = 0; j < show_len; j ++)
                {
                    if(strlen(output_buffer) < (j + i * show_len))   
                        break;
                    file_path_buffer[j] = output_buffer[j + i * show_len];
                }
                
                file_path_buffer[j] = '\0';                                 // 末尾添加\0
                
                debug_output_info.output_screen(0, debug_output_info.font_y_size * (i + 1), file_path_buffer);
            }

            // 屏幕显示行号
            sprintf(output_buffer, "line: %d", line);
            debug_output_info.output_screen(0, debug_output_info.font_y_size * (i + 1), output_buffer);
        }
        else
        {
            char output_buffer[128];
            memset(output_buffer, 0, 128);
            sprintf(output_buffer, "Assert error in %s line %d.\r\n", file, line);
            debug_output_info.output_uart(output_buffer);
        }
    }
}

#if DEBUG_UART_USE_INTERRUPT                                                    // 条件编译 只有在启用串口中断才编译

uint8  debug_ring_buffer[DEBUG_RING_BUFFER_LEN];                                // 创建环形缓冲区
uint32 debug_ring_index = 0;                                                    // 环形缓冲区下标索引

//-------------------------------------------------------------------------------------------------------------------
// @brief       debug 串口中断处理函数 isr.c 中对应串口中断服务函数调用
// @param       void
// @return      void
// Sample usage:            debug_interrupr_handler();
//-------------------------------------------------------------------------------------------------------------------
void debug_interrupr_handler (void)
{
    if(!zf_debug_init_flag) return;
    uart_query_byte(DEBUG_UART_INDEX, &debug_ring_buffer[debug_ring_index ++]); // 读取数据到环形缓冲区
    if(DEBUG_RING_BUFFER_LEN == debug_ring_index)   debug_ring_index = 0;       // 环形缓冲区收尾衔接
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       读取 debug 环形缓冲区数据
// @param       *data       读出数据存放的数组指针
// @return      uint32    读出数据长度
// Sample usage:            uint8 data[64]; len = debug_read_ring_buffer(data);
//-------------------------------------------------------------------------------------------------------------------
uint32 debug_read_ring_buffer (uint8 *data)
{
    if(0 == debug_ring_index)   return 0;
    uint32 temp = debug_ring_index;                                             // 获取数据长度
    memcpy(data, debug_ring_buffer, temp);                                      // 拷贝数据
    debug_ring_index = 0;                                                       // 清空下标
    return temp;
}

#endif

//-------------------------------------------------------------------------     // printf 重定向 此部分不允许用户更改
#if defined(__ICCARM__)
#define PUTCHAR_PROTOTYPE int32_t fputc (int32_t ch, FILE *f)
#elif defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int32_t __io_putchar (int32_t ch)
#endif

#if defined(__ICCARM__)
PUTCHAR_PROTOTYPE
{
    uart_write_byte(DEBUG_UART_INDEX, (ch&0xFF));
    return ch;
}
#else
int32_t fputc (int32_t ch, FILE* f)
{
    uart_write_byte(DEBUG_UART_INDEX, (ch&0xFF));
    return ch;
}
#endif
//-------------------------------------------------------------------------     // printf 重定向 此部分不允许用户更改

//-------------------------------------------------------------------------------------------------------------------
// @brief       启用断言
// @param       void
// @return      void
// Sample usage:                debug_assert_enable();
//-------------------------------------------------------------------------------------------------------------------
void debug_assert_enable (void)
{
    zf_debug_assert_enable = 1;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       禁用断言
// @param       void
// @return      void
// Sample usage:                debug_assert_disable();
//-------------------------------------------------------------------------------------------------------------------
void debug_assert_disable (void)
{
    zf_debug_assert_enable = 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       debug 断言处理函数 此部分不允许用户更改
// @param       pass        判断是否触发断言
// @param       *file       文件名
// @param       line        目标行数
// @return      void
// Sample usage:            这个函数不是直接调用的 使用 zf_commmon_debug.h 中的 zf_assert(x) 接口
//-------------------------------------------------------------------------------------------------------------------
void debug_assert_handler (uint8 pass, char *file, int line)
{
    if(pass || !zf_debug_assert_enable)
        return;

    static uint8 assert_nest_index = 0;

    if(0 != assert_nest_index)
    {
        while(1);
    }
    assert_nest_index ++;

    interrupt_global_disable();

    while(1)
    {
        // 如果代码跳转到这里停住了
        // 一般你的函数参数传递出错了
        // 或者你自己调用的 zf_assert(x) 接口处报错了

        // 如果调用了 debug_init 初始化了 log 输出
        // 就在对应串口输出去查看是哪个文件的哪一行报错

        // 如果没有初始化 debug
        // 那就看看这个 file 的字符串值和 line 的行数
        // 那代表报错的文件路径名称和对应报错行数

        // 再去调试看看是为什么参数出错
//        __NOP();

        debug_output(file, line);
        debug_delay();
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       debug 调试信息处理函数 此部分不允许用户更改
// @param       bool        判断是否触发断言
// @param       *str        要输出的调试信息
// @param       *file       文件名
// @param       line        目标行数
// @return      void
// Sample usage:            这个函数不是直接调用的 使用 zf_commmon_debug.h 中的 zf_log(x, str) 接口
//-------------------------------------------------------------------------------------------------------------------
void debug_log_handler (uint8 pass, char *str, char *file, int line)
{
    if(pass)
        return;
    if(zf_debug_init_flag)
    {
        printf("Log message from %s line %d :\"%s\".\r\n", file, line, str);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       debug 输出绑定信息初始化 此部分不允许用户更改
// @param       *info       debug 输出的信息结构体
// @return      void
// Sample usage:            debug_output_struct_init(info);
//-------------------------------------------------------------------------------------------------------------------
void debug_output_struct_init (debug_output_struct *info)
{
    info->type_index            = 0;

    info->display_x_max         = 0xFFFF;
    info->display_y_max         = 0xFFFF;

    info->font_x_size           = 0xFF;
    info->font_y_size           = 0xFF;

    info->output_uart           = NULL;
    info->output_screen         = NULL;
    info->output_screen_clear   = NULL;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       debug 输出绑定初始化 此部分不允许用户更改
// @param       *info       debug 输出的信息结构体
// @return      void
// Sample usage:            debug_output_init(info);
//-------------------------------------------------------------------------------------------------------------------
void debug_output_init (debug_output_struct *info)
{
    debug_output_info.type_index            = info->type_index;
    
    debug_output_info.display_x_max         = info->display_x_max;
    debug_output_info.display_y_max         = info->display_y_max;

    debug_output_info.font_x_size           = info->font_x_size;
    debug_output_info.font_y_size           = info->font_y_size;
    
    debug_output_info.output_uart           = info->output_uart;
    debug_output_info.output_screen         = info->output_screen;
    debug_output_info.output_screen_clear   = info->output_screen_clear;

    zf_debug_init_flag = 1;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       debug 串口初始化 此部分不允许用户更改
// @param       void
// @return      void
// Sample usage:            debug_init();
//-------------------------------------------------------------------------------------------------------------------
void debug_init (void)
{
    debug_output_struct info;
    debug_output_struct_init(&info);
    info.output_uart = debug_uart_str_output;
    debug_output_init(&info);

    uart_init(
        DEBUG_UART_INDEX,                                                       // 在 zf_common_debug.h 中查看对应值
        DEBUG_UART_BAUDRATE,                                                    // 在 zf_common_debug.h 中查看对应值
        DEBUG_UART_TX_PIN,                                                      // 在 zf_common_debug.h 中查看对应值
        DEBUG_UART_RX_PIN);                                                     // 在 zf_common_debug.h 中查看对应值

    if(DEBUG_UART_USE_INTERRUPT)                                                // 如果启用串口中断
    {
        uart_rx_interrupt(DEBUG_UART_INDEX, 1);                                 // 使能对应串口接收中断
    }
}

