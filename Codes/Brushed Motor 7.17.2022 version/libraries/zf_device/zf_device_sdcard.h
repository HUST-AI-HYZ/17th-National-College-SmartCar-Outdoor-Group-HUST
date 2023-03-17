/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_sdcard
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
* @note             
********************************************************************************************************************/

#ifndef _zf_device_sdcard_h_
#define _zf_device_sdcard_h_

#include "zf_common_clock.h"
#include "zf_common_debug.h"
#include "zf_common_function.h"

#include "zf_driver_delay.h"
#include "zf_driver_sdio.h"
//512B一个块，4B一个float
#define SDCARD_BLOCK_SIZE           (512)
#define SDCARD_TIMEOUT_COUNT        (100)

typedef enum                                                                    // 枚举 SD 卡工作状态 此枚举定义不允许用户修改
{
    SDCARD_ERROR_STATE_NO_ERROR,
    SDCARD_ERROR_STATE_TIME_OUT,
}sdcard_error_code_enum;

typedef enum                                                                    // 枚举 SD 卡类型   此枚举定义不允许用户修改
{
	SDCARD_TYPE_SDSC,                                                           // 标准容量 SD 卡
    SDCARD_TYPE_SDHC,                                                           // 高容量 SD 卡
    SDCARD_TYPE_SDXC,                                                           // 大容量 SD 卡
}sdcard_type_enum;

typedef struct                                                                  // SD 卡信息结构体 此结构体定义不允许用户修改
{
    sdcard_type_enum    card_type;                                              // SD 卡类型
    uint32              block_count;                                            // 块数量
    uint32              block_len;                                              // 块容量长度
    uint32              rca;                                                    // 相对卡地址 <relative card address>
}sdcard_info_struct;

extern sdcard_info_struct sdcard_info;

sdcard_error_code_enum      sdcard_write_blocks     (uint32 block_index, uint32 block_count, uint8 *out_buf);
sdcard_error_code_enum      sdcard_read_blocks      (uint32 block_index, uint32 block_count, uint8 *in_buf);

sdcard_error_code_enum      sdcard_init             (void);

#endif
