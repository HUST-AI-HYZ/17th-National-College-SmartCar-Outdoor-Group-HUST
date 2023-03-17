

#include "zf_common_headfile.h"
#include "VOFA.h"
#include "math.h"

#include "button.h"
#include "parameter.h"
#include "common.h"
#include "flash.h"
#include "gps.h"
#include "isr.h"
#include "motor.h"

//data store 段微操，但是不写回
//计算后才写回
//先用pc跑，再用单片机

int a = 0;

int sum[4] = {0,0,0,0,};

sdcard_error_code_enum return_state = SDCARD_ERROR_STATE_NO_ERROR;

int main (void)
{
    clock_init(SYSTEM_CLOCK_120M);
    debug_init();

    motor_init();


    seekfree_wireless_init();

    FlashReadParams();

    duoji = pwm_mid_angle;

    icm20602_init();

    button_init();

    servo_motor_init();

    gps_init();

    tft180_init();

    //初始化四个关键点
    generating_route_init();

    //g_send = 2;


    tft180_full(RGB565_BLACK);
    system_delay_ms(100);
    tft180_full(RGB565_GRAY);
    system_delay_ms(100);
    tft180_full(RGB565_WHITE);


    pit_init_ms(TIM5_PIT, 1);                                                     // 初始化 PIT 为周期中断 1ms 周期
    interrupt_set_priority(TIM5_IRQn, 1);                                   // 设置 PIT 对周期中断的中断优先级为 1


    for (uint8 i = 0; i < 8; i++)
        tft180_draw_point(x_cursor + i, y_cursor, RGB565_BLACK);


    while (1)
    {
        page_spin();
        
        //拨码器2号损坏
        sum[0] =  gpio_get(SWITCH_1);
        sum[1] =  gpio_get(SWITCH_2);
        sum[2] =  gpio_get(SWITCH_3);
        sum[3] =  gpio_get(SWITCH_4);

			  //a = gpio_get(A0);  给360c的板子就要用这个远程停车
			
        if (write_flash_points == 1)
        {
					  route_read = 2;
					  route_write = 2;
            generating_route();
            write_flash_points = 5;
        }
         
        if (send_order >= 1 && newgpsgot == false && 0)
        {
            send_spin_vofa();
            //  send_spin();
            send_order = 0.0;
        }
        else
        {
            ;
        }

    }
}

