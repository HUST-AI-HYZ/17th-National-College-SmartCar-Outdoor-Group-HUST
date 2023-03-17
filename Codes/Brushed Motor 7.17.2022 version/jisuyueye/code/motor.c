#include "zf_common_headfile.h"
#include "math.h"

#include "button.h"
#include "common.h"

#include "parameter.h"
#include "isr.h"
#include "motor.h"
#include "zf_driver_gpio.h"
#include "zf_driver_pwm.h" // 此文件中的PWM_DUTY_MAX需要注意
#include "gps.h"
//暂定duty值范围：
// mid 2.8
// left 3.6
//right 2.0
int incre_pwm = 0;
//注意单位，厘米每秒
//0.9是折损
#define MAX_CAR_SPEED 696.0
int16 direct_servopwm_max = 600;

void motor_init()
{
   //电机控制pwm
   pwm_init( MotorA, 17000, 0); // PWM 通道0 初始化频率17KHz 占空比初始为0
   pwm_init(MotorB, 17000, 0); // PWM 通道1 初始化频率17KHz 占空比初始为0

}

void servo_motor_init()
{
   pwm_init(SERVO_PIN, 50, 0); // 占空比初始为0
   pwm_set_duty(SERVO_PIN, duoji);

}


void change_motor(int32 duty, uint8 compensate)
{
   uint16 duty_1 = 0;
   uint16 duty_2 = 0;

  
   //过大限幅
   if (duty <= MAX_N_DUTY_1) //duty低于最低输入要求
   {
      duty = MAX_N_DUTY_1;
   }
   else if (duty >= MAX_P_DUTY_1) // duty大于输入要求
   {
      duty = MAX_P_DUTY_1;
   }

   //过小限幅（先不加）
 

   //定方向
   if (duty >=  0)
   {
      duty_1 = 0;
      duty_2 = duty;
   }
   else
   {
      duty_1 = 50000;
      duty_2 = - duty;
   }
   
   
   duty_mid = duty;
   pwm_set_duty(MotorA, duty_1);
   pwm_set_duty(MotorB, duty_2);
}
//由设定速度确定PWM值
void setspeed_to_pwm()
{
   int32 set_duty = 0;
   set_duty = - (int32)(g_speed_set / MAX_CAR_SPEED * PWM_DUTY_MAX);

   change_motor(set_duty,0); 
}

//距离设置
//增量式PI  1000  8
void gps_servo_control()
{
   //利用gps求出实际航向与目标航向的偏差，利用陀螺仪进行精确控制
   //每次计算出需要改变的打角值
  
   //起跑不使用0误差
   
   static float last_duoji = 0;
   //实际打角与目标打角的误差
   float error_icm =    Data_Gyro.angle_sum_once - Use_Data.gps_angle_error_now;

   //利用陀螺仪的求出误差后，进行pd控制
   //直道
   if( (area_tag == DIRECT_AREA || area_tag == ROUND_OUT_AREA )&& gps_mode == 1)
   {
      gps_error_pd_pwm = gps_kp_z * error_icm + gps_kd_z * (error_icm - error_icm_last);
   }
   else if ( (area_tag == ROUND_AREA ) && gps_mode == 1)
   {
      gps_error_pd_pwm = gps_kp_w * error_icm + gps_kd_w * (error_icm - error_icm_last);
   }
   else if (area_tag == ROUND_IN_AREA && gps_mode == 1)
   {
      gps_error_pd_pwm = gps_kp_in * error_icm + gps_kd_in * (error_icm - error_icm_last);
   }
   
   //误差迭代
   error_icm_last = error_icm;
   
   
   //gps_mode1 刚刚换点完成
    
    
   //起步限幅必须做好
    if (area_tag == DIRECT_AREA || area_tag == ROUND_OUT_AREA)
    {
      
       //正常直道                
           if (gps_error_pd_pwm > direct_thres)
          {
              duoji = pwm_mid_angle + direct_thres; 
           }
           else if (gps_error_pd_pwm < -direct_thres)
          {     
              duoji = pwm_mid_angle - direct_thres;
          }
          else
          {
             duoji = pwm_mid_angle + gps_error_pd_pwm;
          }
    }
    else
    {
        duoji = pwm_mid_angle + gps_error_pd_pwm;
    }
    
    //最大限幅
    if (duoji > pwm_max_angle)
    {
       duoji = pwm_max_angle;
     }
     else if (duoji < pwm_min_angle)
    {
       duoji = pwm_min_angle;
    }
   
   
   //要求平滑过渡
   last_duoji = duoji;
   
   pwm_set_duty(SERVO_PIN, duoji);

}
//通过脉冲累加，可以知道一米的距离，电机转了多少圈
//

