#include "parameter.h"
#include "gps.h"


//debug专用，判断是否进入了对应的代码段
int16 debug_flag = 0;

//duty
int32 duty_mid = 0;
//button control
int16 button_mod = 2;

//遥控命令
char CarControlOrder[20];

//运行模式
uint16 car_mode = 0;

//误差角PID控制直道
float gps_kp_z = 1.0;
float gps_kd_z = 0.0;
//PID 控制弯道
float gps_kp_w = 1.0;
float gps_kd_w = 0.0;
//gps采集
float gps_kp_in = 1.0;
float gps_kd_in = 0.0;


float gps_error_pd_pwm = 0.0;

//PID鐩稿叧
//PID缁撴瀯浣?
/*ID_struct  motor_angle;*/
uint16 pwm_max_angle = 4200;
uint16 pwm_min_angle = 2800;
uint16 pwm_mid_angle = 3600;  //有点问题
float start_run = 0.0;



/*************************
 * 速度控制
**************************/
int32 encoder_Q[8]={0,};
int32 pulse_onemeter = 41648;
//电机一圈  1024
// 1m对应电机转41648/1024 = 40.672圈 
//还是有问题？？？
int32 pulse_read = 0;



// 閫熷害PI鍙傛暟
float g_speed_i_max = 10000.0;
float g_speed_i_err = 0.0;

// 鏈熸湜閫熷害
float g_speed_set = 200.0;

//放内侧轮子速度值   //(厘米每秒)(以内轮为基准)
float speed_max_region[SPEED_NUM]=
{
   //默认
    200.0,
   //大角度弯道
    200.0,
   //停车
    0,
};

uint8 speed_level = 0;


//内侧速度*（1+比例值）/2  为实际速度
float g_car_speed = 0.0;

/*******************
 * 角速度控制
*********************/
Gyro_Spi Data_Gyro;



// 杩愯?岃窛绂?
float g_cur_distance = 0.0;

// 绯荤粺鏃堕棿
uint32 g_time = 0;
// 璁℃椂鍣ㄦ椂閽?
uint32 g_clock = 0;

// 鎸夐敭鍊?
uint8 g_button_value = 0;

// 椤甸潰浣胯兘
uint16 g_page = ENABLE;
// 椤甸潰鍊?
uint16 g_page_value = 0;

// 璋冭瘯鍙戦�佸姛鑳?
uint16 g_send = 1;


uint16 g_mode = 1;

float duoji = 3600;

