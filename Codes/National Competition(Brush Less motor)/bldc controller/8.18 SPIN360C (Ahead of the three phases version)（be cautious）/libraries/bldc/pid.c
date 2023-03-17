/*******************************************************************************
Copyright (2016~2017),TopBand CO.,LTD 
FileName: PID.c
Author: Hunter Zhu
Date:   2017.08.15
Description: 通讯协议解析
Version: v0.0

Function List: 
1. InitSpdPI     		PID初始化
2. CalcSpdPI			PID计算
History: 2017/8/15 v0.0  build this moudle
*******************************************************************************/

/*-------------------- Includes -----------------------*/
#include "timer.h"
#include "motor.h"
#include "pid.h"


closed_loop_struct closed_loop;



//-------------------------------------------------------------------------------------------------------------------
//  @brief      绝对值函数
//  @param      void 
//  @return     void					
//  @since      
//-------------------------------------------------------------------------------------------------------------------
int32 myabs(int32 x)
{
    return (x>=0?x:-x);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      PI闭环计算
//  @param      read_speed  当前速度 
//  @return     void					
//  @since      
//-------------------------------------------------------------------------------------------------------------------
void closed_loop_pi_calc(int32 read_speed)
{
    if(read_speed)
    {
        closed_loop.real_speed = read_speed;
    }
    closed_loop.real_speed = read_speed;
    
    closed_loop.error = closed_loop.target_speed - closed_loop.real_speed;

    closed_loop.pout = closed_loop.error * (closed_loop.kp + (float)myabs(closed_loop.error/1000)/1800);
    
    //积分系数根据误差进行动态调节
    closed_loop.iout += closed_loop.error * (closed_loop.ki + (float)myabs(closed_loop.error/1000)/38000);
    
    //积分限幅
    if(closed_loop.iout > closed_loop.out_max)
    {
        closed_loop.iout = closed_loop.out_max;
    }
    else if(closed_loop.iout < -closed_loop.out_max)
    {
        closed_loop.iout = -closed_loop.out_max;
    }
    
    //如果目标速度为0或者电机被关闭则清除积分
    if((0 == closed_loop.target_speed )|| (MOTOR_DISABLE == motor_control.en_status))
    {
        closed_loop.iout = 0;
    }
    
    closed_loop.out = closed_loop.iout + closed_loop.pout;
    
    //输出限幅
    if(closed_loop.out_max < closed_loop.out)
    {
        closed_loop.out = closed_loop.out_max;
    }
    
    if(-closed_loop.out_max > closed_loop.out)
    {
        closed_loop.out = -closed_loop.out_max;
    }
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      PI闭环计算初始化
//  @param      void   
//  @return     void					
//  @since      
//-------------------------------------------------------------------------------------------------------------------
void closed_loop_pi_init(void)
{
    closed_loop.out_max = PWM_PRIOD_LOAD;
    closed_loop.kp = 0.001;
    closed_loop.ki = 0.00001;
    closed_loop.out = 0;
    closed_loop.real_speed = 0;
}
