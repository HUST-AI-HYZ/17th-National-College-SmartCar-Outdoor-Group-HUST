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

#include "zf_driver_gpio.h"
#include "zf_driver_spi.h"

static SPI_Type *spi_index[3] = {SPI1, SPI2, SPI3};

#define SPI_CTRL_CS(x, e)           ((e) ? (spi_index[x]->NSSR &= 0xFFFFFFFE) : (spi_index[x]->NSSR |= 0x00000001)) // SPI 片选 ENABLE/DISABLE 控制片选 拉低/拉高
#define SPI_CTRL_RX(x, e)           ((e) ? (spi_index[x]->GCTL |= 0x00000010) : (spi_index[x]->GCTL &= 0xFFFFFFEF)) // SPI RX 使能 ENABLE/DISABLE

#define SPI_DATA_RX(x)              (spi_index[x]->RXREG)                       // SPI 读数据
#define SPI_DATA_TX(x, data)        (spi_index[x]->TXREG = (data))              // SPI 写数据

#define SPI_STATE_BUY(x)            (spi_index[x]->CSTAT & (0x00001000))        // SPI 忙 正在传输中
#define SPI_STATE_RX_FIFO_COUNT(x)  ((spi_index[x]->CSTAT & (0x00000F00)) >> 8) // SPI 接收缓冲器内数据个数
#define SPI_STATE_TX_FIFO_COUNT(x)  ((spi_index[x]->CSTAT & (0x000000F0)) >> 4) // SPI 发送缓冲器内数据个数
#define SPI_STATE_RX_FIFO_4(x)      (spi_index[x]->CSTAT & (0x00000008))        // SPI 接收缓冲器内数据个数超过四个了
#define SPI_STATE_TX_FIFO_FULL(x)   (spi_index[x]->CSTAT & (0x00000004))        // SPI 发送缓冲器满了
#define SPI_STATE_RX_AVAILABLE(x)   (spi_index[x]->CSTAT & (0x00000002))        // SPI 接收缓冲器收到一个有效数据
#define SPI_STATE_TX_EMPTY(x)       (spi_index[x]->CSTAT & (0x00000001))        // SPI 发送缓冲器空了

#ifndef SPI_SPEED_PRIORITY
//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 8bit 数据读写 内部调用
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       data            数据
// @return      uint8           读取的数据
//-------------------------------------------------------------------------------------------------------------------
static uint8 spi_8bit_data_handler (spi_index_enum spi_n, const uint8 data)
{
    uint8 read_data = 0;

    SPI_CTRL_RX(spi_n, ENABLE);
    SPI_CTRL_CS(spi_n, ENABLE);

    SPI_DATA_TX(spi_n, data);                                                   // 发送数据
    while(!SPI_STATE_RX_AVAILABLE(spi_n));                                      // 接收到有效数据
    read_data = SPI_DATA_RX(spi_n);                                             // 读取数据

    SPI_CTRL_CS(spi_n, DISABLE);
    SPI_CTRL_RX(spi_n, DISABLE);

    return read_data;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       软件 SPI 16bit 数据读写 内部调用
// @param       *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// @param       data            数据
// @return      uint16          读取的数据
//-------------------------------------------------------------------------------------------------------------------
static uint16 spi_16bit_data_handler (spi_index_enum spi_n, const uint16 data)
{
    uint16 read_data = 0;
 
    SPI_CTRL_RX(spi_n, ENABLE);
    SPI_CTRL_CS(spi_n, ENABLE);

    SPI_DATA_TX(spi_n, (uint8)((data & 0xFF00) >> 8));                          // 发送数据
    while(!SPI_STATE_RX_AVAILABLE(spi_n));                                      // 接收到有效数据
    read_data = SPI_DATA_RX(spi_n);                                             // 读取数据

    SPI_DATA_TX(spi_n, (uint8)(data & 0x00FF));                                 // 发送数据
    while(!SPI_STATE_RX_AVAILABLE(spi_n));                                      // 接收到有效数据
    read_data = ((read_data << 8) | SPI_DATA_RX(spi_n));                        // 读取数据

    SPI_CTRL_CS(spi_n, DISABLE);
    SPI_CTRL_RX(spi_n, DISABLE);

    return read_data;
}
#endif

//-------------------------------------------------------------------------------------------------------------------
// @brief       SPI 接口写 8bit 数据
// @param       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// @param       data            数据
// @return      void
// Sample usage:                spi_write_8bit(SPI_1, 0x11);
//-------------------------------------------------------------------------------------------------------------------
void spi_write_8bit (spi_index_enum spi_n, const uint8 data)
{
#ifdef SPI_SPEED_PRIORITY
    SPI_CTRL_CS(spi_n, ENABLE);

    SPI_DATA_TX(spi_n, data);                                                   // 发送数据
    while(!SPI_STATE_TX_EMPTY(spi_n));                                          // 发送为空

    while(SPI_STATE_BUY(spi_n));                                                // 等待传输完毕
    SPI_CTRL_CS(spi_n, DISABLE);
#else
    spi_8bit_data_handler(spi_n, data);
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       SPI 接口写 8bit 数组
// @param       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// @param       *data           数据存放缓冲区
// @param       len             缓冲区长度
// @return      void
// Sample usage:                spi_write_8bit_array(SPI_1, data, 64);
//-------------------------------------------------------------------------------------------------------------------
void spi_write_8bit_array (spi_index_enum spi_n, const uint8 *data, uint32 len)
{
#ifdef SPI_SPEED_PRIORITY
    SPI_CTRL_CS(spi_n, ENABLE);

    while(len --)
    {
        SPI_DATA_TX(spi_n, *data ++);                                           // 发送数据
        while(!SPI_STATE_TX_EMPTY(spi_n));                                      // 发送为空
    }

    while(SPI_STATE_BUY(spi_n));                                                // 等待传输完毕
    SPI_CTRL_CS(spi_n, DISABLE);
#else
    while(len --)
        spi_8bit_data_handler(spi_n, *data ++);
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       SPI 接口写 16bit 数据
// @param       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// @param       data            数据
// @return      void
// Sample usage:                spi_write_16bit(SPI_1, 0x1101);
//-------------------------------------------------------------------------------------------------------------------
void spi_write_16bit (spi_index_enum spi_n, const uint16 data)
{
#ifdef SPI_SPEED_PRIORITY
    SPI_CTRL_CS(spi_n, ENABLE);

    SPI_DATA_TX(spi_n, (uint8)((data & 0xFF00) >> 8));                          // 发送数据
    while(!SPI_STATE_TX_EMPTY(spi_n));                                          // 发送为空

    SPI_DATA_TX(spi_n, (uint8)(data & 0x00FF));                                 // 发送数据
    while(!SPI_STATE_TX_EMPTY(spi_n));                                          // 发送为空

    while(SPI_STATE_BUY(spi_n));                                                // 等待传输完毕
    SPI_CTRL_CS(spi_n, DISABLE);
#else
    spi_16bit_data_handler(spi_n, data);
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       SPI 接口写 16bit 数组
// @param       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// @param       *data           数据存放缓冲区
// @param       len             缓冲区长度
// @return      void
// Sample usage:                spi_write_16bit_array(SPI_1, data, 64);
//-------------------------------------------------------------------------------------------------------------------
void spi_write_16bit_array (spi_index_enum spi_n, const uint16 *data, uint32 len)
{
#ifdef SPI_SPEED_PRIORITY
    SPI_CTRL_CS(spi_n, ENABLE);

    while(len --)
    {
        SPI_DATA_TX(spi_n, (uint8)((*data & 0xFF00) >> 8));                     // 发送数据
        while(!SPI_STATE_TX_EMPTY(spi_n));                                      // 发送为空

        SPI_DATA_TX(spi_n, (uint8)(*data ++ & 0x00FF));                         // 发送数据
        while(!SPI_STATE_TX_EMPTY(spi_n));                                      // 发送为空
    }

    while(SPI_STATE_BUY(spi_n));                                                // 等待传输完毕
    SPI_CTRL_CS(spi_n, DISABLE);
#else
    while(len --)
        spi_16bit_data_handler(spi_n, *data ++);
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       SPI 接口向传感器的寄存器写 8bit 数据
// @param       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// @param       register_name   寄存器地址
// @param       data            数据
// @return      void
// Sample usage:                spi_write_8bit_register(SPI_1, 0x11, 0x01);
//-------------------------------------------------------------------------------------------------------------------
void spi_write_8bit_register (spi_index_enum spi_n, const uint8 register_name, const uint8 data)
{
#ifdef SPI_SPEED_PRIORITY
    SPI_CTRL_CS(spi_n, ENABLE);

    SPI_DATA_TX(spi_n, register_name);                                          // 发送数据
    while(!SPI_STATE_TX_EMPTY(spi_n));                                          // 发送为空

    SPI_DATA_TX(spi_n, data);                                                   // 发送数据
    while(!SPI_STATE_TX_EMPTY(spi_n));                                          // 发送为空

    while(SPI_STATE_BUY(spi_n));                                                // 等待传输完毕
    SPI_CTRL_CS(spi_n, DISABLE);
#else
    spi_8bit_data_handler(spi_n, register_name);
    spi_8bit_data_handler(spi_n, data);
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       SPI 接口向传感器的寄存器写 8bit 数组
// @param       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// @param       register_name   寄存器地址
// @param       *data           数据存放缓冲区
// @param       len             缓冲区长度
// @return      void
// Sample usage:                spi_write_8bit_registers(SPI_1, 0x11, data, 32);
//-------------------------------------------------------------------------------------------------------------------
void spi_write_8bit_registers (spi_index_enum spi_n, const uint8 register_name, const uint8 *data, uint32 len)
{
#ifdef SPI_SPEED_PRIORITY
    SPI_CTRL_CS(spi_n, ENABLE);

    SPI_DATA_TX(spi_n, register_name);                                          // 发送数据
    while(!SPI_STATE_TX_EMPTY(spi_n));                                          // 发送为空

    while(len --)
    {
        SPI_DATA_TX(spi_n, *data ++);                                           // 发送数据
        while(!SPI_STATE_TX_EMPTY(spi_n));                                      // 发送为空
    }

    while(SPI_STATE_BUY(spi_n));                                                // 等待传输完毕
    SPI_CTRL_CS(spi_n, DISABLE);
#else
    spi_8bit_data_handler(spi_n, register_name);
    while(len --)
        spi_8bit_data_handler(spi_n, *data ++);
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       SPI 接口向传感器的寄存器写 16bit 数据
// @param       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// @param       register_name   寄存器地址
// @param       data            数据
// @return      void
// Sample usage:                spi_write_16bit_register(SPI_1, 0x1011, 0x0101);
//-------------------------------------------------------------------------------------------------------------------
void spi_write_16bit_register (spi_index_enum spi_n, const uint16 register_name, const uint16 data)
{
#ifdef SPI_SPEED_PRIORITY
    SPI_CTRL_CS(spi_n, ENABLE);

    SPI_DATA_TX(spi_n, (uint8)((register_name & 0xFF00) >> 8));                 // 发送数据
    while(!SPI_STATE_TX_EMPTY(spi_n));                                          // 发送为空

    SPI_DATA_TX(spi_n, (uint8)(register_name & 0x00FF));                        // 发送数据
    while(!SPI_STATE_TX_EMPTY(spi_n));                                          // 发送为空

    SPI_DATA_TX(spi_n, (uint8)((data & 0xFF00) >> 8));                          // 发送数据
    while(!SPI_STATE_TX_EMPTY(spi_n));                                          // 发送为空

    SPI_DATA_TX(spi_n, (uint8)(data & 0x00FF));                                 // 发送数据
    while(!SPI_STATE_TX_EMPTY(spi_n));                                          // 发送为空

    while(SPI_STATE_BUY(spi_n));                                                // 等待传输完毕
    SPI_CTRL_CS(spi_n, DISABLE);
#else
    spi_16bit_data_handler(spi_n, register_name);
    spi_16bit_data_handler(spi_n, data);
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       SPI 接口向传感器的寄存器写 16bit 数组
// @param       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// @param       register_name   寄存器地址
// @param       *data           数据存放缓冲区
// @param       len             缓冲区长度
// @return      void
// Sample usage:                spi_write_16bit_registers(SPI_1, 0x1011, data, 32);
//-------------------------------------------------------------------------------------------------------------------
void spi_write_16bit_registers (spi_index_enum spi_n, const uint16 register_name, const uint16 *data, uint32 len)
{
#ifdef SPI_SPEED_PRIORITY
    SPI_CTRL_CS(spi_n, ENABLE);

    SPI_DATA_TX(spi_n, (uint8)((register_name & 0xFF00) >> 8));                 // 发送数据
    while(!SPI_STATE_TX_EMPTY(spi_n));                                          // 发送为空

    SPI_DATA_TX(spi_n, (uint8)(register_name & 0x00FF));                        // 发送数据
    while(!SPI_STATE_TX_EMPTY(spi_n));                                          // 发送为空

    while(len --)
    {
        SPI_DATA_TX(spi_n, (uint8)((*data & 0xFF00) >> 8));                     // 发送数据
        while(!SPI_STATE_TX_EMPTY(spi_n));                                      // 发送为空

        SPI_DATA_TX(spi_n, (uint8)(*data ++ & 0x00FF));                         // 发送数据
        while(!SPI_STATE_TX_EMPTY(spi_n));                                      // 发送为空
    }

    while(SPI_STATE_BUY(spi_n));                                                // 等待传输完毕
    SPI_CTRL_CS(spi_n, DISABLE);
#else
    spi_16bit_data_handler(spi_n, register_name);
    while(len--)
        spi_16bit_data_handler(spi_n, *data++);
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       SPI 接口读 8bit 数据
// @param       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// @param       register_name   寄存器地址
// @return      uint8           数据
// Sample usage:                spi_read_8bit(SPI_1);
//-------------------------------------------------------------------------------------------------------------------
uint8 spi_read_8bit (spi_index_enum spi_n)
{
#ifdef SPI_SPEED_PRIORITY
    uint8 data = 0;

    SPI_CTRL_RX(spi_n, ENABLE);
    SPI_CTRL_CS(spi_n, ENABLE);

    SPI_DATA_TX(spi_n, 0);                                                      // 发送数据
    while(!SPI_STATE_RX_AVAILABLE(spi_n));                                      // 接收到有效数据
    data = SPI_DATA_RX(spi_n);                                                  // 读取数据

    SPI_CTRL_CS(spi_n, DISABLE);
    SPI_CTRL_RX(spi_n, DISABLE);
    return data;
#else
    return spi_8bit_data_handler(spi_n, 0);
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       SPI 接口读 8bit 数组
// @param       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// @param       *data           数据存放缓冲区
// @param       len             发送缓冲区长度
// @return      void
// Sample usage:                spi_read_8bit_array(SPI_1, data, 64);
//-------------------------------------------------------------------------------------------------------------------
void spi_read_8bit_array (spi_index_enum spi_n, uint8 *data, uint32 len)
{
#ifdef SPI_SPEED_PRIORITY
    SPI_CTRL_RX(spi_n, ENABLE);
    SPI_CTRL_CS(spi_n, ENABLE);

    while(len --)
    {
        SPI_DATA_TX(spi_n, 0);                                                  // 发送数据
        while(!SPI_STATE_RX_AVAILABLE(spi_n));                                  // 接收到有效数据
        *data ++ = SPI_DATA_RX(spi_n);                                          // 读取数据
    }

    SPI_CTRL_CS(spi_n, DISABLE);
    SPI_CTRL_RX(spi_n, DISABLE);
#else
    while(len --)
        *data ++ = spi_8bit_data_handler(spi_n, 0);
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       SPI 接口读 16bit 数据
// @param       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// @param       register_name   寄存器地址
// @return      uint16          数据
// Sample usage:                spi_read_16bit(SPI_1);
//-------------------------------------------------------------------------------------------------------------------
uint16 spi_read_16bit (spi_index_enum spi_n)
{
#ifdef SPI_SPEED_PRIORITY
    uint16 data = 0;

    SPI_CTRL_RX(spi_n, ENABLE);
    SPI_CTRL_CS(spi_n, ENABLE);

    SPI_DATA_TX(spi_n, 0);                                                      // 发送数据
    while(!SPI_STATE_RX_AVAILABLE(spi_n));                                      // 接收到有效数据
    data = SPI_DATA_RX(spi_n);                                                  // 读取数据

    SPI_DATA_TX(spi_n, 0);                                                      // 发送数据
    while(!SPI_STATE_RX_AVAILABLE(spi_n));                                      // 接收到有效数据
    data = ((data << 8)| SPI_DATA_RX(spi_n));                                   // 读取数据

    SPI_CTRL_CS(spi_n, DISABLE);
    SPI_CTRL_RX(spi_n, DISABLE);
    return data;
#else
    return spi_16bit_data_handler(spi_n, 0);
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       SPI 接口读 16bit 数组
// @param       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// @param       *data           数据存放缓冲区
// @param       len             发送缓冲区长度
// @return      void
// Sample usage:                spi_read_16bit_array(SPI_1, data, 64);
//-------------------------------------------------------------------------------------------------------------------
void spi_read_16bit_array (spi_index_enum spi_n, uint16 *data, uint32 len)
{
#ifdef SPI_SPEED_PRIORITY
    SPI_CTRL_RX(spi_n, ENABLE);
    SPI_CTRL_CS(spi_n, ENABLE);

    while(len --)
    {
        SPI_DATA_TX(spi_n, 0);                                                  // 发送数据
        while(!SPI_STATE_RX_AVAILABLE(spi_n));                                  // 接收到有效数据
        *data = SPI_DATA_RX(spi_n);                                             // 读取数据

        SPI_DATA_TX(spi_n, 0);                                                  // 发送数据
        while(!SPI_STATE_RX_AVAILABLE(spi_n));                                  // 接收到有效数据
        *data = ((*data << 8)| SPI_DATA_RX(spi_n));                             // 读取数据
        data ++;
    }

    SPI_CTRL_CS(spi_n, DISABLE);
    SPI_CTRL_RX(spi_n, DISABLE);
#else
    while(len --)
        *data ++ = spi_16bit_data_handler(spi_n, 0);
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       SPI 接口从传感器的寄存器读 8bit 数据
// @param       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// @param       register_name   寄存器地址
// @return      uint8           数据
// Sample usage:                spi_read_8bit_register(SPI_1, 0x11);
//-------------------------------------------------------------------------------------------------------------------
uint8 spi_read_8bit_register (spi_index_enum spi_n, const uint8 register_name)
{
#ifdef SPI_SPEED_PRIORITY
    uint8 data = 0;

    SPI_CTRL_CS(spi_n, ENABLE);

    SPI_DATA_TX(spi_n, register_name);                                          // 发送数据
    while(!SPI_STATE_TX_EMPTY(spi_n));                                          // 发送为空

    SPI_CTRL_RX(spi_n, ENABLE);

    SPI_DATA_TX(spi_n, 0);                                                      // 发送数据
    while(!SPI_STATE_RX_AVAILABLE(spi_n));                                      // 等待读取一个数据
    data = SPI_DATA_RX(spi_n);                                                  // 读取数据

    SPI_CTRL_CS(spi_n, DISABLE);
    SPI_CTRL_RX(spi_n, DISABLE);
    return data;
#else
    spi_8bit_data_handler(spi_n, register_name);
    return spi_8bit_data_handler(spi_n, 0);
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       SPI 接口从传感器的寄存器读 8bit 数组
// @param       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// @param       register_name   寄存器地址
// @param       *data           数据存放缓冲区
// @param       len             发送缓冲区长度
// @return      void
// Sample usage:                spi_read_8bit_registers(SPI_1, 0x11, data, 32);
//-------------------------------------------------------------------------------------------------------------------
void spi_read_8bit_registers (spi_index_enum spi_n, const uint8 register_name, uint8 *data, uint32 len)
{
#ifdef SPI_SPEED_PRIORITY
    SPI_CTRL_CS(spi_n, ENABLE);

    SPI_DATA_TX(spi_n, register_name);                                          // 发送数据
    while(!SPI_STATE_TX_EMPTY(spi_n));                                          // 发送为空

    SPI_CTRL_RX(spi_n, ENABLE);

    while(len --)
    {
        SPI_DATA_TX(spi_n, 0);                                                  // 发送数据
        while(!SPI_STATE_RX_AVAILABLE(spi_n));                                  // 等待读取一个数据
        *data ++ = SPI_DATA_RX(spi_n);                                          // 读取数据
    }

    SPI_CTRL_CS(spi_n, DISABLE);
    SPI_CTRL_RX(spi_n, DISABLE);
#else
    spi_8bit_data_handler(spi_n, register_name);
    while(len --)
        *data ++ = spi_8bit_data_handler(spi_n, 0);
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       SPI 接口从传感器的寄存器读 16bit 数据
// @param       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// @param       register_name   寄存器地址
// @return      uint16          数据
// Sample usage:                spi_read_16bit_register(SPI_1, 0x1011);
//-------------------------------------------------------------------------------------------------------------------
uint16 spi_read_16bit_register (spi_index_enum spi_n, const uint16 register_name)
{
#ifdef SPI_SPEED_PRIORITY
    uint16 data = 0;
    SPI_CTRL_CS(spi_n, ENABLE);

    SPI_DATA_TX(spi_n, (uint8)((register_name & 0xFF00) >> 8));                 // 发送数据
    while(!SPI_STATE_TX_EMPTY(spi_n));                                          // 发送为空

    SPI_DATA_TX(spi_n, (uint8)(register_name & 0x00FF));                        // 发送数据
    while(!SPI_STATE_TX_EMPTY(spi_n));                                          // 发送为空

    SPI_CTRL_RX(spi_n, ENABLE);

    SPI_DATA_TX(spi_n, 0);                                                      // 发送数据
    while(!SPI_STATE_RX_AVAILABLE(spi_n));                                      // 等待读取一个数据
    data = SPI_DATA_RX(spi_n);                                                  // 读取数据

    SPI_DATA_TX(spi_n, 0);                                                      // 发送数据
    while(!SPI_STATE_RX_AVAILABLE(spi_n));                                      // 等待读取一个数据
    data = ((data << 8)| SPI_DATA_RX(spi_n));                                   // 读取数据

    SPI_CTRL_CS(spi_n, DISABLE);
    SPI_CTRL_RX(spi_n, DISABLE);
    return data;
#else
    spi_16bit_data_handler(spi_n, register_name);
    return spi_16bit_data_handler(spi_n, 0);
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       SPI 接口从传感器的寄存器读 16bit 数组
// @param       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// @param       register_name   寄存器地址
// @param       *data           数据存放缓冲区
// @param       len             发送缓冲区长度
// @return      void
// Sample usage:                spi_read_16bit_registers(SPI_1, 0x1101, data, 32);
//-------------------------------------------------------------------------------------------------------------------
void spi_read_16bit_registers (spi_index_enum spi_n, const uint16 register_name, uint16 *data, uint32 len)
{
#ifdef SPI_SPEED_PRIORITY
    SPI_CTRL_CS(spi_n, ENABLE);

    SPI_DATA_TX(spi_n, (uint8)((register_name & 0xFF00) >> 8));                 // 发送数据
    while(!SPI_STATE_TX_EMPTY(spi_n));                                          // 发送为空

    SPI_DATA_TX(spi_n, (uint8)(register_name & 0x00FF));                        // 发送数据
    while(!SPI_STATE_TX_EMPTY(spi_n));                                          // 发送为空

    SPI_CTRL_RX(spi_n, ENABLE);
 
    while(len --)
    {
        SPI_DATA_TX(spi_n, 0);                                                  // 发送数据
        while(!SPI_STATE_RX_AVAILABLE(spi_n));                                  // 等待读取一个数据
        *data = SPI_DATA_RX(spi_n);                                             // 读取数据

        SPI_DATA_TX(spi_n, 0);                                                  // 发送数据
        while(!SPI_STATE_RX_AVAILABLE(spi_n));                                  // 等待读取一个数据
        *data = ((*data << 8)| SPI_DATA_RX(spi_n));                             // 读取数据
        data ++;
    }

    SPI_CTRL_CS(spi_n, DISABLE);
    SPI_CTRL_RX(spi_n, DISABLE);
#else
    spi_16bit_data_handler(spi_n, register_name);
    while(len --)
        *data ++ = spi_16bit_data_handler(spi_n, 0);
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       SPI 8bit 数据传输 发送与接收数据是同时进行的
// @param       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// @param       write_buffer    发送的数据缓冲区地址
// @param       read_buffer     发送数据时接收到的数据的存储地址(不需要接收则传 NULL)
// @param       len             缓冲区长度
// @return      void
// Sample usage:                spi_transfer_8bit(SPI_1, buf, buf, 1);
//-------------------------------------------------------------------------------------------------------------------
void spi_transfer_8bit (spi_index_enum spi_n, const uint8 *write_buffer, uint8 *read_buffer, uint32 len)
{
#ifdef SPI_SPEED_PRIORITY
    SPI_CTRL_RX(spi_n, ENABLE);
    SPI_CTRL_CS(spi_n, ENABLE);

    while(len --)                                                               // 判断长度
    {
        SPI_DATA_TX(spi_n, *write_buffer ++);                                   // 发送数据
        if(read_buffer != NULL)                                                 // 接收有效
        {
            while(!SPI_STATE_RX_AVAILABLE(spi_n));                              // 等待读取一个数据
            *read_buffer ++ = SPI_DATA_RX(spi_n);                               // 读取数据
        }
        else
        {
            while(!SPI_STATE_TX_EMPTY(spi_n));                                  // 发送为空
        }
    }

    while(SPI_STATE_BUY(spi_n));                                                // 等待传输完毕
    SPI_CTRL_CS(spi_n, DISABLE);
    SPI_CTRL_RX(spi_n, DISABLE);
#else
    while(len --)
    {
        *read_buffer = spi_8bit_data_handler(spi_n, *write_buffer);
        write_buffer ++;
        read_buffer ++;
    }
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       SPI 16bit 数据传输 发送与接收数据是同时进行的
// @param       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// @param       write_buffer    发送的数据缓冲区地址
// @param       read_buffer     发送数据时接收到的数据的存储地址(不需要接收则传 NULL)
// @param       len             缓冲区长度
// @return      void
// Sample usage:                spi_transfer_16bit(SPI_1, buf, buf, 1);
//-------------------------------------------------------------------------------------------------------------------
void spi_transfer_16bit (spi_index_enum spi_n, const uint16 *write_buffer, uint16 *read_buffer, uint32 len)
{
#ifdef SPI_SPEED_PRIORITY
    SPI_CTRL_RX(spi_n, ENABLE);
    SPI_CTRL_CS(spi_n, ENABLE);

    while(len --)                                                               // 判断长度
    {
        SPI_DATA_TX(spi_n, (*write_buffer & 0xFF00) >> 8);                      // 发送数据
        if(read_buffer != NULL)                                                 // 接收有效
        {
            while(!SPI_STATE_RX_AVAILABLE(spi_n));                              // 等待读取一个数据
            *read_buffer = (SPI_DATA_RX(spi_n) & 0x00FF);                       // 读取数据
        }
        else
        {
            while(!SPI_STATE_TX_EMPTY(spi_n));                                  // 发送为空
        }

        SPI_DATA_TX(spi_n, (*write_buffer & 0x00FF));                           // 发送数据
        if(read_buffer != NULL)                                                 // 接收有效
        {
            while(!SPI_STATE_RX_AVAILABLE(spi_n));                              // 等待读取一个数据
            *read_buffer = (*read_buffer << 8) | (SPI_DATA_RX(spi_n)& 0x00FF);  // 读取数据
        }
        else
        {
            while(!SPI_STATE_TX_EMPTY(spi_n));                                  // 发送为空
        }

        write_buffer ++;
        read_buffer ++;
    }

    while(SPI_STATE_BUY(spi_n));                                                // 等待传输完毕
    SPI_CTRL_CS(spi_n, DISABLE);
    SPI_CTRL_RX(spi_n, DISABLE);
#else
    while(len --)
    {
        *read_buffer = spi_16bit_data_handler(spi_n, *write_buffer);
        write_buffer ++;
        read_buffer ++;
    }
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       SPI 接口初始化
// @param       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// @param       mode            SPI 模式 参照 zf_driver_spi.h 内 spi_mode_enum 枚举体定义
// @param       baud            设置 SPI 的波特率 不超过系统时钟的一半 部分速率会被适配成相近的速率
// @param       sck_pin         选择 SCK 引脚 参照 zf_driver_spi.h 内 spi_sck_pin_enum 枚举体定义
// @param       mosi_pin        选择 MOSI 引脚 参照 zf_driver_spi.h 内 spi_mosi_pin_enum 枚举体定义
// @param       miso_pin        选择 MISO 引脚 参照 zf_driver_spi.h 内 spi_miso_pin_enum 枚举体定义
// @param       cs_pin          选择 CS 引脚 参照 zf_driver_spi.h 内 spi_cs_pin_enum 枚举体定义
// @return      void
// Sample usage:                spi_init(SPI_1, 0, 1*1000*1000, SPI1_SCK_A05, SPI1_MOSI_A07, SPI1_MISO_A06, SPI1_NSS_A04);  //硬件SPI初始化  模式0 波特率为1Mhz
//-------------------------------------------------------------------------------------------------------------------
void spi_init (spi_index_enum spi_n, spi_mode_enum mode, uint32 baud, spi_sck_pin_enum sck_pin, spi_mosi_pin_enum mosi_pin, spi_miso_pin_enum miso_pin, spi_cs_pin_enum cs_pin)
{
    afio_init((gpio_pin_enum)(sck_pin & 0xFF), GPO, (gpio_af_enum)((sck_pin & 0xF00) >> 8), GPO_AF_PUSH_PULL);      // 提取对应IO索引 AF功能编码
    afio_init((gpio_pin_enum)(mosi_pin & 0xFF), GPO, (gpio_af_enum)((mosi_pin & 0xF00) >> 8), GPO_AF_PUSH_PULL);    // 提取对应IO索引 AF功能编码
    if(SPI_MISO_NULL != miso_pin)
        afio_init((gpio_pin_enum)(miso_pin & 0xFF), GPI, (gpio_af_enum)((miso_pin & 0xF00) >> 8), GPI_FLOATING_IN); // 提取对应IO索引 AF功能编码
    if(SPI_CS_NULL != cs_pin)
        afio_init((gpio_pin_enum)(cs_pin & 0xFF), GPO, (gpio_af_enum)((cs_pin & 0xF00) >> 8), GPO_AF_PUSH_PULL);    // 提取对应IO索引 AF功能编码

    if(SPI_1 == spi_n)
        RCC->APB2ENR |= (0x00000001 << 12);
    else if(SPI_2 == spi_n)
        RCC->APB1ENR |= (0x00000001 << 14);
    else
        RCC->APB1ENR |= (0x00000001 << 15);

    spi_index[spi_n]->GCTL |= (0x00000001 << 2);                                // 主机模式
    spi_index[spi_n]->CCTL |= (0x00000001 << 3);                                // 8bits 数据
    switch(mode)
    {
        default:
        case SPI_MODE0:
            spi_index[spi_n]->CCTL &= ~(0x00000003);                            // SCK 空闲时低电平
            spi_index[spi_n]->CCTL |= (0x00000001);                             // 第一个时钟边沿开始采样
            break;
        case SPI_MODE1:
            spi_index[spi_n]->CCTL &= ~(0x00000003);                            // SCK 空闲时低电平
            spi_index[spi_n]->CCTL |= (0x00000000);                             // 第二个时钟边沿开始采样
            break;
        case SPI_MODE2:
            spi_index[spi_n]->CCTL &= ~(0x00000003);                            // SCK 空闲时高电平
            spi_index[spi_n]->CCTL |= (0x00000003);                             // 第一个时钟边沿开始采样
            break;
        case SPI_MODE3:
            spi_index[spi_n]->CCTL &= ~(0x00000003);                            // SCK 空闲时高电平
            spi_index[spi_n]->CCTL |= (0x00000002);                             // 第二个时钟边沿开始采样
            break;
    }
    spi_index[spi_n]->CCTL |= (0x00000001 << 4);                                // 在传输数据位的尾时钟沿采样数据 用于高速模式
    if(SYSTEM_CLOCK_120M < system_clock)
    {
        spi_index[spi_n]->SPBRG = (system_clock / 2 / baud);
    }
    else
    {
        spi_index[spi_n]->SPBRG = (system_clock / baud);
    }
    spi_index[spi_n]->CCTL &= ~(0x00000001 << 2);                               // MSB
//    spi_index[spi_n]->GCTL |= (0x00000003 << 3);                                // 使能 接收发送
    spi_index[spi_n]->GCTL |= (0x00000001 << 3);                                // 使能 发送
    spi_index[spi_n]->GCTL |= (0x00000001);                                     // 使能

    while(SPI_STATE_BUY(spi_n));                                                // 等待传输完毕
}
