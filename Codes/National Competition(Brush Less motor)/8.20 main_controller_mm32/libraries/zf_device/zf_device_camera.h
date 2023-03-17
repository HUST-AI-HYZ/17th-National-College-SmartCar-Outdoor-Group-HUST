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

#ifndef _zf_device_camera_h_
#define _zf_device_camera_h_

#include "zf_common_debug.h"
#include "zf_common_interrupt.h"

#include "zf_driver_exti.h"
#include "zf_driver_dma.h"
#include "zf_driver_timer.h"
#include "zf_driver_uart.h"

extern  uint8 camera_send_image_frame_header[4];

void    camera_binary_image_decompression   (const uint8 *data1, uint8 *data2, uint32 image_size);
void    camera_send_image                   (uart_index_enum uartn, const uint8 *image_addr, uint32 image_size);

void    camera_init                         (const uint32 image_size, uint32 data_addr, uint32 buffer_addr);

#endif
