/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_driver_flash
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#ifndef _zf_driver_flash_h_
#define _zf_driver_flash_h_

#include "hal_device_registers.h"

#include "zf_common_typedef.h"

#define FLASH_BASE_ADDR             (0x08000000)
#define FLASH_PAGE_SIZE             (0x00000400)                                // 1K byte
#define FLASH_SECTION_SIZE          (FLASH_PAGE_SIZE * 4)                       // 4K byte
#define FLASH_OPERATION_TIME_OUT    (0x0FFF)

#define FLASH_DATA_BUFFER_SIZE      (FLASH_PAGE_SIZE / sizeof(flash_data_union))// 自动计算每个页能够存下多少个数据

typedef union                                                                   // 固定的数据缓冲单元格式
{
    float   float_type;                                                         // float  类型
    uint32  uint32_type;                                                        // uint32 类型
    int32   int32_type;                                                         // int32  类型
    uint16  uint16_type;                                                        // uint16 类型
    int16   int16_type;                                                         // int16  类型
    uint8   uint8_type;                                                         // uint8  类型
    int8    int8_type;                                                          // int8   类型
}flash_data_union;                                                              // 所有类型数据共用同一个 32bit 地址

typedef enum                                                                    // 枚举 Flash 扇区  此枚举定义不允许用户修改
{
    FLASH_SECTION_0,    FLASH_SECTION_1,    FLASH_SECTION_2,    FLASH_SECTION_3,
    FLASH_SECTION_4,    FLASH_SECTION_5,    FLASH_SECTION_6,    FLASH_SECTION_7,
    FLASH_SECTION_8,    FLASH_SECTION_9,    FLASH_SECTION_10,   FLASH_SECTION_11,
    FLASH_SECTION_12,   FLASH_SECTION_13,   FLASH_SECTION_14,   FLASH_SECTION_15,

    FLASH_SECTION_16,   FLASH_SECTION_17,   FLASH_SECTION_18,   FLASH_SECTION_19,
    FLASH_SECTION_20,   FLASH_SECTION_21,   FLASH_SECTION_22,   FLASH_SECTION_23,
    FLASH_SECTION_24,   FLASH_SECTION_25,   FLASH_SECTION_26,   FLASH_SECTION_27,
    FLASH_SECTION_28,   FLASH_SECTION_29,   FLASH_SECTION_30,   FLASH_SECTION_31,

    FLASH_SECTION_32,   FLASH_SECTION_33,   FLASH_SECTION_34,   FLASH_SECTION_35,
    FLASH_SECTION_36,   FLASH_SECTION_37,   FLASH_SECTION_38,   FLASH_SECTION_39,
    FLASH_SECTION_40,   FLASH_SECTION_41,   FLASH_SECTION_42,   FLASH_SECTION_43,
    FLASH_SECTION_44,   FLASH_SECTION_45,   FLASH_SECTION_46,   FLASH_SECTION_47,

    FLASH_SECTION_48,   FLASH_SECTION_49,   FLASH_SECTION_50,   FLASH_SECTION_51,
    FLASH_SECTION_52,   FLASH_SECTION_53,   FLASH_SECTION_54,   FLASH_SECTION_55,
    FLASH_SECTION_56,   FLASH_SECTION_57,   FLASH_SECTION_58,   FLASH_SECTION_59,
    FLASH_SECTION_60,   FLASH_SECTION_61,   FLASH_SECTION_62,   FLASH_SECTION_63,

    FLASH_SECTION_64,   FLASH_SECTION_65,   FLASH_SECTION_66,   FLASH_SECTION_67,
    FLASH_SECTION_68,   FLASH_SECTION_69,   FLASH_SECTION_70,   FLASH_SECTION_71,
    FLASH_SECTION_72,   FLASH_SECTION_73,   FLASH_SECTION_74,   FLASH_SECTION_75,
    FLASH_SECTION_76,   FLASH_SECTION_77,   FLASH_SECTION_78,   FLASH_SECTION_79,

    FLASH_SECTION_80,   FLASH_SECTION_81,   FLASH_SECTION_82,   FLASH_SECTION_83,
    FLASH_SECTION_84,   FLASH_SECTION_85,   FLASH_SECTION_86,   FLASH_SECTION_87,
    FLASH_SECTION_88,   FLASH_SECTION_89,   FLASH_SECTION_90,   FLASH_SECTION_91,
    FLASH_SECTION_92,   FLASH_SECTION_93,   FLASH_SECTION_94,   FLASH_SECTION_95,

    FLASH_SECTION_96,   FLASH_SECTION_97,   FLASH_SECTION_98,   FLASH_SECTION_99,
    FLASH_SECTION_100,  FLASH_SECTION_101,  FLASH_SECTION_102,  FLASH_SECTION_103,
    FLASH_SECTION_104,  FLASH_SECTION_105,  FLASH_SECTION_106,  FLASH_SECTION_107,
    FLASH_SECTION_108,  FLASH_SECTION_109,  FLASH_SECTION_110,  FLASH_SECTION_111,

    FLASH_SECTION_112,  FLASH_SECTION_113,  FLASH_SECTION_114,  FLASH_SECTION_115,
    FLASH_SECTION_116,  FLASH_SECTION_117,  FLASH_SECTION_118,  FLASH_SECTION_119,
    FLASH_SECTION_120,  FLASH_SECTION_121,  FLASH_SECTION_122,  FLASH_SECTION_123,
    FLASH_SECTION_124,  FLASH_SECTION_125,  FLASH_SECTION_126,  FLASH_SECTION_127
}flash_section_enum;

typedef enum                                                                    // 枚举 Flash 页索引 此枚举定义不允许用户修改
{
    FLASH_PAGE_0,
    FLASH_PAGE_1,
    FLASH_PAGE_2,
    FLASH_PAGE_3,
}flash_page_enum;

extern flash_data_union flash_union_buffer[FLASH_DATA_BUFFER_SIZE];

uint8       flash_check                     (flash_section_enum sector_num, flash_page_enum page_num);
uint8       flash_erase_page                (flash_section_enum sector_num, flash_page_enum page_num);
void        flash_read_page                 (flash_section_enum sector_num, flash_page_enum page_num, uint32 *buf, uint16 len);
uint8       flash_write_page                (flash_section_enum sector_num, flash_page_enum page_num, const uint32 *buf, uint16 len);

void        flash_read_page_to_buffer      (flash_section_enum sector_num, flash_page_enum page_num);
uint8       flash_write_page_from_buffer   (flash_section_enum sector_num, flash_page_enum page_num);
void        flash_buffer_clear             (void);

#endif
