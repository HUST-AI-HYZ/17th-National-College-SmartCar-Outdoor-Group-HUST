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

#ifndef _zf_device_type_h_
#define _zf_device_type_h_

#include "zf_common_clock.h"
#include "zf_common_debug.h"

typedef enum
{
    NO_CAMERE = 0,                                                              // 无摄像头
    CAMERA_BIN_IIC,                                                             // 小钻风 IIC 版本
    CAMERA_BIN_UART,                                                            // 小钻风 UART 版本
    CAMERA_GRAYSCALE,                                                           // 总钻风
    CAMERA_COLOR,                                                               // 凌瞳
}camera_type_enum;

typedef enum
{
    NO_WIRELESS = 0,                                                            // 无设备
    WIRELESS_UART,                                                              // 无线串口
    BULETOOTH_CH9141,                                                           // 蓝牙 CH9141
}wireless_type_enum;

extern camera_type_enum camera_type;
extern wireless_type_enum wireless_type;
extern          uint8     receive[3];
extern          uint8     receive_num;
extern volatile uint8     uart_receive_flag;

void        set_camera_type (camera_type_enum type_set);

void        camera_uart_handler             (void);
void        camera_vsync_handler            (void);
void        camera_dma_handler              (void);

void        set_wireless_type               (wireless_type_enum type_set);

void        wireless_module_uart_handler    (void);

#endif
