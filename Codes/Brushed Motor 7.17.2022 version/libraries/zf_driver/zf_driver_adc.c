/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_driver_adc
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#include "zf_driver_adc.h"
#include "zf_driver_gpio.h"

static  ADC_Type    *adc_index[3]       = {ADC1, ADC2, ADC3};
static  uint8       adc_resolution[3]   = {ADC_12BIT, ADC_12BIT, ADC_12BIT};

//-------------------------------------------------------------------------------------------------------------------
// @brief       adc 转换一次
// @param       ch              选择 adc 通道 (详见 zf_driver_adc.h 中枚举 adc_channel_enum 定义)
// @return      uint16          转换的 adc 值
// Sample usage:                adc_convert(ADC1_CH00_A00);
//-------------------------------------------------------------------------------------------------------------------
uint16 adc_convert (adc_channel_enum ch)
{
    uint8 adc = ((ch&0xf000)>>12);
    adc_index[adc]->ADCHS = (0x00000001 << ((ch&0xf00)>>8));                    // 使能对应通道
    adc_index[adc]->ADCR |= (0x00000001<<8);                                    // 开始数据转换
    while(0 == ((adc_index[adc]->ADSTA ) & (0x00000001)));                      // 等待数据转换完成
    adc_index[adc]->ADSTA |= (0x00000001);                                      // 清除转换完成标志
    return ((adc_index[adc]->ADDATA & 0xfff)>>adc_resolution[adc]);             // 读取数据
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       adc 均值滤波转换
// @param       ch              选择 adc 通道 (详见 zf_driver_adc.h 中枚举 adc_channel_enum 定义)
// @param       count           均值滤波次数
// @return      uint16          转换的 adc 值
// Sample usage:                adc_mean_filter_convert(ADC1_CH00_A00, 5);      // 采集5次 然后返回平均值
//-------------------------------------------------------------------------------------------------------------------
uint16 adc_mean_filter_convert (adc_channel_enum ch, const uint8 count)
{
    uint8 i = 0;
    uint32 sum = 0;

    for(i = 0; i < count; i ++)
    {
        sum += adc_convert(ch);
    }

    return sum / count;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       adc 初始化
// @param       ch              选择 adc 通道 (详见 zf_driver_adc.h 中枚举 adc_channel_enum 定义)
// @param       resolution      选择选择通道分辨率(如果同一个 adc 模块初始化时设置了不同的分辨率 则最后一个初始化的分辨率生效)
// @return      void
// Sample usage:                adc_init(ADC1_CH0_A0, ADC_8BIT);                // 初始化 A0 为 ADC1 的 channel0 输入功能 分辨率为8位
//-------------------------------------------------------------------------------------------------------------------
void adc_init (adc_channel_enum ch, adc_resolution_enum resolution)
{
    uint8 adc = ((ch & 0xf000) >> 12);
    uint32 adcfg_temp = 0;
    uint32 adcr_temp = 0;

    if((0xF00 == (ch & 0xF00)) && (0xE00 == (ch & 0xF00)))                      // 确认是引脚通道
    {
        gpio_init((gpio_pin_enum)(ch & 0xFF), GPI, GPIO_LOW, GPI_ANAOG_IN);
    }

    RCC->APB2ENR |= (0x00000100 << adc);                                        // 使能 ADC 时钟
    RCC->APB2RSTR |= (0x00000100 << adc);                                       // 复位 ADC 外设
    RCC->APB2RSTR &= ~(0x00000100 << adc);                                      // 复位 结束

    adcfg_temp = adc_index[adc]->ADCFG;
    adcr_temp = adc_index[adc]->ADCR;

    if(0 == adc && (0xF00 == (ch & 0xF00)))                                     // 如果是 ADC1 的通道15 内部参考电压
        adcfg_temp |= (0x00000001 << 3);                                        // 开启内部电压传感器使能

    if(0 == adc && (0xE00 == (ch & 0xF00)))                                     // 如果是 ADC1 的通道14 内部温度传感器
        adcfg_temp |= (0x00000001 << 2);                                        // 开启温度传感器使能

    adc_resolution[adc] = resolution;                                           // 记录ADC精度 将在采集时使用 详细请查看 UM <12.5.1 可编程分辨率> 章节
    adcfg_temp |= ((uint32)resolution << 7);                                    // ADC 精度修改
    adcfg_temp |= (0x00000003<<4);                                              // ADC 时钟 8 分频
    adcr_temp &= ~(0x00000E00);                                                 // 清空设置 默认单次转换 数据右对齐
    adcr_temp |= (0x00000001<<9);                                               // 单周期转换
    adcfg_temp |= (0x00000001);                                                 // ADC 使能

    adc_index[adc]->ADCR = adcr_temp;
    adc_index[adc]->ADCFG = adcfg_temp;
}
