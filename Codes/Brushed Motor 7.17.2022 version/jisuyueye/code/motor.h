#ifndef _MOTOR_H
#define _MOTOR_H

#include "common.h"
#include "parameter.h"
/*
// 电机TIM
#define MOTOR_TIM       (TIM_8)
// 电机频率
#define PWM_Motor_FREQ  (10000)*/
#define MAX_N_DUTY_2 -45000
#define MAX_P_DUTY_2 45000

#define MAX_N_DUTY_1 -45000
#define MAX_P_DUTY_1 45000


#define MIDDLE_DUTY 25000
//50000

extern int16 direct_servopwm_max;

#define MotorA   TIM4_PWM_CH1_B6       
#define MotorB   TIM4_PWM_CH2_B7
#define Mt_TIM   TIM_4
#define SERVO_PIN  TIM8_PWM_CH1_C6

void motor_init(void);
void motor_ouput(void);
void change_left_duty_motor(int16 duty, uint8 compensate);
void change_right_duty_motor(int16 duty, uint8 compensate);
void motor_anglecontrol_pwm(void) ;

#endif