#include "common.h"
#include "parameter.h"
#include "encoder_head.h"
#include "zf_common_headfile.h"
#include "gps.h"

bool first_PI_input = true;
//引脚后续还要修改的
#define ENCODER1_TIM		TIM3_ENCOEDER
#define ENCODER1_A			TIM3_ENCOEDER_CH1_B4
#define ENCODER1_DIR			TIM3_ENCOEDER_CH2_B5



void encoder_init()
{
    //一个QTIMER可以 创建两个正交解码
    //这里需要注意一下，如果是带方向输出的编码器，编码器的LSB引脚应该与A相连接 DIR引脚应该与B相连接 不可交叉

    //初始化 QTIMER_1 A相使用QTIMER1_TIMER0_C0 B相使用QTIMER1_TIMER1_C1

    encoder_init_dir(ENCODER1_TIM, ENCODER1_A, ENCODER1_DIR);                
    	

    //qtimer_quad_init(QTIMER_1, QTIMER1_TIMER0_C0, QTIMER1_TIMER1_C1);
    //qtimer_quad_init(QTIMER_2, QTIMER2_TIMER0_C3, QTIMER2_TIMER3_C25);
}

//方向
//一个周期六步换向   两个换向一次高电平

void get_cur_speed() //用全局变量encoder_XX接受编码器数值，并计算速度和速度误差；
{
    uint8 i = 0;
    uint8 j = 0;
    int32 sum = 0;
    int32 temp = 0;
    if (first_PI_input) //初始化队列；
    {
        for (i = 0; i < 8; i++)
        {
            //处理编码器
          if(gpio_get(ENCODER1_DIR))
            encoder_Q[i] = encoder_get_count(ENCODER1_TIM);
          else
            encoder_Q[i] = - encoder_get_count(ENCODER1_TIM);
            
            encoder_clear_count(ENCODER1_TIM);

      
        }
        first_PI_input = false;

        //脉冲累计
        pulse_read = pulse_read + encoder_Q[7];

        g_car_speed = (encoder_Q[7] * 1.0 / 3) * 2000 / 40.672; 
       
    }
    else
    {
        for (i = 0; i < 7; i++)
        {
            encoder_Q[i] = encoder_Q[i + 1]; //队列挪位；
            sum += encoder_Q[i];

        }
        //处理编码器
        if(gpio_get(ENCODER1_DIR))
            encoder_Q[7] = encoder_get_count(ENCODER1_TIM);
         else
            encoder_Q[7] = - encoder_get_count(ENCODER1_TIM);
        
        encoder_clear_count(ENCODER1_TIM);
       
        sum += encoder_Q[7];
 
        //取平均值
        int32 encoder_aver = sum >> 3;
    
        g_car_speed = (encoder_aver * 1.0 / 3) * 2000 / 40.672; //车辆速度
        
        //距离累计
        if ( gps_mode == 2 || gps_mode == 1)
        {
            //走一米电机转40.672圈,每换一次相一次脉冲，一转换三次相
            //distance_from_lastpoint = distance_from_lastpoint + (encoder_Q[7] * 1.0 / 3.0 )/ 40.672 ;
        }
        
        //这个是encoder的累计脉冲
        pulse_read = pulse_read + encoder_Q[7];
    }
}



