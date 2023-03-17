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

#include "zf_driver_gpio.h"
#include "zf_driver_iic.h"

static  I2C_Type *iic_index[2]      = {I2C1, I2C2};
static  uint16 iic_speed_count[2];

//-------------------------------------------------------------------------------------------------------------------
// @brief       IIC 延时 内部调用 不加延时可能导致硬件错误 }:-)
// @param       void
// @return      void
//-------------------------------------------------------------------------------------------------------------------
static void iic_delay (iic_index_enum iic_n)
{
    volatile uint32 i = iic_speed_count[iic_n]*2;
    while(i --)
        __NOP();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IIC 接口写 8bit 数据
// @param       iic_n           IIC 模块号 参照 zf_driver_iic.h 内 iic_index_enum 枚举体定义
// @param       target_addr     目标地址
// @param       data            要写入的数据
// @return      void            
// Sample usage:                iic_write_8bit(IIC_1, 0x7A, 0x01);
//-------------------------------------------------------------------------------------------------------------------
void iic_write_8bit (iic_index_enum iic_n, uint8 target_addr, const uint8 data)
{
    iic_index[iic_n]->ENR &= 0xFFFFFFFE;                                        // IIC 关闭
    iic_index[iic_n]->TAR = target_addr;
    iic_delay(iic_n);
    iic_index[iic_n]->ENR |= 0x00000001;                                        // IIC 使能

    iic_index[iic_n]->DR = data;
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空

    iic_index[iic_n]->ENR |= 0x00000002;                                        // IIC 停止传输
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IIC 接口写 8bit 数组
// @param       iic_n           IIC 模块号 参照 zf_driver_iic.h 内 iic_index_enum 枚举体定义
// @param       target_addr     目标地址
// @param       *data           数据存放缓冲区
// @param       len             缓冲区长度
// @return      void            
// Sample usage:                iic_write_8bit_array(IIC_1, 0x7A, data, 64);
//-------------------------------------------------------------------------------------------------------------------
void iic_write_8bit_array (iic_index_enum iic_n, uint8 target_addr, const uint8 *data, uint32 len)
{
    iic_index[iic_n]->ENR &= 0xFFFFFFFE;                                        // IIC 关闭
    iic_index[iic_n]->TAR = target_addr;
    iic_delay(iic_n);
    iic_index[iic_n]->ENR |= 0x00000001;                                        // IIC 使能

    while(len --)
    {
        iic_index[iic_n]->DR = *data ++;
        while(!(iic_index[iic_n]->SR & 0x00000004));                            // 发送缓冲为空
    }

    iic_index[iic_n]->ENR |= 0x00000002;                                        // IIC 停止传输
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IIC 接口写 16bit 数据
// @param       iic_n           IIC 模块号 参照 zf_driver_iic.h 内 iic_index_enum 枚举体定义
// @param       target_addr     目标地址
// @param       data            要写入的数据
// @return      void            
// Sample usage:                iic_write_16bit(IIC_1, 0x7A,0x0101);
//-------------------------------------------------------------------------------------------------------------------
void iic_write_16bit (iic_index_enum iic_n, uint8 target_addr, const uint16 data)
{
    iic_index[iic_n]->ENR &= 0xFFFFFFFE;                                        // IIC 关闭
    iic_index[iic_n]->TAR = target_addr;
    iic_delay(iic_n);
    iic_index[iic_n]->ENR |= 0x00000001;                                        // IIC 使能

    iic_index[iic_n]->DR = (uint8)((data & 0xFF00)>>8);
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空
    iic_index[iic_n]->DR = (uint8)(data & 0x00FF);
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空

    iic_index[iic_n]->ENR |= 0x00000002;                                        // IIC 停止传输
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IIC 接口写 16bit 数组
// @param       iic_n           IIC 模块号 参照 zf_driver_iic.h 内 iic_index_enum 枚举体定义
// @param       target_addr     目标地址
// @param       *data           数据存放缓冲区
// @param       len             缓冲区长度
// @return      void            
// Sample usage:                iic_write_16bit_array(IIC_1, 0x7A, data, 6);
//-------------------------------------------------------------------------------------------------------------------
void iic_write_16bit_array (iic_index_enum iic_n, uint8 target_addr, const uint16 *data, uint32 len)
{
    iic_index[iic_n]->ENR &= 0xFFFFFFFE;                                        // IIC 关闭
    iic_index[iic_n]->TAR = target_addr;
    iic_delay(iic_n);
    iic_index[iic_n]->ENR |= 0x00000001;                                        // IIC 使能

    while(len--)
    {
        while(!(iic_index[iic_n]->SR & 0x00000004));                            // 发送缓冲为空
        iic_index[iic_n]->DR = (uint8)((*data & 0xFF00)>>8);
        while(!(iic_index[iic_n]->SR & 0x00000004));                            // 发送缓冲为空
        iic_index[iic_n]->DR = (uint8)(*data ++ & 0x00FF);
    }
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空

    iic_index[iic_n]->ENR |= 0x00000002;                                        // IIC 停止传输
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IIC 接口向传感器寄存器写 8bit 数据
// @param       iic_n           IIC 模块号 参照 zf_driver_iic.h 内 iic_index_enum 枚举体定义
// @param       target_addr     目标地址
// @param       register_name   传感器的寄存器地址
// @param       data            要写入的数据
// @return      void            
// Sample usage:                iic_write_8bit_register(IIC_1, 0x7A, 0x01, 0x01);
//-------------------------------------------------------------------------------------------------------------------
void iic_write_8bit_register (iic_index_enum iic_n, uint8 target_addr, const uint8 register_name, const uint8 data)
{
    iic_index[iic_n]->ENR &= 0xFFFFFFFE;                                        // IIC 关闭
    iic_index[iic_n]->TAR = target_addr;
    iic_delay(iic_n);
    iic_index[iic_n]->ENR |= 0x00000001;                                        // IIC 使能

    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空
    iic_index[iic_n]->DR = register_name;
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空

    iic_index[iic_n]->DR = data;
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空

    iic_index[iic_n]->ENR |= 0x00000002;                                        // IIC 停止传输
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IIC 接口向传感器寄存器写 8bit 数组
// @param       iic_n           IIC 模块号 参照 zf_driver_iic.h 内 iic_index_enum 枚举体定义
// @param       target_addr     目标地址
// @param       register_name   传感器的寄存器地址
// @param       *data           数据存放缓冲区
// @param       len             缓冲区长度
// @return      void            
// Sample usage:                iic_write_8bit_registers(IIC_1, 0x7A, 0x01, data, 6);
//-------------------------------------------------------------------------------------------------------------------
void iic_write_8bit_registers (iic_index_enum iic_n, uint8 target_addr, const uint8 register_name, const uint8 *data, uint32 len)
{
    iic_index[iic_n]->ENR &= 0xFFFFFFFE;                                        // IIC 关闭
    iic_index[iic_n]->TAR = target_addr;
    iic_delay(iic_n);
    iic_index[iic_n]->ENR |= 0x00000001;                                        // IIC 使能

    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空
    iic_index[iic_n]->DR = register_name;

    while(len --)
    {
        while(!(iic_index[iic_n]->SR & 0x00000004));                            // 发送缓冲为空
        iic_index[iic_n]->DR = *(data ++);
    }
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空

    iic_index[iic_n]->ENR |= 0x00000002;                                        // IIC 停止传输
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IIC 接口向传感器寄存器写 16bit 数据
// @param       iic_n           IIC 模块号 参照 zf_driver_iic.h 内 iic_index_enum 枚举体定义
// @param       target_addr     目标地址
// @param       register_name   传感器的寄存器地址
// @param       data            要写入的数据
// @return      void            
// Sample usage:                iic_write_16bit_register(IIC_1, 0x7A, 0x0101, 0x0101);
//-------------------------------------------------------------------------------------------------------------------
void iic_write_16bit_register (iic_index_enum iic_n, uint8 target_addr, const uint16 register_name, const uint16 data)
{
    iic_index[iic_n]->ENR &= 0xFFFFFFFE;                                        // IIC 关闭
    iic_index[iic_n]->TAR = target_addr;
    iic_delay(iic_n);
    iic_index[iic_n]->ENR |= 0x00000001;                                        // IIC 使能

    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空
    iic_index[iic_n]->DR = (uint8)((register_name & 0xFF00)>>8);
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空
    iic_index[iic_n]->DR = (uint8)(register_name & 0x00FF);
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空

    iic_index[iic_n]->DR = (uint8)((data & 0xFF00)>>8);
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空
    iic_index[iic_n]->DR = (uint8)(data & 0x00FF);
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空

    iic_index[iic_n]->ENR |= 0x00000002;                                        // IIC 停止传输
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IIC 接口向传感器寄存器写 16bit 数组
// @param       iic_n           IIC 模块号 参照 zf_driver_iic.h 内 iic_index_enum 枚举体定义
// @param       target_addr     目标地址
// @param       register_name   传感器的寄存器地址
// @param       *data           数据存放缓冲区
// @param       len             缓冲区长度
// @return      void            
// Sample usage:                iic_write_16bit_registers(IIC_1, 0x7A, 0x0101, data, 6);
//-------------------------------------------------------------------------------------------------------------------
void iic_write_16bit_registers (iic_index_enum iic_n, uint8 target_addr, const uint16 register_name, const uint16 *data, uint32 len)
{
    iic_index[iic_n]->ENR &= 0xFFFFFFFE;                                        // IIC 关闭
    iic_index[iic_n]->TAR = target_addr;
    iic_delay(iic_n);
    iic_index[iic_n]->ENR |= 0x00000001;                                        // IIC 使能

    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空
    iic_index[iic_n]->DR = (uint8)((register_name & 0xFF00)>>8);
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空
    iic_index[iic_n]->DR = (uint8)(register_name & 0x00FF);

    while(len --)
    {
        while(!(iic_index[iic_n]->SR & 0x00000004));                            // 发送缓冲为空
        iic_index[iic_n]->DR = (uint8)((*data & 0xFF00)>>8);
        while(!(iic_index[iic_n]->SR & 0x00000004));                            // 发送缓冲为空
        iic_index[iic_n]->DR = (uint8)(*data ++ & 0x00FF);
    }
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空

    iic_index[iic_n]->ENR |= 0x00000002;                                        // IIC 停止传输
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IIC 接口读取 8bit 数据
// @param       iic_n           IIC 模块号 参照 zf_driver_iic.h 内 iic_index_enum 枚举体定义
// @param       target_addr     目标地址
// @return      uint8           返回读取的 8bit 数据
// Sample usage:                iic_read_8bit(IIC_1, 0x7A);
//-------------------------------------------------------------------------------------------------------------------
uint8 iic_read_8bit (iic_index_enum iic_n, uint8 target_addr)
{
    iic_index[iic_n]->ENR &= 0xFFFFFFFE;                                        // IIC 关闭
    iic_index[iic_n]->TAR = target_addr;
    iic_delay(iic_n);
    iic_index[iic_n]->ENR |= 0x00000001;                                        // IIC 使能

    uint8 data = 0;

    iic_index[iic_n]->DR |= 0x00000100;                                         // IIC 读
    while(!(iic_index[iic_n]->SR & 0x00000008));                                // 接收缓冲不为空
    data = iic_index[iic_n]->DR;

    iic_index[iic_n]->ENR |= 0x00000002;                                        // IIC 停止传输
    return data;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IIC 接口读取 8bit 数组
// @param       iic_n           IIC 模块号 参照 zf_driver_iic.h 内 iic_index_enum 枚举体定义
// @param       target_addr     目标地址
// @param       *data           数据存放缓冲区
// @param       len             缓冲区长度
// @return      void            
// Sample usage:                iic_read_8bit_array(IIC_1, 0x7A, data, 64);
//-------------------------------------------------------------------------------------------------------------------
void iic_read_8bit_array (iic_index_enum iic_n, uint8 target_addr, uint8 *data, uint32 len)
{
    iic_index[iic_n]->ENR &= 0xFFFFFFFE;                                        // IIC 关闭
    iic_index[iic_n]->TAR = target_addr;
    iic_delay(iic_n);
    iic_index[iic_n]->ENR |= 0x00000001;                                        // IIC 使能

    while(len --)
    {
        iic_index[iic_n]->DR |= 0x00000100;                                     // IIC 读
        while(!(iic_index[iic_n]->SR & 0x00000008));                            // 接收缓冲不为空
        *data ++ = iic_index[iic_n]->DR;
    }

    iic_index[iic_n]->ENR |= 0x00000002;                                        // IIC 停止传输
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IIC 接口读取 16bit 数据
// @param       iic_n           IIC 模块号 参照 zf_driver_iic.h 内 iic_index_enum 枚举体定义
// @param       target_addr     目标地址
// @return      uint16          返回读取的 16bit 数据
// Sample usage:                iic_read_16bit(IIC_1, 0x7A);
//-------------------------------------------------------------------------------------------------------------------
uint16 iic_read_16bit (iic_index_enum iic_n, uint8 target_addr)
{
    iic_index[iic_n]->ENR &= 0xFFFFFFFE;                                        // IIC 关闭
    iic_index[iic_n]->TAR = target_addr;
    iic_delay(iic_n);
    iic_index[iic_n]->ENR |= 0x00000001;                                        // IIC 使能

    uint16 data = 0;

    iic_index[iic_n]->DR |= 0x00000100;                                         // IIC 读
    while(!(iic_index[iic_n]->SR & 0x00000008));                                // 接收缓冲不为空
    data = iic_index[iic_n]->DR;
    iic_index[iic_n]->DR |= 0x00000100;                                         // IIC 读
    while(!(iic_index[iic_n]->SR & 0x00000008));                                // 接收缓冲不为空
    data = ((data << 8)| iic_index[iic_n]->DR);

    iic_index[iic_n]->ENR |= 0x00000002;                                        // IIC 停止传输
    return data;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IIC 接口读取 16bit 数组
// @param       iic_n           IIC 模块号 参照 zf_driver_iic.h 内 iic_index_enum 枚举体定义
// @param       target_addr     目标地址
// @param       *data           数据存放缓冲区
// @param       len             缓冲区长度
// @return      void            
// Sample usage:                iic_read_16bit_array(IIC_1, 0x7A, data, 8);
//-------------------------------------------------------------------------------------------------------------------
void iic_read_16bit_array (iic_index_enum iic_n, uint8 target_addr, uint16 *data, uint32 len)
{
    iic_index[iic_n]->ENR &= 0xFFFFFFFE;                                        // IIC 关闭
    iic_index[iic_n]->TAR = target_addr;
    iic_delay(iic_n);
    iic_index[iic_n]->ENR |= 0x00000001;                                        // IIC 使能

    while(len --)
    {
        iic_index[iic_n]->DR |= 0x00000100;                                     // IIC 读
        while(!(iic_index[iic_n]->SR & 0x00000008));                            // 接收缓冲不为空
        *data = iic_index[iic_n]->DR;
        iic_index[iic_n]->DR |= 0x00000100;                                     // IIC 读
        while(!(iic_index[iic_n]->SR & 0x00000008));                            // 接收缓冲不为空
        *data = ((*data << 8)| iic_index[iic_n]->DR);
        data ++;
    }

    iic_index[iic_n]->ENR |= 0x00000002;                                        // IIC 停止传输
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IIC 接口从传感器寄存器读取 8bit 数据
// @param       iic_n           IIC 模块号 参照 zf_driver_iic.h 内 iic_index_enum 枚举体定义
// @param       target_addr     目标地址
// @param       register_name   传感器的寄存器地址
// @return      uint8           返回读取的 8bit 数据
// Sample usage:                iic_read_8bit_register(IIC_1, 0x7A, 0x01);
//-------------------------------------------------------------------------------------------------------------------
uint8 iic_read_8bit_register (iic_index_enum iic_n, uint8 target_addr, const uint8 register_name)
{
    iic_index[iic_n]->ENR &= 0xFFFFFFFE;                                        // IIC 关闭
    iic_index[iic_n]->TAR = target_addr;
    iic_delay(iic_n);
    iic_index[iic_n]->ENR |= 0x00000001;                                        // IIC 使能

    uint8 data = 0;

    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空
    iic_index[iic_n]->DR = register_name;
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空

    iic_index[iic_n]->DR |= 0x00000100;                                         // IIC 读
    while(!(iic_index[iic_n]->SR & 0x00000008));                                // 接收缓冲不为空
    data = iic_index[iic_n]->DR;

    iic_index[iic_n]->ENR |= 0x00000002;                                        // IIC 停止传输
    return data;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IIC 接口从传感器寄存器读取 8bit 数组
// @param       iic_n           IIC 模块号 参照 zf_driver_iic.h 内 iic_index_enum 枚举体定义
// @param       target_addr     目标地址
// @param       register_name   传感器的寄存器地址
// @param       *data           数据存放缓冲区
// @param       len             缓冲区长度
// @return      void            
// Sample usage:                iic_read_8bit_registers(IIC_1, 0x7A, 0x01, data, 8);
//-------------------------------------------------------------------------------------------------------------------
void iic_read_8bit_registers (iic_index_enum iic_n, uint8 target_addr, const uint8 register_name, uint8 *data, uint32 len)
{
    iic_index[iic_n]->ENR &= 0xFFFFFFFE;                                        // IIC 关闭
    iic_index[iic_n]->TAR = target_addr;
    iic_delay(iic_n);
    iic_index[iic_n]->ENR |= 0x00000001;                                        // IIC 使能

    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空
    iic_index[iic_n]->DR = register_name;
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空

    while(len --)
    {
        iic_index[iic_n]->DR |= 0x00000100;                                     // IIC 读
        while(!(iic_index[iic_n]->SR & 0x00000008));                            // 接收缓冲不为空
        *(data ++) = iic_index[iic_n]->DR;
    }

    iic_index[iic_n]->ENR |= 0x00000002;                                        // IIC 停止传输
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IIC 接口从传感器寄存器读取 16bit 数据
// @param       iic_n           IIC 模块号 参照 zf_driver_iic.h 内 iic_index_enum 枚举体定义
// @param       target_addr     目标地址
// @param       register_name   传感器的寄存器地址
// @return      uint16          返回读取的 16bit 数据
// Sample usage:                iic_read_16bit_register(IIC_1, 0x7A, 0x0101);
//-------------------------------------------------------------------------------------------------------------------
uint16 iic_read_16bit_register (iic_index_enum iic_n, uint8 target_addr, const uint16 register_name)
{
    iic_index[iic_n]->ENR &= 0xFFFFFFFE;                                        // IIC 关闭
    iic_index[iic_n]->TAR = target_addr;
    iic_delay(iic_n);
    iic_index[iic_n]->ENR |= 0x00000001;                                        // IIC 使能

    uint16 data = 0;
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空
    iic_index[iic_n]->DR = (uint8)((register_name & 0xFF00)>>8);
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空
    iic_index[iic_n]->DR = (uint8)(register_name & 0x00FF);
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空

    iic_index[iic_n]->DR |= 0x00000100;                                         // IIC 读
    while(!(iic_index[iic_n]->SR & 0x00000008));                                // 接收缓冲不为空
    data = iic_index[iic_n]->DR;
    iic_index[iic_n]->DR |= 0x00000100;                                         // IIC 读
    while(!(iic_index[iic_n]->SR & 0x00000008));                                // 接收缓冲不为空
    data = ((data << 8)| iic_index[iic_n]->DR);

    iic_index[iic_n]->ENR |= 0x00000002;                                        // IIC 停止传输
    return data;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IIC 接口从传感器寄存器读取 16bit 数组
// @param       iic_n           IIC 模块号 参照 zf_driver_iic.h 内 iic_index_enum 枚举体定义
// @param       target_addr     目标地址
// @param       register_name   传感器的寄存器地址
// @param       *data           数据存放缓冲区
// @param       len             缓冲区长度
// @return      void            
// Sample usage:                iic_read_16bit_registers(IIC_1, 0x7A, 0x0101, data, 8);
//-------------------------------------------------------------------------------------------------------------------
void iic_read_16bit_registers (iic_index_enum iic_n, uint8 target_addr, const uint16 register_name, uint16 *data, uint32 len)
{
    iic_index[iic_n]->ENR &= 0xFFFFFFFE;                                        // IIC 关闭
    iic_index[iic_n]->TAR = target_addr;
    iic_delay(iic_n);
    iic_index[iic_n]->ENR |= 0x00000001;                                        // IIC 使能

    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空
    iic_index[iic_n]->DR = (uint8)((register_name & 0xFF00)>>8);
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空
    iic_index[iic_n]->DR = (uint8)(register_name & 0x00FF);
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空

    while(len --)
    {
        iic_index[iic_n]->DR |= 0x00000100;                                     // IIC 读
        while(!(iic_index[iic_n]->SR & 0x00000008));                            // 接收缓冲不为空
        *data = iic_index[iic_n]->DR;
        iic_index[iic_n]->DR |= 0x00000100;                                     // IIC 读
        while(!(iic_index[iic_n]->SR & 0x00000008));                            // 接收缓冲不为空
        *data = ((*data << 8)| iic_index[iic_n]->DR);
        data ++;
    }

    iic_index[iic_n]->ENR |= 0x00000002;                                        // IIC 停止传输
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IIC 接口 SCCB 模式向传感器寄存器写 8bit 数据
// @param       iic_n           IIC 模块号 参照 zf_driver_iic.h 内 iic_index_enum 枚举体定义
// @param       target_addr     目标地址
// @param       register_name   传感器的寄存器地址
// @param       data            要写入的数据
// @return      void            
// Sample usage:                iic_sccb_write_register(IIC_1, 0x7A, 0x01, 0x01);
//-------------------------------------------------------------------------------------------------------------------
void iic_sccb_write_register (iic_index_enum iic_n, uint8 target_addr, const uint8 register_name, uint8 data)
{
    iic_index[iic_n]->ENR &= 0xFFFFFFFE;                                        // IIC 关闭
    iic_index[iic_n]->TAR = target_addr;
    iic_delay(iic_n);
    iic_index[iic_n]->ENR |= 0x00000001;                                        // IIC 使能

    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空
    iic_index[iic_n]->DR = register_name;
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空

    iic_index[iic_n]->DR = data;
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空

    iic_index[iic_n]->ENR |= 0x00000002;                                        // IIC 停止传输
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IIC 接口 SCCB 模式从传感器寄存器读取 8bit 数据
// @param       iic_n           IIC 模块号 参照 zf_driver_iic.h 内 iic_index_enum 枚举体定义
// @param       target_addr     目标地址
// @param       register_name   传感器的寄存器地址
// @return      uint8           返回读取的 8bit 数据
// Sample usage:                iic_sccb_read_register(IIC_1, 0x7A, 0x01);
//-------------------------------------------------------------------------------------------------------------------
uint8 iic_sccb_read_register (iic_index_enum iic_n, uint8 target_addr, const uint8 register_name)
{
    uint8 data = 0;

    iic_index[iic_n]->ENR &= 0xFFFFFFFE;                                        // IIC 关闭
    iic_index[iic_n]->TAR = target_addr;
    iic_delay(iic_n);
    iic_index[iic_n]->ENR |= 0x00000001;                                        // IIC 使能

    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空
    iic_index[iic_n]->DR = register_name;
    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空

    iic_index[iic_n]->ENR |= 0x00000002;                                        // IIC 停止传输

    iic_index[iic_n]->ENR &= 0xFFFFFFFE;                                        // IIC 关闭
    iic_index[iic_n]->TAR = target_addr;
    iic_delay(iic_n);
    iic_index[iic_n]->ENR |= 0x00000001;                                        // IIC 使能

    iic_index[iic_n]->DR |= 0x00000100;                                         // IIC 读
    while(!(iic_index[iic_n]->SR & 0x00000008));                                // 接收缓冲不为空
    data = iic_index[iic_n]->DR;

    iic_index[iic_n]->ENR |= 0x00000002;                                        // IIC 停止传输
    return data;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IIC 接口初始化 默认 MASTER 模式 不提供 SLAVE 模式
// @param       iic_n           IIC 模块号 参照 zf_driver_iic.h 内 iic_index_enum 枚举体定义
// @param       addr            IIC 地址 这里需要注意 标准七位地址 最高位忽略 写入时请务必确认无误
// @param       speed           IIC 速率 速率不超过 400Khz 不低于 1KHz 低于 1KHz 时钟的传感器赶紧扔了吧
// @param       scl_pin         IIC 时钟引脚 参照 zf_driver_iic.h 内 iic_scl_pin_enum 枚举体定义
// @param       sda_pin         IIC 数据引脚 参照 zf_driver_iic.h 内 iic_sda_pin_enum 枚举体定义
// @return      void            
// Sample usage:                iic_init(IIC_1, 0x7A, 100000, IIC1_SCL_D10, IIC1_SDA_D11);
//-------------------------------------------------------------------------------------------------------------------
void iic_init(iic_index_enum iic_n, uint8 addr, uint32 speed, iic_scl_pin_enum scl_pin, iic_sda_pin_enum sda_pin)
{
    zf_assert((speed >= 1000)&&(speed <= 400000));                              // 至少 1KHz 至多 400KHz 速率
    uint16 temp = system_clock / speed / 2;

    if(SYSTEM_CLOCK_120M < system_clock)
        temp /= 2;
    afio_init((gpio_pin_enum)(scl_pin & 0xFF), GPO, (gpio_af_enum)((scl_pin & 0xF00) >> 8), GPO_AF_PUSH_PULL); // 提取对应IO索引 AF功能编码
    afio_init((gpio_pin_enum)(sda_pin & 0xFF), GPO, (gpio_af_enum)((sda_pin & 0xF00) >> 8), GPO_AF_OPEN_DTAIN);// 提取对应IO索引 AF功能编码
    switch(iic_n)
    {
        case IIC_1:
            RCC->APB1ENR |= (0x00000001 << 21);
            RCC->APB1RSTR |= (0x00000001 << 21);
            RCC->APB1RSTR &= ~(0x00000001 << 21);
            break;
        case IIC_2:
            RCC->APB1ENR |= (0x00000001 << 22);
            RCC->APB1RSTR |= (0x00000001 << 22);
            RCC->APB1RSTR &= ~(0x00000001 << 22);
            break;
    }

    iic_index[iic_n]->ENR &= 0xFFFFFFFE;                                        // IIC 禁止

    iic_speed_count[iic_n] = temp;
    if(100000 < speed)
    {
        iic_index[iic_n]->FSHR = temp;                                          // 写入高电平持续时长
        iic_index[iic_n]->FSLR = temp;                                          // 写入低电平持续时长
        temp = (iic_index[iic_n]->CR & 0xFFFFFFF9) | 0x00000004;                // 设置高速模式             
    }
    else
    {
        iic_index[iic_n]->SSHR = temp;                                          // 写入高电平持续时长
        iic_index[iic_n]->SSLR = temp;                                          // 写入低电平持续时长
        temp = (iic_index[iic_n]->CR & 0xFFFFFFF9) | 0x00000002;                // 设置标准模式
    }

    temp &= 0xFFFFFFEF;                                                         // 七位地址格式
    temp |= 0x00000400;                                                         // 自动 RESTART 信号
    iic_index[iic_n]->CR = temp;

    iic_index[iic_n]->TAR = (iic_index[iic_n]->TAR & 0xFFFFFF00) | (uint32)addr;// 写入地址
    iic_index[iic_n]->RXTLR = 0;
    iic_index[iic_n]->TXTLR = 0;
    iic_index[iic_n]->IMR = 0x000000FFF;
    iic_index[iic_n]->ENR |= 0x00000001;                                        // IIC 使能

    while(!(iic_index[iic_n]->SR & 0x00000004));                                // 发送缓冲为空
}
