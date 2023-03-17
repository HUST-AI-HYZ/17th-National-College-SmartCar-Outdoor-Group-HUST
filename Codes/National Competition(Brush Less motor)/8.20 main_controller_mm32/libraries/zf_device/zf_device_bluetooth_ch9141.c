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

#include "zf_device_type.h"
#include "zf_device_bluetooth_ch9141.h"

static  fifo_struct     bluetooth_ch9141_fifo;
static  uint8           bluetooth_ch9141_buffer[BLUETOOTH_CH9141_BUFFER_SIZE];  // 数据存放数组

static  uint8           bluetooth_ch9141_data;

//-------------------------------------------------------------------------------------------------------------------
//  @brief      蓝牙转串口模块 串口中断回调函数
//  @param      void
//  @return     void
//  Sample usage:
//  @note       该函数在ISR文件 串口中断程序被调用
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_ch9141_uart_callback (void)
{
    uart_query_byte(BLUETOOTH_CH9141_INDEX, &bluetooth_ch9141_data);            // 读取串口数据
    fifo_write_buffer(&bluetooth_ch9141_fifo, &bluetooth_ch9141_data, 1);       // 存入 FIFO
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       蓝牙转串口模块 发送数据
// @param       data            8bit 数据
// @return      uint32          剩余发送长度
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint32 bluetooth_ch9141_send_byte (const uint8 data)
{
    uint16 time_count = 0;
    while(gpio_get(BLUETOOTH_CH9141_RTS_PIN))                                   // 如果RTS为低电平，则继续发送数据
    {
        if(BLUETOOTH_CH9141_TIMEOUT_COUNT < time_count ++)
            return 1;                                                           // 模块忙,如果允许当前程序使用while等待 则可以使用后面注释的while等待语句替换本if语句
        system_delay_ms(1);
    }
    uart_write_byte(BLUETOOTH_CH9141_INDEX, data);                              // 发送最后的数据

    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      蓝牙转串口模块 发送函数
//  @param      buff            需要发送的数据地址
//  @param      len             发送长度
//  @return     uint32          剩余未发送的字节数
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint32 bluetooth_ch9141_send_buff (const uint8 *buff, uint32 len)
{
    uint16 time_count = 0;
    while(30 < len)
    {
        time_count = 0;
        while(gpio_get(BLUETOOTH_CH9141_RTS_PIN) && BLUETOOTH_CH9141_TIMEOUT_COUNT > time_count ++) // 如果RTS为低电平，则继续发送数据
            system_delay_ms(1);
        if(BLUETOOTH_CH9141_TIMEOUT_COUNT <= time_count)
            return len;                                                         // 模块忙,如果允许当前程序使用while等待 则可以使用后面注释的while等待语句替换本if语句
        uart_putbuff(BLUETOOTH_CH9141_INDEX, buff, 30);

        buff += 30;                                                             // 地址偏移
        len -= 30;                                                              // 数量
    }

    time_count = 0;
    while(gpio_get(BLUETOOTH_CH9141_RTS_PIN) && BLUETOOTH_CH9141_TIMEOUT_COUNT > time_count ++)     // 如果RTS为低电平，则继续发送数据
        system_delay_ms(1);
    if(time_count >= BLUETOOTH_CH9141_TIMEOUT_COUNT)
        return len;                                                             // 模块忙,如果允许当前程序使用while等待 则可以使用后面注释的while等待语句替换本if语句
    uart_putbuff(BLUETOOTH_CH9141_INDEX, buff, len);                            // 发送最后的数据

    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       蓝牙转串口模块 发送字符串
// @param       *str            要发送的字符串地址
// @return      uint32          剩余发送长度
// Sample usage:                bluetooth_ch9141_send_string("Believe in yourself.");
//-------------------------------------------------------------------------------------------------------------------
uint32 bluetooth_ch9141_send_string (const char *str)
{
    uint16 time_count = 0;
    uint32 len = strlen(str);
    while(30 < len)
    {
        time_count = 0;
        while(gpio_get(BLUETOOTH_CH9141_RTS_PIN))                               // 如果RTS为低电平，则继续发送数据
        {
            if(BLUETOOTH_CH9141_TIMEOUT_COUNT > time_count ++)
                return len;                                                     // 模块忙,如果允许当前程序使用while等待 则可以使用后面注释的while等待语句替换本if语句
            system_delay_ms(1);
        }
        uart_write_buffer(BLUETOOTH_CH9141_INDEX, (const uint8 *)str, 30);

        str += 30;                                                              // 地址偏移
        len -= 30;                                                              // 数量
    }

    time_count = 0;
    while(gpio_get(BLUETOOTH_CH9141_RTS_PIN))                                   // 如果RTS为低电平，则继续发送数据
    {
        if(BLUETOOTH_CH9141_TIMEOUT_COUNT > time_count ++)
            return len;                                                         // 模块忙,如果允许当前程序使用while等待 则可以使用后面注释的while等待语句替换本if语句
        system_delay_ms(1);
    }
    uart_write_buffer(BLUETOOTH_CH9141_INDEX, (const uint8 *)str, len);         // 发送最后的数据

    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       蓝牙转串口模块 发送摄像头图像至上位机查看图像
// @param       *image_addr     需要发送的图像地址
// @param       image_size      图像的大小
// @return      void
// Sample usage:                bluetooth_ch9141_send_image(&mt9v03x_image[0][0], MT9V03X_IMAGE_SIZE);
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_ch9141_send_image (const uint8 *image_addr, uint32 image_size)
{
    extern uint8 camera_send_image_frame_header[4];
    bluetooth_ch9141_send_buff(camera_send_image_frame_header, 4);
    bluetooth_ch9141_send_buff((uint8 *)image_addr, image_size);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       蓝牙转串口模块 读取函数
// @param       buff            存储的数据地址
// @param       len             长度
// @return      uint32          实际读取字节数
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint32 bluetooth_ch9141_read_buff (uint8 *buff, uint32 len)
{
    uint32 data_len = len;
    fifo_read_buffer(&bluetooth_ch9141_fifo, buff, &data_len, FIFO_READ_AND_CLEAN);
    return data_len;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       蓝牙转串口模块 初始化
// @param       mode            蓝牙模式 MASTER_MODE(主机)或者SLAVE_MODE(从机)
// @return      uint8           初始化状态 0-成功 1-失败
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 bluetooth_ch9141_init (void)
{
    set_wireless_type(BULETOOTH_CH9141);

    fifo_init(&bluetooth_ch9141_fifo, bluetooth_ch9141_buffer, BLUETOOTH_CH9141_BUFFER_SIZE);
    // 本函数使用的波特率为115200 为蓝牙转串口模块的默认波特率 如需其他波特率请使用上位机修改模块参数
    gpio_init(BLUETOOTH_CH9141_RTS_PIN, GPI, 1, GPI_PULL_UP);                   // 初始化流控引脚
    uart_init(BLUETOOTH_CH9141_INDEX, BLUETOOTH_CH9141_BUAD_RATE, BLUETOOTH_CH9141_RX_PIN, BLUETOOTH_CH9141_TX_PIN);
    uart_rx_interrupt(BLUETOOTH_CH9141_INDEX, 1);

    return 0;
}
