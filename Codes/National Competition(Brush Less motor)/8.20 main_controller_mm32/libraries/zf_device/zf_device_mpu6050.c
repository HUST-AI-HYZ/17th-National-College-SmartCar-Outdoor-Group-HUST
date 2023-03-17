/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_mpu6050
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
*                   软件 IIC 通信引脚对应关系
*                   SCL                 查看 zf_device_mpu6050.h 中 MPU6050_SOFT_IIC_SCL 宏定义
*                   SDA                 查看 zf_device_mpu6050.h 中 MPU6050_SOFT_IIC_SDA 宏定义
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   其余引脚悬空
*
*                   硬件 IIC 通信引脚对应关系
*                   SCL                 查看 zf_device_mpu6050.h 中 MPU6050_IIC_SCL 宏定义
*                   SDA                 查看 zf_device_mpu6050.h 中 MPU6050_IIC_SDA 宏定义
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   其余引脚悬空
*                   ------------------------------------
********************************************************************************************************************/

#include "zf_device_mpu6050.h"

int16 mpu_gyro_x = 0, mpu_gyro_y = 0, mpu_gyro_z = 0;                           // 三轴陀螺仪数据      gyro (陀螺仪)
int16 mpu_acc_x = 0, mpu_acc_y = 0, mpu_acc_z = 0;                              // 三轴加速度计数据    acc (accelerometer 加速度计)

#if MPU6050_USE_SOFT_IIC
static soft_iic_info_struct mpu6050_iic_struct;

#define mpu6050_write_register(reg, data)       (soft_iic_write_8bit_register(&mpu6050_iic_struct, (reg), (data)))
#define mpu6050_read_register(reg)              (soft_iic_read_8bit_register(&mpu6050_iic_struct, (reg)))
#define mpu6050_read_registers(reg, data, len)  (soft_iic_read_8bit_registers(&mpu6050_iic_struct, (reg), (data), (len)))
#else
#define mpu6050_write_register(reg, data)       (iic_write_8bit_register(MPU6050_IIC, MPU6050_DEV_ADDR, (reg), (data)))
#define mpu6050_read_register(reg)              (iic_read_8bit_register(MPU6050_IIC, MPU6050_DEV_ADDR, (reg)))
#define mpu6050_read_registers(reg, data, len)  (iic_read_8bit_registers(MPU6050_IIC, MPU6050_DEV_ADDR, (reg), (data), (len)))
#endif

//-------------------------------------------------------------------------------------------------------------------
// @brief		MPU6050 自检 内部调用
// @param		void
// @return		uint8           1-自检失败 0-自检成功
//-------------------------------------------------------------------------------------------------------------------
static uint8 mpu6050_self1_check (void)
{
    uint8 dat = 0, return_state = 0;
    uint16 timeout_count = 0;

    mpu6050_write_register(PWR_MGMT_1, 0x00);                                   // 解除休眠状态
    mpu6050_write_register(SMPLRT_DIV, 0x07);                                   // 125HZ采样率
    while(0x07 != dat)
    {
        if(timeout_count ++ > MPU6050_TIMEOUT_COUNT)
        {
            return_state =  1;
            break;
        }
        dat = mpu6050_read_register(SMPLRT_DIV);
        system_delay_ms(10);
    }
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       获取MPU6050加速度计数据
// @param       void
// @return      void
// Sample usage:                mpu6050_get_acc();                              // 执行该函数后，直接查看对应的变量即可
//-------------------------------------------------------------------------------------------------------------------
void mpu6050_get_acc (void)
{
    uint8 dat[6];

    mpu6050_read_registers(ACCEL_XOUT_H, dat, 6);  
    mpu_acc_x = (int16)(((uint16)dat[0] << 8 | dat[1]));
    mpu_acc_y = (int16)(((uint16)dat[2] << 8 | dat[3]));
    mpu_acc_z = (int16)(((uint16)dat[4] << 8 | dat[5]));
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       获取MPU6050陀螺仪数据
// @param       void
// @return      void
// Sample usage:                mpu6050_get_gyro();                             // 执行该函数后，直接查看对应的变量即可
//-------------------------------------------------------------------------------------------------------------------
void mpu6050_get_gyro (void)
{
    uint8 dat[6];

    mpu6050_read_registers(GYRO_XOUT_H, dat, 6);  
    mpu_gyro_x = (int16)(((uint16)dat[0] << 8 | dat[1]));
    mpu_gyro_y = (int16)(((uint16)dat[2] << 8 | dat[3]));
    mpu_gyro_z = (int16)(((uint16)dat[4] << 8 | dat[5]));
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		初始化 MPU6050
// @param		void
// @return		uint8           1-初始化失败 0-初始化成功
// Sample usage:                mpu6050_init();
//-------------------------------------------------------------------------------------------------------------------
uint8 mpu6050_init (void)
{
    uint8 return_state = 0;
#if MPU6050_USE_SOFT_IIC
    soft_iic_init(&mpu6050_iic_struct, MPU6050_DEV_ADDR, MPU6050_SOFT_IIC_DELAY, MPU6050_SCL_PIN, MPU6050_SDA_PIN);
#else
    iic_init(MPU6050_IIC, MPU6050_DEV_ADDR, MPU6050_IIC_SPEED, MPU6050_SCL_PIN, MPU6050_SDA_PIN);
#endif
    system_delay_ms(100);                                                       // 上电延时

    do{
        if(mpu6050_self1_check())
        {
            // 如果程序在输出了断言信息 并且提示出错位置在这里
            // 那么就是 MPU6050 自检出错并超时退出了
            // 检查一下接线有没有问题 如果没问题可能就是坏了
            zf_assert(0);
            return_state = 1;
            break;
        }
        mpu6050_write_register(PWR_MGMT_1, 0x00);                               // 解除休眠状态
        mpu6050_write_register(SMPLRT_DIV, 0x07);                               // 125HZ采样率
        mpu6050_write_register(MPU6050_CONFIG, 0x04);
        mpu6050_write_register(GYRO_CONFIG, 0x18);                              // 2000
        mpu6050_write_register(ACCEL_CONFIG, 0x10);                             // 8g
        mpu6050_write_register(USER_CONTROL, 0x00);
        mpu6050_write_register(INT_PIN_CFG, 0x02);
    }while(0);
    return return_state;
}
