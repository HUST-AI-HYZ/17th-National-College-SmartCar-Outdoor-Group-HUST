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

#include "zf_driver_dma.h"

static DMA_Type *dma_index[2] = {DMA1, DMA2};

//-------------------------------------------------------------------------------------------------------------------
// @brief       dma 传输数据数量设置
// @param       ch              选择 dma 通道 (详见 zf_driver_dma.h 中枚举 dma_channel_enum 定义)
// @param       size            传输数据数量
// @return      void
// Sample usage:                dma_set_count(MT9V03X_DMA_CH, image_size);
//-------------------------------------------------------------------------------------------------------------------
void dma_set_count (dma_channel_enum ch, const uint16 transfer_count)
{
    dma_index[(ch & 0xF0) >> 4]->CH[ch & 0x0f].CNDTR = transfer_count;          // 传输数量
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       dma 传输使能
// @param       ch              选择 dma 通道 (详见 zf_driver_dma.h 中枚举 dma_channel_enum 定义)
// @return      void
// Sample usage:                dma_enable(MT9V03X_DMA_CH);
//-------------------------------------------------------------------------------------------------------------------
void dma_enable (dma_channel_enum ch)
{
    dma_index[(ch & 0xF0) >> 4]->CH[ch & 0x0f].CCR |= 0x00000001;               // 使能 DMA
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       dma 传输禁止
// @param       ch              选择 dma 通道 (详见 zf_driver_dma.h 中枚举 dma_channel_enum 定义)
// @return      void
// Sample usage:                dma_disable(MT9V03X_DMA_CH);
//-------------------------------------------------------------------------------------------------------------------
void dma_disable (dma_channel_enum ch)
{
    dma_index[(ch & 0xF0) >> 4]->CH[ch & 0x0f].CCR &= 0xFFFFFFFE;               // 禁止 DMA
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       dma 初始化 摄像头接口
// @param       ch              选择 dma 通道 (详见 zf_driver_dma.h 中枚举 dma_channel_enum 定义)
// @param       size            传输数据数量
// @param       peripheral_add  外设地址 摄像头数据接口的地址
// @param       memory_add      摄像头数据存放地址
// @return      void
// Sample usage:                dma_init_camera(MT9V03X_DMA_CH, image_size, (uint32)MT9V03X_DATA_ADD, (uint32)camera_buffer_addr);
//-------------------------------------------------------------------------------------------------------------------
void dma_init_camera (dma_channel_enum ch, const uint32 size, const uint32 peripheral_add, const uint32 memory_add)
{
    RCC->AHB1ENR |= (0x00200000 << ((ch & 0xF0) >> 4));                         // 使能 ADC 时钟
    RCC->AHB1RSTR |= (0x00200000 << ((ch & 0xF0) >> 4));                        // 复位 ADC 外设
    RCC->AHB1RSTR &= ~(0x00200000 << ((ch & 0xF0) >> 4));                       // 复位 结束

    dma_index[(ch & 0xF0) >> 4]->CH[ch & 0x0f].CNDTR = size;                    // 传输数量
    dma_index[(ch & 0xF0) >> 4]->CH[ch & 0x0f].CPAR = peripheral_add;           // 外设地址
    dma_index[(ch & 0xF0) >> 4]->CH[ch & 0x0f].CMAR = memory_add;               //  
    dma_index[(ch & 0xF0) >> 4]->CH[ch & 0x0f].CCR = 0x0000B082;                // 设置 自动重装载 最高优先级 存储器地址递增 使能传输完成中断 使能 DMA

    switch(ch)
    {
        case DMA1_CHANNEL1: interrupt_enable(DMA1_CH1_IRQn);    break;
        case DMA1_CHANNEL2: interrupt_enable(DMA1_CH2_IRQn);    break;
        case DMA1_CHANNEL3: interrupt_enable(DMA1_CH3_IRQn);    break;
        case DMA1_CHANNEL4: interrupt_enable(DMA1_CH4_IRQn);    break;
        case DMA1_CHANNEL5: interrupt_enable(DMA1_CH5_IRQn);    break;
        case DMA1_CHANNEL6: interrupt_enable(DMA1_CH6_IRQn);    break;
        case DMA1_CHANNEL7: interrupt_enable(DMA1_CH7_IRQn);    break;

        case DMA2_CHANNEL1: interrupt_enable(DMA2_CH1_IRQn);    break;
        case DMA2_CHANNEL2: interrupt_enable(DMA2_CH2_IRQn);    break;
        case DMA2_CHANNEL3: interrupt_enable(DMA2_CH3_IRQn);    break;
        case DMA2_CHANNEL4: interrupt_enable(DMA2_CH4_IRQn);    break;
        case DMA2_CHANNEL5: interrupt_enable(DMA2_CH5_IRQn);    break;
    }
}
