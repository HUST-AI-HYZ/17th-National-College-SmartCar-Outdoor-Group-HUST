

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

extern uint16 x_cursor = 96;
extern uint16 y_cursor = 32;
extern uint16 key_num = 0;
extern int8 modify_flag = 0;
extern int8 op = 0;
extern int8 FLASH_WRITE_FLAG = 0;
extern uint32 total_time = 0;
extern int write_flash_points = 0;

int sum[4] = {0,0,0,0,};

sdcard_error_code_enum return_state = SDCARD_ERROR_STATE_NO_ERROR;

int main (void)
{
  /*clock_init(SYSTEM_CLOCK_120M);                                             
    debug_init();                                                               
    
  servo_motor_init();  
  motor_init(); 
  change_motor(-20000, 0);
  pwm_set_duty(SERVO_PIN, 3900);*/
    clock_init(SYSTEM_CLOCK_120M);                                             
    debug_init();                                                               
    
    motor_init();      
    
    seekfree_wireless_init();     
    
     FlashReadParams();  
    
    duoji = pwm_mid_angle;
    
    //icm20602_init();
    
    button_init();   
    
    servo_motor_init();   
    
    encoder_init();
   
    gps_init();
    //my_gps_init();
   
    tft180_init();

    //初始化四个关键点
    generating_route_init();
    
    //放爆转？
    change_motor(1000,0);
   
    tft180_full(RGB565_BLACK);
    system_delay_ms(100);
    tft180_full(RGB565_GRAY);
    system_delay_ms(100);
    tft180_full(RGB565_WHITE);

    pit_init_ms(TIM5_PIT , 1);                                                    // 初始化 PIT 为周期中断 1ms 周期
    interrupt_set_priority(TIM5_IRQn, 1);                                   // 设置 PIT 对周期中断的中断优先级为 1
  
    //每个点的间隔(直道路)
     distance_each_point_direct = 6.0;
     //每个点的间隔（弯道路）
     distance_each_point_round = 6.0;
   
    route_read = route_write + 1;
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
       
       if (write_flash_points == 1)
       {
          generating_route();
          write_flash_points = 5;
       }
       
      if (send_order >= 1 && newgpsgot == false)
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

