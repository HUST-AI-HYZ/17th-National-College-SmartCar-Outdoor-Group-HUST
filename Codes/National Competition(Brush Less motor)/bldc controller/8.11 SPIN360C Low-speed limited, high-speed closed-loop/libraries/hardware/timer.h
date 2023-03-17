#ifndef _timer_H_
#define _timer_H_

#include "common.h"


#define FCY  ((uint32)96000000)                    	    //系统时钟
#define FPWM  ((uint16)40000)                       	//PWM频率
#define PWM_PRIOD_LOAD (uint16)(FCY/FPWM/2)    	        //PWM周期装载值
#define DEADTIME_LOAD (9)	         			        //死区装载值

#define TIM3_TCY      1                           		//定时器3时基单位us  
#define TIM3_PSC_LOAD (uint16)(FCY/1000000*TIM3_TCY-1)  //定时器3装载值
#define TIM3_PRIOD    999                          		//定时器3周期值

void tim1_complementary_pwm(uint16 period,uint8 dead_time);
void tim1_complementary_pwm_control(uint8 status);
void tim3_init(uint16 prescaler, uint16 period);

#endif
