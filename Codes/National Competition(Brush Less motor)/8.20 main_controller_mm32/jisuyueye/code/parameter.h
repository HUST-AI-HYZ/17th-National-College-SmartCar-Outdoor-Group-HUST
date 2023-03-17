#ifndef _PARAMETER_H
#define _PARAMETER_H

#include "common.h"
//#include "general.h"

#define SPEED_NUM 3

float fast_cos(int16_t angle);
float fast_sin(int16_t angle);

/*typedef struct 
{   
    float ref;    //目标值 
    float fdb;    //实际值 
    
    float KP;
    float KI;
    float KD;
    
    float error_sum;
    float error;    //当前偏差e（k）
    float error1; //前一步的偏差
    
    float output;
    float set_error;
}PID_struct;*/
typedef struct 
{
    
    float gyro_x;
    float gyro_y;
    float gyro_z;
    
    float angle_sum_once;
    float angle_sum_area;
    float angle_sum_all;

    float gyro_direction;
}Gyro_Spi;

extern Gyro_Spi Data_Gyro;

typedef struct 
{
    float x;
    float y;
    float z;
}GYRO_STRUCT;

typedef struct 
{
    int16 x;
    int16 y;
    int16 z;
}ACC_STRUCT;

typedef struct
{
  float pitch; 
  float yaw; 
  float roll;
}Euler_struct;

typedef struct
{
  float acc_angle;
  float gyro_angle;
}Pitch_struct;

extern float speed_step;

extern uint16 x_cursor;
extern uint16 y_cursor;
extern uint16 key_num;
extern int8 modify_flag;
extern int8 op;
extern int8 FLASH_WRITE_FLAG;
extern uint32 total_time;
extern int write_flash_points;
//起步转向
extern uint16 start_thres;
//debug
extern int16 debug_flag;

//GPS的PD运算直到
extern float gps_kp_z;
extern float gps_kd_z;
//弯道PID
extern float gps_kp_w;
extern float gps_kd_w;
//GPS获取
extern float gps_kp_in;
extern float gps_kd_in;


extern float gps_error_pd_pwm;

//遥控命令
extern char CarCtrlOrder[20];

//PWM

//button control
extern int16 button_mod;

//左右轮走过距离
extern int32 dis_mid;
//运行模式
extern uint16 car_mode;

extern uint16 pwm_max_angle;
extern uint16 pwm_min_angle;
extern uint16 pwm_mid_angle;

extern Euler_struct g_euler;
extern Pitch_struct g_pitch;
extern ACC_STRUCT g_acc_filter;
extern GYRO_STRUCT g_gyro_filter;
extern GYRO_STRUCT g_gyro_bias;

extern float g_gyro_min;

extern uint16 g_rotate_record_enable;

extern float g_angular_speed;

/*************************
 * 速度控制
**************************/
extern int32 encoder_Q[8];
//一米脉冲数
extern int32 pulse_onemeter;
//duty
extern int32 duty_mid;
//读取到的脉冲数
extern int32 pulse_read;




extern float g_speed_i_max;
extern float g_speed_i_err;

extern float g_speed_set;


//速度部分
extern float g_speed_p;
extern float g_speed_i;




extern float g_car_speed;


extern float speed_max_region[4];
extern float speed_err_in_out[40];
extern uint8 speed_level;
////////////////////////////////////////////
extern uint8 g_motor;
extern int16 g_left_motor_bias;
extern int16 g_right_motor_bias;

extern float g_cur_distance;

extern uint32 g_time;
extern uint32 g_clock;

extern uint8 g_button_value;

extern uint16 g_page;
extern uint16 g_page_value;

extern uint16 g_send;

extern uint16 g_signal[5];
extern float s_y_l_now;
extern float s_x_l_now;
extern float s_x_m_now;
extern float s_x_r_now;
extern float s_y_r_now;
extern float s_x_now;
extern float s_y_now;
extern float g_signal_cali;
extern int16 g_inductor_filter_width;
extern float g_signal_p;
extern float g_signal_d;

extern uint16 g_guide;

extern uint16 g_beep;
extern uint16 g_beep_status;
extern uint16 g_beep_set;

extern uint16 g_mode;

extern float duoji;

extern float g_angular_camera_err;

extern uint16 g_ramp_state;
extern uint16 g_ramp_enable;

extern uint16 g_cross_enable;
extern uint16 g_cross_state;
extern uint16 g_cross_direction;

extern uint16 g_fork_state;
extern uint16 g_fork_direction;
extern uint16 g_fork_enable;

extern uint16 g_ring_state;
extern uint16 g_ring_direction;
extern uint16 g_ring_enable;

extern uint16 g_beep;
extern uint16 g_beep_status;
extern uint16 g_beep_set;
//判断是否启动舵机控制
extern float start_run;
#endif