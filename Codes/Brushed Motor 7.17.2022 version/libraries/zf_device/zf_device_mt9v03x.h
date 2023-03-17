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

#ifndef _zf_device_mt9v03x_h_
#define _zf_device_mt9v03x_h_

#include "zf_common_debug.h"

#include "zf_driver_delay.h"
#include "zf_driver_dma.h"
#include "zf_driver_gpio.h"
#include "zf_driver_uart.h"

//--------------------------------------------------------------------------------------------------
// 引脚配置
//--------------------------------------------------------------------------------------------------
#define MT9V03X_COF_UART        (UART_8     )                                   // 配置摄像头所使用到的串口
#define MT9V03X_COF_BAUR        (9600       )                                   // 总钻风配置串口波特率
#define MT9V03X_COF_UART_TX     (UART8_RX_E0)                                   // 总钻风 UART-TX 引脚 要接在单片机 RX 上
#define MT9V03X_COF_UART_RX     (UART8_TX_E1)                                   // 总钻风 UART-RX 引脚 要接在单片机 TX 上

#define MT9V03X_DMA_CH          (DMA1_CHANNEL4)
#define MT9V03X_DMA_IRQN        (DMA1_CH4_IRQn)

#define MT9V03X_PCLK_PIN        (TIM1_ETR_E7)                                   // PCLK 触发信号 TIM_ETR 引脚禁止随意修改

#define MT9V03X_VSYNC_PIN       (E8)                                            // 场中断引脚
#define MT9V03X_VSYNC_IRQN      (EXTI9_5_IRQn)                                  // 中断号

#define MT9V03X_DATA_PIN        (F0)                                            // 数据引脚 这里是 只能是 GPIOx0 或者 GPIOx8 开始 连续八个引脚例如 F0-F7
#define MT9V03X_DATA_ADD        (gpio_idr_addr(MT9V03X_DATA_PIN))

#define MT9V03X_INIT_TIMEOUT    (0x0080)                                        // 默认的摄像头初始化超时时间 毫秒为单位

//--------------------------------------------------------------------------------------------------
// 摄像头默认参数配置 在此修改摄像头配置
//--------------------------------------------------------------------------------------------------
#define MT9V03X_W               (188)                                           // 图像宽度     范围 [1-752]
#define MT9V03X_H               (120)                                           // 图像高度     范围 [1-480]
#define MT9V03X_IMAGE_SIZE      (MT9V03X_W * MT9V03X_H)                         // 整体图像大小不能超过 65535

#define MT9V03X_AUTO_EXP_DEF    (0  )                                           // 自动曝光设置     默认不开启自动曝光设置  范围 [0-63] 0为关闭
                                                                                //                  如果自动曝光开启  EXP_TIME命令设置自动曝光时间的上限
                                                                                //                  一般情况是不需要开启自动曝光设置 如果遇到光线非常不均匀的情况可以尝试设置自动曝光，增加图像稳定性
#define MT9V03X_EXP_TIME_DEF    (512)                                           // 曝光时间         摄像头收到后会自动计算出最大曝光时间，如果设置过大则设置为计算出来的最大曝光值
#define MT9V03X_FPS_DEF         (50 )                                           // 图像帧率         摄像头收到后会自动计算出最大FPS，如果过大则设置为计算出来的最大FPS
#define MT9V03X_LR_OFFSET_DEF   (0  )                                           // 图像左右偏移量   正值 右偏移   负值 左偏移  列为188 376 752时无法设置偏移
                                                                                //                  摄像头收偏移数据后会自动计算最大偏移，如果超出则设置计算出来的最大偏移
#define MT9V03X_UD_OFFSET_DEF   (0  )                                           // 图像上下偏移量   正值 上偏移   负值 下偏移  行为120 240 480时无法设置偏移
                                                                                //                  摄像头收偏移数据后会自动计算最大偏移，如果超出则设置计算出来的最大偏移
#define MT9V03X_GAIN_DEF        (32 )                                           // 图像增益         范围 [16-64]  增益可以在曝光时间固定的情况下改变图像亮暗程度
#define MT9V03X_PCLK_MODE_DEF   (0  )                                           // 像素时钟模式     范围 [0-1]    默认：0 可选参数为：[0：不输出消隐信号,1：输出消隐信号]
                                                                                //                  通常都设置为0，如果使用CH32V307的DVP接口或STM32的DCMI接口采集需要设置为1
                                                                                //                  仅总钻风 MT9V034 V1.5 以及以上版本支持该命令

// 摄像头命令枚举
typedef enum
{
    MT9V03X_INIT = 0,                                                           // 摄像头初始化命令
    MT9V03X_AUTO_EXP,                                                           // 自动曝光命令
    MT9V03X_EXP_TIME,                                                           // 曝光时间命令
    MT9V03X_FPS,                                                                // 摄像头帧率命令
    MT9V03X_SET_COL,                                                            // 图像列命令
    MT9V03X_SET_ROW,                                                            // 图像行命令
    MT9V03X_LR_OFFSET,                                                          // 图像左右偏移命令
    MT9V03X_UD_OFFSET,                                                          // 图像上下偏移命令
    MT9V03X_GAIN,                                                               // 图像偏移命令
    MT9V03X_PCLK_MODE,                                                          // 像素时钟模式命令(仅总钻风MT9V034 V1.5以及以上版本支持该命令)
    MT9V03X_CONFIG_FINISH,                                                      // 非命令位，主要用来占位计数

    MT9V03X_COLOR_GET_WHO_AM_I = 0xEF,
    MT9V03X_SET_EXP_TIME = 0XF0,                                                // 单独设置曝光时间命令
    MT9V03X_GET_STATUS,                                                         // 获取摄像头配置命令
    MT9V03X_GET_VERSION,                                                        // 固件版本号命令

    MT9V03X_SET_ADDR = 0XFE,                                                    // 寄存器地址命令
    MT9V03X_SET_DATA                                                            // 寄存器数据命令
}m9v03x_cmd_enum;

extern uint8    mt9v03x_finish_flag;                                            // 一场图像采集完成标志位
extern uint8    mt9v03x_image[MT9V03X_H][MT9V03X_W];

uint16      mt9v03x_get_version         (void);
uint8       mt9v03x_set_exposure_time   (uint16 light);
uint8       mt9v03x_set_reg             (uint8 addr, uint16 data);

uint8       mt9v03x_init                (void);

#endif
