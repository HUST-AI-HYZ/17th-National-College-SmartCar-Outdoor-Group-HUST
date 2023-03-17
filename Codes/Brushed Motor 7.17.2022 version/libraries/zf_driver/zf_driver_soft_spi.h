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

#ifndef _zf_driver_soft_spi_h_
#define _zf_driver_soft_spi_h_

#include "zf_common_debug.h"

#include "zf_driver_gpio.h"

#define SOFT_SPI_PIN_NULL       (0xFFFF)                                        // 用于区分是否分配引脚

typedef struct
{
    union
    {
        uint8           config_info;                                            // 配置整体数据
        struct
        {
            uint8       mode        :6;                                         // SPI 模式
            uint8       use_miso    :1;                                         // 是否使用 MISO 引脚
            uint8       use_cs      :1;                                         // 是否使用 CS 引脚
        };
    }config;
    gpio_pin_enum       sck_pin;                                                // 用于记录对应的引脚编号
    gpio_pin_enum       mosi_pin;                                               // 用于记录对应的引脚编号
    gpio_pin_enum       miso_pin;                                               // 用于记录对应的引脚编号
    gpio_pin_enum       cs_pin;                                                 // 用于记录对应的引脚编号
    uint32              delay;                                                  // 模拟 SPI 软延时时长
}soft_spi_info_struct;

void        soft_spi_write_8bit                 (soft_spi_info_struct *soft_spi_obj, const uint8 data);
void        soft_spi_write_8bit_array           (soft_spi_info_struct *soft_spi_obj, const uint8 *data, uint32 len);

void        soft_spi_write_16bit                (soft_spi_info_struct *soft_spi_obj, const uint16 data);
void        soft_spi_write_16bit_array          (soft_spi_info_struct *soft_spi_obj, const uint16 *data, uint32 len);

void        soft_spi_write_8bit_register        (soft_spi_info_struct *soft_spi_obj, const uint8 register_name, const uint8 data);
void        soft_spi_write_8bit_registers       (soft_spi_info_struct *soft_spi_obj, const uint8 register_name, const uint8 *data, uint32 len);

void        soft_spi_write_16bit_register       (soft_spi_info_struct *soft_spi_obj, const uint16 register_name, const uint16 data);
void        soft_spi_write_16bit_registers      (soft_spi_info_struct *soft_spi_obj, const uint16 register_name, const uint16 *data, uint32 len);

uint8       soft_spi_read_8bit                  (soft_spi_info_struct *soft_spi_obj);
void        soft_spi_read_8bit_array            (soft_spi_info_struct *soft_spi_obj, uint8 *data, uint32 len);

uint16      soft_spi_read_16bit                 (soft_spi_info_struct *soft_spi_obj);
void        soft_spi_read_16bit_array           (soft_spi_info_struct *soft_spi_obj, uint16 *data, uint32 len);

uint8       soft_spi_read_8bit_register         (soft_spi_info_struct *soft_spi_obj, const uint8 register_name);
void        soft_spi_read_8bit_registers        (soft_spi_info_struct *soft_spi_obj, const uint8 register_name, uint8 *data, uint32 len);

uint16      soft_spi_read_16bit_register        (soft_spi_info_struct *soft_spi_obj, const uint16 register_name);
void        soft_spi_read_16bit_registers       (soft_spi_info_struct *soft_spi_obj, const uint16 register_name, uint16 *data, uint32 len);

void        soft_spi_8bit_transfer              (soft_spi_info_struct *soft_spi_obj, const uint8 *write_buffer, uint8 *read_buffer, uint32 len);
void        soft_spi_16bit_transfer             (soft_spi_info_struct *soft_spi_obj, const uint16 *write_buffer, uint16 *read_buffer, uint32 len);

void        soft_spi_init                       (soft_spi_info_struct *soft_spi_obj, uint8 mode, uint32 delay, gpio_pin_enum sck_pin, gpio_pin_enum mosi_pin, uint32 miso_pin, uint32 cs_pin);

#endif
