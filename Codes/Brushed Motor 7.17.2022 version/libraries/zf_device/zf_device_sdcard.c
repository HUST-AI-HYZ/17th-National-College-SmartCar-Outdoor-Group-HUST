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

#include "zf_device_sdcard.h"

sdcard_info_struct  sdcard_info;
uint32              sdcard_resp_buf[4];

//-------------------------------------------------------------------------------------------------------------------
// @brief       SDIO 读取多块数据
// @param       block_index     起始块
// @param       block_count     读取块数
// @param       *in_buf         读取缓冲区
// @return      sdcard_error_code_enum  错误码
// Sample usage:                sdcard_read_blocks(0, 1, data_buffer);
//-------------------------------------------------------------------------------------------------------------------
sdcard_error_code_enum sdcard_read_blocks (uint32 block_index, uint32 block_count, uint8 *in_buf)
{
    sdcard_error_code_enum return_state = SDCARD_ERROR_STATE_NO_ERROR;
    uint32 timeout_count = 0;
    uint32 loop = block_count;

    do{
        // 设置 fifo
        sdio_switch_fifo_dir(false);                                            // fifo 方向为读取
        sdio_clear_fifo();

        if(1 == block_count)                                                    // 单块操作
        {
            // cmd17 读单块
            return_state = (sdcard_error_code_enum)sdio_execute_cmd(17, block_index, 0x00000081);
            if(SDCARD_ERROR_STATE_NO_ERROR != return_state)
                break;
            return_state = (sdcard_error_code_enum)sdio_request_resp(SDIO_RESP_TYPE_R1, NULL);
            if(SDCARD_ERROR_STATE_NO_ERROR != return_state)
                break;
        }
        else
        {
            // 设置多块操作的块数量
            sdio_set_block_count(block_count);

            // cmd18 
            return_state = (sdcard_error_code_enum)sdio_execute_cmd(18, block_index, 0u);
            if(SDCARD_ERROR_STATE_NO_ERROR != return_state)
                break;
            return_state = (sdcard_error_code_enum)sdio_request_resp(SDIO_RESP_TYPE_R1, NULL);
            if(SDCARD_ERROR_STATE_NO_ERROR != return_state)
                break;

            sdio_switch_fifo_read_wait(false);
        }

        // 传输并读取数据块
        while (loop--)
        {
            // 触发一个块并更新blk_count寄存器
            sdio_execute_data(0x00000001);

            // 等待数据块
            timeout_count = 0;
            while (0u == (0x00000002 & sdio_get_state()))
            {
                timeout_count ++;
                if(SDIO_TIMEOUT_COUNT < timeout_count)
                {
                    return_state = SDCARD_ERROR_STATE_TIME_OUT;
                    break;
                }
                system_delay_ms(1);
            }
            if(SDCARD_ERROR_STATE_TIME_OUT == return_state)
                break;
            sdio_clear_state(0x00000002);

            // 从 fifo 读取数据
            for (uint32 i = 0u; i < 128u; i ++)
            {
                *((uint32 *)in_buf) = sdio_get_fifo_data();
                in_buf += 4;
            }
        }
        if(SDCARD_ERROR_STATE_TIME_OUT == return_state)
            break;
        
        if(1 != block_count)
        {
            sdio_clear_state(0x00000010);

            // cmd12 停止多读进程的传输
            if(SDIO_ERROR_STATE_TIME_OUT == sdio_execute_cmd(12, 0, 0))
            {
                return_state = SDCARD_ERROR_STATE_TIME_OUT;
                break;
            }
            if(SDIO_ERROR_STATE_TIME_OUT == sdio_request_resp(SDIO_RESP_TYPE_R1, NULL))
            {
                return_state = SDCARD_ERROR_STATE_TIME_OUT;
                break;
            }
        }
    }while(0);

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       SDIO 写入多块数据
// @param       block_index     起始块
// @param       block_count     写入块数
// @param       *out_buf        写入缓冲区
// @return      sdcard_error_code_enum  错误码
// Sample usage:                sdcard_write_blocks(0, 1, data_buffer);
//-------------------------------------------------------------------------------------------------------------------
sdcard_error_code_enum sdcard_write_blocks (uint32 block_index, uint32 block_count, uint8 *out_buf)
{
    sdcard_error_code_enum return_state = SDCARD_ERROR_STATE_NO_ERROR;
    uint32 timeout_count = 0;
    uint32 loop = block_count;

    do{
        // cmd16
        return_state = (sdcard_error_code_enum)sdio_execute_cmd(16, SDCARD_BLOCK_SIZE, 0);
        if(SDCARD_ERROR_STATE_NO_ERROR != return_state)
            break;
        return_state = (sdcard_error_code_enum)sdio_request_resp(SDIO_RESP_TYPE_R1, NULL);
        if(SDCARD_ERROR_STATE_NO_ERROR != return_state)
            break;

        if(1 == block_count)
        {
            // cmd24 写单块命令
            return_state = (sdcard_error_code_enum)sdio_execute_cmd(24, block_index, 0);
            if(SDCARD_ERROR_STATE_NO_ERROR != return_state)
                break;
            return_state = (sdcard_error_code_enum)sdio_request_resp(SDIO_RESP_TYPE_R1, NULL);
            if(SDCARD_ERROR_STATE_NO_ERROR != return_state)
                break;
        }
        else
        {
            // cmd25 写多个块
            return_state = (sdcard_error_code_enum)sdio_execute_cmd(25, block_index, 0);
            if(SDCARD_ERROR_STATE_NO_ERROR != return_state)
                break;
            return_state = (sdcard_error_code_enum)sdio_request_resp(SDIO_RESP_TYPE_R1, NULL);
            if(SDCARD_ERROR_STATE_NO_ERROR != return_state)
                break;
        }

        // cmd13 发送卡状态 检查卡是否切换到 recv 模式
        while (1)
        {
            return_state = (sdcard_error_code_enum)sdio_execute_cmd(13, sdcard_info.rca << 16u, 0u);
            if(SDCARD_ERROR_STATE_NO_ERROR != return_state)
                break;
            return_state = (sdcard_error_code_enum)sdio_request_resp(SDIO_RESP_TYPE_R1, sdcard_resp_buf);
            if(SDCARD_ERROR_STATE_NO_ERROR != return_state)
                break;

            uint32 current_status = (sdcard_resp_buf[0] >> 9)  & 0xf;
            uint32 error_status   = (sdcard_resp_buf[0] >> 19) & 0x1;

            if ((current_status == 6) || (error_status == 1))
            {
                break;
            }
            timeout_count ++;
            if(SDIO_TIMEOUT_COUNT < timeout_count)
            {
                return_state = SDCARD_ERROR_STATE_TIME_OUT;
                break;
            }
            system_delay_ms(1);
        }
        if(SDCARD_ERROR_STATE_TIME_OUT == return_state)
            break;

        // 设置 fifo 写入
        sdio_switch_fifo_dir(true);
        sdio_clear_fifo();

        if(1 != block_count)
            // 设置多个块的块数
            sdio_set_block_count(block_count);

        while (loop--)
        {
            // 准备发送的数据
            for (uint32 i = 0u; i < 128u; i++)
            {
                sdio_set_fifo_data(*((uint32 *)out_buf));
                out_buf += 4u;
            }

            if(1 != block_count)
            {
                timeout_count = 0;
                while (1)
                {
                    // cmd13 检查接收缓冲区是否可用
                    return_state = (sdcard_error_code_enum)sdio_execute_cmd(13, sdcard_info.rca << 16u, 0u);
                    if(SDCARD_ERROR_STATE_NO_ERROR != return_state)
                        break;
                    return_state = (sdcard_error_code_enum)sdio_request_resp(SDIO_RESP_TYPE_R1, sdcard_resp_buf);
                    if(SDCARD_ERROR_STATE_NO_ERROR != return_state)
                        break;
                    uint32 ready_for_data = (sdcard_resp_buf[0] >> 8u) & 0x1;
                    if (ready_for_data == 1)
                    {
                        break;
                    }
                    timeout_count ++;
                    if(SDIO_TIMEOUT_COUNT < timeout_count)
                    {
                        return_state = SDCARD_ERROR_STATE_TIME_OUT;
                        break;
                    }
                    system_delay_ms(1);
                }
                if(SDCARD_ERROR_STATE_TIME_OUT == return_state)
                    break;
            }

            // 发送数据
            sdio_execute_data(0x00000002);

            // 等待 xfer 完成
            timeout_count = 0;
            while ( 0u == (0x00000002 & sdio_get_state()) )
            {
                timeout_count ++;
                if(SDIO_TIMEOUT_COUNT < timeout_count)
                {
                    return_state = SDCARD_ERROR_STATE_TIME_OUT;
                    break;
                }
                system_delay_ms(1);
            }
            if(SDCARD_ERROR_STATE_TIME_OUT == return_state)
                break;
            sdio_clear_state(0x00000002);
        }
        if(SDCARD_ERROR_STATE_TIME_OUT == return_state)
            break;

        // cmd12 停止传输
        return_state = (sdcard_error_code_enum)sdio_execute_cmd(12, 0, 0);
        if(SDCARD_ERROR_STATE_NO_ERROR != return_state)
            break;
        return_state = (sdcard_error_code_enum)sdio_request_resp(SDIO_RESP_TYPE_R1, NULL);
        if(SDCARD_ERROR_STATE_NO_ERROR != return_state)
            break;

        // cmd13 发送卡状态 检查卡是否正常
        timeout_count = 0;
        while (1)
        {
            return_state = (sdcard_error_code_enum)sdio_execute_cmd(13, sdcard_info.rca << 16u, 0);
            if(SDCARD_ERROR_STATE_NO_ERROR != return_state)
                break;
            return_state = (sdcard_error_code_enum)sdio_request_resp(SDIO_RESP_TYPE_R1, sdcard_resp_buf);
            if(SDCARD_ERROR_STATE_NO_ERROR != return_state)
                break;

            uint32 current_status = (sdcard_resp_buf[0] >> 9)  & 0xf;
            uint32 error_status   = (sdcard_resp_buf[0] >> 19) & 0x1;

            if (   ( (current_status != 7) & (current_status != 6) )
                || (error_status == 1) )
            {
                break;
            }
            timeout_count ++;
            if(SDIO_TIMEOUT_COUNT < timeout_count)
            {
                return_state = SDCARD_ERROR_STATE_TIME_OUT;
                break;
            }
            system_delay_ms(1);
        }
        if(SDCARD_ERROR_STATE_TIME_OUT == return_state)
            break;
    }while(0);

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       SD 卡初始化
// @param       void
// @return      sdcard_error_code_enum  错误码
// Sample usage:                sdcard_init();
//-------------------------------------------------------------------------------------------------------------------
sdcard_error_code_enum sdcard_init (void)
{
    uint32 timeout_count = 0;
    sdcard_error_code_enum return_state = SDCARD_ERROR_STATE_NO_ERROR;

    do{
        sdio_init();
        sdio_enable();

        // cmd0
        for (uint32 i = 0u; i < 74u; i++)
        {
            if(SDIO_ERROR_STATE_TIME_OUT == sdio_execute_cmd(0, 0, 0))
            {
                return_state = SDCARD_ERROR_STATE_TIME_OUT;
                break;
            }
        }
        if(SDCARD_ERROR_STATE_TIME_OUT == return_state)
        {
            zf_log(0, "CMD0 timeout.");
            break;
        }

        // cmd8
        while (1)
        {
            if(SDIO_ERROR_STATE_TIME_OUT == sdio_execute_cmd(8, 0x1AA, 0))
            {
                return_state = SDCARD_ERROR_STATE_TIME_OUT;
                break;
            }
            if(SDIO_ERROR_STATE_TIME_OUT == sdio_request_resp(SDIO_RESP_TYPE_R7, sdcard_resp_buf))
            {
                return_state = SDCARD_ERROR_STATE_TIME_OUT;
                break;
            }
            if (sdcard_resp_buf[0] == 0x1AA)
            {
                break;
            }
            timeout_count ++;
            if(SDIO_TIMEOUT_COUNT < timeout_count)
            {
                return_state = SDCARD_ERROR_STATE_TIME_OUT;
                break;
            }
            system_delay_ms(1);
        }
        if(SDCARD_ERROR_STATE_TIME_OUT == return_state)
        {
            zf_log(0, "CMD8 timeout.");
            break;
        }

        // cmd55 + acmd41
        timeout_count = 0;
        while (1)
        {
            // cmd55
            if(SDIO_ERROR_STATE_TIME_OUT == sdio_execute_cmd(55, 0, 0))
            {
                return_state = SDCARD_ERROR_STATE_TIME_OUT;
                break;
            }
            if(SDIO_ERROR_STATE_TIME_OUT == sdio_request_resp(SDIO_RESP_TYPE_R1, sdcard_resp_buf))
            {
                return_state = SDCARD_ERROR_STATE_TIME_OUT;
                break;
            }


            // acmd41
            if(SDIO_ERROR_STATE_TIME_OUT == sdio_execute_cmd(41, 0xC0100000, 0))
            {
                return_state = SDCARD_ERROR_STATE_TIME_OUT;
                break;
            }
            if(SDIO_ERROR_STATE_TIME_OUT == sdio_request_resp(SDIO_RESP_TYPE_R3, sdcard_resp_buf))
            {
                return_state = SDCARD_ERROR_STATE_TIME_OUT;
                break;
            }

            if ( 0u != ( sdcard_resp_buf[0] & (1u << 30)) )
            {
                sdcard_info.card_type = SDCARD_TYPE_SDHC;
            }
            else
            {
                sdcard_info.card_type = SDCARD_TYPE_SDSC;
            }

            if ( 0u != ( sdcard_resp_buf[0] & (1u << 31)) )
            {
                break;
            }
            timeout_count ++;
            if(SDIO_TIMEOUT_COUNT < timeout_count)
            {
                return_state = SDCARD_ERROR_STATE_TIME_OUT;
                break;
            }
            system_delay_ms(1);
        }
        if(SDCARD_ERROR_STATE_TIME_OUT == return_state)
        {
            zf_log(0, "CMD55+ACMD41 timeout.");
            break;
        }
 

        // cmd2 ALL_SEND_CID 识别号码 个体识别号码
        if(SDIO_ERROR_STATE_TIME_OUT == sdio_execute_cmd(2, 0, 0))
        {
            zf_log(0, "CMD2 timeout.");
            return_state = SDCARD_ERROR_STATE_TIME_OUT;
            break;
        }
        if(SDIO_ERROR_STATE_TIME_OUT == sdio_request_resp(SDIO_RESP_TYPE_R2, sdcard_resp_buf))
        {
            zf_log(0, "CMD2 timeout.");
            return_state = SDCARD_ERROR_STATE_TIME_OUT;
            break;
        }

        // cmd3 SEND_RELATIVE_ADDR 请求卡发布一个新的相对地址(RCA)。
        if(SDIO_ERROR_STATE_TIME_OUT == sdio_execute_cmd(3, 0, 0))
        {
            zf_log(0, "CMD3 timeout.");
            return_state = SDCARD_ERROR_STATE_TIME_OUT;
            break;
        }
        if(SDIO_ERROR_STATE_TIME_OUT == sdio_request_resp(SDIO_RESP_TYPE_R6, sdcard_resp_buf))
        {
            zf_log(0, "CMD3 timeout.");
            return_state = SDCARD_ERROR_STATE_TIME_OUT;
            break;
        }

        sdcard_info.rca = sdcard_resp_buf[0] >> 16u;

        // cmd9 + RCA -> CSD  SEND_CSD 地址卡在 cm d行上发送其特定于卡的数据(CSD)
        if(SDIO_ERROR_STATE_TIME_OUT == sdio_execute_cmd(9, sdcard_info.rca << 16u, 0))
        {
            zf_log(0, "CMD9 timeout.");
            return_state = SDCARD_ERROR_STATE_TIME_OUT;
            break;
        }
        if(SDIO_ERROR_STATE_TIME_OUT == sdio_request_resp(SDIO_RESP_TYPE_R2, sdcard_resp_buf))
        {
            zf_log(0, "CMD9 timeout.");
            return_state = SDCARD_ERROR_STATE_TIME_OUT;
            break;
        }

        if (((sdcard_resp_buf[3] >> 22) & 0x3) == 0x1)
        {   // CSD in version 2.0
            // memory capacity = (C_SIZE+1) * 512K byte.
            uint32 c_size = (sdcard_resp_buf[1] >> 8) & 0x3fffff;
            sdcard_info.block_count = c_size << 10u;
            sdcard_info.block_len = (1u << 9u);
        }
        else if (((sdcard_resp_buf[3] >> 22) & 0x3) == 0x0)
        {   // CSD in version 1.0
            uint32 c_size = (sdcard_resp_buf[1] >> 22) | ((sdcard_resp_buf[2] & 0x3) << 10);
            uint32 c_size_mult = ((sdcard_resp_buf[1] >> 7) & 0x7);
            uint32 block_len = (sdcard_resp_buf[2] >> 6) & 0xf;

            sdcard_info.block_count = (c_size + 1) << (c_size_mult + 2);
            sdcard_info.block_len = block_len;
        }

        // cmd13 发送卡状态 检查卡是否切换到待机状态
        timeout_count = 0;
        while (1)
        {
            if(SDIO_ERROR_STATE_TIME_OUT == sdio_execute_cmd(13, sdcard_info.rca << 16u, 0))
            {
                return_state = SDCARD_ERROR_STATE_TIME_OUT;
                break;
            }
            if(SDIO_ERROR_STATE_TIME_OUT == sdio_request_resp(SDIO_RESP_TYPE_R1, sdcard_resp_buf))
            {
                return_state = SDCARD_ERROR_STATE_TIME_OUT;
                break;
            }

            if ((((sdcard_resp_buf[0] >> 9) & 0xf) == 3) || (((sdcard_resp_buf[0] >> 19) & 0x1) == 1))
            {
                break;
            }
            timeout_count ++;
            if(SDIO_TIMEOUT_COUNT < timeout_count)
            {
                return_state = SDCARD_ERROR_STATE_TIME_OUT;
                break;
            }
            system_delay_ms(1);
        }
        if(SDCARD_ERROR_STATE_TIME_OUT == return_state)
        {
            zf_log(0, "CMD13 timeout.");
            break;
        }

        // cmd7 使用 RCA 断言或取消断言
        if(SDIO_ERROR_STATE_TIME_OUT == sdio_execute_cmd(7, sdcard_info.rca << 16u, 0))
        {
            zf_log(0, "CMD7 timeout.");
            return_state = SDCARD_ERROR_STATE_TIME_OUT;
            break;
        }
        if(SDIO_ERROR_STATE_TIME_OUT == sdio_request_resp(SDIO_RESP_TYPE_R1, sdcard_resp_buf))
        {
            zf_log(0, "CMD7 timeout.");
            return_state = SDCARD_ERROR_STATE_TIME_OUT;
            break;
        }

        // cmd13 发送卡状态 检查卡是否切换到传输模式
        timeout_count = 0;
        while (1)
        {
            if(SDIO_ERROR_STATE_TIME_OUT == sdio_execute_cmd(13, sdcard_info.rca << 16u, 0))
            {
                return_state = SDCARD_ERROR_STATE_TIME_OUT;
                break;
            }
            if(SDIO_ERROR_STATE_TIME_OUT == sdio_request_resp(SDIO_RESP_TYPE_R1, sdcard_resp_buf))
            {
                return_state = SDCARD_ERROR_STATE_TIME_OUT;
                break;
            }

            if ((((sdcard_resp_buf[0] >> 9) & 0xf) == 4) || (((sdcard_resp_buf[0] >> 19) & 0x1) == 1))
            {
                break;
            }
            timeout_count ++;
            if(SDIO_TIMEOUT_COUNT < timeout_count)
            {
                return_state = SDCARD_ERROR_STATE_TIME_OUT;
                break;
            }
            system_delay_ms(1);
        }
        if(SDCARD_ERROR_STATE_TIME_OUT == return_state)
        {
            zf_log(0, "CMD13 timeout.");
            break;
        }

        // cmd55 + acmd6 设置卡数据总线宽度
        if(SDIO_ERROR_STATE_TIME_OUT == sdio_execute_cmd(55, sdcard_info.rca << 16u, 0))
        {
            zf_log(0, "CMD55 timeout.");
            return_state = SDCARD_ERROR_STATE_TIME_OUT;
            break;
        }
        if(SDIO_ERROR_STATE_TIME_OUT == sdio_request_resp(SDIO_RESP_TYPE_R1, sdcard_resp_buf))
        {
            zf_log(0, "CMD55 timeout.");
            return_state = SDCARD_ERROR_STATE_TIME_OUT;
            break;
        }

        if(SDIO_ERROR_STATE_TIME_OUT == sdio_execute_cmd(6, 0x2, 0))
        {
            zf_log(0, "CMD6 timeout.");
            return_state = SDCARD_ERROR_STATE_TIME_OUT;
            break;
        }
        if(SDIO_ERROR_STATE_TIME_OUT == sdio_request_resp(SDIO_RESP_TYPE_R1, sdcard_resp_buf))
        {
            zf_log(0, "CMD6 timeout.");
            return_state = SDCARD_ERROR_STATE_TIME_OUT;
            break;
        }

        sdio_set_data_bus_width(SDIO_DATA_BUS_4BIT);
    }while(0);

    return return_state;
}
