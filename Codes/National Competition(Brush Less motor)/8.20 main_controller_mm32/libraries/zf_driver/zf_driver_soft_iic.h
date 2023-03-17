/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_driver_soft_iic
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#ifndef _zf_driver_soft_iic_h_
#define _zf_driver_soft_iic_h_

#include "zf_common_debug.h"

#include "zf_driver_gpio.h"

typedef struct
{
    gpio_pin_enum       scl_pin;                                                // 用于记录对应的引脚编号
    gpio_pin_enum       sda_pin;                                                // 用于记录对应的引脚编号
    uint8               addr;                                                   // 器件地址 七位地址模式
    uint32              delay;                                                  // 模拟 IIC 软延时时长
}soft_iic_info_struct;

void        soft_iic_write_8bit             (soft_iic_info_struct *soft_iic_obj, const uint8 data);
void        soft_iic_write_8bit_array       (soft_iic_info_struct *soft_iic_obj, const uint8 *data, uint32 len);

void        soft_iic_write_16bit            (soft_iic_info_struct *soft_iic_obj, const uint16 data);
void        soft_iic_write_16bit_array      (soft_iic_info_struct *soft_iic_obj, const uint16 *data, uint32 len);

void        soft_iic_write_8bit_register    (soft_iic_info_struct *soft_iic_obj, const uint8 register_name, const uint8 data);
void        soft_iic_write_8bit_registers   (soft_iic_info_struct *soft_iic_obj, const uint8 register_name, const uint8 *data, uint32 len);

void        soft_iic_write_16bit_register   (soft_iic_info_struct *soft_iic_obj, const uint16 register_name, const uint16 data);
void        soft_iic_write_16bit_registers  (soft_iic_info_struct *soft_iic_obj, const uint16 register_name, const uint16 *data, uint32 len);

uint8       soft_iic_read_8bit              (soft_iic_info_struct *soft_iic_obj);
void        soft_iic_read_8bit_array        (soft_iic_info_struct *soft_iic_obj, uint8 *data, uint32 len);

uint16      soft_iic_read_16bit             (soft_iic_info_struct *soft_iic_obj);
void        soft_iic_read_16bit_array       (soft_iic_info_struct *soft_iic_obj, uint16 *data, uint32 len);

uint8       soft_iic_read_8bit_register     (soft_iic_info_struct *soft_iic_obj, const uint8 register_name);
void        soft_iic_read_8bit_registers    (soft_iic_info_struct *soft_iic_obj, const uint8 register_name, uint8 *data, uint32 len);

uint16      soft_iic_read_16bit_register    (soft_iic_info_struct *soft_iic_obj, const uint16 register_name);
void        soft_iic_read_16bit_registers   (soft_iic_info_struct *soft_iic_obj, const uint16 register_name, uint16 *data, uint32 len);

void        soft_iic_sccb_write_register    (soft_iic_info_struct *soft_iic_obj, const uint8 register_name, uint8 data);
uint8       soft_iic_sccb_read_register     (soft_iic_info_struct *soft_iic_obj, const uint8 register_name);

void        soft_iic_init                   (soft_iic_info_struct *soft_iic_obj, uint8 addr, uint32 delay, gpio_pin_enum scl_pin, gpio_pin_enum sda_pin);

#endif

