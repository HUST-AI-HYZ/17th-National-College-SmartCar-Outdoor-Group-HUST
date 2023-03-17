/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_driver_uart
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#include "zf_driver_gpio.h"
#include "zf_driver_uart.h"

static  UART_Type *uart_index[8] = {UART1, UART2, UART3, UART4, UART5, UART6, UART7, UART8};
static  IRQn_Type uart_irq[8] = {UART1_IRQn, UART2_IRQn, UART3_IRQn, UART4_IRQn, UART5_IRQn, UART6_IRQn, UART7_IRQn, UART8_IRQn};

//-------------------------------------------------------------------------------------------------------------------
// @brief       串口字节输出
// @param       uart_n          串口模块号 参照 zf_driver_uart.h 内 uart_index_enum 枚举体定义
// @param       dat             需要发送的字节
// @return      void        
// Sample usage:                uart_write_byte(UART_1, 0xA5);                  // 串口1发送0xA5
//-------------------------------------------------------------------------------------------------------------------
void uart_write_byte (uart_index_enum uart_n, const uint8 dat)
{
    uart_index[uart_n]->TDR = dat;                                              // 写入发送数据
    while(!(uart_index[uart_n]->CSR & (0x00000001)));                           // 等待发送完成
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       串口发送数组
// @param       uart_n          串口模块号 参照 zf_driver_uart.h 内 uart_index_enum 枚举体定义
// @param       *buff           要发送的数组地址
// @param       len             发送长度
// @return      void
// Sample usage:                uart_write_buffer(UART_1, &a[0], 5);
//-------------------------------------------------------------------------------------------------------------------
void uart_write_buffer (uart_index_enum uart_n, const uint8 *buff, uint32 len)
{
    while(len)                                                                  // 循环到发送完
    {
        uart_index[uart_n]->TDR = *buff ++;                                     // 写入发送数据
        while(!(uart_index[uart_n]->CSR & (0x00000001)));                       // 等待发送完成
        len --;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       串口发送字符串
// @param       uart_n          串口模块号 参照 zf_driver_uart.h 内 uart_index_enum 枚举体定义
// @param       *str            要发送的字符串地址
// @return      void
// Sample usage:                uart_write_string(UART_1, "seekfree"); 
//-------------------------------------------------------------------------------------------------------------------
void uart_write_string (uart_index_enum uart_n, const char *str)
{
    while(*str)                                                                 // 一直循环到结尾
    {
        uart_write_byte(uart_n, *str ++);                                       // 发送数据
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       读取串口接收的数据（whlie等待）
// @param       uart_n          串口模块号 参照 zf_driver_uart.h 内 uart_index_enum 枚举体定义
// @param       *dat            接收数据的地址
// @return      uint8           接收的数据
// Sample usage:                uint8 dat = uart_read_byte(UART_1);             // 接收 UART_1 数据  存在在 dat 变量里
//-------------------------------------------------------------------------------------------------------------------
uint8 uart_read_byte (uart_index_enum uart_n)
{
    while(!(uart_index[uart_n]->CSR & (0x00000002)));                           // 等待读取到一个数据
    return (uint8)uart_index[uart_n]->RDR;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       读取串口接收的数据（查询接收）
// @param       uart_n          串口模块号 参照 zf_driver_uart.h 内 uart_index_enum 枚举体定义
// @param       *dat            接收数据的地址
// @return      uint8           1：接收成功   0：未接收到数据
// Sample usage:                uint8 dat; uart_query_byte(UART_1, &dat);       // 接收 UART_1 数据  存在在 dat 变量里
//-------------------------------------------------------------------------------------------------------------------
uint8 uart_query_byte (uart_index_enum uart_n, uint8 *dat)
{
    if(uart_index[uart_n]->CSR & (0x00000002))                                  // 读取到一个数据
    {
        *dat = (uint8)uart_index[uart_n]->RDR;                                  // 存储一个数据
        return 1;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       串口发送中断设置
// @param       uart_n           串口模块号
// @param       status          1：打开中断   0：关闭中断
// @return      void        
// Sample usage:                uart_tx_interrupt(UART_1, 1);                   // 打开串口1发送完成中断
//-------------------------------------------------------------------------------------------------------------------
void uart_tx_interrupt (uart_index_enum uart_n, uint32 status)
{
    if(status)
        uart_index[uart_n]->IER |= (0x00000001);                                // 使能发送完成中断
    else
        uart_index[uart_n]->IER &= ~(0x00000001);                               // 关闭发送完成中断

    if(status)  interrupt_enable(uart_irq[uart_n]);
    else        interrupt_disable(uart_irq[uart_n]);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       串口接收中断设置
// @param       uart_n           串口模块号
// @param       status          1：打开中断   0：关闭中断
// @return      void        
// Sample usage:                uart_rx_interrupt(UART_1, 1);                   // 打开串口1接收完成中断
//-------------------------------------------------------------------------------------------------------------------
void uart_rx_interrupt (uart_index_enum uart_n, uint32 status)
{
    if(status)
        uart_index[uart_n]->IER |= (0x00000002);                                // 使能接收完成中断
    else
        uart_index[uart_n]->IER &= ~(0x00000002);                               // 关闭接收完成中断

    if(status)  interrupt_enable(uart_irq[uart_n]);
    else        interrupt_disable(uart_irq[uart_n]);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       串口初始化
// @param       uart_n          串口模块号 参照 zf_driver_uart.h 内 uart_index_enum 枚举体定义
// @param       baud            串口波特率
// @param       tx_pin          串口发送引脚 参照 zf_driver_uart.h 内 uart_tx_pin_enum 枚举体定义
// @param       rx_pin          串口接收引脚 参照 zf_driver_uart.h 内 uart_rx_pin_enum 枚举体定义
// @return      void            
// Sample usage:                uart_init(UART_1, 115200, UART1_TX_A09, UART1_RX_A10);  // 初始化串口1 波特率115200 发送引脚使用PA09 接收引脚使用PA10
//-------------------------------------------------------------------------------------------------------------------
void uart_init(uart_index_enum uart_n, uint32 baud, uart_tx_pin_enum tx_pin, uart_rx_pin_enum rx_pin)
{
    afio_init((gpio_pin_enum)(tx_pin & 0xFF), GPO, (gpio_af_enum)((tx_pin & 0xF00) >> 8), GPO_AF_PUSH_PULL);// 提取对应IO索引 AF功能编码
    afio_init((gpio_pin_enum)(rx_pin & 0xFF), GPI, (gpio_af_enum)((rx_pin & 0xF00) >> 8), GPI_FLOATING_IN); // 提取对应IO索引 AF功能编码

    switch(uart_n)
    {
        case UART_1:
            RCC->APB2ENR |= ((uint32)0x00000001 << 4);                        // 使能 UART1 时钟
            RCC->APB2RSTR |= ((uint32)0x00000001 << 4);                       // 复位 UART1
            RCC->APB2RSTR &= ~((uint32)0x00000001 << 4);                      // 取消复位 UART1
            break;
        case UART_2:
            RCC->APB1ENR |= ((uint32)0x00000001 << 17);                       // 使能 UART2 时钟
            RCC->APB1RSTR |= ((uint32)0x00000001 << 17);                      // 复位 UART2
            RCC->APB1RSTR &= ~((uint32)0x00000001 << 17);                     // 完成复位 UART2
            break;
        case UART_3:
            RCC->APB1ENR |= ((uint32)0x00000001 << 18);                       // 使能 UART3 时钟
            RCC->APB1RSTR |= ((uint32)0x00000001 << 18);                      // 复位 UART3
            RCC->APB1RSTR &= ~((uint32)0x00000001 << 18);                     // 完成复位 UART3
            break;
        case UART_4:
            RCC->APB1ENR |= ((uint32)0x00000001 << 19);                       // 使能 UART4 时钟
            RCC->APB1RSTR |= ((uint32)0x00000001 << 19);                      // 复位 UART4
            RCC->APB1RSTR &= ~((uint32)0x00000001 << 19);                     // 完成复位 UART4
            break;
        case UART_5:
            RCC->APB1ENR |= ((uint32)0x00000001 << 20);                       // 使能 UART5 时钟
            RCC->APB1RSTR |= ((uint32)0x00000001 << 20);                      // 复位 UART5
            RCC->APB1RSTR &= ~((uint32)0x00000001 << 20);                     // 完成复位 UART5
            break;
        case UART_6:
            RCC->APB2ENR |= ((uint32)0x00000001 << 5);                        // 使能 UART6 时钟
            RCC->APB2RSTR |= ((uint32)0x00000001 << 5);                       // 复位 UART6
            RCC->APB2RSTR &= ~((uint32)0x00000001 << 5);                      // 完成复位 UART6
            break;
        case UART_7:
            RCC->APB1ENR |= ((uint32)0x00000001 << 30);                       // 使能 UART7 时钟
            RCC->APB1RSTR |= ((uint32)0x00000001 << 30);                      // 复位 UART7
            RCC->APB1RSTR &= ~((uint32)0x00000001 << 30);                     // 完成复位 UART7
            break;
        case UART_8:
            RCC->APB1ENR |= ((uint32)0x00000001 << 31);                       // 使能 UART8 时钟
            RCC->APB1RSTR |= ((uint32)0x00000001 << 31);                      // 复位 UART8
            RCC->APB1RSTR &= ~((uint32)0x00000001 << 31);                     // 完成复位 UART8
            break;
    }

    if(SYSTEM_CLOCK_120M < system_clock)
    {
        uart_index[uart_n]->BRR = (system_clock / 2 / baud) / 16;               // 设置波特率
        uart_index[uart_n]->FRA = (system_clock / 2 / baud) % 16;               // 设置波特率
    }
    else
    {
        uart_index[uart_n]->BRR = (system_clock / baud) / 16;                   // 设置波特率
        uart_index[uart_n]->FRA = (system_clock / baud) % 16;                   // 设置波特率
    }
    uart_index[uart_n]->CCR |= (0x00000003 << 4);                               // 8bits 数据位
    uart_index[uart_n]->GCR |= (0x00000019);                                    // 使能 TX RX UART
}
