/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_common_fifo
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#include "zf_common_fifo.h"
#include "zf_common_interrupt.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief       FIFO 头指针位移
// @param       *fifo           FIFO 对象指针
// @param       offset          偏移量
// @return      void
// Sample usage:                fifo_head_offset(fifo, length);
//-------------------------------------------------------------------------------------------------------------------
void fifo_head_offset (fifo_struct *fifo, uint32 offset)
{
    fifo->head += offset;
    
    while(fifo->max <= fifo->head)                                              // 如果范围超过则减缓冲区大小 直到小于最大缓冲区大小
    {
        fifo->head -= fifo->max;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       FIFO 尾指针位移
// @param       *fifo           FIFO 对象指针
// @param       offset          偏移量
// @return      void
// Sample usage:                fifo_end_offset(fifo, *length);
//-------------------------------------------------------------------------------------------------------------------
void fifo_end_offset (fifo_struct *fifo, uint32 offset)
{
    fifo->end += offset;
    
    while(fifo->max <= fifo->end)                                               // 如果范围超过则减缓冲区大小 直到小于最大缓冲区大小
    {
        fifo->end -= fifo->max;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       FIFO 重置缓冲器
// @param       *fifo           FIFO 对象指针
// @return      void
// Sample usage:                fifo_clear(fifo);
//-------------------------------------------------------------------------------------------------------------------
void fifo_clear (fifo_struct *fifo)
{
    fifo->head      = 0;
    fifo->end       = 0;
    fifo->size      = fifo->max;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       FIFO 查询当前数据个数
// @param       *fifo           FIFO 对象指针
// @return      uint32          已使用长度
// Sample usage:                fifo_used(fifo)
//-------------------------------------------------------------------------------------------------------------------
uint32 fifo_used (fifo_struct *fifo)
{
    return (fifo->max - fifo->size);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       向 FIFO 中写入数据
// @param       *fifo           FIFO 对象指针
// @param       *dat            数据来源缓冲区指针
// @param       length          需要写入的数据长度
// @return      fifo_state_enum 操作状态
// Sample usage:                if(fifo_write_buffer(&fifo, data, 32) != FIFO_SUCCESS) while(1);
//-------------------------------------------------------------------------------------------------------------------
fifo_state_enum fifo_write_buffer (fifo_struct *fifo, uint8 *dat, uint32 length)
{
    uint32 temp_length = 0;
    
    if(length < fifo->size)                                                     // 剩余空间足够装下本次数据
    {
        interrupt_global_disable();
        temp_length = fifo->max - fifo->head;                                   // 计算头指针距离缓冲区尾还有多少空间

        if(length > temp_length)                                                // 距离缓冲区尾长度不足写入数据 环形缓冲区分段操作
        {
            memcpy(&fifo->buffer[fifo->head], dat, temp_length);                // 拷贝第一段数据
            fifo_head_offset(fifo, temp_length);                                // 头指针偏移
            dat += temp_length;                                                 // 读取缓冲偏移
            memcpy(&fifo->buffer[fifo->head], dat, length - temp_length);       // 拷贝第一段数据
            fifo_head_offset(fifo, length - temp_length);                       // 头指针偏移
        }
        else
        {
            memcpy(&fifo->buffer[fifo->head], dat, length);                     // 一次完整写入
            fifo_head_offset(fifo, length);                                     // 头指针偏移
        }

        fifo->size -= length;                                                   // 缓冲区剩余长度减小
        interrupt_global_enable();
    }
    else
    {
        return FIFO_SPACE_NO_ENOUGH;
    }
    
    return FIFO_SUCCESS;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       从 FIFO 读取数据
// @param       *fifo           FIFO 对象指针
// @param       *dat            目标缓冲区指针
// @param       *length         读取的数据长度 如果没有这么多数据这里会被修改
// @param       flag            是否变更 FIFO 状态 可选择是否清空读取的数据
// @return      fifo_state_enum 操作状态
// Sample usage:                if(fifo_read_buffer(&fifo, data, 32, FIFO_READ_ONLY) != FIFO_SUCCESS) while(1);
//-------------------------------------------------------------------------------------------------------------------
fifo_state_enum fifo_read_buffer (fifo_struct *fifo, uint8 *dat, uint32 *length, fifo_operation_enum flag)
{
    uint8 data_check = 0;
    uint32 temp_length;

    if(*length > fifo_used(fifo))
    {
        *length = (fifo->max - fifo->size);                                     // 纠正读取的长度
        data_check = 1;                                                         // 标志数据不够
    }

    temp_length = fifo->max - fifo->end;                                        // 计算尾指针距离缓冲区尾还有多少空间
    if(*length <= temp_length)                                                  // 足够一次性读取完毕
    {
        if(NULL != dat)    memcpy(dat, &fifo->buffer[fifo->end], *length);      // 一次性读取完毕
    }
    else
    {
        if(NULL != dat)
        {
            memcpy(dat, &fifo->buffer[fifo->end], temp_length);                 // 拷贝第一段数据
            memcpy(&dat[temp_length], &fifo->buffer[0], *length - temp_length); // 拷贝第二段数据
        }
    }
    
    if(flag == FIFO_READ_AND_CLEAN)                                             // 如果选择读取并更改 FIFO 状态
    {
        interrupt_global_disable();
        fifo_end_offset(fifo, *length);                                         // 移动 FIFO 头指针
        fifo->size += *length;                                                  
        interrupt_global_enable();
    }

    return (data_check ? FIFO_DATA_NO_ENOUGH : FIFO_SUCCESS);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       FIFO 初始化 挂载对应缓冲区
// @param       *fifo           FIFO 对象指针
// @param       *buffer_addr    要挂载的缓冲区
// @param       size            缓冲区大小
// @return      fifo_state_enum 操作状态
// Sample usage:                fifo_init(&user_fifo, user_buffer, 64);
//-------------------------------------------------------------------------------------------------------------------
fifo_state_enum fifo_init (fifo_struct *fifo, uint8 *buffer_addr, uint32 size)
{
    if(NULL == buffer_addr)
        return FIFO_BUFFER_NULL;
    fifo->buffer    = buffer_addr;
    fifo->head      = 0;
    fifo->end       = 0;
    fifo->size      = size;
    fifo->max       = size;
    return FIFO_SUCCESS;
}
