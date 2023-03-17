/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_mt9v03x
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
*                   TXD                 查看 zf_device_mt9v03x.h 中 MT9V03X_COF_UART_TX 宏定义
*                   RXD                 查看 zf_device_mt9v03x.h 中 MT9V03X_COF_UART_RX 宏定义
*                   PCLK                查看 zf_device_mt9v03x.h 中 MT9V03X_PCLK_PIN 宏定义
*                   VSY                 查看 zf_device_mt9v03x.h 中 MT9V03X_VSYNC_PIN 宏定义
*                   D0-D7               查看 zf_device_mt9v03x.h 中 MT9V03X_DATA_PIN 宏定义 从该定义开始的连续八个引脚
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   其余引脚悬空
*                   ------------------------------------
********************************************************************************************************************/

#include "zf_device_camera.h"
#include "zf_device_mt9v03x.h"
#include "zf_device_type.h"

uint8 mt9v03x_finish_flag = 0;                                                  // 一场图像采集完成标志位
uint8 mt9v03x_image[MT9V03X_H][MT9V03X_W];

static          uint16    mt9v03x_version = 0x00;

// 需要配置到摄像头的数据 不允许在这修改参数
static int16 mt9v03x_set_confing_buffer[MT9V03X_CONFIG_FINISH][2]=
{
    {MT9V03X_INIT,              0},                                             // 摄像头开始初始化

    {MT9V03X_AUTO_EXP,          MT9V03X_AUTO_EXP_DEF},                          // 自动曝光设置
    {MT9V03X_EXP_TIME,          MT9V03X_EXP_TIME_DEF},                          // 曝光时间
    {MT9V03X_FPS,               MT9V03X_FPS_DEF},                               // 图像帧率
    {MT9V03X_SET_COL,           MT9V03X_W},                                     // 图像列数量
    {MT9V03X_SET_ROW,           MT9V03X_H},                                     // 图像行数量
    {MT9V03X_LR_OFFSET,         MT9V03X_LR_OFFSET_DEF},                         // 图像左右偏移量
    {MT9V03X_UD_OFFSET,         MT9V03X_UD_OFFSET_DEF},                         // 图像上下偏移量
    {MT9V03X_GAIN,              MT9V03X_GAIN_DEF},                              // 图像增益
    {MT9V03X_PCLK_MODE,         MT9V03X_PCLK_MODE_DEF},                         // 像素时钟模式
};

// 从摄像头内部获取到的配置数据 不允许在这修改参数
static int16 mt9v03x_get_confing_buffer[MT9V03X_CONFIG_FINISH - 1][2]=
{
    {MT9V03X_AUTO_EXP,          0},                                             // 自动曝光设置
    {MT9V03X_EXP_TIME,          0},                                             // 曝光时间
    {MT9V03X_FPS,               0},                                             // 图像帧率
    {MT9V03X_SET_COL,           0},                                             // 图像列数量
    {MT9V03X_SET_ROW,           0},                                             // 图像行数量
    {MT9V03X_LR_OFFSET,         0},                                             // 图像左右偏移量
    {MT9V03X_UD_OFFSET,         0},                                             // 图像上下偏移量
    {MT9V03X_GAIN,              0},                                             // 图像增益
    {MT9V03X_PCLK_MODE,         0},                                             // 像素时钟模式命令 PCLK模式 < 仅总钻风 MT9V034 V1.5 以及以上版本支持该命令 >
};

//-------------------------------------------------------------------------------------------------------------------
// @brief       配置摄像头内部配置信息 内部调用
// @param       uartn           选择使用的串口
// @param       buff            发送配置信息的地址
// @return      uint8           1-失败 0-成功
// Sample usage:                调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
static uint8 mt9v03x_set_config (int16 buff[MT9V03X_CONFIG_FINISH][2])
{
    uint8  send_buffer[4];
    uint16 temp;
    int8 i = 0;
    uint16 timeout_count = 0;
    uint8 return_state = 0;

    uart_receive_flag = 0;
    switch(mt9v03x_version)
    {
        case 0x0230:    i = MT9V03X_PCLK_MODE;  break;
        default:        i = MT9V03X_GAIN;       break;
    }
    // 设置参数  具体请参看问题锦集手册
    // 开始配置摄像头并重新初始化
    for(; i >= 0; i --)
    {
        send_buffer[0] = 0xA5;
        send_buffer[1] = buff[i][0];
        temp = buff[i][1];
        send_buffer[2] = temp >> 8;
        send_buffer[3] = (uint8)temp;
        uart_putbuff(MT9V03X_COF_UART, send_buffer, 4);

        system_delay_ms(2);
    }

    while(!uart_receive_flag)                                                   // 等待接受回传数据
    {
        system_delay_ms(1);
        if(timeout_count ++ > MT9V03X_INIT_TIMEOUT)                             // 超时
        {
            return_state = 1;
            break;
        }
    }
    if(0 == return_state && ((0xff != receive[1]) || (0xff != receive[2])))
        return_state = 1;

    // 以上部分对摄像头配置的数据全部都会保存在摄像头上51单片机的eeprom中
    // 利用set_exposure_time函数单独配置的曝光数据不存储在eeprom中
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       获取摄像头内部配置信息 内部调用
// @param       uartn           选择使用的串口
// @param       buff            接收配置信息的地址
// @return      uint8           1-失败 0-成功
// Sample usage:                调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
static uint8 mt9v03x_get_config (int16 buff[MT9V03X_CONFIG_FINISH - 1][2])
{
    uint16 temp;
    uint8  send_buffer[4];
    int8 i = 0;
    uint16 timeout_count = 0;
    uint8 return_state = 0;

    switch(mt9v03x_version)
    {
        case 0x0230:    i = MT9V03X_PCLK_MODE;  break;
        default:        i = MT9V03X_GAIN;       break;
    }
    for(i = i - 1; i >= 1; i --)
    {
        if(mt9v03x_version < 0x0230 && buff[i][0] == MT9V03X_PCLK_MODE)
            continue;
        send_buffer[0] = 0xA5;
        send_buffer[1] = MT9V03X_GET_STATUS;
        temp = buff[i][0];
        send_buffer[2] = temp >> 8;
        send_buffer[3] = (uint8)temp;
        uart_putbuff(MT9V03X_COF_UART, send_buffer, 4);

        timeout_count = 0;
        while(!uart_receive_flag)                                               // 等待接受回传数据
        {
            system_delay_ms(1);
            if(timeout_count ++ > MT9V03X_INIT_TIMEOUT)                         // 超时
            {
                return_state = 1;
                break;
            }
        }
        if(1 == return_state)
            break;

        uart_receive_flag = 0;
        buff[i][1] = receive[1] << 8 | receive[2];
    }
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       获取摄像头固件版本
// @param       void            选择使用的串口
// @return      uint16          0-获取错误 N-版本号
// Sample usage:                mt9v03x_get_version();                          // 调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
uint16 mt9v03x_get_version (void)
{
    uint16 temp;
    uint8  send_buffer[4];
    uint16 timeout_count = 0;
    uint16 return_state = 0xFF;

    send_buffer[0] = 0xA5;
    send_buffer[1] = MT9V03X_GET_STATUS;
    temp = MT9V03X_GET_VERSION;
    send_buffer[2] = temp >> 8;
    send_buffer[3] = (uint8)temp;
    uart_putbuff(MT9V03X_COF_UART, send_buffer, 4);

    while(!uart_receive_flag)                                                   // 等待接受回传数据
    {
        system_delay_ms(1);
        if(timeout_count ++ > MT9V03X_INIT_TIMEOUT)                             // 超时
        {
            return_state = 0;
            break;
        }
    }
    uart_receive_flag = 0;

    return (0xFF == return_state ? ((uint16)(receive[1] << 8) | receive[2]) : 0);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       单独设置摄像头曝光时间
// @param       uartn           选择使用的串口
// @param       light           设置曝光时间越大图像越亮，摄像头收到后会根据分辨率及FPS计算最大曝光时间如果设置的数据过大，那么摄像头将会设置这个最大值
// @return      uint8           1-失败 0-成功
// Sample usage:                mt9v03x_set_exposure_time(100);                 // 调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
uint8 mt9v03x_set_exposure_time (uint16 light)
{
    uint16 temp;
    uint8  send_buffer[4];
    uint16 timeout_count = 0;
    uint8 return_state = 0;

    send_buffer[0] = 0xA5;
    send_buffer[1] = MT9V03X_SET_EXP_TIME;
    temp = light;
    send_buffer[2] = temp >> 8;
    send_buffer[3] = (uint8)temp;
    uart_putbuff(MT9V03X_COF_UART, send_buffer, 4);

    while(!uart_receive_flag)                                                   // 等待接受回传数据
    {
        system_delay_ms(1);
        if(timeout_count ++ > MT9V03X_INIT_TIMEOUT)                             // 超时
        {
            return_state = 1;
            break;
        }
    }
    if(0 == return_state)
    {
        uart_receive_flag = 0;

        temp = receive[1] << 8 | receive[2];

        if(temp != light)
            return_state = 1;
    }
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       对摄像头内部寄存器进行写操作
// @param       uartn           选择使用的串口
// @param       addr            摄像头内部寄存器地址
// @param       data            需要写入的数据
// @return      uint8           1-失败 0-成功
// Sample usage:                mt9v03x_set_reg(addr, data);                    // 调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
uint8 mt9v03x_set_reg (uint8 addr, uint16 data)
{
    uint16 temp;
    uint8  send_buffer[4];
    uint16 timeout_count = 0;
    uint8 return_state = 0;

    send_buffer[0] = 0xA5;
    send_buffer[1] = MT9V03X_SET_ADDR;
    temp = addr;
    send_buffer[2] = temp >> 8;
    send_buffer[3] = (uint8)temp;
    uart_putbuff(MT9V03X_COF_UART, send_buffer, 4);

    system_delay_ms(10);
    send_buffer[0] = 0xA5;
    send_buffer[1] = MT9V03X_SET_DATA;
    temp = data;
    send_buffer[2] = temp >> 8;
    send_buffer[3] = (uint8)temp;
    uart_putbuff(MT9V03X_COF_UART, send_buffer, 4);

    while(!uart_receive_flag)                                                   // 等待接受回传数据
    {
        system_delay_ms(1);
        if(timeout_count ++ > MT9V03X_INIT_TIMEOUT)                             // 超时
        {
            return_state = 1;
            break;
        }
    }
    if(0 == return_state)
    {
        uart_receive_flag = 0;

        temp = receive[1] << 8 | receive[2];
        if(temp != data)
            return_state = 1;
    }
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       MT9V03X摄像头初始化
// @param       void
// @return      uint8           1-失败 0-成功
// Sample usage:                mt9v03x_init();
//-------------------------------------------------------------------------------------------------------------------
uint8 mt9v03x_init (void)
{
    uint8 num;
    uint8 return_state = 0;

    for(num = 0; num < 8; num ++)
    {
        gpio_init((gpio_pin_enum)(MT9V03X_DATA_PIN + num), GPI, GPIO_LOW, GPI_FLOATING_IN);
    }

    // 初始换串口 配置摄像头
    uart_init(MT9V03X_COF_UART, MT9V03X_COF_BAUR, MT9V03X_COF_UART_RX, MT9V03X_COF_UART_TX);
    uart_rx_irq(MT9V03X_COF_UART, 1);
    system_delay_ms(200);

    set_camera_type(CAMERA_GRAYSCALE);                                          // 设置连接摄像头类型

    // 等待摄像头上电初始化成功 方式有两种：延时或者通过获取配置的方式 二选一
    // system_delay_ms(1000);                                                   // 延时方式
    uart_receive_flag = 0;
//    if(mt9v03x_get_config(mt9v03x_get_confing_buffer))
//    {
//        set_camera_type(NO_CAMERE);
//        return 1;
//    }
    mt9v03x_version = mt9v03x_get_version();                                    // 获取配置的方式

    uart_receive_flag = 0;
    if(mt9v03x_set_config(mt9v03x_set_confing_buffer))
    {
        set_camera_type(NO_CAMERE);
        return_state = 1;
        // 那么就是串口通信出错并超时退出了        // 如果程序在输出了断言信息 并且提示出错位置在这里

        // 检查一下接线有没有问题 如果没问题可能就是坏了
        zf_assert(0);
    }

    if(0 == return_state)
    {
        uart_receive_flag = 0;
        // 获取配置便于查看配置是否正确
        if(mt9v03x_get_config(mt9v03x_get_confing_buffer))
        {
            set_camera_type(NO_CAMERE);
            return_state = 1;
        // 如果程序在输出了断言信息 并且提示出错位置在这里
        // 那么就是串口通信出错并超时退出了
        // 检查一下接线有没有问题 如果没问题可能就是坏了
            zf_assert(0);
        }

        if(0 == return_state)
            camera_init(MT9V03X_IMAGE_SIZE, (uint32)MT9V03X_DATA_ADD, (uint32)&mt9v03x_image[0][0]);
    }

    return return_state;
}
