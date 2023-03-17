/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_camera
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
* @note             
********************************************************************************************************************/

#include "zf_device_camera.h"
#include "zf_device_mt9v03x.h"
#include "zf_device_ov7725.h"
#include "zf_device_scc8660.h"
#include "zf_device_type.h"

uint8 camera_send_image_frame_header[4] = {0x00, 0xFF, 0x01, 0x01};

//-------------------------------------------------------------------------------------------------------------------
// @brief       摄像头二进制图像数据解压为十六进制八位数据 小钻风用
// @param       *data1          摄像头图像数组
// @param       *data2          存放解压数据的地址
// @param       image_size      图像的大小
// @return      void
// Sample usage:                camera_binary_image_decompression(&ov7725_image_binary[0][0], &data_buffer[0][0], OV7725_SIZE);
//-------------------------------------------------------------------------------------------------------------------
void camera_binary_image_decompression (const uint8 *data1, uint8 *data2, uint32 image_size)
{
    uint8  i = 8;

    while(image_size --)
    {
        i = 8;
        while(i --)
        {
            *data2 ++ = (((*data1 >> i) & 0x01) ? 255 : 0);
        }
        data1 ++;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       摄像头图像发送至上位机查看图像
// @param       uartn           使用的串口号
// @param       *image_addr     需要发送的图像地址
// @param       image_size      图像的大小
// @return      void
// Sample usage:                camera_send_image(DEBUG_UART_INDEX, &mt9v03x_image[0][0], MT9V03X_IMAGE_SIZE);
//-------------------------------------------------------------------------------------------------------------------
void camera_send_image (uart_index_enum uartn, const uint8 *image_addr, uint32 image_size)
{
    // 发送命令
    uart_putbuff(uartn, camera_send_image_frame_header, 4);

    // 发送图像
    uart_putbuff(uartn, (uint8 *)image_addr, image_size);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       摄像头初始化
// @param       image_size      图像的大小
// @return      void
// @param       image_size      图像的大小
// @param       data_addr       数据来源外设地址
// @param       buffer_addr     图像缓冲区地址
// @return      void
// Sample usage:                camera_init(MT9V03X_IMAGE_SIZE, (uint32)MT9V03X_DATA_ADD, (uint32)&mt9v03x_image[0][0]);
//-------------------------------------------------------------------------------------------------------------------
void camera_init (const uint32 image_size, uint32 data_addr, uint32 buffer_addr)
{
    switch(camera_type)
    {
        case CAMERA_BIN_IIC:                                                    // IIC 小钻风
        case CAMERA_BIN_UART:                                                   // UART 小钻风
            interrupt_set_priority(OV7725_VSYNC_IRQN, 0x01);                    // 设置 VSY 场中断优先级
            interrupt_set_priority(OV7725_DMA_IRQN, 0x02);                      // 设置 DMA 完成中断优先级

            dma_init_camera(                                                    // 摄像头 DMA 关联初始化
                OV7725_DMA_CH,                                                  // DMA 通道
                image_size,                                                     // 图像大小
                data_addr,                                                      // 指定数据输入的外设地址
                buffer_addr);                                                   // 指定数据存放首地址
            timer_etr_init(OV7725_PCLK_PIN, TIM_ETR_FALLING);                   // 初始化对应的触发定时器
            exti_init(OV7725_VSYNC_PIN, EXTI_TRIGGER_FALLING);                  // 初始化场中断对应的外部中断
            break;
        case CAMERA_GRAYSCALE:                                                  // 总钻风
            interrupt_set_priority(MT9V03X_VSYNC_IRQN, 0x01);                   // 设置 VSY 场中断优先级
            interrupt_set_priority(MT9V03X_DMA_IRQN, 0x02);                     // 设置 DMA 完成中断优先级

            dma_init_camera(                                                    // 摄像头 DMA 关联初始化
                MT9V03X_DMA_CH,                                                 // DMA 通道
                image_size,                                                     // 图像大小
                data_addr,                                                      // 指定数据输入的外设地址
                buffer_addr);                                                   // 指定数据存放首地址
            if(MT9V03X_W > 188)
                timer_etr_init(MT9V03X_PCLK_PIN, TIM_ETR_FALLING);              // 初始化对应的触发定时器
            else
                timer_etr_init(MT9V03X_PCLK_PIN, TIM_ETR_RISING);               // 初始化对应的触发定时器
            exti_init(MT9V03X_VSYNC_PIN, EXTI_TRIGGER_FALLING);                 // 初始化场中断对应的外部中断
            break;
        case CAMERA_COLOR:                                                      // 凌瞳
            interrupt_set_priority(SCC8660_VSYNC_IRQN, 0x01);                   // 设置 VSY 场中断优先级
            interrupt_set_priority(SCC8660_DMA_IRQN, 0x02);                     // 设置 DMA 完成中断优先级

            dma_init_camera(                                                    // 摄像头 DMA 关联初始化
                SCC8660_DMA_CH,                                                 // DMA 通道
                image_size,                                                     // 图像大小
                data_addr,                                                      // 指定数据输入的外设地址
                buffer_addr);                                                   // 指定数据存放首地址
            timer_etr_init(SCC8660_PCLK_PIN, TIM_ETR_RISING);                   // 初始化对应的触发定时器
            exti_init(SCC8660_VSYNC_PIN, EXTI_TRIGGER_FALLING);                 // 初始化场中断对应的外部中断
            break;
        default:
            break;
    }
}
