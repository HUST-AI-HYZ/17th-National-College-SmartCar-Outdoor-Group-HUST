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

#include "zf_driver_flash.h"

flash_data_union flash_union_buffer[FLASH_DATA_BUFFER_SIZE];               // FLASH 操作的数据缓冲区

//-------------------------------------------------------------------------------------------------------------------
// @brief       校验 FLASH 是否有数据
// @param       sector_num      需要写入的扇区编号 参数范围 <FLASH_SECTION_0-FLASH_SECTION_127>
// @param       page_num        当前扇区页的编号   参数范围 <FLASH_PAGE_0-FLASH_PAGE_3>
// @return      uint8           1-有数据 0-没有数据 如果需要对有数据的区域写入新的数据则应该对所在扇区进行擦除操作
// Sample usage:                flash_check(FLASH_SECTION_127, FLASH_PAGE_3);
//-------------------------------------------------------------------------------------------------------------------
uint8 flash_check (flash_section_enum sector_num, flash_page_enum page_num)
{
    uint16 temp_loop = 0;
    uint32 flash_addr = ((FLASH_BASE_ADDR + FLASH_SECTION_SIZE * sector_num + FLASH_PAGE_SIZE * page_num)); // 提取当前 Flash 地址

    for(temp_loop = 0; FLASH_PAGE_SIZE > temp_loop; temp_loop += 4)             // 循环读取 Flash 的值
    {
        if( 0xffffffff != (*(__IO uint32*) (flash_addr + temp_loop)) )        // 如果不是 0xffffffff 那就是有值
            return 1;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       擦除扇区
// @param       sector_num      需要擦除的扇区编号 参数范围 <FLASH_SECTION_0-FLASH_SECTION_127>
// @param       page_num        当前扇区页的编号   参数范围 <FLASH_PAGE_0-FLASH_PAGE_3>
// @return      uint8           1-表示失败 0-表示成功
// Sample usage:                flash_erase_page(FLASH_SECTION_127, FLASH_PAGE_3);
//-------------------------------------------------------------------------------------------------------------------
uint8 flash_erase_page (flash_section_enum sector_num, flash_page_enum page_num)
{
    volatile uint32 time_out = FLASH_OPERATION_TIME_OUT;
    uint32 flash_addr = ((FLASH_BASE_ADDR + FLASH_SECTION_SIZE * sector_num + FLASH_PAGE_SIZE * page_num)); // 提取当前 Flash 地址

    FLASH->KEYR = 0x45670123;                                                   // 写解锁键
    FLASH->KEYR = 0xCDEF89AB;                                                   // 写解锁键

    FLASH->SR = 0x00000034;                                                     // 清除操作标志 EOP WRPRTERR PGERR

    FLASH->CR |= 0x00000002;                                                    // 选择页擦除操作
    FLASH->AR = flash_addr;                                                     // 写入地址
    FLASH->CR |= 0x00000040;                                                    // 开始操作
    do {
        time_out--;
    } while ((0x00000001 == FLASH->SR) && (0x00 != time_out));                  // 等待超时或者完成
    if(FLASH->SR&0x00000015)                                                    // 如果不是操作完成 例如超时或出错
        time_out = 0;

    FLASH->CR = 0;                                                              // 停止所有操作
    FLASH->SR = 0x00000034;                                                     // 清除操作标志 EOP WRPRTERR PGERR

    FLASH->SR = 0x00000020;                                                     // FLASH 操作结束
    FLASH->CR |= 0x00000080;                                                    // FLASH 锁定
    if(!time_out)
        return 1;
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       读取一页
// @param       sector_num      需要读取的扇区编号 参数范围 <FLASH_SECTION_0-FLASH_SECTION_127>
// @param       page_num        当前扇区页的编号   参数范围 <FLASH_PAGE_0-FLASH_PAGE_3>
// @param       buf             需要读取的数据地址   传入的数组类型必须为uint32
// @param       len             需要写入的数据长度   参数范围 1-256
// @return      void
// Sample usage:                flash_read_page(FLASH_SECTION_127, FLASH_PAGE_3, data_buffer, 256);
//-------------------------------------------------------------------------------------------------------------------
void flash_read_page (flash_section_enum sector_num, flash_page_enum page_num, uint32 *buf, uint16 len)
{
    uint16 temp_loop = 0;
    uint32 flash_addr = ((FLASH_BASE_ADDR + FLASH_SECTION_SIZE * sector_num + FLASH_PAGE_SIZE * page_num)); // 提取当前 Flash 地址

    for(temp_loop = 0; temp_loop < len; temp_loop ++)                           // 根据指定长度读取
    {
        *buf ++ = *(__IO uint32*)(flash_addr + temp_loop * 4);                  // 循环读取 Flash 的值
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       编程一页
// @param       sector_num      需要写入的扇区编号 参数范围 <FLASH_SECTION_0-FLASH_SECTION_127>
// @param       page_num        当前扇区页的编号   参数范围 <FLASH_PAGE_0-FLASH_PAGE_3>
// @param       buf             需要写入的数据地址   传入的数组类型必须为uint32
// @param       len             需要写入的数据长度   参数范围 1-256
// @return      uint8           1-表示失败 0-表示成功
// Sample usage:                flash_write_page(FLASH_SECTION_127, FLASH_PAGE_3, data_buffer, 256);
//-------------------------------------------------------------------------------------------------------------------
uint8 flash_write_page (flash_section_enum sector_num, flash_page_enum page_num, const uint32 *buf, uint16 len)
{
    volatile uint32 time_out = FLASH_OPERATION_TIME_OUT;
    uint16 *data_pointer = (uint16 *)buf;
    uint32 flash_addr = ((FLASH_BASE_ADDR + FLASH_SECTION_SIZE * sector_num + FLASH_PAGE_SIZE * page_num)); // 提取当前 Flash 地址

    if(flash_check(sector_num, page_num))                                       // 判断是否有数据 这里是冗余的保护 防止有人没擦除就写入
        flash_erase_page(sector_num, page_num);                                 // 擦除这一页

    FLASH->KEYR = 0x45670123;                                                   // 写解锁键
    FLASH->KEYR = 0xCDEF89AB;                                                   // 写解锁键
    len *= 2;
    while(len --)                                                               // 根据长度
    {
        FLASH->CR |= 0x00000001;
        *(volatile uint16*)flash_addr = (*data_pointer ++);
        time_out = FLASH_OPERATION_TIME_OUT;
        do {
            time_out --;
        } while ((0x00000001 == FLASH->SR) && (0x00 != time_out));              // 等待超时或者完成
        if(FLASH->SR&0x00000015 || 0 == time_out)                               // 如果不是操作完成 例如超时或出错
            return 1;
        flash_addr += 2;
    }
    FLASH->CR |= 0x00000080;                                                    // FLASH 锁定
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       从指定 FLASH 的扇区的指定页码读取数据到缓冲区
// @param       sector_num      需要读取的扇区编号 参数范围 <FLASH_SECTION_0-FLASH_SECTION_127>
// @param       page_num        当前扇区页的编号   参数范围 <FLASH_PAGE_0-FLASH_PAGE_3>
// @return      void
// Sample usage:                flash_read_page_to_buffer(FLASH_SECTION_127, FLASH_PAGE_3);
//-------------------------------------------------------------------------------------------------------------------
void flash_read_page_to_buffer (flash_section_enum sector_num, flash_page_enum page_num)
{
    uint16 temp_loop = 0;
    uint32 flash_addr = ((FLASH_BASE_ADDR + FLASH_SECTION_SIZE * sector_num + FLASH_PAGE_SIZE * page_num)); // 提取当前 Flash 地址

    for(temp_loop = 0; FLASH_DATA_BUFFER_SIZE > temp_loop; temp_loop ++)                            // 根据指定长度读取
    {
        flash_union_buffer[temp_loop].uint32_type = *(__IO uint32*)(flash_addr + temp_loop * 4);    // 循环读取 Flash 的值
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       向指定 FLASH 的扇区的指定页码写入缓冲区的数据
// @param       sector_num      需要写入的扇区编号 参数范围 <FLASH_SECTION_0-FLASH_SECTION_127>
// @param       page_num        当前扇区页的编号   参数范围 <FLASH_PAGE_0-FLASH_PAGE_3>
// @return      uint8           1-表示失败 0-表示成功
// Sample usage:                flash_write_page_from_buffer(FLASH_SECTION_127, FLASH_PAGE_3);
//-------------------------------------------------------------------------------------------------------------------
uint8 flash_write_page_from_buffer (flash_section_enum sector_num, flash_page_enum page_num)
{
    volatile uint32 time_out = FLASH_OPERATION_TIME_OUT;
    uint32 flash_addr = ((FLASH_BASE_ADDR + FLASH_SECTION_SIZE * sector_num + FLASH_PAGE_SIZE * page_num)); // 提取当前 Flash 地址
    uint16 *data_pointer = (uint16 *)flash_union_buffer;
    uint16 data_len = FLASH_DATA_BUFFER_SIZE * 2;

    if(flash_check(sector_num, page_num))                                       // 判断是否有数据 这里是冗余的保护 防止有人没擦除就写入
        flash_erase_page(sector_num, page_num);                                 // 擦除这一页

    FLASH->KEYR = 0x45670123;                                                   // 写解锁键
    FLASH->KEYR = 0xCDEF89AB;                                                   // 写解锁键

    while(data_len --)                                                          // 根据长度
    {
        FLASH->CR |= 0x00000001;
        *(volatile uint16*)flash_addr = (*data_pointer ++);
        time_out = FLASH_OPERATION_TIME_OUT;
        do {
            time_out --;
        } while ((0x00000001 == FLASH->SR) && (0x00 != time_out));              // 等待超时或者完成
        if(FLASH->SR&0x00000015 || 0 == time_out)                               // 如果不是操作完成 例如超时或出错
            return 1;
        flash_addr += 2;
    }
    FLASH->CR |= 0x00000080;                                                    // FLASH 锁定
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       清空数据缓冲区
// @param       void
// @return      void
// Sample usage:                flash_buffer_clear();
//-------------------------------------------------------------------------------------------------------------------
void flash_buffer_clear (void)
{
    memset(flash_union_buffer, 0xFF, FLASH_PAGE_SIZE);
}
