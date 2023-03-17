/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_driver_soft_spi
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#include "zf_driver_soft_spi.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 延时 内部调用
// @param       void
// @return      void
//-------------------------------------------------------------------------------------------------------------------
//static void soft_spi_delay (uint32 delay)
//{
//    volatile uint32 count = delay;
//    while(count --);
//}
#define soft_spi_delay(x)  for(uint32 i = x; i--; )

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 8bit 数据读写 内部调用
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       data            数据
// @return      uint8           读取的数据
//-------------------------------------------------------------------------------------------------------------------
static uint8 soft_spi_8bit_data_handler (soft_spi_info_struct *soft_spi_obj, const uint8 data)
{
    uint8 temp = 0;
    uint8 write_data = data;
    uint8 read_data = 0;

    if(soft_spi_obj->config.use_cs)
    {
        gpio_low(soft_spi_obj->cs_pin);
    }

    if(0 == soft_spi_obj->config.mode || 1 == soft_spi_obj->config.mode)        // CPOL = 0 SCK 空闲低电平
    {
        gpio_low(soft_spi_obj->sck_pin);
    }
    else                                                                        // CPOL = 1 SCK 空闲高电平
    {
        gpio_high(soft_spi_obj->sck_pin);
    }

    if(0 == soft_spi_obj->config.mode % 2)                                      // CPHA = 0 第一个边沿采样
    {
        for(temp = 8; temp > 0; temp --)
        {
            if(0x80 & write_data)
            {
                gpio_high(soft_spi_obj->mosi_pin);
            }
            else
            {
                gpio_low(soft_spi_obj->mosi_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
            gpio_toggle_level(soft_spi_obj->sck_pin);
            write_data = write_data << 1;
            read_data = read_data << 1;
            if(soft_spi_obj->config.use_miso)
            {
                read_data |= gpio_get_level(soft_spi_obj->miso_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
            gpio_toggle_level(soft_spi_obj->sck_pin);
        }
    }
    else                                                                        // CPHA = 1 第二个边沿采样
    {
        for(temp = 8; 0 < temp; temp --)
        {
            gpio_toggle_level(soft_spi_obj->sck_pin);
            if(0x80 & write_data)
            {
                gpio_high(soft_spi_obj->mosi_pin);
            }
            else
            {
                gpio_low(soft_spi_obj->mosi_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
            gpio_toggle_level(soft_spi_obj->sck_pin);
            write_data = write_data << 1;
            read_data = read_data << 1;
            if(soft_spi_obj->config.use_miso)
            {
                read_data |= gpio_get_level(soft_spi_obj->miso_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
        }
    }

    if(soft_spi_obj->config.use_cs)
    {
        gpio_high(soft_spi_obj->cs_pin);
    }
    return read_data;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 16bit 数据读写 内部调用
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       data            数据
// @return      uint16          读取的数据
//-------------------------------------------------------------------------------------------------------------------
static uint16 soft_spi_16bit_data_handler (soft_spi_info_struct *soft_spi_obj, const uint16 data)
{
    uint8 temp = 0;
    uint16 write_data = data;
    uint16 read_data = 0;

    if(soft_spi_obj->config.use_cs)
    {
        gpio_low(soft_spi_obj->cs_pin);
    }

    if(0 == soft_spi_obj->config.mode || 1 == soft_spi_obj->config.mode)        // CPOL = 0 SCK 空闲低电平
    {
        gpio_low(soft_spi_obj->sck_pin);
    }
    else                                                                        // CPOL = 1 SCK 空闲高电平
    {
        gpio_high(soft_spi_obj->sck_pin);
    }

    if(0 == soft_spi_obj->config.mode % 2)                                      // CPHA = 0 第一个边沿采样
    {
        for(temp = 16; 0 < temp; temp --)
        {
            if(0x8000 & write_data)
            {
                gpio_high(soft_spi_obj->mosi_pin);
            }
            else
            {
                gpio_low(soft_spi_obj->mosi_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
            gpio_toggle_level(soft_spi_obj->sck_pin);
            write_data = write_data << 1;
            read_data = read_data << 1;
            if(soft_spi_obj->config.use_miso)
            {
                read_data |= gpio_get_level(soft_spi_obj->miso_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
            gpio_toggle_level(soft_spi_obj->sck_pin);
        }
    }
    else                                                                        // CPHA = 1 第二个边沿采样
    {
        for(temp = 16; 0 < temp; temp --)
        {
            gpio_toggle_level(soft_spi_obj->sck_pin);
            if(write_data & 0x8000)
            {
                gpio_high(soft_spi_obj->mosi_pin);
            }
            else
            {
                gpio_low(soft_spi_obj->mosi_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
            gpio_toggle_level(soft_spi_obj->sck_pin);
            write_data = write_data << 1;
            read_data = read_data << 1;
            if(soft_spi_obj->config.use_miso)
            {
                read_data |= gpio_get_level(soft_spi_obj->miso_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
        }
    }

    if(soft_spi_obj->config.use_cs)
    {
        gpio_high(soft_spi_obj->cs_pin);
    }
    return read_data;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 接口写 8bit 数据
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       data            发送的数据
// @return      void
// Sample usage:                soft_spi_write_8bit(&soft_spi_obj, 1);
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_8bit (soft_spi_info_struct *soft_spi_obj, const uint8 data)
{
    soft_spi_8bit_data_handler(soft_spi_obj, data);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 接口写 8bit 数组
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       *data           数据存放缓冲区
// @param       len             缓冲区长度
// @return      void
// Sample usage:                soft_spi_write_8bit_array(&soft_spi_obj, buf, 16);
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_8bit_array (soft_spi_info_struct *soft_spi_obj, const uint8 *data, uint32 len)
{
    while(len --)
        soft_spi_8bit_data_handler(soft_spi_obj, *data ++);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 接口写 16bit 数据
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       data            发送的数据
// @return      void
// Sample usage:                soft_spi_write_16bit(&soft_spi_obj, 1);
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_16bit (soft_spi_info_struct *soft_spi_obj, uint16 data)
{
    soft_spi_16bit_data_handler(soft_spi_obj, data);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 接口写 16bit 数组
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       *data           数据存放缓冲区
// @param       len             缓冲区长度
// @return      void
// Sample usage:                soft_spi_write_16bit_array(&soft_spi_obj, buf, 16);
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_16bit_array (soft_spi_info_struct *soft_spi_obj, const uint16 *data, uint32 len)
{
    while(len --)
        soft_spi_16bit_data_handler(soft_spi_obj, *data ++);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 接口向传感器的寄存器写 8bit 数据
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       register_name   寄存器地址
// @param       data            发送的数据
// @return      void
// Sample usage:                soft_spi_write_8bit_register(&soft_spi_obj, 1, 1);
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_8bit_register (soft_spi_info_struct *soft_spi_obj, const uint8 register_name, const uint8 data)
{
    soft_spi_8bit_data_handler(soft_spi_obj, register_name);
    soft_spi_8bit_data_handler(soft_spi_obj, data);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 接口向传感器的寄存器写 8bit 数组
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       register_name   寄存器地址
// @param       *data           数据存放缓冲区
// @param       len             缓冲区长度
// @return      void
// Sample usage:                soft_spi_write_8bit_registers(&soft_spi_obj, 1, buf, 16);
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_8bit_registers (soft_spi_info_struct *soft_spi_obj, const uint8 register_name, const uint8 *data, uint32 len)
{
    soft_spi_8bit_data_handler(soft_spi_obj, register_name);
    while(len --)
        soft_spi_8bit_data_handler(soft_spi_obj, *data ++);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 接口向传感器的寄存器写 16bit 数据
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       register_name   寄存器地址
// @param       data            发送的数据
// @return      void
// Sample usage:                soft_spi_write_16bit_register(&soft_spi_obj, 1, 1);
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_16bit_register (soft_spi_info_struct *soft_spi_obj, const uint16 register_name, uint16 data)
{
    soft_spi_16bit_data_handler(soft_spi_obj, register_name);
    soft_spi_16bit_data_handler(soft_spi_obj, data);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 接口向传感器的寄存器写 16bit 数组
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       register_name   寄存器地址
// @param       *data           数据存放缓冲区
// @param       len             缓冲区长度
// @return      void
// Sample usage:                soft_spi_write_16bit_registers(&soft_spi_obj, 1, buf, 16);
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_16bit_registers (soft_spi_info_struct *soft_spi_obj, const uint16 register_name, const uint16 *data, uint32 len)
{
    soft_spi_16bit_data_handler(soft_spi_obj, register_name);
    while(len --)
        soft_spi_16bit_data_handler(soft_spi_obj, *data ++);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 接口读 8bit 数据
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       data            发送的数据
// @return      uint8           返回读取的 8bit 数据
// Sample usage:                soft_spi_read_8bit(&soft_spi_obj);
//-------------------------------------------------------------------------------------------------------------------
uint8 soft_spi_read_8bit (soft_spi_info_struct *soft_spi_obj)
{
    return soft_spi_8bit_data_handler(soft_spi_obj, 0);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 接口读 8bit 数组
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       *data           数据存放缓冲区
// @param       len             缓冲区长度
// @return      void
// Sample usage:                soft_spi_read_8bit_array(&soft_spi_obj, buf, 16);
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_read_8bit_array (soft_spi_info_struct *soft_spi_obj, uint8 *data, uint32 len)
{
    while(len --)
        *data ++ = soft_spi_8bit_data_handler(soft_spi_obj, 0);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 接口读 16bit 数据
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       data            发送的数据
// @return      uint16          返回读取的 16bit 数据
// Sample usage:                soft_spi_read_16bit(&soft_spi_obj);
//-------------------------------------------------------------------------------------------------------------------
uint16 soft_spi_read_16bit (soft_spi_info_struct *soft_spi_obj)
{
    return soft_spi_16bit_data_handler(soft_spi_obj, 0);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 接口读 16bit 数组
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       *data           数据存放缓冲区
// @param       len             缓冲区长度
// @return      void
// Sample usage:                soft_spi_read_16bit_array(&soft_spi_obj, buf, 16);
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_read_16bit_array (soft_spi_info_struct *soft_spi_obj, uint16 *data, uint32 len)
{
    while(len --)
        *data ++ = soft_spi_16bit_data_handler(soft_spi_obj, 0);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 接口从传感器的寄存器读 8bit 数据
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       register_name   寄存器地址
// @param       data            发送的数据
// @return      uint8           返回读取的 8bit 数据
// Sample usage:                soft_spi_read_8bit_register(&soft_spi_obj, 0x01, 0x01);
//-------------------------------------------------------------------------------------------------------------------
uint8 soft_spi_read_8bit_register (soft_spi_info_struct *soft_spi_obj, const uint8 register_name)
{
    soft_spi_8bit_data_handler(soft_spi_obj, register_name);
    return soft_spi_8bit_data_handler(soft_spi_obj, 0);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 接口从传感器的寄存器读 8bit 数组
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       register_name   寄存器地址
// @param       *data           数据存放缓冲区
// @param       len             缓冲区长度
// @return      void
// Sample usage:                soft_spi_read_8bit_registers(&soft_spi_obj, 0x01, buf, 16);
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_read_8bit_registers (soft_spi_info_struct *soft_spi_obj, const uint8 register_name, uint8 *data, uint32 len)
{
    soft_spi_8bit_data_handler(soft_spi_obj, register_name);
    while(len --)
        *data ++ = soft_spi_8bit_data_handler(soft_spi_obj, 0);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 接口从传感器的寄存器读 16bit 数据
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       register_name   寄存器地址
// @param       data            发送的数据
// @return      uint16          返回读取的 16bit 数据
// Sample usage:                soft_spi_read_16bit_register(&soft_spi_obj, 0x0101);
//-------------------------------------------------------------------------------------------------------------------
uint16 soft_spi_read_16bit_register (soft_spi_info_struct *soft_spi_obj, const uint16 register_name)
{
    soft_spi_16bit_data_handler(soft_spi_obj, register_name);
    return soft_spi_16bit_data_handler(soft_spi_obj, 0);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 接口从传感器的寄存器读 16bit 数组
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       register_name   寄存器地址
// @param       *data           数据存放缓冲区
// @param       len             缓冲区长度
// @return      void
// Sample usage:                soft_spi_read_16bit_registers(&soft_spi_obj, 0x0101, buf, 16);
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_read_16bit_registers (soft_spi_info_struct *soft_spi_obj, const uint16 register_name, uint16 *data, uint32 len)
{
    soft_spi_16bit_data_handler(soft_spi_obj, register_name);
    while(len --)
        *data ++ = soft_spi_16bit_data_handler(soft_spi_obj, 0);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 8bit 数据传输 发送与接收数据是同时进行的
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       write_buffer    发送的数据缓冲区地址
// @param       read_buffer     发送数据时接收到的数据的存储地址(不需要接收则传 NULL)
// @param       len             发送的字节数
// @return      void
// Sample usage:                soft_spi_8bit_transfer(&soft_spi_obj, buf, buf, 1);
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_8bit_transfer (soft_spi_info_struct *soft_spi_obj, const uint8 *write_buffer, uint8 *read_buffer, uint32 len)
{
    while(len --)
    {
        *read_buffer = soft_spi_8bit_data_handler(soft_spi_obj, *write_buffer);
        write_buffer ++;
        read_buffer ++;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 16bit 数据传输 发送与接收数据是同时进行的
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       write_buffer    发送的数据缓冲区地址
// @param       read_buffer     发送数据时接收到的数据的存储地址(不需要接收则传 NULL)
// @param       len             发送的字节数
// @return      void
// Sample usage:                soft_spi_16bit_transfer(&soft_spi_obj, buf, buf, 1);
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_16bit_transfer (soft_spi_info_struct *soft_spi_obj, const uint16 *write_buffer, uint16 *read_buffer, uint32 len)
{
    while(len --)
    {
        *read_buffer = soft_spi_16bit_data_handler(soft_spi_obj, *write_buffer);
        write_buffer ++;
        read_buffer ++;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 接口初始化
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       mode            SPI 模式 参照 zf_driver_spi.h 内 spi_mode_enum 枚举体定义
// @param       delay           软件 SPI 延时 就是时钟高电平时间 越短 SPI 速率越高
// @param       sck_pin         选择 SCK 引脚 参照 zf_driver_gpio.h 内 gpio_pin_enum 枚举体定义
// @param       mosi_pin        选择 MOSI 引脚 参照 zf_driver_gpio.h 内 gpio_pin_enum 枚举体定义
// @param       miso_pin        选择 MISO 引脚 如果不需要这个引脚 就填 SOFT_SPI_PIN_NULL
// @param       cs_pin          选择 CS 引脚 如果不需要这个引脚 就填 SOFT_SPI_PIN_NULL
// @return      void
// Sample usage:                spi_init(SPI_1, 0, 1*1000*1000, A5, A7, A6, A4);
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_init (soft_spi_info_struct *soft_spi_obj, uint8 mode, uint32 delay, gpio_pin_enum sck_pin, gpio_pin_enum mosi_pin, uint32 miso_pin, uint32 cs_pin)
{
    soft_spi_obj->config.mode = mode;
    soft_spi_obj->delay = delay;

    soft_spi_obj->sck_pin = sck_pin;
    soft_spi_obj->mosi_pin = mosi_pin;
    if(0 == mode || 1 == mode)
    {
        gpio_init(sck_pin, GPO, GPIO_LOW, GPO_PUSH_PULL);                       // IO 初始化
    }
    else
    {
        gpio_init(sck_pin, GPO, GPIO_HIGH, GPO_PUSH_PULL);                      // IO 初始化
    }
    gpio_init(mosi_pin, GPO, GPIO_HIGH, GPO_PUSH_PULL);                         // IO 初始化

    if(SOFT_SPI_PIN_NULL == miso_pin)
    {
        soft_spi_obj->config.use_miso = 0;
    }
    else
    {
        soft_spi_obj->config.use_miso = 1;
        soft_spi_obj->miso_pin = (gpio_pin_enum)miso_pin;
        gpio_init(soft_spi_obj->miso_pin, GPI, GPIO_HIGH, GPI_FLOATING_IN);     // IO 初始化
    }
    if(SOFT_SPI_PIN_NULL == cs_pin)
    {
        soft_spi_obj->config.use_cs = 0;
    }
    else
    {
        soft_spi_obj->config.use_cs = 1;
        soft_spi_obj->cs_pin = (gpio_pin_enum)cs_pin;
        gpio_init(soft_spi_obj->cs_pin, GPO, GPIO_HIGH, GPO_PUSH_PULL);         // IO 初始化
    }
}
