/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_virtual_oscilloscope
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
* @note             
********************************************************************************************************************/

#ifndef _zf_device_virtual_oscilloscope_h_
#define _zf_device_virtual_oscilloscope_h_

#include "zf_common_typedef.h"

extern uint8 virtual_oscilloscope_data[10];

void    virtual_oscilloscope_data_conversion    (const int16 data1, const int16 data2, const int16 data3, const int16 data4);

#endif
