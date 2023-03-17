
#include "HAL_conf.h"
#include "gpio.h"
#include "adc.h"
#include "timer.h"

#include "pwm_input.h"
#include "opamp.h"
#include "comp.h"
#include "hall.h"
#include "pid.h"
#include "motor.h"
#include "move_filter.h"
#include "bldc_config.h"
#include "isr.h"

int MIN(int32 a , int32 b)
{
	if (a < b)
	{
		return a;
	}
	else
	{
		return b;
	}
	
}

void TIM16_IRQHandler(void)
{
    static uint8 last_trigger_status;//1:上次为下降沿触发 2：上次为上升沿触发
    TIM16->CNT = 0;
    if (TIM16->SR & TIM_IT_CC1)
    {
        TIM16->SR = (uint32_t)~TIM_IT_CC1;//TIM_ClearITPendingBit(TIM16, TIM_IT_CC1);
        if(MOTOR_PWM_PORT->IDR & MOTOR_PWM_PIN)//上升沿    //GPIO_ReadInputDataBit(MOTOR_PWM_PORT, MOTOR_PWM_PIN))
        {
            if(2 != last_trigger_status)//上次不是上升沿触发 本次才有效
            {
                last_trigger_status = 2;
                //获取低电平持续时间并计算周期
                duty_low = TIM16->CCR1;//TIM_GetCapture1(TIM16);
                period = duty_low + duty_high;
                duty_flag = 1;
            }
            
        }
        else//下降沿
        {
            if(1 != last_trigger_status)//上次不是下降沿触发 本次才有效
            {
                last_trigger_status = 1;
                //获取高电平持续时间
                duty_high = TIM16->CCR1;//TIM_GetCapture1(TIM16);
            }
        }
    }

    if (TIM16->SR & TIM_IT_Update) 
    {
        //超时根据引脚状态设置高电平时间
        if(MOTOR_PWM_PORT->IDR & MOTOR_PWM_PIN)//GPIO_ReadInputDataBit(MOTOR_PWM_PORT, MOTOR_PWM_PIN))
        {
            period = 1000;
            duty_high = period;
            duty_low = 0;
        }
        else
        {
            duty_high = 0;
        }
        TIM16->SR = (uint32_t)~TIM_IT_Update;//TIM_ClearITPendingBit(TIM16, TIM_IT_Update);
        duty_flag = 2;//输入信号超时
    }
}


void TIM3_IRQHandler(void)
{
    int8 dir;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) 
	{                           
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);     //清除中断标志位   

        //检查使能开关状态
        motor_en();
        
		//根据接收到的信号，计算方向
        dir = pwm_dir_get();
        dir = dir==0?1:(int8)-1;
        
        //计算pwm信号的占空比
        if(duty_flag)
        {
            if(1 == duty_flag)duty_flag = 0;//如果输入信号超时则不应该清除标志位
            duty_cycle = (float)duty_high/period;
        }
        else
        {
            duty_cycle = 0;
        }
        
        
        //计算速度
    #if BLDC_CLOSE_LOOP_ENABLE==1
        motor_control.set_speed = dir * motor_control.max_speed * duty_cycle;
    #else
        motor_control.set_speed = dir * PWM_PRIOD_LOAD * duty_cycle;
    #endif
        
    #if BLDC_SPEED_CURVE_ENABLE==1
        //计算加速曲线 通过设置参数可以调节加速的响应速度
        motor_speed_curve();
    #else
        closed_loop.target_speed = motor_control.set_speed;
    #endif

        motor_set_dir();
        
    #if BLDC_CLOSE_LOOP_ENABLE==1
        //进行PI闭环计算
        closed_loop_pi_calc(speed_filter.data_average);
    #else
        closed_loop.out = closed_loop.target_speed;
    #endif
		

		
		
        //输出动力
        motor_power_out();
    }
}


void ADC1_IRQHandler(void)
{
	//清除标志位
    ADC_ClearFlag(ADC1, ADC_IT_EOC);
    
    //读取adc的值
    adc_read();
    
	//霍尔扫描
	scan_hall_status();

    if(1 > commutation_delay--)
    {//延时时间到 开始换相
        commutation_delay = 0;
        motor_commutation(next_hall_value);
    }
}


void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) 
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);  	
	}
	
	if(TIM_GetITStatus(TIM1, TIM_IT_Break) != RESET) 
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Break);  
	}
}


