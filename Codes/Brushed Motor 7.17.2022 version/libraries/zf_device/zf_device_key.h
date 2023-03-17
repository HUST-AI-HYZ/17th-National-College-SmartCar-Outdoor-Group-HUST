/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_key
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
*                   // 一般是主板按键对应的引脚
*                   KEY1/S1             查看 zf_device_key.h 中 KEY_LIST[0]
*                   KEY2/S2             查看 zf_device_key.h 中 KEY_LIST[1]
*                   KEY3/S3             查看 zf_device_key.h 中 KEY_LIST[2]
*                   KEY4/S4             查看 zf_device_key.h 中 KEY_LIST[3]
*                   ------------------------------------
********************************************************************************************************************/

#ifndef _zf_device_key_h_
#define _zf_device_key_h_

#include "zf_common_debug.h"

#include "zf_driver_gpio.h"

// 定义按键引脚 用户可以新增可以修改 默认定义四个按键
// 定义按键顺序对应下方 key_index_enum 枚举体中定义的顺序
// 如果用户可以新增按键 那么需要同步在下方 key_index_enum 枚举体中新增按键
#define KEY_LIST                    {G0, G1, G2, G3}

#define KEY_RELEASE_LEVEL           (GPIO_HIGH)                                 // 按键的默认状态 也就是按键释放状态的电平
#define KEY_MAX_SHOCK_PERIOD        (10       )                                 // 按键消抖检测时长 单位毫秒 低于这个时长的信号会被认为是杂波抖动
#define KEY_LONG_PRESS_PERIOD       (1000     )                                 // 最小长按时长 单位毫秒

typedef enum
{
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_NUMBER,
}key_index_enum;                                                                // 按键索引 对应上方定义的按键引脚个数 默认定义四个按键

typedef enum
{
    KEY_RELEASE,                                                                // 按键释放状态
    KEY_CHECK_SHOCK,                                                            // 按键消抖状态
    KEY_SHORT_PRESS,                                                            // 按键短按状态
    KEY_LONG_PRESS,                                                             // 按键长按状态
}key_state_enum;

void            key_scanner     (void);
key_state_enum  key_get_state   (key_index_enum key_n);
void            key_init        (uint32 period);

#endif
