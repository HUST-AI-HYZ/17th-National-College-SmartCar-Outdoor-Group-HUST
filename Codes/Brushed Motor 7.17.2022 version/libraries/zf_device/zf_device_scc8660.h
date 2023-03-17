/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_scc8660
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
*                   TXD                 查看 zf_device_scc8660.h 中 SCC8660_COF_UART_TX 宏定义
*                   RXD                 查看 zf_device_scc8660.h 中 SCC8660_COF_UART_RX 宏定义
*                   PCLK                查看 zf_device_scc8660.h 中 SCC8660_PCLK_PIN 宏定义
*                   VSY                 查看 zf_device_scc8660.h 中 SCC8660_VSYNC_PIN 宏定义
*                   D0-D7               查看 zf_device_scc8660.h 中 SCC8660_DATA_PIN 宏定义 从该定义开始的连续八个引脚
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   其余引脚悬空
*                   ------------------------------------
********************************************************************************************************************/

#ifndef _zf_device_scc8660_h_
#define _zf_device_scc8660_h_

#include "zf_common_debug.h"

#include "zf_driver_delay.h"
#include "zf_driver_dma.h"
#include "zf_driver_gpio.h"
#include "zf_driver_uart.h"

//--------------------------------------------------------------------------------------------------
// 引脚配置
//--------------------------------------------------------------------------------------------------
#define SCC8660_COF_UART        (UART_8)                                        // 配置摄像头所使用到的串口
#define SCC8660_COF_BAUR        (9600  )                                        // 总钻风配置串口波特率
#define SCC8660_COF_UART_TX     (UART8_RX_E0)                                   // 总钻风 UART-TX 引脚 要接在单片机 RX 上
#define SCC8660_COF_UART_RX     (UART8_TX_E1)                                   // 总钻风 UART-RX 引脚 要接在单片机 TX 上

#define SCC8660_DMA_CH          (DMA1_CHANNEL4)
#define SCC8660_DMA_IRQN        (DMA1_CH4_IRQn)

#define SCC8660_PCLK_PIN        (TIM1_ETR_E7)                                   // PCLK 触发信号 TIM_ETR 引脚禁止随意修改

#define SCC8660_VSYNC_PIN       (E8 )                                           // 场中断引脚
#define SCC8660_VSYNC_IRQN      (EXTI9_5_IRQn)                                  // 中断号

#define SCC8660_DATA_PIN        (F0 )                                           // 数据引脚 这里是 只能是 GPIOx0 或者 GPIOx8 开始 连续八个引脚例如 F0-F7
#define SCC8660_DATA_ADD        (gpio_idr_addr(SCC8660_DATA_PIN))

#define SCC8660_INIT_TIMEOUT    (0x0080)                                        // 默认的摄像头初始化超时时间 毫秒为单位

//--------------------------------------------------------------------------------------------------
// 摄像头默认参数配置 在此修改摄像头配置
//--------------------------------------------------------------------------------------------------
#define SCC8660_W               (160)                                           // 实际图像分辨率宽度 可选参数为：160 180
#define SCC8660_H               (120)                                           // 实际图像分辨率高度 可选参数为：120 160
#define SCC8660_IMAGE_SIZE      (SCC8660_W * 2 * SCC8660_H)                     // 整体图像大小 SCC8660_W*2*SCC8660_H 不能超过 65535

#define SCC8660_AUTO_EXP_DEF    (0  )                                           // 自动曝光     默认不开启自动曝光设置  范围 [0-1] 0为关闭
#define SCC8660_BRIGHT_DEF      (300)                                           // 亮度设置     手动曝光默认：300   手动曝光时：参数范围0-65535   自动曝光推荐值：100 自动曝光时参数设置范围0-255
#define SCC8660_FPS_DEF         (60 )                                           // 图像帧率     默认：50        可选参数为：60 50 30 25。 实际帧率还需要看SCC8660_PCLK_DIV参数的设置
#define SCC8660_PCLK_DIV_DEF    (5  )                                           // PCLK分频系数 默认：5         可选参数为：<0:1/1> <1:2/3> <2:1/2> <3:1/3> <4:1/4> <5:1/8>
                                                                                //              分频系数越大，PCLK频率越低，降低PCLK可以减轻DVP接口的干扰，但降低PCLK频率则会影响帧率。若无特殊需求请保持默认。
                                                                                //              例如设置FPS为50帧，但是pclk分频系数选择的为5，则摄像头输出的帧率为50*（1/8）=6.25帧
                                                                                //              其他参数不变的情况下，SCC8660_PCLK_DIV参数越大图像会越亮
#define SCC8660_PCLK_MODE_DEF   (0  )                                           // PCLK模式     默认：0         可选参数为：[0,1] 0：不输出消隐信号 1：输出消隐信号 <通常都设置为0，如果使用STM32的DCMI接口采集需要设置为1>
#define SCC8660_COLOR_MODE_DEF  (0  )                                           // 图像色彩模式 默认：0         可选参数为：[0,1] 0：正常彩色模式   1：鲜艳模式（色彩饱和度提高）
#define SCC8660_DATA_FORMAT_DEF (0  )                                           // 输出数据格式 默认：0         可选参数为：[0-3] 0：RGB565 1：RGB565(字节交换) 2：YUV422(YUYV) 3：YUV422(UYVY)
#define SCC8660_MANUAL_WB_DEF   (0  )                                           // 手动白平衡   默认：0         可选参数为：[0,0x65-0xa0] 0：关闭手动白平衡，启用自动白平衡    其他：手动白平衡 手动白平衡时 参数范围0x65-0xa0

// 摄像头命令枚举
typedef enum
{
    SCC8660_INIT                = 0x00,                                         // 摄像头初始化命令
    SCC8660_AUTO_EXP,                                                           // 自动曝光命令
    SCC8660_BRIGHT,                                                             // 亮度命令
    SCC8660_FPS,                                                                // 摄像头帧率命令
    SCC8660_SET_COL,                                                            // 图像列命令
    SCC8660_SET_ROW,                                                            // 图像行命令
    SCC8660_PCLK_DIV,                                                           // 像素时钟分频命令
    SCC8660_PCLK_MODE,                                                          // 像素时钟模式命令
    SCC8660_COLOR_MODE,                                                         // 色彩模式命令
    SCC8660_DATA_FORMAT,                                                        // 数据格式命令
    SCC8660_MANUAL_WB,                                                          // 手动白平衡命令
    SCC8660_CONFIG_FINISH,                                                      // 非命令位，主要用来占位计数
        
    SCC8660_GET_WHO_AM_I        = 0xEF,                                         // 我是谁命令，用于判断摄像头型号
    SCC8660_SET_BRIGHT          = 0xF0,                                         // 单独设置亮度
    SCC8660_GET_STATUS          = 0XF1,                                         // 获取摄像头配置命令
    SCC8660_GET_VERSION         = 0xF2,                                         // 固件版本号
    SCC8660_SET_MANUAL_WB       = 0xF3,                                         // 单独设置手动白平衡
        
    SCC8660_SET_REG_ADDR        = 0xFE, 
    SCC8660_SET_REG_DATA        = 0xFF, 
}scc8660_cmd_enum;

extern uint8        scc8660_finish_flag;                                        // 一场图像采集完成标志位
extern uint16       scc8660_image[SCC8660_H][SCC8660_W];

uint16      scc8660_get_id              (void);
uint16      scc8660_get_parameter       (uint16 config);
uint16      scc8660_get_version         (void);
uint8       scc8660_set_bright          (uint16 data);
uint8       scc8660_set_white_balance   (uint16 data);
uint8       scc8660_set_reg             (uint8 addr, uint16 data);

uint8       scc8660_init                (void);

#endif
