/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_common_headfile
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#ifndef _zf_common_headfile_h_
#define _zf_common_headfile_h_

#include "stdio.h"
#include "stdint.h"
#include "string.h"

//===================================================芯片 SDK 底层===================================================
#include "hal_device_registers.h"
#include "hal_common.h"
#include "hal_adc.h"
#include "hal_comp.h"
#include "hal_dac.h"
#include "hal_dma.h"
#include "hal_dma_request.h"
#include "hal_exti.h"
#include "hal_fsmc.h"
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "hal_iwdg.h"
#include "hal_rcc.h"
#include "hal_rtc.h"
#include "hal_sdio.h"
#include "hal_spi.h"
#include "hal_syscfg.h"
#include "hal_tim_16b.h"
#include "hal_tim_32b.h"
#include "hal_tim_adv.h"
#include "hal_tim_basic.h"
#include "hal_uart.h"
#include "hal_usb.h"
#include "hal_usb_bdt.h"
#include "hal_wwdg.h"
//===================================================芯片 SDK 底层===================================================

//====================================================开源库公共层====================================================
#include "zf_common_typedef.h"
#include "zf_common_clock.h"
#include "zf_common_debug.h"
#include "zf_common_fifo.h"
#include "zf_common_font.h"
#include "zf_common_function.h"
#include "zf_common_interrupt.h"
//====================================================开源库公共层====================================================

//===================================================芯片外设驱动层===================================================
#include "zf_driver_adc.h"
#include "zf_driver_delay.h"
#include "zf_driver_encoder.h"
#include "zf_driver_exti.h"
#include "zf_driver_flash.h"
#include "zf_driver_gpio.h"
#include "zf_device_gps_tau1201.h"
#include "zf_driver_iic.h"
#include "zf_driver_pit.h"
#include "zf_driver_pwm.h"
#include "zf_driver_sdio.h"
#include "zf_driver_soft_iic.h"
#include "zf_driver_soft_spi.h"
#include "zf_driver_spi.h"
#include "zf_driver_timer.h"
#include "zf_driver_uart.h"
//===================================================芯片外设驱动层===================================================

//===================================================外接设备驱动层===================================================
#include "zf_device_absolute_encoder.h"
#include "zf_device_bluetooth_ch9141.h"
#include "zf_device_camera.h"
#include "zf_device_camera.h"
#include "zf_device_icm20602.h"
#include "zf_device_imu963ra.h"
#include "zf_device_ips114.h"
#include "zf_device_ips200.h"
#include "zf_device_key.h"
#include "zf_device_mpu6050.h"
#include "zf_device_mt9v03x.h"
#include "zf_device_oled.h"
#include "zf_device_ov7725.h"
#include "zf_device_scc8660.h"
#include "zf_device_sdcard.h"
#include "zf_device_tft180.h"
#include "zf_device_tsl1401.h"
#include "zf_device_type.h"
#include "zf_device_virtual_oscilloscope.h"
#include "zf_device_wireless_uart.h"
//===================================================外接设备驱动层===================================================

//===================================================应用组件层===================================================
#include "ffconf.h"
#include "ff.h"
//===================================================应用组件层===================================================

//===================================================用户自定义文件===================================================

//===================================================用户自定义文件===================================================

#endif
