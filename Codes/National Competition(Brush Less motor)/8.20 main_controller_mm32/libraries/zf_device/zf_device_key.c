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

#include "zf_device_key.h"

static uint32               scanner_period = 0;                                 // 按键的扫描周期
static uint32               key_press_time[KEY_NUMBER];                         // 按键信号持续时长
static key_state_enum       key_state[KEY_NUMBER];                              // 按键状态

static const gpio_pin_enum  key_index[KEY_NUMBER] = KEY_LIST;

//-------------------------------------------------------------------------------------------------------------------
// @brief       按键状态扫描 这个函数放在主循环或者 PIT 中断中
// @param       void
// @return      void
// Sample usage:                key_scanner();
//-------------------------------------------------------------------------------------------------------------------
void key_scanner (void)
{
    uint8 i = 0;
    for(i = 0; i < KEY_NUMBER; i ++)
    {
        switch(key_state[i])
        {
            case KEY_RELEASE:
                if(KEY_RELEASE_LEVEL != gpio_get_level(key_index[i]))
                {
                    if(++ key_press_time[i] >= KEY_MAX_SHOCK_PERIOD / scanner_period)
                        key_state[i] = KEY_SHORT_PRESS;
                    else
                        key_state[i] = KEY_CHECK_SHOCK;
                }
                break;
            case KEY_CHECK_SHOCK:
                if(KEY_RELEASE_LEVEL != gpio_get_level(key_index[i]))
                {
                    if(++ key_press_time[i] >= KEY_MAX_SHOCK_PERIOD / scanner_period)
                        key_state[i] = KEY_SHORT_PRESS;
                }
                else
                {
                    key_state[i] = KEY_RELEASE;
                    key_press_time[i] = 0;
                }
                break;
            case KEY_SHORT_PRESS:
                if(++ key_press_time[i] >= KEY_LONG_PRESS_PERIOD / scanner_period)
                    key_state[i] = KEY_LONG_PRESS;
                if(KEY_RELEASE_LEVEL == gpio_get_level(key_index[i]))
                {
                    key_state[i] = KEY_RELEASE;
                    key_press_time[i] = 0;
                }
                break;
            case KEY_LONG_PRESS:
                if(KEY_RELEASE_LEVEL == gpio_get_level(key_index[i]))
                {
                    key_state[i] = KEY_RELEASE;
                    key_press_time[i] = 0;
                }
                break;
                
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       获取按键状态
// @param       key_n           按键索引
// @return      key_state_enum  按键状态
// Sample usage:                key_get_state(KEY_1);
//-------------------------------------------------------------------------------------------------------------------
key_state_enum key_get_state (key_index_enum key_n)
{
    return key_state[key_n];
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       按键初始化
// @param       period          按键扫描周期 以毫秒为单位
// @return      void
// Sample usage:                key_init(10);
//-------------------------------------------------------------------------------------------------------------------
void key_init (uint32 period)
{
    uint8 loop_temp = 0; 
    for(loop_temp = 0; loop_temp < KEY_NUMBER; loop_temp ++)
    {
        gpio_init(key_index[loop_temp], GPI, GPIO_HIGH, GPI_PULL_UP);
        key_state[loop_temp] = KEY_RELEASE;
    }
    scanner_period = period;
}
