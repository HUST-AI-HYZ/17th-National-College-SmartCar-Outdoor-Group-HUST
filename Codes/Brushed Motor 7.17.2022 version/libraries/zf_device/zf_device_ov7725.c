/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_ov7725
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
*                   TXD/SCL             查看 zf_device_ov7725.h 中 OV7725_COF_UART_TX 或 OV7725_COF_IIC_SCL 宏定义
*                   RXD/SDA             查看 zf_device_ov7725.h 中 OV7725_COF_UART_RX 或 OV7725_COF_IIC_SDA 宏定义
*                   PCLK                查看 zf_device_ov7725.h 中 OV7725_PCLK_PIN 宏定义
*                   VSY                 查看 zf_device_ov7725.h 中 OV7725_VSYNC_PIN 宏定义
*                   D0-D7               查看 zf_device_ov7725.h 中 OV7725_DATA_PIN 宏定义 从该定义开始的连续八个引脚
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   其余引脚悬空
*                   ------------------------------------
********************************************************************************************************************/

#include "zf_device_camera.h"
#include "zf_device_ov7725.h"
#include "zf_device_type.h"

uint8 ov7725_finish_flag = 0;
uint8 ov7725_image_binary [OV7725_H][OV7725_W / 8];

// 需要配置到摄像头的数据 不允许在这修改参数
static uint16 ov7725_set_confing_buffer [OV7725_CONFIG_FINISH][2]=
{
    {OV7725_INIT,           0},                                                 // 初始化命令

    {OV7725_RESERVE,        0},                                                 // 保留
    {OV7725_CONTRAST,       OV7725_CONTRAST_DEF},                               // 阈值设置
    {OV7725_FPS,            OV7725_FPS_DEF},                                    // 帧率
    {OV7725_COL,            OV7725_W},                                          // 图像宽度
    {OV7725_ROW,            OV7725_H}                                           // 图像高度
};

// 从摄像头内部获取到的配置数据 不允许在这修改参数
static uint16 ov7725_get_confing_buffer [OV7725_CONFIG_FINISH - 1][2]=
{
    {OV7725_RESERVE,        0},                                                 // 保留
    {OV7725_CONTRAST,       0},                                                 // 阈值设置
    {OV7725_FPS,            0},                                                 // 帧率
    {OV7725_COL,            0},                                                 // 图像宽度
    {OV7725_ROW,            0}                                                  // 图像高度
};

//-------------------------------------------------------------------------------------------------------------------
// @brief       配置摄像头内部配置信息 内部调用
// @param       uartn           选择使用的串口
// @param       buff            发送配置信息的地址
// @return      uint8           1-失败 0-成功
// Sample usage:                调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
static uint8 ov7725_set_config (uint16 buff[OV7725_CONFIG_FINISH][2])
{
    int16 i = 0;
    uint16 temp = 0;
    uint8  send_buffer[4];
    uint16 timeout_count = 0;
    uint8 return_state = 0;

    uart_receive_flag = 0;

    for(i = OV7725_ROW; i >= 0; i --)
    {
        send_buffer[0] = 0xA5;
        send_buffer[1] = buff[i][0];
        temp           = buff[i][1];
        send_buffer[2] = temp >> 8;
        send_buffer[3] = (uint8)temp;

        uart_putbuff(OV7725_COF_UART, send_buffer, 4);
        system_delay_ms(10);
    }

    while(!uart_receive_flag)                                                   // 等待接受回传数据
    {
        system_delay_ms(1);
        if(timeout_count ++ > OV7725_INIT_TIMEOUT)                              // 超时
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
static uint8 ov7725_get_config (uint16 buff[OV7725_CONFIG_FINISH - 1][2])
{
    uint16 temp;
    int8 i;
    uint8  send_buffer[4];
    uint16 timeout_count = 0;
    uint8 return_state = 0;

    for(i = OV7725_ROW - 1; i >= 1; i --)
    {
        send_buffer[0] = 0xA5;
        send_buffer[1] = OV7725_GET_STATUS;
        temp           = buff[i][0];
        send_buffer[2] = temp >> 8;
        send_buffer[3] = (uint8)temp;

        uart_putbuff(OV7725_COF_UART, send_buffer, 4);

        system_delay_ms(10);

        timeout_count = 0;
        while(!uart_receive_flag)                                               // 等待接受回传数据
        {
            system_delay_ms(1);
            if(timeout_count ++ > OV7725_INIT_TIMEOUT)                          // 超时
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
// @brief       小钻风摄像头内部寄存器初始化(内部使用，用户无需调用)
// @param       void
// @return      uint8           1-失败 0-成功
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
static uint8 ov7725_init_iic (void)
{
    soft_iic_info_struct ov7725_iic_struct;
    uint8 ov7725_idcode = 0;
    uint8 return_state = 0;

    uart_rx_irq(OV7725_COF_UART, 0);

    soft_iic_init(&ov7725_iic_struct, OV7725_DEV_ADD, OV7725_COF_IIC_DELAY, OV7725_COF_IIC_SCL, OV7725_COF_IIC_SDA);

    soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_COM7, 0x80);        // 复位摄像头
    system_delay_ms(50);

    do{
        ov7725_idcode = soft_iic_sccb_read_register(&ov7725_iic_struct, OV7725_VER);
        if( ov7725_idcode != OV7725_ID )
        {
            return_state = 1;                                                   // 校验摄像头ID号
            break;
        }

        // ID号确认无误   然后配置寄存器
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_COM4         , 0xC1);  
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_CLKRC        , 0x01);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_COM2         , 0x03);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_COM3         , 0xD0);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_COM7         , 0x40);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_COM8         , 0xCE);   // 0xCE:关闭自动曝光  0xCF：开启自动曝光
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_HSTART       , 0x3F);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_HSIZE        , 0x50);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_VSTRT        , 0x03);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_VSIZE        , 0x78);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_HREF         , 0x00);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_SCAL0        , 0x0A);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_AWB_Ctrl0    , 0xE0);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_DSPAuto      , 0xff);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_DSP_Ctrl2    , 0x0C);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_DSP_Ctrl3    , 0x00);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_DSP_Ctrl4    , 0x00);

        if(OV7725_W == 80)          soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_HOutSize, 0x14);
        else if(OV7725_W == 160)    soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_HOutSize, 0x28);
        else if(OV7725_W == 240)    soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_HOutSize, 0x3c);
        else if(OV7725_W == 320)    soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_HOutSize, 0x50);

        if(OV7725_H == 60)          soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_VOutSize, 0x1E);
        else if(OV7725_H == 120)    soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_VOutSize, 0x3c);
        else if(OV7725_H == 180)    soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_VOutSize, 0x5a);
        else if(OV7725_H == 240)    soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_VOutSize, 0x78);

        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_REG28        , 0x01);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_EXHCH        , 0x10);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_EXHCL        , 0x1F);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_GAM1         , 0x0c);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_GAM2         , 0x16);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_GAM3         , 0x2a);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_GAM4         , 0x4e);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_GAM5         , 0x61);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_GAM6         , 0x6f);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_GAM7         , 0x7b);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_GAM8         , 0x86);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_GAM9         , 0x8e);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_GAM10        , 0x97);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_GAM11        , 0xa4);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_GAM12        , 0xaf);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_GAM13        , 0xc5);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_GAM14        , 0xd7);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_GAM15        , 0xe8);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_SLOP         , 0x20);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_LC_RADI      , 0x00);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_LC_COEF      , 0x13);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_LC_XC        , 0x08);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_LC_COEFB     , 0x14);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_LC_COEFR     , 0x17);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_LC_CTR       , 0x05);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_BDBase       , 0x99);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_BDMStep      , 0x03);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_SDE          , 0x04);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_BRIGHT       , 0x00);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_CNST         , 0x40);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_SIGN         , 0x06);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_UVADJ0       , 0x11);
        soft_iic_sccb_write_register(&ov7725_iic_struct, OV7725_UVADJ1       , 0x02);
    }while(0);
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       获取摄像头固件 ID
// @param       void
// @return      void
// Sample usage:                ov7725_uart_get_id();                           // 调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
uint16 ov7725_uart_get_id (void)
{
    uint16 temp;
    uint8  send_buffer[4];
    uint16 timeout_count = 0;

    send_buffer[0] = 0xA5;
    send_buffer[1] = OV7725_GET_WHO_AM_I;
    temp           = 0;
    send_buffer[2] = temp >> 8;
    send_buffer[3] = (uint8)temp;

    uart_putbuff(OV7725_COF_UART, send_buffer, 4);

    while(!uart_receive_flag)                                                   // 等待接受回传数据
    {
        system_delay_ms(1);
        if(timeout_count ++ > OV7725_INIT_TIMEOUT)                              // 超时
            return 0;
    }
    uart_receive_flag = 0;

    return ((uint16)(receive[1] << 8) | receive[2]);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       获取摄像头固件版本
// @param       uartn           选择使用的串口
// @return      void
// Sample usage:                ov7725_get_version();                           // 调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
uint16 ov7725_get_version (void)
{
    uint16 temp;
    uint8  send_buffer[4];
    uint16 timeout_count = 0;
    uint16 return_state = 0xFF;

    send_buffer[0] = 0xA5;
    send_buffer[1] = OV7725_GET_STATUS;
    temp           = OV7725_GET_VERSION;
    send_buffer[2] = temp >> 8;
    send_buffer[3] = (uint8)temp;

    uart_putbuff(OV7725_COF_UART, send_buffer, 4);

    while(!uart_receive_flag)                                                   // 等待接受回传数据
    {
        system_delay_ms(1);
        if(timeout_count ++ > OV7725_INIT_TIMEOUT)                              // 超时
        {
            return_state = 0;
            break;
        }
    }
    uart_receive_flag = 0;

    return (0xFF == return_state ? ((uint16)(receive[1] << 8) | receive[2]) : 0);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ov7725 摄像头初始化
// @param       NULL
// @return      void
// Sample usage:                ov7725_init();
//-------------------------------------------------------------------------------------------------------------------
uint8 ov7725_init (void)
{
    uint8 num = 0;
    uint16 timeout_count = 0;
    uint8 return_state = 0;

    gpio_init(OV7725_VSYNC_PIN, GPI, GPIO_LOW, GPI_FLOATING_IN);
    while(!num)
    {
        num = gpio_get(OV7725_VSYNC_PIN);
        system_delay_ms(1);
        if(timeout_count ++ > OV7725_INIT_TIMEOUT)                              // 超时
        {
            return_state = 1;
            zf_assert(0);
            // 如果程序在输出了断言信息 并且提示出错位置在这里
            // 那么就是 场信号电平异常
            // 检查一下接线有没有问题 如果没问题可能就是坏了
            break;
        }
    }

    if(0 == return_state)
    {
        for(num = 0; num < 8; num ++)
        {
            gpio_init((gpio_pin_enum)(OV7725_DATA_PIN + num), GPI, GPIO_LOW, GPI_FLOATING_IN);
        }

        uart_init(OV7725_COF_UART, OV7725_COF_BAUR, OV7725_COF_UART_RX, OV7725_COF_UART_TX);
        uart_rx_irq(OV7725_COF_UART, 1);
        system_delay_ms(200);

        set_camera_type(CAMERA_BIN_UART);                                           // 设置连接摄像头类型

        // 获取所有参数
        uart_receive_flag = 0;
        if(ov7725_get_config(ov7725_get_confing_buffer))
        {
            set_camera_type(CAMERA_BIN_IIC);                                        // 设置连接摄像头类型
            if(ov7725_init_iic())
            {
                set_camera_type(NO_CAMERE);
                return_state = 1;
                // 如果程序在输出了断言信息 并且提示出错位置在这里
                // 那么就是 IIC 出错并超时退出了
                // 检查一下接线有没有问题 如果没问题可能就是坏了
                zf_assert(0);
            }
        }
        else
        {
            // 设置所有参数
            uart_receive_flag = 0;
            if(ov7725_set_config(ov7725_set_confing_buffer))
            {
                set_camera_type(NO_CAMERE);
                return_state = 1;
                // 如果程序在输出了断言信息 并且提示出错位置在这里
                // 那么就是串口通信出错并超时退出了
                // 检查一下接线有没有问题 如果没问题可能就是坏了
                zf_assert(0);
            }

            // 获取所有参数
            uart_receive_flag = 0;
            if(ov7725_get_config(ov7725_get_confing_buffer))
            {
                set_camera_type(NO_CAMERE);
                return_state = 1;
                // 如果程序在输出了断言信息 并且提示出错位置在这里
                // 那么就是串口通信出错并超时退出了
                // 检查一下接线有没有问题 如果没问题可能就是坏了
                zf_assert(0);
            }
        }
    }

    if(0 == return_state)
        camera_init(OV7725_IMAGE_SIZE, (uint32)OV7725_DATA_ADD, (uint32)&ov7725_image_binary[0][0]);

    return return_state;
}
