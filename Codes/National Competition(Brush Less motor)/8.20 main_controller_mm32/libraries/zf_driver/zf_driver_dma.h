/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_driver_dma
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
* @note             
********************************************************************************************************************/

#ifndef _zf_driver_dma_h_
#define _zf_driver_dma_h_

#include "hal_device_registers.h"

#include "zf_common_interrupt.h"

typedef enum                                                                    // 枚举 DMA 通道  此枚举定义不允许用户修改
{
    DMA1_CHANNEL1 = 0x00,
    DMA1_CHANNEL2,
    DMA1_CHANNEL3,
    DMA1_CHANNEL4,
    DMA1_CHANNEL5,
    DMA1_CHANNEL6,
    DMA1_CHANNEL7,

    DMA2_CHANNEL1 = 0x10,
    DMA2_CHANNEL2,
    DMA2_CHANNEL3,
    DMA2_CHANNEL4,
    DMA2_CHANNEL5,
}dma_channel_enum;


void    dma_set_count      (dma_channel_enum ch, const uint16 transfer_count);
void    dma_enable         (dma_channel_enum ch);
void    dma_disable        (dma_channel_enum ch);
void    dma_init_camera    (dma_channel_enum ch, const uint32 size, const uint32 peripheral_add, const uint32 memory_add);

#endif
