/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_driver_iic
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#ifndef _zf_driver_iic_h_
#define _zf_driver_iic_h_

#include "hal_device_registers.h"

#include "zf_common_clock.h"
#include "zf_common_debug.h"

typedef enum                                                                    // 枚举串口引脚 此枚举定义不允许用户修改
{
// IIC 1 引脚 SCL
    IIC1_SCL_A9         = 0x0409,                                               // 0x 0[IIC1] 4[AF4] 09[A9]
    IIC1_SCL_A15        = 0x040F,                                               // 0x 0[IIC1] 4[AF4] 0F[A15]
    IIC1_SCL_B6         = 0x0426,                                               // 0x 0[IIC1] 4[AF4] 26[B6]
    IIC1_SCL_B8         = 0x0428,                                               // 0x 0[IIC1] 4[AF4] 28[B8]
    IIC1_SCL_C0         = 0x0440,                                               // 0x 0[IIC1] 4[AF4] 40[C0]
    IIC1_SCL_C6         = 0x0446,                                               // 0x 0[IIC1] 4[AF4] 46[C6]
    IIC1_SCL_D10        = 0x046A,                                               // 0x 0[IIC1] 4[AF4] 6A[D10]

// IIC 2 引脚 SCL
    IIC2_SCL_B10        = 0x142A,                                               // 0x 1[IIC2] 4[AF4] 2A[B10]
    IIC2_SCL_C2         = 0x1442,                                               // 0x 1[IIC2] 4[AF4] 42[C2]
    IIC2_SCL_C8         = 0x1448,                                               // 0x 1[IIC2] 4[AF4] 48[C8]
    IIC2_SCL_E5         = 0x1485,                                               // 0x 1[IIC2] 4[AF4] 85[E5]
}iic_scl_pin_enum;

typedef enum                                                                    // 枚举串口引脚 此枚举定义不允许用户修改
{
// IIC 1 引脚 SDA
    IIC1_SDA_A10        = 0x040A,                                               // 0x 0[IIC1] 4[AF4] 0A[A10]
    IIC1_SDA_A14        = 0x040E,                                               // 0x 0[IIC1] 4[AF4] 0E[A14]
    IIC1_SDA_B7         = 0x0427,                                               // 0x 0[IIC1] 4[AF4] 27[B7]
    IIC1_SDA_B9         = 0x0429,                                               // 0x 0[IIC1] 4[AF4] 29[B9]
    IIC1_SDA_C1         = 0x0441,                                               // 0x 0[IIC1] 4[AF4] 41[C1]
    IIC1_SDA_C7         = 0x0447,                                               // 0x 0[IIC1] 4[AF4] 47[C7]
    IIC1_SDA_D11        = 0x046B,                                               // 0x 0[IIC1] 4[AF4] 6B[D11]

// IIC 2 引脚 SDA
    IIC2_SDA_B11        = 0x142B,                                               // 0x 1[IIC2] 4[AF4] 2B[B11]
    IIC2_SDA_C3         = 0x1443,                                               // 0x 1[IIC2] 4[AF4] 43[C3]
    IIC2_SDA_C9         = 0x1449,                                               // 0x 1[IIC2] 4[AF4] 49[C9]
    IIC2_SDA_E6         = 0x1486,                                               // 0x 1[IIC2] 4[AF4] 86[E6]
}iic_sda_pin_enum;

typedef enum                                                                    // 枚举 IIC 编号 此枚举定义不允许用户修改
{
    IIC_SUCCESS,
    IIC_WRITE_TIMEOUT,
    IIC_READ_TIMEOUT,
}iic_state_enum;

typedef enum                                                                    // 枚举 IIC 编号 此枚举定义不允许用户修改
{
    IIC_1,
    IIC_2,
}iic_index_enum;

void        iic_write_8bit              (iic_index_enum iic_n, uint8 target_addr, const uint8 data);
void        iic_write_8bit_array        (iic_index_enum iic_n, uint8 target_addr, const uint8 *data, uint32 len);

void        iic_write_16bit             (iic_index_enum iic_n, uint8 target_addr, const uint16 data);
void        iic_write_16bit_array       (iic_index_enum iic_n, uint8 target_addr, const uint16 *data, uint32 len);

void        iic_write_8bit_register     (iic_index_enum iic_n, uint8 target_addr, const uint8 register_name, const uint8 data);
void        iic_write_8bit_registers    (iic_index_enum iic_n, uint8 target_addr, const uint8 register_name, const uint8 *data, uint32 len);

void        iic_write_16bit_register    (iic_index_enum iic_n, uint8 target_addr, const uint16 register_name, const uint16 data);
void        iic_write_16bit_registers   (iic_index_enum iic_n, uint8 target_addr, const uint16 register_name, const uint16 *data, uint32 len);

uint8       iic_read_8bit               (iic_index_enum iic_n, uint8 target_addr);
void        iic_read_8bit_array         (iic_index_enum iic_n, uint8 target_addr, uint8 *data, uint32 len);

uint16      iic_read_16bit              (iic_index_enum iic_n, uint8 target_addr);
void        iic_read_16bit_array        (iic_index_enum iic_n, uint8 target_addr, uint16 *data, uint32 len);

uint8       iic_read_8bit_register      (iic_index_enum iic_n, uint8 target_addr, const uint8 register_name);
void        iic_read_8bit_registers     (iic_index_enum iic_n, uint8 target_addr, const uint8 register_name, uint8 *data, uint32 len);

uint16      iic_read_16bit_register     (iic_index_enum iic_n, uint8 target_addr, const uint16 register_name);
void        iic_read_16bit_registers    (iic_index_enum iic_n, uint8 target_addr, const uint16 register_name, uint16 *data, uint32 len);

void        iic_sccb_write_register     (iic_index_enum iic_n, uint8 target_addr, const uint8 register_name, uint8 data);
uint8       iic_sccb_read_register      (iic_index_enum iic_n, uint8 target_addr, const uint8 register_name);

void        iic_init                    (iic_index_enum iic_n, uint8 addr, uint32 speed, iic_scl_pin_enum scl_pin, iic_sda_pin_enum sda_pin);

#ifdef COMPATIBLE_WITH_OLDER_VERSIONS                                           // 兼容旧版本开源库接口名称

#endif

#endif
