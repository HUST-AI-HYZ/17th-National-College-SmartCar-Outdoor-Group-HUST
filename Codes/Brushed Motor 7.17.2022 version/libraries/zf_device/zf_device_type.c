/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_type
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
* @note             
********************************************************************************************************************/

#include "zf_device_type.h"

#include "zf_device_bluetooth_ch9141.h"
#include "zf_device_mt9v03x.h"
#include "zf_device_ov7725.h"
#include "zf_device_scc8660.h"
#include "zf_device_wireless_uart.h"

camera_type_enum    camera_type     = NO_CAMERE;                                // 摄像头类型变量
wireless_type_enum  wireless_type   = NO_WIRELESS;

         uint8     receive[3] = {0, 0, 0};
         uint8     receive_num = 0;
volatile uint8     uart_receive_flag = 0;

//-------------------------------------------------------------------------------------------------------------------
// @brief       设置摄像头类型
// @param       type_set        选定的摄像头类型
// @return      void
// Sample usage:                set_camera_type(CAMERA_GRAYSCALE);
//-------------------------------------------------------------------------------------------------------------------
void set_camera_type (camera_type_enum type_set)
{
    camera_type = type_set;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       摄像头通信串口回调函数 放在对应摄像头的串口中断服务函数里
// @param       void
// @return      void
// Sample usage:                camera_uart_handler();
//-------------------------------------------------------------------------------------------------------------------
void camera_uart_handler (void)
{
    switch(camera_type)                                                         // 查询摄像头类型 未初始化摄像头则此处会进入default
    {
        case CAMERA_BIN_UART:                                                   // 串口小钻风
            if(uart_query_byte(OV7725_COF_UART, &receive[receive_num]))
                receive_num ++;
            break;
        case CAMERA_GRAYSCALE:                                                  // 总钻风
            if(uart_query_byte(MT9V03X_COF_UART, &receive[receive_num]))
                receive_num ++;
            break;
        case CAMERA_COLOR:                                                      // 凌瞳
            if(uart_query_byte(SCC8660_COF_UART, &receive[receive_num]))
                receive_num ++;
            break;
        default:
            break;
    }
    if(1 == receive_num && 0xA5 != receive[0])  receive_num = 0;
    if(3 == receive_num)
    {
        receive_num = 0;
        uart_receive_flag = 1;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       摄像头场中断回调函数 放在对应摄像头的场中断服务函数里
// @param       void
// @return      void
// Sample usage:                camera_vsync_handler();
//-------------------------------------------------------------------------------------------------------------------
void camera_vsync_handler (void)
{
    switch(camera_type)                                                         // 查询摄像头类型 未初始化摄像头则此处会进入default
    {
        case CAMERA_BIN_IIC:                                                    // IIC 小钻风
        case CAMERA_BIN_UART:                                                   // UART 小钻风
            dma_disable(OV7725_DMA_CH);
            dma_set_count(OV7725_DMA_CH, OV7725_IMAGE_SIZE);                    // 设置当前DMA传输的剩余数量 向下递减 该寄存器只能在DMA不工作时更改。
            dma_enable(OV7725_DMA_CH);
            dma_disable(OV7725_DMA_CH);
            dma_set_count(OV7725_DMA_CH, OV7725_IMAGE_SIZE);                    // 设置当前DMA传输的剩余数量 向下递减 该寄存器只能在DMA不工作时更改。
            dma_enable(OV7725_DMA_CH);
            break;
        case CAMERA_GRAYSCALE:                                                  // 总钻风
            dma_disable(MT9V03X_DMA_CH);
            dma_set_count(MT9V03X_DMA_CH, MT9V03X_IMAGE_SIZE);                  // 设置当前DMA传输的剩余数量 向下递减 该寄存器只能在DMA不工作时更改。
            dma_enable(MT9V03X_DMA_CH);
            dma_disable(MT9V03X_DMA_CH);
            dma_set_count(MT9V03X_DMA_CH, MT9V03X_IMAGE_SIZE);                  // 设置当前DMA传输的剩余数量 向下递减 该寄存器只能在DMA不工作时更改。
            dma_enable(MT9V03X_DMA_CH);
            break;
        case CAMERA_COLOR:                                                      // 凌瞳
            dma_disable(SCC8660_DMA_CH);
            dma_set_count(SCC8660_DMA_CH, SCC8660_IMAGE_SIZE);                  // 设置当前DMA传输的剩余数量 向下递减 该寄存器只能在DMA不工作时更改。
            dma_enable(SCC8660_DMA_CH);
            dma_disable(SCC8660_DMA_CH);
            dma_set_count(SCC8660_DMA_CH, SCC8660_IMAGE_SIZE);                  // 设置当前DMA传输的剩余数量 向下递减 该寄存器只能在DMA不工作时更改。
            dma_enable(SCC8660_DMA_CH);
            break;
        default:
            break;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       摄像头 DMA 完成中断回调函数 放在对应摄像头的 DMA 完成中断服务函数里
// @param       void
// @return      void
// Sample usage:                camera_dma_handler();
//-------------------------------------------------------------------------------------------------------------------
void camera_dma_handler (void)
{
    switch(camera_type)                                                         // 查询摄像头类型 未初始化摄像头则此处会进入default
    {
        case CAMERA_BIN_IIC:                                                    // IIC 小钻风
        case CAMERA_BIN_UART:                                                   // UART 小钻风
            dma_disable(OV7725_DMA_CH);
            ov7725_finish_flag = 1;
            break;
        case CAMERA_GRAYSCALE:                                                  // 总钻风
            dma_disable(MT9V03X_DMA_CH);
            mt9v03x_finish_flag = 1;
            break;
        case CAMERA_COLOR:                                                      // 凌瞳
            dma_disable(SCC8660_DMA_CH);
            scc8660_finish_flag = 1;
            break;
        default:
            break;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       设置无线模块类型
// @param       type_set        选定的无线模块类型
// @return      void
// Sample usage:                set_wireless_type(WIRELESS_UART);
//-------------------------------------------------------------------------------------------------------------------
void set_wireless_type (wireless_type_enum type_set)
{
    wireless_type = type_set;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       无线模块串口中断回调函数 放在对应无线模块的串口中断服务函数里
// @param       void
// @return      void
// Sample usage:                wireless_module_uart_handler();
//-------------------------------------------------------------------------------------------------------------------
void wireless_module_uart_handler (void)
{
    switch(wireless_type)                                                       // 查询摄像头类型 未初始化摄像头则此处会进入default
    {
        case WIRELESS_UART:
            wireless_uart_callback();
            break;
        case BULETOOTH_CH9141:
            bluetooth_ch9141_uart_callback();
            break;
        default:
            break;
    }
}
