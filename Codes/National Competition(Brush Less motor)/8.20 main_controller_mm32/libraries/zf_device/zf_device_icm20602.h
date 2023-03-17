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

#ifndef _zf_device_icm20602_h_
#define _zf_device_icm20602_h_

#include "zf_common_clock.h"
#include "zf_common_debug.h"

#include "zf_driver_delay.h"
#include "zf_driver_spi.h"
#include "zf_driver_soft_iic.h"

#define ICM20602_USE_SOFT_IIC       (0)                                         // 默认使用硬件 SPI 方式驱动
#if ICM20602_USE_SOFT_IIC                                                       // 这两段 颜色正常的才是正确的 颜色灰的就是没有用的
//====================================================软件 IIC 驱动====================================================
#define ICM20602_SOFT_IIC_DELAY     (10 )                                       // 软件 IIC 的时钟延时周期 数值越小 IIC 通信速率越快
#define ICM20602_SCL_PIN            (B13)                                       // 软件 IIC SCL 引脚 连接 ICM20602 的 SCL 引脚
#define ICM20602_SDA_PIN            (B15)                                       // 软件 IIC SDA 引脚 连接 ICM20602 的 SDA 引脚
//====================================================软件 IIC 驱动====================================================
#else
//====================================================硬件 SPI 驱动====================================================
#define ICM20602_SPI_SPEED          (system_clock / 12)                         // 硬件 SPI 速率
#define ICM20602_SPI                (SPI_3)                                     // 硬件 SPI 号
#define ICM20602_SPC_PIN            (SPI3_SCK_D12)                              // 硬件 SPI SCK 引脚
#define ICM20602_SDI_PIN            (SPI3_MOSI_D14)                             // 硬件 SPI MOSI 引脚
#define ICM20602_SDO_PIN            (SPI3_MISO_D13)                             // 硬件 SPI MISO 引脚
//====================================================硬件 SPI 驱动====================================================
#endif
#define ICM20602_CS_PIN             (D15)                                       // CS 片选引脚
#define ICM20602_CS(x)              ((x) ? (gpio_high(ICM20602_CS_PIN)) : (gpio_low(ICM20602_CS_PIN)))

#define ICM20602_TIMEOUT_COUNT      (0x00FF)                                    // ICM20602 超时计数

//================================================定义 ICM20602 内部地址================================================
#define ICM20602_DEV_ADDR           (0x69)                                      // SA0接地：0x68 SA0上拉：0x69 模块默认上拉
#define ICM20602_SPI_W              (0x00)
#define ICM20602_SPI_R              (0x80)

#define ICM20602_XG_OFFS_TC_H       (0x04)
#define ICM20602_XG_OFFS_TC_L       (0x05)
#define ICM20602_YG_OFFS_TC_H       (0x07)
#define ICM20602_YG_OFFS_TC_L       (0x08)
#define ICM20602_ZG_OFFS_TC_H       (0x0A)
#define ICM20602_ZG_OFFS_TC_L       (0x0B)
#define ICM20602_SELF_TEST_X_ACCEL  (0x0D)
#define ICM20602_SELF_TEST_Y_ACCEL  (0x0E)
#define ICM20602_SELF_TEST_Z_ACCEL  (0x0F)
#define ICM20602_XG_OFFS_USRH       (0x13)
#define ICM20602_XG_OFFS_USRL       (0x14)
#define ICM20602_YG_OFFS_USRH       (0x15)
#define ICM20602_YG_OFFS_USRL       (0x16)
#define ICM20602_ZG_OFFS_USRH       (0x17)
#define ICM20602_ZG_OFFS_USRL       (0x18)
#define ICM20602_SMPLRT_DIV         (0x19)
#define ICM20602_CONFIG             (0x1A)
#define ICM20602_GYRO_CONFIG        (0x1B)
#define ICM20602_ACCEL_CONFIG       (0x1C)
#define ICM20602_ACCEL_CONFIG_2     (0x1D)
#define ICM20602_LP_MODE_CFG        (0x1E)
#define ICM20602_ACCEL_WOM_X_THR    (0x20)
#define ICM20602_ACCEL_WOM_Y_THR    (0x21)
#define ICM20602_ACCEL_WOM_Z_THR    (0x22)
#define ICM20602_FIFO_EN            (0x23)
#define ICM20602_FSYNC_INT          (0x36)
#define ICM20602_INT_PIN_CFG        (0x37)
#define ICM20602_INT_ENABLE         (0x38)
#define ICM20602_FIFO_WM_INT_STATUS (0x39)
#define ICM20602_INT_STATUS         (0x3A)
#define ICM20602_ACCEL_XOUT_H       (0x3B)
#define ICM20602_ACCEL_XOUT_L       (0x3C)
#define ICM20602_ACCEL_YOUT_H       (0x3D)
#define ICM20602_ACCEL_YOUT_L       (0x3E)
#define ICM20602_ACCEL_ZOUT_H       (0x3F)
#define ICM20602_ACCEL_ZOUT_L       (0x40)
#define ICM20602_TEMP_OUT_H         (0x41)
#define ICM20602_TEMP_OUT_L         (0x42)
#define ICM20602_GYRO_XOUT_H        (0x43)
#define ICM20602_GYRO_XOUT_L        (0x44)
#define ICM20602_GYRO_YOUT_H        (0x45)
#define ICM20602_GYRO_YOUT_L        (0x46)
#define ICM20602_GYRO_ZOUT_H        (0x47)
#define ICM20602_GYRO_ZOUT_L        (0x48)
#define ICM20602_SELF_TEST_X_GYRO   (0x50)
#define ICM20602_SELF_TEST_Y_GYRO   (0x51)
#define ICM20602_SELF_TEST_Z_GYRO   (0x52)
#define ICM20602_FIFO_WM_TH1        (0x60)
#define ICM20602_FIFO_WM_TH2        (0x61)
#define ICM20602_SIGNAL_PATH_RESET  (0x68)
#define ICM20602_ACCEL_INTEL_CTRL   (0x69)
#define ICM20602_USER_CTRL          (0x6A)
#define ICM20602_PWR_MGMT_1         (0x6B)
#define ICM20602_PWR_MGMT_2         (0x6C)
#define ICM20602_I2C_IF             (0x70)
#define ICM20602_FIFO_COUNTH        (0x72)
#define ICM20602_FIFO_COUNTL        (0x73)
#define ICM20602_FIFO_R_W           (0x74)
#define ICM20602_WHO_AM_I           (0x75)
#define ICM20602_XA_OFFSET_H        (0x77)
#define ICM20602_XA_OFFSET_L        (0x78)
#define ICM20602_YA_OFFSET_H        (0x7A)
#define ICM20602_YA_OFFSET_L        (0x7B)
#define ICM20602_ZA_OFFSET_H        (0x7D)
#define ICM20602_ZA_OFFSET_L        (0x7E)
//================================================定义 ICM20602 内部地址================================================

extern int16 icm_gyro_x, icm_gyro_y, icm_gyro_z;                                // 三轴陀螺仪数据
extern int16 icm_acc_x, icm_acc_y,   icm_acc_z;                                   // 三轴加速度计数据

void    icm20602_get_acc            (void);
void    icm20602_get_gyro           (void);
uint8   icm20602_init               (void);

#endif

