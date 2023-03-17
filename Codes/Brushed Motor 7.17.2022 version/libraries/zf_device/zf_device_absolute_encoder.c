/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_absolute_encoder
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
*                   // 硬件 SPI 引脚
*                   SCLK                查看 zf_device_absolute_encoder.h 中 ABSOLUTE_ENCODER_SCLK_PIN 宏定义
*                   MOSI                查看 zf_device_absolute_encoder.h 中 ABSOLUTE_ENCODER_MOSI_PIN 宏定义
*                   MISO                查看 zf_device_absolute_encoder.h 中 ABSOLUTE_ENCODER_MISO_PIN 宏定义
*                   CS                  查看 zf_device_absolute_encoder.h 中 ABSOLUTE_ENCODER_CS_PIN 宏定义
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   ------------------------------------
********************************************************************************************************************/

#include "zf_device_absolute_encoder.h"

static int16 now_location = 0;
static int16 last_location = 0;

#if ABSOLUTE_ENCODER_USE_SOFT_SPI
static soft_spi_info_struct                 absolute_encoder_spi;
#define absolute_encoder_read()             (soft_spi_read_8bit(&absolute_encoder_spi))
#define absolute_encoder_write(data)        (soft_spi_write_8bit(&absolute_encoder_spi, (data)))
#else
#define absolute_encoder_read()             (spi_read_8bit(ABSOLUTE_ENCODER_SPI))
#define absolute_encoder_write(data)        (spi_write_8bit(ABSOLUTE_ENCODER_SPI, (data)))
#endif

//-------------------------------------------------------------------------------------------------------------------
// @brief       绝对值编码器写寄存器 内部调用
// @param       reg             寄存器地址
// @param       data            数据
// @return      void
//-------------------------------------------------------------------------------------------------------------------
static void absolute_encoder_write_register(uint8 reg, uint8 data)
{
    ABSOLUTE_ENCODER_CSN(0);                                                    // 片选拉低选中
    absolute_encoder_write(reg | ABS_ENCODER_SPI_W);                            // 寄存器
    absolute_encoder_write(data);                                               // 数据
    ABSOLUTE_ENCODER_CSN(1);                                                    // 片选拉高释放
    system_delay_us(1);                                                         // 必要操作
    ABSOLUTE_ENCODER_CSN(0);                                                    // 片选拉低选中
    absolute_encoder_read();                                                    // 这里会返回写入是否成功 但不作判断
    absolute_encoder_read();                                                    // 必要操作
    ABSOLUTE_ENCODER_CSN(1);                                                    // 片选拉高释放
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       绝对值编码器读寄存器 内部调用
// @param       reg             寄存器地址
// @return      uint8           数据
//-------------------------------------------------------------------------------------------------------------------
static uint8 absolute_encoder_read_register(uint8 reg)
{
    uint8 data = 0;
    ABSOLUTE_ENCODER_CSN(0);                                                    // 片选拉低选中
    absolute_encoder_write(reg | ABS_ENCODER_SPI_R);                            // 寄存器
    absolute_encoder_write(0x00);                                               // 占位
    ABSOLUTE_ENCODER_CSN(1);                                                    // 片选拉高释放
    system_delay_us(1);                                                         // 必要操作
    ABSOLUTE_ENCODER_CSN(0);                                                    // 片选拉低选中
    data = absolute_encoder_read();                                             // 获取读取的数据
    absolute_encoder_read();                                                    // 必要操作
    ABSOLUTE_ENCODER_CSN(1);                                                    // 片选拉高释放
    return data;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       绝对值编码器读位置 内部调用
// @param       void
// @return      uint16          位置值
//-------------------------------------------------------------------------------------------------------------------
static uint16 absolute_encoder_read_data (void)
{
    uint16 data = 0;
    ABSOLUTE_ENCODER_CSN(0);                                                    // 片选拉低选中
    data = absolute_encoder_read();                                             // 获取高八位数据
    data = (data & 0x00FF) << 8;                                                // 数据位移
    data |= absolute_encoder_read();                                            // 获取低八位数据
    ABSOLUTE_ENCODER_CSN(1);                                                    // 片选拉高释放
    return data;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       绝对值编码器自检 内部调用
// @param       void
// @return      uint8           自检状态
//-------------------------------------------------------------------------------------------------------------------
static uint8 absolute_encoder_self_check (void)
{
    uint8 i = 0, return_state = 0;
    uint8 dat[6] = {0, 0, 0, 0xC0, 0xFF, 0x1C};
    uint16 time_count = 0;
    while(0x1C != absolute_encoder_read_register(6))                            // 获取状态寄存器
    {
        for(i = 0; i < 6; i ++)
        {
            absolute_encoder_write_register(i + 1, dat[i]);                     // 写入默认配置参数
            system_delay_ms(1);
        }
        if(time_count ++ > ABSOLUTE_ENCODER_TIMEOUT_COUNT)                      // 等待超时
        {
            return_state = 1;
            break;
        }
    }
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       绝对值编码器获取当前角度值
// @param       void
// @return      int16           角度值
// Sample usage:                absolute_encoder_get_location();
//-------------------------------------------------------------------------------------------------------------------
int16 absolute_encoder_get_location (void)
{
    last_location = now_location;
    now_location = absolute_encoder_read_data() >> 4;
    return now_location;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       绝对值编码器获取相较上次位置的偏移值
// @param       void
// @return      int16           偏移值
// Sample usage:                absolute_encoder_get_offset();
//-------------------------------------------------------------------------------------------------------------------
int16 absolute_encoder_get_offset (void)
{
    if(myabs(now_location-last_location) > 2048)
        return (now_location > 2048 ? (now_location - 4096 - last_location) : (now_location + 4096 - last_location));
    else
        return (now_location - last_location);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       绝对值编码器初始化
// @param       void
// @return      uint8           初始化状态 0-成功 1-失败
// Sample usage:                absolute_encoder_init();
//-------------------------------------------------------------------------------------------------------------------
uint8 absolute_encoder_init (void)
{
    uint16 zero_position = ABSOLUTE_ENCODER_DEFAULT_ZERO;
#if ABSOLUTE_ENCODER_USE_SOFT_SPI
    soft_spi_init(&absolute_encoder_spi, 0, ABSOLUTE_ENCODER_SOFT_SPI_DELAY, ABSOLUTE_ENCODER_SCLK_PIN, ABSOLUTE_ENCODER_MOSI_PIN, ABSOLUTE_ENCODER_MISO_PIN, SOFT_SPI_PIN_NULL);
#else
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 就去检查你修改的引脚的宏定义是不是跟你的 SPI 宏定义不一致
    zf_assert(ABSOLUTE_ENCODER_SPI != (ABSOLUTE_ENCODER_SCLK_PIN & 0xF000 >> 12));
    zf_assert(ABSOLUTE_ENCODER_SPI != (ABSOLUTE_ENCODER_MOSI_PIN & 0xF000 >> 12));
    spi_init(ABSOLUTE_ENCODER_SPI, SPI_MODE0, ABSOLUTE_ENCODER_SPI_SPEED, ABSOLUTE_ENCODER_SCLK_PIN, ABSOLUTE_ENCODER_MOSI_PIN, ABSOLUTE_ENCODER_MISO_PIN, SPI_CS_NULL);
#endif
    gpio_init(ABSOLUTE_ENCODER_CS_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);

    if(absolute_encoder_self_check())
    {
        // 如果程序在输出了断言信息 并且提示出错位置在这里
        // 那么就是绝对值编码器自检出错并超时退出了
        // 检查一下接线有没有问题 如果没问题可能就是坏了
        zf_assert(0);
    }
    absolute_encoder_write_register(DIR_REG, 0x00);                             // 设置旋转方向 正转数值变小：0x00   反转数值变大：0x80
    zero_position = (uint16)(4096 - zero_position);
    zero_position = zero_position << 4;
    absolute_encoder_write_register(ZERO_L_REG, (uint8)zero_position);          // 设置零位
    absolute_encoder_write_register(ZERO_H_REG, zero_position >> 8);
    return 0;
}
