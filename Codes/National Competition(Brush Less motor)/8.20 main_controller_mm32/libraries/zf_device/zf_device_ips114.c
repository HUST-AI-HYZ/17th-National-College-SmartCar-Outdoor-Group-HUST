/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_ips114
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
* @note             接线定义：
*                   ------------------------------------
*                   模块管脚             单片机管脚
*                   SCL                  查看 zf_device_ips114.h 中 IPS114_SCL_PIN 宏定义
*                   SDA                  查看 zf_device_ips114.h 中 IPS114_SDA_PIN 宏定义
*                   RST                  查看 zf_device_ips114.h 中 IPS114_RST_PIN 宏定义
*                   DC                   查看 zf_device_ips114.h 中 IPS114_DC_PIN 宏定义
*                   CS                   查看 zf_device_ips114.h 中 IPS114_CS_PIN 宏定义
*                   BLK                  查看 zf_device_ips114.h 中 IPS114_BLK_PIN 宏定义
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   最大分辨率 135 * 240
*                   ------------------------------------
********************************************************************************************************************/

#include "zf_device_ips114.h"

static uint16           ips114_pencolor     = IPS114_DEFAULT_PENCOLOR;
static uint16           ips114_bgcolor      = IPS114_DEFAULT_BGCOLOR;

static ips114_dir_enum  ips114_display_dir  = IPS114_DEFAULT_DISPLAY_DIR;
static uint8            ips114_x_max        = 240;
static uint8            ips114_y_max        = 135;

#if IPS114_USE_SOFT_SPI
static soft_spi_info_struct             ips114_spi;
#define ips114_write_8bit_data(data)    (soft_spi_write_8bit(&ips114_spi, (data)))
#define ips114_write_16bit_data(data)   (soft_spi_write_16bit(&ips114_spi, (data)))
#else
#define ips114_write_8bit_data(data)    (spi_write_8bit(IPS114_SPI, (data)))
#define ips114_write_16bit_data(data)   (spi_write_16bit(IPS114_SPI, (data)))
#endif

//-------------------------------------------------------------------------------------------------------------------
// @brief       写命令 内部调用
// @param       void
// @return      void
// @note        内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void ips114_write_index (const uint8 dat)
{
    IPS114_CS(1);
    IPS114_CS(0);
    IPS114_DC(0);
    ips114_write_8bit_data(dat);
    IPS114_DC(1);
    IPS114_CS(1);
    IPS114_CS(0);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       设置显示区域 内部调用
// @param       x1              起始x轴坐标
// @param       y1              起始y轴坐标
// @param       x2              结束x轴坐标
// @param       y2              结束y轴坐标
// @return      void
// @note        内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void ips114_set_region (const uint16 x1, const uint16 y1, const uint16 x2, const uint16 y2)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    // 检查一下你的显示调用的函数 自己计算一下哪里超过了屏幕显示范围
    zf_assert(x1 < ips114_x_max);
    zf_assert(y1 < ips114_y_max);
    zf_assert(x2 < ips114_x_max);
    zf_assert(y2 < ips114_y_max);

    if(ips114_display_dir == IPS114_PORTAIT)
    {
        ips114_write_index(0x2a);                                               // 列地址设置
        ips114_write_16bit_data(x1 + 52);
        ips114_write_16bit_data(x2 + 52);
        ips114_write_index(0x2b);                                               // 行地址设置
        ips114_write_16bit_data(y1 + 40);
        ips114_write_16bit_data(y2 + 40);
        ips114_write_index(0x2c);                                               // 储存器写
    }
    else if(ips114_display_dir == IPS114_PORTAIT_180)
    {
        ips114_write_index(0x2a);                                               // 列地址设置
        ips114_write_16bit_data(x1 + 53);
        ips114_write_16bit_data(x2 + 53);
        ips114_write_index(0x2b);                                               // 行地址设置
        ips114_write_16bit_data(y1 + 40);
        ips114_write_16bit_data(y2 + 40);
        ips114_write_index(0x2c);                                               // 储存器写
    }
    else if(ips114_display_dir == IPS114_CROSSWISE)
    {
        ips114_write_index(0x2a);                                               // 列地址设置
        ips114_write_16bit_data(x1 + 40);
        ips114_write_16bit_data(x2 + 40);
        ips114_write_index(0x2b);                                               // 行地址设置
        ips114_write_16bit_data(y1 + 53);
        ips114_write_16bit_data(y2 + 53);
        ips114_write_index(0x2c);                                               // 储存器写
    }
    else
    {
        ips114_write_index(0x2a);                                               // 列地址设置
        ips114_write_16bit_data(x1 + 40);
        ips114_write_16bit_data(x2 + 40);
        ips114_write_index(0x2b);                                               // 行地址设置
        ips114_write_16bit_data(y1 + 52);
        ips114_write_16bit_data(y2 + 52);
        ips114_write_index(0x2c);                                               // 储存器写
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       1.14寸 TFT液晶显示DEBUG信息初始化
// @param       void
// @return      void
// Sample usage:               内部使用用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void ips114_debug_init (void)
{
    debug_output_struct info;
    debug_output_struct_init(&info);

    info.type_index = 1;
    info.display_x_max = ips114_x_max;
    info.display_y_max = ips114_y_max;

    info.font_x_size = 8;
    info.font_y_size = 16;
    info.output_screen = ips114_show_string;
    info.output_screen_clear = ips114_clear;
        
    debug_output_init(&info);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       液晶清屏函数 将屏幕清空成背景颜色
// @param       void
// @return      void
// Sample usage:                ips114_clear();
//-------------------------------------------------------------------------------------------------------------------
void ips114_clear (void)
{
    uint32 i = ips114_x_max * ips114_y_max;

    ips114_set_region(0, 0, ips114_x_max - 1, ips114_y_max - 1);
    for( ; i > 0; i --)
    {
        ips114_write_16bit_data(ips114_bgcolor);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       液晶屏幕填充函数
// @param       color           颜色格式 RGB565 或者可以使用 zf_common_font.h 内常用颜色宏定义
// @return      void
// Sample usage:                ips114_full(RGB565_BLACK);
//-------------------------------------------------------------------------------------------------------------------
void ips114_full (const uint16 color)
{
    uint32 i = ips114_x_max * ips114_y_max;

    ips114_set_region(0, 0, ips114_x_max - 1, ips114_y_max - 1);
    for( ; i > 0; i --)
    {
        ips114_write_16bit_data(color);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       设置显示方向 这个函数只有在初始化屏幕之前调用才生效
// @param       dir             显示方向  参照 zf_device_ips114.h 内 ips114_dir_enum 枚举体定义
// @return      void
// Sample usage:                ips114_set_dir(IPS114_CROSSWISE);
//-------------------------------------------------------------------------------------------------------------------
void ips114_set_dir (ips114_dir_enum dir)
{
    ips114_display_dir = dir;
    if(dir < 2)
    {
        ips114_x_max = 135;
        ips114_y_max = 240;
    }
    else
    {
        ips114_x_max = 240;
        ips114_y_max = 135;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       设置显示颜色
// @param       pen             颜色格式 RGB565 或者可以使用 zf_common_font.h 内常用颜色宏定义
// @param       bgcolor         颜色格式 RGB565 或者可以使用 zf_common_font.h 内常用颜色宏定义
// @return      void
// Sample usage:                ips114_set_color(RGB565_WHITE, RGB565_BLACK);
//-------------------------------------------------------------------------------------------------------------------
void ips114_set_color (const uint16 pen, const uint16 bgcolor)
{
    ips114_pencolor = pen;
    ips114_bgcolor = bgcolor;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       液晶画点
// @param       x               坐标x方向的起点 参数范围 [0, ips114_x_max-1]
// @param       y               坐标y方向的起点 参数范围 [0, ips114_y_max-1]
// @param       dat             需要显示的颜色
// @return      void
// Sample usage:                ips114_draw_point(0, 0, RGB565_RED);            // 坐标 0,0 画一个红色的点
//-------------------------------------------------------------------------------------------------------------------
void ips114_draw_point (uint16 x, uint16 y, const uint16 color)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    zf_assert(x < ips114_x_max);
    zf_assert(y < ips114_y_max);

    ips114_set_region(x, y, x, y);
    ips114_write_16bit_data(color);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       液晶画线
// @param       x_start         坐标x方向的起点
// @param       y_start         坐标y方向的起点
// @param       x_end           坐标x方向的终点
// @param       y_end           坐标y方向的终点
// @param       dat             需要显示的颜色
// @return      void
// Sample usage:                ips114_draw_line(0, 0, 10, 10, RGB565_RED);     // 坐标 0,0 到 10,10 画一条红色的线
//-------------------------------------------------------------------------------------------------------------------
void ips114_draw_line (uint16 x_start, uint16 y_start, uint16 x_end, uint16 y_end, const uint16 color)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    zf_assert(x_start < ips114_x_max);
    zf_assert(y_start < ips114_y_max);
    zf_assert(x_end < ips114_x_max);
    zf_assert(y_end < ips114_y_max);

    int16 x_dir = (x_start < x_end ? 1 : -1);
    int16 y_dir = (y_start < y_end ? 1 : -1);
    float temp_rate = 0;
    float temp_b = 0;

    if(x_start != x_end)
    {
        temp_rate = (float)(y_start - y_end) / (float)(x_start - x_end);
        temp_b = (float)y_start - (float)x_start * temp_rate;
    }
    else
    {
        while(y_start != y_end)
        {
            ips114_draw_point(x_start, y_start, color);
            y_start += y_dir;
        }
        return;
    }
    if(myabs(y_start - y_end) > myabs(x_start - x_end))
    {
        while(y_start != y_end)
        {
            ips114_draw_point(x_start, y_start, color);
            y_start += y_dir;
            x_start = (int16)(((float)y_start - temp_b) / temp_rate);
        }
    }
    else
    {
        while(x_start != x_end)
        {
            ips114_draw_point(x_start, y_start, color);
            x_start += x_dir;
            y_start = (int16)((float)x_start * temp_rate + temp_b);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       液晶显示字符
// @param       x               坐标x方向的起点 参数范围 [0, ips114_x_max-1]
// @param       y               坐标y方向的起点 参数范围 [0, ips114_y_max-1]
// @param       dat             需要显示的字符
// @return      void
// Sample usage:                ips114_show_char(0, 0, 'x');                    // 坐标 0,0 写一个字符 x
//-------------------------------------------------------------------------------------------------------------------
void ips114_show_char (uint16 x, uint16 y, const char dat)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    zf_assert(x < ips114_x_max);
    zf_assert(y < ips114_y_max);

    uint8 i, j;
    uint8 temp;

    for(i = 0; i < 16; i ++)
    {
        ips114_set_region(x, y + i, x + 7, y + i);
        temp = tft_ascii[dat - 32][i];                                          // 减 32 因为是取模是从空格开始取得 空格在 ascii 中序号是 32
        for(j = 0; j < 8; j ++)
        {
            if(temp & 0x01)   
                ips114_write_16bit_data(ips114_pencolor);
            else
                ips114_write_16bit_data(ips114_bgcolor);
            temp >>= 1;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       液晶显示字符串
// @param       x               坐标x方向的起点 参数范围 [0, ips114_x_max-1]
// @param       y               坐标y方向的起点 参数范围 [0, ips114_y_max-1]
// @param       dat             需要显示的字符串
// @return      void
// Sample usage:                ips114_show_string(0, 0, "seekfree");
//-------------------------------------------------------------------------------------------------------------------
void ips114_show_string (uint16 x, uint16 y, const char dat[])
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    zf_assert(x < ips114_x_max);
    zf_assert(y < ips114_y_max);
    
    uint16 j = 0;
    while(dat[j] != '\0')
    {
        ips114_show_char(x + 8 * j, y, dat[j]);
        j ++;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       液晶显示32位有符号 (去除整数部分无效的0)
// @param       x               坐标x方向的起点 参数范围 [0, ips114_x_max-1]
// @param       y               坐标y方向的起点 参数范围 [0, ips114_y_max-1]
// @param       dat             需要显示的变量 数据类型 int32
// @param       num             需要显示的位数 最高10位  不包含正负号
// @return      void
// Sample usage:                ips114_show_int(0, 0, x, 3);                    // x 可以为 int32 int16 int8 类型
// note:                        负数会显示一个 ‘-’号   正数显示一个空格
//-------------------------------------------------------------------------------------------------------------------
void ips114_show_int (uint16 x, uint16 y, const int32 dat, uint8 num)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    zf_assert(x < ips114_x_max);
    zf_assert(y < ips114_y_max);
    zf_assert(num > 0);
    zf_assert(num <= 10);

    int32 dat_temp = dat;
    int32 offset = 1;
    char data_buffer[12];

    memset(data_buffer, 0, 12);
    memset(data_buffer, ' ', num + 1);

    if(num < 10)
    {
        for(; num > 0; num --)
            offset *= 10;
        dat_temp %= offset;
    }
    int_to_str(data_buffer, dat_temp);
    ips114_show_string(x, y, (const char *)&data_buffer);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       液晶显示32位无符号 (去除整数部分无效的0)
// @param       x               坐标x方向的起点 参数范围 [0, ips114_x_max-1]
// @param       y               坐标y方向的起点 参数范围 [0, ips114_y_max-1]
// @param       dat             需要显示的变量 数据类型 uint32
// @param       num             需要显示的位数 最高10位  不包含正负号
// @return      void
// Sample usage:                ips114_show_uint(0, 0, x, 3);                   // x 可以为 uint32 uint16 uint8 类型
// note:                        负数会显示一个 ‘-’号   正数显示一个空格
//-------------------------------------------------------------------------------------------------------------------
void ips114_show_uint (uint16 x, uint16 y, const uint32 dat, uint8 num)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    zf_assert(x < ips114_x_max);
    zf_assert(y < ips114_y_max);
    zf_assert(num > 0);
    zf_assert(num <= 10);

    uint32 dat_temp = dat;
    int32 offset = 1;
    char data_buffer[12];
    memset(data_buffer, 0, 12);
    memset(data_buffer, ' ', num);

    if(num < 10)
    {
        for(; num > 0; num --)
            offset *= 10;
        dat_temp %= offset;
    }
    uint_to_str(data_buffer, dat_temp);
    ips114_show_string(x, y, (const char *)&data_buffer);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       液晶显示浮点数 (去除整数部分无效的0)
// @param       x               坐标x方向的起点 参数范围 [0, ips114_x_max-1]
// @param       y               坐标y方向的起点 参数范围 [0, ips114_y_max-1]
// @param       dat             需要显示的变量 数据类型 float 或 double
// @param       num             整数位显示长度   最高10位  
// @param       pointnum        小数位显示长度   最高6位
// @return      void
// Sample usage:                ips114_show_float(0, 0, x, 2, 3);               // 显示浮点数 整数显示 2 位 小数显示 3 位
// @note                        特别注意当发现小数部分显示的值与你写入的值不一样的时候，
//                              可能是由于浮点数精度丢失问题导致的，这并不是显示函数的问题，
//                              有关问题的详情，请自行百度学习   浮点数精度丢失问题。
//                              负数会显示一个 ‘-’号   正数显示一个空格
//-------------------------------------------------------------------------------------------------------------------
void ips114_show_float (uint16 x, uint16 y, const float dat, uint8 num, uint8 pointnum)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    zf_assert(x < ips114_x_max);
    zf_assert(y < ips114_y_max);
    zf_assert(num > 0);
    zf_assert(num <= 8);
    zf_assert(pointnum > 0);
    zf_assert(pointnum <= 6);

    float dat_temp = dat;
    float offset = 1.0;
    char data_buffer[17];
    memset(data_buffer, 0, 17);
    memset(data_buffer, ' ', num + pointnum + 2);

    if(num < 10)
    {
        for(; num > 0; num --)
            offset *= 10;
        dat_temp = dat_temp - ((int)dat_temp / (int)offset) * offset;
    }
    float_to_str(data_buffer, dat_temp, pointnum);
    ips114_show_string(x, y, data_buffer);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IPS114 显示二值图像 数据每八个点组成一个字节数据
// @param       x               坐标x方向的起点 参数范围 [0, ips114_x_max-1]
// @param       y               坐标y方向的起点 参数范围 [0, ips114_y_max-1]
// @param       *image          图像数组指针
// @param       width           图像实际宽度
// @param       height          图像实际高度
// @param       dis_width       图像显示宽度 参数范围 [0, ips114_x_max]
// @param       dis_height      图像显示高度 参数范围 [0, ips114_y_max]
// @return      void
// Sample usage:                ips114_show_binary_image(0, 0, ov7725_image_binary[0], OV7725_W, OV7725_H, OV7725_W, OV7725_H);
//-------------------------------------------------------------------------------------------------------------------
void ips114_show_binary_image (uint16 x, uint16 y, const uint8 *image, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    zf_assert(x < ips114_x_max);
    zf_assert(y < ips114_y_max);

    uint32 i = 0, j = 0;
    uint8 temp = 0;
    uint32 width_index = 0, height_index = 0;

    ips114_set_region(x, y, x + dis_width - 1, y + dis_height - 1);             // 设置显示区域

    for(j = 0; j < dis_height; j ++)
    {
        height_index = j * height / dis_height;
        for(i = 0; i < dis_width; i ++)
        {
            width_index = i * width / dis_width;
            temp = *(image + height_index * width / 8 + width_index / 8);       // 读取像素点
            if(0x80 & (temp << (width_index % 8)))
                ips114_write_16bit_data(RGB565_WHITE);
            else
                ips114_write_16bit_data(RGB565_BLACK);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IPS114 显示 8bit 灰度图像 带二值化阈值
// @param       x               坐标x方向的起点 参数范围 [0, ips114_x_max-1]
// @param       y               坐标y方向的起点 参数范围 [0, ips114_y_max-1]
// @param       *image          图像数组指针
// @param       width           图像实际宽度
// @param       height          图像实际高度
// @param       dis_width       图像显示宽度 参数范围 [0, ips114_x_max]
// @param       dis_height      图像显示高度 参数范围 [0, ips114_y_max]
// @param       threshold       二值化显示阈值 0-不开启二值化
// @return      void
// Sample usage:                ips114_show_gray_image(0, 0, mt9v03x_image[0], MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0);
//-------------------------------------------------------------------------------------------------------------------
void ips114_show_gray_image (uint16 x, uint16 y, const uint8 *image, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height, uint8 threshold)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    zf_assert(x < ips114_x_max);
    zf_assert(y < ips114_y_max);

    uint32 i = 0, j = 0;
    uint16 color = 0,temp = 0;
    uint32 width_index = 0, height_index = 0;

    ips114_set_region(x, y, x + dis_width - 1, y + dis_height - 1);             // 设置显示区域

    for(j = 0; j < dis_height; j ++)
    {
        height_index = j * height / dis_height;
        for(i = 0; i < dis_width; i ++)
        {
            width_index = i * width / dis_width;
            temp = *(image + height_index * width + width_index);               // 读取像素点
            if(threshold == 0)
            {
                color = (0x001f & ((temp) >> 3)) << 11;
                color = color | (((0x003f) & ((temp) >> 2)) << 5);
                color = color | (0x001f & ((temp) >> 3));
                ips114_write_16bit_data(color);
            }
            else if(temp < threshold)
                ips114_write_16bit_data(RGB565_BLACK);
            else
                ips114_write_16bit_data(RGB565_WHITE);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IPS114 显示 RGB565 彩色图像
// @param       x               坐标x方向的起点 参数范围 [0, ips114_x_max-1]
// @param       y               坐标y方向的起点 参数范围 [0, ips114_y_max-1]
// @param       *image          图像数组指针
// @param       width           图像实际宽度
// @param       height          图像实际高度
// @param       dis_width       图像显示宽度 参数范围 [0, ips114_x_max]
// @param       dis_height      图像显示高度 参数范围 [0, ips114_y_max]
// @param       color_mode      色彩模式 0-低位在前 1-高位在前
// @return      void
// Sample usage:                ips114_show_rgb565_image(0, 0, scc8660_image[0], SCCB8660_W, SCCB8660_H, SCCB8660_W, SCCB8660_H, 1);
//-------------------------------------------------------------------------------------------------------------------
void ips114_show_rgb565_image (uint16 x, uint16 y, const uint16 *image, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height, uint8 color_mode)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    zf_assert(x < ips114_x_max);
    zf_assert(y < ips114_y_max);

    uint32 i = 0, j = 0;
    uint16 color = 0;
    uint32 width_index = 0, height_index = 0;

    ips114_set_region(x, y, x + dis_width - 1, y + dis_height - 1);             // 设置显示区域

    for(j = 0; j < dis_height; j ++)
    {
        height_index = j * height / dis_height;
        for(i = 0; i < dis_width; i ++)
        {
            width_index = i * width / dis_width;
            color = *(image + height_index * width + width_index);              // 读取像素点
            if(color_mode)
                color = ((color & 0xff) << 8) | (color >> 8);
            ips114_write_16bit_data(color);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IPS114 显示波形
// @param       x               坐标x方向的起点 参数范围 [0, ips114_x_max-1]
// @param       y               坐标y方向的起点 参数范围 [0, ips114_y_max-1]
// @param       *wave           波形数组指针
// @param       width           波形实际宽度
// @param       value_max       波形实际最大值
// @param       dis_width       波形显示宽度 参数范围 [0, ips114_x_max]
// @param       dis_value_max   波形显示最大值 参数范围 [0, ips114_y_max]
// @return      void
// Sample usage:                ips114_show_wave(56,35,data,128,64,128,64);
//-------------------------------------------------------------------------------------------------------------------
void ips114_show_wave (uint16 x, uint16 y, const uint16 *wave, uint16 width, uint16 value_max, uint16 dis_width, uint16 dis_value_max)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    zf_assert(x < ips114_x_max);
    zf_assert(y < ips114_y_max);

    uint32 i = 0, j = 0;
    uint32 width_index = 0, value_max_index = 0;

    ips114_set_region(x, y, x + dis_width - 1, y + dis_value_max - 1);          // 设置显示区域
    for(i = 0; i < dis_value_max; i ++)
    {
        for(j = 0; j < dis_width; j ++)
        {
            ips114_write_16bit_data(ips114_bgcolor); 
        }
    }

    for(i = 0; i < dis_width; i ++)
    {
        width_index = i * width / dis_width;
        value_max_index = *(wave + width_index) * (dis_value_max - 1) / value_max;
        ips114_draw_point(i + x, (dis_value_max - 1) - value_max_index + y, ips114_pencolor);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       汉字显示
// @param       x               坐标x方向的起点 参数范围 [0, ips114_x_max-1]
// @param       y               坐标y方向的起点 参数范围 [0, ips114_y_max-1]
// @param       size            取模的时候设置的汉字字体大小 也就是一个汉字占用的点阵长宽为多少个点 取模的时候需要长宽是一样的
// @param       *chinese_buffer 需要显示的汉字数组
// @param       number          需要显示多少位
// @param       color           显示颜色
// @return      void
// Sample usage:                ips114_show_chinese(0, 0, 16, chinese_test[0], 4, RGB565_RED);// 显示font文件里面的 示例
// @Note                        使用PCtoLCD2002软件取模           阴码、逐行式、顺向   16*16
//-------------------------------------------------------------------------------------------------------------------
void ips114_show_chinese (uint16 x, uint16 y, uint8 size, const uint8 *chinese_buffer, uint8 number, const uint16 color)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    zf_assert(x < ips114_x_max);
    zf_assert(y < ips114_y_max);

    int i, j, k; 
    uint8 temp, temp1, temp2;
    const uint8 *p_data;
    
    temp2 = size / 8;
    
    ips114_set_region(x, y, number * size - 1 + x, y + size - 1);
    
    for(i = 0; i < size; i ++)
    {
        temp1 = number;
        p_data = chinese_buffer + i * temp2;
        while(temp1 --)
        {
            for(k = 0; k < temp2; k ++)
            {
                for(j = 8; j > 0; j --)
                {
                    temp = (*p_data >> (j - 1)) & 0x01;
                    if(temp)    ips114_write_16bit_data(color);
                    else        ips114_write_16bit_data(ips114_bgcolor);
                }
                p_data ++;
            }
            p_data = p_data - temp2 + temp2 * size;
        }   
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       1.14寸 IPS液晶初始化
// @param       void
// @return      void
// Sample usage:                ips114_init();
//-------------------------------------------------------------------------------------------------------------------
void ips114_init (void)
{
#if IPS114_USE_SOFT_SPI
    soft_spi_init(&ips114_spi, 0, IPS114_SOFT_SPI_DELAY, IPS114_SCL_PIN, IPS114_SDA_PIN, SOFT_SPI_PIN_NULL, SOFT_SPI_PIN_NULL);
#else
    zf_assert(IPS114_SPI != (IPS114_SCL_PIN & 0xF000 >> 12));
    zf_assert(IPS114_SPI != (IPS114_SDA_PIN & 0xF000 >> 12));
    spi_init(IPS114_SPI, SPI_MODE0, IPS114_SPI_SPEED, IPS114_SCL_PIN, IPS114_SDA_PIN, SPI_MISO_NULL, SPI_CS_NULL);
#endif

    gpio_init(IPS114_DC_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
    gpio_init(IPS114_RST_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
    gpio_init(IPS114_CS_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
    gpio_init(IPS114_BLK_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);

    ips114_set_dir(ips114_display_dir);
    ips114_set_color(ips114_pencolor, ips114_bgcolor);
	ips114_debug_init();

    IPS114_RST(0);
    system_delay_ms(200);

    IPS114_RST(1);
    system_delay_ms(100);

    ips114_write_index(0x36);
    system_delay_ms(100);
    if      (ips114_display_dir == 0)   ips114_write_8bit_data(0x00);
    else if (ips114_display_dir == 1)   ips114_write_8bit_data(0xC0);
    else if (ips114_display_dir == 2)   ips114_write_8bit_data(0x70);
    else                                ips114_write_8bit_data(0xA0);

    ips114_write_index(0x3A);
    ips114_write_8bit_data(0x05);

    ips114_write_index(0xB2);
    ips114_write_8bit_data(0x0C);
    ips114_write_8bit_data(0x0C);
    ips114_write_8bit_data(0x00);
    ips114_write_8bit_data(0x33);
    ips114_write_8bit_data(0x33);

    ips114_write_index(0xB7);
    ips114_write_8bit_data(0x35);

    ips114_write_index(0xBB);
    ips114_write_8bit_data(0x37);

    ips114_write_index(0xC0);
    ips114_write_8bit_data(0x2C);

    ips114_write_index(0xC2);
    ips114_write_8bit_data(0x01);

    ips114_write_index(0xC3);
    ips114_write_8bit_data(0x12);

    ips114_write_index(0xC4);
    ips114_write_8bit_data(0x20);

    ips114_write_index(0xC6);
    ips114_write_8bit_data(0x0F);

    ips114_write_index(0xD0);
    ips114_write_8bit_data(0xA4);
    ips114_write_8bit_data(0xA1);

    ips114_write_index(0xE0);
    ips114_write_8bit_data(0xD0);
    ips114_write_8bit_data(0x04);
    ips114_write_8bit_data(0x0D);
    ips114_write_8bit_data(0x11);
    ips114_write_8bit_data(0x13);
    ips114_write_8bit_data(0x2B);
    ips114_write_8bit_data(0x3F);
    ips114_write_8bit_data(0x54);
    ips114_write_8bit_data(0x4C);
    ips114_write_8bit_data(0x18);
    ips114_write_8bit_data(0x0D);
    ips114_write_8bit_data(0x0B);
    ips114_write_8bit_data(0x1F);
    ips114_write_8bit_data(0x23);

    ips114_write_index(0xE1);
    ips114_write_8bit_data(0xD0);
    ips114_write_8bit_data(0x04);
    ips114_write_8bit_data(0x0C);
    ips114_write_8bit_data(0x11);
    ips114_write_8bit_data(0x13);
    ips114_write_8bit_data(0x2C);
    ips114_write_8bit_data(0x3F);
    ips114_write_8bit_data(0x44);
    ips114_write_8bit_data(0x51);
    ips114_write_8bit_data(0x2F);
    ips114_write_8bit_data(0x1F);
    ips114_write_8bit_data(0x1F);
    ips114_write_8bit_data(0x20);
    ips114_write_8bit_data(0x23);

    ips114_write_index(0x21);

    ips114_write_index(0x11);
    system_delay_ms(120);

    ips114_write_index(0x29);

    ips114_clear();
}
