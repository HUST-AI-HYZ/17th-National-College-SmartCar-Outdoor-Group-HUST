/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_tsl1401
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
*                   CLK                 查看 zf_device_tsl1401.h 中 TSL1401_CLK_PIN 宏定义
*                   SI                  查看 zf_device_tsl1401.h 中 TSL1401_SI_PIN 宏定义
*                   AO[x]               查看 zf_device_tsl1401.h 中 TSL1401_AO_PIN_BUFFER 宏定义
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   ------------------------------------
********************************************************************************************************************/

#include "zf_device_tsl1401.h"

uint16 tsl1401_data[2][TSL1401_DATA_LEN];                                       // TSL1401 数据存放数组

static uint8 tsl1401_init_state = 0;
uint8 tsl1401_finish_flag;                                                      // TSL1401 数据准备就绪标志位

//-------------------------------------------------------------------------------------------------------------------
// @brief       TSL1401 线阵 TSL1401 数据采集 该函数在 isr.c 中对应 TSL1401_PIT_INDEX 的中断服务函数调用
// @param       void
// @return      void
// Sample usage:            tsl1401_collect_pit_handler();
// note:                    开源库默认使用 TIM8_PIT 那么这个函数默认在 TIM8_UP_IRQHandler 中调用
//-------------------------------------------------------------------------------------------------------------------
void tsl1401_collect_pit_handler (void)
{
    if(!tsl1401_init_state)
        return;

    uint8 i = 0;

    TSL1401_CLK(1);
    TSL1401_SI(0);
    TSL1401_CLK(0);
    TSL1401_SI(1);
    TSL1401_CLK(1);
    TSL1401_SI(0);

    for(i = 0; i < TSL1401_DATA_LEN; i ++)
    {
        TSL1401_CLK(0);
        tsl1401_data[0][i] = adc_convert(TSL1401_AO_PIN);
        tsl1401_data[1][i] = adc_convert(TSL1401_AO_PIN2);
        TSL1401_CLK(1);
    }
    //采集完成标志位置1
    tsl1401_finish_flag = 1;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       TSL1401 线阵 CCD 图像发送至上位机查看图像
// @param       uart_n          串口号
// @param       index           对应接入的哪个 TSL1401 [0-1]
// @return      void            tsl1401_send_data(DEBUG_UART_INDEX, 1);
// Sample usage:                调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
void tsl1401_send_data (uart_index_enum uart_n, uint8 index)
{
    uint8 i;

    uart_putchar(uart_n, 0x00);
    uart_putchar(uart_n, 0xff);
    uart_putchar(uart_n, 0x01);
    uart_putchar(uart_n, 0x00);

    for(i=0; i<TSL1401_DATA_LEN; i++)
    {
        uart_putchar(uart_n, tsl1401_data[index][i] >> 8);                      // 发送高8位
        uart_putchar(uart_n, tsl1401_data[index][i] & 0xFF);                    // 发送高低8位
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       TSL1401 线阵 TSL1401 初始化
// @param       void
// @return      void
// Sample usage:                tsl1401_init();
//-------------------------------------------------------------------------------------------------------------------
void tsl1401_init (void)     
{
    tsl1401_init_state = 1;
    adc_init(TSL1401_AO_PIN, TSL1401_AD_RESOLUTION);
    adc_init(TSL1401_AO_PIN2, TSL1401_AD_RESOLUTION);
    gpio_init(TSL1401_CLK_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
    gpio_init(TSL1401_SI_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
    pit_init_ms(TSL1401_PIT_INDEX, TSL1401_EXPOSURE_TIME);
}

