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

#include "zf_device_virtual_oscilloscope.h"

uint8 virtual_oscilloscope_data[10];

//-------------------------------------------------------------------------------------------------------------------
// @brief       CRC 校验 内部使用 用户无需关心
// @param       buff            需要进行 CRC 计算的数据地址
// @param       crc_cnt         需要进行 CRC 计算的数据个数
// @return      uint16          CRC 校验结果
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
static uint16 crc_check (uint8 *buff, uint8 crc_cnt)
{
    uint16 crc_temp;
    uint8 i, j;
    crc_temp = 0xffff;

    for(i = 0; i < crc_cnt; i ++)
    {
    crc_temp ^= buff[i];
    for(j = 0; j < 8; j ++)
    {
    if (crc_temp & 0x01)
    crc_temp = (crc_temp >> 1) ^ 0xa001;
    else
    crc_temp = crc_temp >> 1;
    }
    }
    return(crc_temp);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       虚拟示波器数据转换函数
// @param       data1           要发送的第一个数据
// @param       data2           要发送的第二个数据
// @param       data3           要发送的第三个数据
// @param       data4           要发送的第四个数据
// @return      void
// Sample usage:                uint8 data_buffer[10];
//                              virtual_oscilloscope_data_conversion(100, 200, 300, 400, data_buffer);
//                              wireless_uart_send_buff(data_buffer, 10);
//-------------------------------------------------------------------------------------------------------------------
void virtual_oscilloscope_data_conversion (const int16 data1, const int16 data2, const int16 data3, const int16 data4)
{
    uint16 crc_16 = 0;

    virtual_oscilloscope_data[0] = (uint8)((uint16)data1 & 0xff);
    virtual_oscilloscope_data[1] = (uint8)((uint16)data1 >> 8);

    virtual_oscilloscope_data[2] = (uint8)((uint16)data2 & 0xff);
    virtual_oscilloscope_data[3] = (uint8)((uint16)data2 >> 8);

    virtual_oscilloscope_data[4] = (uint8)((uint16)data3 & 0xff);
    virtual_oscilloscope_data[5] = (uint8)((uint16)data3>>8);

    virtual_oscilloscope_data[6] = (uint8)((uint16)data4 & 0xff);
    virtual_oscilloscope_data[7] = (uint8)((uint16)data4 >> 8);

    crc_16  = crc_check(virtual_oscilloscope_data, 8);
    virtual_oscilloscope_data[8] = (uint8)(crc_16 & 0xff);
    virtual_oscilloscope_data[9] = (uint8)(crc_16 >> 8);
}
