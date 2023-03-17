/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_driver_sdio
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#ifndef _zf_driver_sdio_h_
#define _zf_driver_sdio_h_

#include "hal_device_registers.h"
#include "hal_sdio.h"

#include "zf_common_typedef.h"
#include "zf_common_clock.h"

#define SDIO_TIMEOUT_COUNT          (100)

typedef enum                                                                    // 枚举 SDIO 工作状态 此枚举定义不允许用户修改
{
    SDIO_ERROR_STATE_NO_ERROR,
    SDIO_ERROR_STATE_TIME_OUT,
}sdio_error_state_enum;

typedef enum                                                                    // 枚举 SDIO 数据总线位宽   此枚举定义不允许用户修改
{
    SDIO_DATA_BUS_1BIT,                                                         // 1bit 数据总线位宽
    SDIO_DATA_BUS_4BIT,                                                         // 4bit 数据总线位宽
}sdio_data_bus_width_enum;

typedef enum                                                                    // SD 卡响应信息类型 此结构体定义不允许用户修改
{
    SDIO_RESP_TYPE_R1,                                                          // 32 bit + 8 bit CRC
    SDIO_RESP_TYPE_R2,                                                          // 128 bit + 8 bit CRC, for CID and CSD
    SDIO_RESP_TYPE_R3,                                                          // 32 bit + 8 bit CRC, for OCR
    SDIO_RESP_TYPE_R6,                                                          // 32 bit + 8 bit CRC, for RCA
    SDIO_RESP_TYPE_R7,                                                          // 32 bit + 8 bit CRC, for card interface condition
}sdio_resp_type_enum;


#define sdio_switch_fifo_dir(x)             ((x) ? (SDIO->BUFCTL |= 0x00000800) : (SDIO->BUFCTL &= 0xFFFFF7FF))
#define sdio_switch_fifo_read_wait(x)       ((x) ? (SDIO->MMCCTRL |= 0x00000400) : (SDIO->MMCCTRL &= 0xFFFFFBFF))
#define sdio_clear_fifo()                   (SDIO->BUFCTL |= 0x00008000)
#define sdio_set_block_count(x)             (SDIO->MMCBLOCKCNT = (x))

#define sdio_set_fifo_data(x)               (SDIO->DATABUF[0] = (x))
#define sdio_get_fifo_data()                (SDIO->DATABUF[0])

uint32                  sdio_get_state              (void);
void                    sdio_clear_state            (uint32 flags);
sdio_error_state_enum   sdio_execute_cmd            (uint8 cmd_index, uint32 param, uint32 flags);
void                    sdio_execute_data           (uint32 data);
sdio_error_state_enum   sdio_request_resp           (sdio_resp_type_enum type, uint32 *resp);

void                    sdio_set_data_bus_width     (sdio_data_bus_width_enum width);
void                    sdio_disable                (void);
void                    sdio_enable                 (void);
void                    sdio_init                   (void);

#endif
