#include "button.h"
#include "zf_driver_gpio.h"
#include "zf_common_headfile.h"
#include "parameter.h"
#include "flash.h"

void move()
{
  uint8 i = x_cursor;
  //left
  if (key_num == 1)
  {
    for (i = 0; i < 8; i++)
      tft180_draw_point(x_cursor + i, y_cursor, RGB565_WHITE);

    if (x_cursor - 8 < 0)
      x_cursor = 152;
    else
      x_cursor = x_cursor - 8;

    for (i = 0; i < 8; i++)
      tft180_draw_point(x_cursor + i, y_cursor, RGB565_BLACK);
  }
  //up
  else if (key_num == 2)
  {
    for (i = 0; i < 8; i++)
      tft180_draw_point(x_cursor + i, y_cursor, RGB565_WHITE);

    if (y_cursor - 16 < 32)
    {
      y_cursor = 112;
      curpage--;
      tft180_full(RGB565_WHITE);
    }
    else
      y_cursor = y_cursor - 16;

    for (i = 0; i < 8; i++)
      tft180_draw_point(x_cursor + i, y_cursor, RGB565_BLACK);
  }
  //down
  else if (key_num == 3)
  {
    for (i = 0; i < 8; i++)
      tft180_draw_point(x_cursor + i, y_cursor, RGB565_WHITE);

    if (y_cursor + 16 >= 128)
    {
      y_cursor = 32;
      curpage++;
      tft180_full(RGB565_WHITE);
    }
    else
      y_cursor = y_cursor + 16;

    for (i = 0; i < 8; i++)
      tft180_draw_point(x_cursor + i, y_cursor, RGB565_BLACK);
  }
  //right
  else if (key_num == 4)
  {
    for (i = 0; i < 8; i++)
      tft180_draw_point(x_cursor + i, y_cursor, RGB565_WHITE);

    if (x_cursor + 8 >= 160)
      x_cursor = 0;
    else
      x_cursor = x_cursor + 8;

    for (i = 0; i < 8; i++)
      tft180_draw_point(x_cursor + i, y_cursor, RGB565_BLACK);
  }
  
  
}

void delay()
{
  uint16 i , j;
  for (i = 400; i > 0; i--)
  {
    for (j = 200; j > 0; j--)
    {
         ;
    }
  }
}

uint8 Button_get()
{
  //left
  if (!gpio_get(KEY1))
  {
    delay();
    if (!gpio_get(KEY1))
    {
      while (!gpio_get(KEY1))
        ;
      return 2;
    }
  }
  //up
  else if (!gpio_get(KEY2))
  {
    delay();
    if (!gpio_get(KEY2))
    {
      while (!gpio_get(KEY2))
        ;
      return 5;
    }
  }
  //down
  else if (!gpio_get(KEY3))
  {
    delay();
    if (!gpio_get(KEY3))
    {
      while (!gpio_get(KEY3))
        ;
      return 1;
    }
  }
  //DOWN
  else if (!gpio_get(KEY4))
  {
    delay();
    if (!gpio_get(KEY4))
    {
      while (!gpio_get(KEY4))
        ;
      return 3;
    }
  }
  //center
  else if (!gpio_get(KEY5))
	{
		delay();
		if (!gpio_get(KEY5))
		{
			while (!gpio_get(KEY5))
				;
			return 4;
		}
	}
  else
  {
    return 0;
  }
  return 0;
}

void button_init()
{
  
  gpio_init(KEY1, GPI, GPIO_HIGH, GPI_PULL_UP);		// 初始化为GPIO浮空输入 默认上拉高电平
  gpio_init(KEY2, GPI, GPIO_HIGH, GPI_PULL_UP); 
  gpio_init(KEY3, GPI, GPIO_HIGH, GPI_PULL_UP); 
  gpio_init(KEY4, GPI, GPIO_HIGH, GPI_PULL_UP); 
  gpio_init(KEY5, GPI, GPIO_HIGH, GPI_PULL_UP); 

    //靠近五项按键的为1
    //从车头方向看过去，右侧是关闭
  gpio_init(SWITCH_1, GPI, GPIO_HIGH, GPI_PULL_UP); // 初始化为GPIO浮空输入 默认上拉高电平
  gpio_init(SWITCH_2, GPI, GPIO_HIGH, GPI_PULL_UP);
  gpio_init(SWITCH_3, GPI, GPIO_HIGH, GPI_PULL_UP); // 初始化为GPIO浮空输入 默认上拉高电平
  gpio_init(SWITCH_4, GPI, GPIO_HIGH, GPI_PULL_UP);
}

void Button_Control()
{
  //center
    if (key_num == 5)
    {
        button_mod++;
    }

    if (button_mod % 2 == 0)
    {
        move();
    }
    else if (button_mod % 2 == 1)
    {
        if (key_num == 2)
        {
            modify_flag = 1;
            op = 1;
        }
        else if (key_num == 3)
        {
            modify_flag = 1;
            op = -1;
        }
        else if (key_num == 1 || key_num == 4)
        {
            move();
        }
    }
  
}
void Flash_Control()
{
    static bool flash_flag = false;
    
    if (button_mod % 2 == 1 && flash_flag == false && gps_mode != COLLECTION_BY_HAND)
    {
        if (gpio_get(SWITCH_4) == 0) 
            //&& gpio_get(SWITCH_4) == 0 )   
           // && gpio_get(SWITCH_3) == 0  ) //&& gpio_get(SWITCH_4) == 0)
        {
           FLASH_WRITE_FLAG = 1;
           flash_flag = true;
        }
    }
    //gps_mode == 2单独保存
    else if (button_mod % 2 == 0 && flash_flag == true && gps_mode != COLLECTION_BY_HAND)
    {
         if (gpio_get(SWITCH_4) == 0)
             //&& (gpio_get(SWITCH_4) == 0 )   
            //&& gpio_get(SWITCH_3) == 0 )//&& gpio_get(SWITCH_4) == 0)
        {
           FLASH_WRITE_FLAG = 1;
           flash_flag = false;
        }
      
    }
  
}




