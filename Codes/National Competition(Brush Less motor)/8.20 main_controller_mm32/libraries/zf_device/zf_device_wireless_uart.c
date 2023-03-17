/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_wireless_uart
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
*                   RX                  查看 zf_device_wrieless_uart.h 中 WRIELESS_UART_RX_PINx 宏定义
*                   TX                  查看 zf_device_wrieless_uart.h 中 WRIELESS_UART_TX_PINx 宏定义
*                   RTS                 查看 zf_device_wrieless_uart.h 中 WRIELESS_UART_RTS_PINx 宏定义
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   其余引脚悬空
*                   ------------------------------------
********************************************************************************************************************/

#include "zf_device_type.h"
#include "zf_device_wireless_uart.h"

static  fifo_struct         wireless_uart_fifo;
static  uint8               wireless_uart_buffer[WIRELESS_UART_BUFFER_SIZE];    // 数据存放数组

static          uint8       wireless_uart_data;
static volatile uint32      wireless_auto_baud_flag = 0;
static volatile uint8       wireless_auto_baud_data[3] = {0x00, 0x01, 0x03};

//-------------------------------------------------------------------------------------------------------------------
//  @brief      无线转串口模块 串口中断回调函数
//  @param      void
//  @return     void
//  Sample usage:               wireless_uart_callback();
//  @note       该函数在ISR文件 串口中断程序被调用
//-------------------------------------------------------------------------------------------------------------------
void wireless_uart_callback (void)
{
    uart_query_byte(WRIELESS_UART_INDEX, &wireless_uart_data);
    fifo_write_buffer(&wireless_uart_fifo, &wireless_uart_data, 1);
#if WIRELESS_UART_AUTO_BAUD_RATE                                                // 开启自动波特率
    if(wireless_auto_baud_flag == 1 && fifo_used(&wireless_uart_fifo) == 3)
    {
        wireless_auto_baud_flag = 3;
        fifo_read_buffer(&wireless_uart_fifo, (uint8 *)wireless_auto_baud_data, (uint32 *)&wireless_auto_baud_flag, FIFO_READ_AND_CLEAN);
    }
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       无线转串口模块 发送数据
// @param       data            8bit 数据
// @return      uint32          剩余发送长度
// Sample usage:                wireless_uart_send_byte(data);
//-------------------------------------------------------------------------------------------------------------------
uint32 wireless_uart_send_byte (const uint8 data)
{
    uint16 time_count = 0;
    uint8 return_state = 0;
    while(gpio_get(WRIELESS_UART_RTS_PIN))                                      // 如果RTS为低电平，则继续发送数据
    {
        if(time_count ++ > WIRELESS_UART_TIMEOUT_COUNT)
        {
            return_state = 1;                                                   // 模块忙,如果允许当前程序使用while等待 则可以使用后面注释的while等待语句替换本if语句
            break;
        }
        system_delay_ms(1);
    }
    if(0 == return_state)
        uart_write_byte(WRIELESS_UART_INDEX, data);                             // 发送最后的数据

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       无线转串口模块 发送数据块
// @param       *buff           发送缓冲区
// @param       len             发送数据长度
// @return      uint32          剩余发送长度
// Sample usage:                wireless_uart_send_buff(buff, 64);
//-------------------------------------------------------------------------------------------------------------------
uint32 wireless_uart_send_buff (const uint8 *buff, uint32 len)
{
    uint16 time_count = 0;
    uint8 return_state = 0;
    while(len > 30)
    {
        time_count = 0;
        while(gpio_get(WRIELESS_UART_RTS_PIN))                                  // 如果RTS为低电平，则继续发送数据
        {
            if(time_count ++ > WIRELESS_UART_TIMEOUT_COUNT)
            {
                return_state = 1;                                               // 模块忙,如果允许当前程序使用while等待 则可以使用后面注释的while等待语句替换本if语句
                break;
            }
            system_delay_ms(1);
        }
        if(1 == return_state)
            break;
        uart_write_buffer(WRIELESS_UART_INDEX, buff, 30);

        buff += 30;                                                             // 地址偏移
        len -= 30;                                                              // 数量
    }

    if(0 == return_state)
    {
        time_count = 0;
        while(gpio_get(WRIELESS_UART_RTS_PIN))                                  // 如果RTS为低电平，则继续发送数据
        {
            if(time_count ++ > WIRELESS_UART_TIMEOUT_COUNT)
            {
                return_state = 1;                                               // 模块忙,如果允许当前程序使用while等待 则可以使用后面注释的while等待语句替换本if语句
                break;
            }
            system_delay_ms(1);
        }
        if(0 == return_state)
            uart_write_buffer(WRIELESS_UART_INDEX, buff, len);                  // 发送最后的数据
    }

    return len;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       无线转串口模块 发送字符串
// @param       *str            要发送的字符串地址
// @return      uint32          剩余发送长度
// Sample usage:                wireless_uart_send_string("Believe in yourself.");
//-------------------------------------------------------------------------------------------------------------------
uint32 wireless_uart_send_string (const char *str)
{
    uint16 time_count = 0;
    uint32 len = strlen((const char *)str);
    uint8 return_state = 0;
    while(len > 30)
    {
        time_count = 0;
        while(gpio_get(WRIELESS_UART_RTS_PIN))                                  // 如果RTS为低电平，则继续发送数据
        {
            if(time_count ++ > WIRELESS_UART_TIMEOUT_COUNT)
            {
                return_state = 1;                                               // 模块忙,如果允许当前程序使用while等待 则可以使用后面注释的while等待语句替换本if语句
                break;
            }
            system_delay_ms(1);
        }
        if(1 == return_state)
            break;
        uart_write_buffer(WRIELESS_UART_INDEX, (const uint8 *)str, 30);

        str += 30;                                                              // 地址偏移
        len -= 30;                                                              // 数量
    }

    if(0 == return_state)
    {
        time_count = 0;
        while(gpio_get(WRIELESS_UART_RTS_PIN))                                  // 如果RTS为低电平，则继续发送数据
        {
            if(time_count ++ > WIRELESS_UART_TIMEOUT_COUNT)
                return len;                                                     // 模块忙,如果允许当前程序使用while等待 则可以使用后面注释的while等待语句替换本if语句
            system_delay_ms(1);
        }
        if(0 == return_state)
        uart_write_buffer(WRIELESS_UART_INDEX, (const uint8 *)str, len);        // 发送最后的数据
    }

    return len;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       无线转串口模块 发送摄像头图像至上位机查看图像
// @param       *image_addr     需要发送的图像地址
// @param       image_size      图像的大小
// @return      void
// Sample usage:                wireless_uart_send_image(&mt9v03x_image[0][0], MT9V03X_IMAGE_SIZE);
//-------------------------------------------------------------------------------------------------------------------
void wireless_uart_send_image (const uint8 *image_addr, uint32 image_size)
{
    extern uint8 camera_send_image_frame_header[4];
    wireless_uart_send_buff(camera_send_image_frame_header, 4);
    wireless_uart_send_buff((uint8 *)image_addr, image_size);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       无线转串口模块 读取缓冲
// @param       *buff           接收缓冲区
// @param       len             读取数据长度
// @return      uint32          实际读取数据长度
// Sample usage:                wireless_uart_read_buff(buff, 32);
//-------------------------------------------------------------------------------------------------------------------
uint32 wireless_uart_read_buff (uint8 *buff, uint32 len)
{
    uint32 data_len = len;
    fifo_read_buffer(&wireless_uart_fifo, buff, &data_len, FIFO_READ_AND_CLEAN);
    return data_len;
}








#define WIRELESS_UART			UART_3												// 无线转串口模块 所使用到的串口
#define WIRELESS_UART_TX		UART3_TX_D8
#define WIRELESS_UART_RX		UART3_RX_D9
#define WIRELESS_UART_BAUD		19200

#define RTS_PIN					B14	

void seekfree_wireless_init (void)
{
	//本函数使用的波特率为115200，为无线转串口模块的默认波特率，如需其他波特率请自行配置模块并修改串口的波特率
	gpio_init(RTS_PIN, GPI, GPIO_HIGH, GPI_PULL_UP);											// 初始化流控引脚

	uart_init (WIRELESS_UART, WIRELESS_UART_BAUD, WIRELESS_UART_TX, WIRELESS_UART_RX);			// 初始化串口
	uart_rx_irq(WIRELESS_UART, ENABLE);
}






















//-------------------------------------------------------------------------------------------------------------------
// @brief       无线转串口模块 初始化
// @param       void
// @return      void
// Sample usage:                wireless_uart_init();
// @note
//-------------------------------------------------------------------------------------------------------------------
uint8 wireless_uart_init (void)
{
    uint8 return_state = 0;
    set_wireless_type(WIRELESS_UART);

    fifo_init(&wireless_uart_fifo, wireless_uart_buffer, WIRELESS_UART_BUFFER_SIZE);
    gpio_init(WRIELESS_UART_RTS_PIN, GPI, GPIO_HIGH, GPI_PULL_UP);
#if(0 == WIRELESS_UART_AUTO_BAUD_RATE)                                          // 关闭自动波特率
    // 本函数使用的波特率为115200 为无线转串口模块的默认波特率 如需其他波特率请自行配置模块并修改串口的波特率
    uart_init (WRIELESS_UART_INDEX, WRIELESS_UART_BUAD_RATE, WRIELESS_UART_RX_PIN, WRIELESS_UART_TX_PIN);   // 初始化串口
    uart_rx_irq(WRIELESS_UART_INDEX, 1);
#elif(1 == WIRELESS_UART_AUTO_BAUD_RATE)                                        // 开启自动波特率
    uint8 rts_init_status;
    uint16 time_count = 0;

    wireless_auto_baud_flag = 0;
    wireless_auto_baud_data[0] = 0;
    wireless_auto_baud_data[1] = 1;
    wireless_auto_baud_data[2] = 3;

    rts_init_status = gpio_get(WRIELESS_UART_RTS_PIN);
    gpio_init(WRIELESS_UART_RTS_PIN, GPO, rts_init_status, GPO_PUSH_PULL);      // 初始化流控引脚

    uart_init (WRIELESS_UART_INDEX, WRIELESS_UART_BUAD_RATE, WRIELESS_UART_RX_PIN, WRIELESS_UART_TX_PIN);   // 初始化串口
    uart_rx_irq(WRIELESS_UART_INDEX, 1);

    system_delay_ms(5);                                                         // 模块上电之后需要延时等待
    gpio_set(WRIELESS_UART_RTS_PIN, !rts_init_status);                          // RTS引脚拉高，进入自动波特率模式
    system_delay_ms(100);                                                       // RTS拉高之后必须延时20ms
    gpio_toggle(WRIELESS_UART_RTS_PIN);                                         // RTS引脚取反

    wireless_auto_baud_flag = 1;

    uart_putchar(WRIELESS_UART_INDEX, wireless_auto_baud_data[0]);              // 发送特定数据 用于模块自动判断波特率
    uart_putchar(WRIELESS_UART_INDEX, wireless_auto_baud_data[1]);              // 发送特定数据 用于模块自动判断波特率
    uart_putchar(WRIELESS_UART_INDEX, wireless_auto_baud_data[2]);              // 发送特定数据 用于模块自动判断波特率
    system_delay_ms(20);

    time_count = 0;
    do{
        if(3 != wireless_auto_baud_flag)                                        // 检验自动波特率是否完成
        {
            while(time_count ++)
                system_delay_ms(1);
        }
        if(time_count >= WIRELESS_UART_TIMEOUT_COUNT)
        {
            return_state = 1;                                                   // 如果程序进入到此语句内 说明自动波特率失败了
            break;
        }

        time_count = 0;
        if( 0xa5 != wireless_auto_baud_data[0] &&                               // 检验自动波特率是否正确
            0xff != wireless_auto_baud_data[1] &&                               // 检验自动波特率是否正确
            0xff != wireless_auto_baud_data[2] )                                // 检验自动波特率是否正确
        {
            while(time_count ++)
                system_delay_ms(1);
        }
        if(time_count >= WIRELESS_UART_TIMEOUT_COUNT)
        {
            return_state = 1;                                                   // 如果程序进入到此语句内 说明自动波特率失败了
            break;
        }
        wireless_auto_baud_flag = 0;

        gpio_init(WRIELESS_UART_RTS_PIN, GPI, 0, GPI_PULL_UP);                  // 初始化流控引脚
        system_delay_ms(10);                                                    // 延时等待 模块准备就绪
    }while(0);
#endif
    return return_state;
}
