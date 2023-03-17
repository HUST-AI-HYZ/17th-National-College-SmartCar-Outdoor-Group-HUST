/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_oled
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
*                   D0                  查看 zf_device_oled.h 中 OLED_D0_PIN 宏定义
*                   D1                  查看 zf_device_oled.h 中 OLED_D1_PIN 宏定义
*                   RES                 查看 zf_device_oled.h 中 OLED_RES_PIN 宏定义
*                   DC                  查看 zf_device_oled.h 中 OLED_DC_PIN 宏定义
*                   CS                  查看 zf_device_oled.h 中 OLED_CS_PIN 宏定义
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   ------------------------------------
********************************************************************************************************************/

#include "zf_device_oled.h"

#if OLED_USE_SOFT_SPI
static soft_spi_info_struct             oled_spi;
#define oled_spi_write_8bit(data)       (soft_spi_write_8bit(&oled_spi, (data)))
#else
#define oled_spi_write_8bit(data)       (spi_write_8bit(OLED_SPI, (data)))
#endif

static oled_dir_enum        oled_display_dir    = OLED_DEFAULT_DISPLAY_DIR;
static oled_font_size_enum  oled_display_font   = OLED_DEFAULT_DISPLAY_FONT;

//-------------------------------------------------------------------------------------------------------------------
// @brief       写8位数据
// @param       data            数据
// @return      void
// Sample usage:
// @note        内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void oled_write_data (const uint8 data)
{
    OLED_CS(0);                                                                 // 使能信号端，拉低时正常使用
    OLED_DC(1);
    oled_spi_write_8bit(data);
    OLED_CS(1);                                                                 // 使能信号端，拉低时正常使用
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       写命令
// @param       cmd             命令
// @return      void
// Sample usage:
// @note        内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void oled_write_command (const uint8 command)
{
    OLED_CS(0);                                                                 // 使能信号端，拉低时正常使用
    OLED_DC(0);
    oled_spi_write_8bit(command);
    OLED_CS(1);                                                                 // 使能信号端，拉低时正常使用
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       OLED显示坐标设置
// @param       x               x轴坐标设置0-127
// @param       y               y轴坐标设置0-7
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
static void oled_set_coordinate (uint8 x, uint8 y)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    // 检查一下你的显示调用的函数 自己计算一下哪里超过了屏幕显示范围
    zf_assert(x < 128);
    zf_assert(y < 8);

    oled_write_command(0xb0 + y);
    oled_write_command(((x & 0xf0) >> 4) | 0x10);
    oled_write_command((x & 0x0f) | 0x00);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       OLED显示DEBUG信息初始化
// @param       void
// @return      void
// Sample usage:               内部使用用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void oled_debug_init (void)
{
    debug_output_struct info;
    debug_output_struct_init(&info);

    info.type_index = 1;

    info.display_x_max = OLED_X_MAX;
    info.display_y_max = OLED_Y_MAX;
    if(oled_display_font == OLED_6x8_FONT)
    {
        info.font_x_size = 6;
        info.font_y_size = 1;
    }
    else if(oled_display_font == OLED_8x16_FONT)
    {
        info.font_x_size = 8;
        info.font_y_size = 2;
    }
    info.output_screen = oled_show_string;
    info.output_screen_clear = oled_clear;
        
    debug_output_init(&info);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       OLED 清屏函数
// @param       void
// @return      void
// Sample usage:                oled_clear();
//-------------------------------------------------------------------------------------------------------------------
void oled_clear (void)
{
    uint8 y, x;

    for(y = 0; y < 8; y ++)
    {
        oled_write_command(0xb0 + y);
        oled_write_command(0x01);
        oled_write_command(0x10);
        for(x = 0; x < OLED_X_MAX; x ++)
            oled_write_data(0x00); 
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       OLED 屏幕填充函数
// @param       color           填充颜色选着(0x00 or 0xff)
// @return      void
// Sample usage:                oled_full(0x00);
//-------------------------------------------------------------------------------------------------------------------
void oled_full (const uint8 color)
{
    uint8 y, x;

    for(y = 0; y < 8; y ++)
    {
        oled_write_command(0xb0 + y);
        oled_write_command(0x01);
        oled_write_command(0x10);
        for(x = 0; x < OLED_X_MAX; x ++)
            oled_write_data(color); 
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       设置显示方向 这个函数只有在初始化屏幕之前调用才生效
// @param       dir             显示方向  参照 zf_device_oled.h 内 oled_dir_enum 枚举体定义
// @return      void
// Sample usage:                oled_set_dir(OLED_CROSSWISE);
//-------------------------------------------------------------------------------------------------------------------
void oled_set_dir (oled_dir_enum dir)
{
    oled_display_dir = dir;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       设置显示字体
// @param       dir             显示方向  参照 zf_device_oled.h 内 oled_font_size_enum 枚举体定义
// @return      void
// Sample usage:                oled_set_font(OLED_8x16_FONT);
//-------------------------------------------------------------------------------------------------------------------
void oled_set_font (oled_font_size_enum font)
{
    oled_display_font = font;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       画点函数
// @param       x               x 轴坐标设置 0-127
// @param       y               y 轴坐标设置 0-7
// @param       color           8 个点数据
// @return      void
// Sample usage:                oled_draw_point(0, 0, 1);
//-------------------------------------------------------------------------------------------------------------------
void oled_draw_point (uint16 x, uint16 y, const uint8 color)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    // 检查一下你的显示调用的函数 自己计算一下哪里超过了屏幕显示范围
    zf_assert(x < 128);
    zf_assert(y < 8);

    oled_set_coordinate(x, y);
    oled_write_command(0xb0 + y);
    oled_write_command(((x & 0xf0) >> 4) | 0x10);
    oled_write_command((x & 0x0f) | 0x00);
    oled_write_data(color);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       OLED显示字符串
// @param       x               x 轴坐标设置 0-127
// @param       y               y 轴坐标设置 0-7
// @param       ch[]            字符串
// @return      void
// Sample usage:                oled_show_string(0, 0, "SEEKFREE");
//-------------------------------------------------------------------------------------------------------------------
void oled_show_string (uint16 x, uint16 y, const char ch[])
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    // 检查一下你的显示调用的函数 自己计算一下哪里超过了屏幕显示范围
    zf_assert(x < 128);
    zf_assert(y < 8);

    uint8 c = 0, i = 0, j = 0;
    while (ch[j] != '\0')
    {
        if(oled_display_font == OLED_6x8_FONT)
        {
            c = ch[j] - 32;
            if(x > 126)
            {
                x = 0;
                y ++;
            }
            oled_set_coordinate(x, y);
            for(i = 0; i < 6; i ++)
                oled_write_data(oled_6x8[c][i]);
            x += 6;
            j ++;
        }
        else
        {
            c = ch[j] - 32;
            if(x > 120)
            {
                x = 0;
                y ++;
            }
            oled_set_coordinate(x, y);
            for(i = 0; i < 8; i ++)
                oled_write_data(oled_8x16[c * 16 + i]);

            oled_set_coordinate(x, y + 1);
            for(i = 0; i < 8; i ++)
                oled_write_data(oled_8x16[c * 16 + i + 8]);
            x += 8;
            j ++;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       液晶显示32位有符号 (去除整数部分无效的0)
// @param       x               x轴坐标设置 0-127
// @param       y               y轴坐标设置 0-7
// @param       dat             需要显示的变量 数据类型 int32
// @param       num             需要显示的位数 最高10位  不包含正负号
// @return      void
// Sample usage:                oled_show_int(0, 0, x, 3);                      // x 可以为 int32 int16 int8 类型
// note:                        负数会显示一个 ‘-’号   正数显示一个空格
//-------------------------------------------------------------------------------------------------------------------
void oled_show_int (uint16 x, uint16 y, const int32 dat, uint8 num)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    // 检查一下你的显示调用的函数 自己计算一下哪里超过了屏幕显示范围
    zf_assert(x < 128);
    zf_assert(y < 8);

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
    oled_show_string(x, y, (const char *)&data_buffer);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       液晶显示32位无符号 (去除整数部分无效的0)
// @param       x               x 轴坐标设置 0-127
// @param       y               y 轴坐标设置 0-7
// @param       dat             需要显示的变量 数据类型 uint32
// @param       num             需要显示的位数 最高10位  不包含正负号
// @return      void
// Sample usage:                oled_show_uint(0, 0, x, 3);                     // x 可以为 uint32 uint16 uint8 类型
//-------------------------------------------------------------------------------------------------------------------
void oled_show_uint (uint16 x,uint16 y,const uint32 dat,uint8 num)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    // 检查一下你的显示调用的函数 自己计算一下哪里超过了屏幕显示范围
    zf_assert(x < 128);
    zf_assert(y < 8);

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
    oled_show_string(x, y, (const char *)&data_buffer);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       液晶显示浮点数 (去除整数部分无效的0)
// @param       x               x 轴坐标设置 0-127
// @param       y               y 轴坐标设置 0-7
// @param       dat             需要显示的变量，数据类型float或double
// @param       num             整数位显示长度   最高10位  
// @param       pointnum        小数位显示长度   最高6位
// @return      void
// Sample usage:                oled_show_float(0, 0, x, 2, 3);                 // 显示浮点数   整数显示2位   小数显示三位
// @note                        特别注意当发现小数部分显示的值与你写入的值不一样的时候，
//                              可能是由于浮点数精度丢失问题导致的，这并不是显示函数的问题，
//                              有关问题的详情，请自行百度学习   浮点数精度丢失问题。
//                              负数会显示一个 ‘-’号   正数显示一个空格
//-------------------------------------------------------------------------------------------------------------------
void oled_show_float (uint16 x,uint16 y,const float dat,uint8 num,uint8 pointnum)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    // 检查一下你的显示调用的函数 自己计算一下哪里超过了屏幕显示范围
    zf_assert(x < 128);
    zf_assert(y < 8);

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
    oled_show_string(x, y, data_buffer);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IPS114 显示二值图像 数据每八个点组成一个字节数据
// @param       x               x 轴坐标设置 0-127
// @param       y               y 轴坐标设置 0-7
// @param       *image          图像数组指针
// @param       width           图像实际宽度
// @param       height          图像实际高度
// @param       dis_width       图像显示宽度 参数范围 [0, 128]
// @param       dis_height      图像显示高度 参数范围 [0, 64]
// @return      void
// Sample usage:                oled_show_binary_image(0, 0, ov7725_image_binary[0], OV7725_W, OV7725_H, OV7725_W, OV7725_H);
//-------------------------------------------------------------------------------------------------------------------
void oled_show_binary_image (uint16 x, uint16 y, const uint8 *image, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    // 检查一下你的显示调用的函数 自己计算一下哪里超过了屏幕显示范围
    zf_assert(x < 128);
    zf_assert(y < 8);

    uint32 i = 0, j = 0, z = 0;
    uint8 dat;
    uint32 width_index = 0, height_index = 0;

    dis_height = dis_height - dis_height % 8;
    dis_width = dis_width - dis_width % 8;
    for(j = 0; j < dis_height; j += 8)
    {
        oled_set_coordinate(x + 0, y + j / 8);
        height_index = j * height / dis_height;
        for(i = 0; i < dis_width; i += 8)
        {
            width_index = i * width / dis_width / 8;
            for(z = 0; z < 8; z ++)
            {
                dat = 0;
                if(*(image + height_index * width / 8 + width_index + width / 8 * 0) & (0x80 >> z)) dat |= 0x01;
                if(*(image + height_index * width / 8 + width_index + width / 8 * 1) & (0x80 >> z)) dat |= 0x02;
                if(*(image + height_index * width / 8 + width_index + width / 8 * 2) & (0x80 >> z)) dat |= 0x04;
                if(*(image + height_index * width / 8 + width_index + width / 8 * 3) & (0x80 >> z)) dat |= 0x08;
                if(*(image + height_index * width / 8 + width_index + width / 8 * 4) & (0x80 >> z)) dat |= 0x10;
                if(*(image + height_index * width / 8 + width_index + width / 8 * 5) & (0x80 >> z)) dat |= 0x20;
                if(*(image + height_index * width / 8 + width_index + width / 8 * 6) & (0x80 >> z)) dat |= 0x40;
                if(*(image + height_index * width / 8 + width_index + width / 8 * 7) & (0x80 >> z)) dat |= 0x80;
                oled_write_data(dat);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IPS114 显示 8bit 灰度图像 带二值化阈值
// @param       x               x 轴坐标设置 0-127
// @param       y               y 轴坐标设置 0-7
// @param       *image          图像数组指针
// @param       width           图像实际宽度
// @param       height          图像实际高度
// @param       dis_width       图像显示宽度 参数范围 [0, 128]
// @param       dis_height      图像显示高度 参数范围 [0, 64]
// @param       threshold       二值化显示阈值 0-不开启二值化
// @return      void
// Sample usage:                oled_show_gray_image(0, 0, mt9v03x_image[0], width, height, 128, 64, x);
//-------------------------------------------------------------------------------------------------------------------
void oled_show_gray_image (uint16 x, uint16 y, const uint8 *image, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height, uint8 threshold)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    // 检查一下你的显示调用的函数 自己计算一下哪里超过了屏幕显示范围
    zf_assert(x < 128);
    zf_assert(y < 8);

    int16 i, j;
    uint8 dat;
    uint32 width_index = 0, height_index = 0;

    dis_height = dis_height - dis_height % 8;

    for(j = 0; j < dis_height; j += 8)
    {
        oled_set_coordinate(x + 0, y + j / 8);
        height_index = j * height / dis_height;
        for(i = 0; i < dis_width; i ++)
        {
            width_index = i * width / dis_width;
            dat = 0;
            if(*(image + height_index * width + width_index + width * 0) > threshold) dat |= 0x01;
            if(*(image + height_index * width + width_index + width * 1) > threshold) dat |= 0x02;
            if(*(image + height_index * width + width_index + width * 2) > threshold) dat |= 0x04;
            if(*(image + height_index * width + width_index + width * 3) > threshold) dat |= 0x08;
            if(*(image + height_index * width + width_index + width * 4) > threshold) dat |= 0x10;
            if(*(image + height_index * width + width_index + width * 5) > threshold) dat |= 0x20;
            if(*(image + height_index * width + width_index + width * 6) > threshold) dat |= 0x40;
            if(*(image + height_index * width + width_index + width * 7) > threshold) dat |= 0x80;
            oled_write_data(dat);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       IPS114 显示波形
// @param       x               x 轴坐标设置 0-127
// @param       y               y 轴坐标设置 0-7
// @param       *wave           波形数组指针
// @param       width           波形实际宽度
// @param       value_max       波形实际最大值
// @param       dis_width       波形显示宽度 参数范围 [0, 128]
// @param       dis_value_max   波形显示最大值 参数范围 [0, 64]
// @return      void
// Sample usage:                oled_show_wave(0, 0, data, 128, 64, 128, 64);
//-------------------------------------------------------------------------------------------------------------------
void oled_show_wave (uint16 x, uint16 y, const uint16 *wave, uint16 width, uint16 value_max, uint16 dis_width, uint16 dis_value_max)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    // 检查一下你的显示调用的函数 自己计算一下哪里超过了屏幕显示范围
    zf_assert(x < 128);
    zf_assert(y < 8);

    uint32 i = 0;
    uint32 width_index = 0, value_max_index = 0;
    uint8 dis_h = 0;

    uint32 x_temp = 0;
    uint32 y_temp = 0;
    for(y_temp = 0; y_temp < dis_value_max; y_temp += 8)
    {
        oled_set_coordinate(x + 0, y + y_temp / 8);
        for(x_temp = 0; x_temp < dis_width; x_temp ++)
            oled_write_data(0x00); 
    }
    for(i = 0; i < dis_width; i ++)
    {
        width_index = i * width / dis_width;
        value_max_index = *(wave + width_index) * (dis_value_max - 1) / value_max;

        dis_h = (dis_value_max - 1) - value_max_index;
        oled_set_coordinate(i + x, dis_h / 8 + y);
        dis_h = (0x01 << dis_h % 8);
        oled_write_data(dis_h);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       汉字显示
// @param       x               横坐标 0-127
// @param       y               纵坐标 0-7
// @param       size            取模的时候设置的汉字字体大小，也就是一个汉字占用的点阵长宽为多少个点，取模的时候需要长宽是一样的。
// @param       *chinese_buffer 需要显示的汉字数组
// @param       number          需要显示多少位
// @return      void
// Sample usage:                oled_show_chinese(0, 6, 16, (const uint8 *)oled_16x16_chinese, 4);
// @Note        使用PCtoLCD2002软件取模       阴码、逐行式、顺向       16*16
//-------------------------------------------------------------------------------------------------------------------
void oled_show_chinese (uint16 x, uint16 y, uint8 size, const uint8 *chinese_buffer, uint8 number)
{
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    // 检查一下你的显示调用的函数 自己计算一下哪里超过了屏幕显示范围
    zf_assert(x < 128);
    zf_assert(y < 8);

    int16 i, j, k;

    for(i = 0; i < number; i ++)
    {
        for(j = 0; j < (size / 8); j ++)
        {
            oled_set_coordinate(x + i * size, y + j);
            for(k = 0; k < 16; k ++)
            {
                oled_write_data(*chinese_buffer);
                chinese_buffer ++;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       OLED初始化函数
// @param       void
// @return      void
// Sample usage:                oled_init();
//-------------------------------------------------------------------------------------------------------------------
void oled_init (void)
{
#if OLED_USE_SOFT_SPI
    soft_spi_init(&oled_spi, 0, OLED_SOFT_SPI_DELAY, OLED_D0_PIN, OLED_D1_PIN, SOFT_SPI_PIN_NULL, SOFT_SPI_PIN_NULL);
#else
    zf_assert(OLED_SPI != (OLED_D0_PIN & 0xF000 >> 12));
    zf_assert(OLED_SPI != (OLED_D1_PIN & 0xF000 >> 12));
    spi_init(OLED_SPI, SPI_MODE0, OLED_SPI_SPEED, OLED_D0_PIN, OLED_D1_PIN, SPI_MISO_NULL, SPI_CS_NULL);
#endif
    gpio_init(OLED_RES_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(OLED_DC_PIN , GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(OLED_CS_PIN , GPO, GPIO_HIGH, GPO_PUSH_PULL);

    oled_set_dir(oled_display_dir);
	oled_debug_init();

    OLED_CS(0);
    OLED_RES(0);
    system_delay_ms(50);
    OLED_RES(1);

    oled_write_command(0xae);                                                   // --turn off oled panel
    oled_write_command(0x00);                                                   // ---set low column address
    oled_write_command(0x10);                                                   // ---set high column address
    oled_write_command(0x40);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         // --set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    oled_write_command(0x81);                                                   // --set contrast control register
    oled_write_command(OLED_BRIGHTNESS);                                        //  Set SEG Output Current Brightness

    if (oled_display_dir == OLED_CROSSWISE)
    {
        oled_write_command(0xa1);                                               // --Set SEG/Column Mapping     0xa0左右反置 0xa1正常
        oled_write_command(0xc8);                                               // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    }
    else
    {
        oled_write_command(0xa0);                                               // --Set SEG/Column Mapping     0xa0左右反置 0xa1正常
        oled_write_command(0xc0);                                               // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    }
    
    oled_write_command(0xa6);                                                   // --set normal display
    oled_write_command(0xa8);                                                   // --set multiplex ratio(1 to 64)
    oled_write_command(0x3f);                                                   // --1/64 duty
    oled_write_command(0xd3);                                                   // -set display offset  Shift Mapping RAM Counter (0x00~0x3F)
    oled_write_command(0x00);                                                   // -not offset
    oled_write_command(0xd5);                                                   // --set display clock divide ratio/oscillator frequency
    oled_write_command(0x80);                                                   // --set divide ratio, Set Clock as 100 Frames/Sec
    oled_write_command(0xd9);                                                   // --set pre-charge period
    oled_write_command(0xf1);                                                   // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    oled_write_command(0xda);                                                   // --set com pins hardware configuration
    oled_write_command(0x12);
    oled_write_command(0xdb);                                                   // --set vcomh
    oled_write_command(0x40);                                                   // Set VCOM Deselect Level
    oled_write_command(0x20);                                                   // -Set Page Addressing Mode (0x00/0x01/0x02)
    oled_write_command(0x02);                                                   // 
    oled_write_command(0x8d);                                                   // --set Charge Pump enable/disable
    oled_write_command(0x14);                                                   // --set(0x10) disable
    oled_write_command(0xa4);                                                   //  Disable Entire Display On (0xa4/0xa5)
    oled_write_command(0xa6);                                                   //  Disable Inverse Display On (0xa6/a7)
    oled_write_command(0xaf);                                                   // --turn on oled panel

    oled_clear();                                                               // 初始清屏
    oled_set_coordinate(0, 0);
}
