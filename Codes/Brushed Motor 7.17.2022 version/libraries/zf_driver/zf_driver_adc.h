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

#ifndef _zf_driver_adc_h_
#define _zf_driver_adc_h_

#include "hal_device_registers.h"

#include "zf_common_typedef.h"

typedef enum                                                                    // 枚举 ADC 引脚通道  此枚举定义不允许用户修改
{
    ADC1_CH0_A0         = 0x0000,                                               // 0x 0[ADC1] 0[CH00] 00[A0]
    ADC1_CH1_A1         = 0x0101,                                               // 0x 0[ADC1] 1[CH01] 01[A1]
    ADC1_CH2_A2         = 0x0202,                                               // 0x 0[ADC1] 2[CH02] 02[A2]
    ADC1_CH3_A3         = 0x0303,                                               // 0x 0[ADC1] 3[CH03] 03[A3]
    ADC1_CH4_A4         = 0x0404,                                               // 0x 0[ADC1] 4[CH04] 04[A4]
    ADC1_CH5_A5         = 0x0505,                                               // 0x 0[ADC1] 5[CH05] 05[A5]
    ADC1_CH6_A6         = 0x0606,                                               // 0x 0[ADC1] 6[CH06] 06[A6]
    ADC1_CH7_A7         = 0x0707,                                               // 0x 0[ADC1] 7[CH07] 07[A7]
    ADC1_CH8_B0         = 0x0820,                                               // 0x 0[ADC1] 8[CH08] 20[B0]
    ADC1_CH9_B1         = 0x0921,                                               // 0x 0[ADC1] 9[CH09] 21[B1]
    ADC1_CH10_C0        = 0x0A40,                                               // 0x 0[ADC1] A[CH10] 40[C0]
    ADC1_CH11_C1        = 0x0B41,                                               // 0x 0[ADC1] B[CH11] 41[C1]
    ADC1_CH12_C2        = 0x0C42,                                               // 0x 0[ADC1] C[CH12] 42[C2]
    ADC1_CH13_C3        = 0x0D43,                                               // 0x 0[ADC1] D[CH13] 43[C3]

    ADC1_CH14_TEMP      = 0x0EFF,                                               // 0x 0[ADC1] E[CH14] 温度传感器通道
    ADC1_CH15_VS        = 0x0FFF,                                               // 0x 0[ADC1] F[CH15] 内部参考电压通道

    ADC2_CH0_A0         = 0x1000,                                               // 0x 1[ADC2] 0[CH00] 00[A0]
    ADC2_CH1_A1         = 0x1101,                                               // 0x 1[ADC2] 1[CH01] 01[A1]
    ADC2_CH2_A2         = 0x1202,                                               // 0x 1[ADC2] 2[CH02] 02[A2]
    ADC2_CH3_A3         = 0x1303,                                               // 0x 1[ADC2] 3[CH03] 03[A3]
    ADC2_CH4_A4         = 0x1404,                                               // 0x 1[ADC2] 4[CH04] 04[A4]
    ADC2_CH5_A5         = 0x1505,                                               // 0x 1[ADC2] 5[CH05] 05[A5]
    ADC2_CH6_A6         = 0x1606,                                               // 0x 1[ADC2] 6[CH06] 06[A6]
    ADC2_CH7_A7         = 0x1707,                                               // 0x 1[ADC2] 7[CH07] 07[A7]
    ADC2_CH8_B0         = 0x1820,                                               // 0x 1[ADC2] 8[CH08] 20[B0]
    ADC2_CH9_B1         = 0x1921,                                               // 0x 1[ADC2] 9[CH09] 21[B1]
    ADC2_CH10_C0        = 0x1A40,                                               // 0x 1[ADC2] A[CH10] 40[C0]
    ADC2_CH11_C1        = 0x1B41,                                               // 0x 1[ADC2] B[CH11] 41[C1]
    ADC2_CH12_C2        = 0x1C42,                                               // 0x 1[ADC2] C[CH12] 42[C2]
    ADC2_CH13_C3        = 0x1D43,                                               // 0x 1[ADC2] D[CH13] 43[C3]
    ADC2_CH14_C4        = 0x1E44,                                               // 0x 1[ADC2] E[CH14] 44[C4]
    ADC2_CH15_C5        = 0x1F45,                                               // 0x 1[ADC2] F[CH15] 45[C5]

    ADC3_CH0_A0         = 0x2000,                                               // 0x 2[ADC3] 0[CH00] 00[A0]
    ADC3_CH1_A1         = 0x2101,                                               // 0x 2[ADC3] 1[CH01] 01[A1]
    ADC3_CH2_A2         = 0x2202,                                               // 0x 2[ADC3] 2[CH02] 02[A2]
    ADC3_CH3_A3         = 0x2303,                                               // 0x 2[ADC3] 3[CH03] 03[A3]
    ADC3_CH4_F6         = 0x24A6,                                               // 0x 2[ADC3] 4[CH04] A6[F6]
    ADC3_CH5_F7         = 0x25A7,                                               // 0x 2[ADC3] 5[CH05] A7[F7]
    ADC3_CH6_F8         = 0x26A8,                                               // 0x 2[ADC3] 6[CH06] A8[F8]
    ADC3_CH7_F9         = 0x27A9,                                               // 0x 2[ADC3] 7[CH07] A9[F9]
    ADC3_CH8_F10        = 0x28AA,                                               // 0x 2[ADC3] 8[CH08] AA[F10]
    ADC3_CH9_B1         = 0x2921,                                               // 0x 2[ADC3] 9[CH09] 21[B1]
    ADC3_CH10_C0        = 0x2A40,                                               // 0x 2[ADC3] A[CH10] 40[C0]
    ADC3_CH11_C1        = 0x2B41,                                               // 0x 2[ADC3] B[CH11] 41[C1]
    ADC3_CH12_C2        = 0x2C42,                                               // 0x 2[ADC3] C[CH12] 42[C2]
    ADC3_CH13_C3        = 0x2D43,                                               // 0x 2[ADC3] D[CH13] 43[C3]
    ADC3_CH14_C4        = 0x2E44,                                               // 0x 2[ADC3] E[CH14] 44[C4]
    ADC3_CH15_C5        = 0x2F45,                                               // 0x 2[ADC3] F[CH15] 45[C5]
}adc_channel_enum;

typedef enum                                                                    // 枚举 ADC 分辨率   此枚举定义不允许用户修改
{
    ADC_12BIT,                                                                  // 12位分辨率
    ADC_11BIT,                                                                  // 11位分辨率
    ADC_10BIT,                                                                  // 10位分辨率
    ADC_9BIT,                                                                   // 9位分辨率
    ADC_8BIT,                                                                   // 8位分辨率
}adc_resolution_enum;

uint16      adc_convert             (adc_channel_enum ch);
uint16      adc_mean_filter_convert (adc_channel_enum ch, const uint8 count);
void        adc_init                (adc_channel_enum ch, adc_resolution_enum resolution);

#endif
