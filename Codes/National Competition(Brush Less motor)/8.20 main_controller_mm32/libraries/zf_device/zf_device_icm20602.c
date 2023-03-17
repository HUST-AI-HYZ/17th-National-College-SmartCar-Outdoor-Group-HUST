/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_icm20602
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
*                   SCL/SPC             查看 zf_device_icm20602.h 中 ICM20602_SPC_PIN 宏定义
*                   SDA/DSI             查看 zf_device_icm20602.h 中 ICM20602_SDI_PIN 宏定义
*                   SA0/SDO             查看 zf_device_icm20602.h 中 ICM20602_SDO_PIN 宏定义
*                   CS                  查看 zf_device_icm20602.h 中 ICM20602_CS_PIN 宏定义
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   其余引脚悬空
*
*                   // 软件 IIC 引脚
*                   SCL/SPC             查看 zf_device_icm20602.h 中 ICM20602_SCL_PIN 宏定义
*                   SDA/DSI             查看 zf_device_icm20602.h 中 ICM20602_SDA_PIN 宏定义
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   其余引脚悬空
*                   ------------------------------------
********************************************************************************************************************/

#include "zf_device_icm20602.h"

int16 icm_gyro_x = 0, icm_gyro_y = 0, icm_gyro_z = 0;                           // 三轴陀螺仪数据      gyro (陀螺仪)
int16 icm_acc_x = 0, icm_acc_y = 0, icm_acc_z = 0;                              // 三轴加速度计数据    acc (accelerometer 加速度计)

#if ICM20602_USE_SOFT_IIC
static soft_iic_info_struct icm20602_iic_struct;

#define icm20602_write_register(reg, data)      (soft_iic_write_8bit_register(&icm20602_iic_struct, (reg), (data)))
#define icm20602_read_register(reg)             (soft_iic_read_8bit_register(&icm20602_iic_struct, (reg)))
#define icm20602_read_registers(reg, data, len) (soft_iic_read_8bit_registers(&icm20602_iic_struct, (reg), (data), (len)))
#else
//-------------------------------------------------------------------------------------------------------------------
// @brief       ICM20602 写寄存器 内部调用
// @param       reg             寄存器地址
// @param       data            数据
// @return      void
//-------------------------------------------------------------------------------------------------------------------
static void icm20602_write_register(uint8 reg, uint8 data)
{
    ICM20602_CS(0);
    spi_write_8bit_register(ICM20602_SPI, reg | ICM20602_SPI_W, data);
    ICM20602_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ICM20602 读寄存器 内部调用
// @param       reg             寄存器地址
// @return      uint8           数据
//-------------------------------------------------------------------------------------------------------------------
static uint8 icm20602_read_register(uint8 reg)
{
    uint8 data = 0;
    ICM20602_CS(0);
    data = spi_read_8bit_register(ICM20602_SPI, reg | ICM20602_SPI_R);
    ICM20602_CS(1);
    return data;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ICM20602 读数据 内部调用
// @param       reg             寄存器地址
// @param       data            数据缓冲区
// @param       len             数据长度
// @return      void
//-------------------------------------------------------------------------------------------------------------------
static void icm20602_read_registers(uint8 reg, uint8 *data, uint32 len)
{
    ICM20602_CS(0);
    spi_read_8bit_registers(ICM20602_SPI, reg | ICM20602_SPI_R, data, len);
    ICM20602_CS(1);
}
#endif

//-------------------------------------------------------------------------------------------------------------------
// @brief       ICM20602 自检 内部调用
// @param       void
// @return      uint8           1-自检失败 0-自检成功
//-------------------------------------------------------------------------------------------------------------------
static uint8 icm20602_self_check (void)
{
    uint8 dat = 0, return_state = 0;
    uint16 timeout_count = 0;

    while(0x12 != dat)                                                          // 判断 ID 是否正确
    {
        if(timeout_count ++ > ICM20602_TIMEOUT_COUNT)
        {
            return_state =  1;
            break;
        }
        dat = icm20602_read_register(ICM20602_WHO_AM_I);
        system_delay_ms(10);
    }
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       获取 ICM20602 加速度计数据
// @param       void
// @return      void
// Sample usage:                icm20602_get_acc();                             // 执行该函数后，直接查看对应的变量即可
//-------------------------------------------------------------------------------------------------------------------
void icm20602_get_acc (void)
{
    uint8 dat[6];

    icm20602_read_registers(ICM20602_ACCEL_XOUT_H, dat, 6);
    icm_acc_x = (int16)(((uint16)dat[0] << 8 | dat[1]));
    icm_acc_y = (int16)(((uint16)dat[2] << 8 | dat[3]));
    icm_acc_z = (int16)(((uint16)dat[4] << 8 | dat[5]));
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       获取ICM20602陀螺仪数据
// @param       void
// @return      void
// Sample usage:                icm20602_get_gyro();                            // 执行该函数后，直接查看对应的变量即可
//-------------------------------------------------------------------------------------------------------------------
void icm20602_get_gyro (void)
{
    uint8 dat[6];

    icm20602_read_registers(ICM20602_GYRO_XOUT_H, dat, 6);
    icm_gyro_x = (int16)(((uint16)dat[0] << 8 | dat[1]));
    icm_gyro_y = (int16)(((uint16)dat[2] << 8 | dat[3]));
    icm_gyro_z = (int16)(((uint16)dat[4] << 8 | dat[5]));
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       初始化 ICM20602
// @param       void
// @return      uint8           1-初始化失败 0-初始化成功
// Sample usage:                icm20602_init();
//-------------------------------------------------------------------------------------------------------------------
uint8 icm20602_init (void)
{
    uint8 val = 0x0, return_state = 0;
    uint16 timeout_count = 0;

    system_delay_ms(10);                                                        // 上电延时

#if ICM20602_USE_SOFT_IIC
    soft_iic_init(&icm20602_iic_struct, ICM20602_DEV_ADDR, ICM20602_SOFT_IIC_DELAY, ICM20602_SCL_PIN, ICM20602_SDA_PIN);
#else
    spi_init(ICM20602_SPI, SPI_MODE0, ICM20602_SPI_SPEED, ICM20602_SPC_PIN, ICM20602_SDI_PIN, ICM20602_SDO_PIN, SPI_CS_NULL);
    gpio_init(ICM20602_CS_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
#endif

    do{
        if(icm20602_self_check())
        {
            // 如果程序在输出了断言信息 并且提示出错位置在这里
            // 那么就是 ICM20602 自检出错并超时退出了
            // 检查一下接线有没有问题 如果没问题可能就是坏了
            zf_assert(0);
            return_state = 1;
            break;
        }

        icm20602_write_register(ICM20602_PWR_MGMT_1, 0x80);                     // 复位设备
        system_delay_ms(2);

        do{                                                                     // 等待复位成功
            val = icm20602_read_register(ICM20602_PWR_MGMT_1);
            if(timeout_count ++ > ICM20602_TIMEOUT_COUNT)
            {
                // 如果程序在输出了断言信息 并且提示出错位置在这里
                // 那么就是 ICM20602 自检出错并超时退出了
                // 检查一下接线有没有问题 如果没问题可能就是坏了
                zf_assert(0);
                return_state = 1;
                break;
            }
        }while(0x41 != val);
        if(1 == return_state)
            break;

        icm20602_write_register(ICM20602_PWR_MGMT_1,     0x01);                 // 时钟设置
        icm20602_write_register(ICM20602_PWR_MGMT_2,     0x00);                 // 开启陀螺仪和加速度计
        icm20602_write_register(ICM20602_CONFIG,         0x01);                 // 176HZ 1KHZ
        icm20602_write_register(ICM20602_SMPLRT_DIV,     0x07);                 // 采样速率 SAMPLE_RATE = INTERNAL_SAMPLE_RATE / (1 + SMPLRT_DIV)
        icm20602_write_register(ICM20602_GYRO_CONFIG,    0x18);                 // ±2000 dps
        icm20602_write_register(ICM20602_ACCEL_CONFIG,   0x10);                 // ±8g
        icm20602_write_register(ICM20602_ACCEL_CONFIG_2, 0x03);                 // Average 4 samples   44.8HZ   //0x23 Average 16 samples
        // ICM20602_GYRO_CONFIG寄存器
        // 设置为:0x00 陀螺仪量程为:±250 dps     获取到的陀螺仪数据除以131           可以转化为带物理单位的数据，单位为：°/s
        // 设置为:0x08 陀螺仪量程为:±500 dps     获取到的陀螺仪数据除以65.5          可以转化为带物理单位的数据，单位为：°/s
        // 设置为:0x10 陀螺仪量程为:±1000dps     获取到的陀螺仪数据除以32.8          可以转化为带物理单位的数据，单位为：°/s
        // 设置为:0x18 陀螺仪量程为:±2000dps     获取到的陀螺仪数据除以16.4          可以转化为带物理单位的数据，单位为：°/s

        // ICM20602_ACCEL_CONFIG寄存器
        // 设置为:0x00 加速度计量程为:±2g          获取到的加速度计数据 除以16384      可以转化为带物理单位的数据，单位：g(m/s^2)
        // 设置为:0x08 加速度计量程为:±4g          获取到的加速度计数据 除以8192       可以转化为带物理单位的数据，单位：g(m/s^2)
        // 设置为:0x10 加速度计量程为:±8g          获取到的加速度计数据 除以4096       可以转化为带物理单位的数据，单位：g(m/s^2)
        // 设置为:0x18 加速度计量程为:±16g         获取到的加速度计数据 除以2048       可以转化为带物理单位的数据，单位：g(m/s^2)
    }while(0);
    return return_state;
}
