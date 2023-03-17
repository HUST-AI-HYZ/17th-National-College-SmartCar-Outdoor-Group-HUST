/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_driver_spi
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#ifndef _zf_driver_spi_h_
#define _zf_driver_spi_h_

#include "hal_device_registers.h"

#include "zf_common_clock.h"
#include "zf_common_debug.h"

#define SPI_SPEED_PRIORITY                                                      // SPI 通信速度优先

typedef enum                                                                    // 枚举串口引脚 此枚举定义不允许用户修改
{
    // SPI1 引脚 SCK
    SPI1_SCK_A5         = 0x0505,                                               // 0x 0[SPI1] 5[AF5] 05[A5]
    SPI1_SCK_B3         = 0x0523,                                               // 0x 0[SPI1] 5[AF5] 23[B3]
    SPI1_SCK_E12        = 0x058C,                                               // 0x 0[SPI1] 5[AF5] 8C[E12]

    // SPI2 引脚 SCK
    SPI2_SCK_B10        = 0x152A,                                               // 0x 1[SPI2] 5[AF5] 2A[B10]
    SPI2_SCK_B13        = 0x152D,                                               // 0x 1[SPI2] 5[AF5] 2D[B13]
    SPI2_SCK_D3         = 0x1563,                                               // 0x 1[SPI2] 5[AF5] 63[D3]
    SPI2_SCK_E2         = 0x1582,                                               // 0x 1[SPI2] 5[AF5] 82[E2]

    // SPI3 引脚 SCK
    SPI3_SCK_B3         = 0x2623,                                               // 0x 2[SPI3] 6[AF6] 23[B3]
    SPI3_SCK_C10        = 0x264A,                                               // 0x 2[SPI3] 6[AF6] 4A[C10]
    SPI3_SCK_D4         = 0x2564,                                               // 0x 2[SPI3] 5[AF5] 64[D4]
    SPI3_SCK_D12        = 0x266C,                                               // 0x 2[SPI3] 6[AF6] 6C[D12]
}spi_sck_pin_enum;

typedef enum                                                                    // 枚举串口引脚 此枚举定义不允许用户修改
{
    // SPI1 引脚 MOSI
    SPI1_MOSI_A7        = 0x0507,                                               // 0x 0[SPI1] 5[AF5] 07[A7]
    SPI1_MOSI_B5        = 0x0525,                                               // 0x 0[SPI1] 5[AF5] 25[B5]
    SPI1_MOSI_E14       = 0x058E,                                               // 0x 0[SPI1] 5[AF5] 8E[E14]

    // SPI2 引脚 MOSI
    SPI2_MOSI_B15       = 0x152F,                                               // 0x 1[SPI2] 5[AF5] 2F[B15]
    SPI2_MOSI_C3        = 0x1543,                                               // 0x 1[SPI2] 5[AF5] 43[C3]
    SPI2_MOSI_E6        = 0x1586,                                               // 0x 1[SPI2] 5[AF5] 86[E6]

    // SPI3 引脚 MOSI
    SPI3_MOSI_B5        = 0x2625,                                               // 0x 2[SPI3] 6[AF6] 25[B5]
    SPI3_MOSI_C12       = 0x264C,                                               // 0x 2[SPI3] 6[AF6] 4C[C12]
    SPI3_MOSI_D6        = 0x2566,                                               // 0x 2[SPI3] 5[AF5] 66[D6]
    SPI3_MOSI_D14       = 0x266E,                                               // 0x 2[SPI3] 6[AF6] 6E[D14]
}spi_mosi_pin_enum;

typedef enum                                                                    // 枚举串口引脚 此枚举定义不允许用户修改
{
    SPI_MISO_NULL       = 0xFFFF,                                               // 不使用硬件 MISO 引脚就填这个

    // SPI1 引脚 MISO
    SPI1_MISO_A6        = 0x0506,                                               // 0x 0[SPI1] 5[AF5] 06[A6]
    SPI1_MISO_B4        = 0x0524,                                               // 0x 0[SPI1] 5[AF5] 24[B4]
    SPI1_MISO_E13       = 0x058D,                                               // 0x 0[SPI1] 5[AF5] 8D[E13]

    // SPI2 引脚 MISO
    SPI2_MISO_B14       = 0x152E,                                               // 0x 1[SPI2] 5[AF5] 2E[B14]
    SPI2_MISO_C2        = 0x1542,                                               // 0x 1[SPI2] 5[AF5] 42[C2]
    SPI2_MISO_C6        = 0x1546,                                               // 0x 1[SPI2] 5[AF5] 46[C6]
    SPI2_MISO_E5        = 0x1585,                                               // 0x 1[SPI2] 5[AF5] 85[E5]

    // SPI3 引脚 MISO
    SPI3_MISO_B4        = 0x2624,                                               // 0x 2[SPI3] 6[AF6] 24[B4]
    SPI3_MISO_C7        = 0x2547,                                               // 0x 2[SPI3] 5[AF5] 47[C7]
    SPI3_MISO_C11       = 0x264B,                                               // 0x 2[SPI3] 6[AF6] 4B[C11]
    SPI3_MISO_D5        = 0x2565,                                               // 0x 2[SPI3] 5[AF5] 65[D5]
    SPI3_MISO_D13       = 0x266D,                                               // 0x 2[SPI3] 6[AF6] 6D[D13]
}spi_miso_pin_enum;

typedef enum                                                                    // 枚举串口引脚 此枚举定义不允许用户修改
{
    SPI_CS_NULL       = 0xFFFF,                                                 // 不使用硬件 CS 引脚就填这个

    // SPI1 引脚 CS
    SPI1_CS_A4         = 0x0504,                                                // 0x 0[SPI1] 5[AF5] 04[A4]
    SPI1_CS_A15        = 0x050F,                                                // 0x 0[SPI1] 5[AF5] 0F[A15]
    SPI1_CS_E11        = 0x058B,                                                // 0x 0[SPI1] 5[AF5] 8B[E11]

    // SPI2 引脚 CS
    SPI2_CS_B9         = 0x1529,                                                // 0x 1[SPI2] 5[AF5] 29[B9]
    SPI2_CS_B12        = 0x152C,                                                // 0x 1[SPI2] 5[AF5] 2C[B12]
    SPI2_CS_E3         = 0x1583,                                                // 0x 1[SPI2] 5[AF5] 83[E3]
    SPI2_CS_E4         = 0x1584,                                                // 0x 1[SPI2] 5[AF5] 84[E4]

    // SPI3 引脚 CS
    SPI3_CS_A4         = 0x2604,                                                // 0x 2[SPI3] 6[AF6] 04[A4]
    SPI3_CS_A15        = 0x260F,                                                // 0x 2[SPI3] 6[AF6] 0F[A15]
    SPI3_CS_D7         = 0x2567,                                                // 0x 2[SPI3] 5[AF5] 67[D7]
    SPI3_CS_D15        = 0x266F,                                                // 0x 2[SPI3] 6[AF6] 6F[D15]
}spi_cs_pin_enum;

typedef enum                                                                    // 枚举 SPI 模式 此枚举定义不允许用户修改
{
    SPI_MODE0,
    SPI_MODE1,
    SPI_MODE2,
    SPI_MODE3,
}spi_mode_enum;

typedef enum                                                                    // 枚举 SPI 编号 此枚举定义不允许用户修改
{
    SPI_1,
    SPI_2,
    SPI_3,
}spi_index_enum;

void        spi_write_8bit                  (spi_index_enum spi_n, const uint8 data);
void        spi_write_8bit_array            (spi_index_enum spi_n, const uint8 *data, uint32 len);

void        spi_write_16bit                 (spi_index_enum spi_n, const uint16 data);
void        spi_write_16bit_array           (spi_index_enum spi_n, const uint16 *data, uint32 len);

void        spi_write_8bit_register         (spi_index_enum spi_n, const uint8 register_name, const uint8 data);
void        spi_write_8bit_registers        (spi_index_enum spi_n, const uint8 register_name, const uint8 *data, uint32 len);

void        spi_write_16bit_register        (spi_index_enum spi_n, const uint16 register_name, const uint16 data);
void        spi_write_16bit_registers       (spi_index_enum spi_n, const uint16 register_name, const uint16 *data, uint32 len);

uint8       spi_read_8bit                   (spi_index_enum spi_n);
void        spi_read_8bit_array             (spi_index_enum spi_n, uint8 *data, uint32 len);

uint16      spi_read_16bit                  (spi_index_enum spi_n);
void        spi_read_16bit_array            (spi_index_enum spi_n, uint16 *data, uint32 len);

uint8       spi_read_8bit_register          (spi_index_enum spi_n, const uint8 register_name);
void        spi_read_8bit_registers         (spi_index_enum spi_n, const uint8 register_name, uint8 *data, uint32 len);

uint16      spi_read_16bit_register         (spi_index_enum spi_n, const uint16 register_name);
void        spi_read_16bit_registers        (spi_index_enum spi_n, const uint16 register_name, uint16 *data, uint32 len);

void        spi_transfer_8bit               (spi_index_enum spi_n, const uint8 *write_buffer, uint8 *read_buffer, uint32 len);
void        spi_transfer_16bit              (spi_index_enum spi_n, const uint16 *write_buffer, uint16 *read_buffer, uint32 len);

void        spi_init                        (spi_index_enum spi_n, spi_mode_enum mode, uint32 baud, spi_sck_pin_enum sck_pin, spi_mosi_pin_enum mosi_pin, spi_miso_pin_enum miso_pin, spi_cs_pin_enum cs_pin);

#ifdef COMPATIBLE_WITH_OLDER_VERSIONS                                           // 兼容旧版本开源库接口名称
#define spi_mosi(spi_n, mosi, miso, len)    (spi_transfer_8bit((spi_n), (mosi), (miso), (len)))
#endif

#endif
